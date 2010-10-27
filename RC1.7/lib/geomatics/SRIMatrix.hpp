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
 * @file SRIMatrix.hpp
 * Template routines for efficient manipulation of square root matricies,
 * used for least squares estimation and the SRI form of the Kalman filter.
 */

//------------------------------------------------------------------------------------
#ifndef SQUAREROOTINFORMATION_MATRICIES_INCLUDE
#define SQUAREROOTINFORMATION_MATRICIES_INCLUDE

//------------------------------------------------------------------------------------
// system includes
// GPSTk
#include "Vector.hpp"
#include "Matrix.hpp"
#include "StringUtils.hpp"
// geomatics

namespace gpstk
{

   //---------------------------------------------------------------------------------
   // This routine uses the Householder algorithm to update the SRI
   // state and covariance.
   // Input:
   //    R  a priori SRI matrix (upper triangular, dimension N)
   //    Z  a priori SRI data vector (length N)
   //    A  concatentation of H and D : A = H || D, where
   //    H  Measurement partials, an M by N matrix.
   //    D  Data vector, of length M
   //       H and D may have row dimension > M; then pass M:
   //    M  (optional) Row dimension of H and D
   // Output:
   //    Updated R and Z.  H is trashed, but the data vector D
   //    contains the residuals of fit (D - A*state).
   // Return values:
   //    SrifMU returns void, but throws exceptions if the input matrices
   // or vectors have incompatible dimensions.
   // 
   // Measurment noise associated with H and D must be white
   // with unit covariance.  If necessary, the data can be 'whitened'
   // before calling this routine in order to satisfy this requirement.
   // This is done as follows.  Compute the lower triangular square root 
   // of the covariance matrix, L, and replace H with inverse(L)*H and
   // D with inverse(L)*D.
   // 
   //    The Householder transformation is simply an orthogonal
   // transformation designed to make the elements below the diagonal
   // zero.  It works by explicitly performing the transformation, one
   // column at a time, without actually constructing the transformation
   // matrix. The matrix is transformed as follows
   //   [  A(m,n) ] => [ sum       a       ]
   //   [         ] => [  0    A'(m-1,n-1) ]
   // after which the same transformation is applied to A' matrix, until A'
   // has only one row or column. The transformation that zeros the diagonal
   // below the (k,k) element also replaces the (k,k) element and modifies
   // the matrix elements for columns >= k and rows >=k, but does not affect
   // the matrix for columns < k or rows < k.
   //    Column k (=0..min(m,n)-1) of the input matrix A(m,n) can be zeroed
   // below the diagonal (columns < k have already been so zeroed) as follows:
   //    let y be the vector equal to column k at the diagonal and below,
   //       ( so y(j)==A(k+j,k), y(0)==A(k,k), y.size = m-k )
   //    let sum = -sign(y(0))*|y|,
   //    define vector u by u(0) = y(0)-sum, u(j)=y(j) for j>0 (j=1..m-k)
   //    and finally define b = 1/(sum*u(0)).
   // Redefine column k with A(k,k)=sum and A(k+j,k)=0, j=1..m, and
   // then for each column j > k, (j=k+1..n)
   //    compute g = b*sum[u(i)*A(k+i,j)], i=0..m-k-1,
   //    replace A(k+i,j) with A(k+i,j)+g*u(i), for i=0..m-k-1
   // Most algorithms don't handle special cases:
   // 1. If column k is already zero below the diagonal, but A(k,k)!=0, then
   // y=[A(k,k),0,0,...0], sum=-A(k,k), u(0)=2A(k,k), u=[2A(k,k),0,0,...0]
   // and b = -1/(2*A(k,k)^2). Then, zeroing column k only changes the sign
   // of A(k,k), and for the other columns j>k, g = -A(k,j)/A(k,k) and only
   // row k is changed.
   // 2. If column k is already zero below the diagonal, AND A(k,k) is zero,
   // then y=0,sum=0,u=0 and b is infinite...the transformation is undefined.
   // However this column should be skipped (Biermann Appendix VII.B).
   //
   // Ref: Bierman, G.J. "Factorization Methods for Discrete Sequential
   //      Estimation," Academic Press, 1977.
   
