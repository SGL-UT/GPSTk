//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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
 * @file SRIFilter.hpp
 * Include file defining class SRIFilter.
 * class SRIFilter implements the square root information matrix form of the
 * Kalman filter and smoother.
 *
 * Reference: "Factorization Methods for Discrete Sequential Estimation,"
 *             G.J. Bierman, Academic Press, 1977.
 */

//------------------------------------------------------------------------------------
#ifndef CLASS_SQUAREROOT_INFORMATION_FILTER_INCLUDE
#define CLASS_SQUAREROOT_INFORMATION_FILTER_INCLUDE

//------------------------------------------------------------------------------------
// system
#include <ostream>
// GPSTk
#include "Vector.hpp"
#include "Matrix.hpp"
// geomatics
#include "SRI.hpp"
#include "SparseMatrix.hpp"

namespace gpstk
{

//------------------------------------------------------------------------------------
/** class SRIFilter inherits SRI and implements a square root information filter,
 * which is the square root formulation of the Kalman filter algorithm. SRIFilter may
 * be used for Kalman filtering, smoothing, or for simple least squares, including
 * weighted, linear or linearized, robust and/or sequential algorithms.
 *
 * At any point the state X and covariance P are related to the SRI by
 * X = inverse(R) * z , P = inverse(R) * inverse(transpose(R)), or
 * R = upper triangular square root (Cholesky decomposition) of the inverse of P,
 * and z = R * X.
 *
 * The SRIFilter implements Kalman filter algorithm, which includes sequential least
 * squares (measurement update), dynamic propagation (time update), and smoothing
 * (technically the term 'Kalman filter algorithm' is reserved for the classical
 * algorithm just as Kalman presented it, in terms of a state vector and its
 * covariance matrix).
 *
 * The SRIFilter measurment update (which is actually just linear least squares) is
 * half of the SRIFilter (Kalman filter) - there is a 'time update' that propagates
 * the SRI (and thus the state and covariance) forward in time using the dynamical
 * model of the filter. These are algebraically equivalent to the classical Kalman
 * algorithm, but are more efficient and numerically stable (actually the Kalman
 * algorithm has been shown to be numerically unstable!). The SRIFilter smoothing
 * algorithms consists of a 'backwards' filter, implemented by applying a
 * 'smoother update' to the SRIFilter at each point in reverse order.
 *
 *  Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential Estimation,"
 *       Academic Press, 1977.
 */
class SRIFilter : public SRI {
public:
      /// empty constructor
   SRIFilter(void) throw();

      /// constructor given the dimension N.
      /// @param N dimension of the SRIFilter.
   SRIFilter(const unsigned int N)
      throw();

      /// constructor given a Namelist; its dimension determines the SRI dimension.
      /// @param NL Namelist for the SRIFilter.
   SRIFilter(const Namelist& NL)
      throw();

      /// explicit constructor - throw if the dimensions are inconsistent.
      /// @param R  Initial information matrix, an upper triangular matrix of dim N.
      /// @param Z  Initial information data vector, of length N.
      /// @param NL Namelist for the SRIFilter, also of length N.
      /// @throw MatrixException if dimensions are not consistent.
   SRIFilter(const Matrix<double>& R,
             const Vector<double>& Z,
             const Namelist& NL)
      throw(MatrixException);

      /// copy constructor
      /// @param right SRIFilter to be copied
   SRIFilter(const SRIFilter& right)
      throw()
      { *this = right; }

      /// operator=
      /// @param right SRIFilter to be copied
   SRIFilter& operator=(const SRIFilter& right)
      throw();

      /// SRIF (Kalman) simple linear measurement update with optional weight matrix
      /// @param H  Partials matrix, dimension MxN.
      /// @param D  Data vector, length M; on output D is post-fit residuals.
      /// @param CM Measurement covariance matrix, dimension MxM.
      /// @throw if dimension N does not match dimension of SRI, or if other
      ///        dimensions are inconsistent, or if CM is singular.
   void measurementUpdate(const Matrix<double>& H, Vector<double>& D,
                          const Matrix<double>& CM=SRINullMatrix)
   throw(MatrixException,VectorException);

      /// SRIF (Kalman) simple linear measurement update with optional weight matrix
      /// SparseMatrix version
      /// @param H  Partials matrix, dimension MxN.
      /// @param D  Data vector, length M; on output D is post-fit residuals.
      /// @param CM Measurement covariance matrix, dimension MxM.
      /// @throw if dimension N does not match dimension of SRI, or if other
      ///        dimensions are inconsistent, or if CM is singular.
   void measurementUpdate(const SparseMatrix<double>& H, Vector<double>& D,
                          const SparseMatrix<double>& CM=SRINullSparseMatrix)
   throw(MatrixException,VectorException);

