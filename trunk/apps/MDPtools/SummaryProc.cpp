#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/MDPtools/SummaryProc.cpp#6 $"

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

#include "Geodetic.hpp"
#include "SummaryProc.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


//-----------------------------------------------------------------------------
MDPSummaryProcessor::MDPSummaryProcessor(gpstk::MDPStream& in, std::ofstream& out)
   : MDPProcessor(in, out),
     numEpochs(0), numObsEpochMsg(0),
     firstObs(true), firstPvt(true), firstNav(true), firstSelftest(true),
     firstEpochTime(gpstk::DayTime::END_OF_TIME),
     lastEpochTime(gpstk::DayTime::BEGINNING_OF_TIME),
     prevEpochTime(gpstk::DayTime::BEGINNING_OF_TIME),
     obsRateEst(0), pvtRateEst(0),
     prevObs(maxChannel+1),
     chanGapList(maxChannel+1)
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

   out << endl << "Observation Epoch message summary:" << endl;

   if (firstObs)
      out << "No Observation Epoch messages processed." << endl;
   else
   {

      double dt = lastEpochTime - firstEpochTime;
      out << "  Processed " << numObsEpochMsg
          << " observation epoch messages spanning "
          << numEpochs << " epochs."
          << endl
          << "  Obs data spans " << firstEpochTime.printf(timeFormat) 
          << " to " << lastEpochTime.printf(timeFormat)
          << " (" << secondsAsHMS(dt) << ")"
          << endl
          << "  Obs output rate is " << obsRateEst
          << " sec."
          << endl << endl;

      if (epochGapList.size())
         for (DayTimePairList::const_iterator i=epochGapList.begin(); i!=epochGapList.end(); i++)
            out << "  Data gap from " << i->first.printf(timeFormat)
                << " to " << i->second .printf(timeFormat)
                << " ( " << secondsAsHMS(i->first - i->second) << " )."
                << endl;

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
         if (verboseLevel<2)
            break;
         out << endl;
      }
   }

   out << endl << "PVT Solution message summary:" << endl;
   if (firstPvt)
      out << "  No PVT Solution messages processed." << endl;
   else
   {
      out << "  PVT output rate is " << pvtRateEst << " sec." << endl;
   }


   out << endl << "Navigation Subframe message summary:" << endl;
   if (firstNav)
      out << "  No Navigation Subframe messages processed." << endl;
   else
   {
      out << "  TBD." << endl;
   }
      
   out << endl;
   if (badMessages.size())
   {
      out << "Received " << badMessages.size() << " messages with an error." << endl;
      if (verboseLevel)
         for (MDPList::const_iterator i=badMessages.begin(); i!=badMessages.end(); i++)
            i->dump(out);
   }
}


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
      firstEpochTime = msg.time;
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
         gpstk::DayTime first =  prevEpochTime + dt;
         gpstk::DayTime second = msg.time - dt;
         epochGapList.push_back(DayTimePair(first, second));
         if (verboseLevel)
         {
            out << msg.time.printf(timeFormat)
                << "  Obs output rate " << dt << " sec";
            if (obsRateEst != 0)
               out << "was " << obsRateEst << " sec).";
            out << endl;
         }
         obsRateEst = dt;
      }
   }
   lastEpochTime=msg.time;
   // Keep track of the number of epochs we have processed
   if (prevEpochTime != msg.time)
      numEpochs++;

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

   // First check to see if this channel has been used yet...
   if (prevObs[chan].prn == 0)
   {
      // do nothing
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
                << ", " << secondsAsHMS(dt)
                << endl;
         if (verboseLevel>2)
         {
            out << "  prev obs on chan " << chan << endl;
            prevObs[chan].dump(out);
            out << "  curr obs:" << endl;
            msg.dump(out);
         }
      }

      // Look for discontinuities in the lock count
      // Since this can be quite verbose, only do it in verbose mode
      if (verboseLevel)
      {
         for (gpstk::MDPObsEpoch::ObsMap::const_iterator i = msg.obs.begin();
              i != msg.obs.end(); i++)
         {
            const gpstk::MDPObsEpoch::Observation& curr=i->second;
            if (prevObs[chan].haveObservation(i->first.first, i->first.second))
            {
               gpstk::MDPObsEpoch::Observation prev = prevObs[chan].getObservation(i->first.first, i->first.second);
               if (curr.lockCount - prev.lockCount != 1)
               {
                  // The current ash2mdp has periods where it outputs every
                  // message with a lock count of zero 
                  if ((prev.lockCount > 0 && verboseLevel) || verboseLevel>1)
                     out << msg.time.printf(timeFormat)
                         << "  Lock count reset prn " << prn
                         << ", chan " << chan
                         << ", " << asString(i->first.first)
                         << " "  << asString(i->first.second)
                         << " (" << prev.lockCount
                         << " -> " << curr.lockCount
                         << ")" << endl;
               }
            }
         }
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
}


//-----------------------------------------------------------------------------
void MDPSummaryProcessor::process(const gpstk::MDPNavSubframe& msg)
{
   if (!msg)
   {
      badMessages.push_back(msg);
      return;
   }

   if (firstNav)
   {
      firstNav = false;
      if (verboseLevel)
         out << msg.time.printf(timeFormat)
             << "  Received first Navigation Subframe message"
             << endl;
   }
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
      oss << setw(2);
   oss << s;

   if (seconds >1e-3)
      oss << "." << setw(3) << seconds*1e3;

   if (h==0 && m==0)
      oss << " s";
   return oss.str();
}
