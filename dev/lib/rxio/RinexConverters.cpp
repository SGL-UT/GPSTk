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

/** @file Translates between various similiar objects */

#include "StringUtils.hpp"
#include "RinexObsID.hpp"

#include "RinexConverters.hpp"

using namespace std;

namespace gpstk
{
   short snr2ssi(float x)
   {
      // These values were obtained from the comments in a RINEX obs file that was
      // generated from a TurboBinary file recorded on an AOA Benchmark  receiver
      if (x>316) return 9;
      if (x>100) return 8;
      if (x>31.6) return 7;
      if (x>10) return 6;
      if (x>3.2) return 5;
      if (x>0) return 4;
      return 0;
   }

   RinexObsData::RinexObsTypeMap makeRinexObsTypeMap(const MDPObsEpoch& moe) throw()
   {
      gpstk::RinexObsData::RinexObsTypeMap rotm;
      MDPObsEpoch::ObsMap ol=moe.obs;
      MDPObsEpoch::ObsMap::const_iterator j;

      // The C1 Rinex obs is easy
      j = ol.find(MDPObsEpoch::ObsKey(ccL1,rcCA));
      if (j!=ol.end())
      {
         rotm[gpstk::RinexObsHeader::C1].data = j->second.pseudorange;
         rotm[gpstk::RinexObsHeader::C1].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::C1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::L1].data = j->second.phase;
         rotm[gpstk::RinexObsHeader::L1].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::L1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::D1].data = j->second.doppler;
         rotm[gpstk::RinexObsHeader::D1].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::D1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::S1].data = j->second.snr;
      }

      // Now get the P1, L1, D1, S1 obs
      j = ol.find(MDPObsEpoch::ObsKey(ccL1, rcYcode));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL1, rcPcode));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL1, rcCodeless));
      if (j != ol.end())
      {
         rotm[gpstk::RinexObsHeader::P1].data = j->second.pseudorange;
         rotm[gpstk::RinexObsHeader::P1].lli  = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::P1].ssi  = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::L1].data = j->second.phase;
         rotm[gpstk::RinexObsHeader::L1].lli  = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::L1].ssi  = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::D1].data = j->second.doppler;
         rotm[gpstk::RinexObsHeader::D1].lli  = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::D1].ssi  = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::S1].data = j->second.snr;
      }
      
      // Now get the P2, L2, D2, S2 obs
      j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcYcode));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcPcode));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcCodeless));
      if (j != ol.end())
      {
         rotm[gpstk::RinexObsHeader::P2].data = j->second.pseudorange;
         rotm[gpstk::RinexObsHeader::P2].lli  = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::P2].ssi  = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::L2].data = j->second.phase;
         rotm[gpstk::RinexObsHeader::L2].lli  = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::L2].ssi  = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::D2].data = j->second.doppler;
         rotm[gpstk::RinexObsHeader::D2].lli  = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::D2].ssi  = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::S2].data = j->second.snr;
      }

      // Now get the C2
      j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcCM));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcCL));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcCMCL));
      if (j != ol.end())
      {
         rotm[gpstk::RinexObsHeader::C2].data = j->second.pseudorange;
         rotm[gpstk::RinexObsHeader::C2].lli  = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::C2].ssi  = snr2ssi(j->second.snr);
      }
      return rotm;
   }

   RinexObsData makeRinexObsData(const gpstk::MDPEpoch& mdp)
   {
      RinexObsData rod;

      rod.clockOffset=0;
      rod.numSvs = mdp.size();
      rod.epochFlag = 0;
      rod.time = mdp.begin()->second.time;

      for (MDPEpoch::const_iterator i=mdp.begin(); i!=mdp.end(); i++)
      {
         const MDPObsEpoch& moe = i->second;
         gpstk::SatID svid(moe.prn, gpstk::SatID::systemGPS);
         rod.obs[svid] = makeRinexObsTypeMap(moe);
      }
      return rod;
   }

   // Try to convert the given pages into an EngAlmanc object. Returns true
   // upon success. This routine is tuned for two different types of nav data.
   //
   // The first is for a receiver that outputs all 4/5 subframes from a given
   // code/carrier. Basically it looks for a 12.5 minute cycle that starts
   // with page 1 from subframe 4. It makes sure that there hasn't been a
   // cutover during it by checking that all sv pages (i.e. svid 1-32) have
   // the same toa as the last page 25 (svid 51). This mode is the default and
   // is set with the requireFull parameter.
   //
   // The second is for a receiver that only puts out a set of 4/5 subframes
   // that "should" be a complete almanac. Note that it doesn't output pages
   // for SVs that are set to the default data.
   //
   // The only receiver that this has been tested on is the Ashtech Z(Y)12.
   // 
   // In the IS-GPS-200D, see pages 72-79, 82, 105
   bool makeEngAlmanac(EngAlmanac& alm,
                       const AlmanacPages& pages,
                       bool requireFull) throw()
   {
      AlmanacPages::const_iterator sf4p1  = pages.find(SubframePage(4,  1));
      AlmanacPages::const_iterator sf4p18 = pages.find(SubframePage(4, 18));
      AlmanacPages::const_iterator sf4p25 = pages.find(SubframePage(4, 25));
      AlmanacPages::const_iterator sf5p25 = pages.find(SubframePage(5, 25));

      // These pages are required for a reasonable alm
      if (sf4p18==pages.end() || sf4p25==pages.end() || sf5p25==pages.end())
         return false;

      long sf4p1sow=0;
      if (requireFull)
      {
         if (sf4p1==pages.end())
            return false;
         else
            sf4p1sow = sf4p1->second.getHOWTime();
      }

      int week=sf4p18->second.time.GPSfullweek();
      
      for (int p=1; p<=25; p++)
      {
         for (int sf=4; sf<=5; sf++)
         {
            AlmanacPages::const_iterator i = pages.find(SubframePage(sf, p));
            if (i == pages.end())
            {
               if (requireFull)
                  return false;
               else
                  continue;
            }

            // All pages have to be contingious for the full alm mode.
            if (requireFull)
            {
               long sow = i->second.getHOWTime(); 
               if (sow != sf4p1sow + (sf-4)*6 + (p-1)*30)
                  return false;
            }

            long sfa[10];
            long long_sfa[10];
            i->second.fillArray(sfa);
            copy( &sfa[0], &sfa[10], long_sfa);
            if (!alm.addSubframe(long_sfa, week))
               return false;
         }
      }
      return true;
   }

   // Try to convert the given pages into an EngEphemeris object. Returns true
   // upon success.
   bool makeEngEphemeris(EngEphemeris& eph, const EphemerisPages& pages)
   {
      EphemerisPages::const_iterator sf[4];

      sf[1] = pages.find(1);
      if (sf[1] == pages.end())
         return false;
      
      sf[2] = pages.find(2);
      if (sf[2] == pages.end())
         return false;

      sf[3] = pages.find(3);
      if (sf[3] == pages.end())
         return false;

      long t1 = sf[1]->second.getHOWTime();
      long t2 = sf[2]->second.getHOWTime();
      long t3 = sf[3]->second.getHOWTime();
      if (t2 != t1+6 || t3 != t1+12)
         return false;

      int prn = sf[1]->second.prn;
      int week = sf[1]->second.time.GPSfullweek();
      long sfa[10];
      long long_sfa[10];

      for (int i=1; i<=3; i++)
      {
         sf[i]->second.fillArray(sfa);
         for( int j = 0; j < 10; j++ )
            long_sfa[j] = static_cast<long>( sfa[j] );
         if (!eph.addSubframe(long_sfa, week, prn, 0))
            return false;
      }

      if (eph.isData(1) && eph.isData(2) && eph.isData(3))
         return true;

      return false;
   }
} // end of namespace gpstk
