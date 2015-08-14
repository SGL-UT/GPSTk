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
 * @file DataOutput.cpp
 * Output intermediate data to files for program DDBase.
 */

//------------------------------------------------------------------------------------
// TD DataOutput.cpp error msg for failure to open output files
// TD DataOutput.cpp add clock polynomial Evaluate(tt) to CLK output

//------------------------------------------------------------------------------------
// includes
// system
#include <fstream>

// GPSTk

// DDBase
#include "DDBase.hpp"
#include "CommandInput.hpp"
#include "format.hpp"
#include "TimeString.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- this module only

//------------------------------------------------------------------------------------
// called from DDBase.cpp
int OutputRawData(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN OutputRawData()" << endl;

   if(CI.OutputRawFile.empty()) return 0;

   size_t i;
   CommonTime tt;
   map<string,Station>::const_iterator it;
   map<GSatID,RawData>::const_iterator jt;
   format f133(13,3),f52(5,2);

      // open an output file for RAW data
   ofstream rawofs;
   rawofs.open(CI.OutputRawFile.c_str(),ios::out);
   if(rawofs.is_open()) {
      oflog << "Opened file " << CI.OutputRawFile << " for raw data output.." << endl;
      rawofs << "# " << Title << endl;
      rawofs << "RAW site sat week   sec_wk   count    L1_cyc        L2_cyc"
         << "          P1_m          P2_m          ER_m      EL    AZ\n";
   }
   else {
      // TD error msg
      return -1;
   }

      // loop over stations
   for(it=Stations.begin(); it != Stations.end(); it++) {

         // loop over satellites
      for(jt=it->second.RawDataBuffers.begin();
          jt != it->second.RawDataBuffers.end(); jt++) {

            // loop over epochs
         for(i=0; i<jt->second.count.size(); i++) {

            tt = FirstEpoch + jt->second.count[i]*CI.DataInterval;

            rawofs << "RAW " << it->first << " " << jt->first << " "
               << printTime(tt,"%4F %10.3g")
               << " " << setw(5) << jt->second.count[i]
               << " " << f133 << jt->second.L1[i]
               << " " << f133 << jt->second.L2[i]
               << " " << f133 << jt->second.P1[i]
               << " " << f133 << jt->second.P2[i]
               << " " << f133 << jt->second.ER[i]
               << " " << f52 << jt->second.elev[i]
               << " " << f52 << jt->second.az[i]
               << endl;

         }  // end loop over epochs

      }  // loop over satellites

   }  // loop over stations
   
      // close output file
   rawofs.close();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end OutputRawData()

//------------------------------------------------------------------------------------
// called from EditDDs.cpp
int OutputRawDData(const DDid& ddid, const DDData& dddata, const vector<int>& mark)
   throw(Exception)
{
try {
   bool TripleOut=true;                      // output triple differences as well
   if(CI.OutputRawDDFile.empty()) return 0;

   static ofstream rddofs;

      // allow caller to close the file...
   if(mark.size() == 0) {
      if(rddofs.is_open()) rddofs.close();
      return 0;
   }

   if(!rddofs.is_open()) {           // first call : open the file
      if(CI.Verbose) oflog << "BEGIN OutputRawDData()" << endl;
      rddofs.open(CI.OutputRawDDFile.c_str(),ios::out);
      if(rddofs.is_open()) {
         oflog << "Opened file " << CI.OutputRawDDFile
            << " for raw DD data output." << endl;
         rddofs << "# " << Title << endl;
         rddofs << "RDD sit1 sit2 sat ref week  sec_wk     flag      DDL1_m"
            << "           "
            << "DDL2_m           DDER_m            resL1_m          resL2_m";
         if(CI.Frequency == 3) rddofs << "          WLbias_m";
         rddofs << endl;
         if(TripleOut) rddofs
            << "RTD sit1 sit2 sat ref week  sec_wk     flag      TDL1_m"
            << "           TDL2_m           TDER_m" << endl;
      }
      else {
         // TD error msg
         return -1;
      }
   }

   size_t i;
   double wlb;
   CommonTime tt;
   format f166(16,6);

      // loop over epochs
   for(i=0; i<dddata.count.size(); i++) {

      tt = FirstEpoch + dddata.count[i]*CI.DataInterval;

      if(CI.Frequency == 3)
         wlb =   wl1p * dddata.DDL1[i]      // wide lane range minus phase
               + wl2p * dddata.DDL2[i]      // = WL phase - NL range
               - wl1r * dddata.DDP1[i] 
               - wl2r * dddata.DDP2[i];

      rddofs << "RDD " << ddid << " " << printTime(tt,"%4F %10.3g")
         << " " << setw(2) << mark[i]
         << " " << f166 << dddata.DDL1[i]
         << " " << f166 << dddata.DDL2[i]
         << " " << f166 << dddata.DDER[i]
         << " " << f166 << dddata.DDL1[i] - dddata.DDER[i]
         << " " << f166 << dddata.DDL2[i] - dddata.DDER[i];
      if(CI.Frequency == 3) rddofs << " " << f166 << wlb;
      rddofs << endl;

      if(TripleOut && i>0) {
         // wlb is a dummy here, = delta time for this triple diff
         wlb = (dddata.count[i]-dddata.count[i-1])*CI.DataInterval;
         rddofs << "RTD " << ddid << " " << printTime(tt,"%4F %10.3g")
            << " " << setw(2) << 10*mark[i]+mark[i-1]
            << " " << f166 << (dddata.DDL1[i]-dddata.DDL1[i-1])/wlb
            << " " << f166 << (dddata.DDL2[i]-dddata.DDL2[i-1])/wlb
            << " " << f166 << (dddata.DDER[i]-dddata.DDER[i-1])/wlb
            << endl;
      }
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end OutputRawDData()

//------------------------------------------------------------------------------------
int OutputDDData(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN OutputDDData()" << endl;

   if(CI.OutputDDDFile.empty()) return 0;

   size_t i;
   double wlb;
   CommonTime tt;
   map<DDid,DDData>::const_iterator it;
   format f166(16,6);

      // open an output file for DDD data
   ofstream dddofs;
   dddofs.open(CI.OutputDDDFile.c_str(),ios::out);
   if(dddofs.is_open()) {
      oflog << "Opened file " << CI.OutputDDDFile << " for DD data output." << endl;
      dddofs << "# " << Title << endl;
      dddofs << "DDD sit1 sit2 sat ref week  sec_wk           DDL1_m           "
         << "DDL2_m           DDER_m            resL1_m          resL2_m";
      if(CI.Frequency == 3) dddofs << "          WLbias_m";
      dddofs << endl;
   }
   else {
      // TD error msg
      return -1;
   }

      // loop over DDids
   for(it=DDDataMap.begin(); it != DDDataMap.end(); it++) {

         // loop over epochs
      for(i=0; i<it->second.count.size(); i++) {

         tt = FirstEpoch + it->second.count[i]*CI.DataInterval;

         if(CI.Frequency == 3)
            wlb =   wl1p * it->second.DDL1[i]      // wide lane range minus phase
                  + wl2p * it->second.DDL2[i]      // = WL phase - NL range
                  - wl1r * it->second.DDP1[i] 
                  - wl2r * it->second.DDP2[i];

         dddofs << "DDD " << it->first << " " << printTime(tt,"%4F %10.3g")
            << " " << f166 << it->second.DDL1[i]
            << " " << f166 << it->second.DDL2[i]
            << " " << f166 << it->second.DDER[i]
            << " " << f166 << it->second.DDL1[i] - it->second.DDER[i]
            << " " << f166 << it->second.DDL2[i] - it->second.DDER[i];
         if(CI.Frequency == 3) dddofs << " " << f166 << wlb;
         dddofs << endl;
      }

   }

   dddofs.close();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end OutputDDData()

//------------------------------------------------------------------------------------
int OutputClockData(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN OutputClockData()" << endl;

   if(CI.OutputClkFile.empty()) return 0;

   size_t i;
   CommonTime tt;
   map<string,Station>::const_iterator it;
   format f166(16,6),f92(9,2,2),f96(9,6);

      // open an output file for Clk data
   ofstream clkofs;
   clkofs.open(CI.OutputClkFile.c_str(),ios::out);
   if(clkofs.is_open()) {
      oflog << "Opened file " << CI.OutputClkFile << " for DD data output." << endl;
      clkofs << "# " << Title << endl;
      clkofs << "CLK site week  sec_wk   Rx_clk_bias(m)   Sig(m)   TT_off(s)\n";
   }
   else {
      // TD error msg
      return -1;
   }


      // loop over stations
   for(it=Stations.begin(); it != Stations.end(); it++) {

         // loop over epochs
      for(i=0; i<it->second.ClockBuffer.size(); i++) {

         tt = FirstEpoch + it->second.CountBuffer[i]*CI.DataInterval;

         clkofs << "CLK " << it->first << " " << printTime(tt,"%4F %10.3g")
            << " " << f166 << it->second.ClockBuffer[i]
            << " " << f92 << it->second.ClkSigBuffer[i]
            // TD add clock polynomial Evaluate(tt)
            << " " << f92 << it->second.RxTimeOffset[i]
            << endl;

      }  // loop over epochs

   }  // loop over stations
   
   clkofs.close();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end OutputClockData()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
