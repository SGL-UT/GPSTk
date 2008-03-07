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

#include <string>
#include <sstream>

#include <list>
#include <map>

#include "DayTime.hpp"
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
   Ashtech2MDP(const string& applName)
      throw()
      : week(-1),
        InOutFramework<AshtechStream,MDPStream>(
           applName, "Converts Ashtech Z(Y)-12 serial streaming format to "
           "MDP format.")
   {}

   bool initialize(int argc, char *argv[]) throw()
    {
       CommandOptionWithNumberArg weekOption(
          'w', "week",
          "The full GPS week in which this data starts");

      if (!InOutFramework<AshtechStream, MDPStream>::initialize(argc,argv))
         return false;

      if (weekOption.getCount())
         week = StringUtils::asInt(weekOption.getValue()[0]);

      AshtechData::debugLevel = debugLevel;
      if (debugLevel>2)
         AshtechData::hexDump = true;

      if (debugLevel>4)
         MDPHeader::hexDump = true;

      return true;
   }
   
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

      while (input >> hdr)
      {
         if (debugLevel>2)
            cout << "---" << endl;
         if (pben.checkId(hdr.id) && (input >> pben) && pben)
         {
            if (debugLevel>2)
               pben.dump(cout);

            svCount = 0;

            // If we don't know what week it is, we really can't do much...
            if (!knowWeek)
               continue;

            if (!knowSOW)
            {
               knowSOW=true;
               time.sow = pben.sow;
               if (debugLevel)
                  cout << "sow is: " << time.sow << endl;
            }
            else
            {
               double dt = pben.sow - time.sow;
               time.sow = pben.sow;
               if (std::abs(dt) > HALFWEEK)
               {
                  time.week++;
                  if (debugLevel)
                     cout << "Bumped week. Time is now " << time << " (dt:" << dt << ")" << endl;
               }
            }

            if (knowSOW && knowWeek)
            {
               MDPPVTSolution pvt = makeMDPPVTSolution(pben, time.week);
               pvt.freshnessCount = fc++;
               output << pvt << flush;
               if (debugLevel>3)
                  pvt.dump(cout);
            }
         }
         else if (mben.checkId(hdr.id) && (input >> mben) && mben)
         {
            if (debugLevel>2)
               mben.dump(cout);
            if (svCount==0)
               svCount = mben.left+1;

            if (knowSOW && knowWeek)
            {
               hint[mben.svprn].time = DayTime(time.week, time.sow);
               hint[mben.svprn].numSVs = svCount;
               MDPObsEpoch moe = makeMDPObsEpoch(mben, hint[mben.svprn]);
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
                  DayTime now;
                  time.week = (now.GPSfullweek() & ~0x3ff) | week10;
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
               DayTime t = DayTime(time.week, sf.getHOWTime()) - 6;
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

   GPSWeekSecond time;
   int week;
};


int main(int argc, char *argv[])
{
   Ashtech2MDP crap(argv[0]);
   
   if (!crap.initialize(argc, argv))
      exit(0);
   
   crap.run();
}
