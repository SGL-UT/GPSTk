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

#include <list>
#include <iostream>

#include "Stats.hpp"
#include "PowerSum.hpp"

#include <stdlib.h>
#include <math.h>

using namespace std;

// Generate a norimally distributed deviate with zero mean and unit variance. 
// Kudos to Press, Flannery, Teukolsky, and Veterling. And the man, Knuth.
// If you don't care about being thread safe, you can make v1 a static and use
// for every other call. Also this uses the libc standard rand() function
// which really blows by most accounts. If you want this to be a 'good'
// normal distribution, use a different rand()
double gasdev()
{
   const double mr2 = 2.0 / RAND_MAX;
   double v1, v2, fac;
   
   double r;
   do
   {
      v1 = mr2*rand() - 1;
      v2 = mr2*rand() - 1;
      r = v1*v1 + v2*v2;
   } while (r >= 1);
   fac = sqrt(-2*log(r)/r);
   return v2*fac;
}


int main(int argc, char *argv[])
{
   gpstk::PowerSum ps;
   gpstk::Stats<double> s;

   for (int i=0; i<100000; i++)
   {
      double rv = gasdev();
      ps.add(rv);
      s.Add(rv);
   }

   ps.dump(cout);

   cout << "Stats class average:" << s.Average()
        << " stddev:" << s.StdDev() << endl;

   double e1 = std::abs(s.Average() - ps.average());
   double e2 = std::abs(s.StdDev() - sqrt(ps.variance()));
   cout << "Disagreement in average: " << e1 << endl
        << "Disagreement in standard deviation: " << e2 << endl;

   if (e1 > 1e-3 || e2 > 1e-3 || 
       std::abs(ps.average()) > 1e-3 || 
       std::abs(sqrt(ps.variance())-1) > 1e-3 || 
       std::abs(ps.skew()) > 0.01 ||
       std::abs(ps.kurtosis()-3) > 0.05)
   {
      cout << "Error in computed values" << endl;
      return -1;
   }
   cout << "Looks good to me..." << endl;
   return 0;
}
