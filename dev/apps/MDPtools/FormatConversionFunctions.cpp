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

#include "FormatConversionFunctions.hpp"

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
         rotm[gpstk::RinexObsHeader::C1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::C1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::L1].data = j->second.phase;
         rotm[gpstk::RinexObsHeader::L1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::L1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::D1].data = j->second.doppler;
         rotm[gpstk::RinexObsHeader::D1].lli = j->second.lockCount ? 1 : 0;
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
         rotm[gpstk::RinexObsHeader::P1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::P1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::L1].data = j->second.phase;
         rotm[gpstk::RinexObsHeader::L1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::L1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::D1].data = j->second.doppler;
         rotm[gpstk::RinexObsHeader::D1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::D1].ssi = snr2ssi(j->second.snr);

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
         rotm[gpstk::RinexObsHeader::P2].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::P2].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::L2].data = j->second.phase;
         rotm[gpstk::RinexObsHeader::L2].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::L2].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::D2].data = j->second.doppler;
         rotm[gpstk::RinexObsHeader::D2].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::D2].ssi = snr2ssi(j->second.snr);

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
         rotm[gpstk::RinexObsHeader::C2].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::C2].ssi = snr2ssi(j->second.snr);
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

   SvObsEpoch makeSvObsEpoch(const MDPObsEpoch& mdp) throw()
   {
      SvObsEpoch obs;
      MDPObsEpoch::ObsMap::const_iterator i;
      for (i=mdp.obs.begin(); i!=mdp.obs.end(); i++)
      {
         CarrierCode cc = i->first.first;
         RangeCode rc = i->first.second;
         const MDPObsEpoch::Observation& mdp_obs = i->second;

         ObsID::CarrierBand cb;
         ObsID::TrackingCode tc;

         switch(cc)
         {
            case ccL1: cb = ObsID::cbL1; break;
            case ccL2: cb = ObsID::cbL2; break;
            case ccL5: cb = ObsID::cbL5; break;
            default:   cb = ObsID::cbUnknown;
         }

         switch (rc)
         {
            case rcCA:       tc = ObsID::tcCA; break;
            case rcPcode:    tc = ObsID::tcP; break;
            case rcYcode:    tc = ObsID::tcY; break;
            case rcCodeless: tc = ObsID::tcW; break;
            case rcCM:       tc = ObsID::tcCA; break;
            case rcCL:       tc = ObsID::tcCA; break;
            case rcMcode1:   tc = ObsID::tcCA; break;
            case rcMcode2:   tc = ObsID::tcCA; break;
            case rcCMCL:     tc = ObsID::tcCA; break;
            default:         tc = ObsID::tcUnknown;
         }

         obs[ObsID(ObsID::otRange,   cb, tc)] = mdp_obs.pseudorange;
         obs[ObsID(ObsID::otPhase,   cb, tc)] = mdp_obs.phase;
         obs[ObsID(ObsID::otDoppler, cb, tc)] = mdp_obs.doppler;
         obs[ObsID(ObsID::otSNR,     cb, tc)] = mdp_obs.snr;
         obs[ObsID(ObsID::otLLI,     cb, tc)] = mdp_obs.lockCount;
      }
      return obs;
   }

   SvObsEpoch makeSvObsEpoch(const RinexObsData::RinexObsTypeMap& rotm) throw()
   {
      SvObsEpoch soe;

      RinexObsData::RinexObsTypeMap::const_iterator rotm_itr;
      for (rotm_itr = rotm.begin(); rotm_itr != rotm.end(); rotm_itr++)
      {
         const RinexObsHeader::RinexObsType& rot = rotm_itr->first;
         const RinexObsData::RinexDatum& rd = rotm_itr->second;
         RinexObsID oid(rot);
         soe[oid] = rd.data;
         if (rd.ssi>0)
         {
            oid.type = ObsID::otSSI;
            soe[oid] = rd.ssi;
         }
      }

      return soe;
   }

   ObsEpoch makeObsEpoch(const RinexObsData& rod) throw()
   {
      ObsEpoch oe;
      oe.time = rod.time;

      RinexObsData::RinexSatMap::const_iterator rsm_itr;
      for (rsm_itr = rod.obs.begin(); rsm_itr != rod.obs.end(); rsm_itr++)
      {
         const SatID svid(rsm_itr->first);
         const RinexObsData::RinexObsTypeMap& rotm = rsm_itr->second;
         oe[svid] = makeSvObsEpoch(rotm);
      }

      return oe;
   }


   ObsEpoch makeObsEpoch(const MDPEpoch& mdp) throw()
   {
      ObsEpoch oe;
      oe.time = mdp.begin()->second.time;

      for (MDPEpoch::const_iterator i=mdp.begin(); i!=mdp.end(); i++)
      {
         const MDPObsEpoch& moe = i->second;
         gpstk::SatID svid(moe.prn, gpstk::SatID::systemGPS);
         oe[svid] = makeSvObsEpoch(moe);
      }
      return oe;
   }

   // Try to convert the given pages into an EngAlmanc object. Returns true
   // upon success. This algorithm will only work for a receiver that outputs
   // all 4/5 subframes from a code/carrier. Basically it looks for a 12.5 minute
   // cycle that starts with page 1 from subframe 4.
   // It makes sure that there hasn't been a cutover during it by checking that
   // all sv pages (i.e. svid 1-32) have the same toa as the last page 25
   // (svid 51).
   // In the IS-GPS-200D, see pages 72-79, 82, 105
   bool makeEngAlmanac(EngAlmanac& alm, const AlmanacPages& pages)
   {
      if (pages.size()!=50)
         return false;

      AlmanacPages::const_iterator i, firstPage, lastPage;
      firstPage = pages.find(SubframePage(4, 1));
      lastPage = pages.find(SubframePage(5,25));
      long dt = lastPage->second.getHOWTime() - firstPage->second.getHOWTime();

      // 25 pairs of pages every 30 seconds = 750 seconds minus the 24 seconds
      // that the three ephemeris pages take gives us 726 seconds
      if (dt != 726)
         return false;

      int week = firstPage->second.time.GPSfullweek();
      long sfa[10];
      long long_sfa[10];
      for (i = pages.begin(); i != pages.end(); i++)
      {
         i->second.fillArray(sfa);
         copy( &sfa[0], &sfa[10], long_sfa);
         if (!alm.addSubframe(long_sfa, week))
            return false;
      }
      cout << endl;
      
      double p51Toa=alm.getToa();
      double svToa;
      for (int prn=1; prn<=32; prn++)
      {
         try {
            svToa = alm.getToa(gpstk::SatID(prn, SatID::systemGPS));
            if (svToa != p51Toa)
               return false;
         }
         catch (EngAlmanac::SVNotPresentException& e)
         {}
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


   WxObservation makeWxObs(const SMODFData& smod) throw()
   {
      WxObservation wx;

      wx.t = smod.time;

      if (smod.tempSource)
      {
         wx.temperature = smod.temp;
         wx.temperatureSource = WxObservation::obsWx;
      }
      else 
         wx.temperatureSource = WxObservation::noWx;;

      if (smod.pressSource)
      {
         wx.pressure = smod.pressure;
         wx.pressureSource = WxObservation::obsWx;
      }
      else 
         wx.pressureSource = WxObservation::noWx;;

      if (smod.humidSource)
      {
         wx.humidity = smod.humidity;
         wx.humiditySource = WxObservation::obsWx;
      }
      else 
         wx.humiditySource = WxObservation::noWx;
         
      return wx;
   }

   ObsID getObsID(const SMODFData& smod) throw()
   {
      if (smod.type==0)
         return ObsID(ObsID::otRange, ObsID::cbL1L2, ObsID::tcUnknown);
      else if (smod.type==9)
         return ObsID(ObsID::otPhase, ObsID::cbL1L2, ObsID::tcUnknown);

      return ObsID(ObsID::otUnknown, ObsID::cbUnknown, ObsID::tcUnknown);
   }
      
} // end of namespace gpstk