   /// Square root information measurement update, with new data in the form of a
   /// single matrix concatenation of H and D: A = H || D.
   /// See doc for the overloaded SrifMU().
   template <class T>
   void SrifMU(Matrix<T>& R, Vector<T>& Z, Matrix<T>& A, unsigned int M=0)
      throw(MatrixException)
   {
      if(A.cols() <= 1 || A.cols() != R.cols()+1 || Z.size() < R.rows()) {
         if(A.cols() > 1 && R.rows() == 0 && Z.size() == 0) {
            // create R and Z
            R = Matrix<double>(A.cols()-1,A.cols()-1,0.0);
            Z = Vector<double>(A.cols()-1,0.0);
         }
         else {
            MatrixException me("Invalid input dimensions:\n  R has dimension "
               + StringUtils::asString<int>(R.rows()) + "x"
               + StringUtils::asString<int>(R.cols()) + ",\n  Z has length "
               + StringUtils::asString<int>(Z.size()) + ",\n  and A has dimension "
               + StringUtils::asString<int>(A.rows()) + "x"
               + StringUtils::asString<int>(A.cols()));
            GPSTK_THROW(me);
         }
      }
   
      const T EPS=-T(1.e-200);
      unsigned int m=M, n=R.rows();
      if(m==0 || m > A.rows()) m=A.rows();
      unsigned int np1=n+1;         // if np1 = n, state vector Z is not updated
      unsigned int i,j,k;
      T dum, delta, beta;
   
      for(j=0; j<n; j++) {          // loop over columns
         T sum = T(0);
         for(i=0; i<m; i++)
            sum += A(i,j)*A(i,j);   // sum squares of elements in this column below d
         if(sum <= T(0)) continue;
   
         dum = R(j,j);
         sum += dum * dum;          // add diagonal element
         sum = (dum > T(0) ? -T(1) : T(1)) * ::sqrt(sum);
         delta = dum - sum;
         R(j,j) = sum;
   
         if(j+1 > np1) break;
   
         beta = sum*delta;          // beta must be negative
         if(beta > EPS) continue;
         beta = T(1)/beta;
   
         for(k=j+1; k<np1; k++) {   // columns to right of diagonal
            sum = delta * (k==n ? Z(j) : R(j,k));
            for(i=0; i<m; i++)
               sum += A(i,j) * A(i,k);
            if(sum == T(0)) continue;
   
            sum *= beta;
            if(k==n) Z(j) += sum*delta;
            else   R(j,k) += sum*delta;
   
            for(i=0; i<m; i++)
               A(i,k) += sum * A(i,j);
         }
      }
   }  // end SrifMU
    

   //---------------------------------------------------------------------------------
   // This is simply SrifMU(R,Z,A) with H and D passed in rather
   // than concatenated into a single Matrix A = H || D.
   
   /// Square root information filter (Srif) measurement update (MU).
   /// Use the Householder transformation to combine the information stored in the
   /// square root information (SRI) covariance matrix R and state Z with new
   /// information in the given partials matrix and data vector to produce an updated
   /// SRI {R,Z}. Measurement noise associated with the new information (H and D) is
   /// assumed to be white with unit covariance. If necessary, the data may be
   /// 'whitened' by multiplying H and D by the inverse of the lower triangular
   /// square root of the covariance matrix; that is,
   /// compute L = Cholesky(Measurement covariance) and let H = L*H, D = L*D.
   /// @param  R  Upper triangluar apriori SRI covariance matrix of dimension N
   /// @param  Z  A priori SRI state vector of length N
   /// @param  H  Partials matrix of dimension MxN, unchanged on output.
   /// @param  D  Data vector of length M; on output contains the residuals of fit.
   /// @param  M  If H and D have dimension M' > M, then call with M = data length;
   ///             otherwise M = 0 (the default) and is ignored.
   /// @throw MatrixException if the input has inconsistent dimensions.
   template <class T>
   void SrifMU(Matrix<T>& R,
               Vector<T>& Z,
               const Matrix<T>& H,
               Vector<T>& D,
               unsigned int M=0)
      throw(MatrixException)
   {
      Matrix<double> A;
      try { A = H || D; }
      catch(MatrixException& me) { GPSTK_RETHROW(me); }
   
      SrifMU(R,Z,A,M);
   
         // copy residuals out of A into D
      D = Vector<double>(A.colCopy(A.cols()-1));
   }
   