      /// SRIF (Kalman) time update
      /// This routine uses the Householder transformation to propagate the SRIFilter
      /// state and covariance through a time step.
      /// If the existing SRI state is of dimension n, and the number of noise
      /// parameter is ns, then the inputs must be dimensioned as indicated.
      /// @param PhiInv Matrix<double>
      ///        Inverse of state transition matrix, an n by n matrix.
      ///        PhiInv is destroyed on output.
      /// @param Rw Matrix<double>
      ///        a priori square root information matrix for the process
      ///        noise, an ns by ns upper triangular matrix. Rw is modified on output.
      /// @param G Matrix<double>
      ///        The n by ns matrix associated with process noise, which relates the
      ///        state to the noise variables.  The process noise covariance is
      ///        G*Q*transpose(G) where inverse(Q) is transpose(Rw)*Rw.
      ///        G is destroyed on output.
      /// @param Zw Vector<double>
      ///        a priori 'state' associated with the process noise,
      ///        a vector with ns elements.  Usually set to zero by
      ///        the calling routine (for unbiased process noise). Used for output.
      /// @param Rwx Matrix<double>
      ///        An ns by n matrix which is set to zero by this routine 
      ///        and used for output.
      /// 
      /// Output:
      ///   The updated square root information matrix and SRIF state (R,Z) and
      /// the matrices which are used in smoothing: Rw, Zw, Rwx.
      /// 
      /// @throw MatrixException if the input is inconsistent
      /// @return void
      /// 
      /// Method:
      ///   This SRIF time update method treats the process noise and mapping
      /// information as a separate data equation, and applies a Householder
      /// transformation to the (appended) equations to solve for an updated
      /// state.  Thus there is another 'state' variable associated with 
      /// whatever state variables have process noise.  The matrix G relates
      /// the process noise variables to the regular state variables, and 
      /// appears in the term GQtranspose(G) of the covariance.  If all n state
      /// variables have process noise, then ns=n and G is an n by n matrix.
      /// Since some (or all) of the state variables may not have process 
      /// noise, ns may be zero.  [Ref. Bierman ftnt pg 122 seems to indicate that
      /// variables with zero process noise can be handled by ns=n & setting a
      /// column of G=0.  But note that the case of the matrix G=0 is the
      /// same as ns=0, because the first ns columns would be zero below the
      /// diagonal in that case anyway, so the HH transformation would be 
      /// null.]
      ///   For startup, all of the a priori information and state arrays may
      /// be zero.  That is, "no information" would imply that R and Z are zero,
      /// as well as Zw.  A priori information (covariance) and state
      /// are handled by setting P = inverse(R)*transpose(inverse((R)), Z = R*X.
      /// Normally Zw is reset to zero before each call.
      ///   There are three ways to handle non-zero process noise covariance.
      /// (1) If Q is the (known) a priori process noise covariance Q, then
      /// set Q=Rw(-1)*Rw(-T), and G=1.
      /// (2) Transform process noise covariance matrix to UDU form, Q=UDU,
      /// then set G=U  and Rw = (D)**-1/2.
      /// (3) Take the sqrt of process noise covariance matrix Q, then set
      /// G=this sqrt and Rw = 1.  [2 and 3 have been tested.]
      ///   The routine applies a Householder transformation to a large
      /// matrix formed by concatenation of the input matricies.  Two preliminary 
      /// steps are to form Rd = R*PhiInv (stored in PhiInv) and -Rd*G (stored in 
      /// G) by matrix multiplication, and to set Rwx to the zero matrix.  
      /// Then the Householder transformation is applied to the following
      /// matrix, dimensions are shown in ():
      ///      _  (ns)   (n)   (1)  _          _                  _
      /// (ns) |    Rw     0     Zw   |   ==>  |   Rw   Rwx   Zw    |
      /// (n)  |  -Rd*G   Rd     Z    |   ==>  |   0     R    Z     | .
      ///      -                    -          -                  -
      /// The SRI matricies R and Rw remain upper triangular.
      ///
      ///   The matrix Rwx is related to the sensitivity of the state
      /// estimate to the unmodeled parameters in Zw.  The sensitivity matrix
      /// is          Sen = -inverse(Rw)*Rwx,
      /// where perturbation in model X = 
      ///               Sen * diagonal(a priori sigmas of parameter uncertainties).
      ///
      ///   The quantities Rw, Rwx and Zw on output are to be saved and used
      /// in the sqrt information fixed interval smoother (SRIS), during the
      /// backward filter process.
      /// -------------------------------------------------------------------
      /// Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential
      ///      Estimation," Academic Press, 1977, pg 121.
   void timeUpdate(Matrix<double>& PhiInv,
                   Matrix<double>& Rw,
                   Matrix<double>& G,
                   Vector<double>& zw,
                   Matrix<double>& Rwx)
      throw(MatrixException);

