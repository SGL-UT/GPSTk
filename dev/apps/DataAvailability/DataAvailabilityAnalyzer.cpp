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

/** @file Performs a data availability analysis of the input data. In general,
    availability is determined by station and satellite position.

    This program refers to the items that can be specified as the (in)dependant
    variables in the analysis as ObservationIDs or oid. The complete list of
    these are:
    el      Elevation, degrees (0-90)
    az      Azimuth, degrees (0-360, 0=north)
    time    Time in mjd (or as specified)
    prn     SV ID (1-31)
    ccid    Three character Rinex 3.0 observation specification: tna
              t: observation type (C, L, D, S)
              n: carrier band (1, 2, 5)
              a: code tracked (C, P, W, Y, M, I, Q, ...)
    ch      Channel (1-99)
    snr     Signal to noise ratio (C/C0) in dB-Hz
    health  SV health bits
    tcnt    Continuous tracking count
    iod     Issue of data
    intrk   Number of SVs in track
    tama    Time above mask angle
*/

#include <map>

#include "StringUtils.hpp"

#include "DataAvailabilityAnalyzer.hpp"
#include "RinexObsID.hpp"
#include "RinexObsStream.hpp"
#include "RinexObsData.hpp"
#include "FFIdentifier.hpp"
#include "DataReader.hpp"
#include "FormatConversionFunctions.hpp"

#include "MSCData.hpp"
#include "MSCStream.hpp"

#include "MDPStream.hpp"

using namespace std;
using namespace gpstk;


//------------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//------------------------------------------------------------------------------
DataAvailabilityAnalyzer::DataAvailabilityAnalyzer(const std::string& applName)
   throw()
   : timeFormat("%Y %j %02H:%02M:%04.1f"),
     BasicFramework(
        applName,
        "Performs a data availability analysis of the input data. In general,"
        "availability is determined by station and satellite position."),
     
     inputOpt('o', "obs", 
              "Where to get the data to analyze.", true),
     
     outputOpt('\0', "output",
               "Where to send the output. The default is stdout."),
     
     independantOpt('x', "indepndant",
                    "The independant variable in the analysis. The default is time."),
     
     ephFileOpt('e', "eph",  "Where to get the ephemeris data. Can be "
                   " rinex, fic, or sp3", true),
     
     mscFileOpt('c', "msc", "Station coordinate file"),
     
     msidOpt('m', "msid", "Station to process data for. Used to select "
                "a station position from the msc file."),
     
     timeFmtOpt('t', "time-format", "Daytime format specifier used for "
                   "times in the output. The default is \"" 
                   + timeFormat + "\"."),
     
     startTimeOpt('\0', "start-time", "%4Y/%03j/%02H:%02M:%05.2f",
                  "Ignore data before this time. (%4Y/%03j/%02H:%02M:%05.2f)"),
     
     stopTimeOpt('\0',  "stop-time", "%4Y/%03j/%02H:%02M:%05.2f",
                 "Ignore any data after this time"),
     
     timeSpanOpt('l', "time-span", "How much data to process, in seconds"),

     maskAngleOpt('\0', "mask-angle",
                  "Ignore anomalies on SVs below this elevation. The default is 10 degrees."),
     
     timeMaskOpt('\0', "time-mask",
                 "Ignore anomalies on SVs that haven't been above the mask angle for this number of seconds. The default is 0 seconds."),

     badHealthMaskOpt('b', "bad-health",
                      "Ignore anomalies associated with SVs that are marked unhealthy."),

     smashAdjacentOpt('s', "smash-adjacent",
                      "Combine adjacent lines from the same PRN."),

     maskAngle(10), badHealthMask(false), timeMask(0), smashAdjacent(false),
     epochRate(0), epochCount(0)
{
   // Set up a couple of helper arrays to map from enum <-> string
   obsItemName[oiUnknown] = "unk";
   obsItemName[oiElevation] = "el";
   obsItemName[oiAzimuth] = "az";
   obsItemName[oiTime] = "time";
   obsItemName[oiPRN] = "prn";
   obsItemName[oiCCID] = "ccid";
   obsItemName[oiSNR] = "snr";
   obsItemName[oiHealth] = "health";
   obsItemName[oiTrackCount] = "tcnt";
   obsItemName[oiIOD] = "iod";

   for (ObsItemName::const_iterator i=obsItemName.begin(); i!=obsItemName.end(); i++)
      obsItemId[i->second] = i->first;
}


