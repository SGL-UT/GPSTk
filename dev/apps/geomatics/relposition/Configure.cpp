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
 * @file Configure.cpp
 * Manage configuration details, at various points, for program DDBase.
 */

//------------------------------------------------------------------------------------
// system includes

// GPSTk

// DDBase
#include "DDBase.hpp"
#include "RinexUtilities.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// local data
BCEphemerisStore BCEphList;               // global pEph will point to one of these
SP3EphemerisStore SP3EphList;

SimpleTropModel TropModelSimple;          // CI.pTropModel will point to one of these
GGTropModel TropModelGG;
GGHeightTropModel TropModelGGh;
NBTropModel TropModelNB;
SaasTropModel TropModelSaas;

//------------------------------------------------------------------------------------
// prototypes -- this module only
int Initialize(void);
int UpdateConfig(void);
void ReadAllObsHeaders(void);       // ReadObsFiles.cpp
int ConfigureEstimation(void);      // Estimation.cpp

//------------------------------------------------------------------------------------
int Configure(int which)
{
try {
   if(which == 1) return Initialize();
   if(which == 2) return UpdateConfig();
   if(which == 3) {
      if(CI.Verbose) oflog << "BEGIN Configure(3)" << endl;
      return ConfigureEstimation();            // Estimation.cpp
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end Configure()

//------------------------------------------------------------------------------------
// Configure(1)
// open and read navigation files
// open and read headers of all observation files
int Initialize(void)
{
try {
   int i,j;

   if(CI.Verbose) oflog << "BEGIN Configure(1)" << endl;
   if(CI.Frequency == 1) wave = wl1;
   if(CI.Frequency == 2) wave = wl2;
      // NB wave should never be used for L3 -- see warning in CommandInput.cpp
   else if(CI.Frequency == 3) wave = wl1;

      // open nav files and read EphemerisStore
   if(!CI.NavPath.empty())
      for(i=0; i<CI.NavFileNames.size(); i++)
         CI.NavFileNames[i] = CI.NavPath + "/" + CI.NavFileNames[i];

      // fill ephemeris store -- this routine in RinexUtilities.cpp
   FillEphemerisStore(CI.NavFileNames, SP3EphList, BCEphList);

      // dump SP3 store to log
   if(SP3EphList.size()) {
      if(CI.Verbose) SP3EphList.dump(0,oflog);
   }
   else if(CI.Verbose) oflog << "SP3 Ephemeris store is empty" << endl;

      // dump BCE store to log
   if(BCEphList.size()) {
         // this causes the CorrectedEphemerisRange routines to pick the
         // closest TOE in either future or past of the epoch, rather
         // than the closest in the past -- see BCEphemerisStore.hpp
      BCEphList.SearchNear();

      if(CI.Debug) BCEphList.dump(1,oflog);
      else if(CI.Verbose) BCEphList.dump(0,oflog);
   }
   else if(CI.Verbose) oflog << "BC Ephemeris store is empty" << endl;

      // assign pointer
   if(SP3EphList.size()) pEph = &SP3EphList;
   else if(BCEphList.size()) pEph = &BCEphList;
   else {
      cerr << "Initialize ERROR: no ephemeris. Abort." << endl;
      oflog << "Initialize ERROR: no ephemeris. Abort." << endl;
      return 1;
   }

      // open all EOP files and fill the EOPstore
   if(!CI.EOPPath.empty())
      for(i=0; i<CI.EOPFileNames.size(); i++)
         CI.EOPFileNames[i] = CI.EOPPath + "/" + CI.EOPFileNames[i];

   if(CI.EOPFileNames.size() > 0) {
      for(i=0; i<CI.EOPFileNames.size(); i++)
         EOPList.addFile(CI.EOPFileNames[i]);
   }
   else {
      try {
         EOPList.addIERSFile("finals.daily");
      }
      catch(FileMissingException& fme) {
         string msg("DDBase was unable to find any Earth Orientation parameters:\n"
           " either add option --EOPFile <file> or put file 'finals.daily' in the"
           " current directory.\n  (http://maia.usno.navy.mil/ser7/finals.daily)\n");
         cerr << msg;
         oflog << msg;
         GPSTK_RETHROW(fme);
      }
   }

   if(EOPList.size()) {
      if(CI.Debug) EOPList.dump(1,oflog);
      else if(CI.Verbose) EOPList.dump(0,oflog);
   }
   else oflog << "Warning - no Earth Orientation Parameters were input\n";

      // read all headers and store information in Station object
   ReadAllObsHeaders();

      // assign trop model for RAIM (model for DD est assigned in Configure(2))
      // NB using another, like Saastamoinen, here, is problematic because it
      // requires height, latitude and DOY input, [ this because RAIM calls
      // CI.pTropModel->correction(elevation) ], and that info is different for
      // different sites and not all available.
   CI.pTropModel = &TropModelSimple;
      // TD per site
   CI.pTropModel->setWeather(CI.DefaultTemp,CI.DefaultPress,CI.DefaultRHumid);

      // Define first and last epochs
   FirstEpoch = DayTime::BEGINNING_OF_TIME;
   LastEpoch = DayTime::END_OF_TIME;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Configure(2)
int UpdateConfig(void)
{
try {
   if(CI.Verbose) oflog << "BEGIN Configure(2)" << endl;

      // configure trop model for each station
      // dump height, zenith delays, etc
   map<string,Station>::iterator it;
   oflog << "Zenith tropospheric delays and station heights" << endl;
   for(it=Stations.begin(); it != Stations.end(); it++) {
      Station& st=it->second;
      oflog << "  Station " << it->first
            << " (" << (st.fixed ? "":"not ") << "fixed)" << endl;
      oflog << "    Position:  " << st.pos.printf("%13.3x m %13.3y m %13.3z m")
            << endl;
      oflog << "    Position:  " << st.pos.printf("%A deg N, %L deg E, %h m")
            << endl;
      oflog << "    Weather " << setprecision(1) << st.temp << " deg C, "
                            << setprecision(2) << st.press << " mbars, "
                            << setprecision(1) << st.rhumid << "%" << endl;

      st.pTropModel->setWeather(st.temp,st.press,st.rhumid);
      st.pTropModel->setReceiverHeight(st.pos.getHeight());
      st.pTropModel->setReceiverLatitude(st.pos.getGeodeticLatitude());
      st.pTropModel->setDayOfYear(int(FirstEpoch.DOYday()));

      oflog << "    Trop (model: " << st.TropType << fixed << ") zenith delays:"
         << " dry " << setprecision(6) << st.pTropModel->dry_zenith_delay();
      oflog << " m, wet " << setprecision(6) << st.pTropModel->wet_zenith_delay();
      oflog << " m, total "
            << setprecision(6) << st.pTropModel->correction(90.0) << " m";
      oflog << endl;
   }

      // check how much data there is

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
