#pragma ident "$Id: $"

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

/**
 * @file ReadRawData.cpp
 * Read RINEX observation file data, all epochs and all files; part of program DDBase.
 */

//------------------------------------------------------------------------------------
// TD ReadRawData.cpp error msg for failure to open output RAW file
// TD ReadRawData.cpp is LastEpoch ever used?

//------------------------------------------------------------------------------------
// includes
// system
#include <fstream>

// GPSTk

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// local data
static DayTime EarliestTime;// earliest timetag among newly-input observation epochs
static int ngood;           // number of good data points, this epoch
static double sow;          // GPS seconds of week of current epoch
ofstream ofprs;             // output file for PRS solution - extern in ComputeRAIM..

//------------------------------------------------------------------------------------
// prototypes -- others
int OutputClockData(void);                                  // DataIO.cpp
int ReadNextObs(ObsFile& of);                               // ReadObsFiles.cpp
int ProcessRawData(ObsFile& obsfile, DayTime& timetag);     // ProcessRawData.cpp
// prototypes -- this module only
int FindEarliestTime(void);
void ComputeSolutionEpoch(void);

//------------------------------------------------------------------------------------
int ReadRawData(void)
{
try {
   int iret,nfile,ntotal;

   if(CI.Verbose) oflog << "BEGIN ReadRawData()" << endl;
   if(CI.Screen) cout << "Reading raw data and computing PR solution ..." << endl;

   if(!CI.OutputPRSFile.empty()) {
      ofprs.open(CI.OutputPRSFile.c_str(),ios::out);
      if(!ofprs) {
         // TD error msg
         CI.OutputPRSFile = string("");
      }
      else {
         oflog << "Opened file " << CI.OutputPRSFile << " for PRS output." << endl;
         ofprs << "# " << Title << endl;
         ofprs << "PRS site ns week  sec wk              dX(m)            dY(m)"
               << "            dZ(m)           clk(m)   rms(m) PRNs..."
               //<< "    (ret) Valid/NotValid"
               << endl;
      }
   }

      // loop over all epochs in all files
   do {

         // find earliest time among open, active files, and synchronize reading
      iret = FindEarliestTime();
      if(iret == 1) {
         if(CI.Debug) oflog << "End of data reached in ReadRawData." << endl;
         iret = 0;
         break;
      }
      if(iret == 2) {
         if(CI.Verbose) oflog << "After end time (quit) : "
            << EarliestTime.printf("%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
         iret = 0;
         break;
      }
      if(iret == 3) {
         if(CI.Debug) oflog << "Before begin time : "
            << EarliestTime.printf("%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
         iret = 0;
         continue;
      }

      if(CI.Debug) oflog << "Found " << ngood << " stations with data at epoch "
         << EarliestTime.printf("%Y/%m/%d %H:%M:%6.3f=%F/%10.3g") << endl;

         // round receiver epoch to even multiple of data interval, else even second
      ComputeSolutionEpoch();

         // preprocess at this epoch
      for(nfile=0; nfile<ObsFileList.size(); nfile++) {

            // skip files that are 'dead' or out of synch
         if(!ObsFileList[nfile].valid) continue;
         if(fabs(ObsFileList[nfile].Robs.time - EarliestTime) >= 0.5) continue;

            // process at the nominal receive time
         iret = ProcessRawData(ObsFileList[nfile],ObsFileList[nfile].Robs.time);
         if(iret) break;

      }  // end loop over observation files

   } while(iret == 0);       // end loop over all epochs

   if(iret) return iret;

   if(CI.Screen) cout << "Last  epoch is "
      << SolutionEpoch.printf("%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;
   if(CI.Verbose) oflog << "Last  epoch is "
      << SolutionEpoch.printf("%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;

      // was there any data?
   for(ntotal=0,nfile=0; nfile<ObsFileList.size(); nfile++) {
      //if(!ObsFileList[nfile].valid) continue;
      if(ObsFileList[nfile].nread <= 0)
         ObsFileList[nfile].valid = false;
      else
         ntotal += ObsFileList[nfile].nread;
   }
   if(CI.Verbose)
      oflog << "Total: " << ObsFileList.size() << " files, "
         << ntotal << " epochs were read." << endl;
   if(CI.Screen)
      cout << "Total: " << ObsFileList.size() << " files, "
         << ntotal << " epochs were read." << endl;

   if(ntotal == 0) {
      oflog << "No data found. Abort." << endl;
      if(CI.Screen)
         cout << "No data found. Abort." << endl;
      return -3;
   }

      // average PR solution
   {
      bool ok=true;
      map<string,Station>::const_iterator it;
      for(it=Stations.begin(); it != Stations.end(); it++) {
         Position& pos=Stations[it->first].pos;

         if(CI.Verbose)
            oflog << "For station " << it->first << " read "
               << it->second.PRSXstats.N() << " good data epochs." << endl;

         if(it->second.PRSXstats.N() <= 0) {
            oflog << "Warning - No good data found for station " << it->first << endl;
            ok = false;
            continue;
         }

         Position PRsol;
         PRsol.setECEF(it->second.PRSXstats.Average(),
                       it->second.PRSYstats.Average(),
                       it->second.PRSZstats.Average());
         if(CI.Verbose) {
            oflog << "Average PR solution for site " << it->first
               << fixed << setprecision(5)
               << " " << setw(15) << it->second.PRSXstats.Average()
               << " " << setw(15) << it->second.PRSYstats.Average()
               << " " << setw(15) << it->second.PRSZstats.Average()
               << endl;
            oflog << "Std-dev PR solution for site " << it->first
               << fixed << setprecision(5)
               << " " << setw(15) << it->second.PRSXstats.StdDev()
               << " " << setw(15) << it->second.PRSYstats.StdDev()
               << " " << setw(15) << it->second.PRSZstats.StdDev()
               << endl;
         }
         if(CI.Screen) {
            cout << "Average PR solution for site " << it->first
               << fixed << setprecision(5)
               << " " << setw(15) << it->second.PRSXstats.Average()
               << " " << setw(15) << it->second.PRSYstats.Average()
               << " " << setw(15) << it->second.PRSZstats.Average()
               << endl;
            cout << "Std-dev PR solution for site " << it->first
               << fixed << setprecision(5)
               << " " << setw(15) << it->second.PRSXstats.StdDev()
               << " " << setw(15) << it->second.PRSYstats.StdDev()
               << " " << setw(15) << it->second.PRSZstats.StdDev()
               << endl;
         }

            // use PR solution if apriori position not given
         if(it->second.usePRS) {
            pos = PRsol;
            oflog << "Adopting average pseudorange solution for "
               << it->first << " position"
               //<< pos.printf(" : %.4x %.4y %.4z meters ECEF")
               << endl;
            if(CI.Screen)
               cout << "Adopting average pseudorange solution for "
               << it->first << " position"
               //<< pos.printf(" : %.4x %.4y %.4z meters ECEF")
               << endl;
         }
         //else if(pos.getRadius() < 1.) {
         //}
         else {
            // sanity check...
            // keep this low! large position errors have enduring effects in editing!
            if(range(pos,PRsol) > 50.0) {
               oflog << "Warning - Pseudorange solution is far from input "
                  << "position for station " << it->first << " : delta = "
                  << fixed << setprecision(3) << range(pos,PRsol)
                  << " meters. Abort." << endl;
               cerr << "Warning - Pseudorange solution is far from input "
                  << "position for station " << it->first << " : delta = "
                  << fixed << setprecision(3) << range(pos,PRsol)
                  << " meters. Abort." << endl;
               iret = -1;
               OutputClockData();      // usually done in ClockModel() later...
            }
         }
      }

      if(!ok) {
         oflog << "One or more stations have no data. Abort." << endl;
         cerr << "One or more stations have no data. Abort." << endl;
         iret = -3;
      }
   }

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end ReadRawData()

//------------------------------------------------------------------------------------
// read the data for the next (earliest in future) observation epoch
int FindEarliestTime(void)
{
try {
   int iret,nfile;

   EarliestTime = DayTime(DayTime::END_OF_TIME);

      // loop over all (open) obs files
   for(nfile=0; nfile<ObsFileList.size(); nfile++) {

         // is this a valid, active file?
      if(!ObsFileList[nfile].valid) continue;

      iret = ReadNextObs(ObsFileList[nfile]);
      if(iret < 0) {            // error or EOF -- set file 'dead'
         ObsFileList[nfile].valid = false;
         continue;
      }
         // success - file is active
      else if(ObsFileList[nfile].Robs.time < EarliestTime)
         EarliestTime = ObsFileList[nfile].Robs.time;

   }  // end loop over all obs files

      // if no more data is available, EarliestTime will never get set
   if(EarliestTime == DayTime(DayTime::END_OF_TIME)) return 1;

      // if past end time, quit
   if(EarliestTime > CI.EndTime) return 2;

      // synchronize reading at EarliestTime
   for(ngood=0,nfile=0; nfile<ObsFileList.size(); nfile++) {
      if(!ObsFileList[nfile].valid) continue;
         // if this data time == EarliestTime, process and set flag to read again
      if(fabs(ObsFileList[nfile].Robs.time - EarliestTime) < 1.) {
         ngood++;
         ObsFileList[nfile].getNext = true;
      }
      else ObsFileList[nfile].getNext = false;
   }

      // apply time limits
   if(EarliestTime < CI.BegTime) return 3;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void ComputeSolutionEpoch(void)
{
try {
   double dt;

      // round receiver epoch to even multiple of data interval, else even second
   SolutionEpoch = EarliestTime;
   sow = SolutionEpoch.GPSsecond();
   sow = CI.DataInterval * double(int(sow/CI.DataInterval + 0.5));
   SolutionEpoch += (sow - SolutionEpoch.GPSsecond());
   if(CI.Debug) oflog << "Solution epoch is "
      << SolutionEpoch.printf("%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;

      // save first and last epochs
   if(fabs(FirstEpoch-DayTime::BEGINNING_OF_TIME) < 0.1) {
      FirstEpoch = SolutionEpoch;
      if(CI.Screen)
         cout << "First epoch is "
            << FirstEpoch.printf("%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;
      if(CI.Verbose)
         oflog << "First epoch is "
            << FirstEpoch.printf("%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;

         // compute rotation matrix that corrects for earth orientation
      //PNS = ident<double>(3);
   }     // end if first epoch

   LastEpoch = SolutionEpoch;    // TD use LastEpoch?

      // compute the current count
   dt = SolutionEpoch-FirstEpoch;
   Count = int(dt/CI.DataInterval + 0.5);

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
