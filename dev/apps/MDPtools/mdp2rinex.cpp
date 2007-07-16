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
#include "LoopedFramework.hpp"

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


class MDP2Rinex : public gpstk::LoopedFramework
{
public:
   MDP2Rinex(const std::string& applName)
      throw()
      : LoopedFramework(applName, "Converts an MDP stream to RINEX."),
        obsFileOpt('o', "obs",   "Filename to write RINEX obs data to. The filename of '-' means to use stdout.", true),
        navFileOpt('n', "nav",   "Filename to write RINEX nav data to."),
        mdpFileOpt('i', "mdp-input", "Filename to read MDP data from. The filename of '-' means to use stdin.", true),
        c2Opt('c', "l2c", "Enable output of L2C data in C2"),
        antPosOpt('p',"pos", "Antenna position to write into obs file header.  Format as string: \"X Y Z\"."),
        thinningOpt('t', "thinning", "A thinning factor for the data, specified in seconds between points. Default: none.")
   {
      navFileOpt.setMaxCount(1);
      obsFileOpt.setMaxCount(1);
      mdpFileOpt.setMaxCount(1);
      antPosOpt.setMaxCount(1);
   } //MDP2Rinex::MDP2Rinex()

   bool initialize(int argc, char *argv[]) throw()
   {
      if (!LoopedFramework::initialize(argc,argv)) return false;

      if (mdpFileOpt.getCount())
         if (mdpFileOpt.getValue()[0] != "-")
            mdpInput.open(mdpFileOpt.getValue()[0].c_str());
         else
         {
            if (debugLevel)
               cout << "Taking input from stdin." << endl;
            mdpInput.copyfmt(std::cin);
            mdpInput.clear(std::cin.rdstate());
            mdpInput.basic_ios<char>::rdbuf(std::cin.rdbuf());
         }
      

      if (obsFileOpt.getCount())
         if (obsFileOpt.getValue()[0] != "-")
            rinexObsOutput.open(obsFileOpt.getValue()[0].c_str(), std::ios::out);
         else
         {
            if (debugLevel)
               cout << "Sending output to stdout." << endl;
            rinexObsOutput.copyfmt(std::cout);
            rinexObsOutput.clear(std::cout.rdstate());
            rinexObsOutput.basic_ios<char>::rdbuf(std::cout.rdbuf());
         }

      if (navFileOpt.getCount())
         rinexNavOutput.open(navFileOpt.getValue()[0].c_str(), std::ios::out);
      else
         rinexNavOutput.clear(std::ios::badbit);

      if (thinningOpt.getCount())
      {
         thin = true;
         thinning = gpstk::StringUtils::asInt(thinningOpt.getValue()[0]);
         if (debugLevel)
            cout << "Thinning data modulo " << thinning << " seconds." << endl;
      }
      else
         thin = false;

      firstObs = true;;
      firstEph = true;

      MDPHeader::debugLevel = debugLevel;

      return true;
   } // MDP2Rinex::initialize()
   
protected:
   virtual void spinUp()
   {
      if (!mdpInput)
      {
         cout << "Error: could not open input." << endl;
         exit(-1);
      }

      mdpInput.exceptions(fstream::failbit);
      rinexObsOutput.exceptions(fstream::failbit);
      if (rinexNavOutput)
         rinexNavOutput.exceptions(fstream::failbit);
      
      roh.valid |= gpstk::RinexObsHeader::allValid21;
      roh.fileType = "Observation";
      roh.fileProgram = "mdp2rinex";
      roh.markerName = "Unknown";
      roh.observer = "Unknown";
      roh.agency = "Unknown";
      roh.antennaOffset = gpstk::Triple(0,0,0);
      //roh.antennaPosition = gpstk::Triple(0,0,0);
      roh.wavelengthFactor[0] = 1;
      roh.wavelengthFactor[1] = 1;
      roh.recType = "Unknown MDP";
      roh.recVers = "Unknown";
      roh.recNo = "1";
      roh.antType = "Unknown";
      roh.antNo = "1";
      roh.obsTypeList.push_back(gpstk::RinexObsHeader::C1);
      roh.obsTypeList.push_back(gpstk::RinexObsHeader::P1);
      roh.obsTypeList.push_back(gpstk::RinexObsHeader::L1);
      roh.obsTypeList.push_back(gpstk::RinexObsHeader::D1);
      roh.obsTypeList.push_back(gpstk::RinexObsHeader::S1);
      roh.obsTypeList.push_back(gpstk::RinexObsHeader::P2);
      roh.obsTypeList.push_back(gpstk::RinexObsHeader::L2);
      roh.obsTypeList.push_back(gpstk::RinexObsHeader::D2);
      roh.obsTypeList.push_back(gpstk::RinexObsHeader::S2);
      if (antPosOpt.getCount())
      {
        double x, y, z;
        sscanf(antPosOpt.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
        antPos = gpstk::Triple(x,y,z);
      }
      else
        antPos = gpstk::Triple(0,0,0);
        
      roh.antennaPosition = antPos;
        
      if (c2Opt.getCount())
         roh.obsTypeList.push_back(gpstk::RinexObsHeader::C2);

      rnh.valid = gpstk::RinexNavHeader::allValid21;
      rnh.fileType = "Navigation";
      rnh.fileProgram = "mdp2rinex";
      rnh.fileAgency = "Unknown";
      rnh.version = 2.1;
   } // MDP2Rinex::spinUp()


   virtual void process(MDPNavSubframe& nav)
   {
      if (!rinexNavOutput)
         return;

      if (firstEph)
      {
         rinexNavOutput << rnh;
         cout << "Got first nav SF" << endl;
      }

      firstEph=false;

      nav.cookSubframe();
      if (!nav.checkParity())
         return;

      short sfid = nav.getSFID();
      if (sfid > 3)
         return;

      short week = nav.time.GPSfullweek();
      long sow = nav.getHOWTime();
      if ( sow >604800)
         return;

      DayTime howTime(week, sow);

      if (nav.range != rcCA || nav.carrier != ccL1)
         return;

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
      if (!rinexObsOutput)
         return;

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
               rinexObsOutput << roh;
               firstObs=false;
               if (debugLevel)
                  cout << "Got first obs" << endl;
            }

            RinexObsData rod;
            rod = makeRinexObsData(epoch);
            rinexObsOutput << rod;
         }
         epoch.clear();
         prevTime = t;
      }
      epoch[obs.prn] = obs;
   } // MDP2Rinex::process(MDPObsEpoch)


   virtual void process()
   {
      MDPHeader header;
      MDPNavSubframe nav;
      MDPObsEpoch obs;

      try 
      {
         mdpInput >> header;
         switch (header.id)
         {
            case MDPNavSubframe::myId :
               mdpInput >> nav;
               if (nav.rdstate())
                  cout << "Error decoding nav " << nav.rdstate() << endl;
               else
                  process(nav);
               break;
            
            case MDPObsEpoch::myId :
               mdpInput >> obs;
               if (obs.rdstate())
                  cout << "Error decoding obs " << obs.rdstate() << endl;
               else
                  process(obs);
               break;
         }
      }
      catch (EndOfFile& e)
      {
         if (debugLevel)
            cout << e << endl;
         timeToDie = true;
      }
      timeToDie |= !mdpInput;
   } // MDP2Rinex::process()

   virtual void shutDown()
   {}

