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

#include <string>
#include <sstream>

#include <list>
#include <map>

#include "TimeString.hpp"
#include "Epoch.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeConstants.hpp"
#include "Exception.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"

#include "StringUtils.hpp"
#include "InOutFramework.hpp"

#include "MDPStream.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"

#include "AshtechStream.hpp"
#include "AshtechMBEN.hpp"
#include "AshtechPBEN.hpp"
#include "AshtechALB.hpp"
#include "AshtechEPB.hpp"

#include "ObsUtils.hpp"
#include "RinexConverters.hpp"

using namespace std;
using namespace gpstk;


class Ashtech2MDP : public InOutFramework<AshtechStream, MDPStream>
{
public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
    Ashtech2MDP(const string& applName)
      throw()
      : week(-1),
        InOutFramework<AshtechStream,MDPStream>(
           applName, "Converts Ashtech Z(Y)-12 serial streaming format to "
           "MDP format. This makes for a good input to mdp2rinex for generating "
           "RINEX data from ashtech data.")
   {}
#pragma clang diagnostic pop
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   bool initialize(int argc, char *argv[]) throw()
    {
       CommandOptionWithNumberArg weekOption(
          'w', "week",
          "The full GPS week in which this data starts");

       CommandOptionWithAnyArg clipOption(
          'c', "clip",
          "A string that specifies a section of the file to remove. Example: "
          "-c 123-456 will remove bytes starting with 123 through 456.");

       CommandOptionNoArg smoothOption(
          's', "smooth",
          "Incorporate the ashtech smoothing for the pseudorange");

       if (!InOutFramework<AshtechStream, MDPStream>::initialize(argc,argv))
         return false;

      if (weekOption.getCount())
         week = StringUtils::asInt(weekOption.getValue()[0]);

      AshtechData::debugLevel = debugLevel>1?debugLevel-1:0;
      if (debugLevel>2)
         AshtechData::hexDump = true;

      if (debugLevel>4)
         MDPHeader::hexDump = true;

      for (size_t i=0; i<clipOption.getCount(); i++)
      {
         string s=clipOption.getValue()[i];
         int sep = s.find_first_not_of("0123456789kKMG");
         unsigned long start = StringUtils::asUnsigned(s.substr(0,sep));
         unsigned long end = StringUtils::asUnsigned(
            s.substr(sep+1,std::string::npos));
         cutList.push_back( pair< unsigned long, unsigned long>(start, end) );
      }

      removeSmoothing = smoothOption.getCount();

      if (debugLevel)
      {
         cout << "Removing bytes:";
         for (CutList::const_iterator i=cutList.begin(); i != cutList.end(); i++)
            cout << " " << i->first << "..." << i->second;
         cout << endl;
         if (removeSmoothing)
            cout << "Removing smoothing from pseudoranges" << endl;
      }

      return true;
   }
#pragma clang diagnostic pop
protected:
   virtual void spinUp()
   {}

