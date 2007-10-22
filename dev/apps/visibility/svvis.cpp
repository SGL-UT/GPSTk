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

#include "LoopedFramework.hpp"
#include "CommandOptionWithTimeArg.hpp"

#include "EphReader.hpp"
#include "MSCData.hpp"
#include "MSCStream.hpp"

#include "FFIdentifier.hpp"

using namespace std;
using namespace gpstk;

class SVVis : public BasicFramework
{
public:
   SVVis(const string& applName) throw()
   : BasicFramework(
      applName,
      "Compute when satellites are visible at a given point on the earth")
   {};

   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void spinUp() {};

   virtual void process();
   
   virtual void shutDown() {};

private:
   EphReader ephReader;
   double minElev;
   DayTime startTime, stopTime;
   Triple rxPos;
   double timeStep;
   bool printElev;
   int graphElev;
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
         "The defaut is to just to output the PRN of the sv.");
   
   if (!BasicFramework::initialize(argc,argv)) return false;

   // get the minimum elevation
   if (minElevOpt.getCount())
      minElev = StringUtils::asDouble((minElevOpt.getValue())[0]);
   else
      minElev = 0;

   // get the ephemeris source(s)
   ephReader.verboseLevel = verboseLevel;
   FFIdentifier::debugLevel = debugLevel;
   for (int i=0; i<ephFileOpt.getCount(); i++)
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
      long sow = static_cast<long>(startTime.GPSsow());
      sow -= sow % static_cast<long>(timeStep);
      startTime.setGPS(startTime.GPSfullweek(), static_cast<double>(sow));
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

   if (debugLevel)
      cout << "debugLevel: " << debugLevel << endl
           << "verboseLevel: " << verboseLevel << endl
           << "rxPos: " << rxPos << endl
           << "minElev: " << minElev << endl
           << "graphElev: " << graphElev << endl
           << "startTime: " << startTime << endl
           << "stopTime: " << stopTime << endl;
  
   return true;
}


void SVVis::process()
{
   gpstk::XvtStore<SatID>& ephStore = *ephReader.eph;
   DayTime t = startTime;

   Xvt rxXvt;
   rxXvt.x = rxPos;

   cout << "# date     time      #: ";
   for (int prn=1; prn <= MAX_PRN; prn++)
      cout << left << setw(3) << prn;
   cout << endl;

   string up, prev_up, el;
   int n_up;
   for (DayTime t=startTime; t < stopTime; t+=1)
   {
      up = "";
      el = "";
      n_up = 0;
      for (int prn=1; prn <= MAX_PRN; prn++)
      {
         try
         {
            using namespace StringUtils;
            Xvt svXvt = ephStore.getXvt(SatID(prn, SatID::systemGPS),t);
            double elev = rxXvt.x.elvAngle(svXvt.x);
            if (elev>=minElev)
            {
               up += leftJustify(asString(prn), 3);
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
      long sod=static_cast<long>(t.DOYsecond());
      if (up != prev_up || (graphElev && (sod % graphElev==0)) )
      {
         cout << t << " " << setw(2) << n_up << ": ";
         if (printElev)
            cout << el;
         else
            cout << up;
         cout << endl;
      }
      prev_up = up;
   }
}

int main(int argc, char *argv[])
{
   SVVis crap(argv[0]);

   if (!crap.initialize(argc, argv))
      exit(0);

   crap.run();
}
