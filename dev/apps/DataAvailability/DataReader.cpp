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
#include <map>

#include "SP3EphemerisStore.hpp"
#include "RinexEphemerisStore.hpp"
#include "GPSGeoid.hpp"

#include "RinexObsStream.hpp"
#include "RinexObsData.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexMetStream.hpp"
#include "RinexMetData.hpp"

#include "FICStream.hpp"
#include "FICData.hpp"

#include "SMODFStream.hpp"
#include "SMODFData.hpp"

#include "MSCData.hpp"
#include "MSCStream.hpp"

#include "MDPObsEpoch.cpp"
#include "MDPStream.hpp"

#include "DataReader.hpp"
#include "FFIdentifier.hpp"

#include "FormatConversionFunctions.hpp"

namespace gpstk
{
   using namespace std;

   DataReader::DataReader()
      : verbosity(1), 
        timeFormat("%4y %03j %02H:%02M:%5.1f"), 
        startTime(DayTime::BEGINNING_OF_TIME),
        stopTime(DayTime::END_OF_TIME),
        msid(0), eph(NULL), 
        haveEphData(false), haveObsData(false), havePosData(false)
   {}


   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read(CommandOption& files)
   {
      string fn;
      try
      {
         for (int i=0; i<files.getCount(); i++)
         {
            fn = (files.getValue())[i];
            FFIdentifier ffid(fn);
            switch (ffid)
            {
               case FFIdentifier::tRinexObs: read_rinex_obs_data(fn); break;
               case FFIdentifier::tRinexNav: break;
               case FFIdentifier::tRinexMet: break;
               case FFIdentifier::tSMODF:    read_smo_data(fn); break;
               case FFIdentifier::tFIC:      read_fic_data(fn); break;
               case FFIdentifier::tMDP:      read_mdp_data(fn); break;
               case FFIdentifier::tSP3:      read_sp3_data(fn); break;
               default:
                  if (verbosity) 
                     cout << "Could not determine the format of " << fn << endl;
            }
            filesRead.push_back(fn);
         }
      }
      catch (exception e)
      {
         cout << "Error reading data " << endl;
      }
   } // end of read()


   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read_msc_file(const string& fn)
   {
      if (msid == 0)
      {
         cout << "Monitor station id is not set." << endl;
         return;
      }

      MSCStream msc(fn.c_str(), ios::in);
      MSCData mscd;
      while (msc >> mscd)
      {
         if (mscd.station == msid)
         {
            antennaPosition = mscd.coordinates;
            if (verbosity>1)
               cout << "Station " << msid
                    << " location: " << antennaPosition << endl;
            break;
         }
      }
   }


   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read_rinex_obs_data(const string& fn)
   {
      RinexObsStream ros(fn.c_str(), ios::in);
      ros.exceptions(fstream::failbit);
      if (verbosity>2)
         cout << "Reading " << fn << " as RINEX obs."<< endl;

      RinexObsHeader roh;
      ros >> roh;

      DayTime t0(DayTime::END_OF_TIME);
      DayTime t1(DayTime::BEGINNING_OF_TIME);
      DayTime t2(DayTime::BEGINNING_OF_TIME);

      if (roh.valid && RinexObsHeader::antennaTypeValid)
         antennaPosition = roh.antennaPosition;

      RinexObsData rod;
      while (ros >> rod)
      {
         const DayTime& t = rod.time;
         if (t<startTime || t>stopTime)
            continue;

         oem[t] = makeObsEpoch(rod);
      }

      haveObsData = true;
      if (verbosity>1)
         cout << "Read " << fn << " as RINEX obs."<< endl;

   } // end of read_rinex_data()



   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read_mdp_data(const string& fn)
   {
      MDPStream mdps(fn.c_str(), ios::in);
      mdps.exceptions(fstream::failbit);

      if (verbosity>2)
         cout << "Reading " << fn << " as MDP."<< endl;

      MDPObsEpoch obs;
      MDPHeader header;

      if (verbosity>1)
         header.debugLevel = 1;

      while (mdps >> header)
      {
         switch (header.id)
         {
            case MDPObsEpoch::myId:
               mdps >> obs;

               const DayTime& t = obs.time;
               if (t<startTime || t>stopTime)
                  continue;

               SatID svid(obs.prn, SatID::systemGPS);
               ObsEpoch& oe = oem[t];
               oe.time = t;
               oe[svid] = makeSvObsEpoch(obs);
               break;
         }
      }

      haveObsData = true;
      if (verbosity>1)
         cout << "Read " << fn << " as MDP."<< endl;

   } // end of read_mdp_data()


   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read_smo_data(const string& fn)
   {
      SMODFStream smo(fn.c_str(), ios::in);
      smo.exceptions(fstream::failbit);
      if (verbosity>2)
         cout << "Reading " << fn << " as SMODF."<< endl;

      SMODFData smodata;

      while (smo >> smodata)
      {
         if (smodata.station != msid)
            continue;

         const DayTime& t = smodata.time;
         if (t<startTime || t>stopTime)
            continue;

         ObsEpoch& oe = oem[t];
         oe.time = t;

         SatID svid(smodata.PRNID, SatID::systemGPS);
         SvObsEpoch& soe = oe[svid];
         soe.svid=svid;

         soe[getObsID(smodata)] = smodata.obs * 1000;
      }

      haveObsData = true;
      if (verbosity>1)
         cout << "Read " << fn << " as SMODF obs."<< endl;

   } // end of read_smo_data()


