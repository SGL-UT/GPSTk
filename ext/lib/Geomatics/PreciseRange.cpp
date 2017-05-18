//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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

/// @file PreciseRange.cpp
/// Implement computation of range and associated quantities from XvtStore,
/// given receiver position and time.

// GPSTk includes
#include "MiscMath.hpp"
#include "GPSEllipsoid.hpp"
#include "GNSSconstants.hpp"
#include "Xvt.hpp"
// geomatics
#include "SunEarthSatGeometry.hpp"
#include "SolarPosition.hpp"

#include "PreciseRange.hpp"

using namespace std;

namespace gpstk
{
   double PreciseRange::ComputeAtTransmitTime(const CommonTime& nomRecTime,
                                              const double pr,
                                              const Position& Receiver,
                                              const SatID sat,
                                              const AntexData& antenna,
                                              SolarSystem& SolSys,
                                              const XvtStore<SatID>& Eph,
                                              const bool isCOM)
      throw(Exception)
   {
   try {
      int i;
      Position Rx(Receiver);
      GPSEllipsoid ellips;
      Xvt svPosVel;

      // nominal transmit time
      transmit = nomRecTime;     // receive time on receiver's clock
      transmit -= pr/ellips.c(); // correct for measured time of flight and Rx clock
   
      // get the satellite position at the nominal time, computing and
      // correcting for the satellite clock bias and other delays
      try {
         svPosVel = Eph.getXvt(sat,transmit);
         SatR.setECEF(svPosVel.x[0],svPosVel.x[1],svPosVel.x[2]);
      }
      // this should be a 'no ephemeris' exception
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }

      // update the transmit time for sat clk bias + relativity
      transmit -= svPosVel.clkbias + svPosVel.relcorr;

      // Sagnac effect
      // ref. Ashby and Spilker, GPS: Theory and Application, 1996 Vol 1, pg 673.
      // this is w(Earth) * (SatR cross Rx).Z() / c*c in seconds
      // beware numerical error by differencing very large to get very small
      Sagnac = ( (SatR.X()/ellips.c()) * (Rx.Y()/ellips.c())
               - (SatR.Y()/ellips.c()) * (Rx.X()/ellips.c()) ) * ellips.angVelocity();
      transmit -= Sagnac;

      // compute other delays -- very small
      // 2GM/c^2 = 0.00887005608 m^3/s^2 * s^2/m^2 = m
      double rx = Rx.radius();
      if(::fabs(rx) < 1.e-8) GPSTK_THROW(Exception("Rx at origin!"));
      double rs = SatR.radius();
      double dr = range(SatR,Rx);
      relativity2 = -0.00887005608 * ::log((rx+rs+dr)/(rx+rs-dr));
      transmit -= relativity2 / ellips.c();

      // iterate satellite position
      try {
         svPosVel = Eph.getXvt(sat,transmit);
         // Do NOT replace these with Xvt
         SatR.setECEF(svPosVel.x[0],svPosVel.x[1],svPosVel.x[2]);
         SatV.setECEF(svPosVel.v[0],svPosVel.v[1],svPosVel.v[2]);
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }

      // ----------------------------------------------------------
      // save relativity and satellite clock
      relativity = svPosVel.relcorr * ellips.c();
      satclkbias = svPosVel.clkbias * ellips.c();
      satclkdrift = svPosVel.clkdrift * ellips.c();
   
      // correct for Earth rotation
      double sxyz[3], wt;
      rawrange = range(SatR,Rx);
      wt = ellips.angVelocity() * rawrange/ellips.c();
      sxyz[0] =  ::cos(wt)*SatR.X() + ::sin(wt)*SatR.Y();
      sxyz[1] = -::sin(wt)*SatR.X() + ::cos(wt)*SatR.Y();
      sxyz[2] = SatR.Z();
      SatR.setECEF(sxyz);
      sxyz[0] =  ::cos(wt)*SatV.X() + ::sin(wt)*SatV.Y();
      sxyz[1] = -::sin(wt)*SatV.X() + ::cos(wt)*SatV.Y();
      sxyz[2] = SatV.Z();
      SatV.setECEF(sxyz);
   
      // geometric range, again
      rawrange = range(SatR,Rx);

      // Compute line of sight, satellite to receiver
      Triple S2R(Rx.X()-SatR.X(),Rx.Y()-SatR.Y(),Rx.Z()-SatR.Z());
      S2R = S2R.unitVector();

      // ----------------------------------------------------------
      // satellite antenna pco and pcv
      if(isCOM && antenna.isValid()) {
         static const double fact1GPS=2.5458;         // (alpha+1)/alpha GPS
         static const double fact2GPS=-1.5458;        // -1/alpha
         static const double fact1GLO=2.53125;        // (alpha+1)/alpha GLO
         static const double fact2GLO=-1.53125;       // -1/alpha
         double fact1,fact2;
         string freq1,freq2;

         // rotation matrix from satellite attitude: Rot*[XYZ]=[body frame]
         Matrix<double> SVAtt;

         // get satellite attitude from SolarSystem; if not valid, get low accuracy
         // version from SunEarthSatGeometry and SolarPosition.
         if(SolSys.EphNumber() != -1) {
            SVAtt = SolSys.SatelliteAttitude(transmit, SatR);
         }
         else {
            double AR;     // angular radius of sun
            Position Sun = SolarPosition(transmit, AR);
            SVAtt = SatelliteAttitude(SatR, Sun);
         }

         // get factors and frequencies for system
         if(sat.system == SatID::systemGlonass) {
            fact1=fact1GLO; fact2=fact2GLO;
            freq1="R01"; freq2="R02";
         }
         else {
            fact1=fact1GPS; fact2=fact2GPS;
            freq1="G01"; freq2="G02";
         }

         // phase center offset vector in body frame
         Triple pco1 = antenna.getPhaseCenterOffset(freq1);
         Triple pco2 = antenna.getPhaseCenterOffset(freq2);
         Vector<double> PCO(3);
         for(i=0; i<3; i++)            // body frame, mm -> m, iono-free combo
            PCO(i) = (fact1*pco1[i]+fact2*pco2[i])/1000.0;

         // PCO vector (from COM to PC) in ECEF XYZ frame, m
         SatPCOXYZ = transpose(SVAtt) * PCO;

         Triple pcoxyz(SatPCOXYZ(0),SatPCOXYZ(1),SatPCOXYZ(2));
         // line of sight phase center offset
         satLOSPCO = pcoxyz.dot(S2R);                       // meters

         // phase center variation TD should this should be subtracted from rawrange?
         // get the body frame azimuth and nadir angles
         double nadir,az;
         SatelliteNadirAzimuthAngles(SatR, Rx, SVAtt, nadir, az);
         satLOSPCV = 0.001*(fact1 * antenna.getPhaseCenterVariation(freq1, az, nadir)
                         + fact2 * antenna.getPhaseCenterVariation(freq2, az, nadir));
      }
      else {
         satLOSPCO = satLOSPCV = 0.0;
         SatPCOXYZ=Vector<double>(3,0.0);
      }
   
      // ----------------------------------------------------------
      // direction cosines
      for(i=0; i<3; i++) cosines[i] = -S2R[i];            // receiver to satellite
   
      // elevation and azimuth
      elevation = Rx.elevation(SatR);
      azimuth = Rx.azimuth(SatR);
      elevationGeodetic = Rx.elevationGeodetic(SatR);
      azimuthGeodetic = Rx.azimuthGeodetic(SatR);
   
      // return corrected ephemeris range
      return (rawrange-satclkbias-relativity-relativity2-satLOSPCO+satLOSPCV);

   }  // end try
   catch(gpstk::Exception& e) { GPSTK_RETHROW(e); }

   }  // end PreciseRange::ComputeAtTransmitTime
   
}  // namespace gpstk
