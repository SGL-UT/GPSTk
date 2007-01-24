#pragma ident "$Id$"


/** @file Various presentations/analysis on MDP streams */

//lgpl-license START
//lgpl-license END

#include "MDPProcessors.hpp"

using namespace std;
using namespace gpstk;

gpstk::MDPStream d1;
std::ofstream d2;


MDPProcessor::MDPProcessor() :
      timeFormat("%4Y/%03j/%02H:%02M:%04.1f"),
      stopTime(gpstk::DayTime::END_OF_TIME),
      startTime(gpstk::DayTime::BEGINNING_OF_TIME),
      timeSpan(-1), processBad(false), bugMask(0),
      debugLevel(0), verboseLevel(0), in(d1), out(d2), die(false),
      pvtOut(false), obsOut(false), navOut(false), tstOut(false)
{}

MDPProcessor::MDPProcessor(gpstk::MDPStream& in, std::ofstream& out) :
      timeFormat("%4Y/%03j/%02H:%02M:%04.1f"),
      stopTime(gpstk::DayTime::END_OF_TIME),
      startTime(gpstk::DayTime::BEGINNING_OF_TIME),
      timeSpan(-1), processBad(false), bugMask(0),
      debugLevel(0), verboseLevel(0), in(in), out(out), die(false),
      pvtOut(false), obsOut(false), navOut(false), tstOut(false)
{}

void MDPProcessor::process()
{
   MDPHeader header;

   msgCount=0;
   firstFC=0;
   lastFC=0;
   fcErrorCount=0;

   while (!die && in >> header)
   {
      if (startTime == DayTime(DayTime::BEGINNING_OF_TIME) && timeSpan>0)
      {
         startTime = header.time;
         if (debugLevel)
            cout << "startTime: " << startTime << endl;
      }
      
      if (stopTime == DayTime(DayTime::END_OF_TIME) && timeSpan>0)
      {
         stopTime = startTime + timeSpan;
         if (debugLevel)
            cout << "stopTime: " << stopTime << endl;
      }

      if (header.time > stopTime)
         return;
            
      if (header.time < startTime)
         continue;

      msgCount++;

      if (verboseLevel>3)
         out << "Record: " << in.recordNumber
             << ", message: " << msgCount << endl;

      if (msgCount == 1)
         firstFC = lastFC = in.header.freshnessCount;
      else
      {
         if (in.header.freshnessCount != static_cast<unsigned short>(lastFC+1))
         {
            fcErrorCount++;
            if (verboseLevel)
               cout << header.time.printf(timeFormat)
                    <<" Freshness count error.  Previous was " << lastFC
                    << " current is " << in.header.freshnessCount << endl;
         }
         lastFC = in.header.freshnessCount;
      }

      switch (in.header.id)
      {
         case gpstk::MDPObsEpoch::myId:
            if (obsOut)
            {
               gpstk::MDPObsEpoch obs;
               in >> obs;
               if (obs || processBad)
                  process(obs);
            }
            break;

         case gpstk::MDPPVTSolution::myId:
            if (pvtOut)
            {
               gpstk::MDPPVTSolution pvt;
               in >> pvt;
               if (pvt || processBad)
                  process(pvt);
            }
            break;

         case gpstk::MDPNavSubframe::myId:
            if (navOut)
            {
               gpstk::MDPNavSubframe nav;
               in >> nav;
               if (nav || processBad)
                  process(nav);
            }
            break;

         case gpstk::MDPSelftestStatus::myId:
            if (tstOut) 
            {
               gpstk::MDPSelftestStatus sts;
               in >> sts;
               if (sts || processBad)
                  process(sts);
            }
            break;
      } // end of switch()
   } // end of while()
}


//-----------------------------------------------------------------------------
MDPTableProcessor::MDPTableProcessor(gpstk::MDPStream& in, std::ofstream& out) :
   MDPProcessor(in, out), headerDone(false)
{}


//-----------------------------------------------------------------------------
void MDPTableProcessor::outputHeader()
{
   if (headerDone)
      return;

   if (obsOut)
      out << "# time, 300, prn, chan, hlth, #SVs, ele, az, code, carrier, LC, SNR, range, phase, doppler" << endl;
   if (pvtOut)
      out << "# time, 301, #SV, dtime, ddtime, x, y, z, vx, vy, vz" << endl;
   if (navOut)
      out << "# time, 310, prn, carrier_code, range_code, nav_code, word1, word2, ..." << endl;
   if (tstOut)
      out << "# time, 400, tstTime, startTime, Tant, Trx, status, cpu, freq, ssw" << endl;

   headerDone=true;
}


//-----------------------------------------------------------------------------
void MDPTableProcessor::process(const gpstk::MDPObsEpoch& oe)
{
   outputHeader();
   MDPObsEpoch::ObsMap::const_iterator i;
   for (i = oe.obs.begin(); i != oe.obs.end(); i++)
   {
      const MDPObsEpoch::Observation& obs=i->second;
      out << oe.time.printf(timeFormat)
          << fixed
          << ", " << setw(3) << oe.id
          << ", " << setw(2) << (int) oe.prn
          << ", " << setw(2) << (int) oe.channel
          << ", " << setw(2) << hex << (int) oe.status << dec
          << ", " << setw(2) << (int) oe.numSVs
          << setprecision(1)
          << ", " << setw(2) << (int) oe.elevation
          << ", " << setw(3) << (int) oe.azimuth
          << ", " << setw(1) << obs.range
          << ", " << setw(1) << obs.carrier
          << ", " << setw(7) << obs.lockCount
          << setprecision(2)
          << ", " << setw(5) << obs.snr
          << setprecision(4)
          << ", " << setw(13) << obs.pseudorange
          << ", " << setw(14) << obs.phase
          << ", " << setw(10) << obs.doppler
          << endl;
   }
}


