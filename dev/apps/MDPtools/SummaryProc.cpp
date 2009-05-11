#pragma ident "$Id$"

/*
  This intended to perform a quick summary/analysis of the data in a MDP file
  or stream. The idea is teqc +meta or +mds with a little bit of +qc thrown
  in for good measure.

  Mainly driven by the needs of the receiver test cases. The following are
  some of the test cases that this class is to support.

  RS-13  | needs to report data gaps |
  RS-16  | needs to report data gaps (based upon presense of data, not sv visibility) |
  RS-31  | Needs to report jumps in the clock offset reported in the PVT messages |
  RS-32  | "" |
  RS-58  | Analyze the changes in lock count over tracking anomolies |
  RS-72  | |
  RS-133  | Heh, need to run this for 90 days... |
*/

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
//============================================================================

#include "Geodetic.hpp"
#include "EngEphemeris.hpp"
#include "SummaryProc.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


//-----------------------------------------------------------------------------
MDPSummaryProcessor::MDPSummaryProcessor(gpstk::MDPStream& in, std::ofstream& out)
   : MDPProcessor(in, out),
     numEpochs(0), numObsEpochMsg(0),
     firstObs(true), firstPvt(true), firstNav(true), firstSelftest(true),
     firstObsTime(gpstk::DayTime::END_OF_TIME),
     lastObsTime(gpstk::DayTime::BEGINNING_OF_TIME),
     firstNavTime(gpstk::DayTime::END_OF_TIME),
     lastNavTime(gpstk::DayTime::BEGINNING_OF_TIME),
     prevEpochTime(gpstk::DayTime::BEGINNING_OF_TIME),
     obsRateEst(0), pvtRateEst(0),
     prevObs(maxChannel+1),
     chanGapList(maxChannel+1),
     svCountErrorCount(0),
     navSowErrors(0),
     navSowMiscompares(0),
     navParityErrors(0),
     navSubframes(0)
{
   elevBins.push_back(elevationPair(10,90));
   elevBins.push_back(elevationPair( 0, 5));
   elevBins.push_back(elevationPair( 5,10));
   elevBins.push_back(elevationPair(10,20));
   elevBins.push_back(elevationPair(20,60));
   elevBins.push_back(elevationPair(60,90));
   processBad = true;
}


//-----------------------------------------------------------------------------
MDPSummaryProcessor::~MDPSummaryProcessor()
{
   using gpstk::RangeCode;
   using gpstk::CarrierCode;
   using gpstk::StringUtils::asString;
   
   out << "Done processing data." << endl << endl;

   out << endl << "Header summary:" << endl;
   out << "  Processed "<< msgCount << " headers." << endl
       << "  First freshness count was " << hex << firstFC << dec << endl
       << "  Last freshness count was  " << hex << lastFC << dec << endl
       << "  Encountered " << fcErrorCount << " breaks in the freshness count" << endl;

   out << endl << "Observation Epoch message summary:" << endl;

   if (firstObs)
      out << "No Observation Epoch messages processed." << endl;
   else
   {
      double dt = lastObsTime - firstObsTime;
      out << "  Processed " << numObsEpochMsg
          << " observation epoch messages spanning "
          << numEpochs << " epochs."
          << endl
          << "  Obs data spans " << firstObsTime.printf(timeFormat) 
          << " to " << lastObsTime.printf(timeFormat)
          << " (" << secondsAsHMS(dt) << ")"
          << endl
          << "  Obs output rate is " << setprecision(2) << obsRateEst
          << " sec."
          << endl;

      out << "  Receiver data gaps:" << endl;
      int rxGapCount=0;
      for (DayTimePairList::const_iterator i=epochGapList.begin(); i!=epochGapList.end(); i++)
         if (std::abs(i->first - i->second - obsRateEst) > 1e-3)
            out << "    " << rxGapCount++ << ": "
                << i->second.printf(timeFormat)
                << " to " << i->first.printf(timeFormat)
                << " ( " << secondsAsHMS(i->first - i->second) << " )."
                << endl;
      if (rxGapCount==0)
         out << "    none" << endl;

      out << endl;

      for (elevBinList::const_iterator i=elevBins.begin(); i!=elevBins.end(); i++)
      {
         const ocm &oc = whack[*i];
         if (oc.size())
            out << "  Elevation: " << i->first << "..." << i->second << endl;
         for (ocm::const_iterator j=oc.begin(); j!=oc.end(); j++)
         {
            const rc_set ccs = j->first;
            if (ccs.size())
            {
               out << setw(10) << j->second << "   ";
               for (rc_set::const_iterator k=ccs.begin(); k!=ccs.end(); k++)
                  out << "(" << asString(k->second)
                      << ", " << asString(k->first) << ")";
               out << endl;
            }
         }
         for (rcpm::const_iterator j= lol[*i].begin(); j != lol[*i].end(); j++)
            out << setw(10) << j->second << "   "
                << asString(j->first.second) << " " << asString(j->first.first)
                << " lock count discontinuities" << endl;
         if (verboseLevel<2)
            break;
         out << endl;
      }
      
      out << "Encountered " << svCountErrorCount << " SV count errors." << endl
          << endl;
   }

   out << endl << "PVT Solution message summary:" << endl;
   if (firstPvt)
      out << "  No PVT Solution messages processed." << endl;
   else
   {
      double dt = lastPvtTime - firstPvtTime;
      out << "  Pvt data spans " << firstPvtTime.printf(timeFormat) 
          << " to " << lastPvtTime.printf(timeFormat)
          << " (" << secondsAsHMS(dt) << ")"
          << endl
          << "  PVT output rate is " << setprecision(2) << pvtRateEst << " sec."
          << endl << endl;
   }


   out << endl << "Navigation Subframe message summary:" << endl;
   if (firstNav)
      out << "  No Navigation Subframe messages processed." << endl;
   else
   {
      double dt = lastNavTime - firstNavTime;
      double parityErrorPct = 100.0 * navParityErrors / navSubframes;
      double sowErrorPct = 100.0 * navSowErrors / navSubframes;
      out << "  Nav data spans " << firstNavTime.printf(timeFormat) 
          << " to " << lastNavTime.printf(timeFormat)
          << " (" << secondsAsHMS(dt) << ")" << endl
          << setw(10) << navSubframes      << "   Subframes received" << endl
          << setw(10) << navParityErrors   << "   Parity errors ("
          << parityErrorPct << " %)" << endl
          << setw(10) << navSowErrors      << "   Bogus HOW SOWs ("
          << sowErrorPct << " %)" << endl
          << setw(10) << navSowMiscompares << "   SOW vs header time miscompares" << endl
          << endl;
   }

   out << endl;
   if (badMessages.size())
   {
      out << "Received " << badMessages.size() << " messages with an error." << endl;

      if (verboseLevel>2)
      {
         out << "Headers from the bad messages:" << endl;
         for (MDPList::const_iterator i=badMessages.begin(); i!=badMessages.end(); i++)
            i->dump(out);
      }
   }
}  // ~MDPSummaryProcessor()


