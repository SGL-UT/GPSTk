//$Id$

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
#include <set>  

#include "TimeString.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeConstants.hpp"
#include "Exception.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "Epoch.hpp"
#include "StringUtils.hpp"
#include "InOutFramework.hpp"
#include "ObsUtils.hpp"

#include "MDPStream.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"
#include "MDPSelftestStatus.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


class MDPScrubber : public InOutFramework<MDPStream, MDPStream>
{
public:
   MDPScrubber(const string& applName)
      throw()
      : InOutFramework<MDPStream, MDPStream>(
         applName, "Reverts the navigation bit streams to be upright, as transmitted from the SVs and "
         "applies a half cycle bias to the phase data when the nav data was inverted."),
        ca(ccL1, rcCA), y1(ccL1, rcYcode), y2(ccL2, rcYcode), fc(0), lateNav(9)
   {}
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   bool initialize(int argc, char *argv[]) throw()
   {
      CommandOptionWithAnyArg 
         lateNavOpt('l', "late-nav", "How late the nav data can be, in seconds. Defaults to "
                    + asString(lateNav, 1) + " seconds.");
      
      if (!InOutFramework<MDPStream, MDPStream>::initialize(argc,argv))
         return false;

      if (lateNavOpt.getCount())
         lateNav = asDouble(lateNavOpt.getValue()[0]);

      return true;
   }
#pragma clang diagnostic pop
protected:
   virtual void spinUp(){}
   virtual void process();
   virtual void shutDown();
   void processObs();
   void processNav();
   void processPVT();
   void processSTS();

   MDPObsEpoch::ObsKey ca;
   MDPObsEpoch::ObsKey y1;
   MDPObsEpoch::ObsKey y2;
   typedef pair<MDPObsEpoch::ObsKey, short> NavIndex;
   typedef map<NavIndex, MDPNavSubframe> NavMap;
   NavMap ephData;
   map<NavIndex, bool> prevObsInv;

   MDPEpoch epoch;  // Where we accumulate all the SVs for a single epoch

   void dumpNavMap(const CommonTime& t, const NavMap& nm);
   string nmsf2str(const NavMap& nm, const NavIndex& k, const CommonTime& t);

   // Used to characterize the time delta between the nav data and the obs data in the 
   // Z     cnt
   map<long, long> dtMap;

   CommonTime obsTime;
   unsigned long fc;
   double lateNav;
};


int main(int argc, char *argv[])
{
   MDPScrubber crap(argv[0]);
   
   if (!crap.initialize(argc, argv))
      exit(0);
   
   crap.run();
}


void MDPScrubber::process()
{
   MDPHeader header;

   while (!input.eof())     
   {
      input >> header;

      if (!input)
         break;
   
      switch (input.header.id)
      {
         case gpstk::MDPObsEpoch::myId:       processObs(); break;
         case gpstk::MDPNavSubframe::myId:    processNav(); break;
         case gpstk::MDPPVTSolution::myId:    processPVT(); break;
         case gpstk::MDPSelftestStatus::myId: processSTS(); break;
      }
   }
   timeToDie=true;
}

void MDPScrubber::shutDown()
{
   if (debugLevel)
   {
      cout << "dt map:" << endl;
      map<long, long>::const_iterator i;
      for (i = dtMap.begin(); i != dtMap.end(); i++)
         cout << setw(5) << i->first << "  " << i->second << endl;
   }
}


