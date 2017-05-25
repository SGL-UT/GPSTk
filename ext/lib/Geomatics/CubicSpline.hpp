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

/// @file CubicSpline.hpp
/// Cubic spline interpolation on an array of data (x,y) not necessarily evenly spaced
/// but with x strictly increasing.
 
#ifndef INCLUDE_CUBIC_SPLINE_HPP
#define INCLUDE_CUBIC_SPLINE_HPP

#include <vector>
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "logstream.hpp"

namespace gpstk
{
   /** @addtogroup math */
   //@{
 
   /// Cubic spline interpolation.
   template <class T> class CubicSpline
   {
   public:
      /// Empty constructor - NB must call Initialize() before Eval().
      CubicSpline() : N(0) { }

      /// Constructor given vectors of data X(N) Y(N); calls Initialize(X,Y).
      CubicSpline(const std::vector<T>& X, const std::vector<T>& Y) throw(Exception)
         { Initialize(X,Y); }

      /// Initialize array of second derivatives, which is used by Interpolate().
      /// Called by constructor. If the arrays are shorter than 4 points, then linear
      /// interpolation is used. Do not fix derivatives at end points; if this is
      /// desired, call Initialize(X,Y,deriv1,derivN).
      /// @param X  Vector of data for independent variable, must parallel Y
      /// @param Y  Vector of data for dependent variable, must parallel X
      void Initialize(const std::vector<T>& X, const std::vector<T>& Y)
         throw(Exception) { build(X,Y,false); }

      /// Initialize array of second derivatives with the values of the derivative
      /// at the first and last points set by input.  Cf. Initialize(X,Y);
      /// @param X  Vector of data for independent variable, must parallel Y
      /// @param Y  Vector of data for dependent variable, must parallel X
      /// @param DYDX1 Initial value of derivative dy/dx
      /// @param DYDXN Final value of derivative dy/dx
      void Initialize(const std::vector<T>& X, const std::vector<T>& Y,
                                                const T DYDX1, const T DYDXN)
         throw(Exception)
      {
         fd1 = DYDX1;
         fdN = DYDXN;
         build(X,Y,true);
      }

      /// Determine if the given value of x is within the range of the data used to
      /// initialize the object; if it is not, return the value of Y which is closest
      /// to x (either Y[0] or Y[N-1]).
      /// @param x  value of independent variable X at which to test limits
      /// @param y  return value of data Y at endpoint closest to x
      /// @return true if x lies within the range of the data.
      /// @throw if the object has not been initialized by a call to Initialize().
      bool testLimits(const T& x, T& y) throw(Exception)
      {
         if(N == 0) {
            Exception e("Must call Initialize() first");
            GPSTK_THROW(e);
         }
         if(x <= X[0]) { y = Y[0]; return false; }
         if(x >= X[N-1]) { y = Y[N-1]; return false; }
         return true;
      }

      /// Compute the value of y at the given x, using interpolation by cubic spline.
      /// The object must have been initialized before this call, either by using
      /// the constructor CubicSpline(X,Y) or by calling Initialize() with the data
      /// arrays X and Y.
      /// @param x  value of independent variable X at which to evaluate spline.
      /// @return y(x) the interpolated value
      /// @throw if the object has not been initialized by a call to Initialize(), or
      ///    if the given x is outside the range of the data used in Initialize().
      T Evaluate(const T& x) throw(Exception)
      {
         if(N == 0) {
            Exception e("Must call Initialize() first");
            GPSTK_THROW(e);
         }
         if(x < X[0] || x > X[N-1]) {
            Exception e("Input value is outside range determined by Initialize()");
            GPSTK_THROW(e);
         }

         // find x in the array X
         int i,k;
         if(x == X[0]) return Y[0];
         for(i=1; i<N; i++) {
            if(x == X[i]) return Y[i];
            if(X[i-1] < x && x <= X[i]) {
               k = i;
               break;
            }
         }

         // interpolate
         return interpolate(k,x);
      }

      /// Compute interpolated values of y at a vector of x's; cf. Evalute(x).
      /// @param x  vector of values of independent variable X at which to evaluate.
      /// @return y  output vector of interpolated values parallel to x.
      /// @throw if the object has not been initialized by a call to Initialize(), or
      ///    if the given x is outside the range of the data used in Initialize().
      std::vector<T> Evaluate(const std::vector<T>& x) throw(Exception)
      {
         if(N == 0) {
            Exception e("Must call Initialize() first");
            GPSTK_THROW(e);
         }

         int n = x.size();
         std::vector<T> y(n);

         int i,k=0;
         for(i=0; i<n; i++) {
            if(x[i] < X[0] || x[i] > X[N-1]) {
               Exception e(std::string("Input value at index ")
                  + StringUtils::asString(i)
                  + std::string(" is outside range determined by Initialize()"));
               GPSTK_THROW(e);
            }

            // find x[i] in the array X: X[k-1] < x[i] <= X[k]
            if(k < N && x[i] > X[k]) {
               while(x[i] > X[k]) k++;  // move up
            }
            else if(k > 0 && x[i] < X[k-1]) {
               while(x[i] <= X[k-1]) k--;    // move down
            }

            if(x[i] == X[k])
               y[i] = Y[k];
            else if(x[i] == X[k-1])
               y[i] = Y[k-1];
            else
               y[i] = interpolate(k,x[i]);
         }

         return y;
      }

      /// Return the current size of the second derivative array.
      int size(void) const throw()
         { return S.size(); }

   private:
      /// Initialize array of second derivatives with the values of the data x(N) and
      /// y(N). If the first derivative at the end points has been fixed, the input
      /// boolean is true. Called by all versions of Initialize().
      /// @param x  Vector of data for independent variable, must parallel y, and must
      ///             be strictly increasing.
      /// @param y  Vector of data for dependent variable, must parallel x.
      /// @param fixEnds  if true, initial and final values of derivative dy/dx have
      ///             been set using member data fd1 and fdN.
      /// @throw if input is invalid: arrays are empty or x not strictly increasing.
      void build(const std::vector<T>& x, const std::vector<T>& y, const bool fixEnds)
         throw(Exception)
      {
         int i;

         // clear old values
         X.clear(); Y.clear(); S.clear();

         // size of array S
         N = (x.size() < y.size() ? x.size() : y.size());
         if(N == 0) {
            Exception e("Input data array(s) empty");
            GPSTK_THROW(e);
         }
         
         // resize arrays
         X = std::vector<T>(N);
         Y = std::vector<T>(N);
         S = std::vector<T>(N);

         // x must be strictly increasing; use this loop to copy out the data.
         X[0] = x[0]; Y[0] = y[0];
         for(i=1; i<N; i++) {
            if(x[i-1] >= x[i]) {
               Exception e("Input data array X is not strictly increasing");
               GPSTK_THROW(e);
            }
            X[i] = x[i];
            Y[i] = y[i];
         }

         // min 4 points needed for cubic spline
         if(N <= 3) {                  // linear interpolation
            for(i=0; i<N; i++) S[i] = T(0);
            return;
         }

         // fix derivatives at endpoints
         T dx1, dx2;
         if(!fixEnds) {
            // this fits parabola to endpoints
            dx1 = X[1]-X[0];
            dx2 = X[2]-X[0];
            fd1 = ((Y[1]-Y[0])/(dx1*dx1) - (Y[2]-Y[0])/(dx2*dx2))
                             / (T(1)/dx1 - T(1)/dx2);

            dx1 = X[N-2]-X[N-1];
            dx2 = X[N-3]-X[N-1];
            fdN = ((Y[N-2]-Y[N-1])/(dx1*dx1) - (Y[N-3]-Y[N-1])/(dx2*dx2))
                                 / (T(1)/dx1 - T(1)/dx2);
         }

         // second derivatives at endpoints...
         S[0] = T(6)*((Y[1]-Y[0])/(X[1]-X[0]) - fd1);
         S[N-1] = T(6)*(fdN + (Y[N-2]-Y[N-1])/(X[N-1]-X[N-2]));

         // ...and at point in between
         for(i=1; i<N-1; i++)
            S[i] = T(6)*(  Y[i-1]/(X[i]-X[i-1])
                         - Y[i] * (T(1)/(X[i]-X[i-1])+T(1)/(X[i+1]-X[i]))
                         + Y[i+1]/(X[i+1]-X[i]));

         // finish with recursion
         std::vector<T> A(N);
         A[0] = T(2)*(X[1]-X[0]);
         A[1] = T(1.5)*(X[1]-X[0]) + T(2)*(X[2]-X[1]);
         S[1] -= T(0.5)*S[0];
         for(i=2; i<N-1; i++) {
            dx1 = (X[i]-X[i-1])/A[i-1];
            A[i] = T(2)*(X[i+1]-X[i-1]) - dx1*(X[i]-X[i-1]);
            S[i] -= dx1*S[i-1];
         }
         dx1 = (X[N-1]-X[N-2])/A[N-2];
         A[N-1] = (T(2)-dx1)*(X[N-1]-X[N-2]);
         S[N-1] -= dx1*S[N-2];

         // back substitute to get second derivatives
         S[N-1] /= A[N-1];
         for(i=N-2; i>=0; i--) {
            S[i] -= (X[i+1]-X[i])*S[i+1];
            S[i] /= A[i];
         }
      }

      /// Given an index k into the array S, and a value x such that
      /// X[k-1] < x < X[k], find the interpolated value y at x.
      T interpolate(const int k, const T x) throw()
      {
         T dxr(X[k]-x), dxl(x-X[k-1]), dx(X[k]-X[k-1]);
         return (( dxl * (Y[k] - S[k]*dx*dx/T(6))
                 +       (S[k-1]*dxr*dxr*dxr + S[k]*dxl*dxl*dxl)/T(6)
                 + dxr * (Y[k-1] - S[k-1]*dx*dx/T(6)))/dx);
      }

      /// Data arrays
      std::vector<T> X, Y;

      /// Array of second derivatives, computed by build() and used in interpolate().
      std::vector<T> S;

      /// Values of the derivative dy/dx at the first and last data points
      T fd1, fdN;

      /// size of the arrays X,Y,S
      int N;

   }; // end class CubicSpline

   //@}

}  // namespace

#endif
