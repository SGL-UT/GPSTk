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
 * @file ReadObsFiles.cpp
 * Open and read input RINEX observation files; part of program DDBase.
 */

//------------------------------------------------------------------------------------
// system includes

// GPSTk

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- this module only
void ReadAllObsHeaders(void);
int ReadNextObs(ObsFile& of);

//------------------------------------------------------------------------------------
void ReadAllObsHeaders(void)
{
try {
   int i,j;
   if(CI.Verbose) oflog << "BEGIN ReadAllObsHeaders()" << endl;

      // open all obs files and read headers
   for(i=0; i<ObsFileList.size(); i++) {
      string filename;

         // initialize -- this will mark unopened file
      ObsFileList[i].ins.close();         // just in case
      ObsFileList[i].ins.clear();         // just in case
      ObsFileList[i].nread = -1;
      ObsFileList[i].valid = false;

         // filename
      if(!CI.InputPath.empty()) filename = CI.InputPath + "/";
      filename += ObsFileList[i].name;

         // open
      ObsFileList[i].ins.open(filename.c_str(),ios_base::in);
      if(!ObsFileList[i].ins) {
         oflog << "Warning: File " << filename
            << " could not be opened. Ignore." << endl;
         if(CI.Screen) cout << "Warning: File " << filename
               << " could not be opened. Ignore." << endl;
         continue;
      }
      ObsFileList[i].ins.exceptions(ios::failbit);

         // read header
      try {
         ObsFileList[i].ins >> ObsFileList[i].Rhead;
      }
      catch(FFStreamError& e) {
         cerr << "Caught an FFStreamError while reading header of input obs file "
            << filename << " :\n" << e.getText(0) << endl;
         ObsFileList[i].ins.close();
         continue;
      }
      catch(Exception& e) {
         cerr << "Caught a gpstk exception while reading header of input obs file "
            << filename << " :\n" << e.getText(0) << endl;
         ObsFileList[i].ins.close();
         continue;
      }

         // check that file contains C1/P1,P2,L1,L2
      ObsFileList[i].inP1 = -1;
      ObsFileList[i].inP2 = -1;
      ObsFileList[i].inL1 = -1;
      ObsFileList[i].inL2 = -1;
      ObsFileList[i].inC1 = -1;
      ObsFileList[i].inD1 = -1;
      ObsFileList[i].inD2 = -1;
      ObsFileList[i].inS1 = -1;
      ObsFileList[i].inS2 = -1;
      for(j=0; j<ObsFileList[i].Rhead.obsTypeList.size(); j++) {
         if(ObsFileList[i].Rhead.obsTypeList[j]==RinexObsHeader::convertObsType("C1"))
            ObsFileList[i].inC1 = j;
         if(ObsFileList[i].Rhead.obsTypeList[j]==RinexObsHeader::convertObsType("L1"))
            ObsFileList[i].inL1 = j;
         if(ObsFileList[i].Rhead.obsTypeList[j]==RinexObsHeader::convertObsType("L2"))
            ObsFileList[i].inL2 = j;
         if(ObsFileList[i].Rhead.obsTypeList[j]==RinexObsHeader::convertObsType("P1"))
            ObsFileList[i].inP1 = j;
         if(ObsFileList[i].Rhead.obsTypeList[j]==RinexObsHeader::convertObsType("P2"))
            ObsFileList[i].inP2 = j;
         if(ObsFileList[i].Rhead.obsTypeList[j]==RinexObsHeader::convertObsType("D1"))
            ObsFileList[i].inD1 = j;
         if(ObsFileList[i].Rhead.obsTypeList[j]==RinexObsHeader::convertObsType("D2"))
            ObsFileList[i].inD2 = j;
         if(ObsFileList[i].Rhead.obsTypeList[j]==RinexObsHeader::convertObsType("S1"))
            ObsFileList[i].inS1 = j;
         if(ObsFileList[i].Rhead.obsTypeList[j]==RinexObsHeader::convertObsType("S2"))
            ObsFileList[i].inS2 = j;
      }

      ObsFileList[i].nread = 0;
      ObsFileList[i].valid = true;
      ObsFileList[i].getNext = true;

      if(CI.Verbose) oflog << endl;
      oflog << "Opened and read header of observation file: " << filename << endl;
      if(CI.Screen)
         cout << "Opened and read header of observation file: " << filename << endl;
      if(CI.Verbose) ObsFileList[i].Rhead.dump(oflog);
      
         // prepare for RAIM solution
      Stations[ObsFileList[i].label].PRS.RMSLimit = CI.PRSrmsLimit;
      Stations[ObsFileList[i].label].PRS.Algebraic = CI.PRSalgebra;
      Stations[ObsFileList[i].label].PRS.MaxNIterations = CI.PRSnIter;
      Stations[ObsFileList[i].label].PRS.Convergence = CI.PRSconverge;
      Stations[ObsFileList[i].label].PRS.Solution.resize(4);
      Stations[ObsFileList[i].label].PRSXstats.Reset();
      Stations[ObsFileList[i].label].PRSYstats.Reset();
      Stations[ObsFileList[i].label].PRSZstats.Reset();
      if(CI.Debug) {
         Stations[ObsFileList[i].label].PRS.Debug = true;
         Stations[ObsFileList[i].label].PRS.pDebugStream = &oflog;
      }

   }  // end loop over input observation files
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int ReadNextObs(ObsFile& of)
{
try {
      // read the next observation epoch
      // decimate to even multiples of DataInterval
   while(1) {
      try {
         if(CI.Debug) oflog << "ReadNextObs for file " << of.name << endl;
         if(!of.getNext) return 1;
         of.ins >> of.Robs;
      }
      catch(FFStreamError& e) {
         if(CI.Verbose)
            oflog << "ReadNextObs caught an FFStreamError while reading obs in file "
               << of.name << " :\n" << e << endl;
         return -2;
      }
      catch(Exception& e) {
         if(CI.Verbose)
            oflog << "ReadNextObs caught an exception while reading obs in file "
               << of.name << " :\n" << e << endl;
         return -3;
      }

      if(!of.ins) {
         if(CI.Verbose) oflog << "EOF found on file " << of.name << endl;
         return -1;                    // EOF
      }

      //temp
      if(CI.Debug) {
         oflog << "ReadNextObs finds SVs:";
         RinexObsData::RinexSatMap::const_iterator it;
         for(it=of.Robs.obs.begin(); it != of.Robs.obs.end(); ++it)
            oflog << " " << it->first;
         oflog << endl;
      }

      // is the timetag an even multiple of DataInterval?
      double sow = of.Robs.time.GPSsecond();
      double frac = sow - CI.DataInterval*double(int(sow/CI.DataInterval + 0.5));
      if(fabs(frac) < 0.5) break;
      else if(CI.Debug) oflog << "skip epoch "
         << of.Robs.time.printf("%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
   }

   of.nread++;                               // success

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
