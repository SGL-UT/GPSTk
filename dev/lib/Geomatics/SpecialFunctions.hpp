#pragma ident "$Id: SpecialFunctions.hpp 292 2009-09-02 20:29:30Z BrianTolman $"

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================
//============================================================================
//
// This software developed by Applied Research Laboratories at the University
// of Texas at Austin, under contract to an agency or agencies within the U.S. 
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software. 
//
// Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file SpecialFunctions.hpp
 * Implementation of special functions, including log Gamma, factorial, binomial
 * coefficients, beta, incomplete beta, Student distribution, F distribution, etc.
 * Based on Numerical Recipes and Abramowitz and Stegun
 */

#ifndef SPECIAL_FUNCTIONS_INCLUDE
#define SPECIAL_FUNCTIONS_INCLUDE

#include <cmath>
#include <limits>
#include "Exception.hpp"

namespace gpstk
{
   /// Natural log of the gamma function for positive argument.
   /// Gamma(x) = integral(0 to inf) { t^(x-1) exp(-t) dt }
   /// @param x  double argument, x must be > 0
   /// @return   double ln(gamma(x)), the natural log of the gamma function of x.
   /// @throw    if the input argument is <= 0
   double lnGamma(const double& x) throw(Exception)
   {
      static const double con[8] = {
         76.18009172947146, -86.50532032941677, 24.01409824083091,
         -1.231739572450155, 1.208650973866179e-3, -5.395239384953e-6,
         1.000000000190015, 2.5066282746310005 };
      if(x <= 0) {
         Exception e("Non-positive argument in lnGamma()");
         GPSTK_THROW(e);
      }
      double y(x);
      double t(x+5.5);
      t -= (x+0.5) * ::log(t);
      double s(con[6]);
      for(int i=0; i<=5; i++) s += con[i] / ++y;
      return (-t + ::log(con[7]*s/x));
   }

   /// Factorial of an integer, returned as a double.
   /// @param n  int argument, n must be >= 0
   /// @return   n! or factorial(n), as a double
   /// @throw    if the input argument is < 0
   double factorial(const int& n) throw(Exception)
   {
      if(n < 0) {
         Exception e("Negative argument in factorial()");
         GPSTK_THROW(e);
      }

      if(n > 32) return ::exp(lnGamma(double(n+1)));

      static double store[33] = { 1.0, 1.0, 2.0, 6.0, 24.0, 120.0 };
      static int nstore=5;

      while(nstore < n) {
         int i = nstore++;
         store[nstore] = store[i] * nstore;
      }

      return store[n];
   }

   /// ln of Factorial of an integer, returned as a double.
   /// @param n  int argument, n must be >= 0
   /// @return   ln(n!) or natural log of factorial(n), as a double
   /// @throw    if the input argument is < 0
   double lnFactorial(const int& n) throw(Exception)
   {
      if(n < 0) {
         Exception e("Negative argument in lnFactorial()");
         GPSTK_THROW(e);
      }
      if(n <= 1) return 0.0;
      return lnGamma(double(n+1));
   }

   /// Binomial coefficient (n k) = n!/[k!(n-k)!], 0 <= k <= n.
   /// (n k) is the number of combinations of n things taken k at a time.
   /// NB. (n+1 k) = [ (n+1)/(n-k+1) ] (n k) = (n k) + (n k-1)
   /// NB. (n k+1) = [ (n-k)/(k+1) ] (n k)
   /// @param n  int n must be >= 0
   /// @param k  int k must be >= 0 and >= n
   /// @return   (n k), the binomial coefficient
   /// @throw    if the input argument do not satisfy 0 <= k <= n
   double binomialCoeff(const int& n, const int& k) throw(Exception)
   {
      if(n < 0 || k > n) {
         Exception e("Invalid arguments in binomialCoeff()"); 
         GPSTK_THROW(e);
      }

      if(n <= 32)
         return (factorial(n) / (factorial(k)*factorial(n-k)));

      return floor(0.5 + ::exp(lnFactorial(n)-lnFactorial(k)-lnFactorial(n-k)));
   }

   /// Beta function. Beta(x,y)=Beta(y,x)=integral(0 to 1) {t^(x-1)*(1-t)^(y-1) dt}.
   /// Also, Beta(x,y) = gamma(x)*gamma(y)/gamma(x+y).
   /// @param double x  first argument
   /// @param double y  second argument
   /// @return          beta(x,y)
   /// @throw           if either input argument is <= 0
   double beta(const double& x, const double& y) throw(Exception)
   {
      try {
         return ::exp(lnGamma(x) + lnGamma(y) - lnGamma(x+y));
      }
      catch(Exception& e) {
         e.addText("Called by beta(x,y)");
         GPSTK_RETHROW(e);
      }
   }

