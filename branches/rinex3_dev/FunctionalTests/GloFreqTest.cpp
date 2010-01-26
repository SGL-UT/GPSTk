#pragma ident "$Id: GloFreqNo.cpp 1709 2009-02-18 20:27:47Z btolman $"

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

//
// Tests the GLONASS frequency number singleton.
//

#include <iostream>
#include <iomanip>

#include "CivilTime.hpp"
#include "GloFreqIndex.hpp"
#include "SatPass.hpp"

using namespace std;
using namespace gpstk;

int main()
{
   int ierr1, ierr2, ierr3;
   ierr1 = 0;
   ierr2 = 0;

   GloFreqIndex glo;

   // For testing, fill map with known frequency index data.
   glo.knownIndex();

   cout << endl << "SVID v. frequency index known as of Jan. 2010:" << endl << endl;
   cout << "SVID  index   G1         err   G2         err   G3         err" << endl;

   // Loop over the 24 GLONASS SVs.
   for (int i = 1; i <= 24; i++)
   {
      RinexSatID id = RinexSatID(i,SatID::systemGlonass);

      double freq1 = glo.getGloFreq(id,1,ierr1); // G1
      double freq2 = glo.getGloFreq(id,2,ierr2); // G2
      double freq3 = glo.getGloFreq(id,3,ierr3); // fake G3, to test error-handling

      cout << "R" << setw(2) << setfill('0') << i << "   "
           << setw(4) << setfill(' ') << glo.getGloIndex(id) << "    "
           << fixed << setprecision(4) << setw(9) << setfill(' ')
           << freq1 << "  " << ierr1 << "     "
           << fixed << setprecision(4) << setw(9) << setfill(' ')
           << freq2 << "  " << ierr2 << "     "
           << fixed << setprecision(4) << setw(9) << setfill(' ')
           << freq3 << "  " << ierr3 << endl;
   }

   cout << endl << endl
        << "Now reading data from RINEX 2 file to test algorithmic determination:"
        << endl << endl;

   vector<double> r1, r2, p1, p2;

   vector<std::string> obsTypes;
   obsTypes.push_back("C1");
   obsTypes.push_back("P1");
   obsTypes.push_back("P2");
   obsTypes.push_back("L1");
   obsTypes.push_back("L2");
   obsTypes.push_back("EL");
   obsTypes.push_back("AZ");

   vector<SatPass> list;

   // Read in data from a known RINEX 2 file.
   std::string filename = "ARL82620.09O_RF";
   cout << "Filename: " << filename << endl << endl;
   vector<std::string> filenames;
   filenames.push_back(filename);
   int iread = SatPassFromRinexFiles(filenames,obsTypes,30.0,list);

   cout << endl << "Satellite passes present:" << endl;
   for (int i = 0; i < list.size(); i++)
      cout << "   " << list[i].getSat().toString()
           << "   start: " << CivilTime(list[i].getFirstGoodTime())
           << "   end: "   << CivilTime(list[i].getLastGoodTime())
           << endl;
   cout << endl;

   for (int i = 0; i < list.size(); i++)
   {
      // Clear the vectors first.
      r1.clear();
      r2.clear();
      p1.clear();
      p2.clear();

      // Refactor the data vectors.
      for (int j = 0; j < list[i].size(); j++)
      {
         // Check for min. elevation angle before accepting.
         // Use 15 degrees, per BT.
         if (list[i].data(j,"EL") > 15.0)
         {
            r1.push_back(list[i].data(j,"P1"));
            r2.push_back(list[i].data(j,"P2"));
            p1.push_back(list[i].data(j,"L1"));
            p2.push_back(list[i].data(j,"L2"));
         }
      }

      glo.addPass(list[i].getSat(),CommonTime(list[i].getFirstGoodTime()),r1,p1,r2,p2);
   }

   int calccheck = glo.calcIndex();

   cout << "calcIndex return value = " << calccheck << endl << endl;

   cout << endl << "SV ID   index" << endl;

   // Output the final results in SV order.
   for (int i = 1; i <= 24; i++)
   {
      RinexSatID id(i,SatID::systemGlonass);
      cout << id << "     " << setw(4) << setfill(' ')
           << glo.getGloIndex(id) // lookup method
           << endl;
   }

   cout << endl;

   glo.dump(cout);

   exit(0);
}