//------------------------------------------------------------------------------
// Here the command line options parsed and used to configure the program
//------------------------------------------------------------------------------
bool DataAvailabilityAnalyzer::initialize(int argc, char *argv[]) throw()
{
   if (!BasicFramework::initialize(argc,argv)) return false;

   if (debugLevel)
      cout << "debugLevel: " << debugLevel << endl
           << "verboseLevel: " << verboseLevel << endl;

   if (outputOpt.getCount())
   {
      output.open(outputOpt.getValue()[0].c_str(), std::ios::out);
      if (debugLevel)
         cout << "Sending output to" 
              << outputOpt.getValue()[0]
              << endl;
   }
   else
   {
      if (debugLevel)
         cout << "Sending output to stdout" << endl;
      output.copyfmt(std::cout);
      output.clear(std::cout.rdstate());
      output.std::basic_ios<char>::rdbuf(std::cout.rdbuf());
   }

   if (timeFmtOpt.getCount())
      timeFormat = timeFmtOpt.getValue()[0];

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

   if (maskAngleOpt.getCount())
      maskAngle = StringUtils::asDouble(maskAngleOpt.getValue()[0]);

   if (timeMaskOpt.getCount())
      timeMask = StringUtils::asDouble(timeMaskOpt.getValue()[0]);

   if (badHealthMaskOpt.getCount())
      badHealthMask = true;

   if (smashAdjacentOpt.getCount())
      smashAdjacent = true;

   oiX = oiTime;
   if (independantOpt.getCount())
   {
      ObsItemId::const_iterator i;
      i = obsItemId.find(independantOpt.getValue()[0]);
      if (i == obsItemId.end())
      {
         cout << "Cound not find obs item. Valid items are:" << endl;
         for (i=obsItemId.begin(); i!=obsItemId.end(); i++)
            cout << i->first << " ";
         cout << endl;
         exit(-1);
      }
      else
      {
         oiX = i->second;
      }
   }

   if (verboseLevel)
   {
      cout << "Using " << obsItemName[oiX] << " as the independant variable." << endl;
      cout << "Using a mask angle of " << maskAngle << " degrees" << endl;
      if (badHealthMask)
         cout << "Ignore anomalies associated with SVs marked unhealthy." << endl;
      else
         cout << "Including anomalies associated with SVs marked unhealthy." << endl;
      MDPHeader::debugLevel = debugLevel;
   }

   return true;
}

class ObsReader
{
public:
   const string fn;
   FFIdentifier inputType;
   RinexObsStream ros;
   MDPStream mdps;
   RinexObsHeader roh;
   int verboseLevel;
   unsigned long epochCount;

   ObsReader(const string& str)
      : fn(str), inputType(str), verboseLevel(0), epochCount(0)
   {
      if (inputType == FFIdentifier::tRinexObs)
      {
         ros.open(fn.c_str(), ios::in);
         cout << "Reading " << fn << " as RINEX obs data." << endl;
         ros.exceptions(fstream::failbit);
         ros >> roh;
      }
      else if (inputType == FFIdentifier::tMDP)
      {
         mdps.open(fn.c_str(), ios::in);
         cout << "Reading " << fn << " as MDP data." << endl;
      }
   };

   ObsEpoch getObsEpoch()
   {
      ObsEpoch oe;
      if (inputType == FFIdentifier::tRinexObs)
      {
         RinexObsData rod;
         ros >> rod;
         oe = makeObsEpoch(rod);
      }
      else if (inputType == FFIdentifier::tMDP)
      {
         MDPEpoch moe;
         mdps >> moe;
         oe = makeObsEpoch(moe);
      }
      epochCount++;
      return oe;
   }

   bool operator()()
   {
      if (inputType == FFIdentifier::tRinexObs)
         return ros;
      else if (inputType == FFIdentifier::tMDP)
         return mdps;
      return false;
   }
};


