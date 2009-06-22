#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file SRI.hpp
 * Include file defining class SRI.
 * class SRI implements the square root information methods, used for least squares
 * estimation and the SRI form of the Kalman filter.
 *
 * Reference: "Factorization Methods for Discrete Sequential Estimation,"
 *             by G.J. Bierman, Academic Press, 1977.
 */

//------------------------------------------------------------------------------------
// TD go back thru and add const and throw() everywhere, also in Namelist
// TD check that names CAN have different length than R and Z -- see zeroAll

//------------------------------------------------------------------------------------
#ifndef CLASS_SQUAREROOTINFORMATION_INCLUDE
#define CLASS_SQUAREROOTINFORMATION_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ostream>
#include <sstream>
// GPSTk
#include "Matrix.hpp"
// geomatics
#include "Namelist.hpp"

namespace gpstk
{

//------------------------------------------------------------------------------------
/// constant (empty) Matrix used for default input arguments
extern const Matrix<double> SRINullMatrix;

//------------------------------------------------------------------------------------
// fundamental routines
/// Compute inverse of upper triangular matrix, returning smallest and largest
/// eigenvalues.
/// @param UT upper triangular matrix to be inverted
/// @param ptrS pointer to <T> small, on output *ptrS contains smallest eigenvalue.
/// @param ptrB pointer to <T> small, on output *ptrB contains largest eigenvalue.
/// @return inverse of input matrix.
/// @throw MatrixException if input is not square (assumed upper triangular as well).
/// @throw SingularMatrixException if input is singular.
template <class T>
Matrix<T> inverseUT(const Matrix<T>& UT,
                    T *ptrSmall,
                    T *ptrBig)
   throw(MatrixException);

/// Compute the product of an upper triangular matrix and its transpose.
/// @param UT upper triangular matrix
/// @return product UT * transpose(UT)
/// @throw MatrixException if input is not square (assumed upper triangular as well).
template <class T>
Matrix<T> UTtimesTranspose(const Matrix<T>& UT)
   throw(MatrixException);

/// Square root information filter (Srif) measurement update (MU).
/// Use the Householder transformation to combine the information stored in the square
/// root information (SRI) covariance matrix R and state Z with new information in
/// the given partials matrix and data vector to produce an updated SRI {R,Z}.
/// Measurement noise associated with the new information (H and D) is assumed to be
/// white with unit covariance. If necessary, the data may be 'whitened' by
/// multiplying H and D by the inverse of the lower triangular square root of the
/// covariance matrix; that is, compute L = Cholesky(Measurement covariance) and
/// let H = L*H, D = L*D.
/// @param  R  Upper triangluar apriori SRI covariance matrix of dimension N
/// @param  Z  A priori SRI state vector of length N
/// @param  H  Partials matrix of dimension MxN, trashed on output.
/// @param  D  Data vector of length M; on output contains the residuals of fit.
/// @param  M  If H and D have dimension M' > M, then call with M = true data length;
///             otherwise M = 0 (the default) and is ignored.
/// @throw MatrixException if the input has inconsistent dimensions.
template <class T>
void SrifMU(Matrix<T>& R,
            Vector<T>& Z,
            Matrix<T>& H,
            Vector<T>& D,
            unsigned int M=0)
   throw(MatrixException);

/// Square root information measurement update, with new data in the form of a
/// single matrix concatenation of H and D: A = H || D.
/// See doc for the overloaded SrifMU().
template <class T>
void SrifMU(Matrix<T>& R,
            Vector<T>& Z,
            Matrix<T>& A,
            unsigned int M=0)
   throw(MatrixException);

//------------------------------------------------------------------------------------
/// class SRI encapsulates all the information associated with the solution of a set
/// of simultaneous linear equations. It is used in least squares estimation (linear
/// and linearized) and is the basis of the preferred implementation of Kalman
/// filtering. An SRI consists of just three things:
/// (1) 'R', the 'information matrix', which is an upper triangular matrix of
/// dimension N, equal to the inverse of the square root (or Cholesky decomposition)
/// of the solution covariance matrix,
/// (2) 'Z', the 'SRI state vector' of length N (parallels the components of R),
/// (not to be confused with the regular state vector X), and
/// (3) 'names', a Namelist used to label the elements of R and Z (parallels and
/// labels rows and columns of R and elements of Z). A Namelist is part of class SRI
/// because the manipulations of SRI (see functions below) requires a consistent way
/// of manipulating the different individual elements of R and Z, in addition it
/// allows the user to attach 'human-readable' labels to the elements of the state
/// vector, which is useful in adding, dropping and bumping states, and it makes
/// printed results more readable (see the LabelledMatrix class in Namelist.hpp).
///
/// The set of simultaneous equations represented by an SRI is R * X = Z, where X is
/// the (unknown) state vector (the conventional solution vector) also of dimension N.
/// The state X is solved for as X = inverse(R) * Z, and the covariance matrix of the
/// state X is equal to transpose(inverse(R))*inverse(R).
///
/// Least squares estimation via SRI is very simple and efficient; it uses the
/// Householder transformation to convert the problem to upper triangular form, and
/// then uses very efficient algorithms to invert the information matrix to find the
/// solution and its covariance. The usual matrix equation is H * X = D,
/// where H is the 'design matrix' or the 'partials matrix', of dimension M x N,
/// X is the (unknown) solution vector of length N, and D is the 'data' or
/// 'measurement' vector of length M. In the least squares 'update' of the SRI,
/// this set of information {H,D} is concatenated with the existing SRI {R,Z} to
/// form an (N+M x N+1) matrix Q which has R in the upper left, Z upper right,
/// H lower left and D lower right. This extended matrix is then subjected to a
/// Householder transformation (see class Matrix), which will put (at least the
/// first N columns of) Q into upper triangular form. The result is a new, updated
/// SRI (R and Z) in the place of the old, while in place of D are residuals of fit
/// corresponding to the measurements in D (the H part of Q is trashed). This result,
/// in fact (see the reference), produces an updated SRI which gives precisely the
/// usual least squares solution for the combined 'a priori SRI + new data' problem.
/// This algorithm is called a 'measurement update' of the SRI.
///
/// It is most enlightening to think of the SRI and this process in terms of
/// 'information'. The SRI contains all the 'information' which has come from
/// updates that have been made to it using (H,D) pairs. Initially, the SRI is all
/// zeros, which corresponds to 'no information'. This overcomes one serious problem
/// with conventional least squares and the Kalman algorithm, namely that a
/// 'zero information' starting value cannot be correctly expressed, because in that
/// case the covariance matrix is singular and the state vector is indeterminate;
/// in the SRI method this is perfectly consistent - the covariance matrix is
/// singular because the information matrix (R) is zero, and thus the state
/// is entirely indeterminate. As new 'information' (in the form of data D and
/// partials matrix H pairs) is added to the SRI (via the Householder algorithm),
/// the 'information' stored in R and Z is increased and they become non-zero.
/// (By the way note that the number of rows in the {H,D} information is arbitrary -
/// information can be added in 'batches' - M large - or one - M=1 - piece at a time.)
/// When there is enough information, R becomes non-singular, and so can be inverted
/// and the solution and covariance can be computed. As the amount of information
/// becomes large, elements of R become large, and thus elements of the covariance
/// (think of covariance as a measure of uncertainty - the opposite or inverse of
/// information) become small.
///
/// The structure of the SRI method allows some powerful techniques to be used in 
/// manipulating, combining and separating state elements and the information
/// associated with them in SRIs. For example, if the measurement updates have
/// failed to increase the information about one particular state element, then
/// that element, and its information, may be removed from the problem by deleting
/// that element's row and column of R, and its element of Z (and then
/// re-triangularizing the SRI). In general, any subset of an SRI may be separated,
/// or the SRI split (see the routine of that name below - note the caveats) into
/// two separate SRIs. For another example, SRI allows the information of a each
/// state element to be selectively reduced or even zeroed, simply by multiplying
/// the corresponding elements of R and Z by a factor; in Kalman filtering this
/// is called a 'Q bump' of the element and is very important in some filtering
/// applications. There are methods (see below) consistently to merge (operator+()),
/// split, and permute elements of, SRIs.
///
/// Kalman filtering is an important application of SRI methods (actually it is
/// called 'square root information filtering' or SRIF - technically the term
/// 'Kalman filter algorithm' is reserved for the classical algorithm just as
/// Kalman presented it, in terms of a state vector and its covariance matrix).
/// The measurment update described above (which is actually just linear least
/// squares) is half of the SRIF (Kalman filter) - there is a 'time update' that
/// propagates the SRI (and thus the state and covariance) forward in time using
/// the dynamical model of the filter. These are algebraically equivalent to the
/// classical Kalman algorithm, but are more efficient and numerically stable
/// (actually the Kalman algorithm has been shown to be numerically unstable!).
/// There are even SRI smoothing algorithms, corresponding to Kalman smoothers,
/// which consist of a 'backwards' filter, implemented by applying a
/// 'smoother update' to the SRI at each point in reverse order.
///
/// Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential Estimation,"
///      Academic Press, 1977.
class SRI {
public:
      /// empty constructor
   SRI(void) throw() { }

