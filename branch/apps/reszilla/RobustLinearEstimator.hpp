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

#ifndef ROBUSTLINEARESTIMATOR_HPP
#define ROBUSTLINEARESTIMATOR_HPP

#include <map>
#include <vector>
#include <iostream>

#include <DayTime.hpp>

typedef std::pair<double, double> DoubleDouble;
typedef std::vector< DoubleDouble > DoubleDoubleVec;

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
       medianY(0), stripY(0), debugLevel(0),
       valid(false)
   {};

   void process(const DoubleDoubleVec& d);
   void process(DoubleDoubleVec::const_iterator b,
                DoubleDoubleVec::const_iterator e);

   double a, b, abdev;
   double medianY, stripY;
   int debugLevel;
   double stripPercent;
   bool valid;
   double eval(const double x) const {return a+b*x;};

private:
   DoubleDoubleVec data;
   double sumX, sumY, sumXX, sumXY;

   double rofunc(const double b_est);
};

#endif
