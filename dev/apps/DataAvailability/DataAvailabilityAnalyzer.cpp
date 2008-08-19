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
#include "GPSEphemerisStore.hpp"
#include "MSCData.hpp"
#include "MSCStream.hpp"
#include "Position.hpp"

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
                "Where to get the ephemeris data. Acceptable formats include " +
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
     
     snrThresholdOpt('\0', "snr",
                     "Discard data with an SNR less than this value. The default "
                     " is 20 dB-Hz."),
     
     trackAngleOpt('\0', "track-angle",
                  "Assume the receiver starts tracking at this elevation. The "
                  "default is 0 degrees."),

     timeMaskOpt('\0', "time-mask",
                 "Ignore anomalies on SVs that haven't been above the mask"
                 " angle for this number of seconds. The default is 0 "
                 "seconds."),

     badHealthMaskOpt('b', "bad-health",
                      "Ignore anomalies associated with SVs that are marked "
                      "unhealthy."),

     smashAdjacentOpt('s', "smash-adjacent",
                      "Combine adjacent lines from the same PRN."),

     maskAngle(10), trackAngle(0), badHealthMask(false), timeMask(0),
     smashAdjacent(false), ignoreEdges(true),
     epochRate(0), epochCounter(0), allMissingCounter(0), 
     totalSvEpochCounter(0), expectedSvEpochCounter(0), receivedSvEpochCounter(0),
     anyMissingCounter(0), haveAntennaPos(false)
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
         cout << "Sending output to "  << outputOpt.getValue()[0] << endl;
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

   if (snrThresholdOpt.getCount())
      snrThreshold = StringUtils::asDouble(snrThresholdOpt.getValue()[0]);
   else
      snrThreshold = 20.0;

   if (maskAngleOpt.getCount())
      maskAngle = StringUtils::asDouble(maskAngleOpt.getValue()[0]);

   if (trackAngleOpt.getCount())
      trackAngle = StringUtils::asDouble(trackAngleOpt.getValue()[0]);

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
      output << "Using " << obsItemName[oiX] << " as the independant variable." 
             << endl
             << "Using a mask angle of " << maskAngle << " degrees" << endl
             << "Using a track angle of " << trackAngle << " degrees" << endl;
      if (haveAntennaPos)
         output << "Antenna position: " << antennaPos << " m ecef" << endl;
      output << "Ignoring data with SNR < " << snrThreshold << " dB-Hz" << endl
             << "Start time is " << startTime.printf(timeFormat) << endl
             << "Stop time is " << stopTime.printf(timeFormat) << endl
             << "Time span is " << timeSpan << " seconds" << endl;
      
      if (smashAdjacent)
         output << "Merging adjacent anomalies." << endl;
      if (ignoreEdges && smashAdjacent)
         output << "Ignoring single epoch anomalies close to the mask angle." << endl;
      if (badHealthMask)
         output << "Ignore anomalies associated with SVs marked unhealthy."
                << endl;
      else
         output << "Including anomalies associated with SVs marked unhealthy."
                << endl;
      if (!ignorePrn.empty())
      {
         output << "Ignoring PRNs:";
         copy(ignorePrn.begin(), ignorePrn.end(),
              ostream_iterator<int>(output, " "));
         output << endl;
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
   if (typeid(*ephData.eph) == typeid(GPSEphemerisStore))
   {
      GPSEphemerisStore& bce = dynamic_cast<GPSEphemerisStore&>(*ephData.eph);
      bce.SearchNear();
      if (verboseLevel)
         output << "Using search near for ephemeris." << endl;
   }
   eph = ephData.eph;


   const string fn=inputOpt.getValue()[0];
   ObsReader obsReader(fn);
   if (verboseLevel)
      output << "Reading obs from " << fn << endl;

   if (obsReader.inputType == FFIdentifier::tRinexObs && !haveAntennaPos)
   {
      antennaPos = obsReader.roh.antennaPosition;
      if (verboseLevel>1)
         output << "Antenna position read from RINEX obs file:"
                << antennaPos << endl;
   }

   if (obsReader.inputType == FFIdentifier::tSMODF)
      obsReader.msid = msid;

   epochRate = obsReader.estimateObsInterval();

   if (epochRate < 0)
   {
      cout << "Could not determine data rate.  Sorry."
           << " This program is really\nwritten to just work with data that "
           << "is being collected at a fixed data rate.\nI guess it could be"
           << " re-written to work for changing data rates but I am too\n"
           << "lazy to do that right now. I'm not, however, too lazy to "
           << "write needlessly long\ndiagnostic messages." 
           << endl;
      exit(-1);
   }

   if (verboseLevel)
      output << "Data rate is " << epochRate << " seconds." << endl;
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

   if (d) oss << d << " d " << setw(2);
   if (h) oss << h << ":" << setw(2);
   if (h || m) oss << m << ":" << setw(2);

   oss << s << "." << static_cast<int>(seconds*10);

   return oss.str();
}