private:
   gpstk::RinexObsHeader roh;
   gpstk::RinexNavHeader rnh;
   MDPStream mdpInput;
   RinexObsStream rinexObsOutput;
   RinexNavStream rinexNavOutput;
   MDPEpoch epoch;

   typedef std::pair<gpstk::RangeCode, gpstk::CarrierCode> RangeCarrierPair;
   typedef std::pair<RangeCarrierPair, short> NavIndex;
   typedef std::map<NavIndex, gpstk::MDPNavSubframe> NavMap;
   NavMap ephData;
   std::map<NavIndex, gpstk::EphemerisPages> ephPageStore;
   std::map<NavIndex, gpstk::EngEphemeris> ephStore;

   bool thin;
   int thinning;
   bool firstObs, firstEph;
   gpstk::DayTime prevTime;
   gpstk::Triple antPos;
   gpstk::CommandOptionWithAnyArg mdpFileOpt, navFileOpt, obsFileOpt;
   gpstk::CommandOptionWithAnyArg thinningOpt, antPosOpt, c2Opt;
};


int main(int argc, char *argv[])
{
   try
   {
      MDP2Rinex crap(argv[0]);

      if (!crap.initialize(argc, argv))
         exit(0);

      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cout << exc << endl; }
   catch (std::exception &exc)
   { cout << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cout << "Caught unknown exception" << endl; }
}