//-----------------------------------------------------------------------------
void MDPTableProcessor::process(const gpstk::MDPPVTSolution& pvt)
{
   outputHeader();
   out << pvt.time.printf(timeFormat)
       << fixed
       << ", " << setw(3) << pvt.id
       << ", " << setw(2) << (int)pvt.numSVs
       << setprecision(3)
       << ", " << setw(12) << pvt.dtime*1e9
       << setprecision(6)
       << ", " << setw(9)  << pvt.ddtime*1e9
       << setprecision(3)
       << ", " << setw(12) << pvt.x[0]
       << ", " << setw(12) << pvt.x[1]
       << ", " << setw(12) << pvt.x[2]
       << setprecision(3)
       << ", " << setw(8) << pvt.v[0]
       << ", " << setw(8) << pvt.v[1]
       << ", " << setw(8) << pvt.v[2]
       << endl;
}


//-----------------------------------------------------------------------------
void MDPTableProcessor::process(const gpstk::MDPNavSubframe& sf)
{
   outputHeader();
   out << sf.time.printf(timeFormat)
       << fixed
       << ", " << setw(3) << sf.id
       << ", " << setw(2) << sf.prn
       << ", " << int(sf.carrier)
       << ", " << int(sf.range)
       << ", " << int(sf.nav);

   if (verboseLevel)
   {
      out <<  setfill('0') << hex;
      for(int i = 1; i < sf.subframe.size(); i++)
         out << ", " << setw(8) << uppercase << sf.subframe[i];
      out << dec << setfill(' ');
   }
   out << endl;
}


//-----------------------------------------------------------------------------
void MDPTableProcessor::process(const gpstk::MDPSelftestStatus& sts)
{
   outputHeader();
   out << sts.time.printf(timeFormat)
       << fixed
       << ", " << setw(3) << sts.id
       << ", " << sts.selfTestTime.printf("%4F/%9.2g")
       << ", " << sts.firstPVTTime.printf("%4F/%9.2g")
       << ", " << setprecision(1) << sts.antennaTemp
       << ", " << setprecision(1) << sts.receiverTemp
       << ", " << hex << sts.status << dec
       << ", " << setprecision(1) << sts.cpuLoad
       << ", " << hex << sts.extFreqStatus << dec
       << ", " << hex << sts.saasmStatusWord << dec
       << endl;
}


//-----------------------------------------------------------------------------
void MDPVerboseProcessor::process(const gpstk::MDPObsEpoch& oe)
{
   if (verboseLevel)
   {
      oe.dump(out);
      out << endl;
   }
   else
   {
      out << oe.getName() << "-:"
          << " T:" << oe.time.printf(timeFormat)
          << left
          << " #SV:" << setw(2) << (int)oe.numSVs
          << " Ch:" << setw(2) << (int)oe.channel
          << " PRN:" << setw(2) << (int)oe.prn
          << " El:" << setw(2) << (int)oe.elevation;
      
      MDPObsEpoch::ObsMap::const_iterator i;
      for (i = oe.obs.begin(); i != oe.obs.end(); i++)
      {
         const MDPObsEpoch::Observation& obs=i->second;
         out << " " << StringUtils::asString(obs.carrier)
             << "-" << StringUtils::asString(obs.range);
      }
      out << endl;
   }


}


//-----------------------------------------------------------------------------
void MDPVerboseProcessor::process(const gpstk::MDPPVTSolution& pvt)
{
   if (verboseLevel)
   {
      pvt.dump(out);
      out << endl;
   }
   else
   {
      out << pvt.getName() << "-:"
          << " T:" << pvt.time.printf(timeFormat)
          << left
          << " #SV:" << setw(2) << (int)pvt.numSVs
          << " X:" << StringUtils::asString(pvt.x[0], 3)
          << " Y:" << StringUtils::asString(pvt.x[1], 3)
          << " Z:" << StringUtils::asString(pvt.x[2], 3)
          << endl;
   }
}


//-----------------------------------------------------------------------------
void MDPVerboseProcessor::process(const gpstk::MDPNavSubframe& sf)
{
   if (verboseLevel)
   {
      sf.dump(out);
      out << endl;
   }
   else
   {
      out << sf.getName() << "-:"
          << " T:" << sf.time.printf(timeFormat)
          << " PRN:" << sf.prn
          << " " << StringUtils::asString(sf.carrier)
          << "-" << StringUtils::asString(sf.range)
          << " " << static_cast<int>(sf.nav)
          << endl;
   }
}


//-----------------------------------------------------------------------------
void MDPVerboseProcessor::process(const gpstk::MDPSelftestStatus& sts)
{
   sts.dump(out);
   out << endl;
}
