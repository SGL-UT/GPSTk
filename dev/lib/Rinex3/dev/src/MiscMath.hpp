#pragma ident "$Id: MiscMath.hpp 281 2006-11-07 04:26:04Z ocibu $"



/**
 * @file MiscMath.hpp
 * Miscellaneous mathematical algorithms
 */
 
#ifndef GPSTK_MISC_MATH_HPP
#define GPSTK_MISC_MATH_HPP

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






#include <vector>
#include "MathBase.hpp"

namespace gpstk
{
   /** @defgroup math Mathematical algorithms */
   //@{

   /** Perform Lagrange interpolation on the data (X[i],Y[i]), i=1,N (N=X.size()),
    * returning the value of Y(x). Also return an estimate of the estimation error in 'err'.
    * Assumes k=X.size() is even, and that x is between X[j-1] and X[j], where j=k/2.
    */
   template <class T>
   T LagrangeInterpolation(const std::vector<T>& X, const std::vector<T>& Y, const T& x, T& err)
   {
      size_t i,j,k;
      T y,del;
      std::vector<T> D,Q;

      err = T(0);
      k = X.size()/2;
      if(x == X[k]) return Y[k];
      if(x == X[k-1]) return Y[k-1];
      if(ABS(x-X[k-1]) < ABS(x-X[k])) k=k-1;
      for(i=0; i<X.size(); i++) {
         Q.push_back(Y[i]);
         D.push_back(Y[i]);
      }
      y = Y[k--];
      for(j=1; j<X.size(); j++) {
         for(i=0; i<X.size()-j; i++) {
            del = (Q[i+1]-D[i])/(X[i]-X[i+j]);
            D[i] = (X[i+j]-x)*del;
            Q[i] = (X[i]-x)*del;
         }
         err = (2*k < X.size()-j ? Q[k+1] : D[k--]);
         y += err;
      }
      return y;
   }  // end T LagrangeInterpolation(vector, vector, const T, T&)

   // The following is a
   // Straightforward implementation of Lagrange polynomial and its derivative
   // { all sums are over index=0,N-1; Xi is short for X[i]; Lp is dL/dx;
   //   y(x) is the function being approximated. }
   // y(x) = SUM[Li(x)*Yi]
   // Li(x) = PROD(j!=i)[x-Xj] / PROD(j!=i)[Xi-Xj]
   // dy(x)/dx = SUM[Lpi(x)*Yi]
   // Lpi(x) = SUM(k!=i){PROD(j!=i,j!=k)[x-Xj]} / PROD(j!=i)[Xi-Xj]
   // Define Pi = PROD(j!=i)[x-Xj], Di = PROD(j!=i)[Xi-Xj],
   // Qij = PROD(k!=i,k!=j)[x-Xk] and Si = SUM(j!=i)Qij.
   // then Li(x) = Pi/Di, and Lpi(x) = Si/Di.
   // Qij is symmetric, there are only N(N+1)/2 - N of them, so store them
   // in a vector of length N(N+1)/2, where Qij==Q[i+j*(j+1)/2] (ignore i=j).

   /** Perform Lagrange interpolation on the data (X[i],Y[i]), i=1,N (N=X.size()),
    * returning the value of Y(x) and dY(x)/dX.
    * Assumes that x is between X[k-1] and X[k], where k=N/2.
    * Warning: for use with the precise (SP3) ephemeris only when velocity is not
    * available; estimates of velocity, and especially clock drift, not as accurate.
    */
   template <class T>
   void LagrangeInterpolation(const std::vector<T>& X, const std::vector<T>& Y, const T& x, T& y, T& dydx)
   {
      size_t i,j,k,N=X.size(),M;
      M = (N*(N+1))/2;
      std::vector<T> P(N,T(1)),Q(M,T(1)),D(N,T(1));
      for(i=0; i<N; i++) {
         for(j=0; j<N; j++) {
            if(i != j) {
               P[i] *= x-X[j];
               D[i] *= X[i]-X[j];
               if(i < j) {
//std::cout << "Compute Q[" << i << "," << j << "=" << (i+(j*(j+1))/2) << "] = 1 ";
                  for(k=0; k<N; k++) {
                     if(k == i || k == j) continue;
//std::cout << " * (x-X[" << k << "])";
                     Q[i+(j*(j+1))/2] *= (x-X[k]);
                  }
//std::cout << " = " << Q[i+(j*(j+1))/2] << std::endl;
               }
            }
         }
      }
      y = dydx = T(0);
      for(i=0; i<N; i++) {
         y += Y[i]*(P[i]/D[i]);
         T S(0);
         for(k=0; k<N; k++) if(i != k) {
            if(k<i) S += Q[k+(i*(i+1))/2]/D[i];
            else    S += Q[i+(k*(k+1))/2]/D[i];
         }
         dydx += Y[i]*S;
      }
   }  // end void LagrangeInterpolation(vector, vector, const T, T&, T&)

   /// Perform the root sum square of aa, bb and cc
   template <class T>
   T RSS (T aa, T bb, T cc)
   {
      T a(ABS(aa)), b(ABS(bb)), c(ABS(cc));
      if ( (a > b) && (a > c) )
         return a * SQRT(1 + (b/a)*(b/a) + (c/a)*(c/a));
      if ( (b > a) && (b > c) )
         return b * SQRT(1 + (a/b)*(a/b) + (c/b)*(c/b));
      if ( (c > b) && (c > a) )
         return c * SQRT(1 + (b/c)*(b/c) + (a/c)*(a/c));

      if (a == b)
      {
         if (b == c)
            return a * SQRT(T(3));
         a *= SQRT(T(2));
         if (a > c)
            return a * SQRT(1 + (c/a)*(c/a));
         else
            return c * SQRT(1 + (a/c)*(a/c));
      }
      if (a == c)
      {
         a *= SQRT(T(2));
         if (a > b)
            return a * SQRT(1 + (b/a)*(b/a));
         else
            return b * SQRT(1 + (a/b)*(a/b));
      }
      if (b == c)
      {
         b *= SQRT(T(2));
         if (b > a)
            return b * SQRT(1 + (a/b)*(a/b));
         else
            return a * SQRT(1 + (b/a)*(b/a));
      }

      return T(0);
   }

   /// Perform the root sum square of aa, bb
   template <class T>
   T RSS (T aa, T bb)
   {
      return RSS(aa,bb,T(0));
   }

 
   /// Perform the root sum square of aa, bb, cc and dd
   template <class T>
   T RSS (T aa, T bb, T cc, T dd)
   {
#define swapValues(x,y) \
   { T temporalStorage; \
   temporalStorage = x; x = y; y = temporalStorage; }

      T a(ABS(aa)), b(ABS(bb)), c(ABS(cc)), d(ABS(dd));

      // For numerical reason, let's just put the biggest in "a" (we are not sorting)
      if (a < b) std::swap(a,b);
      if (a < c) swapValues(a,c);
      if (a < d) swapValues(a,d);

      return a * SQRT(1 + (b/a)*(b/a) + (c/a)*(c/a) + (d/a)*(d/a));
   }

   //@}

}  // namespace gpstk

#endif
