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

/**
 * @file PreciseRange.cpp
 * Implement computation of range and associated quantities from XvtStore,
 * given receiver position and time.
 */

// GPSTk includes
#include "MiscMath.hpp"
#include "GPSEllipsoid.hpp"
#include "GNSSconstants.hpp"
#include "GNSSconstants.hpp"
#include "Xvt.hpp"
// geomatics
#include "SunEarthSatGeometry.hpp"
#include "PreciseRange.hpp"

using namespace std;

namespace gpstk
{
   double PreciseRange::ComputeAtTransmitTime(const CommonTime& nomRecTime,
                                              const double pr,
                                              const Position& Receiver,
                                              const SatID sat,
                                              const AntexData& antenna,
                                              const SolarSystem& SSEph,
                                              const EarthOrientation& EO,
                                              const XvtStore<SatID>& Eph,
                                              const bool isCOM)
      throw(Exception)
   {
   try {
      int i;
      Position Rx(Receiver);
      GPSEllipsoid ell;
      Xvt svPosVel;

      // nominal transmit time
      transmit = nomRecTime;     // receive time on receiver's clock
      transmit -= pr/ell.c();  // correct for measured time of flight and Rx clock

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
      Sagnac = ( (SatR.X()/ell.c()) * (Rx.Y()/ell.c())
               - (SatR.Y()/ell.c()) * (Rx.X()/ell.c()) ) * ell.angVelocity();
      transmit -= Sagnac;

      // compute other delays -- very small
      // 2GM/c^2 = 0.00887005608 m^3/s^2 * s^2/m^2 = m
      double rx = Rx.radius();
      double rs = SatR.radius();
      double dr = range(SatR,Rx);
      relativity2 = -0.00887005608 * ::log((rx+rs+dr)/(rx+rs-dr));
      transmit -= relativity2 / ell.c();

      // iterate satellite position
      try {
         svPosVel = Eph.getXvt(sat,transmit);
         // Do NOT replace these with Xvt
         SatR.setECEF(svPosVel.x[0],svPosVel.x[1],svPosVel.x[2]);
         SatV.setECEF(svPosVel.v[0],svPosVel.v[1],svPosVel.v[2]);
      }
      catch(InvalidRequest& e) { GPSTK_RETHROW(e); }

      // ----------------------------------------------------------
      //// compute relativity separate from satellite clock
      //relativity = RelativityCorrection(SatR,SatV) * ell.c();

      // save relativity (m), clock bias (m) and drift (m/s)
      relativity = svPosVel.relcorr * ell.c();
      satclkbias = svPosVel.clkbias * ell.c();
      satclkdrift = svPosVel.clkdrift * ell.c();

      // correct for Earth rotation
      double sxyz[3], wt;
      rawrange = range(SatR,Rx);
      wt = ell.angVelocity() * rawrange/ell.c();
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
         double sf;
         // rotation matrix from satellite attitude: Rot*[XYZ]=[body frame]
         Matrix<double> Rotation;
         if(SSEph.JPLNumber() > -1) {
            // use full JPL ephemeris
            Rotation = SatelliteAttitude(transmit, SatR, SSEph, EO, sf);
         }
         else {
            // use SolarPosition
            Rotation = SatelliteAttitude(transmit, SatR, sf);
         }

         // phase center offset vector in body frame (at L1)
         Triple pco = antenna.getPhaseCenterOffset(1);
         Vector<double> PCO(3);
         for(i=0; i<3; i++) PCO(i) = pco[i]/1000.0;      // body frame, mm -> m

         // PCO vector (from COM to PC) in ECEF XYZ frame, m
         SatPCOXYZ = transpose(Rotation) * PCO;

         Triple pcoxyz(SatPCOXYZ(0),SatPCOXYZ(1),SatPCOXYZ(2));
         satLOSPCO = pcoxyz.dot(S2R);                       // meters

         // phase center variation NB. this should be subtracted from rawrange
         // get the body frame azimuth and nadir angles
         double nadir,az;
         SatelliteNadirAzimuthAngles(SatR, Rx, Rotation, nadir, az);
         satLOSPCV = antenna.getPhaseCenterVariation(1, az, nadir) / 1000.; // meters

      }
      else {
         satLOSPCO = satLOSPCV = 0.0;
         SatPCOXYZ=Vector<double>(3,0.0);
      }

      // ----------------------------------------------------------
      // other quanitites
      // direction cosines
      // cosines[0] = (Rx.X()-SatR.X())/rawrange;
      // cosines[1] = (Rx.Y()-SatR.Y())/rawrange;
      // cosines[2] = (Rx.Z()-SatR.Z())/rawrange;
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


   //------------------------------------------------------------------
   //// should be unnecessary now
   //double RelativityCorrection(const Position& R, const Position& V)
   //{
   //   // relativity correction is added to dtime by the
   //   // XvtStore::getPrnXvt routines...
   //   // dtr = -2*dot(R,V)/(c*c) = -4.4428e-10(s/sqrt(m)) * ecc * sqrt(A(m)) * sinE
   //   // compute it separately here, in units seconds.
   //   double dtr = -2*(R.X()/C_MPS)*(V.X()/C_MPS)
   //                -2*(R.Y()/C_MPS)*(V.Y()/C_MPS)
   //                -2*(R.Z()/C_MPS)*(V.Z()/C_MPS);

   //   return dtr;
   //}

}  // namespace gpstk
