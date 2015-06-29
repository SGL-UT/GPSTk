/// @file SRIFilter.cpp  Implementation of class SRIFilter.
/// class SRIFilter implements the square root information matrix form of the
/// Kalman filter.
///
/// Reference: "Factorization Methods for Discrete Sequential Estimation,"
///             G.J. Bierman, Academic Press, 1977.

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

//------------------------------------------------------------------------------------
#include "SRIFilter.hpp"
#include "RobustStats.hpp"
#include "StringUtils.hpp"

//------------------------------------------------------------------------------------
// TD
using namespace std;

namespace gpstk
{

using namespace StringUtils;

//------------------------------------------------------------------------------------
// empty constructor
SRIFilter::SRIFilter(void)
   throw()
{
   defaults();
}

//------------------------------------------------------------------------------------
// constructor given the dimension N.
SRIFilter::SRIFilter(const unsigned int N)
   throw()
{
   defaults();
   R = Matrix<double>(N,N,0.0);
   Z = Vector<double>(N,0.0);
   names = Namelist(N);
}

//------------------------------------------------------------------------------------
// constructor given a Namelist, its dimension determines the SRI dimension.
SRIFilter::SRIFilter(const Namelist& NL)
   throw()
{
   defaults();
   if(NL.size() <= 0) return;
   R = Matrix<double>(NL.size(),NL.size(),0.0);
   Z = Vector<double>(NL.size(),0.0);
   names = NL;
}

//------------------------------------------------------------------------------------
// explicit constructor - throw if the dimensions are inconsistent.
SRIFilter::SRIFilter(const Matrix<double>& Rin,
                     const Vector<double>& Zin,
                     const Namelist& NLin)
   throw(MatrixException)
{
   defaults();
   if(Rin.rows() != Rin.cols() ||
      Rin.rows() != Zin.size() ||
      Rin.rows() != NLin.size()) {
      MatrixException me("Invalid input dimensions: R is "
         + asString<int>(Rin.rows()) + "x"
         + asString<int>(Rin.cols()) + ", Z has length "
         + asString<int>(Zin.size()) + ", and NL has length "
         + asString<int>(NLin.size())
         );
      GPSTK_THROW(me);
   }
   R = Rin;
   Z = Zin;
   names = NLin;
}

//------------------------------------------------------------------------------------
// operator=
SRIFilter& SRIFilter::operator=(const SRIFilter& right)
   throw()
{
   R = right.R;
   Z = right.Z;
   names = right.names;
   //valid = right.valid;
   return *this;
}

//------------------------------------------------------------------------------------
// SRIF (Kalman) measurement update, or least squares update
// Returns unwhitened residuals in D
void SRIFilter::measurementUpdate(const Matrix<double>& H, Vector<double>& D,
                                  const Matrix<double>& CM)
   throw(MatrixException,VectorException)
{
   if(H.cols() != R.cols() || H.rows() != D.size() ||
      (&CM != &SRINullMatrix && (CM.rows() != D.size() || CM.cols() != D.size())) )
   {
      string msg("\nInvalid input dimensions:\n  SRI is ");
      msg += asString<int>(R.rows()) + "x"
          + asString<int>(R.cols()) + ",\n  Partials is "
          + asString<int>(H.rows()) + "x"
          + asString<int>(H.cols()) + ",\n  Data has length "
          + asString<int>(D.size());
      if(&CM != &SRINullMatrix) msg += ",\n  and Cov is "
          + asString<int>(CM.rows()) + "x"
          + asString<int>(CM.cols());

      MatrixException me(msg);
      GPSTK_THROW(me);
   }
   try {
         // whiten partials and data
      Matrix<double> P(H);
      Matrix<double> CHL(lowerCholesky(CM));
      if(&CM != &SRINullMatrix) {
         Matrix<double> L(inverseLT(CHL));
         P = L * P;
         D = L * D;
      }

         // update *this with the whitened information
      SrifMU(R, Z, P, D);

         // un-whiten residuals
      if(&CM != &SRINullMatrix) {         // same if above creates CHL
         D = CHL * D;
      }
   }
   catch(MatrixException& me) { GPSTK_RETHROW(me); }
   catch(VectorException& ve) { GPSTK_RETHROW(ve); }
}

//------------------------------------------------------------------------------------
// SRIF (Kalman) measurement update, or least squares update -- SparseMatrix version
// Returns unwhitened residuals in D
void SRIFilter::measurementUpdate(const SparseMatrix<double>& H, Vector<double>& D,
                                  const SparseMatrix<double>& CM)
   throw(MatrixException,VectorException)
{
   if(H.cols() != R.cols() || H.rows() != D.size() ||
      (&CM != &SRINullSparseMatrix &&
         (CM.rows() != D.size() || CM.cols() != D.size())) )
   {
      string msg("\nInvalid input dimensions:\n  SRI is ");
      msg += asString<int>(R.rows()) + "x"
          + asString<int>(R.cols()) + ",\n  Partials is "
          + asString<int>(H.rows()) + "x"
          + asString<int>(H.cols()) + ",\n  Data has length "
          + asString<int>(D.size());
      if(&CM != &SRINullSparseMatrix) msg += ",\n  and Cov is "
          + asString<int>(CM.rows()) + "x"
          + asString<int>(CM.cols());

      MatrixException me(msg);
      GPSTK_THROW(me);
   }
   try {
      SparseMatrix<double> A(H || D);
      SparseMatrix<double> CHL;
         // whiten partials and data
      if(&CM != &SRINullSparseMatrix) {
         CHL = lowerCholesky(CM);
         SparseMatrix<double> L(inverseLT(CHL));
         A = L * A;
      }

         // update *this with the whitened information
      SrifMU(R, Z, A);

         // copy out D and un-whiten residuals
      D = Vector<double>(A.colCopy(A.cols()-1));
      if(&CM != &SRINullSparseMatrix) {      // same if above creates CHL
         D = CHL * D;
      }
   }
   catch(MatrixException& me) { GPSTK_RETHROW(me); }
   catch(VectorException& ve) { GPSTK_RETHROW(ve); }
}

//------------------------------------------------------------------------------------
// SRIF (Kalman) time update see SrifTU for doc.
void SRIFilter::timeUpdate(Matrix<double>& PhiInv,
                           Matrix<double>& Rw,
                           Matrix<double>& G,
                           Vector<double>& Zw,
                           Matrix<double>& Rwx)
   throw(MatrixException)
{
   try { SrifTU(R, Z, PhiInv, Rw, G, Zw, Rwx); }
   catch(MatrixException& me) { GPSTK_RETHROW(me); }
}

//------------------------------------------------------------------------------------
// SRIF (Kalman) smoother update see SrifSU for doc.
void SRIFilter::smootherUpdate(Matrix<double>& Phi,
                               Matrix<double>& Rw,
                               Matrix<double>& G,
                               Vector<double>& Zw,
                               Matrix<double>& Rwx)
   throw(MatrixException)
{
   try { SrifSU(R, Z, Phi, Rw, G, Zw, Rwx); }
   catch(MatrixException& me) { GPSTK_RETHROW(me); }
}

//------------------------------------------------------------------------------------
void SRIFilter::DMsmootherUpdate(Matrix<double>& P,
                                 Vector<double>& X,
                                 Matrix<double>& Phinv,
                                 Matrix<double>& Rw,
                                 Matrix<double>& G,
                                 Vector<double>& Zw,
                                 Matrix<double>& Rwx)
   throw(MatrixException)
{
   try { SrifSU_DM(P, X, Phinv, Rw, G, Zw, Rwx); }
   catch(MatrixException& me) { GPSTK_RETHROW(me); }
}

//------------------------------------------------------------------------------------
// reset the computation, i.e. remove all stored information
void SRIFilter::zeroAll(void)
{
   SRI::zeroAll();
}

//------------------------------------------------------------------------------------
// reset the computation, i.e. remove all stored information, and
// optionally change the dimension. If N is not input, the
// dimension is not changed.
// @param N new SRIFilter dimension (optional).
void SRIFilter::Reset(const int N)
{
   if(N > 0 && N != (int)R.rows()) {
      R.resize(N,N,0.0);
      Z.resize(N,0.0);
   }
   else
      SRI::zeroAll(N);
}

//------------------------------------------------------------------------------------
// private beyond this
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Kalman time update.
// This routine uses the Householder transformation to propagate the SRIFilter
// state and covariance through a time step.
// Input:
// R       a priori square root information (SRI) matrix (an n by n 
//            upper triangular matrix)
// Z       a priori SRIF state vector, of length n (state is X, Z = R*X).
// PhiInv  Inverse of state transition matrix, an n by n matrix.
//            PhiInv is destroyed on output.
// Rw      a priori square root information matrix for the process
//            noise, an ns by ns upper triangular matrix
// G       The n by ns matrix associated with process noise.  The 
//            process noise covariance is G*Q*transpose(G) where inverse(Q)
//            is transpose(Rw)*Rw. G is destroyed on output.
// Zw      a priori 'state' associated with the process noise,
//            a vector with ns elements.  Usually set to zero by
//            the calling routine (for unbiased process noise).
// Rwx     An ns by n matrix which is set to zero by this routine 
//            but is used for output.
// 
// Output:
//    The updated square root information matrix and SRIF state (R,Z) and
// the matrices which are used in smoothing: Rw, Zw, Rwx.
// Note that PhiInv and G are trashed, and that Rw and Zw are modified.
// 
// Return values:
//    SrifTU returns void, but throws exceptions if the input matrices
// or vectors have incompatible dimensions or incorrect types.
// 
// Method:
//    This SRIF time update method treats the process noise and mapping
// information as a separate data equation, and applies a Householder
// transformation to the (appended) equations to solve for an updated
// state.  Thus there is another 'state' variable associated with 
// whatever state variables have process noise.  The matrix G relates
// the process noise variables to the regular state variables, and 
// appears in the term GQG(trans) of the covariance.  If all n state
// variables have process noise, then ns=n and G is an n by n matrix.
// Since some (or all) of the state variables may not have process 
// noise, ns may be zero.  [Bierman ftnt pg 122 seems to indicate that
// variables with zero process noise can be handled by ns=n & setting a
// column of G=0.  But note that the case of the matrix G=0 is the
// same as ns=0, because the first ns columns would be zero below the
// diagonal in that case anyway, so the HH transformation would be 
// null.]
//    For startup, all of the a priori information and state arrays may
// be zero.  That is, "no information" would imply that R and Z are zero,
// as well as Rw and Zw.  A priori information (covariance) and state
// are handled by setting P = inverse(R)*transpose(inverse((R)), Z = R*X.
//    There are three ways to handle non-zero process noise covariance.
// (1) If Q is the (known) a priori process noise covariance Q, then
// set Q=Rw(-1)*Rw(-T), and G=1.
// (2) Transform process noise covariance matrix to UDU form, Q=UDU,
// then set G=U  and Rw = (D)**-1/2.
// (3) Take the sqrt of process noise covariance matrix Q, then set
// G=this sqrt and Rw = 1.  [2 and 3 have been tested.]
//    The routine applies a Householder transformation to a large
// matrix formed by concatenation of the input matricies.  Two preliminary 
// steps are to form Rd = R*PhiInv (stored in PhiInv) and -Rd*G (stored in 
// G) by matrix multiplication, and to set Rwx to the zero matrix.  
// Then the Householder transformation is applied to the following
// matrix, dimensions are shown in ():
//       _  (ns)   (n)   (1)  _          _                  _
// (ns) |    Rw     0     Zw   |   ==>  |   Rw   Rwx   Zw    |
// (n)  |  -Rd*G   Rd     Z    |   ==>  |   0     R    Z     | .
//       -                    -          -                  -
// The SRI matricies R and Rw remain upper triangular.
//
//    For the programmer:  after Rwx is set to zero, G is made into 
// -Rd*G and PhiInv is made into R*PhiInv, the transformation is applied 
// to the matrix:
//       _   (ns)   (n)   (1) _
// (ns) |    Rw    Rwx    Zw   |
// (n)  |     G    PhiInv  Z   |
//       -                    -
// then the (upper triangular) matrix R is copied out of PhiInv into R.
// -------------------------------------------------------------------
//    The matrix Rwx is related to the sensitivity of the state
// estimate to the unmodeled parameters in Zw.  The sensitivity matrix
// is          Sen = -inverse(Rw)*Rwx,
// where perturbation in model X = 
//             Sen * diagonal(a priori sigmas of parameter uncertainties).
// -------------------------------------------------------------------
//    The quantities Rw, Rwx and Zw on output are to be saved and used
// in the sqrt information fixed interval smoother (SRIS), during the
// backward filter process.
// -------------------------------------------------------------------
// Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential
//      Estimation," Academic Press, 1977, pg 121.
// -------------------------------------------------------------------
template <class T>
void SRIFilter::SrifTU(Matrix<T>& R,
                       Vector<T>& Z,
                       Matrix<T>& PhiInv,
                       Matrix<T>& Rw,
                       Matrix<T>& G,
                       Vector<T>& Zw,
                       Matrix<T>& Rwx)
   throw(MatrixException)
{
   const T EPS=-T(1.e-200);
   unsigned int n=R.rows(),ns=Rw.rows();
   unsigned int i,j,k;
   T sum, beta, delta, dum;

   if(PhiInv.rows() < n || PhiInv.cols() < n ||
      G.rows() < n || G.cols() < ns ||
      R.cols() != n ||
      Rwx.rows() < ns || Rwx.cols() < n ||
      Z.size() < n || Zw.size() < ns) {
      MatrixException me("Invalid input dimensions:\n  R is "
         + asString<int>(R.rows()) + "x"
         + asString<int>(R.cols()) + ", Z has length "
         + asString<int>(Z.size()) + "\n  PhiInv is "
         + asString<int>(PhiInv.rows()) + "x"
         + asString<int>(PhiInv.cols()) + "\n  Rw is "
         + asString<int>(Rw.rows()) + "x"
         + asString<int>(Rw.cols()) + "\n  G is "
         + asString<int>(G.rows()) + "x"
         + asString<int>(G.cols()) + "\n  Zw has length "
         + asString<int>(Zw.size()) + "\n  Rwx is "
         + asString<int>(Rwx.rows()) + "x"
         + asString<int>(Rwx.cols())
         );
      GPSTK_THROW(me);
   }

   try {
      // initialize
      Rwx = T(0);
      PhiInv = R * PhiInv;                   // set PhiInv = Rd = R*PhiInv
      G = -PhiInv * G;
      // fixed Matrix problem - unary minus should not return an l-value
      //G = -(PhiInv * G);                     // set G = -Rd*G

      // temp
      //Matrix <T> A;
      //A = (Rw || Rwx || Zw) && (G || PhiInv || Z);
      //cout << "SrifTU - :\n" << fixed << setw(10) << setprecision(5) << A << endl;

      //---------------------------------------------------------------
      for(j=0; j<ns; j++) {               // loop over first ns columns
         sum = T(0);
         for(i=0; i<n; i++)               // rows of -Rd*G
            sum += G(i,j)*G(i,j);
         dum = Rw(j,j);
         sum += dum*dum;
         sum = (dum > T(0) ? -T(1) : T(1)) * ::sqrt(sum);
         delta = dum - sum;
         Rw(j,j) = sum;

         beta = sum * delta;
         if(beta > EPS) continue;
         beta = T(1)/beta;

            // apply jth Householder transformation
            // to submatrix below and right of (j,j)
         if(j+1 < ns) {                   // apply to G
            for(k=j+1; k<ns; k++) {       // columns to right of diagonal
               sum = delta * Rw(j,k);
               for(i=0; i<n; i++)         // rows of G
                  sum += G(i,j)*G(i,k);
               if(sum == T(0)) continue;
               sum *= beta;
               Rw(j,k) += sum*delta;
               for(i=0; i<n; i++)         // rows of G again
                  G(i,k) += sum * G(i,j);
            }
         }

            // apply jth Householder transformation
            // to Rwx and PhiInv
         for(k=0; k<n; k++) {             // columns of Rwx and PhiInv
            sum = delta * Rwx(j,k);
            for(i=0; i<n; i++)            // rows of PhiInv and G
               sum += PhiInv(i,k) * G(i,j);
            if(sum == T(0)) continue;
            sum *= beta;
            Rwx(j,k) += sum*delta;
            for(i=0; i<n; i++)            // rows of PhiInv and G
               PhiInv(i,k) += sum * G(i,j);
         }                                // end loop over columns of Rwx and PhiInv

            // apply jth Householder transformation
            // to Zw and Z
         sum = delta * Zw(j);
         for(i=0; i<n; i++)               // rows of G and elements of Z
            sum += Z(i) * G(i,j);
         if(sum == T(0)) continue;
         sum *= beta;
         Zw(j) += sum * delta;
         for(i=0; i<n; i++)               // rows of G and elements of Z
            Z(i) += sum * G(i,j);
      }                                   // end loop over first ns columns

      //---------------------------------------------------------------
      for(j=0; j<n; j++) {                // loop over columns of Rwx and PhiInv
         sum = T(0);
         for(i=j+1; i<n; i++)             // rows of PhiInv
            sum += PhiInv(i,j)*PhiInv(i,j);
         dum = PhiInv(j,j);
         sum += dum*dum;
         sum = (dum > T(0) ? -T(1) : T(1)) * ::sqrt(sum);
         delta = dum - sum;
         PhiInv(j,j) = sum;
         beta = sum*delta;
         if(beta > EPS) continue;
         beta = T(1)/beta;

            // apply jth Householder transformation to columns of PhiInv on row j
         for(k=j+1; k<n; k++) {           // columns of PhiInv
            sum = delta * PhiInv(j,k);
            for(i=j+1; i<n; i++)
               sum += PhiInv(i,j)*PhiInv(i,k);
            if(sum == T(0)) continue;
            sum *= beta;
            PhiInv(j,k) += sum*delta;
            for(i=j+1; i<n; i++)
               PhiInv(i,k) += sum * PhiInv(i,j);
         }

            // apply jth Householder transformation to Z
         sum = delta *Z(j);
         for(i=j+1; i<n; i++)
            sum += Z(i) * PhiInv(i,j);
         if(sum == T(0)) continue;
         sum *= beta;
         Z(j) += sum*delta;
         for(i=j+1; i<n; i++)
            Z(i) += sum * PhiInv(i,j);
      }                                   // end loop over cols of Rwx and PhiInv

      // temp
      //A = (Rw || Rwx || Zw) && (G || PhiInv || Z);
      //cout << "SrifTU + :\n" << fixed << setw(10) << setprecision(5) << A << endl;

         // copy transformed R out of PhiInv
      for(j=0; j<n; j++)
         for(i=0; i<=j; i++)
            R(i,j) = PhiInv(i,j);
   }
   catch(MatrixException& me) { GPSTK_RETHROW(me); }
}  // end SrifTU

//------------------------------------------------------------------------------------
// Kalman smoother update.
// This routine uses the Householder transformation to propagate the SRIF
// state and covariance through a smoother (backward filter) step.
// Input:
// R     A priori square root information (SRI) matrix (an N by N 
//          upper triangular matrix)
// z     a priori SRIF state vector, an N vector (state is x, z = R*x).
// Phi   State transition matrix, an N by N matrix. Phi is destroyed on output.
// Rw    A priori square root information matrix for the process
//          noise, an Ns by Ns upper triangular matrix (which has 
//          Ns(Ns+1)/2 elements).
// G     The N by Ns matrix associated with process noise.  The 
//          process noise covariance is GQGtrans where Qinverse 
//          is Rw(trans)*Rw. G is destroyed on output.
// Zw    A priori 'state' associated with the process noise,
//          a vector with Ns elements. Zw is destroyed on output.
// Rwx   An Ns by N matrix. Rwx is destroyed on output.
//
// The inputs Rw,Zw,Rwx are the output of the SRIF time update, and these and
// Phi and G are associated with the same timestep.
// 
// Output:
//    The updated square root information matrix and SRIF smoothed state (R,z).
// All other inputs are trashed.
// 
// Return values:
//    SrifSU returns void, but throws exceptions if the input matrices
// or vectors have incompatible dimensions or incorrect types.
// 
// Method:
//    The fixed interval square root information smoother (SRIS) is 
// composed of two Kalman filters, one identical with the square root 
// information filter (SRIF), the other similar but operating on the
// data in reverse order and combining the current (smoothed) state
// with elements output by the SRIF in its forward run and saved.
// Thus a smoother is composed of a forward filter which saves all of
// its output, followed by a backward filter which makes use of that
// saved information.
//    This form of the SRIF backward filter algorithm is equivalent to the
// Dyer-McReynolds SRIS algorithm, which uses less computer resources, but
// propagates the state and covariance rather than the SRI (R,z). (As always,
// at any point the state X and covariance P are related to the SRI by
// X = R^-1 * z , P = R^-1 * R^-T.)
//    For startup of the backward filter, the state after the final 
// measurement update of the SRIF is given another time update, the
// output of which is identified with the a priori values for the 
// backward filter.  Backward filtering proceeds from there, the N+1st
// point, toward the first point.
//
//    In this implementation of the backward filter, the Householder
// transformation is applied to the following matrix
// (dimensions are shown in ()):
// 
//       _  (Ns)     (N)      (1) _          _                  _
// (Ns) |  Rw+Rwx*G  Rwx*Phi  Zw   |   ==>  |   Rw   Rwx   Zw    |
// (N)  |  R*G       R*Phi    z    |   ==>  |   0     R    z     | .
//       -                        -          -                  -
// The SRI matricies R and Rw remain upper triangular.
//
//    For the programmer: First create an NsXNs matrix A, then
// Rw+Rwx*G -> A, Rwx*Phi -> Rwx, R*Phi -> Phi, and R*G -> G, and
// the transformation is applied to the matrix:
//
//       _ (Ns)   (N)  (1) _
// (Ns) |   A    Rwx   Zw   |
// (N)  |   G    Phi   z    |
//       -                 -
// then the (upper triangular) matrix R is copied out of Phi into R.
//
// Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential
//      Estimation," Academic Press, 1977, pg 216.
template <class T>
void SRIFilter::SrifSU(Matrix<T>& R,
                       Vector<T>& Z,
                       Matrix<T>& Phi,
                       Matrix<T>& Rw,
                       Matrix<T>& G,
                       Vector<T>& Zw,
                       Matrix<T>& Rwx)
   throw(MatrixException)
{
   unsigned int N=R.rows(),Ns=Rw.rows();

   if(Phi.rows() < N || Phi.cols() < N ||
      G.rows() < N || G.cols() < Ns ||
      R.cols() != N ||
      Rwx.rows() < Ns || Rwx.cols() < N ||
      Z.size() < N || Zw.size() < Ns) {
      MatrixException me("Invalid input dimensions:\n  R is "
         + asString<int>(R.rows()) + "x"
         + asString<int>(R.cols()) + ", Z has length "
         + asString<int>(Z.size()) + "\n  Phi is "
         + asString<int>(Phi.rows()) + "x"
         + asString<int>(Phi.cols()) + "\n  Rw is "
         + asString<int>(Rw.rows()) + "x"
         + asString<int>(Rw.cols()) + "\n  G is "
         + asString<int>(G.rows()) + "x"
         + asString<int>(G.cols()) + "\n  Zw has length "
         + asString<int>(Zw.size()) + "\n  Rwx is "
         + asString<int>(Rwx.rows()) + "x"
         + asString<int>(Rwx.cols())
         );
      GPSTK_THROW(me);
   }

   const T EPS=-T(1.e-200);
   size_t i, j, k;
   T sum, beta, delta, diag;

try {
      // Rw+Rwx*G -> A
   Matrix<T> A;
   A = Rw + Rwx*G;
   Rwx = Rwx * Phi;
   Phi = R * Phi;
   G = R * G;

         //-----------------------------------------
         // HouseHolder Transformation

         // Loop over first Ns columns
   for(j=0; j<Ns; j++) {                  // columns of A
      sum = T(0);
      for(i=j+1; i<Ns; i++) {             // rows i below diagonal in A
         sum += A(i,j) * A(i,j);
      }
      for(i=0; i<N; i++) {                // all rows i in G
         sum += G(i,j) * G(i,j);
      }

      diag = A(j,j);
      sum += diag*diag;
      sum = (diag > T(0) ? -T(1) : T(1)) * ::sqrt(sum);
      delta = diag - sum;
      A(j,j) = sum;
      beta = sum*delta;
      if(beta > EPS) continue;
      beta = T(1)/beta;

            // apply jth HH trans to submatrix below and right of j,j
      for(k=j+1; k<Ns; k++) {                // cols to right of diag
         sum = delta * A(j,k);
         for(i=j+1; i<Ns; i++) {             // rows of A below diagonal
            sum += A(i,j)*A(i,k);
         }
         for(i=0; i<N; i++) {                // all rows of G
            sum += G(i,j)*G(i,k);
         }
         if(sum == T(0)) continue;
         sum *= beta;
            //------------------------------------------
         A(j,k) += sum*delta;

         for(i=j+1; i<Ns; i++) {             // rows of A > j (same loops again)
            A(i,k) += sum * A(i,j);
         }
         for(i=0; i<N; i++) {                // all rows of G (again)
            G(i,k) += sum * G(i,j);
         }
      }

            // apply jth HH trans to Rwx and Phi sub-matrices
      for(k=0; k<N; k++) {                // all columns of Rwx / Phi
         sum = delta * Rwx(j,k);
         for(i=j+1; i<Ns; i++) {          // rows of Rwx below j
            sum += A(i,j) * Rwx(i,k);
         }
         for(i=0; i<N; i++) {             // all rows of Phi
            sum += G(i,j) * Phi(i,k);
         }
         if(sum == T(0)) continue;
         sum *= beta;
         Rwx(j,k) += sum*delta;
         for(i=j+1; i<Ns; i++) {          // rows of Rwx below j (again)
            Rwx(i,k) += sum * A(i,j);
         }
         for(i=0; i<N; i++) {             // all rows of Phi (again)
            Phi(i,k) += sum * G(i,j);
         }
      }

            // apply jth HH trans to Zw and Z
      sum = delta * Zw(j);
      for(i=j+1; i<Ns; i++) {             // rows (elements) of Zw below j
         sum += A(i,j) * Zw(i);
      }
      for(i=0; i<N; i++) {                // all rows (elements) of Z
         sum += Z(i) * G(i,j);
      }
      if(sum == T(0)) continue;
      sum *= beta;
      Zw(j) += sum*delta;
      for(i=j+1; i<Ns; i++) {             // rows of Zw below j (again)
         Zw(i) += sum * A(i,j);
      }
      for(i=0; i<N; i++) {                // all rows of Z (again)
         Z(i) += sum * G(i,j);
      }
   }

         // Loop over columns past the Ns block: all of Rwx and Phi
   for(j=0; j<N; j++) {
      sum = T(0);
      for(i=j+1; i<N; i++) {              // rows of Phi at and below j
         sum += Phi(i,j) * Phi(i,j);
      }
      diag = Phi(j,j);
      sum += diag*diag;
      sum = (diag > T(0) ? -T(1) : T(1)) * ::sqrt(sum);
      delta = diag - sum;
      Phi(j,j) = sum;
      beta = sum*delta;
      if(beta > EPS) continue;
      beta = T(1)/beta;

               // apply HH trans to Phi sub-block below and right of j,j
      for(k=j+1; k<N; k++) {                 // columns k > j
         sum = delta * Phi(j,k);
         for(i=j+1; i<N; i++) {              // rows below j
            sum += Phi(i,j) * Phi(i,k);
         }
         if(sum == T(0)) continue;
         sum *= beta;
         Phi(j,k) += sum*delta;
         for(i=j+1; i<N; i++) {              // rows below j (again)
            Phi(i,k) += sum * Phi(i,j);
         }
      }
               // Now apply to the Z column
      sum = delta * Z(j);
      for(i=j+1; i<N; i++) {                 // rows of Z below j
         sum += Z(i) * Phi(i,j);
      }
      if(sum == T(0)) continue;
      sum *= beta;
      Z(j) += sum*delta;
      for(i=j+1; i<N; i++) {                 // rows of Z below j (again)
         Z(i) += sum * Phi(i,j);
      }
   }
      //------------------------------
      // Transformation finished

      //-------------------------------------
      // copy transformed R out of Phi into R
   R = T(0);
   for(j=0; j<N; j++) {
      for(i=0; i<=j; i++) {
         R(i,j) = Phi(i,j);
      }
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end SrifSU

//------------------------------------------------------------------------------
// Covariance/State version of the Kalman smoother update (Dyer-McReynolds).
// This routine implements the Dyer-McReynolds form of the state and covariance
// recursions which constitute the backward filter of the Square Root
// Information Smoother.
// 
// Input: (assume N and Ns are greater than zero)
//		Vector X(N)				A priori state, derived from SRI (R*X=Z)
// 	Matrix P(N,N)			A priori covariance, derived from SRI (P=R^-1*R^-T)
// 	Matrix Rw(Ns,Ns)		Process noise covariance (UT), output of SRIF TU
// 	Matrix Rwx(Ns,N)		PN 'cross term', output of SRIF TU
// 	Vector Zw(Ns)			Process noise state, output of SRIF TU
// 	Matrix Phinv(N,N)		Inverse of state transition, saved at SRIF TU
// 	Matrix G(N,Ns)			Noise coupling matrix, saved at SRIF TU
// Output:
// 	Updated X and P. The other inputs are trashed.
// 
// Method:
// 	The fixed interval square root information smoother (SRIS) is 
// composed of two Kalman filters, one identical with the square root 
// information filter (SRIF), the other similar but operating on the
// data in reverse order and combining the current (smoothed) state
// with elements output by the SRIF in its forward run and saved.
// Thus a smoother is composed of a forward filter which saves all of
// its output, followed by a backward filter which makes use of that
// saved information.
// 	This form of the SRIS algorithm is equivalent to the SRIS backward
// filter Householder transformation algorithm, but uses less computer
// resources. It is not necessary to update both the state and the
// covariance, although doing both at once is less expensive than
// doing them separately. (This routine does both.)
// 	For startup of the backward filter, the state after the final 
// measurement update of the SRIF is given another time update, the
// output of which is identified with the a priori values for the 
// backward filter.  Backward filtering proceeds from there, the N+1st
// point, toward the first point.
//
// Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential
//      Estimation," Academic Press, 1977, pg 216.
template <class T>
void SRIFilter::SrifSU_DM(Matrix<T>& P,
                          Vector<T>& X,
                          Matrix<T>& Phinv,
                          Matrix<T>& Rw,
                          Matrix<T>& G,
                          Vector<T>& Zw,
                          Matrix<T>& Rwx)
   throw(MatrixException)
{
   unsigned int N=P.rows(),Ns=Rw.rows();

   if(P.cols() != P.rows() ||
      X.size() != N ||
      Rwx.cols() != N ||
      Zw.size() != Ns ||
      Rwx.rows() != Ns || Rwx.cols() != N ||
      Phinv.rows() != N || Phinv.cols() != N ||
      G.rows() != N || G.cols() != Ns ) {
      MatrixException me("Invalid input dimensions:\n  P is "
         + asString<int>(P.rows()) + "x"
         + asString<int>(P.cols()) + ", X has length "
         + asString<int>(X.size()) + "\n  Phinv is "
         + asString<int>(Phinv.rows()) + "x"
         + asString<int>(Phinv.cols()) + "\n  Rw is "
         + asString<int>(Rw.rows()) + "x"
         + asString<int>(Rw.cols()) + "\n  G is "
         + asString<int>(G.rows()) + "x"
         + asString<int>(G.cols()) + "\n  Zw has length "
         + asString<int>(Zw.size()) + "\n  Rwx is "
         + asString<int>(Rwx.rows()) + "x"
         + asString<int>(Rwx.cols())
         );
      GPSTK_THROW(me);
   }

try {
   G = G * inverseLUD(Rw);
   Matrix<T> F;
   F = ident<T>(N) + G*Rwx;
   // update X
   Vector<T> C;
   C = F*X - G*Zw;
   X = Phinv * C;
   // update P
   P = F*P*transpose(F) + G*transpose(G);
   P = Phinv*P*transpose(Phinv);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
} // end SrifSU_DM

// Modification for case with control vector: Xj+1 = Phi*Xj + Gwj + u
template <class T>
void DMsmootherUpdateWithControl(Matrix<double>& P,
                                 Vector<double>& X,
                                 Matrix<double>& Phinv,
                                 Matrix<double>& Rw,
                                 Matrix<double>& G,
                                 Vector<double>& Zw,
                                 Matrix<double>& Rwx,
                                 Vector<double>& U)
      throw(MatrixException)
{
   unsigned int N=P.rows(),Ns=Rw.rows();

   if(P.cols() != P.rows() ||
      X.size() != N ||
      Rwx.cols() != N ||
      Zw.size() != Ns ||
      Rwx.rows() != Ns || Rwx.cols() != N ||
      Phinv.rows() != N || Phinv.cols() != N ||
      G.rows() != N || G.cols() != Ns ||
      U.size() != N) {
      MatrixException me("Invalid input dimensions:\n  P is "
         + asString<int>(P.rows()) + "x"
         + asString<int>(P.cols()) + ", X has length "
         + asString<int>(X.size()) + "\n  Phinv is "
         + asString<int>(Phinv.rows()) + "x"
         + asString<int>(Phinv.cols()) + "\n  Rw is "
         + asString<int>(Rw.rows()) + "x"
         + asString<int>(Rw.cols()) + "\n  G is "
         + asString<int>(G.rows()) + "x"
         + asString<int>(G.cols()) + "\n  Zw has length "
         + asString<int>(Zw.size()) + "\n  Rwx is "
         + asString<int>(Rwx.rows()) + "x"
         + asString<int>(Rwx.cols()) + "\n  U has length "
         + asString<int>(U.size())
         );
      GPSTK_THROW(me);
   }

try {
   G = G * inverseLUD(Rw);
   Matrix<T> F;
   F = ident<T>(N) + G*Rwx;
   // update X
   Vector<T> C;
   C = F*X - G*Zw - U;
   X = Phinv * C;
   // update P
   P = F*P*transpose(F) + G*transpose(G);
   P = Phinv*P*transpose(Phinv);
   P += outer(U,U);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
} // end DMsmootherUpdateWithControl

//------------------------------------------------------------------------------------
} // end namespace gpstk
