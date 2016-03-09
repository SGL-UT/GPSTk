/// @file SpecialFuncs.cpp
/// Implementation of special functions, including log Gamma, factorial, binomial
/// coefficients, beta, incomplete beta, and error functions, as well as
/// probability density functions (PDFs) for various distributions, with their
/// cumulative distribution (CDF) and percent point (inverse CDF) functions;
/// these include the Chi square, Student t and F distributions.
/// The percent point function PPf() is the inverse of the CDF() :
///    PPf(alpha,N1,N2) == F where alpha=CDF(F,N1,N2).
/// References: the NIST Engineering Statistics Handbook, 2006
/// http://www.itl.nist.gov/div898/handbook/ and Abramowitz and Stegun.

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

#include <cmath>
#include <limits>
#include "SpecialFuncs.hpp"
//#include "logstream.hpp"   // TEMP

namespace gpstk
{

// Natural log of the gamma function for positive argument.
// Gamma(x) = integral(0 to inf) { t^(x-1) exp(-t) dt }
// param x  argument, x must be > 0
// return   double ln(gamma(x)), the natural log of the gamma function of x.
// throw    if the input argument is <= 0
double lnGamma(const double& x) throw(Exception)
{
   try {
      static const double con[8] = {
         76.18009172947146, -86.50532032941677, 24.01409824083091,
         -1.231739572450155, 1.208650973866179e-3, -5.395239384953e-6,
         1.000000000190015, 2.5066282746310005 };

      if(x <= 0) GPSTK_THROW(Exception("Non-positive argument"));

      double y(x);
      double t(x+5.5);
      t -= (x+0.5) * ::log(t);
      double s(con[6]);
      for(int i=0; i<=5; i++) s += con[i] / ++y;

      return (-t + ::log(con[7]*s/x));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Gamma(x) the gamma function for positive argument.
// Gamma(x) = integral(0 to inf) { t^(x-1) exp(-t) dt }
// param    x argument, x must be > 0
// return   double Gamma(x), the gamma function of x.
// throw    if the input argument is <= 0
double Gamma(const double& x) throw(Exception)
{
   try {
      return ::exp(lnGamma(double(x)));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Factorial of an integer, returned as a double.
// param    n argument, n must be >= 0
// return   n! or factorial(n), as a double
// throw    if the input argument is < 0
double factorial(const int& n) throw(Exception)
{
   try {
      if(n < 0) GPSTK_THROW(Exception("Negative argument"));

      if(n > 32) return ::exp(lnGamma(double(n+1)));

      static double store[33] = { 1.0, 1.0, 2.0, 6.0, 24.0, 120.0 };
      static int nstore=5;

      while(nstore < n) {
         int i = nstore++;
         store[nstore] = store[i] * nstore;
      }

      return store[n];

   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// ln of Factorial of an integer, returned as a double.
// param    n argument, n must be >= 0
// return   ln(n!) or natural log of factorial(n), as a double
// throw    if the input argument is < 0
double lnFactorial(const int& n) throw(Exception)
{
   try {
      if(n < 0) GPSTK_THROW(Exception("Negative argument"));
      if(n <= 1) return 0.0;
      return lnGamma(double(n+1));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Binomial coefficient (n k) = n!/[k!(n-k)!], 0 <= k <= n.
// (n k) is the number of combinations of n things taken k at a time.
// NB. (n+1 k) = [ (n+1)/(n-k+1) ] (n k) = (n k) + (n k-1)
// NB. (n k+1) = [ (n-k)/(k+1) ] (n k)
// param n  int n must be >= 0
// param k  int k must be >= 0 and <= n
// return   (n k), the binomial coefficient
// throw    if the input argument do not satisfy 0 <= k <= n
double binomialCoeff(const int& n, const int& k) throw(Exception)
{
   try {
      if(n < 0 || k > n) GPSTK_THROW(Exception("Invalid arguments"));

      if(n <= 32) return (factorial(n) / (factorial(k)*factorial(n-k)));

      return floor(0.5 + ::exp(lnFactorial(n)-lnFactorial(k)-lnFactorial(n-k)));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Beta function. Beta(x,y)=Beta(y,x)=integral(0 to 1) {t^(x-1)*(1-t)^(y-1) dt}.
// Also, Beta(x,y) = gamma(x)*gamma(y)/gamma(x+y).
// param x  first argument
// param y  second argument
// return          beta(x,y)
// throw           if either input argument is <= 0
double beta(const double& x, const double& y) throw(Exception)
{
   try {
      return ::exp(lnGamma(x) + lnGamma(y) - lnGamma(x+y));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Incomplete gamma function P(a,x), evaluated using series representation.
// P(a,x) = (1/gamma(a)) integral (0 to x) { exp(-t) t^(a-1) dt }
// param a  first argument, a > 0
// param x  second argument, x >= 0
// return          P(a,x)
// throw           if input arguments have a <= 0 or x < 0
double seriesIncompGamma(const double& a, const double& x) throw(Exception)
{
   try {
      if(x < 0) GPSTK_THROW(Exception("Negative first argument"));
      if(a <= 0) GPSTK_THROW(Exception("Non-positive second argument"));

      static const int imax(600);
      static const double eps(10*std::numeric_limits<double>().epsilon());

      double lngamma(lnGamma(a));

      double atmp(a),sum(1.0/a);
      double del(sum);
      for(int i=1; i<=imax; i++) {
         ++atmp;
         del *= x/atmp;
         sum += del;
         if(::fabs(del) < ::fabs(sum)*eps)
            return (sum * ::exp(-x + a * ::log(x) - lngamma));
      }
      GPSTK_THROW(Exception("Overflow; first arg too big"));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }

   return 0.0;
}

// Incomplete gamma function Q(a,x), evaluated using continued fractions.
// Q(a,x) = (1/gamma(a)) integral (x to inf) { exp(-t) t^(a-1) dt }
// param a  first argument, a > 0
// param x  second argument, x >= 0
// return          Q(a,x)
// throw           if input arguments have a <= 0 or x < 0
double contfracIncompGamma(const double& a, const double& x) throw(Exception)
{
   try {
      if(x < 0) GPSTK_THROW(Exception("Negative first argument"));
      if(a <= 0) GPSTK_THROW(Exception("Non-positive second argument"));

      static const int imax(600);
      static const double eps(10*std::numeric_limits<double>().epsilon());
      static const double fpmin(10*std::numeric_limits<double>().min());

      double lngamma(lnGamma(a));

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

      if(i > imax) GPSTK_THROW(Exception("Overflow; first arg too big"));

      return (::exp(-x + a * ::log(x) - lngamma) * h);
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Incomplete gamma function P(a,x), a,x > 0.
// P(a,x) = (1/gamma(a)) integral (0 to x) { exp(-t) t^(a-1) dt }; a > 0, x >= 0
// param a  first argument, a > 0
// param x  second argument, x >= 0
// return          P(a,x)
// throw           if input arguments have a <= 0 or x < 0
double incompGamma(const double& a, const double& x) throw(Exception)
{
   try {
      if(x < 0) GPSTK_THROW(Exception("Negative first argument"));
      if(a <= 0) GPSTK_THROW(Exception("Non-positive second argument"));

      if(x < a+1.0)
         return seriesIncompGamma(a,x);
      else
         return (1.0 - contfracIncompGamma(a,x));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Complement of incomplete gamma function Q(a,x), a > 0, x >= 0.
// Q(a,x) = (1/gamma(a)) integral (x to inf) { exp(-t) t^(a-1) dt }
// param a  first argument, a > 0
// param x  second argument, x >= 0
// return          Q(a,x)
// throw           if input arguments have a <= 0 or x < 0
double compIncompGamma(const double& a, const double& x) throw(Exception)
{
   try {
      if(x < 0) GPSTK_THROW(Exception("Negative first argument"));
      if(a <= 0) GPSTK_THROW(Exception("Non-positive second argument"));

      if(x < a+1.0)
         return (1.0 - seriesIncompGamma(a,x));
      else
         return contfracIncompGamma(a,x);
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Error function erf(x). erf(x) = 2/sqrt(pi) * integral (0 to x) { exp(-t^2) dt }
// param x  input argument
// return          erf(x)
double errorFunc(const double& x) throw(Exception)
{
   if(x < 0) GPSTK_THROW(Exception("Negative first argument"));
   try {
      return (x < 0.0 ? -incompGamma(0.5,x*x) : incompGamma(0.5,x*x));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Complementary error function erfc(x). erfc(x) = 1-erf(x)
// param x  input argument
// return          erfc(x)
double compErrorFunc(const double& x) throw(Exception)
{
   if(x < 0) GPSTK_THROW(Exception("Negative first argument"));
   try {
      return (x < 0.0 ? 1.0+incompGamma(0.5,x*x) : compIncompGamma(0.5,x*x));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Compute continued fractions portion of incomplete beta function I_x(a,b)
// Routine used internally for Incomplete beta function I_x(a,b)
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
   if(i > imax)
      GPSTK_THROW(Exception("Overflow; a or b too big"));

   return h;
}

// Incomplete beta function I_x(a,b), 0<=x<=1, a,b>0
// I sub x (a,b) = (1/beta(a,b)) integral (0 to x) { t^(a-1)*(1-t)^(b-1)dt }
// param x  input value, 0 <= x <= 1
// param a  input value, a > 0
// param b  input value, b > 0
// return          Incomplete beta function I_x(a,b)
double incompleteBeta(const double& x, const double& a, const double& b)
   throw(Exception)
{
   if(x < 0 || x > 1) GPSTK_THROW(Exception("Invalid x argument"));
   if(a <= 0 || b <= 0) GPSTK_THROW(Exception("Non-positive argument"));

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
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// ----------------- probability distributions -----------------------

// Normal distribution of sample mean mu and sample std deviation sig
// (location and scale parameters, resp.).
// \code
// NormalPDF(x,mu,sig) = exp(-(x-mu)*(x-mu)/(2*sig*sig));
// NormalCDF(x,mu,sig) = 0.5*(1+erf((x-mu)/(::sqrt(2)*sig));
// \endcode
// For both theoretical and practical reasons, the normal distribution is
// probably the most important distribution in statistics.
// Many classical statistical tests are based on the assumption that the data
// follow a normal distribution. (This assumption should be tested before
// applying these tests.) In modeling applications, such as linear and non-linear
// regression, the error term is often assumed to follow a normal distribution
// with fixed location (mu) and scale (sig).
// The normal distribution is widely used. Part of its appeal is that it is well
// behaved and mathematically tractable. However, the central limit theorem
// provides a theoretical basis for why it has wide applicability.
// The central limit theorem states that as the sample size n becomes large,
// the following occur:
//   The sampling distribution of the mean becomes approximately normal
//      regardless of the distribution of the original variable.
//   The sampling distribution of the mean is centered at the population mean,
//      mu, of the original variable. In addition, the standard deviation of the
//      sampling distribution of the mean approaches sig/sqrt(n). 
// Probability density function (PDF) of the Normal distribution.
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.1
// param x   input statistic
// param mu  mean of the sample (location parameter of the distribution)
// param sig std dev of the sample (scale parameter of the distribution)
// return         Normal distribution probability density
double NormalPDF(const double& x, const double& mu, const double& sig)
   throw(Exception)
{
   try {
      return (::exp(-(x-mu)*(x-mu)/(2.0*sig*sig)));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Cumulative distribution function (CDF) of the Normal-distribution.
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.1
// param x   input statistic
// param mu  mean of the sample (location parameter of the distribution)
// param sig std dev of the sample (scale parameter of the distribution)
// return           Normal distribution probability
double NormalCDF(const double& x, const double& mu, const double& sig)
   throw(Exception)
{
   if(sig <= 0.0) GPSTK_THROW(Exception("Non-positive sigma"));

   try {
      static const double sqrt2(::sqrt(2.0));
      double arg(x-mu);
      double erf = errorFunc(::fabs(arg)/(sqrt2*sig));
      return (0.5 * (1.0 + (arg < 0.0 ? -erf : erf)));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Normal-distribution percent point function, or inverse of the Normal CDF.
// This function(prob,mu,sig) == X where prob = NormalCDF(X,mu,sig).
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.1
// param prob probability or significance level of the test, >=0 and < 1
// param mu  mean of the sample (location parameter of the distribution)
// param sig std dev of the sample (scale parameter of the distribution)
// return        X the statistic at this probability
double invNormalCDF(double prob, const double& mu, const double& sig)
   throw(Exception)
{
   try {
      if(prob < 0 || prob >= 1)
         GPSTK_THROW(Exception("Invalid probability argument"));
      if(sig <= 0.0) GPSTK_THROW(Exception("Non-positive sigma"));

      static const double eps(1000000*std::numeric_limits<double>().epsilon());

      if(prob < eps)
         return 0.0;
      if(1.0-prob < eps)
         GPSTK_THROW(Exception("Invalid probability -- too close to 1.0"));

      // find X such that prob == NormalCDF(X,mu,sig); use bracket method
      // we know 0.5 = NormalCDF(muwhen prob = 0.5, X = mu
      // also invNormalCDF(1-prob,mu,sig) = 2*mu - invNormalCDF(prob,mu,sig)
      // so make alpha >= 0.5
      bool swap(false);
      double alpha(prob);
      if(prob < 0.5) { swap=true; alpha=1.0-prob; }

      // we know a0 = NormalCDF(X0,mu,sig) where a0=0.5,X0=mu and alpha >= 0.5
      double X,X0(mu),X1,a;
      // first find X1 such that a1 = NormalCDF(X1,mu,sig) and a1 > alpha
      X1 = 2.0;
      while((a = NormalCDF(X1,mu,sig)) <= alpha) { X1 *= 2.0; }

      // bracket
      int niter(0);      // iteration count
      while(1) {
         X = (X0+X1)/2.0;
         a = NormalCDF(X,mu,sig);
         //LOG(INFO) << "LOOP invNormalCDF X = " << niter << " " << std::fixed
         //<< std::setprecision(15) << X0 << " < " << X << " < " << X1
         //<< " and a = " << a << " alpha = " << alpha << " a-alpha = "
         //<< std::scientific << std::setprecision(2) << a-alpha << " eps " << eps
         //<< " fabs(a-alpha)-eps " << ::fabs(alpha-a)-eps;
         if(::fabs(alpha-a) < eps) break;
         if(a > alpha) { X1 = X; } else { X0 = X; }
         if(++niter > 100) GPSTK_THROW(Exception("Failed to converge"));
      }

      return (swap ? 2.0*mu-X : X);
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Probability density function (PDF) of the Chi-square distribution.
// The chi-square distribution results when n independent variables with
// standard normal distributions are squared and summed; x=RSS(variables).
//
// A chi-square test (Snedecor and Cochran, 1983) can be used to test if the
// standard deviation of a population is equal to a specified value. This test
// can be either a two-sided test or a one-sided test. The two-sided version
// tests against the alternative that the true standard deviation is either
// less than or greater than the specified value. The one-sided version only
// tests in one direction.
// The chi-square hypothesis test is defined as:
//   H0:   sigma = sigma0
//   Ha:   sigma < sigma0    for a lower one-tailed test
//   sigma > sigma0          for an upper one-tailed test
//   sigma <>sigma0          for a two-tailed test
//   Test Statistic:   T = T = (N-1)*(s/sigma0)**2
//      where N is the sample size and s is the sample standard deviation.
// The key element of this formula is the ratio s/sigma0 which compares the ratio
// of the sample standard deviation to the target standard deviation. As this
// ratio deviates from 1, the more likely is rejection of the null hypothesis.
// Significance Level:  alpha.
// Critical Region:  Reject the null hypothesis that the standard deviation
// is a specified value, sigma0, if
//   T > chisquare(alpha,N-1)     for an upper one-tailed alternative
//   T < chisquare(1-alpha,N-1)   for a lower one-tailed alternative
//   T < chisquare(1-alpha,N-1)   for a two-tailed test or
//   T < chisquare(1-alpha,N-1)
// where chi-square(p,N-1) is the critical value or inverseCDF of the chi-square
// distribution with N-1 degrees of freedom. 
//
// param x input statistic, equal to an RSS(); x >= 0
// param n    input value for number of degrees of freedom, n > 0
// return         probability Chi-square probability (xsq,n)
double ChisqPDF(const double& x, const int& n) throw(Exception)
{
   if(x < 0) GPSTK_THROW(Exception("Negative statistic"));
   if(n <= 0)
      GPSTK_THROW(Exception("Non-positive degrees of freedom"));

   try {
      double dn(double(n)/2.0);
      return ( ::exp(-x/2.0) * ::pow(x,dn-1.0) / (::pow(2.0,dn) * Gamma(dn)) );
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Cumulative distribution function (CDF) of the Chi-square-distribution.
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.6
// param x  input statistic value, the RSS of variances, X >= 0
// param n     degrees of freedom of sample, n > 0
// return          probability that the sample variance is less than X.
double ChisqCDF(const double& x, const int& n) throw(Exception)
{
   if(x < 0) GPSTK_THROW(Exception("Negative statistic"));
   if(n <= 0)
      GPSTK_THROW(Exception("Non-positive degrees of freedom"));

   try {
      // NB this incompGamma(n/2,x/2) == NIST's incompGamma(n/2,x/2)/Gamma(n/2)
      return incompGamma(double(n)/2.0,x/2.0);
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Chi-square-distribution percent point function, or inverse of the Chisq CDF.
// This function(alpha,N) == Y where alpha = ChisqCDF(Y,N).
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.6
// param alpha probability or significance level of the test, >=0 and < 1
// param n   degrees of freedom of sample, n > 0
// return        X the statistic (an RSS of variances) at this probability
double invChisqCDF(double alpha, int n) throw(Exception)
{
   try {
      if(alpha < 0 || alpha >= 1)
         GPSTK_THROW(Exception("Invalid probability argument"));
      if(n <= 0)
         GPSTK_THROW(Exception("Non-positive degree of freedom"));

      static const double eps(1000000*std::numeric_limits<double>().epsilon());
      if(alpha < eps)
         return 0.0;
      if(1.0-alpha < eps)
         GPSTK_THROW(Exception("Invalid probability -- too close to 1.0"));

      // find X such that alpha == ChisqCDF(X,n); use bracket method
      // we know a0 = ChisqCDF(F0,n) where a0=F0=0
      double X,X0(0.0),X1,a;
      // first find X1 such that a1 = ChisqCDF(X1,N) and a1 > alpha
      X1 = 2.0;
      while((a = ChisqCDF(X1,n)) <= alpha) { X1 *= 2.0; }

      // bracket
      int niter(0);      // iteration count
      while(1) {
         X = (X0+X1)/2.0;
         a = ChisqCDF(X,n);
         //LOG(INFO) << "LOOP invChisqCDF X = " << niter << " " << std::fixed
         //<< std::setprecision(15) << X0 << " < " << X << " < " << X1
         //<< " and a = " << a << " alpha = " << alpha << " a-alpha = "
         //<< std::scientific << std::setprecision(2) << a-alpha << " eps " << eps
         //<< " fabs(a-alpha)-eps " << ::fabs(alpha-a)-eps;
         if(::fabs(alpha-a) < eps) break;
         if(a > alpha) { X1 = X; } else { X0 = X; }
         if(++niter > 100) GPSTK_THROW(Exception("Failed to converge"));
      }

      return X;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Probability density function (PDF) of the Student t distribution.
// The null hypotheses that test the true mean, mu, against the standard or
// assumed mean, mu0 are:
//   H0: mu = mu0
//   H0: mu <= mu0
//   H0: mu >= mu0 
// The basic statistics for the test are the sample mean and the standard
// deviation. The form of the test statistic depends on whether the poulation
// standard deviation, sigma, is known or is estimated from the data at hand.
// The more typical case is where the standard deviation must be estimated
// from the data, and the test statistic is
//    t = (Ybar - mu0/(s/SQRT(N))
// where the sample mean is
//    Ybar = (1/N)*SUM[i=1 to N]Y(i)
// and the sample standard deviation is
//    s = SQRT{(1/(N-1))*SUM[i=1 to N][Y(i) - Ybar)**2}
// with N - 1 degrees of freedom.
// For a test at significance level (probability) alpha, where alpha is chosen to
// be small, typically .01, .05 or .10, the hypothesis associated with each case
// enumerated above is rejected if:
//   |t| >= t(alpha/2,N-1)
//   t >= t(alpha,N-1)
//   t <= -t(alpha,N-1) 
//   where t(alpha/2,N-1) is the upper alpha/2 critical value (inverse CDF)
// from the t distribution with N-1 degrees of freedom.
// param X input statistic
// param n    input value for number of degrees of freedom, n > 0
// return         probability density
double StudentsPDF(const double& X, const int& n) throw(Exception)
{
   if(n <= 0)
      GPSTK_THROW(Exception("Non-positive degrees of freedom"));

   try {
      double dn(n);
      return (::pow(1.0 + X*X/dn, -(dn+1)/2.0) / (::sqrt(dn) * beta(0.5,0.5*dn)));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Cumulative Distribution Function CDF() for Student-t-distribution CDF.
// If X is a random variable following a normal distribution with mean zero and
// variance unity, and chisq is a random variable following an independent
// chi-square distribution with n degrees of freedom, then the distribution of
// the ratio X/sqrt(chisq/n) is called Student's t-distribution with n degrees
// of freedom. The probability that |X/sqrt(chisq/n)| will be less than a fixed
// constant t is StudentCDF(t,n);
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.4
// Abramowitz and Stegun 26.7.1
// param t  input statistic value
// param n     degrees of freedom of first sample, n > 0
// return          probability that the sample is less than X.
double StudentsCDF(const double& t, const int& n)
   throw(Exception)
{
   if(n <= 0) GPSTK_THROW(Exception("Non-positive degree of freedom"));

   try {
      // NB StudentsCDF(-t,n) = 1.0-StudentsCDF(t,n);
      double x = 0.5*incompleteBeta(double(n)/(t*t+double(n)),double(n)/2,0.5);
      if(t >= 0.0) return (1.0 - x);
      return (x);
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Students-t-distribution percent point function, or inverse of the Student CDF.
// This function(prob,n) == Y where prob = StudentsCDF(Y,n).
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.4
// param prob probability or significance level of the test, >=0 and < 1
// param n   degrees of freedom of sample, n > 0
// return        t the statistic at this probability
double invStudentsCDF(double prob, int n) throw(Exception)
{
   try {
      if(prob < 0 || prob >= 1)
         GPSTK_THROW(Exception("Invalid probability argument"));
      if(n <= 0)
         GPSTK_THROW(Exception("Non-positive degree of freedom"));

      static const double eps(1000000*std::numeric_limits<double>().epsilon());
      if(prob < eps)
         return 0.0;
      if(1.0-prob < eps)
         GPSTK_THROW(Exception("Invalid probability -- too close to 1.0"));

      // find X such that prob == StudentsCDF(X,n); use bracket method

      // NB StudentsCDF(-t,n) = 1.0-StudentsCDF(t,n);
      bool swap(false);
      double alpha(prob);
      if(prob < 0.5) { swap=true; alpha=1.0-prob; }

      // we know a0 = StudentsCDF(t0,n) where a0=0.5,t0=0 and alpha >= 0.5
      double t,t0(0.0),t1,a;
      // first find t1 such that a1 = StudentsCDF(t1,n) and a1 > alpha
      t1 = 2.0;
      while((a = StudentsCDF(t1,n)) <= alpha) { t1 *= 2.0; }

      // bracket
      int niter(0);      // iteration count
      while(1) {
         t = (t0+t1)/2.0;
         a = StudentsCDF(t,n);
         //LOG(INFO) << "LOOP invStudentsCDF t = " << niter << " " << std::fixed
         //<< std::setprecision(15) << t0 << " < " << t << " < " << t1
         //<< " and a = " << a << " alpha = " << alpha << " a-alpha = "
         //<< std::scientific << std::setprecision(2) << a-alpha << " eps " << eps
         //<< " fabs(a-alpha)-eps " << ::fabs(alpha-a)-eps;
         if(::fabs(alpha-a) < eps) break;
         if(a > alpha) { t1 = t; } else { t0 = t; }
         if(++niter > 100) GPSTK_THROW(Exception("Failed to converge"));
      }

      return (swap ? -t : t);
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// F-distribution cumulative distribution function FDistCDF(F,n1,n2) F>=0 n1,n2>0.
// This function occurs in the statistical test of whether two observed samples
// have the same variance. If F is the ratio of the observed dispersion (variance)
// of the first sample to that of the second, where the first sample has n1
// degrees of freedom and the second has n2 degrees of freedom, then this function
// returns the probability that F would be as large as it is if the first
// sample's distribution has smaller variance than the second's. In other words,
// FDistCDF(f,n1,n2) is the significance level at which the hypothesis
// "sample 1 has smaller variance than sample 2" can be rejected.
// A small numerical value implies a significant rejection, in turn implying
// high confidence in the hypothesis "sample 1 has variance greater than or equal
// to that of sample 2".
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.5
// param F  input statistic value, the ratio variance1/variance2, F >= 0
// param n1    degrees of freedom of first sample, n1 > 0
// param n2    degrees of freedom of second sample, n2 > 0
// return          probability that the sample is less than F.
double FDistCDF(const double& F, const int& n1, const int& n2)
   throw(Exception)
{
   if(F < 0) GPSTK_THROW(Exception("Negative statistic"));
   if(n1 <= 0 || n2 <= 0) GPSTK_THROW(Exception("Non-positive degree of freedom"));

   try {
      return (1.0 - incompleteBeta(double(n2)/(double(n2)+double(n1)*F),
                                   double(n2)/2.0,double(n1)/2.0));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Probabiliy density function for F distribution
// The F distribution is the ratio of two chi-square distributions with degrees
// of freedom N1 and N2, respectively, where each chi-square has first been
// divided by its degrees of freedom.
// An F-test (Snedecor and Cochran, 1983) is used to test if the standard
// deviations of two populations are equal. This test can be a two-tailed test or
// a one-tailed test.
// The F hypothesis test is defined as:
//   H0:   s1 = s2     (sN is sigma or std deviation)
//   Ha:   s1 < s2     for a lower one tailed test
//         s1 > s2     for an upper one tailed test
//         s1 != s2    for a two tailed test 
// Test Statistic: F = s1^2/s2^2 where s1^2 and s2^2 are the sample variances.
// The more this ratio deviates from 1, the stronger the evidence for unequal
// population variances. Significance Level is alpha, a probability (0<=alpha<=1).
// The hypothesis that the two standard deviations are equal is rejected if
//    F > PP(alpha,N1-1,N2-1)     for an upper one-tailed test
//    F < PP(1-alpha,N1-1,N2-1)     for a lower one-tailed test
//    F < PP(1-alpha/2,N1-1,N2-1)   for a two-tailed test
//    F > PP(alpha/2,N1-1,N2-1)
// where PP(alpha,k-1,N-1) is the percent point function of the F distribution
// [PPfunc is inverse of the CDF : PP(alpha,N1,N2) == F where alpha=CDF(F,N1,N2)]
// with N1 and N2 degrees of freedom and a significance level of alpha. 
// 
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.5
// param x probability or significance level of the test, >=0 and < 1
// param n1   degrees of freedom of first sample, n1 > 0
// param n2   degrees of freedom of second sample, n2 > 0
// return         the statistic (a ratio variance1/variance2) at this prob
double FDistPDF(double x, int n1, int n2) throw(Exception)
{
   try {
      double dn1(n1),dn2(n2);
      double F = Gamma((dn1+dn2)/2.0) / (Gamma(dn1/2.0)*Gamma(dn2/2.0));
      F *= ::pow(dn1/dn2,dn1/2.0) * ::pow(x,dn1/2.0 - 1.0);
      F /= ::pow(1.0+x*dn1/dn2,(dn1+dn2)/2.0);
      return F;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// F-distribution percent point function, or inverse of the F-dist CDF.
// this function(prob,N1,N2) == F where prob = FDistCDF(F,N1,N2).
// Ref http://www.itl.nist.gov/div898/handbook/ 1.3.6.6.5
// param prob probability or significance level of the test, >=0 and < 1
// param n1  degrees of freedom of first sample, n1 > 0
// param n2  degrees of freedom of second sample, n2 > 0
// return        F the statistic (a ratio variance1/variance2) at this prob
double invFDistCDF(double prob, int n1, int n2) throw(Exception)
{
   try {
      if(prob < 0 || prob >= 1)
         GPSTK_THROW(Exception("Invalid probability argument"));
      if(n1 <= 0 || n2 <= 0)
         GPSTK_THROW(Exception("Non-positive degree of freedom"));

      static const double eps(100000*std::numeric_limits<double>().epsilon());
      if(prob < eps)
         return 0.0;
      if(1.0-prob < eps)
         GPSTK_THROW(Exception("Invalid probability -- too close to 1.0"));

      // find F such that prob == FDistCDF(F,n1,n2); use bracket method

      // NB Abramowitz and Stegan 26.6.9: FDistCDF(F,n1,n2) = 1-FDistCDF(1/F,n2,n1)
      bool swap(false);
      int N1(n1),N2(n2);
      double alpha(prob);
      if(prob < 0.5) { swap=true; N1=n2, N2=n1; alpha=1.0-prob; }

      // we know a0 = FDistCDF(F0,N1,N2) where a0=F0=0 and alpha >= 0.5
      double F,F0(0.0),F1,a;
      // first find F1 such that a1 = FDistCDF(F1,N1,N2) and a1 > alpha
      F1 = 2.0;
      while((a = FDistCDF(F1,N1,N2)) <= alpha) { F1 *= 2.0; }

      // bracket
      int n(0);      // iteration count
      while(1) {
         F = (F0+F1)/2.0;
         a = FDistCDF(F,N1,N2);
         //LOG(INFO) << "LOOP invFDistCDF F = " << n << " " << std::fixed
         //<< std::setprecision(15) << F0 << " < " << F << " < " << F1
         //<< " and a = " << a << " alpha = " << alpha << " a-alpha = "
         //<< std::scientific << std::setprecision(2) << a-alpha << " eps " << eps
         //<< " fabs(a-alpha)-eps " << ::fabs(alpha-a)-eps;
         if(::fabs(alpha-a) < eps) break;
         if(a > alpha) { F1 = F; } else { F0 = F; }
         n++;
         if(n > 100) GPSTK_THROW(Exception("Failed to converge"));
      }

      return (swap ? 1.0/F : F);
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

}  // end namespace