   //---------------------------------------------------------------------------------
   // Compute Cholesky decomposition of symmetric positive definite matrix using Crout
   // algorithm. A = L*L^T where A and L are (nxn) and L is lower triangular reads:
   // [ A00 A01 A02 ... A0n ] = [ L00  0   0  0 ...  0 ][ L00 L10 L20 ... L0n ]
   // [ A10 A11 A12 ... A1n ] = [ L10 L11  0  0 ...  0 ][  0  L11 L21 ... L1n ]
   // [ A20 A21 A22 ... A2n ] = [ L20 L21 L22 0 ...  0 ][  0   0  L22 ... L2n ]
   //           ...                      ...                  ...
   // [ An0 An1 An2 ... Ann ] = [ Ln0 Ln1 Ln2 0 ... Lnn][  0   0   0  ... Lnn ]
   //   but multiplying out gives
   //          A              = [ L00^2
   //                           [ L00*L10  L11^2+L10^2
   //                           [ L00*L20  L11*L21+L10*L20 L22^2+L21^2+L20^2
   //                                 ...
   //    Aii = Lii^2 + sum(k=0,i-1) Lik^2
   //    Aij = Lij*Ljj + sum(k=0,j-1) Lik*Ljk
   // These can be inverted by looping over columns, and filling L from diagonal down.
   
   /// Compute lower triangular square root of a symmetric positive definite matrix
   /// (Cholesky decomposition) Crout algorithm.
   /// @param A Matrix to be decomposed; symmetric and positive definite, unchanged
   /// @return Matrix lower triangular square root of input matrix
   /// @throw MatrixException if input Matrix is not square
   /// @throw MatrixException if input Matrix is not positive definite
   template <class T>
   Matrix<T> lowerCholesky(const Matrix<T>& A) throw(MatrixException)
   {
      if(A.rows() != A.cols() || A.rows() == 0) {
         MatrixException me("Invalid input dimensions: "
               + StringUtils::asString<int>(A.rows()) + "x"
               + StringUtils::asString<int>(A.cols()));
         GPSTK_THROW(me);
      }
   
      const unsigned int n=A.rows();
      unsigned int i,j,k;
      Matrix<T> L(n,n,0.0);
   
      for(j=0; j<n; j++) {             // loop over cols
         T d(A(j,j));
         for(k=0; k<j; k++) d -= L(j,k)*L(j,k);
         if(d <= 0.0) {
            MatrixException e("Non-positive eigenvalue; "
               "Cholesky requires positive-definite input");
            GPSTK_THROW(e);
         }
         L(j,j) = ::sqrt(d);
         for(i=j+1; i<n; i++) {        // loop over rows below diagonal
            d = A(i,j);
            for(k=0; k<j; k++) d -= L(i,k)*L(j,k);
            L(i,j) = d/L(j,j);
         }
      }
   
      return L;
   }


   //---------------------------------------------------------------------------------
   /// Compute upper triangular square root of a symmetric positive definite matrix
   /// (Cholesky decomposition) Crout algorithm; that is A = transpose(U)*U.
   /// Note that this result will be equal to
   /// transpose(lowerCholesky(A)) == transpose(Ch.L from class Cholesky), NOT Ch.U;
   /// class Cholesky computes L,U where A = L*LT = U*UT [while A=UT*U here].
   /// @param A   Matrix to be decomposed; symmetric and positive definite, unchanged
   /// @return Matrix upper triangular square root of input matrix
   /// @throw MatrixException if input Matrix is not square
   /// @throw MatrixException if input Matrix is not positive definite
   template <class T>
   Matrix<T> upperCholesky(const Matrix<T>& A) throw(MatrixException)
      { return transpose(lowerCholesky(A)); }


   //---------------------------------------------------------------------------------
   /// Compute inverse of a symmetric positive definite matrix using Cholesky
   /// decomposition.
   /// @param A   Matrix to be inverted; symmetric and positive definite, unchanged
   /// @return Matrix inverse of input matrix
   /// @throw MatrixException if input Matrix is not square
   /// @throw SingularMatrixException if input Matrix is singular
   template <class T>
   Matrix<T> inverseCholesky(const Matrix<T>& A) throw(MatrixException)
   {
      try {
         Matrix<T> L(lowerCholesky(A));
         Matrix<T> Uinv(inverseUT(transpose(L)));
         Matrix<T> Ainv(UTtimesTranspose(Uinv));
         return Ainv;
      }
      catch(MatrixException& me) {
         me.addText("Called by inverseCholesky()");
         GPSTK_RETHROW(me);
      }
   }


   //---------------------------------------------------------------------------------
   // Invert the upper triangular matrix stored in the square matrix UT, using a very
   // efficient algorithm. Throw MatrixException if the matrix is singular.
   // If the pointers are defined, on exit (but not if an exception is thrown),
   // they return the smallest and largest eigenvalues of the matrix.
   
