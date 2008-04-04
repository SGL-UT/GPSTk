/**
 * @file SRIleastSquares.hpp
 * Include file defining class SRIleastSquares, which inherits class SRI and
 * implements a general least squares algorithm that includes linear or linearized
 * problems, weighting, robust estimation, and sequential estimation.
 */

//------------------------------------------------------------------------------------
#ifndef CLASS_SRI_LEAST_SQUARES_INCLUDE
#define CLASS_SRI_LEAST_SQUARES_INCLUDE

//------------------------------------------------------------------------------------
// system
#include <ostream>
// GPSTk
#include "Vector.hpp"
#include "Matrix.hpp"
#include "SRI.hpp"

namespace gpstk {

//------------------------------------------------------------------------------------
/** class SRIleastSquares inherits SRI and implements a general least squares
 * algorithm using SRI, including weighted, linear or linearized, robust and/or
 * sequential algorithms.
 *
 * At any point the state X and covariance P are related to the SRI by
 * X = inverse(R) * z , P = inverse(R) * inverse(transpose(R)), or
 * R = upper triangular square root (Cholesky decomposition) of the inverse of P,
 * and z = R * X.
 */
class SRIleastSquares : public SRI {
public:
   /// empty constructor
   SRIleastSquares(void) throw();

   /// constructor given the dimension N.
   /// @param N dimension of the SRIleastSquares.
   SRIleastSquares(const unsigned int N)
      throw();

   /// constructor given a Namelist; its dimension determines the SRI dimension.
   /// @param NL Namelist for the SRIleastSquares.
   SRIleastSquares(const Namelist& NL)
      throw();

   /// explicit constructor - throw if the dimensions are inconsistent.
   /// @param R  Initial information matrix, an upper triangular matrix of dim N.
   /// @param Z  Initial information data vector, of length N.
   /// @param NL Namelist for the SRIleastSquares, also of length N.
   /// @throw MatrixException if dimensions are not consistent.
   SRIleastSquares(const Matrix<double>& R,
             const Vector<double>& Z,
             const Namelist& NL)
      throw(MatrixException);

   /// copy constructor
   /// @param right SRIleastSquares to be copied
   SRIleastSquares(const SRIleastSquares& right)
      throw()
      { *this = right; }

   /// operator=
   /// @param right SRIleastSquares to be copied
   SRIleastSquares& operator=(const SRIleastSquares& right)
      throw();

   /// A general least squares update, NOT the SRIF (Kalman) measurement update.
   /// Given data and measurement covariance, compute a solution and
   /// covariance using the appropriate least squares algorithm.
   /// @param D   Data vector, length M
   ///               Input:  raw data
   ///               Output: post-fit residuals
   /// @param X   Solution vector, length N
   ///               Input:  nominal solution X0 (zero when doLinearized is false)
   ///               Output: final solution
   /// @param Cov Covariance matrix, dimension (N,N)
   ///               Input:  (If doWeight is true) inverse measurement covariance
   ///                       or weight matrix(M,M)
   ///               Output: Solution covariance matrix (N,N)
   /// @param LSF Pointer to a function which is used to define the equation
   ///            to be solved.
   ///            LSF arguments are:
   ///            X  Nominal solution (input)
   ///            f  Values of the equation f(X) (length M) (output)
   ///            P  Partials matrix df/dX evaluated at X (dimension M,N) (output)
   ///        When doLinearize is false, LSF ignores X and returns the (constant)
   ///        partials matrix P and zero for f(X).
   /// @throw MatrixException if the input is inconsistent
   /// Return values: 0 ok
   ///               -1 Problem is underdetermined (M<N) // TD -- naturalized sol?
   ///               -2 Problem is singular
   ///               -3 Algorithm failed to converge
   ///               -4 Algorithm diverged
   int dataUpdate(Vector<double>& D,
                  Vector<double>& X,
                  Matrix<double>& Cov,
                  void (LSF)(Vector<double>& X,
                             Vector<double>& f,
                             Matrix<double>& P)
      )
      throw(MatrixException);

   /// output operator
   friend std::ostream& operator<<(std::ostream& s,
                                   const SRIleastSquares& srif);

   /// remove all stored information by setting the SRI to zero
   /// (does not re-dimension).
   void zeroAll(void);

   /// Return true if the solution is valid, i.e. if the problem is non-singular.
   bool isValid() { return valid; }

   /// reset the computation, i.e. remove all stored information, and
   /// optionally change the dimension. If N is not input, the
   /// dimension is not changed.
   /// @param N new SRIleastSquares dimension (optional).
   void Reset(const int N=0) throw(Exception);

   // ------------- member functions ---------------
   /// Get the current solution vector
   /// @return current solution vector
   Vector<double> Solution(void) { return Xsave; }

   /// Get the number of iterations used in last call to leastSquaresEstimation()
   /// @return the number of iterations
   int Iterations() { return number_iterations; }

   /// Get the convergence value found in last call to leastSquaresEstimation()
   /// @return the convergence value
   double Convergence() { return rms_convergence; }

   /// Get the condition number of the covariance matrix from last calls
   /// to leastSquaresEstimation() (Larger means 'closer to singular' except
   /// zero means condition number is infinite)
   double ConditionNumber() { return condition_number; }

   // ------------- member data ---------------
   /// limit on the number of iterations
   int iterationsLimit;

   /// limit on the RSS change in solution which produces success
   double convergenceLimit;

   /// upper limit on the RSS change in solution which produces an abort
   double divergenceLimit;

   /// if true, weight the equation using the inverse of covariance matrix
   /// on input - default is false
   bool doWeight;

   /// if true, weight the equation using robust statistical techniques
   /// - default is false
   bool doRobust;

   /// if true, save information for a sequential solution - default is false
   bool doSequential;

   /// if true, equation F(X)=D is non-linear, the algorithm will be iterated,
   /// and LSF must return partials matrix and F(X). - default is false
   bool doLinearize;

   /// if true, output intermediate results
   bool doVerbose;

private:
   /// initialization used by constructors - leastSquaresEstimation() only
   void defaults(void) throw()
   {
      iterationsLimit = 10;
      convergenceLimit = 1.e-8;
      divergenceLimit = 1.e10;
      doWeight = false;
      doRobust = false;
      doLinearize = false;
      doSequential = false;
      doVerbose = false;
      number_iterations = number_batches = 0;
      rms_convergence = condition_number = 0.0;
      valid = false;
   }

   // private member data - inherits from SRI
      // inherit SRI Information matrix, an upper triangular (square) matrix
   //Matrix<double> R;
      // inherit SRI state vector, of length equal to dimension (row and col) of R.
   //Vector<double> Z;
      // inherit SRI Namelist parallel to R and Z, labelling elements of state vector.
   //Namelist names;

   // --------- private member data ------------
   /// indicates if the filter is valid - set false when singular
   bool valid;

   /// current number of iterations
   int number_iterations;

   /// current number of batches seen
   int number_batches;

   /// RMS change in state, used for convergence test
   double rms_convergence;

   /// condition number, defined in inversion to get state and covariance
   double condition_number;

   /// solution X consistent with current information RX=z
   Vector<double> Xsave;

}; // end class SRIleastSquares

} // end namespace gpstk

//------------------------------------------------------------------------------------
#endif