//-----------------------------------------------------------------------------
void MDPSummaryProcessor::process(const gpstk::MDPObsEpoch& msg)
{
   if (!msg)
   {
      badMessages.push_back(msg);
      return;
   }

   // First, do gross accounting on the rate the obs are coming in
   if (firstObs)
   {
      firstObsTime = msg.time;
      firstObs = false;
      if (verboseLevel)
         out << msg.time.printf(timeFormat)
             << "  Received first Observation Epoch message"
             << endl;
   }
   else
   {
      double dt = msg.time - prevEpochTime;
      if (std::abs(dt - obsRateEst) > 1e-3 && dt>0)
      {
         if (obsRateEst > 0)
         {
            gpstk::DayTime first =  prevEpochTime + dt;
            gpstk::DayTime second = msg.time - dt;
            epochGapList.push_back(DayTimePair(first, second));
            if (verboseLevel)
            {
               out << msg.time.printf(timeFormat)
                   << "  Obs output rate " << dt << " sec";
               if (obsRateEst != 0)
                  out << " (was " << obsRateEst << " sec).";
               out << endl;
            }
         }
         obsRateEst = dt;
      }
   }

   lastObsTime=msg.time;

   // Next, make a set of the obs that this epoch has and add 
   // this to a list in the appropriate elevation bin
   rc_set ccs;
   for (gpstk::MDPObsEpoch::ObsMap::const_iterator i = msg.obs.begin();
        i != msg.obs.end(); i++)
   {
      const gpstk::MDPObsEpoch::Observation& obs=i->second;
      rcpair rcPair(obs.range, obs.carrier);
      ccs.insert(rcPair);
   }

   // figure out what bins we should update code/carrier counts on
   for (elevBinList::const_iterator i=elevBins.begin(); i!=elevBins.end(); i++)
      if (msg.elevation >= i->first && msg.elevation <= i->second)
         whack[*i][ccs]++;

   // This part does some accounting on a per channel basis. The intent is to
   // look for when there is a gap in the data on a channel.
   int prn=msg.prn;
   int chan=msg.channel;

   if (prevObs[chan].prn == 0 || prevObs[chan].prn != prn)
   {
      if (verboseLevel>1)
         out << msg.time.printf(timeFormat)
             << "  PRN " << prn << " now on channel " << chan << endl;
   }
   else
   {
      // flag when there is a gap on this channel
      double dt = msg.time - prevObs[chan].time;
      if (std::abs(dt) < 1e-3)
      {
         out << msg.time.printf(timeFormat)
             << "  Got two consecutive obs on channel "
             << chan << " with the same time." << endl;
         if (verboseLevel)
            msg.dump(out), prevObs[chan].dump(out);
      }
      else if (obsRateEst == 0)
      {
         // do nothing
      }
      else if ( (std::abs(dt - obsRateEst) > 1e-3) &&
                (prevObs[chan].prn == msg.prn) )
      {
         gpstk::DayTime first =  prevObs[chan].time + dt;
         gpstk::DayTime second = msg.time - dt;
         chanGapList[chan].push_back(DayTimePair(first, second));
         if (verboseLevel)
            out << msg.time.printf(timeFormat)
                << "  Data gap on channel " << chan
                << ", prn " << msg.prn << ", " << secondsAsHMS(dt)
                << endl;
         if (verboseLevel>2)
         {
            out << "  prev obs on chan " << chan << endl;
            prevObs[chan].dump(out);
            out << "  curr obs on chan" << chan << endl;
            msg.dump(out);
         }
      }

      // Look for discontinuities in the lock count
      // Since this can be quite verbose, only do it in verbose mode
      for (gpstk::MDPObsEpoch::ObsMap::const_iterator i = msg.obs.begin();
           i != msg.obs.end(); i++)
      {
         const gpstk::MDPObsEpoch::Observation& curr=i->second;
         if (prevObs[chan].haveObservation(curr.carrier, curr.range))
         {
            gpstk::MDPObsEpoch::Observation prev =
               prevObs[chan].getObservation(curr.carrier, curr.range);
            if (curr.lockCount - prev.lockCount != 1 && prev.lockCount > 0)
            {
               // figure out what bins we should update loss-of-lock counts on
               rcpair rcPair(curr.range, curr.carrier);
               for (elevBinList::const_iterator j=elevBins.begin();
                    j != elevBins.end(); j++)
                  if (msg.elevation >= j->first && msg.elevation <= j->second)
                     lol[*j][rcPair]++;

               if (verboseLevel)
                  out << msg.time.printf(timeFormat)
                      << "  Lock count discontinuity on prn " << prn
                      << ", chan " << chan
                      << ", " << asString(i->first.first)
                      << " "  << asString(i->first.second)
                      << ", elev " << msg.elevation
                      << " (" << prev.lockCount
                      << " -> " << curr.lockCount
                      << ")" << endl;
            }
         }
      }
   }

   // Keep track of the number of epochs we have processed and check the
   // numSVs field.
   if (prevEpochTime != msg.time)
   {
      numEpochs++;

      int prevActual=0;
      int prevReported=0;
      for (size_t i = 1; i<=maxChannel; i++)
      {
         if (prevObs[i].time == prevEpochTime)
         {
            prevActual++;
            if (prevReported==0)
               prevReported = prevObs[i].numSVs;
         }
      }
      if (prevActual != prevReported)
      {
         svCountErrorCount++;
         if (! (bugMask & 0x01))
            out << prevEpochTime.printf(timeFormat)
                << "  Epoch claimed " << prevReported
                << " SVs but only received " << prevActual << endl;
      }
   }

   prevObs[chan] = msg;
   prevEpochTime = msg.time;
   numObsEpochMsg++;
}