//------------------------------------------------------------------------------
// Load all the data to analyze.
//------------------------------------------------------------------------------
void DataAvailabilityAnalyzer::spinUp()
{      
   ephData.verbosity = verboseLevel;
   ephData.read(ephFileOpt);

   if (!ephData.haveEphData)
   {
      cout << "Didn't get any ephemeris data from the eph files. "
           << "Exiting." << endl;
      return;
   }

   msid = 0;
   bool haveAntennaPos=false;
   if (msidOpt.getCount())
      msid = StringUtils::asUnsigned(msidOpt.getValue()[0]);

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
         cout << "Did not find station " << msid << " in " << fn << "." << endl;
   }

   const string fn=inputOpt.getValue()[0];
   ObsReader obsReader(fn);

   if (obsReader.inputType == FFIdentifier::tRinexObs && !haveAntennaPos)
   {
      antennaPos = obsReader.roh.antennaPosition;
      if (verboseLevel>1)
         cout << "Antenna position read from RINEX obs file:"
              << antennaPos << endl;
   }

   DayTime t0;
   ObsEpoch oe;
   int i,j;
   for (i=j=0; i<100 && j<10 && obsReader(); i++)
   {
      oe = obsReader.getObsEpoch();
      double dt = oe.time - t0;
      if (std::abs(dt - epochRate) > 0.1)
      {
         epochRate = dt;
         j = 0;
      }
      else
         j++;
      t0 = oe.time;
   }
      
   if (j<10)
   {
      cout << "Could not determine data rate after " << i << " epochs. Sorry. This program is really\nwritten to just work with data that is being collected at a fixed data rate.\nI guess it could be re-written to work for changing data rates but I am too\nlazy to do that right now. I'm not, however, too lazy to write needlessly long\ndiagnostic messages." << endl;
      exit(-1);
   }

   if (verboseLevel)
      cout << "Data rate is " << epochRate << " after " << i << " epochs." << endl;
}


std::string secAsHMS(double seconds, bool frac=false)
{
   std::ostringstream oss;
   oss << setfill('0');
   
   if (seconds<0)
      oss << "-";

   seconds = std::abs(seconds);
   long d=0,h=0,m=0,s=0;
   s = static_cast<long>(std::floor(seconds));
   seconds -= static_cast<double>(s);

   if (s > 86400) { d = s/86400; s %= 86400; }
   if (s > 3600)  { h = s/3600;  s %= 3600;  }
   if (s > 60)    { m = s/60;    s %= 60;    }

   if (d) oss << d << " d ";
   if (h) oss << setw(2) << h << ":";
   if (m) oss << setw(2) << m << ":";

   if (h || m)
      oss << setfill('0');
   else
      oss << setfill(' ');

   oss << setw(2) << s;

   if (seconds>=0.1 && frac)
      oss << "." << static_cast<int>(seconds*10);
   else
      oss << "  ";

   return oss.str();
}