   /// Compute inverse of upper triangular matrix, returning smallest and largest
   /// eigenvalues.
   /// @param UT upper triangular matrix to be inverted
   /// @param ptrS pointer to <T> small, on output *ptrS contains smallest eigenvalue.
   /// @param ptrB pointer to <T> small, on output *ptrB contains largest eigenvalue.
   /// @return inverse of input matrix.
   /// @throw MatrixException if input is not square (assumed upper triangular also).
   /// @throw SingularMatrixException if input is singular.
   template <class T>
   Matrix<T> inverseUT(const Matrix<T>& UT, T *ptrSmall=NULL, T *ptrBig=NULL)
      throw(MatrixException)
   {
      if(UT.rows() != UT.cols() || UT.rows() == 0) {
         MatrixException me("Invalid input dimensions: "
               + StringUtils::asString<int>(UT.rows()) + "x"
               + StringUtils::asString<int>(UT.cols()));
         GPSTK_THROW(me);
      }
   
      unsigned int i,j,k,n=UT.rows();
      T big(0),small(0),sum,dum;
      Matrix<T> Inv(UT);
   
         // start at the last row,col
      dum = UT(n-1,n-1);
      if(dum == T(0)) {
         SingularMatrixException e("Singular matrix");
         GPSTK_THROW(e);
      }
   
      big = small = fabs(dum);
      Inv(n-1,n-1) = T(1)/dum;
      if(n == 1) return Inv;                 // 1x1 matrix
      for(i=0; i<n-1; i++) Inv(n-1,i)=0;
   
         // now move to rows i = n-2 to 0
      for(i=n-2; i>=0; i--) {
         if(UT(i,i) == T(0))
            throw SingularMatrixException("Singular matrix");
   
         if(fabs(UT(i,i)) > big) big = fabs(UT(i,i));
         if(fabs(UT(i,i)) < small) small = fabs(UT(i,i));
         dum = T(1)/UT(i,i);
         Inv(i,i) = dum;                        // diagonal element first
   
            // now do off-diagonal elements (i,i+1) to (i,n-1)
         for(j=i+1; j<n; j++) {
            sum = T(0);
            for(k=i+1; k<=j; k++)
               sum += Inv(k,j) * UT(i,k);
            Inv(i,j) = - sum * dum;
         }
         for(j=0; j<i; j++) Inv(i,j)=0;
   
         if(i==0) break;         // NB i is unsigned, hence 0-1 = 4294967295!
      }
   
      if(ptrSmall) *ptrSmall=small;
      if(ptrBig) *ptrBig=big;
   
      return Inv;
   }
   

   //---------------------------------------------------------------------------------
   // Given an upper triangular matrix UT, compute the symmetric matrix
   // UT * transpose(UT) using a very efficient algorithm.
   /// Compute the product of an upper triangular matrix and its transpose.
   /// @param UT upper triangular matrix
   /// @return product UT * transpose(UT)
   /// @throw MatrixException if input is not square (assumed upper triangular also).
   template <class T>
   Matrix<T> UTtimesTranspose(const Matrix<T>& UT)
      throw(MatrixException)
   {
      const unsigned int n=UT.rows();
      if(n == 0 || UT.cols() != n) {
         MatrixException me("Invalid input dimensions: "
               + StringUtils::asString<int>(UT.rows()) + "x"
               + StringUtils::asString<int>(UT.cols()));
         GPSTK_THROW(me);
      }
   
      unsigned int i,j,k;
      T sum;
      Matrix<T> S(n,n);
   
      for(i=0; i<n-1; i++) {        // loop over rows of UT, except the last
         sum = T(0);                // diagonal element (i,i)
         for(j=i; j<n; j++)
            sum += UT(i,j)*UT(i,j);
         S(i,i) = sum;
         for(j=i+1; j<n; j++) {     // loop over columns to right of (i,i)
            sum = T(0);
            for(k=j; k<n; k++)
               sum += UT(i,k) * UT(j,k);
            S(i,j) = S(j,i) = sum;
         }
      }
      S(n-1,n-1) = UT(n-1,n-1)*UT(n-1,n-1);   // the last diagonal element
   
      return S;
   }
   
   
} // end namespace gpstk
   
#endif // SQUAREROOTINFORMATION_MATRICIES_INCLUDE
