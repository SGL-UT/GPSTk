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

/** @file Converts an MDP stream into RINEX obs/nav files */

#include "StringUtils.hpp"
#include "InOutFramework.hpp"

#include "RinexObsStream.hpp"
#include "RinexObsData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"

#include "MDPStream.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"

#include "RinexConverters.hpp"

using namespace std;
using namespace gpstk;


class MDP2Rinex : public InOutFramework<MDPStream, RinexObsStream>
{
public:
   MDP2Rinex(const string& applName)
      throw()
      : InOutFramework<MDPStream, RinexObsStream>(
         applName, "Converts an MDP stream to RINEX.")
   {} //MDP2Rinex::MDP2Rinex()

   bool initialize(int argc, char *argv[]) throw()
   {
      CommandOptionWithAnyArg 
         navFileOpt('n', "nav",
                    "Filename to write RINEX nav data to."),
         c2Opt('c', "l2c",
               "Enable output of L2C data in C2"),
         antPosOpt('p',"pos",
                   "Antenna position to write into obs file header. "
                   "Format as string: \"X Y Z\"."),
         thinningOpt('t', "thinning", 
                     "A thinning factor for the data, specified in seconds "
                     "between points. Default: none.");

      if (!InOutFramework<MDPStream, RinexObsStream>::initialize(argc,argv))
         return false;

      if (navFileOpt.getCount())
         rinexNavOutput.open(navFileOpt.getValue()[0].c_str(), ios::out);
      else
         rinexNavOutput.clear(ios::badbit);

      if (thinningOpt.getCount())
      {
         thin = true;
         thinning = StringUtils::asInt(thinningOpt.getValue()[0]);
         if (debugLevel)
            cout << "Thinning data modulo " << thinning << " seconds." << endl;
      }
      else
         thin = false;


      roh.valid |= RinexObsHeader::allValid21;
      roh.fileType = "Observation";
      roh.fileProgram = "mdp2rinex";
      roh.markerName = "Unknown";
      roh.observer = "Unknown";
      roh.agency = "Unknown";
      roh.antennaOffset = Triple(0,0,0);
      //roh.antennaPosition = Triple(0,0,0);
      roh.wavelengthFactor[0] = 1;
      roh.wavelengthFactor[1] = 1;
      roh.recType = "Unknown MDP";
      roh.recVers = "Unknown";
      roh.recNo = "1";
      roh.antType = "Unknown";
      roh.antNo = "1";
      roh.obsTypeList.push_back(RinexObsHeader::C1);
      roh.obsTypeList.push_back(RinexObsHeader::P1);
      roh.obsTypeList.push_back(RinexObsHeader::L1);
      roh.obsTypeList.push_back(RinexObsHeader::D1);
      roh.obsTypeList.push_back(RinexObsHeader::S1);
      roh.obsTypeList.push_back(RinexObsHeader::P2);
      roh.obsTypeList.push_back(RinexObsHeader::L2);
      roh.obsTypeList.push_back(RinexObsHeader::D2);
      roh.obsTypeList.push_back(RinexObsHeader::S2);
        
      if (antPosOpt.getCount())
      {
        double x, y, z;
        sscanf(antPosOpt.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
        antPos = Triple(x,y,z);
      }
      else
        antPos = Triple(0,0,0);

      roh.antennaPosition = antPos;

      if (c2Opt.getCount())
         roh.obsTypeList.push_back(RinexObsHeader::C2);
        
      rnh.valid = RinexNavHeader::allValid21;
      rnh.fileType = "Navigation";
      rnh.fileProgram = "mdp2rinex";
      rnh.fileAgency = "Unknown";
      rnh.version = 2.1;


      firstObs = true;;
      firstEph = true;

      MDPHeader::debugLevel = debugLevel;
      MDPHeader::hexDump = debugLevel>3;

      if (!input)
      {
         cout << "Error: could not open input." << endl;
         return false;
      }

      if (!output)
      {
         cout << "Error: could not open output." << endl;
         return false;
      }

      return true;
   } // MDP2Rinex::initialize()
   
protected:
   virtual void spinUp()
   {}

   virtual void process(MDPNavSubframe& nav)
   {
      cout << "nav ";

      if (!rinexNavOutput)
         return;

      cout << "1";
      if (firstEph)
      {
         rinexNavOutput << rnh;
         cout << "Got first nav SF" << endl;
      }


      MDPNavSubframe tmp = nav;

      // First try the data assuming it is already upright
      tmp.cooked = true;
      bool parityGood = tmp.checkParity();
      if (!parityGood)
      {
         if (debugLevel && firstEph)
            cout << "Raw subframe" << endl;
         nav.cooked = false;
         nav.cookSubframe();
         parityGood = nav.checkParity();
      }
      else
      {
         if (debugLevel && firstEph)
            cout << "Cooked subframe" << endl;
      }

      cout << "2";
      firstEph=false;

      if (!parityGood)
      {
         if (debugLevel)
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
         if (debugLevel)
            cout << "Bad week" << endl;
         return;
      }

      cout << "3";

      nav.dump(cout);
      DayTime howTime(week, sow);

      if (nav.range != rcCA || nav.carrier != ccL1)
         return;

      cout <<"4" << endl;
      NavIndex ni(RangeCarrierPair(nav.range, nav.carrier), nav.prn);
      ephData[ni] = nav;

      ephPageStore[ni][sfid] = nav;
      EngEphemeris engEph;
      if (makeEngEphemeris(engEph, ephPageStore[ni]))
      {
         RinexNavData rnd(engEph);
         rinexNavOutput << rnd;
         ephPageStore[ni].clear();
      }
   } // MDP2Rinex::process(MDPNavSubframe)


   virtual void process(MDPObsEpoch& obs)
   {

      const DayTime& t=epoch.begin()->second.time;

      if (!firstObs && t<prevTime)
      {
         if (debugLevel)
            cout << "Out of order data at " << t << endl;
         return;
      }

      if (epoch.size() > 0 && t != obs.time)
      {
         if (!thin || (static_cast<int>(t.DOYsecond()) % thinning) == 0)
         {
            if (firstObs)
            {
               roh.firstObs = t;
               output << roh;
               firstObs=false;
               if (debugLevel)
                  cout << "Got first obs" << endl;
            }

            RinexObsData rod;
            rod = makeRinexObsData(epoch);
            output << rod;
         }
         epoch.clear();
         prevTime = t;
      }
      epoch[obs.prn] = obs;
   } // MDP2Rinex::process(MDPObsEpoch)


   virtual void process()
   {
      static MDPHeader header;
      MDPNavSubframe nav;
      MDPObsEpoch obs;

      input >> header;
      
      switch (header.id)
      {
         case MDPNavSubframe::myId :
            input >> nav;
            if (!nav)
               cout << "Error decoding nav " << endl;
            else
               process(nav);
            break;
            
         case MDPObsEpoch::myId :
            input >> obs;
            if (!obs)
               cout << "Error decoding obs " << endl;
            else
               process(obs);
            break;
      }
      timeToDie |= !input;
   } // MDP2Rinex::process()

   virtual void shutDown()
   {}

private:
   RinexObsHeader roh;
   RinexNavHeader rnh;
   RinexNavStream rinexNavOutput;
   MDPEpoch epoch;

   typedef pair<RangeCode, CarrierCode> RangeCarrierPair;
   typedef pair<RangeCarrierPair, short> NavIndex;
   typedef map<NavIndex, MDPNavSubframe> NavMap;
   NavMap ephData;
   map<NavIndex, EphemerisPages> ephPageStore;
   map<NavIndex, EngEphemeris> ephStore;

   bool thin;
   int thinning;
   bool firstObs, firstEph;
   DayTime prevTime;
   Triple antPos;
};


int main(int argc, char *argv[])
{
   MDP2Rinex crap(argv[0]);
   
   if (!crap.initialize(argc, argv))
      exit(0);

   crap.run();
}
