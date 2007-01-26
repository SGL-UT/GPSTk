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

/**
 * @file NavMerge.cpp
 * Read, summarize and optionally merge, Rinex navigation files.
 * NavMerge will merge any number of Rinex nav files into a unique superset,
 * and either write them out to a new Rinex file (if an output file is given),
 * or write a summary of the data to the screen. NavMerge also finds
 * and fixes full week number when it is inconsistent with epoch.
 */

//------------------------------------------------------------------------------------
#include <string>
#include <vector>

#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavStream.hpp"
#include "BCEphemerisStore.hpp"

//------------------------------------------------------------------------------------

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
void BadArg(string& arg) { cout << "Error: nothing follows option " << arg << endl; }

//------------------------------------------------------------------------------------
// Returns 0 when successful.
int main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout <<
"Usage: NavMerge [options] <RINEX nav file name(s)>\n"
"  Options are:\n"
"    [-o|--out] <file>     Output RINEX navigation file name. If omitted, a data summary is displayed.\n"
"    [-tb|--begTime] <tb>  Output data only if epoch is within 4 hours of the interval (tb,te).\n"
"    [-te|--endTime] <te>    If one of (te,tb) is omitted, they are made equal.\n"
"                            Times are either 'year,mon,day,hr,min,sec' or 'GPSweek,secOfWeek'\n"
"  NB. NavMerge corrects data for output when GPS full week number is inconsistent with epoch time.\n"
      ;

      return -1;
   }

   try
   {
      int i;
      string arg,filename,outfile,YMDformat("%Y,%m,%d,%H,%M,%f"),GPSformat("%F,%g");
      DayTime tb,te;

      te = tb = DayTime::BEGINNING_OF_TIME;

      i = 1;
      while(i < argc) {
         arg = string(argv[i]);
         if(arg == "--out" || arg.substr(0,2) == "-o") {
            if(arg == "--out") {
               argv[i][0] = '\0';
               if(++i == argc) { BadArg(arg); break; }
               outfile = string(argv[i]);
            }
            else
               outfile = arg.substr(2);
            cout << "Output file name is " << outfile << endl;
            argv[i][0] = '\0';
         }
         else if(arg == "--begTime" || arg == "-tb" || arg.substr(0,3) == "-tb") {
            if(arg.substr(0,3) == "-tb" && arg.size() > 3)
               arg = arg.substr(3);
            else {
               argv[i][0] = '\0';
               if(++i == argc) { BadArg(arg); break; }
               arg = string(argv[i]);
            }

            if(numWords(arg,',') == 2)
               tb.setToString(arg,GPSformat);
            else if(numWords(arg,',') == 6)
               tb.setToString(arg,YMDformat);
            else
               cout << "Unable to understand timetag option: " << arg << endl;
            argv[i][0] = '\0';
         }
         else if(arg == "--endTime" || arg == "-te" || arg.substr(0,3) == "-te") {
            if(arg.substr(0,3) == "-te" && arg.size() > 3)
               arg = arg.substr(3);
            else {
               argv[i][0] = '\0';
               if(++i == argc) { BadArg(arg); break; }
               arg = string(argv[i]);
            }

            if(numWords(arg,',') == 2)
               te.setToString(arg,GPSformat);
            else if(numWords(arg,',') == 6)
               te.setToString(arg,YMDformat);
            else
               cout << "Unable to understand timetag option: " << arg << endl;
            argv[i][0] = '\0';
         }

         i++;
      }

      if(te != DayTime::BEGINNING_OF_TIME &&
         tb == DayTime::BEGINNING_OF_TIME) tb = te;
      else
      if(tb != DayTime::BEGINNING_OF_TIME &&
         te == DayTime::BEGINNING_OF_TIME) te = tb;
      if(tb > te) { DayTime tt=tb; tb=te; te=tt; }

      if(tb != DayTime::BEGINNING_OF_TIME)
         cout << "Time limits are " << tb.printf(YMDformat)
               << " - " << te.printf(YMDformat) << endl;

      RinexNavHeader rnh,rnhout;
      RinexNavData rne;
      BCEphemerisStore EphStore;
      RinexNavStream RNFileOut;

      if(outfile != string("")) {
         RNFileOut.open(outfile.c_str(),ios::out);
         RNFileOut.exceptions(fstream::failbit);
         rnhout.version = 2.1;
         rnhout.valid |= RinexNavHeader::versionValid;
         rnhout.fileType = string("NAVIGATION");
         rnhout.fileProgram = string("NavMerge");
         rnhout.fileAgency = string("ARL:UT/SGL/GPSTK");
         rnhout.valid |= RinexNavHeader::runByValid;
         rnhout.commentList.clear();
         rnhout.valid |= RinexNavHeader::commentValid;
         rnhout.valid |= RinexNavHeader::endValid;
      }

      int na=1,n=0,nf;
      while(na < argc) {
         filename = string(argv[na]);
         if(filename == string("")) { na++; continue; }

         try {
            RinexNavStream RNFileIn(filename.c_str());
            if(!RNFileIn) {
               cout << "Could not open file " << filename << endl;
               na++;
               continue;
            }
            RNFileIn.exceptions(fstream::failbit);
      
            RNFileIn >> rnh;
            if(rnh.valid & RinexNavHeader::ionAlphaValid) {
               for(i=0; i<4; i++) rnhout.ionAlpha[i]=rnh.ionAlpha[i];
               rnhout.valid |= RinexNavHeader::ionAlphaValid;
            }
            if(rnh.valid & RinexNavHeader::ionBetaValid) {
               for(i=0; i<4; i++) rnhout.ionBeta[i]=rnh.ionBeta[i];
               rnhout.valid |= RinexNavHeader::ionBetaValid;
            }
            if(rnh.valid & RinexNavHeader::deltaUTCValid) {
               rnhout.A0 = rnh.A0;
               rnhout.A1 = rnh.A1;
               rnhout.UTCRefWeek = rnh.UTCRefWeek;
               rnhout.UTCRefTime = rnh.UTCRefTime;
               rnhout.valid |= RinexNavHeader::deltaUTCValid;
            }
            if(rnh.valid & RinexNavHeader::leapSecondsValid) {
               rnhout.leapSeconds = rnh.leapSeconds;
               rnhout.valid |= RinexNavHeader::leapSecondsValid;
            }

            nf = 0;
            while (RNFileIn >> rne)
            {
               nf++;
               n++;
               // check that week number (associated with HOW) is consistent with TOC.
               // (NB. in Rinex nav file, the week number is associated with the TOE;
               // RinexNavData converts it to associate with the HOW)
               int wkTOC,wk;
               wk = rne.weeknum;                // 'weeknum' associated with HOW
               wkTOC = rne.time.GPSfullweek();  // 'time' comes from epoch line
               if(ABS(wk-wkTOC) > 1) {          // HOW and TOC should be w/in 1 week
                  double dt = double(wk-wkTOC)/1024.0;
                  dt += (dt < 0.0 ? -0.5 : 0.5);
                  wk -= int(dt) * 1024;
                  if(ABS(wk-wkTOC) > 1) {
                     cout << "WARNING: Ephemeris in " << filename
                        << " for satellite G"
                        << setw(2) << setfill('0') << rne.PRNID << setfill(' ')
                        << " at time " << rne.time
                        << " has inconsistent week number " << rne.weeknum << endl;
                  }
                  else {
                     cout << "NavMerge corrected the week in G"
                        << setw(2) << setfill('0') << rne.PRNID << setfill(' ')
                        << " " << rne.time
                        << " " << filename << endl;
                     rne.weeknum = wk;
                  }
               }
                  // if healthy, add to the store
               if(rne.health == 0) EphStore.addEphemeris(rne);
            }
         }
         catch(Exception& e) {
            cout << "Exception: " << e << endl;
         }
         na++;
         cout << "Read " << setw(4) << nf << " ephemerides from file "
            << filename << endl;
      }
      cout << "Read " << setw(4) << n << " total ephemerides." << endl;

         // pull out all the ephemerides
      list<EngEphemeris> EphList;
      i = EphStore.addToList(EphList);

      if(outfile != string("")) {
            // write the output header
         RNFileOut << rnhout;

            // write out all the ephemerides
         list<EngEphemeris>::iterator it=EphList.begin();
         n=0;
         while(it != EphList.end()) {
            rne = RinexNavData(*it);
            if(tb == DayTime::BEGINNING_OF_TIME ||
               (rne.time - tb > -14400.0 && rne.time - te < 14400.0))
            {
               n++;
               RNFileOut << rne;
            }
            it++;
         }
         cout << "Wrote " << setw(3) << n << " unique ephemerides to file "
            << outfile << endl;
      }
      else {
         EphStore.dump(1);
      }

      return 0;
   }
   catch(Exception& e) { cout << e; }
   catch (...) { cout << "unknown error.  Done." << endl; }
   return 1;

   return 0;
}
