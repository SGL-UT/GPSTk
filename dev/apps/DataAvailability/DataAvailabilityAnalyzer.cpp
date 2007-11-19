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
    intrk   Number of SVs in track
    tama    Time above mask angle
*/

#include <map>
#include <set>
#include <vector>
#include <list>
#include <algorithm>

#include "StringUtils.hpp"
#include "ObsID.hpp"

#include "DataAvailabilityAnalyzer.hpp"

#include "EphReader.hpp"
#include "ObsReader.hpp"

#include "MSCData.hpp"
#include "MSCStream.hpp"

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
        "Performs a data availability analysis of the input data. In general, "
        "availability is determined by station and satellite position."),
     
     inputOpt('o', "obs", 
              "Where to get the data to analyze.", true),
     
     outputOpt('\0', "output",
               "Where to send the output. The default is stdout."),
     
     independantOpt('x', "indepndant",
                    "The independant variable in the analysis. The default "
                    "is time."),
     
     ephFileOpt('e', "eph",
                "Where to get the ephemeris data. Can be " +
                EphReader::formatsUnderstood() + ".", true),
     
     rxPosOpt('p', "position",
              "Receiver antenna position in ECEF (x,y,z) coordinates.  Format "
              "as a string: \"X Y Z\"."),
     
     ignorePrnOpt('\0', "ignore-prn",
                  "Specify the PRN of an SV to not report on in the output. "
                  "Repeat to specify multiple SVs"),
     
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
                  "Ignore anomalies on SVs below this elevation. The default"
                  " is 10 degrees."),
     
     timeMaskOpt('\0', "time-mask",
                 "Ignore anomalies on SVs that haven't been above the mask"
                 " angle for this number of seconds. The default is 0 "
                 "seconds."),

     badHealthMaskOpt('b', "bad-health",
                      "Ignore anomalies associated with SVs that are marked "
                      "unhealthy."),

     smashAdjacentOpt('s', "smash-adjacent",
                      "Combine adjacent lines from the same PRN."),

     maskAngle(10), badHealthMask(false), timeMask(0), smashAdjacent(false),
     epochRate(0), epochCounter(0), allMissingCounter(0), 
     anyMissingCounter(0), pointsMissedCounter(0), haveAntennaPos(false)
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

   for (ObsItemName::const_iterator i=obsItemName.begin(); 
        i!=obsItemName.end(); i++)
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
      output.basic_ios<char>::rdbuf(std::cout.rdbuf());
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

   for (int i=0; i<ignorePrnOpt.getCount(); i++)
      ignorePrn.insert(StringUtils::asInt(ignorePrnOpt.getValue()[i]));

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

   // get the antenna position
   if (rxPosOpt.getCount())
   {
      double x,y,z;
      sscanf(rxPosOpt.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
      antennaPos[0] = x;
      antennaPos[1] = y;
      antennaPos[2] = z;
      haveAntennaPos = true;
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
            antennaPos = mscd.coordinates;
            haveAntennaPos=true;
            break;
         }
      }
      if (!haveAntennaPos)
         cout << "Did not find station " << msid << " in " << fn << "." << endl;
   }

   if (verboseLevel)
   {
      cout << "Using " << obsItemName[oiX] << " as the independant variable." 
           << endl
           << "Using a mask angle of " << maskAngle << " degrees" << endl;
      if (haveAntennaPos)
         cout << "Antenna position: " << antennaPos << " m ecef" << endl;

      cout << "Start time is " << startTime.printf(timeFormat) << endl
           << "Stop time is " << stopTime.printf(timeFormat) << endl
           << "Time span is " << timeSpan << " seconds" << endl;
      
      if (badHealthMask)
         cout << "Ignore anomalies associated with SVs marked unhealthy."
              << endl;
      else
         cout << "Including anomalies associated with SVs marked unhealthy."
              << endl;
      if (!ignorePrn.empty())
      {
         cout << "Ignoring PRNs:";
         copy(ignorePrn.begin(), ignorePrn.end(),
              ostream_iterator<int>(cout, " "));
         cout << endl;
      }
      MDPHeader::debugLevel = debugLevel;
   }

   return true;
}