      /// constructor given the dimension N.
   SRI(const unsigned int)
      throw();

      /// constructor given a Namelist, its dimension determines the SRI dimension.
   SRI(const Namelist&)
      throw();

      /// explicit constructor - throw if the dimensions are inconsistent.
   SRI(const Matrix<double>&,
       const Vector<double>&,
       const Namelist&)
      throw(MatrixException);

      /// copy constructor
   SRI(const SRI&)
      throw();

      /// operator=
   SRI& operator=(const SRI& right)
      throw();

   // modify SRIs

      /// Permute the SRI elements to match the input Namelist, which may differ with
      /// the SRI Namelist by AT MOST A PERMUTATION; throw if this is not true.
   void permute(const Namelist&)
      throw(MatrixException,VectorException);

      /// split an SRI into two others, this one matching the input Namelist, the
      /// other containing whatever is left. The input Namelist must be a non-trivial
      /// subset of this->names; throw MatrixException if it is not. NB. Interpreting
      /// the results of a split() and merge (operator+()) operations should be done
      /// very carefully; remember that the SRI contains both solution and noise,
      /// and that the results of these operations are not always as expected,
      /// particularly note that split() and operator+() are usually NOT reversible.
   void split(const Namelist&, SRI&)
      throw(MatrixException,VectorException);