   /// Incomplete gamma function P(a,x), evaluated using series representation.
   /// P(a,x) = (1/gamma(a)) integral (0 to x) { exp(-t) t^(a-1) dt }
   /// @param double a  first argument, a > 0
   /// @param double x  second argument, x >= 0
   /// @return          P(a,x)
   /// @throw           if input arguments have a <= 0 or x < 0
   double seriesIncompGamma(const double& a, const double& x) throw(Exception)
   {
      if(x < 0) {
         Exception e("Negative first argument in seriesIncompGamma()");
         GPSTK_THROW(e);
      }
      if(a <= 0) {
         Exception e("Non-positive second argument in seriesIncompGamma()");
         GPSTK_THROW(e);
      }

      double lngamma(lnGamma(a));

      static const int imax(100);
      static const double eps(10*std::numeric_limits<double>().epsilon());

      double atmp(a),sum(1.0/a);
      double del(sum);
      for(int i=1; i<=imax; i++) {
         ++atmp;
         del *= x/atmp;
         sum += del;
         if(::fabs(del) < ::fabs(sum)*eps)
            return (sum * ::exp(-x + a * ::log(x) - lngamma));
      }
      Exception e("Overflow in seriesIncompGamma; first arg too big");
      GPSTK_THROW(e);

      return 0.0;
   }

   /// Incomplete gamma function Q(a,x), evaluated using continued fractions.
   /// Q(a,x) = (1/gamma(a)) integral (x to inf) { exp(-t) t^(a-1) dt }
   /// @param double a  first argument, a > 0
   /// @param double x  second argument, x >= 0
   /// @return          Q(a,x)
   /// @throw           if input arguments have a <= 0 or x < 0
   double contfracIncompGamma(const double& a, const double& x) throw(Exception)
   {
      if(x < 0) {
         Exception e("Negative first argument in contfracIncompGamma()");
         GPSTK_THROW(e);
      }
      if(a <= 0) {
         Exception e("Non-positive second argument in contfracIncompGamma()");
         GPSTK_THROW(e);
      }

      double lngamma(lnGamma(a));

      static const int imax(100);
      static const double eps(10*std::numeric_limits<double>().epsilon());
      static const double fpmin(10*std::numeric_limits<double>().min());

      double b(x+1.0-a),c(1.0/fpmin);
      double d(1.0/b);
      double h(d);
      int i;
      for(i=1; i<=imax; i++) {
         double an(-i*(i-a));
         b += 2.0;
         d = an*d+b;
         if(::fabs(d) < fpmin) d=fpmin;
         c = b+an/c;
         if(::fabs(c) < fpmin) c=fpmin;
         d = 1.0/d;
         double del(d*c);
         h *= del;
         if(::fabs(del-1.0) < eps) break;
      }
      if(i > imax) {
         Exception e("Overflow in contfracIncompGamma; first arg too big");
         GPSTK_THROW(e);
      }

      return (::exp(-x + a * ::log(x) - lngamma) * h);
   }

   /// Incomplete gamma function P(a,x), a,x > 0.
   /// P(a,x) = (1/gamma(a)) integral (0 to x) { exp(-t) t^(a-1) dt }; a > 0, x >= 0
   /// @param double a  first argument, a > 0
   /// @param double x  second argument, x >= 0
   /// @return          P(a,x)
   /// @throw           if input arguments have a <= 0 or x < 0
   double incompGamma(const double& a, const double& x) throw(Exception)
   {
      if(x < 0) {
         Exception e("Negative first argument in incompGamma()");
         GPSTK_THROW(e);
      }
      if(a <= 0) {
         Exception e("Non-positive second argument in incompGamma()");
         GPSTK_THROW(e);
      }

      if(x < a+1.0)
         return seriesIncompGamma(a,x);
      else
         return (1.0 - contfracIncompGamma(a,x));
   }

