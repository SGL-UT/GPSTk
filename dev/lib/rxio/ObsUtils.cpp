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
#include "icd_200_constants.hpp"

#include "ObsUtils.hpp"

using namespace std;

namespace gpstk
{
   //---------------------------------------------------------------------------
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
            case rcCM:       tc = ObsID::tcC2M; break;
            case rcCL:       tc = ObsID::tcC2L; break;
            case rcMcode1:   tc = ObsID::tcM; break;
            case rcMcode2:   tc = ObsID::tcM; break;
            case rcCMCL:     tc = ObsID::tcC2LM; break;
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


   //---------------------------------------------------------------------------
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
         if (rd.lli>0)
         {
            oid.type = ObsID::otLLI;
            soe[oid] = -1;
         }
      }

      return soe;
   }


   //---------------------------------------------------------------------------
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


   //---------------------------------------------------------------------------
   ObsEpoch makeObsEpoch(const MDPEpoch& mdp) throw()
   {
      ObsEpoch oe;
      oe.time = mdp.begin()->second.time;

      for (MDPEpoch::const_iterator i=mdp.begin(); i!=mdp.end(); i++)
      {
         const MDPObsEpoch& moe = i->second;
         SatID svid(moe.prn, SatID::systemGPS);
         oe[svid] = makeSvObsEpoch(moe);
      }
      return oe;
   }


   //---------------------------------------------------------------------------
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

   //---------------------------------------------------------------------------
   void addMDPObservation(MDPObsEpoch& moe,
                          const AshtechMBEN::code_block& cb,
                          CarrierCode cc,
                          RangeCode rc,
                          const MDPObsEpoch& moe_hint) throw()
   {
      // fixup the range code to match what is indicated by the goodbad flag
      if (rc != rcCA)
         switch (cb.goodbad)
         {
            case 22: rc = rcPcode; break;
            case 24: rc = rcYcode; break;
            case 25: rc = rcCodeless; break;
         }

      float chipRate=PY_CHIP_FREQ;
      if (rc == rcCA)
         chipRate = CA_CHIP_FREQ;

      MDPObsEpoch::Observation obs;
      obs.carrier = cc;
      obs.range = rc;
      obs.snr = cb.snr(chipRate);
      obs.pseudorange = cb.raw_range * C_GPS_M;
      obs.phase = cb.full_phase;
      obs.doppler = -cb.doppler; // yea, the Ashtech sign is backwards

      if (moe_hint.haveObservation(cc, rc))
      {
         MDPObsEpoch::Observation obs_hint = moe_hint.getObservation(cc, rc);
         obs.bw = obs_hint.bw;
         obs.lockCount = obs_hint.lockCount++;
      }
      else
      {
         obs.bw=1;
         obs.lockCount = 0;
      }

      moe.obs[MDPObsEpoch::ObsKey(cc, rc)] = obs;   
   }


   //---------------------------------------------------------------------------
   MDPObsEpoch makeMDPObsEpoch(
      const AshtechMBEN& mben,
      const MDPObsEpoch& hint) throw()
   {
      MDPObsEpoch moe;

      // Get the full time from the hint and make the sow match the MBEN
      moe.time = hint.time;
      double sow1 = moe.time.GPSsecond();
      int sow2 = static_cast<int>(sow1/1800);
      double sow3 = static_cast<double>(sow2 * 1800);
      double sow_mben = 50e-3 * mben.seq;
      double sow4 = sow3 + sow_mben;
      moe.time.setGPS(moe.time.GPSfullweek(), sow4);

      moe.numSVs = mben.left;
      moe.channel = mben.chid;
      moe.prn = mben.svprn;
      moe.status = hint.status;
      moe.elevation = mben.el;
      moe.azimuth = mben.az;

      addMDPObservation(moe, mben.ca, ccL1, rcCA, hint);

      if (mben.id == AshtechMBEN::mpcId)
      {
         addMDPObservation(moe, mben.p1, ccL1, rcPcode, hint);
         addMDPObservation(moe, mben.p2, ccL2, rcPcode, hint);
      }
      return moe;
   }


   //---------------------------------------------------------------------------
   MDPPVTSolution makeMDPPVTSolution(
      const AshtechPBEN& pben,
      const unsigned week) throw()
   {
      MDPPVTSolution pvt;

      pvt.x[0] = pben.navx;
      pvt.x[1] = pben.navy;
      pvt.x[3] = pben.navz;
      pvt.dtime = pben.navt / C_GPS_M;
      pvt.v[0] = pben.navxdot;
      pvt.v[1] = pben.navydot;
      pvt.v[2] = pben.navzdot;
      pvt.ddtime = pben.navtdot / C_GPS_M;
      
      pvt.time.setGPS(week, pben.sow);
      pvt.timep = pvt.time + pvt.dtime;

      pvt.fom = pben.pdop;
      pvt.numSVs = (int)pben.numSV;
      pvt.pvtMode = 0;
      pvt.corrections = 0;
      
      return pvt;
   }

}
