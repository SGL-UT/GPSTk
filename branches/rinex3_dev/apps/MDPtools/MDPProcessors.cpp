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

/** @file Various presentations/analysis on MDP streams */

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
      pvtOut(false), obsOut(false), navOut(false), tstOut(false),
      followEOF(false)
{}

MDPProcessor::MDPProcessor(gpstk::MDPStream& in, std::ofstream& out) :
      timeFormat("%4Y/%03j/%02H:%02M:%04.1f"),
      stopTime(gpstk::DayTime::END_OF_TIME),
      startTime(gpstk::DayTime::BEGINNING_OF_TIME),
      timeSpan(-1), processBad(false), bugMask(0),
      debugLevel(0), verboseLevel(0), in(in), out(out), die(false),
      pvtOut(false), obsOut(false), navOut(false), tstOut(false),
      followEOF(false)
{}

void MDPProcessor::process()
{
   MDPHeader header;

   msgCount=0;
   firstFC=0;
   lastFC=0;
   fcErrorCount=0;

   while (!die)
   {
      in >> header;

      if (in.eof())
      {
         if (followEOF)
            in.clear();
         else
            die=true;
         continue;
      }
      
      if (!in)
      {
         if (debugLevel)
         {
            cout << "File corruption detected." << endl;
            in.dumpState();
         }
         continue;
      }
      
      if (startTime == DayTime(DayTime::BEGINNING_OF_TIME) && timeSpan>0)
      {
         startTime = header.time;
         if (debugLevel)
            out << "startTime: " << startTime << endl;
      }
      
      if (stopTime == DayTime(DayTime::END_OF_TIME) && timeSpan>0)
      {
         stopTime = startTime + timeSpan;
         if (debugLevel)
            out << "stopTime: " << stopTime << endl;
      }

      if (header.time > stopTime)
         return;
            
      if (header.time < startTime)
         continue;

      msgCount++;

      if (verboseLevel>5 || debugLevel>2)
         out << "Record: " << in.recordNumber
             << ", message: " << msgCount << endl;

      switch (header.id)
      {
         case gpstk::MDPObsEpoch::myId:
         {
            gpstk::MDPObsEpoch obs;
            in >> obs;
            if (obs || processBad)
            {
               processFC(header);
               if (obsOut)
                  process(obs);
            }
            break;
         }

         case gpstk::MDPPVTSolution::myId:
         {
            gpstk::MDPPVTSolution pvt;
            in >> pvt;
            if (pvt || processBad)
            {
               processFC(header);
               if (pvtOut)
                  process(pvt);
            }
            break;
         }
         
         case gpstk::MDPNavSubframe::myId:
         {
            gpstk::MDPNavSubframe nav;
            in >> nav;
            if (nav || processBad)
            {
               processFC(header);
               if (navOut)
                  process(nav);
            }
            break;
         }
         
         case gpstk::MDPSelftestStatus::myId:
         {
            gpstk::MDPSelftestStatus sts;
            in >> sts;
            if (sts || processBad)
            {
               processFC(header);
               if (tstOut) 
                  process(sts);
            }
            break;
         }
         default:
            if (debugLevel)
               cout << "Unreconized id:" << in.header.id << endl;
      } // end of switch()
   } // end of while()
}


