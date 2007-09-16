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
   int minElevation;
   DayTime startTime, stopTime;
   double timeSpan;
};


bool SVVis::initialize(int argc, char *argv[]) throw()
{
   CommandOptionWithNumberArg 
      minElevOpt(
         'm', "min-elev",
         "Give an integer for the elevation (degrees) above which you want to find more than 12 SVs at a given time.", true);     
   
   CommandOptionWithAnyArg 
      antennaPositionopt(
         'p', "position",
         "Antenna position in ECEF (x,y,z) coordinates.  Format as a string: \"X Y Z\".",false),
         
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
   int minElev = StringUtils::asInt((minElevOpt.getValue())[0]);

   if (startTimeOpt.getCount())
      startTime = startTimeOpt.getTime()[0];
   else
      startTime = DayTime::BEGINNING_OF_TIME;

   if (stopTimeOpt.getCount())
      stopTime = stopTimeOpt.getTime()[0];
   else
      stopTime = DayTime::END_OF_TIME;

   if (timeSpanOpt.getCount())
      timeSpan = StringUtils::asDouble(timeSpanOpt.getValue()[0]);
   else
      timeSpan = 1e99;

   // get the ephemeris source(s)
   ephReader.verboseLevel = verboseLevel;
   for (int i=0; i<ephFilesOpt.getCount(); i++)
      ephReader.read(ephFilesOpt.getValue()[i]);
   if (ephData.eph == NULL)
   {
      cout << "Didn't get any ephemeris data from the eph files. "
           << "Exiting." << endl;
      exit(-1);
   }
   if (msid && mscFileOpt.getCount())
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
            antennaPos = mscd.coordinates;
            if (verboseLevel>1)
               cout << "Antenna position read from MSC file:"
                    << antennaPos << " (msid: "
                    << msid << ")" << endl;
            haveAntennaPos=true;
            break;
         }
      }
      if (!haveAntennaPos)
         cout << "Did not find station " << msid << " in " << fn 
              << "." << endl;
   }


   if (debugLevel)
      cout << "debugLevel: " << debugLevel << endl
           << "verboseLevel: " << verboseLevel << endl
           << "minEl: " << minEl << endl;

  
   // get the antenna position
   Xvt antPVT;
   double x,y,z;
   sscanf(antennaPosition.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
   antPVT.x[0] = x; antPVT.x[1] = y; antPVT.x[2] = z;
   ECEF antPos = Triple(antPVT.x);
  
   // get initial and final times for analysis. extra code b/c sscanf reads in int's but setYMDHMS needs shorts
   short year, month, day, hour, minute;
   double seconds;

   DayTime tStartDT = ephStore.getInitialTime();
   year = tStartDT.year();
   month = tStartDT.month();
   day = tStartDT.day();
   hour = tStartDT.hour();
   minute = tStartDT.minute();
   seconds = tStartDT.second();
   DayTime tstart;
   tstart.setYMDHMS(year,month,day,hour,minute,seconds);
  
   DayTime tEndDT = ephStore.getFinalTime();
   year = tEndDT.year();
   month = tEndDT.month();
   day = tEndDT.day();
   hour = tEndDT.hour();
   minute = tEndDT.minute();
   seconds = tEndDT.second();
   DayTime tend;
   tend.setYMDHMS(year,month,day,hour,minute,seconds);
  
   DayTime t = tstart;

   cout << "Start Time: " << tstart << " End Time: " << tend << endl;


   return true;
}

void SVVis::process()
   {      
      gpstk::EphemerisStore& ephStore = *ephReader.eph;
      while (t < tend)
      {
         short numSVsAboveElv = 0;
         short prn = 1;
         while (prn <= gpstk::MAX_PRN)
         {
            Xvt peXVT;
            bool NoEph = false;
            try
            {
               peXVT = ephStore.getPrnXvt(prn,t);
            }
            catch(gpstk::Exception& e) 
            {
               if (verbose) {cout << e << endl;}
               NoEph = true;
            }
            double elvAngle = 0;
            if (!NoEph)
            {
               try {elvAngle = antPVT.x.elvAngle(peXVT.x);}
               catch(gpstk::Exception& e) {if (verbose) {cout << e << endl;}}
               if ( elvAngle > minEl ) { numSVsAboveElv++; }
            }
            prn++;
         }
         if (numSVsAboveElv > 12)
         { 
            cout << "Found " << numSVsAboveElv << " SVs above " << minEl << " degrees at " << t << endl;
            prn = 1;
            while (prn < gpstk::MAX_PRN)
            {
               bool NoEph = false;
               Xvt peXVT;
               try 
               {
                  peXVT = ephStore.getPrnXvt(prn,t);
               }
               catch(gpstk::Exception& e) {NoEph = true;}
               double elvAngle = 0;
               double azAngle = 0;
               if (!NoEph)
               {
                  try {elvAngle = antPVT.x.elvAngle(peXVT.x);}
                  catch(gpstk::Exception& e) {if (verbose) {cout << e << endl;}}
                  if ( elvAngle > 0 ) 
                  { 
                     cout << t << "  PRN " << setw(2) << prn << " : elev: " << elvAngle;
                     try {azAngle = antPVT.x.azAngle(peXVT.x);}
                     catch(gpstk::Exception& e) {if (verbose) {cout << e << endl;}}
                     if ( azAngle > 0 ) { cout << "  azim: " << azAngle; }
                     cout << " degrees\n";
                  }
               }
               prn++;
            }
         }
         t += 10;  
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