   virtual void process()
   {
      bool knowSOW(false), knowWeek(false);

      if (week>0)
      {
         knowWeek = true;
         time.week = week;
      }

      AshtechData hdr;
      AshtechPBEN pben;
      AshtechMBEN mben;
      AshtechEPB  epb;
      AshtechALB  alb;
      unsigned short fc=0;
      vector<MDPObsEpoch> hint(33);
      short svCount = 0;
      double dt = 0;  // Used to keep track of the _expected_ time between pvt messages
      long pben_count = 0;

      while (input >> hdr)
      {
         if (debugLevel>2)
            cout << "---" << endl;

         bool skip=false;
         CutList::const_iterator i;
         for (i = cutList.begin(); i != cutList.end() && !skip; i++)
         {
            size_t cb=input.tellg();
            skip = cb >= i->first && cb <= i->second;
            if (skip && debugLevel)
               cout << "x:" << cb << ",";
         }
         if (skip)
            continue;

         if (pben.checkId(hdr.id) && (input >> pben) && pben)
         {
            if (debugLevel>2)
               pben.dump(cout);

            pben_count++;

            svCount = 0;

            // If we don't know what week it is, we really can't do much...
            if (!knowWeek)
               continue;

            if (pben_count==1)
            {
               time.sow = pben.sow;
               continue;
            }

            knowSOW = true;
            
            double this_dt = pben.sow - time.sow;
            time.sow = pben.sow;

            if (dt==0 && this_dt>0)
               dt = this_dt;
            else if (this_dt <= 0 && -this_dt < HALFWEEK)
            {
               // It looks like something is garbled
               dt = 0;
               knowSOW = false;
               continue;
            }
            else if (dt>0 && -this_dt > HALFWEEK)
            {
               time.week++;
               if (debugLevel)
                  cout << "Bumped week." << this_dt << endl;
            }

            if (debugLevel)
               cout << "PVT time: "
                    << printTime(GPSWeekSecond(time.week, time.sow),"%03j %02H:%02M:%04.1f")
                    << endl;

            MDPPVTSolution pvt = makeMDPPVTSolution(pben, time.week);
            pvt.freshnessCount = fc++;
            output << pvt << flush;
            if (debugLevel>3)
               pvt.dump(cout);
         }
         else if (mben.checkId(hdr.id) && (input >> mben) && mben)
         {
            if (debugLevel>2)
               mben.dump(cout);
            if (svCount==0)
               svCount = mben.left+1;

            if (knowSOW && knowWeek)
            {
               hint[mben.svprn].time = GPSWeekSecond(time.week, time.sow);
               hint[mben.svprn].numSVs = svCount;
               MDPObsEpoch moe = makeMDPObsEpoch(mben, hint[mben.svprn], removeSmoothing);
               moe.freshnessCount = fc++;
               hint[mben.svprn] = moe;
               output << moe << flush;
               if (debugLevel>3)
                  moe.dump(cout);
            }
         }
         else if (epb.checkId(hdr.id) && (input >> epb) && epb)
         {
            if (debugLevel>2)
               epb.dump(cout);
            MDPNavSubframe sf;
            if (!knowWeek)
            {
               EphemerisPages ephPageStore;
               for (int s=1; s<=3; s++)
               {
                  for (int w=1; w<=10; w++)
                     sf.subframe[w] = epb.word[s][w];
                  ephPageStore[s] = sf;
               }
               EngEphemeris engEph;
               if (makeEngEphemeris(engEph, ephPageStore))
               {
                  int week10 = 0x3ff & engEph.getFullWeek();
                  SystemTime now;
                  if (debugLevel)
                     cout << now << endl;
                  GPSWeekSecond gs(now);
                  time.week = (gs.week & ~0x3ff) | week10;
                  if (debugLevel)
                     cout << "week is " << time.week << endl;
                  knowWeek=true; 
               }
               else
                  continue;
            }



            sf.carrier = ccL1;
            sf.range = rcCA;
            sf.nav = ncICD_200_2;
            sf.prn = epb.prn;
            for (int s=1; s<=3; s++)
            {
               for (int w=1; w<=10; w++)
                  sf.subframe[w] = epb.word[s][w];
               long sow = sf.getHOWTime();
               if (sow>FULLWEEK || sow<0)
                  continue;
               CommonTime t = CommonTime(GPSWeekSecond(time.week, sf.getHOWTime())) - 6;
               sf.freshnessCount = fc++;
               sf.time = t;
               output << sf << flush;
               if (debugLevel>3)
                  sf.dump(cout);
            }
         }
         else if (alb.checkId(hdr.id) && (input >> alb))
         {
            MDPNavSubframe sf;
            if (debugLevel>3)
               alb.dump(cout);
         }
      }

      timeToDie = true;
   }

   virtual void shutDown()
   {}

   bool removeSmoothing;
   GPSWeekSecond time;
   int week;
   typedef  list< pair<unsigned long,unsigned long> >CutList;
   CutList cutList;
};


int main(int argc, char *argv[])
{
   Ashtech2MDP crap(argv[0]);
   
   if (!crap.initialize(argc, argv))
      exit(0);
   
   crap.run();
}
