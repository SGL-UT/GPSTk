#pragma ident "$Id$"


/**
 * @file NavMerge.cpp
 * Read, summarize and optionally merge, Rinex navigation files.
 * NavMerge will merge any number of Rinex nav files into a unique superset,
 * and either write them out to a new Rinex file (if an output file is given),
 * or write a summary of the data to the screen. NavMerge also finds
 * and fixes full week number when it is inconsistent with epoch.
 */

//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavStream.hpp"
#include "BCEphemerisStore.hpp"
#include "DayTime.hpp"

//------------------------------------------------------------------------------------

using namespace std;

// Returns 0 when successful.
int main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout
         << "Usage: NavMerge [options] <Rinex nav file(s)...>\n"
         << "  Options are:\n"
         << "   -o<file>       Write all data to an output Rinex nav file.\n"
         << "                    If omitted, a data summary is written to the screen.\n"
         << "   -tb<timetag>   Output only if epoch is within 4 hours of the interval (tb,te).\n"
         << "   -te<timetag>     If te or tb is missing, they are made equal.\n"
         << "                    Timetags have the form year,mon,day,HH,min,sec OR GPSweek,sow\n"
         << "  NavMerge will also correct the output data when the GPS full week number is\n"
         << "    inconsistent with the epoch time.\n";
      return -1;
   }

   try
   {
      int i;
      string filename,outfile,YMDformat("%Y,%m,%d,%H,%M,%f"),GPSformat("%F,%g");
      gpstk::DayTime tb,te;
      te = tb = gpstk::DayTime::BEGINNING_OF_TIME;

      i = 1;
      while(i < argc) {
         filename = argv[i];        // filename is used as a temp here
         if(filename[0]=='-' && (filename[1]=='o' || filename[1]=='O')) {
            outfile = filename.substr(2);
            cout << "Output file name is " << outfile << endl;
            argv[i][0] = '\0';
         }
         else if(filename[0]=='-' && (filename[1]=='t' || filename[1]=='T')) {
            if(filename[2]=='b' || filename[2]=='B') {
               filename = filename.substr(3);
               if(gpstk::StringUtils::numWords(filename,',') == 2)
                  tb.setToString(filename,GPSformat);
               else if(gpstk::StringUtils::numWords(filename,',') == 6)
                  tb.setToString(filename,YMDformat);
               else
                  cout << "Unable to understand timetag option: " << argv[i] << endl;
            }
            else if(filename[2]=='e' || filename[2]=='E') {
               filename = filename.substr(3);
               if(gpstk::StringUtils::numWords(filename,',') == 2)
                  te.setToString(filename,GPSformat);
               else if(gpstk::StringUtils::numWords(filename,',') == 6)
                  te.setToString(filename,YMDformat);
               else
                  cout << "Unable to understand timetag option: " << argv[i] << endl;
            }
            argv[i][0] = '\0';
         }
         i++;
      }
      if(te != gpstk::DayTime::BEGINNING_OF_TIME &&
         tb == gpstk::DayTime::BEGINNING_OF_TIME) tb = te;
      else
      if(tb != gpstk::DayTime::BEGINNING_OF_TIME &&
         te == gpstk::DayTime::BEGINNING_OF_TIME) te = tb;
      if(tb > te) { gpstk::DayTime tt=tb; tb=te; te=tt; }

      gpstk::RinexNavHeader rnh,rnhout;
      gpstk::RinexNavData rne;
      gpstk::BCEphemerisStore EphStore;
      gpstk::RinexNavStream RNFileOut;

      if(outfile != string("")) {
         RNFileOut.open(outfile.c_str(),ios::out);
         RNFileOut.exceptions(fstream::failbit);
         rnhout.version = 2.11;
         rnhout.valid |= gpstk::RinexNavHeader::versionValid;
         rnhout.fileType = string("NAVIGATION");
         rnhout.fileProgram = string("NavMerge");
         rnhout.fileAgency = string("GPSTK");
         rnhout.valid |= gpstk::RinexNavHeader::runByValid;
         rnhout.commentList.clear();
         rnhout.valid |= gpstk::RinexNavHeader::commentValid;
         rnhout.valid |= gpstk::RinexNavHeader::endValid;
      }

      int na=1,n=0,nf;
      while(na < argc) {
         filename = argv[na];
         if(filename == string("")) { na++; continue; }

         try {
            gpstk::RinexNavStream RNFileIn(filename.c_str());
            if(!RNFileIn) {
               cout << "Could not open file " << filename << endl;
               na++;
               continue;
            }
            RNFileIn.exceptions(fstream::failbit);
      
            RNFileIn >> rnh;
            if(rnh.valid & gpstk::RinexNavHeader::ionAlphaValid) {
               for(i=0; i<4; i++) rnhout.ionAlpha[i]=rnh.ionAlpha[i];
               rnhout.valid |= gpstk::RinexNavHeader::ionAlphaValid;
            }
            if(rnh.valid & gpstk::RinexNavHeader::ionBetaValid) {
               for(i=0; i<4; i++) rnhout.ionBeta[i]=rnh.ionBeta[i];
               rnhout.valid |= gpstk::RinexNavHeader::ionBetaValid;
            }
            if(rnh.valid & gpstk::RinexNavHeader::deltaUTCValid) {
               rnhout.A0 = rnh.A0;
               rnhout.A1 = rnh.A1;
               rnhout.UTCRefWeek = rnh.UTCRefWeek;
               rnhout.UTCRefTime = rnh.UTCRefTime;
               rnhout.valid |= gpstk::RinexNavHeader::deltaUTCValid;
            }
            if(rnh.valid & gpstk::RinexNavHeader::leapSecondsValid) {
               rnhout.leapSeconds = rnh.leapSeconds;
               rnhout.valid |= gpstk::RinexNavHeader::leapSecondsValid;
            }

            nf = 0;
            while (RNFileIn >> rne)
            {
               nf++;
               n++;
                  // check that week number (associated with HOW)
                  // is consistent with TOC.
                  // (NB. in Rinex nav file, the week number is
                  // associated with the TOE; gpstk::RinexNavData
                  // converts it to associate with the HOW)
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
         catch(gpstk::Exception& e) {
            cout << "Exception: " << e << endl;
         }
         na++;
         cout << "Read " << setw(4) << nf << " ephemerides from file "
            << filename << endl;
      }
      cout << "Read " << setw(4) << n << " total ephemerides." << endl;

         // pull out all the ephemerides
      list<gpstk::EngEphemeris> EphList;
      i = EphStore.addToList(EphList);

      if(outfile != string("")) {
            // write the output header
         RNFileOut << rnhout;

            // write out all the ephemerides
         list<gpstk::EngEphemeris>::iterator it=EphList.begin();
         n=0;
         while(it != EphList.end()) {
            rne = gpstk::RinexNavData(*it);
            if(tb == gpstk::DayTime::BEGINNING_OF_TIME ||
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
   catch(gpstk::Exception& e)
   {
      cout << e;
      return 1;
   }
   catch (...)
   {
      cout << "unknown error.  Done." << endl;
      return 1;
   }

   return 0;
}