   // ---------------------------------------------------------------------
   // Read in ephemeris data in rinex format
   // ---------------------------------------------------------------------
   void DataReader::read_rinex_nav_data(const string& fn)
   {
      BCEphemerisStore* bce;
      if (eph == NULL)
      {
         bce = new(BCEphemerisStore);
      }
      else
      {
         if (typeid(*eph) != typeid(BCEphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         bce = dynamic_cast<BCEphemerisStore*>(eph);
      }
      if (verbosity>2)
         cout << "Reading " << fn << " as RINEX nav."<< endl;
         
      RinexNavStream rns(fn.c_str(), ios::in);
      rns.exceptions(ifstream::failbit);
      RinexNavData rnd;
      while (rns >> rnd)
         bce->addEphemeris(rnd);

      haveEphData = true;
      if (eph==NULL)
         eph = dynamic_cast<EphemerisStore*>(bce);
      if (verbosity>1)
         cout << "Read " << fn << " as RINEX nav."<< endl;
   } // end of read_rinex_nav_data()


   void DataReader::read_fic_data(const string& fn)
   {
      BCEphemerisStore* bce;

      if (eph == NULL)
      {
         bce = new(BCEphemerisStore);
      }
      else
      {
         if (typeid(*eph) != typeid(BCEphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         bce = dynamic_cast<BCEphemerisStore*>(eph);
      }
      if (verbosity>2)
         cout << "Reading " << fn << " as FIC nav."<< endl;
      
      FICStream fs(fn.c_str(), ios::in);
      FICHeader header;
      fs >> header;
      
      FICData data;
      while(fs >> data)
         if (data.blockNum==9) // Only look at the eng ephemeris
            bce->addEphemeris(data);

      haveEphData = true;
      if (eph==NULL)
         eph = dynamic_cast<EphemerisStore*>(bce);
      if (verbosity>1)
         cout << "Read " << fn << " as FIC nav."<< endl;
   } // end of read_fic_data()

   void DataReader::read_sp3_data(const string& fn)
   {
      SP3EphemerisStore* pe;

      if (eph == NULL)
      {
         pe = new(SP3EphemerisStore);
      }
      else
      {
         if (typeid(*eph) != typeid(SP3EphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         pe = dynamic_cast<SP3EphemerisStore*>(eph);
      }
      if (verbosity>2)
         cout << "Reading " << fn << " as SP3 ephemeris."<< endl;

      SP3Stream pefile(fn.c_str(),ios::in);
      pefile.exceptions(ifstream::failbit);
      
      SP3Header header;
      pefile >> header;

      SP3Data data;
      while(pefile >> data)
         pe->addEphemeris(data);

      haveEphData = true;
      if (eph==NULL)
         eph = dynamic_cast<EphemerisStore*>(pe);
      if (verbosity>1)
         cout << "Read " << fn << " as SP3 ephemeris."<< endl;
   } // end of read_sp3_data()
}
