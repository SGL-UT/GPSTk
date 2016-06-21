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

#include <iostream>
#include <set>

#include "ValarrayUtils.hpp"
#include "SparseBinnedStats.hpp"

using namespace std;
using namespace gpstk;
using namespace ValarrayUtils;

int main(void)
{
   try
   {

      double mpSrc[]=
      {
         0.9, -1.6, 1.8, -.5,
         1.0,  1.1, -.5, -.3,
         .3,   -.25, 0.4, .05
      };

      valarray<double> mpVals(mpSrc,12);

      double elSrc[]=
      {
         5,   6,   7,    8,
         11,  30,  34,   42,
         50,  60,  70,   80
      };

      valarray<double> elVals(elSrc,12);

      SparseBinnedStats<double> mstats;
      mstats.addBin(0,10);
      mstats.addBin(10,45);
      mstats.addBin(45, 90);
      mstats.addBin(10, 90);

      cout << "# bins: " << mstats.bins.size() << endl;

      mstats.addData(mpVals, elVals);

      cout << "MP data: " << endl << mpVals << endl;
      cout << "Elevation data: " << endl << elVals << endl;

      for (int i=0; i<mstats.stats.size(); i++)
      {
         cout << "From " << mstats.bins[i].lowerBound;
         cout << " to " <<  mstats.bins[i].upperBound;
         cout << ": " << mstats.stats[i].StdDev() << endl;
      }
      cout << "Total points used: " << mstats.usedCount << endl;
      cout << "         rejected: " << mstats.rejectedCount << endl;

   }
   catch (Exception& e)
   {
      cerr << e << endl;
   }

   return 0;
}