void DataAvailabilityAnalyzer::InView::update(
   short prn,
   const DayTime& time,
   const ECEF& rxpos,
   const EphemerisStore& eph,
   GeoidModel& gm,
   float maskAngle)
{
   try
   {
      this->prn = prn;
      this->time = time;
      // We really don't care about the observed range deviation, the
      // ObsRngDev class is just a convient way to get the azimuth, 
      // elevation, health, iodc, 
      ObsRngDev ord(0, prn, time, rxpos, eph, gm);
      vfloat el=ord.getElevation();

      if (el.is_valid() && el > 0)
      {
         if (!up)
         {
            firstEpoch = time;
            up = true;
            aboveMask = false;
            epochCount = 0;
            snr = 0;
            inTrack = 0;
         }
         else
         {
            rising = el > elevation;
         }
         if (el > maskAngle && !aboveMask)
         {
            aboveMask = true;
            firstEpochAboveMask = time;
         }
      }
      else
      {
         up = false;
         aboveMask = false;
      }
      elevation = ord.getElevation();
      azimuth = ord.getAzimuth();
      iodc = ord.getIODC();
      health = ord.getHealth();
   }
   catch (EphemerisStore::NoEphemerisFound& e)
   {
      up = false;
      aboveMask = false;
      elevation = 0;
      azimuth = 0;
      iodc = 0;
      health = 0;
   }
} // end of InView::update()


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DataAvailabilityAnalyzer::MissingList DataAvailabilityAnalyzer::smash(
   const MissingList& ml) const
{
   MissingList sml;
   for (MissingList::const_iterator i = ml.begin(); i != ml.end(); i++)
   {
      if (i == ml.begin())
      {
         sml.push_back(*i);
         continue;
      }

      InView& prev = *sml.rbegin();
      const InView& curr = *i;
      if (curr.prn == prev.prn && smashAdjacent)
      {
         prev.smashCount++;
         prev.time = curr.time;
         prev.elevation = curr.elevation;
         prev.azimuth = curr.azimuth;
         prev.snr = curr.snr;
         prev.epochCount = curr.epochCount;
      }
      else
      {
         sml.push_back(*i);
      }
   }
   return sml;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataAvailabilityAnalyzer::process()
{
   const string fn=inputOpt.getValue()[0];
   ObsReader obsReader(fn);

   double x=RSS(antennaPos[0], antennaPos[1], antennaPos[2]);
   
   if (x<1)
   {
      cout << "Warning! The antenna appears to be within one meter of the" << endl
           << "center of the geoid. Please go check it." << endl;
      return;
   }

   ObsEpoch oe, prev_oe;

   DayTime firstEpochTime, lastEpochTime;
   while (obsReader())
   {
      oe = obsReader.getObsEpoch();
      if (startTime > oe.time)
         continue;
      if (stopTime < oe.time)
         break;
      
      if (obsReader.epochCount==1)
      {
         firstEpochTime = oe.time;
         if (verboseLevel)
            cout << "Start time: " << firstEpochTime.printf(timeFormat) << endl;
      }
      else
      {
         lastEpochTime = oe.time;
         if (lastEpochTime - firstEpochTime > timeSpan)
            break;
         
         processEpoch(antennaPos, oe, prev_oe);
      }
      prev_oe = oe;
   }

   if (verboseLevel)
      cout << "Stop time: " << lastEpochTime.printf(timeFormat) << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataAvailabilityAnalyzer::processEpoch(
   const Triple& ap, 
   const ObsEpoch& oe,
   const ObsEpoch& prev_oe)
{
   EphemerisStore& eph = *ephData.eph;
   ECEF rxpos(ap);
   InView::dump ivdumper(cout, timeFormat);


   for (DayTime t = prev_oe.time + epochRate; t <= oe.time; t += epochRate)
   {
      for (int prn=1; prn<=32; prn++)
         inView[prn].update(prn, t, rxpos, eph, gm, maskAngle);

      if (verboseLevel>2)
      {
         cout << t.printf(timeFormat) << "  SVs in view: ";
         for (int prn=1; prn<=32; prn++)
            if (inView[prn].up)
               cout << prn << "(" << setprecision(3)
                    << inView[prn].elevation << ") ";
         cout << endl;
      }

      if (t != oe.time)
      {
         InView iv;
         iv.prn = 0;
         iv.time = t;
         missingList.push_back(iv);
         continue;
      }

      for (int prn=1; prn<=32; prn++)
      {
         ObsEpoch::const_iterator oei;
         SatID svid(prn, SatID::systemGPS);
         
         oei = oe.find(svid);
         InView& iv=inView[prn];
         iv.inTrack = oe.size();

         if (oei == oe.end())  // No data from this SV
         {
            if (oe.size()<12 && iv.elevation>maskAngle && 
                (!iv.health || !badHealthMask))
            {
               missingList.push_back(iv);
            }
         }
         else // There is data from this SV
         {
            if (verboseLevel>3)
               cout << oei->first << " " << oei->second << endl;
            if (verboseLevel>3)
               ivdumper(iv);
            if (!iv.up)
            {
               missingList.push_back(iv);
            }
            else
            {
               iv.epochCount++;
               const SvObsEpoch& soe = oei->second;
               SvObsEpoch::const_iterator q;
               
               q = soe.find(RinexObsID(RinexObsHeader::S1));
               if (q != soe.end())
                  iv.snr = q->second;
            }
         }
      }
   }
}

void DataAvailabilityAnalyzer::shutDown()
{
   MissingList sml = smash(missingList);

   for_each(sml.begin(), sml.end(), InView::dump(cout, timeFormat));
}


bool DataAvailabilityAnalyzer::InView::dump::operator()(const InView& iv)
{
   double timeUp     = iv.time - iv.firstEpoch;
   double timeUpMask = iv.time - iv.firstEpochAboveMask;
   char dir;
   if (iv.elevation > 0)
      dir = iv.rising ? '^' : 'v';
   else
      dir = ' ';

   string ccid="all";
   
   cout << left << iv.time.printf(timeFormat)
        << "+" << setw(4) << iv.smashCount
        << " prn:";

   if (iv.prn>0)
   {
      cout << setw(3) << iv.prn
           << " ccid:" << ccid
           << fixed << right
           << " el:" << setprecision(2) << setw(6) << iv.elevation
           << dir
           << " az:" << setprecision(0) << setw(3) << iv.azimuth
           << " hlth:" << hex << setw(2) << iv.health << dec;
   
      if (iv.up)
         cout << " snr:" << setprecision(1) << setw(4) << iv.snr
              << " tama:" << right << setw(11) <<  secAsHMS(timeUpMask);
   }
   else
      cout << "all";

   cout << endl;

   return true;
}