//------------------------------------------------------------------------------
// This is where we fill out additional data in the list, smash adjacent obs, 
// and remove nuisance anomalies
//------------------------------------------------------------------------------
DataAvailabilityAnalyzer::MissingList DataAvailabilityAnalyzer::processList(
   const MissingList& ml,
   const EphemerisStore& eph)
{
   Position rx(antennaPos);   
   MissingList sml;

   for (MissingList::const_iterator i = ml.begin(); i != ml.end(); i++)
   {
      InView curr = *i;
      
      // calculate the number of SV above the track and mask angles
      int ama = 0, ata = 0;
      for (int prn=1; prn <= gpstk::MAX_PRN; prn++)
      {
         Position sv;
         try
         {
            Xvt svXVT = eph.getXvt(SatID(prn, SatID::systemGPS), curr.time);
            sv = svXVT.x;

            double el = rx.elevationGeodetic(sv);
            if (el > trackAngle)
               ata++;
            if (el > maskAngle)
               ama++;
         }
         catch(gpstk::Exception& e) 
         {
            if (verboseLevel> 3)
               output << e << endl;
         }
      }

      anyMissingCounter++;
      if (curr.prn == 0)
         allMissingCounter++;     

      curr.numAboveMaskAngle = ama;
      curr.numAboveTrackAngle = ata;

      // Search for the previous in the list from this prn
      MissingList::reverse_iterator j;
      MissingList::reverse_iterator k=sml.rend();
      for (j = sml.rbegin(); j != k; j++)
         if (j->prn == curr.prn)
            break;

      if (j == sml.rend())
      {
         sml.push_back(curr);
      }
      else
      {
         InView& prev = *j;
         double dt=std::abs(curr.time - prev.time - epochRate);
         if (smashAdjacent && dt < 1e-3)
         {
            prev.smashCount++;
            prev.span = prev.smashCount * epochRate;
            prev.time = curr.time;
            prev.last_elevation = curr.elevation;
            prev.last_azimuth = curr.azimuth;
            prev.obs = curr.obs;
            prev.epochCount = curr.epochCount;
            prev.numAboveMaskAngle = 
               max(curr.numAboveMaskAngle, prev.numAboveMaskAngle);
            prev.numAboveTrackAngle = 
               max(curr.numAboveTrackAngle, prev.numAboveTrackAngle);
            prev.receivedCount = curr.receivedCount;
            prev.expectedCount = curr.expectedCount;
         }
         else
            sml.push_back(curr);
      }
   }

   // Go through removing single epoch/prn anomolies that are close to the elevation
   // mast angle.
   if (ignoreEdges && smashAdjacent)
   {
      MissingList sml2;
      
      for (MissingList::const_iterator i = sml.begin(); i != sml.end(); i++)
      {
         InView curr = *i;
         if (curr.elevation < maskAngle * 1.1 && curr.smashCount <2)
            continue;
         sml2.push_back(curr);
      }
      return sml2;
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

   while (obsReader)
   {
      oe = obsReader.getObsEpoch();
      if (!obsReader)
         break;
      if (startTime > oe.time)
         continue;
      if (stopTime < oe.time)
         break;
      
      epochCounter++;
      oe = removeBadObs(oe);

      if (obsReader.epochCount==1)
      {
         firstEpochTime = oe.time;
         if (verboseLevel)
            output << "First observation is at " 
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
      output << "Last observation is at " << lastEpochTime.printf(timeFormat) 
             << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ObsEpoch DataAvailabilityAnalyzer::removeBadObs(const ObsEpoch& oe)
{
   // Remove data that has an SNR less than the threshold and all LLI/
   // SSI obs.
   ObsEpoch oe2;
   oe2.time = oe.time;
   oe2.rxClock = oe.rxClock;

   for (ObsEpoch::const_iterator oei = oe.begin(); oei != oe.end(); oei++)
   {
      const SvObsEpoch& soe = oei->second;
      SvObsEpoch soe2;
      soe2.svid = soe.svid;
      soe2.azimuth = soe.azimuth;
      soe2.elevation = soe.elevation;

      for (SvObsEpoch::const_iterator p=soe.begin(); p != soe.end(); p++)
      {
         const ObsID& oid = p->first;
         if (oid.type == ObsID::otSSI || oid.type == ObsID::otLLI)
            continue;
         if (oid.type == ObsID::otSNR && p->second >= snrThreshold)
            soe2[p->first] = p->second;
         else
         {
            ObsID snrId(ObsID::otSNR, oid.band, oid.code);
            SvObsEpoch::const_iterator snr_itr = soe.find(snrId);
            if (snr_itr != soe.end() && snr_itr->second < snrThreshold)
            {
               if (verboseLevel>2)
                  output << oe.time.printf(timeFormat)
                         << " Ignoring " << soe.svid
                         << " " << oid
                         << " SNR:" << snr_itr->second << endl;
               continue;
            }
            soe2[p->first] = p->second; 
         }
      }
      oe2[oei->first] = soe2;
   }
   return oe2;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataAvailabilityAnalyzer::processEpoch(
   const Triple& ap, 
   const ObsEpoch& oe,
   const ObsEpoch& prev_oe)
{
   ECEF rxpos(ap);
   
   for (DayTime t = prev_oe.time + epochRate; t <= oe.time; t += epochRate)
   {
      for (int prn=1; prn<=32; prn++)
         inView[prn].update(prn, t, rxpos, *eph, gm, maskAngle, trackAngle);
      
      if (verboseLevel>3)
      {
         output << t.printf(timeFormat) << "  SVs in view: ";
         for (int prn=1; prn<=32; prn++)
            if (inView[prn].up)
               output << prn << "(" << setprecision(3)
                      << inView[prn].elevation << ") ";
         output << endl;
      }

      // Figure out how many SVs could and should have been tracked
      int ata=0;
      for (int prn=1; prn<=32; prn++)
         if (inView[prn].elevation > maskAngle)
            ata++;
      totalSvEpochCounter += ata;
      expectedSvEpochCounter += std::min(ata, 12);

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

	 if (iv.elevation < maskAngle)
	    continue;

         iv.inTrack = oe.size();

         if (oei == oe.end())  // No data from this SV
         {
            if (oe.size()<12 && (!iv.health || !badHealthMask))
            {
               missingList.push_back(iv);
            }
         }
         else // There is at least some data from this SV
         {
            if (verboseLevel>3)
               output << oei->first << " " << oei->second << endl;
            if (verboseLevel>3)
               output << iv;

            receivedSvEpochCounter++;

            if (iv.health == 0)
               iv.receivedCount++;

            // This adds obs that we receive that *aren't* supposed to be there
            // Note that we don't flag obs that are present but below the 
            // mask/track angle as being anomalous 
            if (!iv.up)
            {
               missingList.push_back(iv);
            }
            else
            {
               iv.epochCount++;
               const SvObsEpoch& soe = oei->second;

               iv.obs = soe;

               iv.obsGained.clear();
               iv.obsLost.clear();

               // Now figure out if there was an obs from this SV for the
               // previous epoch, if no, no prob. That should have been caught
               // by the visibility check.
               ObsEpoch::const_iterator poei = prev_oe.find(svid);
               if (poei == prev_oe.end())
                  continue;

               // At this point we know there is some data from the SV, so
               // figure out if the obs set is different from the previous
               const SvObsEpoch& psoe = poei->second;
               set<ObsID> curr, prev;
               SvObsEpoch::const_iterator q;
               for (q=soe.begin(); q != soe.end(); q++)
                  curr.insert(q->first);

               for (q=psoe.begin(); q != psoe.end(); q++)
                  prev.insert(q->first);

               set_difference(curr.begin(), curr.end(),
                              prev.begin(), prev.end(),
                              inserter(iv.obsGained, iv.obsGained.end()));

               set_difference(prev.begin(), prev.end(),
                              curr.begin(), curr.end(),
                              inserter(iv.obsLost, iv.obsLost.end()));

               if (!iv.obsGained.empty() || !iv.obsLost.empty())
               {
                  if (verboseLevel>2)
                     output << t.printf(timeFormat) << " prn:" << svid.id
                            << " gained:" << iv.obsGained
                            << " lost:" << iv.obsLost << endl;
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
   
   output << "\n Availability Raw Results :" << endl
          << endl
          << "Start                 End        #     PRN    Elv          Az  Hlth  ama ata" << endl
          << "============================================================================" << endl;
   
   for_each(sml.begin(), sml.end(), InView::dumper(output, timeFormat));

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
   float maskAngle,
   float trackAngle)
{
   SatID svid(prn, SatID::systemGPS);
   try
   {
      this->prn = prn;
      this->time = time;

      Xvt svXVT = eph.getXvt(svid, time);
      Position sv(svXVT.x);
      Position rx(rxpos);
      double el = rx.elevationGeodetic(sv);
      if (el > 0)
      {
         if (!up)
         {
            firstEpoch = time;
            up = true;
            aboveMask = false;
            epochCount = 0;
            obs = SvObsEpoch();
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
      elevation = el;
      azimuth = rx.azimuth(sv);
      if (typeid(eph) == typeid(GPSEphemerisStore))
      {
         const GPSEphemerisStore& bce = dynamic_cast<const GPSEphemerisStore&>(eph);
         health = bce.getSatHealth(svid, time);
      }
      if (health == 0 && el > maskAngle)
         expectedCount++;
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
void DataAvailabilityAnalyzer::InView::dump(ostream& s, const string fmt)
   const
{
   DayTime t0 = time - span;
   double timeUp     = t0 - firstEpoch;
   double timeUpMask = t0 - firstEpochAboveMask;
   char dir;
   if (elevation > 0)
      dir = rising ? '^' : 'v';
   else
      dir = ' ';

   s << left << t0.printf(fmt);
   if (smashCount)
      s << " - " << time.printf("%02H:%02M:%04.1f");
   else
      s << "   " << "          ";
   s << " " << setw(5) << smashCount+1
     << " ";

   if (prn>0)
   {
      s << setw(3) << prn << " " << fixed << right
        << setprecision(2) << setw(6) << elevation;
      if (smashCount)
         s << " - " << setprecision(2) << left << setw(5) << last_elevation << right;
      else
         s << dir << setw(7) << " ";
      s << "  "
        << setprecision(0) << setw(3) << azimuth << "  "
        << hex << setw(2) << health << "  " << dec
        << "   "
        << left << setw(2) << numAboveMaskAngle << "  "
        << left << setw(2) << numAboveTrackAngle << "  ";

      if (false && up) // in hindsight, this isn't proving to be that usefull
         s << right << setw(9) << secAsHMS(timeUp)
           << " " << setw(9) << secAsHMS(timeUpMask) << " ";

      if (!obsLost.empty())
         s << "-" << obsLost;
      if (!obsLost.empty() && !obsGained.empty())
         s << ", ";
      if (!obsGained.empty())
         s << "+" << obsGained;
      
      s << right;

      if (!up)
         s << " below horizon ";

      if (!aboveMask)
         s << " below mask angle ";
   }
   else
   {
      s << "all" << setw(28) << " " 
        << left << setw(2) << numAboveMaskAngle << " "
        << left << setw(2) << numAboveTrackAngle << "  ";
   }

   s << endl;
}


void DataAvailabilityAnalyzer::outputSummary()
{
   vector<long> missed(32);
   for (int prn=1; prn<=32; prn++)
      if (inView[prn].expectedCount)
         missed[prn] = inView[prn].expectedCount - inView[prn].receivedCount;
      else
         missed[prn] = 0;

   unsigned long channelLoss = totalSvEpochCounter - expectedSvEpochCounter;
   unsigned long missedSvEpochCounter = expectedSvEpochCounter - receivedSvEpochCounter;
   double pctMiss = 100.0 * missedSvEpochCounter / expectedSvEpochCounter;

   output << endl
          << " Summary:" << endl
          << endl
          << "Analysis spans " <<  firstEpochTime
          << " through " << lastEpochTime << endl
          << left << fixed
          << "Total number of epochs with data: " << epochCounter << endl
          << "Epochs with any data missing: " << anyMissingCounter << endl
          << "Epochs without data from any SV: " << allMissingCounter << endl
          << "SV-Epochs expected: " << expectedSvEpochCounter << endl
          << setprecision(5)
          << "Channel Loss: " << 100.0*channelLoss/totalSvEpochCounter
          << " % (" << channelLoss << ")" << endl
          << "SV-Epochs missed: " << 100.0* missedSvEpochCounter/expectedSvEpochCounter
          << " % (" << missedSvEpochCounter << ")" << endl;

   if (verboseLevel)
   {
      output << endl << "prn expected    missed" << endl;
      for (int prn=1; prn<=32; prn++)
         if (inView[prn].expectedCount)
            output << setw(2) << prn << "    "
                   << setw(8) << inView[prn].expectedCount << "  "
                   << setw(8) << setprecision(5)
                   << 100.0*missed[prn] / inView[prn].expectedCount << " %"
                   << " (" << missed[prn] << ")" << endl;
   }
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
               s << ObsID::cbStrings[i->band] << " " << ObsID::tcStrings[i->code];
            }
   }
}
