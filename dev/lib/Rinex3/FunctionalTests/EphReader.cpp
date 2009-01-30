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

#include "XvtStore.hpp"
#include "SP3EphemerisStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "YumaAlmanacStore.hpp"
#include "SEMAlmanacStore.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"

#include "FICStream.hpp"
#include "FICData.hpp"

#include "MDPStream.hpp"
#include "MDPNavSubframe.hpp"
#include "RinexConverters.hpp"

#include "YumaStream.hpp"
#include "YumaData.hpp"

#include "SEMStream.hpp"
#include "SEMData.hpp"

#include "FFIdentifier.hpp"
#include "EphReader.hpp"

using namespace std;

namespace gpstk
{

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
   void EphReader::read(const std::string& fn)
      throw(FileMissingException)
   {
      FFIdentifier ffid(fn);

      switch (ffid)
      {
         case FFIdentifier::tRinexNav: read_rinex_nav_data(fn); break;
         case FFIdentifier::tFIC:      read_fic_data(fn);       break;
         case FFIdentifier::tSP3:      read_sp3_data(fn);       break;
         case FFIdentifier::tYuma:     read_yuma_data(fn);      break;
         case FFIdentifier::tSEM:      read_sem_data(fn);       break;
         case FFIdentifier::tMDP:      read_mdp_data(fn);       break;
         default:
            if (verboseLevel) 
               cout << "Could not determine the format of " << fn << endl;
      }

      filesRead.push_back(fn);
      if (verboseLevel>1)
         cout << "Ephemeris initial time: " << eph->getInitialTime() 
              << ", final time: " << eph->getFinalTime() << endl;
   } // end of read()


// ---------------------------------------------------------------------
// Read in ephemeris data in rinex format
// ---------------------------------------------------------------------
   void EphReader::read_rinex_nav_data(const string& fn)
   {
      GPSEphemerisStore* bce;
      if (eph == NULL)
      {
         bce = new(GPSEphemerisStore);
         eph = dynamic_cast<EphemerisStore*>(bce);
      }
      else
      {
         if (typeid(*eph) != typeid(GPSEphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         bce = dynamic_cast<GPSEphemerisStore*>(eph);
      }
      if (verboseLevel>2)
         cout << "Reading " << fn << " as RINEX nav."<< endl;
         
      RinexNavStream rns(fn.c_str(), ios::in);
      rns.exceptions(ifstream::failbit);
      RinexNavData rnd;
      while (rns >> rnd)
         bce->addEphemeris(rnd);

      if (verboseLevel>1)
         cout << "Read " << fn << " as RINEX nav. " << endl;
   } // end of read_rinex_nav_data()


   void EphReader::read_fic_data(const string& fn)
   {
      GPSEphemerisStore* bce;

      if (eph == NULL)
      {
         bce = new(GPSEphemerisStore);
         eph = dynamic_cast<EphemerisStore*>(bce);
      }
      else
      {
         if (typeid(*eph) != typeid(GPSEphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         bce = dynamic_cast<GPSEphemerisStore*>(eph);
      }
      if (verboseLevel>2)
         cout << "Reading " << fn << " as FIC nav."<< endl;
      
      FICStream fs(fn.c_str(), ios::in);
      FICHeader header;
      fs >> header;
      
      FICData data;
      while (fs >> data)
         if (data.blockNum==9) // Only look at the eng ephemeris
            bce->addEphemeris(data);

      if (verboseLevel>1)
         cout << "Read " << fn << " as FIC nav."<< endl;
   } // end of read_fic_data()


   void EphReader::read_mdp_data(const string& fn)
   {
      GPSEphemerisStore* bce;

      if (eph == NULL)
      {
         bce = new(GPSEphemerisStore);
         eph = dynamic_cast<EphemerisStore*>(bce);
      }
      else
      {
         if (typeid(*eph) != typeid(GPSEphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         bce = dynamic_cast<GPSEphemerisStore*>(eph);
      }
      if (verboseLevel>2)
         cout << "Reading " << fn << " as MDP nav."<< endl;

      MDPStream mdps(fn.c_str(), ios::in);
      MDPHeader header;
      MDPNavSubframe nav;
      typedef pair<RangeCode, CarrierCode> RangeCarrierPair;
      typedef pair<RangeCarrierPair, short> NavIndex;
      typedef map<NavIndex, MDPNavSubframe> NavMap;
      NavMap ephData;
      map<NavIndex, EphemerisPages> ephPageStore;
      map<NavIndex, EngEphemeris> ephStore;
      bool firstEph=true;

      while (mdps >> header)
         if (header.id == MDPNavSubframe::myId)
         {
            mdps >> nav;
            if (!nav)
            {
               if (mdps && verboseLevel>2)
                  cout << "Error decoding nav " << endl;
            }
            else
            {
               MDPNavSubframe tmp = nav;

               // First try the data assuming it is already upright
               tmp.cooked = true;
               bool parityGood = tmp.checkParity();
               if (!parityGood)
               {
                  if (verboseLevel>3 && firstEph)
                     cout << "Raw subframe" << endl;
                  nav.cooked = false;
                  nav.cookSubframe();
                  parityGood = nav.checkParity();
               }
               else
               {
                  if (verboseLevel>3 && firstEph)
                     cout << "Cooked subframe" << endl;
               }

               firstEph = false;

               if (!parityGood)
               {
                  if (verboseLevel>2)
                     cout << "Parity error" << endl;
                  return;
               }

               short sfid = nav.getSFID();
               if (sfid > 3)
                  return;

               short week = nav.time.GPSfullweek();
               long sow = nav.getHOWTime();
               if (sow > DayTime::FULLWEEK)
               {
                  if (verboseLevel>2)
                     cout << "Bad week" << endl;
                  return;
               }

               if (verboseLevel>3)
                  nav.dump(cout);

               DayTime howTime(week, sow);

               if (nav.range != rcCA || nav.carrier != ccL1)
                  return;

               NavIndex ni(RangeCarrierPair(nav.range, nav.carrier), nav.prn);
               ephData[ni] = nav;

               ephPageStore[ni][sfid] = nav;
               EngEphemeris engEph;
               if (makeEngEphemeris(engEph, ephPageStore[ni]))
               {
                  bce->addEphemeris(engEph);
                  ephPageStore[ni].clear();
               }
            }
         }

      if (verboseLevel>1)
         cout << "Read " << fn << " as MDP nav."<< endl;
   } // end of read_mdp_data()


   void EphReader::read_sp3_data(const string& fn)
   {
      SP3EphemerisStore* pe;

      if (eph == NULL)
      {
         pe = new(SP3EphemerisStore);
         eph = dynamic_cast<EphemerisStore*>(pe);
      }
      else
      {
         if (typeid(*eph) != typeid(SP3EphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         pe = dynamic_cast<SP3EphemerisStore*>(eph);
      }
      if (verboseLevel>2)
         cout << "Reading " << fn << " as SP3 ephemeris."<< endl;

      SP3Stream fs(fn.c_str(),ios::in);
      fs.exceptions(ifstream::failbit);
      
      SP3Header header;
      fs >> header;

      SP3Data data;
      data.version = header.version;
   
      while (fs >> data)
         pe->addEphemeris(data);

      if (verboseLevel>1)
         cout << "Read " << fn << " as SP3 ephemeris."<< endl;
   } // end of read_sp3_data()


   void EphReader::read_yuma_data(const string& fn)
   {
      YumaAlmanacStore* alm;

      if (eph == NULL)
      {
         alm = new(YumaAlmanacStore);
         eph = dynamic_cast<EphemerisStore*>(alm);
      }
      else
      {
         if (typeid(*eph) != typeid(YumaAlmanacStore))
            throw(FFStreamError("Don't mix nav data types..."));
         alm = dynamic_cast<YumaAlmanacStore*>(eph);
      }
      if (verboseLevel>2)
         cout << "Reading " << fn << " as Yuma almanc."<< endl;

      YumaStream fs(fn.c_str(),ios::in);
      fs.exceptions(ifstream::failbit);
      
      YumaHeader header;
      fs >> header;

      YumaData data;
      while (fs)
      {
         fs >> data;
         if (fs || fs.eof())
            alm->addAlmanac(data);
      }

      if (verboseLevel>1)
         cout << "Read " << fn << " as Yuma almanac."<< endl;
   } // end of read_yuma_data()


   void EphReader::read_sem_data(const string& fn)
   {
      SEMAlmanacStore* alm;

      if (eph == NULL)
      {
         alm = new(SEMAlmanacStore);
         eph = dynamic_cast<EphemerisStore*>(alm);
      }
      else
      {
         if (typeid(*eph) != typeid(SEMAlmanacStore))
            throw(FFStreamError("Don't mix nav data types..."));
         alm = dynamic_cast<SEMAlmanacStore*>(eph);
      }
      if (verboseLevel>2)
         cout << "Reading " << fn << " as SEM almanc."<< endl;

      SEMStream fs(fn.c_str(),ios::in);
      fs.exceptions(ifstream::failbit);
      
      SEMHeader header;
      fs >> header;

      SEMData data;
      while (fs)
      {
         fs >> data;
         if (fs || fs.eof())
            alm->addAlmanac(data);
      }

      if (verboseLevel>1)
         cout << "Read " << fn << " as Yuma almanac."<< endl;
   } // end of read_sem_data()

}
