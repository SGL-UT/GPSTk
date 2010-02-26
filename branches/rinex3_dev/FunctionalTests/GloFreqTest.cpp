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

int main(int argc, char *argv[])
{
   GloFreqIndex glo;
   vector<int> numPassesStart(24), numPassesEnd(24);
   cout << endl;

   if (argc == 1)
   {
      cout << "Testing knownIndex() functionality (command options ignored)." << endl;

      int ierr1, ierr2, ierr3;
      ierr1 = 0;
      ierr2 = 0;

      // For testing, fill map with known frequency index data.
      glo.knownIndex();

      cout << endl << "SVID v. frequency index known as of Jan. 2010:" << endl << endl;
      cout << "SVID  index   G1         err   G2         err   G3         err" << endl;

      // Loop over the 24 GLONASS SVs.
      for (int i = 1; i <= 24; i++)
      {
         RinexSatID id = RinexSatID(i,SatID::systemGlonass);

         double freq1 = glo.getFreqTruth(id,1,ierr1); // G1
         double freq2 = glo.getFreqTruth(id,2,ierr2); // G2
         double freq3 = glo.getFreqTruth(id,3,ierr3); // fake G3, to test error-handling

         cout << "R" << setw(2) << setfill('0') << i << "   "
              << setw(4) << setfill(' ') << glo.getIndex(id) << "    "
              << fixed << setprecision(4) << setw(9) << setfill(' ')
              << freq1 << "  " << ierr1 << "     "
              << fixed << setprecision(4) << setw(9) << setfill(' ')
              << freq2 << "  " << ierr2 << "     "
              << fixed << setprecision(4) << setw(9) << setfill(' ')
              << freq3 << "  " << ierr3 << endl;
      }

   }
   else if (argc <= 3)
   {
      cout << endl << endl
           << "Reading data from RINEX 2 Obs file " << argv[1]
           << " to test algorithmic determination:"
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

      vector<SatPass> list, copylist, sortedlist;

      // Read in data from a known RINEX 2 file.
      cout << "Input RINEX 2 Obs filename: " << argv[1] << endl << endl;
      vector<std::string> filenames;
      filenames.push_back(argv[1]);
      int iread = SatPassFromRinexFiles(filenames,obsTypes,30.0,list);
      if (iread == 0)
      {
         cout << "No files read -- SatPassFromRinexFiles returns value 0 -- exiting." << endl;
         exit(-1);
      }

      // Output basic info on the passes.
      cout << endl << "Satellite passes present:" << endl;
      for (int i = 0; i < list.size(); i++)
      {
         if (list[i].getSat().systemChar() == 'R')
         {
            numPassesStart[list[i].getSat().id] += 1;
            cout << "   " << list[i].getSat().toString()
                 << "   start: " << CivilTime(list[i].getFirstGoodTime())
                 << "   end: "   << CivilTime(list[i].getLastGoodTime())
                 << endl;
         }
      }
      cout << endl;

      // Copy the list for sorting.
      copylist = list;

      // Sort the passes by GNSS & SV ID. [GPS]
      for (int i = 1; i <= 32; i++) // GPS SVs
      {
         cout << endl;
         GSatID satid(i,GSatID::systemGPS);
         cout << "Constructed GSatID " << satid << endl;
         vector<SatPass>::iterator it = copylist.begin();
         while (it != copylist.end())
         {
            if ((*it).getSat() == satid)
            {
               cout << "Adding SatPass to sortedlist." << endl;
               sortedlist.push_back(*it);
               copylist.erase(it);
               it--;
            }
            it++;
         }
         cout << "Finished GPS while() loop." << endl;
      }

      // Output basic info on the sorted passes.
      cout << endl << "Satellite passes present:" << endl;
      for (int i = 0; i < sortedlist.size(); i++)
      {
         cout << "   " << sortedlist[i].getSat().toString()
              << "   start: " << CivilTime(sortedlist[i].getFirstGoodTime())
              << "   end: "   << CivilTime(sortedlist[i].getLastGoodTime())
              << endl;
      }
      cout << endl;

      // Sort the passes by GNSS & SV ID. [GLO]
      for (int i = 1; i <= 24; i++) // GLONASS SVs
      {
         cout << endl;
         GSatID satid(i,GSatID::systemGlonass);
         cout << "Constructed GSatID " << satid << endl;
         vector<SatPass>::iterator it = copylist.begin();
         while (it != copylist.end())
         {
            if ((*it).getSat() == satid)
            {
               cout << "Adding SatPass to sortedlist." << endl;
               sortedlist.push_back(*it);
               copylist.erase(it);
               it--;
            }
            it++;
         }
         cout << "Finished GLO while() loop." << endl;
      }

      // Output basic info on the sorted passes.
      cout << endl << "Satellite passes present:" << endl;
      for (int i = 0; i < sortedlist.size(); i++)
      {
         cout << "   " << sortedlist[i].getSat().toString()
              << "   start: " << CivilTime(sortedlist[i].getFirstGoodTime())
              << "   end: "   << CivilTime(sortedlist[i].getLastGoodTime())
              << endl;
      }
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
            if (list[i].getSat().systemChar() == 'R'
                && list[i].data(j,"EL") > 15.0      )
            {
               r1.push_back(list[i].data(j,"P1"));
               r2.push_back(list[i].data(j,"P2"));
               p1.push_back(list[i].data(j,"L1"));
               p2.push_back(list[i].data(j,"L2"));
            }
         }

         int j = glo.addPass(list[i].getSat(),
                             CommonTime(list[i].getFirstGoodTime()),
                             r1,p1,r2,p2,1);

         if (j == 0)
            numPassesEnd[list[i].getSat().id] += 1;
      }
      cout << endl;
      cout << "Summary of satellite passes:" << endl;
      cout << "  SV ID   # in   # kept # fail" << endl;
      int countIn(0),countOut(0);
      for (int i = 1; i <= 24; i++) // SVs numbered 1..24
      {
         countIn  += numPassesStart[i];
         countOut += numPassesEnd[i];
         cout << "  "    << setw(2) << setfill('0') << i
              << "     " << setw(2) << setfill(' ') << numPassesStart[i]
              << "     " << setw(2) << setfill(' ') << numPassesEnd[i]
              << "     " << setw(2) << setfill(' ') << numPassesStart[i]-numPassesEnd[i]
              << endl;
      }
      cout << "  total   "
           << setw(2) << setfill(' ') << countIn  << "     "
           << setw(2) << setfill(' ') << countOut << "     "
           << setw(2) << setfill(' ') << countIn-countOut << endl;

      glo.calcIndex();

      cout << endl << "SV ID   index" << endl;

      // Output the final results in SV order.

      for (int i = 1; i <= 24; i++)
      {
         RinexSatID id(i,SatID::systemGlonass);
         cout << id << "     " << setw(4) << setfill(' ')
              << glo.getIndex(id) // lookup method
              << endl;
      }
      cout << endl;

      glo.dump(cout);
   }
   else
   {
      cout << "GloFreqTest infile [outfile]" << endl << endl;
      exit(-1);
   }

   exit(0);
}

