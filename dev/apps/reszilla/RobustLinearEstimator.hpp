#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/RobustLinearEstimator.hpp#3 $"

#ifndef ROBUSTLINEARESTIMATOR_HPP
#define ROBUSTLINEARESTIMATOR_HPP

#include <map>
#include <vector>
#include <iostream>

#include <DayTime.hpp>

typedef std::vector< std::pair<double, double> > DoubleDoubleVec;

// An object to compute a robust estimate of a linear fit to the
// series data given. It fits y = a + bx by the criterion of least absolute
// deviations. The fitted parameters a and b are output, along with abdev, which
// is the mean absolute deviation (in y) of the experimental points from the
// fitted line.
class RobustLinearEstimator
{
public:

   RobustLinearEstimator()
      :a(0), b(0), abdev(0), stripPercent(0.995),
      sumX(0), sumY(0), sumXX(0), sumXY(0),
      medianY(0), stripY(0), debugLevel(0)
   {};

   void process(const DoubleDoubleVec& d);
   double a, b, abdev;
   double medianY, stripY;
   int debugLevel;
   double stripPercent;
   double eval(const double x) const {return a+b*x;};

private:
   typedef DoubleDoubleVec DDV;
   DDV data;
   double sumX, sumY, sumXX, sumXY;

   double rofunc(const double b_est);
};

#endif