      /// extend this SRI to include the given Namelist, with no added information;
      /// names in the input namelist which are not unique are ignored.
   SRI& operator+=(const Namelist&)
      throw(MatrixException,VectorException);

      /// reshape this SRI to match the input Namelist, by calling other member
      /// functions, including split(), operator+() and permute()
   void reshape(const Namelist&)
      throw(MatrixException,VectorException);

      /// merge an SRI into this one. NB names may be reordered in the result.
   void merge(const SRI& S)
      throw(MatrixException,VectorException)
   { *this += S; }

      /// merge this SRI with the given input SRI. ? should this be operator&=() ?
      /// NB may reorder the names in the resulting Namelist.
   SRI& operator+=(const SRI&)
      throw(MatrixException,VectorException);

      /// merge two SRIs to produce a third. ? should this be operator&() ?
   friend SRI operator+(const SRI&,
                        const SRI&)
      throw(MatrixException,VectorException);

      /// Zero out the nth row of R and the nth element of Z, removing all
      /// information about that element.
   void zeroOne(const unsigned int n)
      throw();

      /// Zero out all the first n rows of R and elements of Z, removing all
      /// information about those elements. Default value of the input is 0,
      /// meaning zero out the entire SRI.
   void zeroAll(const int n=0)
      throw();

      /// Zero out (set all elements to zero) the state (Vector Z) only.
   void zeroState(void)
      throw()
   { Z = 0.0; }

      /// Reset the SRI, meaning zero it and optionally change the dimension to n.
      /// @param n Dimension of the new object (optional).
   //void reset(int n=0)
   //   throw();

      /// Shift the state vector by a constant vector X0; does not change information
      /// i.e. let R * X = Z => R' * (X-X0) = Z'
      /// throw on invalid input dimension
   void shift(const Vector<double>&)
      throw(MatrixException);

      /// Transform this SRI with the transformation matrix T;
      /// i.e. R -> T * R * inverse(T) and Z -> T * Z. The matrix inverse(T)
      /// may optionally be supplied as input, otherwise it is computed from
      /// T. NB names in this SRI are most likely changed; but this routine does
      /// not change the Namelist. Throw MatrixException if the input has
      /// the wrong dimension or cannot be inverted.
   void transform(const Matrix<double>& T,
                  const Matrix<double>& invT=SRINullMatrix)
      throw(MatrixException,VectorException);

      /// Transform the state by the transformation matrix T; i.e. X -> T*X,
      /// without transforming the SRI; this is done by right multiplying R by
      /// inverse(T), which is the input. Thus R -> R*inverse(T),
      /// so R*inverse(T)*T*X = Z.  Input is the _inverse_ of the transformation.
      /// throw MatrixException if input dimensions are wrong.
   void transformState(const Matrix<double>& invT)
      throw(MatrixException);

