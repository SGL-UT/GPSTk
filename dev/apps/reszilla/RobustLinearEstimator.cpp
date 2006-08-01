#pragma ident "$Id$"


#include <math.h>
#include <algorithm>
#include <limits>

#include "RobustLinearEstimator.hpp"

#define SIGN(a,b) ((b) >= 0.0 ? std::abs(a) : -std::abs(a))

using namespace std;


double median(vector<double>& vec)
{
   size_t N=vec.size();
   double m=vec[N/2];
   if (! (N & 1))
      m = 0.5 * (m + vec[N/2-1]);
   return m;
}

double medTail(vector<double>& vec, double f)
{
   size_t N=vec.size();
   size_t M=std::max(static_cast<int>(N*(1-f)), 0);
   double med = median(vec);
   double ml = std::abs(vec[M]-med);
   double mh = std::abs(vec[N-M]-med);
   return std::max(ml, mh);
}


void RobustLinearEstimator::process(const DoubleDoubleVec& d)
{
   std::vector<double> y(d.size());
   size_t j=0;
   for (DDV::const_iterator i=d.begin(); i!=d.end(); i++,j++)
      y[j] = i->second;

   sort(y.begin(), y.end());
   stripY=medTail(y, stripPercent);
   medianY=median(y);

   // As a first guess for a and b, find the least-squares fit
   for (DDV::const_iterator i=d.begin(); i!=d.end(); i++)
   {
      if (std::abs(i->second - medianY) > stripY)
         continue;
      data.push_back(*i);
      double x = i->first;
      double y = i->second;
      sumX += x;
      sumY += y;
      sumXY += x*y;
      sumXX += x*x;
   }

   if (data.size()<3)
      return;
   double n = static_cast<double>(data.size());
   double del = n*sumXX - sumX*sumX;
   a = (sumXX*sumY - sumX*sumXY) / del;
   b = (n*sumXY - sumX*sumY) / del;

   // Now compute the chi^2 for the LR computed a, b
   double chisq=0;
   for (DDV::const_iterator i=data.begin(); i!=data.end(); i++)
   {
      double x = i->first;
      double y = i->second;
      double tmp = y - (a + b*x);
      chisq += tmp*tmp;
   }

   // The standard deviation will give some idea of how big an iteration
   // step to take.
   double sig_b = std::sqrt(chisq/del);
   double b1 = b;

   if (debugLevel)
      cout << setprecision(3) << fixed
           << "n=" << (int)n << " sig_b=" << sig_b << " chisq=" << chisq << endl
           << "a="<< a << " b=" << b << endl;

   double f1 = rofunc(b1);

   // If the sigma on b is already zero, then the current values of a & b are
   // perfect. This should never happen with N>3, I think.
   if (sig_b == 0.0)
      return;

   double b2 = b + SIGN(1*sig_b, f1);
   double f2 = rofunc(b2);

   if (b2 == b1)
      return;

   while (f1*f2 > 0)
   {
      b  = b2 + 1.6*(b2-b1);
      b1 = b2;
      f1 = f2;
      b2 = b;
      f2 = rofunc(b2);
   }
   sig_b *= 0.01;

   // Refine until the error is a negligible number of standard deviations
   while (std::abs(b2-b1) > sig_b)
   {
      b = b1 + 0.5 * (b2-b1);
      if (b == b1 || b == b2) 
         break;
      double f = rofunc(b);
      if (f*f1 >= 0)
      {
         f1=f;
         b1=b;
      }
      else
      {
         f2=f;
         b2=b;
      }
   }
}

// Evaluates the right-hand side of equation (15.7.16) of Numerical Recipes 
// for a given value of b.
// Note that this function updates a, and abdev
double RobustLinearEstimator::rofunc(const double b_est)
{
   vector<double> abxVec(data.size());

   size_t j=0;
   for (DDV::const_iterator i=data.begin(); i!=data.end(); i++)
   {
      double x = i->first;
      double y = i->second;
      abxVec[j++] = y - b_est * x;
   }

   sort(abxVec.begin(), abxVec.end());
   a = median(abxVec);

   std::numeric_limits<double> dl;
   const double eps=dl.epsilon();
   abdev=0;
   double sum=0;
   for (DDV::const_iterator i=data.begin(); i != data.end(); i++)
   {
      double x=i->first;
      double y=i->second;
      double d = y - (b_est * x + a);
      abdev += std::abs(d);
      if (y != 0)
         d /= std::abs(y);
      if (std::abs(d) > eps)
         sum += (d >= 0 ? x : -x);
   }

   abdev /= data.size();

   if (debugLevel)
      cout <<"a="<< a << " b=" << b << " b_est=" << b_est 
           << " f=" << sum << " abdev=" << abdev << endl;

   return sum;
}
