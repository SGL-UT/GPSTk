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
#include "TimeString.hpp"
#include "Epoch.hpp"
#include "GPSWeekSecond.hpp"

// GPSTk

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// local data
static CommonTime EarliestTime;// earliest timetag among newly-input observation epochs
static int ngood;           // number of good data points, this epoch
static double sow;          // GPS seconds of week of current epoch
ofstream ofprs;             // output file for PRS solution
ofstream *pofs=NULL;        // pointer to output file stream (&ofprs)

//------------------------------------------------------------------------------------
// prototypes -- others
int OutputClockData(void) throw(Exception);              // DataOutput.cpp
int ReadNextObs(ObsFile& of) throw(Exception);           // ReadObsFiles.cpp
int ProcessRawData(ObsFile& obsfile, CommonTime& timetag, ofstream *pofs)
   throw(Exception);                                     // ProcessRawData.cpp
// prototypes -- this module only
int FindEarliestTime(void) throw(Exception);
void ComputeSolutionEpoch(void) throw(Exception);

//------------------------------------------------------------------------------------
int ReadAndProcessRawData(void) throw(Exception)
{
try {
   int iret,ntotal;
   size_t nfile;

   if(CI.Verbose) oflog << "BEGIN ReadAndProcessRawData()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;
   if(CI.Screen) cout << "Reading raw data and computing PR solution ..." << endl;

   if(!CI.OutputPRSFile.empty()) {
      ofprs.open(CI.OutputPRSFile.c_str(),ios::out);
      if(!ofprs.is_open()) {
         // TD error msg
         CI.OutputPRSFile = string("");
      }
      else {
         oflog << "Opened file " << CI.OutputPRSFile << " for PRS output." << endl;
         ofprs << "# " << Title << endl;
         ofprs << "PRS site ns week  sec wk              dX(m)            dY(m)"
               << "            dZ(m)           clk(m)   rms(m) slope PRNs..."
               //<< "    (ret) Valid/NotValid"
               << endl;
      }
      pofs = &ofprs;
   }

      // loop over all epochs in all files
   do {

         // find earliest time among open, active files, and synchronize reading
      iret = FindEarliestTime();
      if(iret == 1) {
         if(CI.Debug) oflog << "End of data reached in ReadAndProcessRawData."
            << endl;
         iret = 0;
         break;
      }
      if(iret == 2) {
         if(CI.Verbose) oflog << "After end time (quit) : "
            << printTime(EarliestTime,"%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
         iret = 0;
         break;
      }
      if(iret == 3) {
         if(CI.Debug) oflog << "Before begin time : "
            << printTime(EarliestTime,"%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
         iret = 0;
         continue;
      }

      if(CI.Debug) oflog << "Found " << ngood << " stations with data at epoch "
         << printTime(EarliestTime,"%Y/%m/%d %H:%M:%6.3f=%F/%10.3g") << endl;

         // round receiver epoch to even multiple of data interval, else even second
      ComputeSolutionEpoch();

         // preprocess at this epoch
      for(nfile=0; nfile<ObsFileList.size(); nfile++) {

            // skip files that are 'dead' or out of synch
         if(!ObsFileList[nfile].valid) continue;
         if(fabs(ObsFileList[nfile].Robs.time - EarliestTime) >= 0.5) continue;

            // process at the nominal receive time
         iret = ProcessRawData(ObsFileList[nfile],ObsFileList[nfile].Robs.time,pofs);
         if(iret) break;

      }  // end loop over observation files

   } while(iret == 0);       // end loop over all epochs

   if(!CI.OutputPRSFile.empty()) ofprs.close();

   if(iret) return iret;

   if(CI.Screen) cout << "Last  epoch is "
      << printTime(SolutionEpoch,"%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;
   if(CI.Verbose) oflog << "Last  epoch is "
      << printTime(SolutionEpoch,"%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;

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
               //<< pos.printf(" : %.4x %.4y %.4z meters Position")
               << endl;
            if(CI.Screen)
               cout << "Adopting average pseudorange solution for "
               << it->first << " position"
               //<< pos.printf(" : %.4x %.4y %.4z meters Position")
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
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end ReadAndProcessRawData()

//------------------------------------------------------------------------------------
// read the data for the next (earliest in future) observation epoch
int FindEarliestTime(void) throw(Exception)
{
try {
   int iret;
   size_t nfile;

   EarliestTime = CommonTime::END_OF_TIME;

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
   if(EarliestTime == CommonTime::END_OF_TIME) return 1;

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
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void ComputeSolutionEpoch(void) throw(Exception)
{
try {
   double dt;

      // round receiver epoch to even multiple of data interval, else even second
   SolutionEpoch = EarliestTime;
   sow = static_cast<GPSWeekSecond>(SolutionEpoch).sow;
   sow = CI.DataInterval * double(int(sow/CI.DataInterval + 0.5));
   SolutionEpoch += (sow - static_cast<GPSWeekSecond>(SolutionEpoch).sow);
   if(CI.Debug) oflog << "Solution epoch is "
      << printTime(SolutionEpoch,"%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;

      // save first and last epochs
   if(fabs(FirstEpoch-CommonTime::BEGINNING_OF_TIME) < 0.1) {
      FirstEpoch = SolutionEpoch;
      if(CI.Screen)
         cout << "First epoch is "
            << printTime(FirstEpoch,"%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;
      if(CI.Verbose)
         oflog << "First epoch is "
            << printTime(FirstEpoch,"%Y/%02m/%02d %2H:%02M:%6.3f = %F/%10.3g") << endl;

         // compute rotation matrix that corrects for earth orientation
      //PNS = ident<double>(3);
   }     // end if first epoch

   LastEpoch = SolutionEpoch;    // TD use LastEpoch?

      // compute the current count
   dt = SolutionEpoch-FirstEpoch;
   Count = int(dt/CI.DataInterval + 0.5);

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
