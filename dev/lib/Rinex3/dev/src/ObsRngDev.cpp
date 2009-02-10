#pragma ident "$Id: ObsRngDev.cpp 1461 2008-11-18 19:46:55Z ocibu $"

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

/**
 * @file ObsRngDev.hpp
 * Observed range deviation computation & storage.
 */

#include <typeinfo>

#include "EphemerisRange.hpp"
#include "EngEphemeris.hpp"
#include "GPSEphemerisStore.hpp"
#include "MiscMath.hpp"
#include "icd_200_constants.hpp"

#include "ObsRngDev.hpp"

namespace gpstk
{

   // these are the L1-L2 correction factors per ICD-GPS-200, sec. 20.3.3.3.3.2
   static const double GAMMA = 1.64694444444444444; // (1575.42/1227.6)^2
   static const double IGAMMA = 1-GAMMA;

   bool ObsRngDev::debug=false;

   ObsRngDev::ObsRngDev(
      const double prange,
      const SatID& svid,
      const DayTime& time,
      const ECEF& rxpos,
      const XvtStore<SatID>& eph,
      GeoidModel& gm,
      bool svTime)
      : obstime(time), svid(svid), ord(0), wonky(false)
   {
      computeOrd(prange, rxpos, eph, gm, svTime);
      Geodetic gx(rxpos, &gm);
      NBTropModel nb(gx.getAltitude(), gx.getLatitude(), time.DOYday());
      computeTrop(nb);
   }

   ObsRngDev::ObsRngDev(
      const double prange,
      const SatID& svid,
      const DayTime& time,
      const ECEF& rxpos,
      const XvtStore<SatID>& eph,
      GeoidModel& gm,
      const IonoModelStore& ion,
      IonoModel::Frequency fq,
      bool svTime)
         : obstime(time), svid(svid), ord(0), wonky(false)
   {
      computeOrd(prange, rxpos, eph, gm, svTime);
      Geodetic gx(rxpos, &gm);
      NBTropModel nb(gx.getAltitude(), gx.getLatitude(), time.DOYday());
      computeTrop(nb);
      iono = ion.getCorrection(time, gx, elevation, azimuth, fq);
      ord -= iono;
   }

   ObsRngDev::ObsRngDev(
      const double prange,
      const SatID& svid,
      const DayTime& time,
      const ECEF& rxpos,
      const XvtStore<SatID>& eph,
      GeoidModel& gm,
      const TropModel& tm,
      bool svTime)
         : obstime(time), svid(svid), ord(0), wonky(false)
   {
      computeOrd(prange, rxpos, eph, gm, svTime);
      computeTrop(tm);
   }

   ObsRngDev::ObsRngDev(
      const double prange,
      const SatID& svid,
      const DayTime& time,
      const ECEF& rxpos,
      const XvtStore<SatID>& eph,
      GeoidModel& gm,
      const TropModel& tm,
      const IonoModelStore& ion,
      IonoModel::Frequency fq,
      bool svTime)
         : obstime(time), svid(svid), ord(0), wonky(false)
   {
      computeOrd(prange, rxpos, eph, gm, svTime);
      computeTrop(tm);
      Geodetic gx(rxpos, &gm);
      iono = ion.getCorrection(time, gx, elevation, azimuth, fq);
      ord -= iono;
   }


   ObsRngDev::ObsRngDev(
      const double prange1,
      const double prange2,
      const SatID& svid,
      const DayTime& time,
      const ECEF& rxpos,
      const XvtStore<SatID>& eph,
      GeoidModel& gm,
      bool svTime)
         : obstime(time), svid(svid), ord(0), wonky(false)
   {
      // for dual frequency see ICD-GPS-211, section 20.3.3.3.3.3
      double icpr = (prange2 - GAMMA * prange1)/IGAMMA;
      iono = prange1 - icpr;

      computeOrd(icpr, rxpos, eph, gm, svTime);
      Geodetic gx(rxpos, &gm);
      NBTropModel nb(gx.getAltitude(), gx.getLatitude(), time.DOYday());
      computeTrop(nb);
   }