   /// Complement of incomplete gamma function Q(a,x), a > 0, x >= 0.
   /// Q(a,x) = (1/gamma(a)) integral (x to inf) { exp(-t) t^(a-1) dt }
   /// @param double a  first argument, a > 0
   /// @param double x  second argument, x >= 0
   /// @return          Q(a,x)
   /// @throw           if input arguments have a <= 0 or x < 0
   double compIncompGamma(const double& a, const double& x) throw(Exception)
   {
      if(x < 0) {
         Exception e("Negative first argument in compIncompGamma()");
         GPSTK_THROW(e);
      }
      if(a <= 0) {
         Exception e("Non-positive second argument in compIncompGamma()");
         GPSTK_THROW(e);
      }

      if(x < a+1.0)
         return (1.0 - seriesIncompGamma(a,x));
      else
         return contfracIncompGamma(a,x);
   }

   /// Error function erf(x). erf(x) = 2/sqrt(pi) * integral (0 to x) { exp(-t^2) dt }
   /// @param double x  input argument
   /// @return          erf(x)
   double errorFunc(const double& x) throw(Exception)
   {
      if(x < 0) {
         Exception e("Negative first argument in errorFunc()");
         GPSTK_THROW(e);
      }
      try {
         return (x < 0.0 ? -incompGamma(0.5,x*x) : incompGamma(0.5,x*x));
      }
      catch(Exception& e) {
         e.addText("Called by errorFunc()");
         GPSTK_RETHROW(e);
      }
   }

   /// Complementary error function erfc(x). erfc(x) = 1-erf(x)
   /// @param double x  input argument
   /// @return          erfc(x)
   double compErrorFunc(const double& x) throw(Exception)
   {
      if(x < 0) {
         Exception e("Negative first argument in compErrorFunc()");
         GPSTK_THROW(e);
      }
      try {
         return (x < 0.0 ? 1.0+incompGamma(0.5,x*x) : compIncompGamma(0.5,x*x));
      }
      catch(Exception& e) {
         e.addText("Called by compErrorFunc()");
         GPSTK_RETHROW(e);
      }
   }

   /// Chi-square probability function. ChisqProbability(xsq,n) is defined as the
   /// probability that the observed chi-squared for a correct model with n degrees
   /// of freedom should be less than the value xsq.
   /// @param double xsq input value for chi-squared, xsq > 0
   /// @param int n      input value for number of degrees of freedom, n > 0
   /// @return           Chi-squared probability (xsq,n)
   double ChisqProbability(const double& x, const int& n) throw(Exception)
   {
      if(x <= 0) {
         Exception e("Non-positive chi-sq argument in ChisqProbability()");
         GPSTK_THROW(e);
      }
      if(n < 0) {
         Exception e("Non-positive degrees of freedom in ChisqProbability()");
         GPSTK_THROW(e);
      }

      return incompGamma(double(n)/2.0,x/2.0);
   }

   /// Complementary chi-square probability function. CompChisqProbability(xsq,n) is
   /// defined as the probability that the observed chi-squared with n degrees of
   /// freedom will exceed the value xsq even for a correct model.
   /// @param double xsq input value for chi-squared, xsq > 0
   /// @param int n      input value for number of degrees of freedom, n > 0
   /// @return           Complementary chi-squared probability (xsq,n)
   double CompChisqProbability(const double& x, const int& n) throw(Exception)
   {
      if(x <= 0) {
         Exception e("Non-positive chi-sq argument in CompChisqProbability()");
         GPSTK_THROW(e);
      }
      if(n < 0) {
         Exception e("Non-positive degrees of freedom in CompChisqProbability()");
         GPSTK_THROW(e);
      }

      return compIncompGamma(double(n)/2.0,x/2.0);
   }

   // Compute continued fractions portion of incomplete beta function I_x(a,b)
   /// Routine used internally for Incomplete beta function I_x(a,b)
   double cfIBeta(const double& x, const double& a, const double& b) throw(Exception)
   {
      static const int imax(100);
      static const double eps(10*std::numeric_limits<double>().epsilon());
      static const double fpmin(10*std::numeric_limits<double>().min());
      const double qab(a+b);
      const double qap(a+1.0);
      const double qam(a-1.0);
      double c(1),d(1-qab*x/qap),aa,del;
      if(::fabs(d) < fpmin) d=fpmin;
      d = 1.0/d;
      double h(d);
      int i,i2;
      for(i=1; i<=imax; i++) {
         i2 = 2*i;
         aa = i*(b-i)*x/((qam+i2)*(a+i2));
         d = 1.0 + aa*d;
         if(::fabs(d) < fpmin) d=fpmin;
         c = 1.0 + aa/c;
         if(::fabs(c) < fpmin) c=fpmin;
         d = 1.0/d;
         h *= d*c;
         aa = -(a+i)*(qab+i)*x/((a+i2)*(qap+i2));
         d = 1.0 + aa*d;
         if(::fabs(d) < fpmin) d=fpmin;
         c = 1.0 + aa/c;
         if(::fabs(c) < fpmin) c=fpmin;
         d = 1.0/d;
         del = d*c;
         h *= del;
         if(::fabs(del-1.0) < eps) break;
      }
      if(i > imax) {
         Exception e("Overflow in cfIBeta(); a or b too big");
         GPSTK_THROW(e);
      }
      return h;
   }

