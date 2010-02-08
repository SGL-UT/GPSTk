#pragma ident "$Id$"

/*
  Think, navdmp for mdp, with bonus output that you get data from all code/carrier
  combos.
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
//  Copyright 2008, The University of Texas at Austin
//
//============================================================================

#include "Geodetic.hpp"
#include "NavProc.hpp"

#include "RinexConverters.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


//-----------------------------------------------------------------------------
MDPNavProcessor::MDPNavProcessor(MDPStream& in, std::ofstream& out)
   : MDPProcessor(in, out),
     firstNav(true), almOut(false), ephOut(false), minimalAlm(false)
{
   timeFormat = "%4Y/%03j/%02H:%02M:%02S";

   binByElevation = true;
   if (binByElevation)
   {
      double binSize=5;
      for (double x=0; x<90; x+=binSize)
         bins.push_back(Histogram::BinRange(x, x+binSize));
   }
   else
   {
      bins.push_back(Histogram::BinRange(0, 30));
      double binSize=3;
      for (double x=30; x<60; x+=binSize)
         bins.push_back(Histogram::BinRange(x, x+binSize));
      bins.push_back(Histogram::BinRange(60, 99));
   }
}


//-----------------------------------------------------------------------------
MDPNavProcessor::~MDPNavProcessor()
{
   using gpstk::RangeCode;
   using gpstk::CarrierCode;
   using gpstk::StringUtils::asString;
   
   out << "Done processing data." << endl << endl;
   if (firstNav)
   {
      out << "  No Navigation Subframe messages processed." << endl;
      return;
   }

   out << "Parity Errors" << endl;
   out << " elev ";
   std::map<RangeCarrierPair, Histogram>::const_iterator peh_itr, sfc_itr;
   for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
   {
      const RangeCarrierPair& rcp=peh_itr->first;
      out << "     " << asString(rcp.second)
          << "-"    << leftJustify(asString(rcp.first), 2)
          << "      ";
   }
   out << endl;

   Histogram::BinRangeList::const_iterator brl_itr;
   for (brl_itr = bins.begin(); brl_itr != bins.end(); brl_itr++)
   {
      const Histogram::BinRange& br = *brl_itr ;
      out << setprecision(0) << fixed
          << right << setw(2) << br.first << "-"
          << left  << setw(2) << br.second << ":";

      for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
      {
         const RangeCarrierPair& rcp=peh_itr->first;
         Histogram pec=peh_itr->second;
         Histogram sfc=sfCount[rcp];
         out << right << setw(7) << pec.bins[br] << "/" 
             << left << setw(8) << sfc.bins[br];
      }

      out << endl;
   }

   // Whoever would write a reference like this should be shot...
   out << right << setw(2) << peHist.begin()->second.bins.begin()->first.first << "-"
       << left << setw(2) << peHist.begin()->second.bins.rbegin()->first.second << ":";

   unsigned long sfc=0,pec=0;
   for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
   {
      const RangeCarrierPair& rcp=peh_itr->first;
      sfc += sfCount[rcp].total;
      pec += peh_itr->second.total;
      out << right << setw(7) << peh_itr->second.total << "/"
          << left << setw(8) << sfCount[rcp].total;
   }
   out << endl;

   // If elevation is being used for binning, then we compute statistics for
   // all data from 5 to 90 degrees.
   if (binByElevation)
   {
      out << " 5-90: ";
      for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
      {
         const RangeCarrierPair& rcp=peh_itr->first;
         unsigned long sfc=0,pec=0;
         for (brl_itr = bins.begin(); brl_itr != bins.end(); brl_itr++)
         {
            const Histogram::BinRange& br = *brl_itr ;
            if (br.second <= 5.0)
               continue;
         
            sfc += sfCount[rcp].bins[br];
            pec += peHist[rcp].bins[br];
         }
         out << right << setprecision(3) << setw(10)
             << 100.0 * pec / sfc
             << " %    ";
      }
      out << endl << endl;
   }


   out << "Navigation Subframe message summary:" << endl
       << "  navSubframeCount: " << sfc << endl
       << "  badNavSubframeCount: " << pec << endl
       << "  percent bad: " << setprecision(3) << 100.0*pec/sfc << " %" << endl;
}


class CRC24Q
{
public:
   static const std::size_t bit_count = 24;

   // 0,1,3,4,5,6,7,10,11,14,17,18,23,24
   //1000 0110 0100 1100 1111 1011: 0x864cfb
   CRC24Q()
      : rem(0) , poly(0x864cfb)
   {};

   void process_bit( bool bit)
   {
      rem ^= ( bit ? 0x00800000 : 0 );
      bool const  pdiv = static_cast<bool>( rem & 0x00800000 );
      rem <<= 1;
      if ( pdiv )
         rem ^= poly;
   };

   void process_bits( uint8_t bits, std::size_t bit_count = 8)
   {
      bits <<= 8 - bit_count;

      for ( std::size_t i = bit_count ; i > 0u ; --i, bits <<= 1u )
         process_bit( static_cast<bool>(bits & 0x80) );
   };

   void process_bytes( void const *buffer, std::size_t byte_count)
   {
      uint8_t const *b = static_cast<uint8_t const *>(buffer);
      for (int i=0; i<byte_count; i++)
         process_bits(*b++);
   };

   uint32_t checksum() const
   { return rem & 0x00ffffff; };

private:
   uint32_t  rem;
   const uint32_t poly;
};


//-----------------------------------------------------------------------------
void MDPNavProcessor::process(const MDPNavSubframe& msg)
{
   if (firstNav)
   {
      firstNav = false;
      if (verboseLevel)
         out << msg.time.printf(timeFormat)
             << "  Received first Navigation Subframe message"
             << endl;
   }

   // Ignore nav data from codeless tracking
   if (msg.range == rcCodeless)
      return;

   RangeCarrierPair rcp(msg.range, msg.carrier);
   NavIndex ni(rcp, msg.prn);

   MDPNavSubframe umsg = msg;

   ostringstream oss;
   oss << umsg.time.printf(timeFormat)
       << "  PRN:" << setw(2) << umsg.prn
       << " " << asString(umsg.carrier)
       << ":" << setw(4) << left << asString(umsg.range)
       << "  ";
   string msgPrefix = oss.str();

   // Lets ignore L2 C/A data for the time being
   if (rcp.first == rcCA && rcp.second == ccL2)
      return;

   if (sfCount.find(rcp) == sfCount.end())
   {
      sfCount[rcp].resetBins(bins);
      peHist[rcp].resetBins(bins);
   }
   
   if (binByElevation)
     sfCount[rcp].addValue(el[ni]);
   else
     sfCount[rcp].addValue(snr[ni]);

   // For the moment, we need to processess each type of nav data uniquely - this needs to change.
   if (msg.nav == ncICD_200_2)
   {
      umsg.cookSubframe();
      if (verboseLevel>3 && umsg.neededCooking)
         out << msgPrefix << "Subframe required cooking" << endl;

      if (verboseLevel>2)
         umsg.dump(cout);

      if (!umsg.parityGood)
      {
         if (verboseLevel)
            out << msgPrefix << "Parity error"
                << " SNR:" << fixed << setprecision(1) << snr[ni]
                << " EL:" << el[ni]
                << endl;

         if (binByElevation)
            peHist[rcp].addValue(el[ni]);
         else
            peHist[rcp].addValue(snr[ni]);

         return;
      }

      short sfid = umsg.getSFID();
      short svid = umsg.getSVID();
      bool isAlm = sfid > 3;
      long sow = umsg.getHOWTime();
      short page = ((sow-6) / 30) % 25 + 1;

      if (((isAlm && almOut) || (!isAlm && ephOut))
          && verboseLevel>2)
      {
         out << msgPrefix
             << "SOW:" << setw(6) << sow
             << " NC:" << static_cast<int>(umsg.nav)
             << " I:" << umsg.inverted
             << " SFID:" << sfid;
         if (isAlm)
            out << " SVID:" << svid
                << " Page:" << page;
         out << endl;
      }

      // Sanity check on the header time versus the HOW time
      short week = umsg.time.GPSfullweek();
      if (sow <0 || sow>=604800)
      {
         if (verboseLevel>1)
            out << msgPrefix << "  Bad SOW: " << sow << endl;
         return;
      }
      
      DayTime howTime(week, umsg.getHOWTime());
      if (howTime == umsg.time)
      {
         if (verboseLevel && ! (bugMask & 0x4))
            out << msgPrefix << " Header time==HOW time" << endl;
      }
      else if (howTime != umsg.time+6)
      {
         if (verboseLevel>1)
            out << msgPrefix << " HOW time != hdr time+6, HOW:"
                << howTime.printf(timeFormat)
                << endl;
         if (verboseLevel>3)
            umsg.dump(out);
         return;
      }

      prev[ni] = curr[ni];
      curr[ni] = umsg;

      if (prev[ni].parityGood && 
          prev[ni].inverted != curr[ni].inverted && 
          curr[ni].time - prev[ni].time <= 12)
      {
         if (verboseLevel)
            out << msgPrefix << "Polarity inversion"
                << " SNR:" << fixed << setprecision(1) << snr[ni]
                << " EL:" << el[ni]
                << endl;
      }      

      if (isAlm && almOut)
      {
         AlmanacPages& almPages = almPageStore[ni];
         EngAlmanac& engAlm = almStore[ni];
         SubframePage sp(sfid, page);
         almPages[sp] = umsg;

         if (makeEngAlmanac(engAlm, almPages, !minimalAlm))
         {
            out << msgPrefix << "Built complete almanac" << endl;
            if (verboseLevel>2)
               dump(out, almPages);
            if (verboseLevel>1)
               engAlm.dump(out);
            almPages.clear();
            engAlm = EngAlmanac();
         }            
      }
      if (!isAlm && ephOut)
      {
         EphemerisPages& ephPages = ephPageStore[ni];
         ephPages[sfid] = umsg;
         EngEphemeris engEph;
         try
         {
            if (makeEngEphemeris(engEph, ephPages))
            {
               out << msgPrefix << "Built complete ephemeris, iocd:0x"
                   << hex << setw(3) << engEph.getIODC() << dec
                   << endl;
               if (verboseLevel>2)
                  dump(out, ephPages);
               if (verboseLevel>1)
                  out << engEph;
               ephStore[ni] = engEph;
            }
         }
         catch (Exception& e)
         {
            out << e << endl;
         }
      }
   }
   else if (msg.nav == ncICD_200_4)
   {
      // As of October 2009, all but one SV are modulating a constant L2C nav bitstream
      // and the one SV is just sending a type 0 CNAV message.
      if (verboseLevel>2)
         msg.dump(cout);

      int bsfc=0;
      for (int i=1; i<=10; i++)
         if (msg.subframe[i] != 0)
            bsfc++;

      if (bsfc)
      {
         CRC24Q crc;
         for (int i=1; i<=10; i++)
         {
            uint32_t tmp = msg.subframe[i];
            for (int j=0; j<30; j++, tmp<<=1)
               crc.process_bit( tmp & 0x20000000);
         }

         if (crc.checksum() != 0)
         {
            if (verboseLevel)
               out << msgPrefix << "Parity error"
                   << " SNR:" << fixed << setprecision(1) << snr[ni]
                   << " EL:" << el[ni]
                   << endl;
            if (binByElevation)
               peHist[rcp].addValue(el[ni]);
            else
               peHist[rcp].addValue(snr[ni]);
         }
      }
   }
}  // end of process()


void  MDPNavProcessor::process(const MDPObsEpoch& msg)
{
   if (!msg)
      return;

   for (MDPObsEpoch::ObsMap::const_iterator i = msg.obs.begin();
        i != msg.obs.end(); i++)
   {
      const MDPObsEpoch::Observation& obs=i->second;      
      NavIndex ni(RangeCarrierPair(obs.range, obs.carrier), msg.prn);
      snr[ni] = obs.snr;
      el[ni] = msg.elevation;
   }
}