//-----------------------------------------------------------------------------
void MDPProcessor::processFC(const MDPHeader& header)
{
   if (msgCount == 1)
      firstFC = lastFC = in.header.freshnessCount;
   else
   {
      if (in.header.freshnessCount != static_cast<unsigned short>(lastFC+1))
      {
         fcErrorCount++;
         if (verboseLevel)
            out << header.time.printf(timeFormat)
                <<"  Freshness count error.  Previous was " << lastFC
                << " current is " << in.header.freshnessCount << endl;
         if (debugLevel)
            in.header.dump(cout);
         }
      lastFC = in.header.freshnessCount;
   }
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
      out << "# time, 300, prn, chan, hlth, #SVs, ele, az, code, carrier, LC, SNR, range, phase, doppler, record #" << endl;
   if (pvtOut)
      out << "# time, 301, #SV, dtime, ddtime, x, y, z, vx, vy, vz, record #" << endl;
   if (navOut)
      out << "# time, 310, prn, carrier_code, range_code, nav_code, word1, word2, ..." << endl;
   if (tstOut)
      out << "# time, 400, tstTime, startTime, Tant, Trx, status, cpu, freq, ssw, record #" << endl;

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
          << ", " << setw(10) << in.recordNumber
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
       << ", " << setw(10) << in.recordNumber
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
       << ", " << sts.selfTestTime.printf(timeFormat)
       << ", " << sts.firstPVTTime.printf(timeFormat) //"%4F/%9.2g")
       << ", " << setprecision(1) << sts.antennaTemp
       << ", " << setprecision(1) << sts.receiverTemp
       << ", " << hex << sts.status << dec
       << ", " << setprecision(1) << sts.cpuLoad
       << ", " << hex << sts.extFreqStatus << dec
       << ", " << hex << sts.saasmStatusWord << dec
       << ", " << setw(10) << in.recordNumber
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


//-----------------------------------------------------------------------------
MDPCSVProcessor::MDPCSVProcessor(gpstk::MDPStream& in, std::ofstream& out) :
   MDPProcessor(in, out), headerDone(false)
{}


//-----------------------------------------------------------------------------
void MDPCSVProcessor::outputHeader()
{
   if (headerDone)
      return;

   if (obsOut)
      out << "# mjd, 300, prn, chan, hlth, nSVs, ele, az, code, carrier, LC, SNR, range, phase, doppler" << endl;
   if (pvtOut)
      out << "# mjd, 301, #SV, dtime, ddtime, x, y, z, vx, vy, vz" << endl;
   if (navOut)
      out << "# mjd, 310, prn, carrier_code, range_code, nav_code, word1, word2, ..." << endl;
   if (tstOut)
      out << "# mjd, 400, tstTime, startTime, Tant, Trx, status, cpu, freq, ssw" << endl;

   headerDone=true;
}


//-----------------------------------------------------------------------------
// This is done as a sprintf to make the code as fast as possible.
void MDPCSVProcessor::process(const gpstk::MDPObsEpoch& oe)
{
   outputHeader();

   sprintf(buff0, "%f, %3d, %2d, %2d, %2d, %2d, %2.0f, %3.0f, ",
           oe.time.MJDdate(), oe.id, oe.prn, oe.channel, oe.status, oe.numSVs,
           oe.elevation, oe.azimuth);

   MDPObsEpoch::ObsMap::const_iterator i;
   for (i = oe.obs.begin(); i != oe.obs.end(); i++)
   {
      const MDPObsEpoch::Observation& obs=i->second;
      sprintf(buff1, "%1d, %1d, %7ld, %5.2f, %14.4f, %14.4f, %10f\n",
              obs.range, obs.carrier, obs.lockCount, obs.snr,
              obs.pseudorange,obs.phase,obs.doppler);
      out << buff0 << buff1;
   }
}



//-----------------------------------------------------------------------------
void MDPCSVProcessor::process(const gpstk::MDPPVTSolution& pvt)
{
   outputHeader();

   out << pvt.time.MJDdate()
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
void MDPCSVProcessor::process(const gpstk::MDPNavSubframe& sf)
{
   outputHeader();
   out << sf.time.MJDdate()
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
void MDPCSVProcessor::process(const gpstk::MDPSelftestStatus& sts)
{
   outputHeader();
   out << sts.time.printf(timeFormat)
       << fixed
       << ", " << setw(3) << sts.id
       << ", " << sts.selfTestTime.MJDdate()
       << ", " << sts.firstPVTTime.MJDdate()
       << ", " << setprecision(1) << sts.antennaTemp
       << ", " << setprecision(1) << sts.receiverTemp
       << ", " << hex << sts.status << dec
       << ", " << setprecision(1) << sts.cpuLoad
       << ", " << hex << sts.extFreqStatus << dec
       << ", " << hex << sts.saasmStatusWord << dec
       << endl;
}