      /// SRIF (Kalman) smoother update
      /// This routine uses the Householder transformation to propagate the SRIF
      /// state and covariance through a smoother (backward filter) step.
      /// If the existing SRI state is of dimension N, and the number of noise
      /// parameter is Ns, then the inputs must be dimensioned as indicated.
      /// @param Phi Matrix<double>
      ///        State transition matrix, an N by N matrix.
      ///        Phi is destroyed on output.
      /// @param Rw Matrix<double>
      ///        A priori square root information matrix for the process
      ///        noise, an Ns by Ns upper triangular matrix (which has 
      ///        Ns(Ns+1)/2 elements), output of the time update.
      /// @param G Matrix<double>
      ///        The n by ns matrix associated with process noise, which relates the
      ///        state to the noise variables.  The process noise covariance is
      ///        G*Q*transpose(G) where inverse(Q) is transpose(Rw)*Rw,
      ///        also input to the time update. G is destroyed on output.
      /// @param zw Vector<double>
      ///        A priori 'state' associated with the process noise,
      ///        a vector with Ns elements, output of the time update.
      /// @param Rwx Matrix<double> An Ns by N matrix, output of the time update.
      ///
      /// The inputs Rw,zw,Rwx are the output of the SRIF time update, and these and
      /// Phi and G are associated with the same timestep. All the inputs are trashed
      /// on output.
      /// 
      /// @throw MatrixException if the input is inconsistent
      /// @return void
      /// 
      /// Method:
      ///   The fixed interval square root information smoother (SRIS) is 
      /// composed of two Kalman filters, one identical with the square root 
      /// information filter (SRIF), the other similar but operating on the
      /// data in reverse order and combining the current (smoothed) state
      /// with elements output by the SRIF in its forward run.
      /// Thus a smoother is composed of a forward filter which saves all of
      /// its output, followed by a backward filter which makes use of that
      /// saved information.
      ///   This form of the SRIF backward filter algorithm is equivalent to the
      /// Dyer-McReynolds SRIS algorithm, which uses less computer resources, but
      /// propagates the state and covariance rather than the SRI (R,z). [As always,
      /// at any point the state X and covariance P are related to the SRI by
      /// X = inverse(R) * z , P = inverse(R) * inverse(transpose(R)).]
      ///   For startup of the backward filter, the state after the final 
      /// measurement update of the SRIF is given another time update, the
      /// output of which is identified with the a priori values for the 
      /// backward filter.  Backward filtering proceeds from there, the N+1st
      /// point, toward the first point.
      ///
      ///   In this implementation of the backward filter, the Householder
      /// transformation is applied to the following matrix
      /// [dimensions are shown in ()]:
      /// 
      ///      _  (Ns)     (N)      (1) _          _                  _
      /// (Ns) |  Rw+Rwx*G  Rwx*Phi  zw   |   ==>  |   Rw   Rwx   zw    |
      /// (N)  |  R*G       R*Phi    z    |   ==>  |   0     R    z     | .
      ///      -                        -          -                  -
      /// The SRI matricies R and Rw remain upper triangular.
      ///
      /// Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential
      ///      Estimation," Academic Press, 1977, pg 216.
   void smootherUpdate(Matrix<double>& Phi,
                       Matrix<double>& Rw,
                       Matrix<double>& G,
                       Vector<double>& zw,
                       Matrix<double>& Rwx)
      throw(MatrixException);