      /// Decrease the information in this SRI for, or 'Q bump', the element
      /// with the input index.  This means that the uncertainty and the state
      /// element given by the index are divided by the input factor q; the
      /// default input is zero, which means zero out the information (q = infinite).
      /// A Q bump by factor q is equivalent to 'de-weighting' the element by q.
      /// No effect if in is out of range.
   void Qbump(const unsigned int& in,
              const double& q=0.0)
      throw(MatrixException,VectorException);

      /// Fix the state element with the input index to the input value, and
      /// collapse the SRI by removing that element.
      /// No effect if index is out of range.
   void biasFix(const unsigned int&,
                const double&)
      throw(MatrixException,VectorException);

      /// Vector version of biasFix, with Namelist identifying the states.
      /// Fix the given state elements to the input value, and
      /// collapse the SRI by removing those elements.
      /// No effect if name is not found.
   void biasFix(const Namelist& drops,
                const Vector<double>& biases)
      throw(MatrixException,VectorException);

      /// Add a priori or constraint information in the form of an ordinary
      /// state vector and covariance matrix.
      /// @param Cov Covariance matrix of same dimension as this SRIFilter
      /// @param X   State vector of same dimension as this SRIFilter
      /// @throw if input is invalid: dimensions are wrong or Cov is singular.
   void addAPriori(const Matrix<double>& Cov, const Vector<double>& X)
      throw(MatrixException);

      /// Add a priori or constraint information in the form of an information
      /// matrix (inverse covariance) and ordinary state.
      /// @param ICov Inverse covariance matrix of same dimension as this SRIFilter
      /// @param X    State vector of same dimension as this SRIFilter
      /// @throw if input is invalid: dimensions are wrong
   void addAPrioriInformation(const Matrix<double>& ICov, const Vector<double>& X)
      throw(MatrixException);

      /// SRIF (Kalman) measurement update, or least squares update
      /// Call the SRI measurement update for this SRI and the given input. See doc.
      /// for SrifMU().
   void measurementUpdate(Matrix<double>& Partials,
                          Vector<double>& Data)
      throw(MatrixException)
   { SrifMU(R, Z, Partials, Data); }

      /// Compute the state X and the covariance matrix C of the state, where
      /// C = transpose(inverse(R))*inverse(R) and X = inverse(R) * Z.
      /// Optional pointer arguments will return smallest and largest
      /// eigenvalues of the R matrix, which is a measure of singularity.
      /// @param X State vector (output)
      /// @param C Covariance of the state vector (output)
      /// @param ptrSmall Pointer to double, on output *ptrSmall set to smallest
      ///                 eigenvalue of R
      /// @param ptrBig Pointer to double, on output *ptrBig set to largest
      ///                 eigenvalue of R
      /// @throw SingularMatrixException if R is singular.
      /// NB this is the most efficient way to invert the SRI equation.
   void getStateAndCovariance(Vector<double>& X,
                              Matrix<double>& C,
                              double *ptrSmall=NULL,
                              double *ptrBig=NULL)
      throw(MatrixException,VectorException);

      // member access
      /// return the size of the SRI, which is the dimension of R(rows and columns),
      /// Z and names.
   unsigned int size(void) const
      throw()
   { return R.rows(); }

      /// access the Namelist of the SRI
   Namelist getNames(void)
      throw()
   { return names; }

      /// access the name of a specific state element, given its index.
      /// returns 'out-of-range' if the index is out of range.
   std::string getName(const unsigned int in)
      throw()
   { return names.getName(in); }

      /// assign the name of a specific state element, given its index;
      /// no effect, and return false, if the name is not unique;
      /// return true if successful.
   bool setName(const unsigned int in,
                const std::string& name)
      throw()
   { return names.setName(in,name); }

      /// return the index of the name in the Namelist that matches the input, or
      /// -1 if not found.
   unsigned int index(std::string& name)
      throw()
   { return names.index(name); }

      /// output operator
   friend std::ostream& operator<<(std::ostream& s,
                                   const SRI&);

protected:
   // member data
      /// Information matrix, an upper triangular (square) matrix
   Matrix<double> R;

      /// SRI state vector, of length equal to the dimension (row and col) of R.
   Vector<double> Z;

      /// Namelist parallel to R and Z, labelling the elements of the state vector.
   Namelist names;

}; // end class SRI

} // end namespace gpstk

//------------------------------------------------------------------------------------
#endif
