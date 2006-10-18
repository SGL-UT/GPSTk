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

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "MSCData.hpp"
#include "MSCStream.hpp"
#include "RinexMetStream.hpp"
#include "RinexMetData.hpp"

#include "ObsReader.hpp"

#include "SP3EphemerisStore.hpp"
#include "BCEphemerisStore.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"

#include "FICStream.hpp"
#include "FICData.hpp"

#include "readers.hpp"
#include "ObsUtils.hpp"

using namespace std;
using namespace gpstk;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_msc_data(
   std::string& fn, 
   unsigned long msid,
   Triple& ap)
{
   MSCStream msc(fn.c_str(), ios::in);
   MSCData mscd;
   while (msc >> mscd)
   {
      if (mscd.station == msid)
      {
         ap = mscd.coordinates;
         if (verbosity)
            cout << "Station " << msid
                 << " location: " << ap << endl;
         break;
      }
   }
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_obs_data(
   CommandOption& files,
   unsigned long msid, 
   ObsEpochMap& oem, 
   Triple& ap,
   const DayTime& startTime,
   const DayTime& stopTime)
{
   string fn;
   FFIdentifier firstType;

   try
   {
      for (int i=0; i<files.getCount(); i++)
      {
         fn = (files.getValue())[i];
         ObsReader obsReader(fn, verbosity);
         obsReader.msid = msid;

         if (i==0)
         {
            firstType = obsReader.inputType;
            if (obsReader.inputType == FFIdentifier::tRinexObs)
            {
               ap = obsReader.roh.antennaPosition;
               if (verbosity>1)
                  cout << "Antenna position read from RINEX obs file:"
                       << ap << endl;
            }
         }
         else if (obsReader.inputType != firstType)
         {
            cout << "Don't change file types on me..." << endl;
            break;
         }

         while (true)
         {
            ObsEpoch oe(obsReader.getObsEpoch());
            if (!obsReader())
               break;
            if (oe.time > startTime && oe.time < stopTime)
            {
               oem[oe.time] = oe;
               oem[oe.time].time = oe.time;
            }
         }
      }
   }
   catch (std::exception e)
   {
      cout << "Error reading obs data file " << fn << endl;
   }
   
   if (verbosity)
      cout << "Have obs data from " << oem.begin()->first.printf(timeFormat)
           << " to " << oem.rbegin()->first.printf(timeFormat) << endl;
} // end of read_obs_data()


// ---------------------------------------------------------------------
// Read in weather data into the indicated store
// ---------------------------------------------------------------------
WxObsData* read_met_data(CommandOption& files)
{
   WxObsData *wod = new(WxObsData);

   for (int i=0; i<files.getCount(); i++)
   {
      string fn = (files.getValue())[i];
      RinexMetStream rms;
      try { rms.open(fn.c_str(), ios::in); }
      catch (...) {
         cout << "Error reading weather data from file " << fn << endl;
         exit(-1);
      }
      if (verbosity)
         cout << "Reading met data from " << fn << "." << endl;

      RinexMetData rmd;
      while (rms >> rmd)
      {
         WxObservation wob(
            rmd.time,
            rmd.data[RinexMetHeader::TD],
            rmd.data[RinexMetHeader::PR],
            rmd.data[RinexMetHeader::HR]);
         wod->insertObservation(wob);
      }

   }

   if (verbosity)
      if (wod->firstTime < wod->lastTime)
         cout << "Have met data from " << wod->firstTime 
              << " to " << wod->lastTime << "." << endl;
      else
         cout << "No met data for you!" << endl;

   return wod;
} // end of read_met_data()


// ---------------------------------------------------------------------
// Read in ephemeris data into the indicated store
// ---------------------------------------------------------------------
EphemerisStore* read_eph_data(CommandOption& files)
{
   EphemerisStore* eph=NULL;
   BCEphemerisStore* bce;
   SP3EphemerisStore* pe;

   try
   {
      // First try the first file as a RINEX nav file
      bce = new(BCEphemerisStore);
      for (int i=0; i<files.getCount(); i++)
      {
         string fn = (files.getValue())[i];
         if (verbosity>2 && i==0)
            cout << "Attepmting to read " << fn << " as RINEX nav." << endl;
         
         RinexNavStream rns(fn.c_str(), ios::in);
         rns.exceptions(ifstream::failbit);
         RinexNavData rnd;

         while (rns >> rnd)
            bce->addEphemeris(rnd);
      }
      if (verbosity)
         cout << "Read RINEX format broadcast ephemerides." << endl;
      eph = bce;
   }
   catch (FFStreamError& e)
   {
      if (verbosity>4)
         cout << e << endl;
      delete bce;
   }

   // If the file failed to read as a RINEX file, try it as a SP3 file
   if (!eph) try
   {
      pe = new(SP3EphemerisStore);
      for (int i=0; i<files.getCount(); i++)
      {
         string fn = (files.getValue())[i];
         if (verbosity>2 && i==0)
            cout << "Attempting to read " << fn << " as SP3." << endl;
         
         SP3Stream pefile(fn.c_str(),ios::in);
         pefile.exceptions(ifstream::failbit);

         SP3Header header;
         pefile >> header;

         SP3Data data;
         while(pefile >> data)
            pe->addEphemeris(data);
      }
      if (verbosity)
         cout << "Read SP3 format precise ephemerides." << endl;
      eph = pe;
   }
   catch (FFStreamError& e)
   {
      if (verbosity>4)
         cout << e << endl;
      delete pe;
   }

   
   // If the file failed to read as a RINEX or SP3 file, try it as a FIC file
   if (!eph) try
   {
      bce = new(BCEphemerisStore);
      for (int i=0; i<files.getCount(); i++)
      {
         string fn = (files.getValue())[i];
         if (verbosity>2 && i==0)
            cout << "Attempting to read " << fn << " as FIC." << endl;
         
         FICStream fs(fn.c_str());
         // Note that we don't want to set the stream failbit since the binary
         // read routines take care of throwing all the error conditions.

         FICHeader header;
         fs >> header;

         FICData data;
         while(fs >> data)
            if (data.blockNum==9) // Only look at the eng ephemeris
               bce->addEphemeris(data);
      }
      if (verbosity)
         cout << "Read FIC format broadcast ephemerides." << endl;
      eph = bce;
   }
   catch (FFStreamError& e)
   {
      if (verbosity>4)
         cout << e << "Blarf, man." << endl;
      delete bce;
   }


   // If the file failed to read as a RINEX, FIC, or SP3 file give up
   if (!eph)
   {
      cout << "Could not determine type of nav data supplied" << endl;
      eph = new(BCEphemerisStore);
   }
   else
   {
      if (verbosity>1)
         if (eph->getInitialTime() < eph->getFinalTime())
            cout << "Have ephemeris data from "
                 << eph->getInitialTime().printf(timeFormat)
                 << " to " << eph->getFinalTime().printf(timeFormat) << endl;
         else
            cout << "Do not have any ephemeris data" << endl;
   }

   return eph;
} // end of read_eph_data()
