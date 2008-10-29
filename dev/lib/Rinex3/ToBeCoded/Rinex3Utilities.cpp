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
 * @file Rinex3Utilities.cpp
 * Miscellaneous RINEX3-related utilities.
 */

//------------------------------------------------------------------------------------
// system includes

// GPSTk includes 
#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3Utilities.hpp"

namespace gpstk {

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------
int RegisterARLUTExtendedTypes(void)
{
try {
   unsigned int EPPS = //0x60
      Rinex3ObsHeader::ObsID::EPdepend | Rinex3ObsHeader::ObsID::PSdepend;
   unsigned int L1L2 = //0x06
      Rinex3ObsHeader::ObsID::L1depend | Rinex3ObsHeader::ObsID::L2depend;
   unsigned int P1P2 = //0x18
      Rinex3ObsHeader::ObsID::P1depend | Rinex3ObsHeader::ObsID::P2depend;
   unsigned int EPEP=Rinex3ObsHeader::ObsID::EPdepend;//0x20
   unsigned int PELL=EPPS | L1L2;//0x66
   unsigned int PEPP=EPPS | P1P2;//0x78
   unsigned int PsLs=L1L2 | P1P2;//0x1E
   unsigned int L1P1 = //0x0A
      Rinex3ObsHeader::ObsID::L1depend | Rinex3ObsHeader::ObsID::P1depend;
   unsigned int L2P2 = //0x14
      Rinex3ObsHeader::ObsID::L2depend | Rinex3ObsHeader::ObsID::P2depend;
   int j;
   j = RegisterExtendedObsID("ER","Ephemeris range",     "meters", EPPS);
   if(j) return j;
   j = RegisterExtendedObsID("RI","Iono Delay, Range",   "meters", P1P2);
   if(j) return j;
   j = RegisterExtendedObsID("PI","Iono Delay, Phase",   "meters", L1L2);
   if(j) return j;
   j = RegisterExtendedObsID("TR","Tropospheric Delay",  "meters", EPPS);
   if(j) return j;
   j = RegisterExtendedObsID("RL","Relativity Correct.", "meters", EPEP);
   if(j) return j;
   j = RegisterExtendedObsID("SC","SV Clock Bias",       "meters", EPEP);
   if(j) return j;
   j = RegisterExtendedObsID("EL","Elevation Angle",     "degrees",EPPS);
   if(j) return j;
   j = RegisterExtendedObsID("AZ","Azimuth Angle",       "degrees",EPPS);
   if(j) return j;
   j = RegisterExtendedObsID("SR","Slant TEC (PR)",      "TECU",   P1P2);
   if(j) return j;
   j = RegisterExtendedObsID("SP","Slant TEC (Ph)",      "TECU",   L1L2);
   if(j) return j;
   j = RegisterExtendedObsID("VR","Vertical TEC (PR)",   "TECU",   PEPP);
   if(j) return j;
   j = RegisterExtendedObsID("VP","Vertical TEC (Ph)",   "TECU",   PELL);
   if(j) return j;
   j = RegisterExtendedObsID("LA","Lat Iono Intercept",  "degrees",EPPS);
   if(j) return j;
   j = RegisterExtendedObsID("LO","Lon Iono Intercept",  "degrees",EPPS);
   if(j) return j;
   j = RegisterExtendedObsID("P3","TFC(IF) Pseudorange", "meters", P1P2);
   if(j) return j;
   j = RegisterExtendedObsID("L3","TFC(IF) Phase",       "meters", L1L2);
   if(j) return j;
   j = RegisterExtendedObsID("PF","GeoFree Pseudorange", "meters", P1P2);
   if(j) return j;
   j = RegisterExtendedObsID("LF","GeoFree Phase",       "meters", L1L2);
   if(j) return j;
   j = RegisterExtendedObsID("PW","WideLane Pseudorange","meters", P1P2);
   if(j) return j;
   j = RegisterExtendedObsID("LW","WideLane Phase",      "meters", L1L2);
   if(j) return j;
   j = RegisterExtendedObsID("MP","Multipath (=M3)",     "meters", PsLs);
   if(j) return j;
   j = RegisterExtendedObsID("R1","(P1 + L1)/2"         ,"meters", L1P1);
   if(j) return j;
   j = RegisterExtendedObsID("R2","(P2 + L2)/2"         ,"meters", L2P2);
   if(j) return j;
   j = RegisterExtendedObsID("M1","L1 Range minus Phase","meters", L1P1);
   if(j) return j;
   j = RegisterExtendedObsID("M2","L2 Range minus Phase","meters", L2P2);
   if(j) return j;
   j = RegisterExtendedObsID("M3","IF Range minus Phase","meters", PsLs);
   if(j) return j;
   j = RegisterExtendedObsID("M4","GF Range minus Phase","meters", PsLs);
   if(j) return j;
   j = RegisterExtendedObsID("M5","WL Range minus Phase","meters", PsLs);
   if(j) return j;
   j = RegisterExtendedObsID("XR","Non-dispersive Range","meters", PsLs);
   if(j) return j;
   j = RegisterExtendedObsID("XI","Ionospheric delay",   "meters", PsLs);
   if(j) return j;
   j = RegisterExtendedObsID("X1","Range Error L1",      "meters", PsLs);
   if(j) return j;
   j = RegisterExtendedObsID("X2","Range Error L2",      "meters", PsLs);
   if(j) return j;
   j = RegisterExtendedObsID("SX","Satellite ECEF-X",    "meters", EPEP);
   if(j) return j;
   j = RegisterExtendedObsID("SY","Satellite ECEF-Y",    "meters", EPEP);
   if(j) return j;
   j = RegisterExtendedObsID("SZ","Satellite ECEF-Z",    "meters", EPEP);
   if(j) return j;
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
bool isSP3File(const string& file)
{
try {
   SP3Header header;
   SP3Stream strm(file.c_str());
   strm.exceptions(fstream::failbit);
   try { strm >> header; } catch(Exception& e) { return false; }
   strm.close();
   return true;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
bool isRinex3NavFile(const string& file)
{
try {
   Rinex3NavHeader header;
   Rinex3NavStream rnstream;
   try {
      rnstream.open(file.c_str(),ios::in);
      if(!rnstream) return false;
      rnstream.exceptions(fstream::failbit);
   }
   catch(Exception& e) { return false; }
   catch(exception& e) { return false; }
   try { rnstream >> header; }
   catch(Exception& e) { return false; }
   catch(exception& e) { return false; }
   rnstream.close();
   return true;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
bool isRinex3ObsFile(const string& file)
{
try {
   Rinex3ObsHeader header;
   Rinex3ObsStream rostream;
   try {
      rostream.open(file.c_str(),ios::in);
      if(!rostream) return false;
      rostream.exceptions(fstream::failbit);
   }
   catch(Exception& e) { return false; }
   catch(exception& e) { return false; }
   try { rostream >> header; }
   catch(Exception& e) { return false; }
   catch(exception& e) { return false; }
   rostream.close();
   return true;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void sortRinex3ObsFiles(vector<string>& files)
{
try {
   // build a hash with key = start time, value = filename
   map<CommonTime,string> hash;
   for(int n=0; n<files.size(); n++) {
      Rinex3ObsHeader header;
      Rinex3ObsStream rostream(files[n].c_str());
      rostream.exceptions(fstream::failbit);
      try { rostream >> header; } catch(Exception& e) {
         rostream.close();
         continue;
      }
      rostream.close();
      if(!header.isValid()) continue;
      hash[header.firstObs] = files[n];
   }
   // return the sorted file names
   files.clear();
   map<CommonTime,string>::const_iterator it = hash.begin();
   while(it != hash.end()) {
      files.push_back(it->second);
      it++;
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int FillEphemerisStore(const vector<string>& files, SP3EphemerisStore& PE,
      GPSEphemerisStore& BCE)
{
try {
   int nread=0;
   Rinex3NavHeader rnh;
   Rinex3NavData rne;
   for(int i=0; i<files.size(); i++) {
      if(files[i].empty()) throw Exception("File name is empty");
      Rinex3NavStream strm(files[i].c_str());
      if(!strm) throw Exception("Could not open file " + files[i]);
      strm.close();
      if(isRinex3NavFile(files[i])) {
         Rinex3NavStream RNFileIn(files[i].c_str());
         RNFileIn.exceptions(fstream::failbit);
         try {
            RNFileIn >> rnh;
            while (RNFileIn >> rne)
            {
               // check health...
               if(rne.health == 0)
                  BCE.addEphemeris(rne);
            }
            nread++;
         }
         catch(Exception& e) {
            //cerr << "Caught Exception while reading Rinex3 Nav file " << files[i]
            //   << " :\n" << e << endl;
            //continue;
            GPSTK_RETHROW(e);
         }
      }
      else if(isSP3File(files[i])) {
         try {
            PE.loadFile(files[i]);
         }
         catch(Exception& e) {
            //cerr << "Caught Exception while reading SP3 Nav file " << files[i]
            //   << " :\n" << e << endl;
            //continue;
            GPSTK_RETHROW(e);
         }
         nread++;
      }
      else
         throw Exception("File " + files[i] + " is neither BCE nor PE file.");
   }
   return nread;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

} // end namespace gpstk

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
