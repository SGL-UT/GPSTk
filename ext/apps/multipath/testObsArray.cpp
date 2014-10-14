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

#include "SatID.hpp"
#include "CommonTime.hpp"
#include "ObsArray.hpp"
#include "ValarrayUtils.hpp"


using namespace std;
using namespace gpstk;
using namespace ValarrayUtils;

int main(int argc, char* argv[])
{
   try {

      ObsArray oa;

      cout << oa.add(RinexObsHeader::C1) << endl;
      cout << oa.add(RinexObsHeader::P2) << endl;
      cout << oa.add("C1-P2") << endl;
      cout << oa.add("P1-wl1*L1-1/(1-gamma)*(wl1*L1-wl2*L2)");
      
      cout << "There are " << oa.getNumObsTypes() << " obs indices." << endl;

      cout << "Reading input files." << endl;
//      oa.load("netrs027.06o.30s","netrs027.06n");
      oa.load("arl_256.06o","arl_256.06n");
      cout << "Done reading." << endl;

      cout << "Values directly using operator(size_t, size_t): " << endl;
      for (int j=0; j <oa.getNumObsTypes(); j++)
         for (int i=0; i<12; i++)
         {
            cout << "(" << i << "," << j << ")";
            cout << setprecision(12) << oa(i,j) << endl;
         }

      cout << "Getting values with a slice: " << endl;
      slice myslice(0,20,1);  
      valarray <double> copyObs = oa.observation[myslice];
      slice_array <double> obsSample = oa.observation[myslice];
      cout << "There are " << copyObs.size() << " elements in this slice." << endl;
      for (int i=0; i<20; i++)
         cout << copyObs[i] << endl;

      cout << "Operations on a slice: " << endl;
      cout << "Mean value is: " << copyObs.sum() / copyObs.size() << endl;
      //cout << "Mean value is: " << oa.observation[myslice].sum() / oa.observation[myslice].size() << endl;

      SatID thisPrn(9,SatID::systemGPS);
      valarray<bool> prnIdx = (oa.satellite==thisPrn);
      valarray<double> prnObs = oa.observation[prnIdx];
      valarray<CommonTime> prnTime = oa.epoch[prnIdx];
      cout << "Data for PRN 9:" << endl;
      for (int i=0; i<12; i++)
      {
         cout << prnTime[i].GPSfullweek() << " ";
         cout << prnTime[i].GPSsow() << " ";
         cout << prnObs[i] << endl;
      }
      
      set<CommonTime> allepochs = unique(oa.epoch);  
      cout << "Unique epochs:" << endl << allepochs << endl;
      
      set<SatID> allprns = unique(oa.satellite);
      cout << "Unique satellites: " << endl << allprns << endl;

      set<long> allpasses = unique(oa.pass);
      cout << "Unique passes: " << endl << allpasses << endl;
      
      exit(0);
   }
   catch(Exception& ex)
   {
      cerr << "Exception caught" << endl;
      cerr << ex << endl;
   }
}
