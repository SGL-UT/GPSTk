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
   long msid;
   Triple rxPos;
   double timeStep;
};


bool SVVis::initialize(int argc, char *argv[]) throw()
{
   CommandOptionWithAnyArg 
      minElevOpt(
         'm', "min-elev",
         "Give an integer for the elevation (degrees) above which you want to find more than 12 SVs at a given time.", true),

      rxPosOpt(
         'p', "position",
         "Receiver antenna position in ECEF (x,y,z) coordinates.  Format as a string: \"X Y Z\".",false),
         
      ephFileOpt(
         'e', "eph",
         "Where to get the ephemeris data. Can be rinex, fic, or sp3", true),

      mscFileOpt(
         'c', "msc",
         "Station coordinate file"),

      msidOpt(
         'm', "msid",
         "Station to process data for. Used to select a station position from the msc file."),

      timeSpanOpt(
         'l', "time-span",
         "How much data to process, in seconds");

   CommandOptionWithTimeArg
      startTimeOpt(
         '\0', "start-time", "%4Y/%03j/%02H:%02M:%05.2f",
         "Ignore data before this time. (%4Y/%03j/%02H:%02M:%05.2f)"),

      stopTimeOpt(
         '\0',  "stop-time", "%4Y/%03j/%02H:%02M:%05.2f",
         "Ignore any data after this time");
   
   if (!BasicFramework::initialize(argc,argv)) return false;

   // get the minimum elevation
   if (minElevOpt.getCount())
      minElev = StringUtils::asDouble((minElevOpt.getValue())[0]);
   else
      minElev = 0;

   // get the ephemeris source(s)
   ephReader.verboseLevel = verboseLevel;
   for (int i=0; i<ephFileOpt.getCount(); i++)
      ephReader.read(ephFileOpt.getValue()[i]);
   if (ephReader.eph == NULL)
   {
      cout << "Didn't get any ephemeris data from the eph files. "
           << "Exiting." << endl;
      exit(-1);
   }

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
   else if (msid && mscFileOpt.getCount())
   {
      string fn = mscFileOpt.getValue()[0];
      if (verboseLevel)
         cout << "Reading " << fn << " as MSC data." << endl;
      MSCStream mscs(fn.c_str(), ios::in);
      MSCData mscd;
      while (mscs >> mscd)
      {
         if (mscd.station == msid)
         {
            rxPos = mscd.coordinates;
            if (verboseLevel>1)
               cout << "Antenna position read from MSC file:"
                    << rxPos << " (msid: "
                    << msid << ")" << endl;
            haveRxPos=true;
            break;
         }
      }
      if (!haveRxPos)
         cout << "Did not find station " << msid << " in " << fn 
              << "." << endl;
   }
   if (!haveRxPos)
      return false;

   if (startTimeOpt.getCount())
      startTime = startTimeOpt.getTime()[0];
   else
      startTime = ephReader.eph->getInitialTime();

   if (stopTimeOpt.getCount())
      stopTime = stopTimeOpt.getTime()[0];
   else
      stopTime = ephReader.eph->getFinalTime();

   if (timeSpanOpt.getCount())
   {
      double dt = StringUtils::asDouble(timeSpanOpt.getValue()[0]);
      stopTime = startTime + dt;
   }

   timeStep=900;

   if (debugLevel)
      cout << "debugLevel: " << debugLevel << endl
           << "verboseLevel: " << verboseLevel << endl
           << "rxPos: " << rxPos << endl
           << "minElev: " << minElev << endl
           << "startTime: " << startTime << endl
           << "stopTime: " << stopTime << endl;
  
   return true;
}


void SVVis::process()
{
   gpstk::EphemerisStore& ephStore = *ephReader.eph;
   DayTime t = startTime;

   Xvt rxXvt;
   rxXvt.x = rxPos;

   for (DayTime t=startTime; t < stopTime; t+=timeStep)
   {
      for (int prn=1; prn <= MAX_PRN; prn++)
      {
         try
         {
            Xvt svXvt = ephStore.getPrnXvt(prn,t);
            double elev = rxXvt.x.elvAngle(svXvt.x);
         }
         catch(gpstk::Exception& e)
         {
            cout << "Caught one!" << endl << e << endl;
         }
      }
   }
}

int main(int argc, char *argv[])
{
   try
   {
      SVVis crap(argv[0]);

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
