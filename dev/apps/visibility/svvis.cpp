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

#include <iostream>

#include <set>
#include <list>
#include <vector>
#include <utility>
#include <algorithm>

#include "BasicFramework.hpp"
#include "CommandOptionWithTimeArg.hpp"

#include "EphReader.hpp"
#include "MSCData.hpp"
#include "MSCStream.hpp"

#include "FFIdentifier.hpp"

#include "GPSWeekSecond.hpp"
#include "TimeString.hpp"
#include "YDSTime.hpp"

using namespace std;
using namespace gpstk;

class TrackData
{
public:
   TrackData(const SatID& s, const CommonTime& u, const CommonTime& d, float el)
         :sat(s), up(u), down(d), maxEl(el) {}
   TrackData& update(const CommonTime& t, float el)
   { down = t; maxEl = (maxEl > el ? maxEl : el); return *this; }
   bool operator<(const TrackData& otr) const;
   bool operator==(const TrackData& otr) const
   { return (sat == otr.sat && up == otr.up); }


   SatID sat;
   CommonTime up;
   CommonTime down;
   float maxEl;
};

bool TrackData::operator<(const TrackData& otr) const
{
   if (up == otr.up)
      return (sat < otr.sat);
   return (up < otr.up);
}

class SVVis : public BasicFramework
{
public:
   SVVis(const string& applName) throw()
   : BasicFramework(
      applName,
      "Compute when satellites are visible at a given point on the earth"),
     timeFormat("%4Y %03j %02H:%02M:%02S")
   {};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   bool initialize(int argc, char *argv[]) throw();
#pragma clang diagnostic pop
protected:
   virtual void spinUp() {};

   virtual void process();
   
   virtual void shutDown() {};

private:
   EphReader ephReader;
   double minElev;
   CommonTime startTime, stopTime;
   Triple rxPos;
   double timeStep;
   bool printElev;
   int graphElev;
   bool riseSet;
   bool tabular;
   string timeFormat;
};


