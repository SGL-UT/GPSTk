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
void ReadAllObsHeaders(void) throw(Exception);
int ReadNextObs(ObsFile& of) throw(Exception);

//------------------------------------------------------------------------------------
void ReadAllObsHeaders(void) throw(Exception)
{
try {
   int i,j,k;
   if(CI.Verbose) oflog << "BEGIN ReadAllObsHeaders()" << endl;

      // open all obs files and read headers
   for(i=0; i<ObsFileList.size(); i++) {
      string filename;

         // initialize -- this will mark unopened file
      ObsFileList[i].ins.close();
      ObsFileList[i].ins.clear();
      ObsFileList[i].nread = -1;
      ObsFileList[i].dt = -1.0;
      ObsFileList[i].firstTime = DayTime::BEGINNING_OF_TIME;
      ObsFileList[i].valid = false;

         // filename
      if(!CI.InputPath.empty()) filename = CI.InputPath + "/";
      filename += ObsFileList[i].name;

         // open
      ObsFileList[i].ins.open(filename.c_str(),ios_base::in);

         // did open succeed?
      if(!ObsFileList[i].ins) {
         oflog << "Warning: File " << filename
            << " could not be opened. Ignore." << endl;
         if(CI.Screen) cout << "Warning: File " << filename
               << " could not be opened. Ignore." << endl;
         continue;
      }

         // set exceptions
      ObsFileList[i].ins.exceptions(ios_base::failbit);

      // read the header twice; first do all the initialization,
      // including reading some obs records to determine initial
      // time and nominal time spacing, second to reset the file
      // handle at the first obs.
      for(k=0; k<2; k++) {

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

         // if this is the second reading, quit
      if(k==1) break;

         // check that file contains C1/P1,P2,L1,L2,S1,S2
      ObsFileList[i].inC1 = ObsFileList[i].inP1 = ObsFileList[i].inP2 = -1;
      ObsFileList[i].inL1 = ObsFileList[i].inL2 = -1;
      ObsFileList[i].inD1 = ObsFileList[i].inD2 = -1;
      ObsFileList[i].inS1 = ObsFileList[i].inS2 = -1;
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

         // read some obs records to compute the first time and the nominal DT
      {
         int jj,kk,nleast,nepochs=0,ndt[9]={-1,-1,-1, -1,-1,-1, -1,-1,-1};
         double dt,bestdt[9];
         DayTime prev=DayTime::END_OF_TIME;
         while(1) {
            try { ObsFileList[i].ins >> ObsFileList[i].Robs; }
            catch(Exception& e) { break; }   // simply quit if meet failure
            if(!ObsFileList[i].ins) break;   // or EOF
            dt = ObsFileList[i].Robs.time - prev;
            if(dt > 0.0) {
               for(j=0; j<9; j++) {
                  if(ndt[j] <= 0) { bestdt[j]=dt; ndt[j]=1; break; }
                  if(fabs(dt-bestdt[j]) < 0.0001) { ndt[j]++; break; }
                  if(i == 8) {
                     kk=0; nleast=ndt[kk];
                     for(jj=1; jj<9; jj++) if(ndt[jj] <= nleast) {
                        kk=jj; nleast=ndt[jj];
                     }
                     ndt[kk]=1; bestdt[kk]=dt;
                  }
               }
            }
            nepochs++;
            if(nepochs > 10) break;
            if(nepochs == 1) ObsFileList[i].firstTime = ObsFileList[i].Robs.time;
            prev = ObsFileList[i].Robs.time;
         }
         // save the result
         for(jj=1,kk=0; jj<9; jj++) if(ndt[jj]>ndt[kk]) kk=jj;
         ObsFileList[i].dt = bestdt[kk];
         if(CI.Verbose) oflog
         << "Found interval " << ObsFileList[i].dt << ", and first epoch " 
         << ObsFileList[i].firstTime.printf("%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g")
         << endl;
      }

         // go back and do it again
      ObsFileList[i].ins.close();
      ObsFileList[i].ins.clear();
      ObsFileList[i].ins.open(filename.c_str(),ios_base::in);
      ObsFileList[i].ins.exceptions(ios_base::failbit);
      } // end reading twice

   }  // end loop over input observation files
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int ReadNextObs(ObsFile& of) throw(Exception)
{
try {
      // read the next observation epoch
      // decimate to even multiples of DataInterval
   while(1) {
      int iret;
      try {
         if(CI.Debug) oflog << "ReadNextObs for file " << of.name << endl;
         if(!of.getNext) return 1;

         // read obs data
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

      // test EOF
      if(!of.ins) {
         if(CI.Verbose) oflog << "EOF found on file " << of.name << endl;
         return -1;                    // EOF
      }

      //temp
      //if(CI.Debug) {
      //   oflog << "ReadNextObs finds SVs:";
      //   RinexObsData::RinexSatMap::const_iterator it;
      //   for(it=of.Robs.obs.begin(); it != of.Robs.obs.end(); ++it)
      //      oflog << " " << it->first;
      //   oflog << endl;
      //}

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