      /// Covariance/State version of the Kalman smoother update (Dyer-McReynolds).
      /// This routine implements the Dyer-McReynolds form of the state and covariance
      /// recursions which constitute the backward filter of the Square Root
      /// Information Smoother; it is equivalent to the SRI form implemented in
      /// SRIFilter::smootherUpdate().
      /// NB. This routine does NOT use the SRIFilter object; it is implemented as a
      /// member function to be consistent with other updates.
      /// 
      /// @param X Vector<double> X(N)
      ///          A priori state, derived from SRI (R*X=Z)
      /// @param P Matrix<double> P(N,N)
      ///          A priori covariance, derived from SRI (P=R^-1*R^-T)
      /// @param Rw Matrix<double> Rw(Ns,Ns)
      ///          Process noise covariance (UT), output of SRIF TU
      /// @param Rwx Matrix<double> Rwx(Ns,N)
      ///          PN 'cross term', output of SRIF TU
      /// @param Zw Vector<double> Zw(Ns)
      ///          Process noise state, output of SRIF TU
      /// @param Phinv Matrix<double> Phinv(N,N)
      ///          Inverse of state transition, saved at SRIF TU
      /// @param G Matrix<double> G(N,Ns)
      ///          Noise coupling matrix, saved at SRIF TU
      /// @throw MatrixException if the input is inconsistent
      /// @return void
      /// On return, X and P are the updated state and covariance, and the
      /// other inputs are trashed.
      /// 
      /// Method:
      ///   The fixed interval square root information smoother (SRIS) is 
      /// composed of two Kalman filters, one identical with the square root 
      /// information filter (SRIF), the other similar but operating on the
      /// data in reverse order and combining the current (smoothed) state
      /// with elements output by the SRIF in its forward run and saved.
      /// Thus a smoother is composed of a forward filter which saves all of
      /// its output, followed by a backward filter which makes use of that
      /// saved information.
      ///   This form of the SRIS algorithm is equivalent to the SRIS backward
      /// filter Householder transformation algorithm, but uses less computer
      /// resources. It is not necessary to update both the state and the
      /// covariance, although doing both at once is less expensive than
      /// doing them separately. (This routine does both.) Besides being more
      /// efficient, it requires the inverse of the state transition matrix,
      /// which is what was used in the time update (the SRIS requires the
      /// non-inverse state transition matrix).
      ///   For startup of the backward filter, the state after the final 
      /// measurement update of the SRIF is given another time update, the
      /// output of which is identified with the a priori values for the 
      /// backward filter.  Backward filtering proceeds from there, the N+1st
      /// point, toward the first point.
      ///
      /// Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential
      ///      Estimation," Academic Press, 1977.
      ///
   static void DMsmootherUpdate(Matrix<double>& P,
                                Vector<double>& X,
                                Matrix<double>& Phinv,
                                Matrix<double>& Rw,
                                Matrix<double>& G,
                                Vector<double>& Zw,
                                Matrix<double>& Rwx)
      throw(MatrixException);

      /// Modification for case with control vector: Xj+1 = Phi*Xj + Gwj + u
   static void DMsmootherUpdateWithControl(Matrix<double>& P,
                                           Vector<double>& X,
                                           Matrix<double>& Phinv,
                                           Matrix<double>& Rw,
                                           Matrix<double>& G,
                                           Vector<double>& Zw,
                                           Matrix<double>& Rwx,
                                           Vector<double>& U)
      throw(MatrixException);

      /// remove all stored information by setting the SRI to zero
      /// (does not re-dimension).
   void zeroAll(void);

      /// reset the computation, i.e. remove all stored information, and
      /// optionally change the dimension. If N is not input, the
      /// dimension is not changed.
      /// @param N new SRIFilter dimension (optional).
   void Reset(const int N=0);

private:
      /// SRIF time update (non-SRI version); SRIFilter::timeUpdate for doc.
   template <class T>
   static void SrifTU(Matrix<T>& R,
                      Vector<T>& Z,
                      Matrix<T>& Phi,
                      Matrix<T>& Rw,
                      Matrix<T>& G,
                      Vector<T>& Zw,
                      Matrix<T>& Rwx)
      throw(MatrixException);

      /// SRIF smoother update (non-SRI version); SRIFilter::smootherUpdate for doc.
   template <class T>
   static void SrifSU(Matrix<T>& R,
                      Vector<T>& Z,
                      Matrix<T>& Phi,
                      Matrix<T>& Rw,
                      Matrix<T>& G,
                      Vector<T>& Zw,
                      Matrix<T>& Rwx)
      throw(MatrixException);

      /// SRIF smoother update in covariance / state form;
      /// see SRIFilter::DMsmootherUpdate() for doc.
   template <class T>
   static void SrifSU_DM(Matrix<T>& P,
                         Vector<T>& X,
                         Matrix<T>& Phinv,
                         Matrix<T>& Rw,
                         Matrix<T>& G,
                         Vector<T>& Zw,
                         Matrix<T>& Rwx)
      throw(MatrixException);

      /// initialization used by constructors
   void defaults(void) throw()
   {
      //valid = false;
   }

   // private member data - inherits from SRI
      // inherit SRI Information matrix, an upper triangular (square) matrix
   //Matrix<double> R;
      // inherit SRI state vector, of length equal to dimension (row and col) of R.
   //Vector<double> Z;
      // inherit SRI Namelist parallel to R and Z, labelling elements of state vector.
   //Namelist names;

   // --------- private member data ------------
   // TD how to implement valid?
   // indicates if filter is valid - set false when inversion finds singularity.
   //bool valid;

}; // end class SRIFilter

} // end namespace gpstk

//------------------------------------------------------------------------------------
#endif
