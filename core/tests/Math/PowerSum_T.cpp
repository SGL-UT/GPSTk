//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================

#include <list>
#include <iostream>

#include "Stats.hpp"
#include "PowerSum.hpp"
#include "Exception.hpp"
#include "TestUtil.hpp"
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
   TUDEF("PowerSum", "fail");
   gpstk::PowerSum ps;
   gpstk::Stats<double> s;

   for (int i=0; i<100000; i++)
   {
      double rv = gasdev();
      ps.add(rv);
      s.Add(rv);
   }

      //ps.dump(cout);

      //cout << "Stats class average:" << s.Average()
      //     << " stddev:" << s.StdDev() << endl;

   double e1 = std::abs(s.Average() - ps.average());
   double e2 = std::abs(s.StdDev() - sqrt(ps.variance()));
      //cout << "Disagreement in average: " << e1 << endl
      //     << "Disagreement in standard deviation: " << e2 << endl;

   try
   {
      TUCSM("average");
      TUASSERT(e1 < 1e-3);
      TUCSM("variance");
      TUASSERT(e2 < 1e-3);
      TUCSM("average");
      TUASSERT(std::abs(ps.average()) < 1e-3);
         // 2e-3 : tolerance is dependent on platform and can be improved with
         // better random number generators
      TUCSM("variance");
      TUASSERT(std::abs(sqrt(ps.variance())-1) < 2e-3);
      TUCSM("skew");
      TUASSERT(std::abs(ps.skew()) < 0.01);
      TUCSM("kurtosis");
      TUASSERT(std::abs(ps.kurtosis()-3) < 0.05);
   }
   catch (gpstk::Exception& e)
   {
      cout << e;
      TUFAIL("Exception");
   }


      //if (e1 > 1e-3 || e2 > 1e-3 || 
      //    std::abs(ps.average()) > 1e-3 || 
      //    std::abs(sqrt(ps.variance())-1) > 2e-3 || 
      //    std::abs(ps.skew()) > 0.01 ||
      //    std::abs(ps.kurtosis()-3) > 0.05)
      //{
      //   cout << "Error in computed values" << endl;
      //   return -1;
      //}
      //cout << "Looks good to me..." << endl;
      //return 0;
   TURETURN();
}