//------------------------------------------------------------------------------
// Load all the data to analyze.
//------------------------------------------------------------------------------
void DataAvailabilityAnalyzer::spinUp()
{      
   EphReader ephData;
   ephData.verboseLevel = verboseLevel;
   for (int i=0; i < ephFileOpt.getCount(); i++)
      ephData.read(ephFileOpt.getValue()[i]);

   if (ephData.eph == NULL)
   {
      cout << "Didn't get any ephemeris data from the eph files. "
           << "Exiting." << endl;
      exit(-1);
   }
   eph = ephData.eph;


   const string fn=inputOpt.getValue()[0];
   ObsReader obsReader(fn);

   if (obsReader.inputType == FFIdentifier::tRinexObs && !haveAntennaPos)
   {
      antennaPos = obsReader.roh.antennaPosition;
      if (verboseLevel>1)
         cout << "Antenna position read from RINEX obs file:"
              << antennaPos << endl;
   }

   if (obsReader.inputType == FFIdentifier::tSMODF)
      obsReader.msid = msid;

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
      cout << "Could not determine data rate after " << i << " epochs. Sorry."
           << " This program is really\nwritten to just work with data that "
           << "is being collected at a fixed data rate.\nI guess it could be"
           << " re-written to work for changing data rates but I am too\n"
           << "lazy to do that right now. I'm not, however, too lazy to "
           << "write needlessly long\ndiagnostic messages." 
           << endl;
      exit(-1);
   }

   if (verboseLevel)
      cout << "Data rate is " << epochRate << " seconds after " << i 
           << " epochs." << endl;
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


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DataAvailabilityAnalyzer::MissingList DataAvailabilityAnalyzer::processList(
   const MissingList& ml,
   const EphemerisStore& eph)
{
   MissingList sml;
   for (MissingList::const_iterator i = ml.begin(); i != ml.end(); i++)
   {
      InView curr = *i;
      
      // calculate SV visibility info
      short prnTemp = 1;
      short numSVsInView = 0;
      ECEF rxpos(antennaPos);
      
      while (prnTemp <= gpstk::MAX_PRN)
      {
         Xvt svXVT;
         bool NoEph = false;
   
         try {svXVT = eph.getXvt(SatID(prnTemp, SatID::systemGPS), curr.time);}
         catch(gpstk::Exception& e) 
         {
            if (verboseLevel> 3) {cout << e << endl;}
            NoEph = true;
         }
         double elvAngle = 0;
         if (!NoEph)
         {
            try {
               elvAngle = rxpos.elvAngle(svXVT.x);}
            catch(gpstk::Exception& e) {
               if (verboseLevel > 1) cout << e << endl;}
            if (elvAngle > maskAngle)  { numSVsInView++; }
         } 
         prnTemp++;
      }     
      
      curr.numSVsVisible = numSVsInView;
      InView& prev = *sml.rbegin();

       // increment counter if there isn't data from any SVs
      if (curr.prn == 0)
      {
         allMissingCounter++;     
         pointsMissedCounter += numSVsInView;
      }
      else
         pointsMissedCounter++;
      
      if (i == ml.begin())
      {
         sml.push_back(curr);
         anyMissingCounter++;
         continue;
      }
      else if (prev.time != curr.time)
         anyMissingCounter++;
      
      // smash together epochs if necessary
      if (curr.prn == prev.prn && smashAdjacent)
      {
         prev.smashCount++;
         prev.time = curr.time;
         prev.elevation = curr.elevation;
         prev.azimuth = curr.azimuth;
         prev.snr = curr.snr;
         prev.epochCount = curr.epochCount;
         prev.numSVsVisible = max(curr.numSVsVisible, prev.numSVsVisible);
      }
      else
      {
         sml.push_back(curr);
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
   
   if (msidOpt.getCount() && obsReader.inputType == FFIdentifier::tSMODF)
   {
      msid = StringUtils::asUnsigned(msidOpt.getValue()[0]);
      obsReader.msid = msid;
   }
      
   double x=RSS(antennaPos[0], antennaPos[1], antennaPos[2]);
   
   if (x<1)
   {
      cout << "Warning! The antenna appears to be within one meter of the" 
           << endl << "center of the geoid. Please go check it." << endl;
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
      
      epochCounter++;

      if (obsReader.epochCount==1)
      {
         firstEpochTime = oe.time;
         if (verboseLevel)
            cout << "First observation is at " 
                 << firstEpochTime.printf(timeFormat) << endl;
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
      cout << "Last observation is at " << lastEpochTime.printf(timeFormat) 
           << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataAvailabilityAnalyzer::processEpoch(
   const Triple& ap, 
   const ObsEpoch& oe,
   const ObsEpoch& prev_oe)
{
   ECEF rxpos(ap);
   InView::dump ivdumper(cout, timeFormat);
   
   for (DayTime t = prev_oe.time + epochRate; t <= oe.time; t += epochRate)
   {
      for (int prn=1; prn<=32; prn++)
         inView[prn].update(prn, t, rxpos, *eph, gm, maskAngle);
      
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
         if (ignorePrn.find(prn) != ignorePrn.end())
            continue;

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
               
               q = soe.find(ObsID(ObsID::otSNR, ObsID::cbL1, ObsID::tcCA));
               if (q != soe.end())
                  iv.snr = q->second;

               iv.obsGained.clear();
               iv.obsLost.clear();

               // Now figure out if there was an obs from this SV for the
               // previous epoch, if no, no prob. That should have been caught
               // by the visibility check.
               ObsEpoch::const_iterator poei = prev_oe.find(svid);
               if (poei == prev_oe.end())
                  break;

               // At this point we know there is some data from the SV, so
               // figure out if the obs set is different from the previous
               const SvObsEpoch& psoe = poei->second;
               set<ObsID> curr, prev;
               for (q=soe.begin(); q != soe.end(); q++)
                  if (q->first.type != ObsID::otSSI &&
                      q->first.type != ObsID::otLLI)
                     curr.insert(q->first);

               for (q=psoe.begin(); q != psoe.end(); q++)
                  if (q->first.type != ObsID::otSSI &&
                      q->first.type != ObsID::otLLI)
                     prev.insert(q->first);

               set_difference(curr.begin(), curr.end(),
                              prev.begin(), prev.end(),
                              inserter(iv.obsGained, iv.obsGained.end()));

               set_difference(prev.begin(), prev.end(),
                              curr.begin(), curr.end(),
                              inserter(iv.obsLost, iv.obsLost.end()));

               if (!iv.obsGained.empty() || !iv.obsLost.empty())
               {
                  if (verboseLevel>1)
                     cout << t.printf(timeFormat) << " prn:" << svid.id
                          << " +" << iv.obsGained
                          << " -" << iv.obsLost << endl;
                  missingList.push_back(iv);
               }
            } // else
         }    // else      
      }       // for (int prn=1; prn<=32; prn++)
   }          // for (DayTime t=prev_oe.time+epochRate;t<=oe.time;t+= epochRate)
}             // void DataAvailabilityAnalyzer::processEpoch()


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataAvailabilityAnalyzer::shutDown()
{
   MissingList sml = processList(missingList, *eph);
   
   cout << "\n Availability Raw Results :\n\n";
   cout << "      Time          smash   PRN    Elv    Az  Hlth  SNR  #ama"
        << "    tama    ccid\n"
        << "=================================================================="
        << "======================\n";
   
   for_each(sml.begin(), sml.end(), InView::dump(cout, timeFormat));

   outputSummary();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
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
      // elevation, health 

      ObsRngDev ord(0, SatID(prn, SatID::systemGPS), time, rxpos, eph, gm);
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
      health = 0; //ord.getHealth();
   }
   catch (InvalidRequest& e)
   {
      up = false;
      aboveMask = false;
      elevation = 0;
      azimuth = 0;
      health = 0;
   }
} // end of InView::update()


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool DataAvailabilityAnalyzer::InView::dump::operator()(const InView& iv)
{
   double timeUp     = iv.time - iv.firstEpoch;
   double timeUpMask = iv.time - iv.firstEpochAboveMask;
   char dir;
   if (iv.elevation > 0)
      dir = iv.rising ? '^' : 'v';
   else
      dir = ' ';

   cout << left << iv.time.printf(timeFormat)
        << "   " << setw(4) << iv.smashCount << "  ";

   if (iv.prn>0)
   {
      cout << setw(3) << iv.prn << " " << fixed << right
           << setprecision(2) << setw(6) << iv.elevation
           << dir << "  "
           << setprecision(0) << setw(3) << iv.azimuth << "  "
           << hex << setw(2) << iv.health << "   " << dec;
   
      if (iv.up)
         cout << setprecision(1) << setw(4) << iv.snr;
      else
         cout << "-el ";
      
      cout << right << setw(6) << iv.numSVsVisible;      

      if (iv.up)
      {
         if (timeUpMask>0)
            cout << right << setw(12) <<  secAsHMS(timeUpMask);
         else
            cout << setw(12) << " ";
      }
      else
         cout << right << setw(12) << " -el  ";

      if (iv.obsLost.empty() || iv.obsGained.empty())
         cout << "all";
      else
         cout << iv.obsLost << " -> " << iv.obsGained;
   }
   else
   {
      cout << "All";
      cout << right << setw(30) << iv.numSVsVisible;
   }

   cout << endl;

   return true;
}

void DataAvailabilityAnalyzer::outputSummary()
{
   cout << "\n\n Summary:\n\n";
   
   cout << right << setw(40) << "Total number of epochs with data: " 
        << left  << setw(10) << epochCounter << endl;
   cout << right << setw(40) << "Epochs with any # of missed points: "
        << left  << setw(10) << anyMissingCounter << endl;
   cout << right << setw(40) << "Epochs without data from any SV: "
        << left  << setw(10) << allMissingCounter << endl;
   cout << right << setw(40) << "Total number of points missed: "
        << left  << setw(10) << pointsMissedCounter << endl << endl;
}

void dump(ostream& s, const ObsSet& obs, int detail)
{
   if (obs.empty())
      s << "--- ";
   else
   {
      if (detail>0)
         copy(obs.begin(), obs.end(), ostream_iterator<ObsID>(s, ", "));
      else
         for (ObsSet::const_iterator i=obs.begin(); i!=obs.end(); i++)
            if (i->type==ObsID::otRange)
            {
               if (i != obs.begin())
                  s << ",";
               s << ObsID::cbStrings[i->band] << ObsID::tcStrings[i->code];
            }
   }
}

std::ostream& operator<<(std::ostream& s, const ObsSet& obs)
{
   dump(s, obs);
   return s;
}