//-----------------------------------------------------------------------------
void MDPSummaryProcessor::process(const gpstk::MDPPVTSolution& msg)
{
   if (!msg)
   {
      badMessages.push_back(msg);
      return;
   }

   if (firstPvt)
   {
      firstPvt = false;
      firstPvtTime = msg.time;
      if (verboseLevel)
         out << msg.time.printf(timeFormat)
             << "  Received first PVT Solution message"
             << endl;
   }
   else
   {
      double dt = msg.time - prevPvt.time;
      if (std::abs(dt - pvtRateEst) > 1e-3 && dt>0)
      {
         if (verboseLevel)
         {
            out << msg.time.printf(timeFormat)
                << "  PVT output rate " << dt << " sec";
            if (pvtRateEst != 0)
               out << "(was " << pvtRateEst << " sec).";
            out << endl;
         }
         pvtRateEst = dt;

         // flag when there is a gap
         double dt = msg.time - prevPvt.time;
         if (std::abs(dt) < 1e-3)
         {
            out << msg.time.printf(timeFormat)
                << "  Got two consecutive PVT messages with the same time." << endl;
            if (verboseLevel)
               msg.dump(out), prevPvt.dump(out);
         }
         else if (pvtRateEst == 0)
         {
            // do nothing
         }
         else if ( std::abs(dt - pvtRateEst) > 1e-3 )
         {
            gpstk::DayTime first =  prevPvt.time + dt;
            gpstk::DayTime second = msg.time - dt;
            if (verboseLevel)
               out << msg.time.printf(timeFormat)
                   << "  Gap in PVT messages: "  << secondsAsHMS(dt)
                   << endl;
            if (verboseLevel>2)
            {
               out << "  prev pvt:" << endl;
               prevPvt.dump(out);
               out << "  curr obs:" << endl;
               msg.dump(out);
            }
         }
      
         // Look for discontinuities in the recevier clock estimate
         double ddt = msg.dtime - prevPvt.dtime;
         double dtdt = ddt/(msg.time - prevPvt.time);
         double dtdtErr = std::abs(dtdt - msg.ddtime);
         if (dtdt > 1e-6)
            out << msg.time.printf(timeFormat)
                << "  Clock jump: " << setprecision(3) << scientific << ddt
                << " sec, (" << dtdt << " vs " << msg.ddtime
                << " sec/sec)"
                << fixed << endl;
         else if (dtdtErr > 1e-8 && verboseLevel)
            out << msg.time.printf(timeFormat)
                << "  Clock error: " << setprecision(3) << scientific << ddt
                << " sec, (" << dtdt << " vs " << msg.ddtime
                << " sec/sec)"
                << fixed << endl;
      }

      prevPvt = msg;
      lastPvtTime = msg.time;
   }
}

