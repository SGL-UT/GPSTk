#pragma ident "$Id: //depot/sgl/gpstk/dev/src/ObsRngDev.cpp#7 $"

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

#include "EphemerisRange.hpp"
#include "EngEphemeris.hpp"
#include "BCEphemerisStore.hpp"
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
      double prange,
      short prn,
      const DayTime& time,
      const ECEF& rxpos,
      const EphemerisStore& eph,
      GeoidModel& gm,
      bool svTime)
         : obstime(time), prn(prn), ord(0)
   {
      computeOrd(prange, rxpos, eph, gm, svTime);
      Geodetic gx(rxpos, &gm);
      NBTropModel nb(gx.getAltitude(), gx.getLatitude(), time.DOYday());
      computeTrop(nb);
   }

   ObsRngDev::ObsRngDev(
      double prange,
      short prn,
      const DayTime& time,
      const ECEF& rxpos,
      const EphemerisStore& eph,
      GeoidModel& gm,
      const IonoModelStore& ion,
      IonoModel::Frequency fq,
      bool svTime)
         : obstime(time), prn(prn), ord(0)
   {
      computeOrd(prange, rxpos, eph, gm, svTime);
      Geodetic gx(rxpos, &gm);
      NBTropModel nb(gx.getAltitude(), gx.getLatitude(), time.DOYday());
      computeTrop(nb);
      iono = ion.getCorrection(time, gx, elevation, azimuth, fq);
      ord -= iono;
   }

   ObsRngDev::ObsRngDev(
      double prange,
      short prn,
      const DayTime& time,
      const ECEF& rxpos,
      const EphemerisStore& eph,
      GeoidModel& gm,
      const TropModel& tm,
      bool svTime)
         : obstime(time), prn(prn), ord(0)
   {
      computeOrd(prange, rxpos, eph, gm, svTime);
      computeTrop(tm);
   }

   ObsRngDev::ObsRngDev(
      double prange,
      short prn,
      const DayTime& time,
      const ECEF& rxpos,
      const EphemerisStore& eph,
      GeoidModel& gm,
      const TropModel& tm,
      const IonoModelStore& ion,
      IonoModel::Frequency fq,
      bool svTime)
         : obstime(time), prn(prn), ord(0)
   {
      computeOrd(prange, rxpos, eph, gm, svTime);
      computeTrop(tm);
      Geodetic gx(rxpos, &gm);
      iono = ion.getCorrection(time, gx, elevation, azimuth, fq);
      ord -= iono;
   }


   ObsRngDev::ObsRngDev(
      double prange1, double prange2,
      short prn,
      const DayTime& time,
      const ECEF& rxpos,
      const EphemerisStore& eph,
      GeoidModel& gm,
      bool svTime)
         : obstime(time), prn(prn), ord(0)
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
      double prange1, double prange2,
      short prn,
      const DayTime& time,
      const ECEF& rxpos,
      const EphemerisStore& eph,
      const GeoidModel& gm,
      const TropModel& tm,
      bool svTime)
         : obstime(time), prn(prn), ord(0)
   {
      // for dual frequency see ICD-GPS-211, section 20.3.3.3.3.3
      double icpr = (prange2 - GAMMA * prange1)/IGAMMA;
      iono = prange1 - icpr;

      computeOrd(icpr, rxpos, eph, gm, svTime);
      computeTrop(tm);
   }

   // This should be used for obs in GPS (i.e. receiver) time.
   void ObsRngDev::computeOrdRx(
      double obs,
      const ECEF& rxpos,
      const EphemerisStore& eph,
      const GeoidModel& gm)
   {
      // This algorithm should really be moved to Xvt.preciseRho() or somewhere
      Xvt svpos;
      double rawrange;
      double tof = 0.070;
      double tof_old = 0;
      DayTime ttime;

      for (int i=0; fabs(tof-tof_old) > 1.0e-13 && i<=5; i++)
      {
         ttime = obstime - tof;
         tof_old = tof;

         svpos = eph.getPrnXvt(prn, ttime);

         rawrange = rxpos.slantRange(svpos.x);
         tof = rawrange/gm.c();

         // Perform earth rotation correction
         double wt = gm.angVelocity()*tof;
         Triple tmp;
         tmp[0] =     svpos.x[0] + wt*svpos.x[1];
         tmp[1] = -wt*svpos.x[0] +    svpos.x[1];
         tmp[2] = svpos.x[2];

         rawrange = rxpos.slantRange(tmp);
         tof = rawrange/gm.c();
      }

      double svclkbias = svpos.dtime*gm.c();
      if (typeid(eph) == typeid(BCEphemerisStore))
      {
         const BCEphemerisStore& bce = dynamic_cast<const BCEphemerisStore&>(eph);
         const EngEphemeris& eph = bce.findEphemeris(prn,obstime);
         iodc = eph.getIODC();
         health = eph.getHealth();
      }

      // Now add in the corrections
      rho = rawrange - svclkbias;

      // compute the azimuth and elevation of the SV
      azimuth = rxpos.azAngle(svpos.x);
      elevation = rxpos.elvAngle(svpos.x);

      // and now calculate the ORD
      ord = obs - rho;

      if (debug)
      {
         std::ios::fmtflags oldFlags = std::cout.flags();
         std::cout << *this << std::endl
                   << std::setprecision(3) << std::fixed
                   << "  obs=" << obs
                   << ", rho=" << (double)rho
                   << ", obs-rho=" << (double)ord
                   << std::setprecision(9)
                   << ", tof=" << tof
                   << std::endl 
                   << std::setprecision(3)
                   << "  sv.x=" << svpos.x
                   << ", sv.v=" << svpos.v
                   << std::endl
                   << "  rx.x=" << rxpos
                   << std::setprecision(4) << std::scientific
                   << ", sv bias=" << svpos.dtime
                   << ", sv drift=" << svpos.ddtime
                   << std::endl;
         std::cout.flags(oldFlags);
      }
   }


// This should be used for obs in SV time
   void ObsRngDev::computeOrdTx(
      double obs,
      const ECEF& rxpos,
      const EphemerisStore& eph,
      const GeoidModel& gm)
   {
      Xvt svpos = eph.getPrnXvt(prn, obstime);

      // compute the range from the station to the SV
      rho = svpos.preciseRho(rxpos, gm);

      // and now calculate the ORD
      ord = obs - rho;

      if (typeid(eph) == typeid(BCEphemerisStore))
      {
         const BCEphemerisStore& bce = dynamic_cast<const BCEphemerisStore&>(eph);
         const EngEphemeris& eph = bce.findEphemeris(prn,obstime);
         iodc = eph.getIODC();
         health = eph.getHealth();
      }

      // compute the azimuth and elevation of the SV
      azimuth = rxpos.azAngle(svpos.x);
      elevation = rxpos.elvAngle(svpos.x);

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
                   << "  sv.x=" << svpos.x
                   << ", sv.v=" << svpos.v
                   << std::endl
                   << "  rx.x=" << rxpos
                   << std::setprecision(4) << std::scientific
                   << ", sv bias=" << svpos.dtime
                   << ", sv drift=" << svpos.ddtime
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
        << " prn=" << std::setw(2) << ord.prn
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
        << " iodc=" << ord.iodc;
      s.flags(oldFlags);
      return s;
   }
}