   ObsRngDev::ObsRngDev(
      const  double prange1,
      const double prange2,
      const SatID& svid,
      const DayTime& time,
      const ECEF& rxpos,
      const XvtStore<SatID>& eph,
      const GeoidModel& gm,
      const TropModel& tm,
      bool svTime)
         : obstime(time), svid(svid), ord(0), wonky(false)
   {
      // for dual frequency see ICD-GPS-211, section 20.3.3.3.3.3
      double icpr = (prange2 - GAMMA * prange1)/IGAMMA;
      iono = prange1 - icpr;

      computeOrd(icpr, rxpos, eph, gm, svTime);
      computeTrop(tm);
   }

   // This should be used for obs in GPS (i.e. receiver) time.
   void ObsRngDev::computeOrdRx(
      const double obs,
      const ECEF& rxpos,
      const XvtStore<SatID>& eph,
      const GeoidModel& gm)
   {
      CorrectedEphemerisRange cer;
      rho = cer.ComputeAtTransmitTime(obstime, obs, rxpos, svid, eph);
      azimuth = cer.azimuth;
      elevation = cer.elevation;
      ord = obs - rho;

      if (typeid(eph) == typeid(GPSEphemerisStore))
      {
         const GPSEphemerisStore& bce = dynamic_cast<const GPSEphemerisStore&>(eph);
         const EngEphemeris& eph = bce.findEphemeris(svid, obstime);
         iodc = eph.getIODC();
         health = eph.getHealth();
      }

      if (debug)
      {
         std::ios::fmtflags oldFlags = std::cout.flags();
         std::cout << *this << std::endl
                   << std::setprecision(3) << std::fixed
                   << "  obs=" << obs
                   << ", rho=" << (double)rho
                   << ", obs-rho=" << (double)ord
                   << std::endl
                   << "  rx.x=" << rxpos
                   << std::setprecision(4) << std::scientific
                   << ", sv bias=" << cer.svclkbias
                   << ", sv drift=" << cer.svclkdrift
                   << std::endl;
         std::cout.flags(oldFlags);
      }
   }


// This should be used for obs tagged in SV time
   void ObsRngDev::computeOrdTx(
      double obs,
      const ECEF& rxpos,
      const XvtStore<SatID>& eph,
      const GeoidModel& gm)
   {
      CorrectedEphemerisRange cer;
      rho = cer.ComputeAtTransmitSvTime(obstime, obs, rxpos, svid, eph);
      azimuth = cer.azimuth;
      elevation = cer.elevation;
      ord = obs - rho;
      if (debug)
      {
         std::ios::fmtflags oldFlags = std::cout.flags();
         std::cout << *this << std::endl
                   << std::setprecision(3) << std::fixed
                   << "  obs=" << obs
                   << ", rho=" << (double)rho
                   << ", obs-rho=" << (double)ord
                   << std::endl
                   << std::setprecision(3)
                   << "  sv.x=" << cer.svPosVel.x
                   << ", sv.v=" << cer.svPosVel.v
                   << std::endl
                   << "  rx.x=" << rxpos
                   << std::setprecision(4) << std::scientific
                   << ", sv bias=" << cer.svPosVel.dtime
                   << ", sv drift=" << cer.svPosVel.ddtime
                   << std::endl;
         std::cout.flags(oldFlags);
      }
   }
  
   void ObsRngDev::computeTrop(const TropModel& tm)
   {
      trop = tm.correction(elevation);
      ord -= trop;
   }

   std::ostream& operator<<(std::ostream& s, const ObsRngDev& ord)
      throw()
   {
      std::ios::fmtflags oldFlags = s.flags();
      s << "t=" << ord.obstime.printf("%Y/%03j %02H:%02M:%04.1f")
        << " prn=" << std::setw(2) << ord.svid.id
        << std::setprecision(4)
        << " az=" << std::setw(3) << ord.azimuth
        << " el=" << std::setw(2) << ord.elevation
        << std::hex
        << " h=" << std::setw(1) << ord.health
        << std::dec << std::setprecision(4)
        << " ord=" << ord.ord
        << " ion=" << ord.iono
        << " trop=" << ord.trop
        << std::hex
        << " iodc=" << ord.iodc
        << " wonky=" << ord.wonky;
      s.flags(oldFlags);
      return s;
   }
}