//-----------------------------------------------------------------------------
void MDPSummaryProcessor::process(const gpstk::MDPNavSubframe& msg)
{
   if (!msg)
   {
      badMessages.push_back(msg);
      return;
   }

   gpstk::MDPNavSubframe umsg = msg;
   navSubframes++;
   string desc = msg.time.printf(timeFormat) + 
      "  Subframe from " + leftJustify(asString(umsg.prn), 2) +
      " " + leftJustify(asString(umsg.carrier), 2) +
      " " + leftJustify(asString(umsg.range), 2);

   umsg.cookSubframe();   
   
   if (!(bugMask & 0x2) && !umsg.parityGood)
   {
      navParityErrors++;
      if (verboseLevel>1)
         out << desc << ", bad parity" << endl;
      if (verboseLevel>2)
         umsg.dump(out);
      return;
   }

   if (umsg.inverted)
      desc += ", inverted";
   else
      desc += ", upright";

   long how_sow = umsg.getHOWTime();
   long hdr_sow = static_cast<long>(umsg.time.GPSsow());
   if (how_sow < 0 || how_sow >= 604800)
   {
      navSowErrors++;
      if (verboseLevel>1)
         out << desc << ", bogus HOW SOW(" << how_sow << ")" << endl;
      return;
   }

   if ( (how_sow != hdr_sow+6 && how_sow != hdr_sow) ||
        (how_sow == hdr_sow && !(bugMask & 0x4))        )
   {
      navSowMiscompares++;
      if (verboseLevel>1)
         out << desc << ", HOW/header time miscompare, how:" 
             << how_sow << " header:" << hdr_sow << endl;
      return;
   }

   if (umsg && firstNav)
   {
      firstNav = false;
      firstNavTime = umsg.time;
      desc += ", first good subframe";
      if (verboseLevel && verboseLevel<=2)
         out << desc << endl;
   }

   if (verboseLevel>2)
      out << desc << endl;

   lastNavTime = umsg.time;
}


//-----------------------------------------------------------------------------
void MDPSummaryProcessor::process(const gpstk::MDPSelftestStatus& msg)
{
   if (!msg)
   {
      badMessages.push_back(msg);
      return;
   }

   if (firstSelftest)
   {
      firstSelftest = false;
      if (verboseLevel)
         out << msg.time.printf(timeFormat)
             << "  Received first Selftest Status message"
             << endl;
   } 

   if (verboseLevel)
   {
      if (msg.extFreqStatus != prevSelftestStatus.extFreqStatus)
         out << msg.time.printf(timeFormat)
             << "  External Frequency Status: "
             << msg.extFreqStatus
             << endl;
      
      if (msg.saasmStatusWord != prevSelftestStatus.saasmStatusWord)
         out << msg.time.printf(timeFormat)
             << "  SAASM Status Word: 0x"
             << hex << msg.saasmStatusWord << dec
             << endl;
   }
   prevSelftestStatus = msg;
}

std::string MDPSummaryProcessor::secondsAsHMS(double seconds) const
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
      oss << fixed << setprecision(1) << setw(4) << seconds+s;
   else
      oss << setfill(' ') << fixed << setprecision(1) << seconds+s << " s";

   return oss.str();
}
