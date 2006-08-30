#pragma ident "$Id$"

/**
 * @file RinexUtilities.cpp
 * Miscellaneous utilities for use with Rinex data.
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
#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexObsHeader.hpp"

#include "RinexUtilities.hpp"

#include <iostream>

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------
int RegisterARLUTExtendedTypes(void)
{
   unsigned int EPPS = //0x60
      RinexObsHeader::RinexObsType::EPdepend | RinexObsHeader::RinexObsType::PSdepend;
   unsigned int L1L2 = //0x06
      RinexObsHeader::RinexObsType::L1depend | RinexObsHeader::RinexObsType::L2depend;
   unsigned int P1P2 = //0x18
      RinexObsHeader::RinexObsType::P1depend | RinexObsHeader::RinexObsType::P2depend;
   unsigned int EPEP=RinexObsHeader::RinexObsType::EPdepend;//0x20
   unsigned int PELL=EPPS | L1L2;//0x66
   unsigned int PEPP=EPPS | P1P2;//0x78
   unsigned int PsLs=L1L2 | P1P2;//0x1E
   unsigned int L1P1 = //0x0A
      RinexObsHeader::RinexObsType::L1depend | RinexObsHeader::RinexObsType::P1depend;
   unsigned int L2P2 = //0x14
      RinexObsHeader::RinexObsType::L2depend | RinexObsHeader::RinexObsType::P2depend;
   int j;
   j = RegisterExtendedRinexObsType("ER","Ephemeris range",     "meters", EPPS);
   if(j) return j;
   j = RegisterExtendedRinexObsType("RI","Iono Delay, Range",   "meters", P1P2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("PI","Iono Delay, Phase",   "meters", L1L2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("TR","Tropospheric Delay",  "meters", EPPS);
   if(j) return j;
   j = RegisterExtendedRinexObsType("RL","Relativity Correct.", "meters", EPEP);
   if(j) return j;
   j = RegisterExtendedRinexObsType("SC","SV Clock Bias",       "meters", EPEP);
   if(j) return j;
   j = RegisterExtendedRinexObsType("EL","Elevation Angle",     "degrees",EPPS);
   if(j) return j;
   j = RegisterExtendedRinexObsType("AZ","Azimuth Angle",       "degrees",EPPS);
   if(j) return j;
   j = RegisterExtendedRinexObsType("SR","Slant TEC (PR)",      "TECU",   P1P2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("SP","Slant TEC (Ph)",      "TECU",   L1L2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("VR","Vertical TEC (PR)",   "TECU",   PEPP);
   if(j) return j;
   j = RegisterExtendedRinexObsType("VP","Vertical TEC (Ph)",   "TECU",   PELL);
   if(j) return j;
   j = RegisterExtendedRinexObsType("LA","Lat Iono Intercept",  "degrees",EPPS);
   if(j) return j;
   j = RegisterExtendedRinexObsType("LO","Lon Iono Intercept",  "degrees",EPPS);
   if(j) return j;
   j = RegisterExtendedRinexObsType("P3","TFC(IF) Pseudorange", "meters", P1P2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("L3","TFC(IF) Phase",       "meters", L1L2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("PF","GeoFree Pseudorange", "meters", P1P2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("LF","GeoFree Phase",       "meters", L1L2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("PW","WideLane Pseudorange","meters", P1P2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("LW","WideLane Phase",      "meters", L1L2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("MP","Multipath (=M3)",     "meters", PsLs);
   if(j) return j;
   j = RegisterExtendedRinexObsType("M1","L1 Range minus Phase","meters", L1P1);
   if(j) return j;
   j = RegisterExtendedRinexObsType("M2","L2 Range minus Phase","meters", L2P2);
   if(j) return j;
   j = RegisterExtendedRinexObsType("M3","IF Range minus Phase","meters", PsLs);
   if(j) return j;
   j = RegisterExtendedRinexObsType("M4","GF Range minus Phase","meters", PsLs);
   if(j) return j;
   j = RegisterExtendedRinexObsType("M5","WL Range minus Phase","meters", PsLs);
   if(j) return j;
   j = RegisterExtendedRinexObsType("XR","Non-dispersive Range","meters", PsLs);
   if(j) return j;
   j = RegisterExtendedRinexObsType("XI","Ionospheric delay",   "meters", PsLs);
   if(j) return j;
   j = RegisterExtendedRinexObsType("X1","Range Error L1",      "meters", PsLs);
   if(j) return j;
   j = RegisterExtendedRinexObsType("X2","Range Error L2",      "meters", PsLs);
   if(j) return j;
   j = RegisterExtendedRinexObsType("SX","Satellite ECEF-X",    "meters", EPEP);
   if(j) return j;
   j = RegisterExtendedRinexObsType("SY","Satellite ECEF-Y",    "meters", EPEP);
   if(j) return j;
   j = RegisterExtendedRinexObsType("SZ","Satellite ECEF-Z",    "meters", EPEP);
   if(j) return j;
   return 0;
}

//------------------------------------------------------------------------------------
bool isSP3File(const string& file)
{
   SP3Header header;
   SP3Stream strm(file.c_str());
   strm.exceptions(fstream::failbit);
   try { strm >> header; } catch(gpstk::Exception& e) { return false; }
   strm.close();
   return true;
}
//------------------------------------------------------------------------------------
bool isRinexNavFile(const string& file)
{
   RinexNavHeader header;
   RinexNavStream rnstream(file.c_str());
   rnstream.exceptions(fstream::failbit);
   try { rnstream >> header; } catch(gpstk::Exception& e) { return false; }
   rnstream.close();
   return true;
}
//------------------------------------------------------------------------------------
bool isRinexObsFile(const string& file)
{
   RinexObsHeader header;
   RinexObsStream rostream(file.c_str());
   rostream.exceptions(fstream::failbit);
   try { rostream >> header; } catch(gpstk::Exception& e) { return false; }
   rostream.close();
   return true;
}
//------------------------------------------------------------------------------------
int FillEphemerisStore(const vector<string>& files, SP3EphemerisStore& PE,
      BCEphemerisStore& BCE)
{
try {
   int nread=0;
   RinexNavHeader rnh;
   RinexNavData rne;
   for(int i=0; i<files.size(); i++) {
      if(files[i].empty()) throw Exception("File name is empty");
      RinexNavStream strm(files[i].c_str());
      if(!strm) throw Exception("Could not open file " + files[i]);
      strm.close();
      if(isRinexNavFile(files[i])) {
         RinexNavStream RNFileIn(files[i].c_str());
         RNFileIn.exceptions(fstream::failbit);
         try {
            RNFileIn >> rnh;
            //if(Verbose) {
            //   logof << "Input Rinex Nav file header\n";
            //   rnh.dump(logof);
            //}
            while (RNFileIn >> rne)
            {
               if(rne.health == 0)
                  BCE.addEphemeris(rne);
            }
            nread++;
         }
         catch(gpstk::Exception& e) {
            cerr << "Caught Exception while reading Rinex Nav file " << files[i]
               << " :\n" << e << endl;
            continue;
         }
      }
      else if(isSP3File(files[i])) {
         try {
            PE.loadFile(files[i]);
         }
         catch(gpstk::Exception& e) {
            cerr << "Caught Exception while reading SP3 Nav file " << files[i]
               << " :\n" << e << endl;
            continue;
         }
         nread++;
      }
      else throw Exception("File " + files[i] + " is neither BCE nor PE file.");
   }
   return nread;
}
catch(gpstk::Exception& e) {
   cerr << "RinexUtilities:FillEphemerisStore caught an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
catch (...) {
   cerr << "RinexUtilities:FillEphemerisStore caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