void MDPScrubber::processObs()
{
   MDPObsEpoch obs;
   input >> obs;
   if (!obs) return;

   // If we have an epoch of data and the current obs is from a different time
   // output the data we have.
   if (epoch.size() > 0 && epoch.begin()->second.time != obs.time)
   {
      MDPEpoch::iterator i;
      for (i=epoch.begin(); i != epoch.end(); i++)
      {
         i->second.numSVs = epoch.size();
         i->second.freshnessCount = fc++;
         output << i->second;
      }
      if (debugLevel)
         dumpNavMap(epoch.begin()->second.time, ephData);
      epoch.clear();
   }

   MDPObsEpoch newObs=obs;
   newObs.obs.clear();
   
   MDPObsEpoch::ObsMap& obsmap = obs.obs;
    #pragma unused(obsmap)
   MDPObsEpoch::ObsMap::iterator j;
   for (j=obs.obs.begin(); j != obs.obs.end(); j++)
   {
      MDPObsEpoch::ObsKey ok(j->first);
      CarrierCode cc(ok.first);
      RangeCode rc(ok.second);

      if (rc == rcCMCL)
      {
         // Since we don't really have L2C nav at the moment, just keep those obs as-is
         newObs.obs[ok] = j->second;
         continue;
      }

      NavIndex ni(j->first, obs.prn);
      NavMap::const_iterator sfi = ephData.find(ni);
      
      if (sfi == ephData.end())
      {
         if (debugLevel>1)
            cout << printTime(obs.time, "%02d/%02m/%02y %02H:%02M:%05.2f") << " "
                 << obs.prn << " el:" << obs.elevation << " " << cc << ":" << rc
                 << " No subframe data. Dropping obs." << endl;
         continue;
      }

      const MDPNavSubframe& sf = sfi->second;
      if (sf.nav!=ncICD_200_2)
      {
         cout << printTime(obs.time, "%02d/%02m/%02y %02H:%02M:%05.2f") << " "
              << obs.prn << " " << cc << ":" << rc << " el:" << obs.elevation << " "
              << " Don't know how to handle this nav data. Dropping obs." << endl;
         continue;
      }

      short week = static_cast<GPSWeekSecond>(sf.time).week;
      long sow = sf.getHOWTime();
      CommonTime sft(GPSWeekSecond(week, sow));
      double dt = obs.time - sft;
      long z = dt/1.5;
      dtMap[z] += 1;

      if (dt > lateNav || dt < -3)
      {
         if (dt < 900 && debugLevel>1) // if the subframe is more than 15 minutes old, we are probably on a new pass
            cout << printTime(obs.time, "%02d/%02m/%02y %02H:%02M:%04.1f") << " "
                 << obs.prn << " " << cc << ":" << rc << " el:" << obs.elevation << " "
                 << " dt:" << dt << " Dropping obs." << endl;
         continue;
      }

      newObs.obs[ok] = j->second;

      if (prevObsInv[ni] && !sf.inverted && debugLevel)
         cout << printTime(obs.time, "%02d/%02m/%02y %02H:%02M:%04.1f") << " "
              << obs.prn << " " << cc << ":" << rc << " el:" << obs.elevation << " "
              << " Inversion change." << endl;

      if (sf.inverted)
      {
         newObs.obs[ok].phase += 0.5;
         prevObsInv[ni] = true;
      }
      else
         prevObsInv[ni] = false;
   }

   // Only output obs with at least C/A, Y1, and Y2
   if (newObs.obs.find(ca) == newObs.obs.end() ||
       newObs.obs.find(y1) == newObs.obs.end() ||
       newObs.obs.find(y2) == newObs.obs.end())
   {
      if (debugLevel>1)
         cout << printTime(obs.time, "%02d/%02m/%02y %02H:%02M:%04.1f") << " "
              << obs.prn << " el:" << obs.elevation 
              << "A code is missing. Dropping obs." << endl;
      return;
   }      

   // we have to use an insert here because this is a multi-map to
   // allow for the same SV to be tracked on multiple channels.
   epoch.insert(pair<const int, MDPObsEpoch>(newObs.prn, newObs));
}


void MDPScrubber::processNav()
{
   MDPNavSubframe nav;
   input >> nav;
   if (!nav) return;

   MDPNavSubframe unmolested = nav;
   unmolested.freshnessCount = fc++;
   nav.cookSubframe();
   if (debugLevel>2)
      nav.dump(cout);

   if (!nav.parityGood)
   {
      output << unmolested;
      return;
   }

   NavIndex ni(MDPObsEpoch::ObsKey(nav.carrier, nav.range), nav.prn);
   ephData[ni] = nav;

   // set the nav upright
   if (nav.inverted)
      for (int i=1; i<=10; i++)
         unmolested.subframe[i] ^= 0x3fffffff;
   output << unmolested;
}


string MDPScrubber::nmsf2str(const NavMap& nm, const NavIndex& k, const CommonTime& t)
{
   NavMap::const_iterator sfi = nm.find(k);
   if (sfi == nm.end())
      return "x";
   else
   {
      const MDPNavSubframe& sf = sfi->second;      
      short week = static_cast<GPSWeekSecond>(sf.time).week;
      long sow = sf.getHOWTime();
      CommonTime sft(GPSWeekSecond(week, sow));
      double dt = t - sft;
      if (dt > lateNav || dt < -3)
         return "-";
      else
         return sf.inverted ? "v" : "^";
   }
}


void MDPScrubber::dumpNavMap(const CommonTime& t, const NavMap& nm)
{
   cout << printTime(t, "%02d/%02m/%02y %02H:%02M:%04.1f") << "  ";
   for (int prn=1; prn<=32; prn++)
      cout << nmsf2str(nm, NavIndex(ca, prn), t)
           << nmsf2str(nm, NavIndex(y1, prn), t)
           << nmsf2str(nm, NavIndex(y2, prn), t) << " ";
   cout << endl;
}


void MDPScrubber::processPVT()
{
   MDPPVTSolution pvt;
   input >> pvt;
   if (!pvt) return;
   pvt.freshnessCount = fc++;
   output << pvt;
}


void MDPScrubber::processSTS()
{
   MDPSelftestStatus sts;
   input >> sts;
   if (!sts) return;
   sts.freshnessCount = fc++;
   output << sts;
}