   /// Incomplete beta function I_x(a,b), 0<=x<=1, a,b>0
   /// I sub x (a,b) = (1/beta(a,b)) integral (0 to x) { t^(a-1)*(1-t)^(b-1)dt }
   /// @param double x  input value, 0 <= x <= 1
   /// @param double a  input value, a > 0
   /// @param double b  input value, b > 0
   /// @return          Incomplete beta function I_x(a,b)
   double incompleteBeta(const double& x, const double& a, const double& b)
      throw(Exception)
   {
      if(x < 0 || x > 1) {
         Exception e("Invalid x argument in incompleteBeta()");
         GPSTK_THROW(e);
      }
      if(a <= 0 || b <= 0) {
         Exception e("Non-positive argument in incompleteBeta()");
         GPSTK_THROW(e);
      }

      if(x == 0) return 0.0;
      if(x == 1) return 1.0;

      try {
         double factor = ::exp(lnGamma(a+b) - lnGamma(a) - lnGamma(b)
                                    + a * ::log(x) + b * ::log(1.0-x));
         if(x < (a+1.0)/(a+b+2.0))
            return factor*cfIBeta(x,a,b)/a;
         else
            return 1.0-factor*cfIBeta(1.0-x,b,a)/b;
      }
      catch(Exception& e) {
         e.addText("Called by incompleteBeta()");
         GPSTK_RETHROW(e);
      }
   }

   /// Student's distribution probability function S(t,n), for n degrees of freedom.
   /// Student's distribution probability is used in the test of whether two observed
   /// distributions have the same mean. S(t,n) is the probability, for n degrees of
   /// freedom, that a statistic t (measuring the observed difference of means)
   /// would be smaller than the observed value if the means were in fact the same.
   /// Two means are significantly different if, e.g. S(t,n) > 0.99;
   /// in other words 1-S(t,n) is the significance level at which the hypothesis
   /// that the means are equal is disproved.
   /// @param double t  input statistic value
   /// @param int n     degrees of freedom, n > 0
   /// @return          Student's distribution probability P(t,n)
   double StudentsDistProbability(const double& t, const int& n) throw(Exception)
   {
      if(n <= 0) {
         Exception e("Non-positive degrees of freedom in StudentsDistribution()");
         GPSTK_THROW(e);
      }

      return (1.0 - incompleteBeta(double(n)/(t*t+double(n)),double(n)/2,0.5));
   }

   /// F distribution probability function F(f,n1,n2), f>=0, n1,n2>0
   /// This function occurs in the statistical test of whether two observed samples
   /// have the same variance. If f is the ratio of the observed dispersion of the
   /// first sample to that of the second one, where the first sample has n1 degrees
   /// of freedom and the second has n2 degrees of freedom, then this function
   /// returns the probability that f would be as large as it is if the first
   /// sample's distribution has smaller variance than the second's. In other words,
   /// FDistribution(f,n1,n2) is the significance level at which the hypothesis
   /// "sample 1 has smaller variance than sample 2" can be rejected.
   /// A small numerical value implies a significant rejection, in turn implying
   /// high confidence in the hypothesis "sample 1 has variance greater than or equal
   /// to that of sample 2".
   /// @param double f  input statistic value, the ratio variance1/variance2, f >= 0
   /// @param int n1    degrees of freedom of first sample, n1 > 0
   /// @param int n2    degrees of freedom of second sample, n2 > 0
   /// @return          F distribution F(f,n1,n2)
   double FDistProbability(const double& f, const int& n1, const int& n2)
      throw(Exception)
   {
      if(f < 0) {
         Exception e("Negative statistic in FDistribution()");
         GPSTK_THROW(e);
      }
      if(n1 <= 0 || n2 <= 0) {
         Exception e("Non-positive degrees of freedom in FDistribution()");
         GPSTK_THROW(e);
      }

      return incompleteBeta(double(n2)/(double(n2)+double(n1)*f),
                                      double(n2)/2.0,double(n1)/2.0);
   }

}  // end namespace

#endif // SPECIAL_FUNCTIONS_INCLUDE