bool SVVis::initialize(int argc, char *argv[]) throw()
{
   
   CommandOptionWithAnyArg 
      minElevOpt(
         '\0', "elevation-mask",
         "The elevation above which an SV is visible. The default is 0 degrees."),

      rxPosOpt(
         'p', "position",
         "Receiver antenna position in ECEF (x,y,z) coordinates.  Format as "
         "a string: \"X Y Z\"."),
         
      ephFileOpt(
         'e', "eph",
         "Where to get the ephemeris data. Can be "
         + EphReader::formatsUnderstood() + ".", true),

      mscFileOpt(
         'c', "msc",
         "Station coordinate file."),

      msidOpt(
         'm', "msid",
         "Station number to use from the msc file."),

      graphElevOpt(
         '\0', "graph-elev",
         "Output data at the specified interval. Interval is in seconds."),

      timeSpanOpt(
         'l', "time-span",
         "How much data to process, in seconds. Default is 86400.");

   CommandOptionWithTimeArg
      startTimeOpt(
         '\0', "start-time", "%4Y/%03j/%02H:%02M:%05.2f",
         "When to start computing positions. The default is the start of the "
         "ephemers data. (%4Y/%03j/%02H:%02M:%05.2f)"),

      stopTimeOpt(
         '\0',  "stop-time", "%4Y/%03j/%02H:%02M:%05.2f",
         "When to stop computing positions. The default is one day after "
         "the start time");

   CommandOptionNoArg
      printElevOpt(
         '\0', "print-elev",
         "Print the elevation of the sv at each change in tracking. "
         "The defaut is to just to output the PRN of the sv."),

      riseSetOpt(
           '\0', "rise-set",
           "Print the visibility data by PRN in rise-set pairs."),
      
      tabularOpt(
         '\0', "tabular",
         "Print the visibility data in a tabular format."),
      
      recentDataOpt(
         '\0', "recent-eph",
         "Use this if the ephemeris data provided uses 10-bit GPS weeks "
         "and it should be converted to the current epoch or to the "
         "epoch current to the \"start-time\", if specified.");

   CommandOptionMutex outputType;
   outputType.addOption(&riseSetOpt);
   outputType.addOption(&tabularOpt);
   
   if (!BasicFramework::initialize(argc,argv)) return false;

   if (recentDataOpt.getCount())
   {
      CommonTime t;
      t.setTimeSystem(TimeSystem::GPS);
      if (startTimeOpt.getCount())
         t = startTimeOpt.getTime()[0];
      EphReader::modify10bitWeeks(static_cast<GPSWeekSecond>(t).week);
   }

   // get the minimum elevation
   if (minElevOpt.getCount())
      minElev = StringUtils::asDouble((minElevOpt.getValue())[0]);
   else
      minElev = 0;

      // get the ephemeris source(s)
   ephReader.verboseLevel = verboseLevel;
   FFIdentifier::debugLevel = debugLevel;
   for (size_t i=0; i<ephFileOpt.getCount(); i++)
      ephReader.read(ephFileOpt.getValue()[i]);
   if (ephReader.eph == NULL)
   {
      cout << "Didn't get any ephemeris data from the eph files. "
           << "Exiting." << endl;
      exit(-1);
   }

   if (debugLevel)
      ephReader.eph->dump(cout, debugLevel-1);

   // get the antenna position
   bool haveRxPos = false;
   if (rxPosOpt.getCount())
   {
      double x,y,z;
      sscanf(rxPosOpt.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
      rxPos[0] = x;
      rxPos[1] = y;
      rxPos[2] = z;
      haveRxPos = true;
   }
   else if (msidOpt.getCount() && mscFileOpt.getCount())
   {
      long msid = StringUtils::asUnsigned(msidOpt.getValue()[0]);
      string fn = mscFileOpt.getValue()[0];
      MSCStream mscs(fn.c_str(), ios::in);
      MSCData mscd;
      while (mscs >> mscd)
      {
         if (mscd.station == msid)
         {
            rxPos = mscd.coordinates;
            haveRxPos=true;
            break;
         }
      }
      if (!haveRxPos)
         cout << "Did not find station " << msid << " in " << fn << "." << endl;
   }

   if (!haveRxPos)
      return false;

   timeStep=900;

   if (startTimeOpt.getCount())
      startTime = startTimeOpt.getTime()[0];
   else
   {
      startTime = ephReader.eph->getInitialTime();
      long sow = static_cast<long>(static_cast<GPSWeekSecond>(startTime).sow);
      sow -= sow % static_cast<long>(timeStep);
      startTime = GPSWeekSecond(static_cast<GPSWeekSecond>(startTime).week, static_cast<double>(sow));
      startTime += timeStep;
   }

   if (stopTimeOpt.getCount())
      stopTime = stopTimeOpt.getTime()[0];
   else
      stopTime = ephReader.eph->getFinalTime();

   if (timeSpanOpt.getCount())
   {
      double dt = StringUtils::asDouble(timeSpanOpt.getValue()[0]);
      stopTime = startTime + dt;
   }



   if (graphElevOpt.getCount())
      graphElev = StringUtils::asInt(graphElevOpt.getValue()[0]);
   else
      graphElev = 0;

   printElev = printElevOpt.getCount() > 0;
   
   riseSet = riseSetOpt.getCount() > 0;
   
   tabular = tabularOpt.getCount() > 0;

   if (debugLevel)
      cout << "debugLevel: " << debugLevel << endl
           << "verboseLevel: " << verboseLevel << endl
           << "rxPos: " << rxPos << endl
           << "minElev: " << minElev << endl
           << "graphElev: " << graphElev << endl
           << "startTime: " << printTime(startTime, timeFormat) << endl
           << "stopTime: " << printTime(stopTime, timeFormat) << endl;
  
   return true;
}


void SVVis::process()
{
   gpstk::XvtStore<SatID>& ephStore = *ephReader.eph;
   CommonTime t = startTime;
   t.setTimeSystem(TimeSystem::GPS);
   Xvt rxXvt;
   rxXvt.x = rxPos;
   typedef map<int, TrackData> TrackDataMap;
   TrackDataMap lastTrack, thisTrack;
   typedef set<TrackData> TrackDataSet;
   TrackDataSet passes;

   typedef list<TrackData> RiseSetList;
   typedef vector<RiseSetList> PRNRiseSets;
   PRNRiseSets prs;

   if (riseSet)
   {
      prs.resize(MAX_PRN + 1);
   }
   else if (!tabular)
   {
      cout << "# date     time      #: ";
      for (int prn=1; prn <= MAX_PRN; prn++)
         cout << left << setw(3) << prn;
      cout << endl;
   }

   string up, prev_up, el;
   int n_up;
   startTime.setTimeSystem(TimeSystem::GPS);
   stopTime.setTimeSystem(TimeSystem::GPS); 
   for (CommonTime t=startTime; t < stopTime; t+=1)
   {
      thisTrack.clear();
      up = "";
      el = "";
      n_up = 0;
      SatID sat(1, SatID::systemGPS);
      for (sat.id = 1; sat.id <= MAX_PRN; sat.id++)
      {
         try
         {
            using namespace StringUtils;
            Xvt svXvt = ephStore.getXvt(sat, t);
            double elev = rxXvt.x.elvAngle(svXvt.x);
            if (elev>=minElev)
            {
               if (riseSet || tabular)
               {
                  TrackDataMap::iterator tdmi = lastTrack.find(sat.id);
                  if (tdmi == lastTrack.end())
                  {
                        // the PRN just came up
                     thisTrack.insert(make_pair(sat.id, TrackData(sat, t, stopTime, elev)));
                  }
                  else
                  {
                        // the PRN is already in track
                     tdmi->second.update(t, elev);
                     thisTrack.insert(*tdmi);
                     lastTrack.erase(tdmi);
                  }
               }
               
               up += leftJustify(asString(sat.id), 3);
               el += leftJustify(asString(elev,0), 3);
               n_up++;
            }
            else
            {
               up += "   ";
               el += "   ";
            }
         }
         catch(gpstk::Exception& e)
         {
            up += " ? ";
            el += " ? ";
            if (debugLevel)
               cout << e << endl;
         }
      }

      if (riseSet)
      {
            // Move the objects left in lastTrack to the track collection.
         for(TrackDataMap::const_iterator tdmci = lastTrack.begin();
             tdmci != lastTrack.end(); tdmci++)
         {
            prs[tdmci->second.sat.id].push_back(tdmci->second);
         }
         lastTrack = thisTrack;         
      }
      else if (tabular)
      {
            // Move the objects left in lastTrack to the track collection.
         for(TrackDataMap::const_iterator tdmci = lastTrack.begin();
             tdmci != lastTrack.end(); tdmci++)
         {
            passes.insert(passes.end(), tdmci->second);
         }
         lastTrack = thisTrack;
      }
      else
      {
         long sod=static_cast<long>(static_cast<YDSTime>(t).sod);
         if (up != prev_up || (graphElev && (sod % graphElev==0)) )
         {
            cout << t << " " << setw(2) << n_up << ": ";
            if (printElev)
               cout << el;
            else
               cout << up;
            cout << endl;
         }
      }
      prev_up = up;
   }

   if (riseSet)
   {
      RiseSetList::const_iterator rsli;
      for (size_t prn = 1; prn < prs.size(); prn++)
      {
         cout << setw(2) << left << prn;
         const RiseSetList& rsl = prs[prn];
         for (rsli = rsl.begin(); rsli != rsl.end(); rsli++)
            cout << " (" << printTime(rsli->up,timeFormat) 
                 << ", " << printTime(rsli->down,timeFormat) << ")";
         cout << endl;
      }
   }
   else if (tabular)
   {
      cout << "SEARCH_INTERVAL: " << printTime(startTime,timeFormat)
           << " to " << printTime(stopTime,timeFormat)
           << endl
           << "ELEVATION_CUTOFF: " << setprecision(3) << fixed << minElev 
           << endl
           << "#     Rise (Yr DOY HMS) Set  (Yr DOY HMS) El Sys          Parameters"
           << endl;

      TrackDataSet::const_iterator tdsi;
      for (tdsi = passes.begin(); tdsi != passes.end(); tdsi++)
      {
         cout << "PASS: "
              << printTime(tdsi->up,timeFormat) << " "
              << printTime(tdsi->down,timeFormat) << " "
              << setfill('0') << right << setw(2)
              << static_cast<int>(tdsi->maxEl) << " "
              << setfill(' ') << left << setw(13) 
              << SatID::convertSatelliteSystemToString(tdsi->sat.system)
              << "PRN=" << setfill('0') << right << setw(2) << tdsi->sat.id
              << endl;            
      }
   }
}

int main(int argc, char *argv[])
{
   SVVis crap(argv[0]);

   if (!crap.initialize(argc, argv))
      exit(0);

   crap.run();
}
