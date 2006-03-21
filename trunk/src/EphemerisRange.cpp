#pragma ident "$Id: //depot/sgl/gpstk/dev/src/EphemerisRange.cpp#2 $"

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
 * @file EphemerisRange.cpp
 * Computation of range and associated quantities from EphemerisStore,
 * given receiver position and time.
 */

#include "EphemerisRange.hpp"
#include "MiscMath.hpp"
#include "GPSGeoid.hpp"
#include "icd_200_constants.hpp"
#include "geometry.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
      // Compute the corrected range at RECEIVE time, from receiver at position Rx,
      // to the GPS satellite given by PRN prn, as well as all the CER quantities,
      // given the nominal receive time tr_nom and an EphemerisStore.
   double CorrectedEphemerisRange::ComputeAtReceiveTime(const DayTime& tr_nom,
                                                        const Position& Rx,
                                                        const int prn,
                                                        const EphemerisStore& Eph)
   {
   try {
      int nit;
      double tof,tof_old,wt,sx,sy;
      GPSGeoid geoid;

      nit = 0;
      tof = 0.07;       // initial guess 70ms
      do {
            // best estimate of transmit time
         transmit = tr_nom;
         transmit -= tof;
         tof_old = tof;
            // get SV position
         try {
            svPosVel = Eph.getPrnXvt(prn,transmit);
         }
         catch(EphemerisStore::NoEphemerisFound& e)
         {
            GPSTK_RETHROW(e);
         }
         catch(InvalidRequest& ir)
         {
            EphemerisStore::NoEphemerisFound nef(ir);
            GPSTK_THROW(nef);
         }
            // compute new time of flight
         tof = RSS(svPosVel.x[0]-Rx.X(),
                   svPosVel.x[1]-Rx.Y(),
                   svPosVel.x[2]-Rx.Z());
         tof /= geoid.c();
            // correct for Earth rotation
         wt = geoid.angVelocity()*tof;
         sx =  cos(wt)*svPosVel.x[0] + sin(wt)*svPosVel.x[1];
         sy = -sin(wt)*svPosVel.x[0] + cos(wt)*svPosVel.x[1];
         svPosVel.x[0] = sx;
         svPosVel.x[1] = sy;
         sx =  cos(wt)*svPosVel.v[0] + sin(wt)*svPosVel.v[1];
         sy = -sin(wt)*svPosVel.v[0] + cos(wt)*svPosVel.v[1];
         svPosVel.v[0] = sx;
         svPosVel.v[1] = sy;
            // update raw range and time of flight
         rawrange = RSS(svPosVel.x[0]-Rx.X(),
                        svPosVel.x[1]-Rx.Y(),
                        svPosVel.x[2]-Rx.Z());
         tof = rawrange/geoid.c();

      } while(ABS(tof-tof_old)>1.e-13 && ++nit<5);

      relativity = RelativityCorrection(svPosVel) * C_GPS_M;
      // relativity correction is added to dtime by the
      // EphemerisStore::getPrnXvt routines...

      svclkbias = svPosVel.dtime*C_GPS_M - relativity;
      svclkdrift = svPosVel.ddtime * C_GPS_M;

      cosines[0] = (Rx.X()-svPosVel.x[0])/rawrange;
      cosines[1] = (Rx.Y()-svPosVel.x[1])/rawrange;
      cosines[2] = (Rx.Z()-svPosVel.x[2])/rawrange;

      Position SV(svPosVel);
      elevation = Rx.elevation(SV);
      azimuth = Rx.azimuth(SV);

      return (rawrange-svclkbias-relativity);
   }
      catch(gpstk::Exception& e) {
         GPSTK_RETHROW(e);
      }
   }  // end CorrectedEphemerisRange::ComputeAtReceiveTime

      // Compute the corrected range at TRANSMIT time, from receiver at position Rx,
      // to the GPS satellite given by PRN prn, as well as all the CER quantities,
      // given the nominal receive time tr_nom and an EphemerisStore, as well as
      // the raw measured pseudorange.
   double CorrectedEphemerisRange::ComputeAtTransmitTime(const DayTime& tr_nom,
                                                         const double& pr,
                                                         const Position& Rx,
                                                         const int prn,
                                                         const EphemerisStore& Eph)
   {
   try {
      unsigned long ref;
      DayTime tt;
      GPSGeoid geoid;

      // 0-th order estimate of transmit time = receiver - pseudorange/c
      transmit = tr_nom;
      transmit -= pr/C_GPS_M;
      tt = transmit;

      // correct for SV clock
      for(int i=0; i<2; i++) {
         // get SV position
         try {
            svPosVel = Eph.getPrnXvt(prn,tt);
         }
         catch(EphemerisStore::NoEphemerisFound& e)
         {
            GPSTK_RETHROW(e);
         }
         catch(InvalidRequest& ir)
         {
            EphemerisStore::NoEphemerisFound nef(ir);
            GPSTK_THROW(nef);
         }
         tt = transmit;
         tt -= svPosVel.dtime;      // clock and relativity
      }

      // correct for Earth rotation
      double tof = RSS(svPosVel.x[0]-Rx.X(),
                       svPosVel.x[1]-Rx.Y(),
                       svPosVel.x[2]-Rx.Z())/geoid.c();
      double wt = geoid.angVelocity()*tof;
      double sx =  cos(wt)*svPosVel.x[0] + sin(wt)*svPosVel.x[1];
      double sy = -sin(wt)*svPosVel.x[0] + cos(wt)*svPosVel.x[1];
      svPosVel.x[0] = sx;
      svPosVel.x[1] = sy;
      sx =  cos(wt)*svPosVel.v[0] + sin(wt)*svPosVel.v[1];
      sy = -sin(wt)*svPosVel.v[0] + cos(wt)*svPosVel.v[1];
      svPosVel.v[0] = sx;
      svPosVel.v[1] = sy;
      // raw range
      rawrange = RSS(svPosVel.x[0]-Rx.X(),
                     svPosVel.x[1]-Rx.Y(),
                     svPosVel.x[2]-Rx.Z());

      relativity = RelativityCorrection(svPosVel) * C_GPS_M;
      // relativity correction is added to dtime by the
      // EphemerisStore::getPrnXvt routines...

      svclkbias = svPosVel.dtime*C_GPS_M - relativity;
      svclkdrift = svPosVel.ddtime * C_GPS_M;

      cosines[0] = (Rx.X()-svPosVel.x[0])/rawrange;
      cosines[1] = (Rx.Y()-svPosVel.x[1])/rawrange;
      cosines[2] = (Rx.Z()-svPosVel.x[2])/rawrange;

      Position SV(svPosVel);
      elevation = Rx.elevation(SV);
      azimuth = Rx.azimuth(SV);

      return (rawrange-svclkbias-relativity);
   }
   catch(gpstk::Exception& e) {
      GPSTK_RETHROW(e);
   }
   }  // end CorrectedEphemerisRange::ComputeAtTransmitTime

   double RelativityCorrection(const Xvt& svPosVel)
   {
      // relativity correction is added to dtime by the
      // EphemerisStore::getPrnXvt routines...
      // dtr = -2*dot(R,V)/(c*c) = -4.4428e-10(s/sqrt(m)) * ecc * sqrt(A(m)) * sinE
      // compute it separately here, in units seconds.
      double dtr = -2*(svPosVel.x[0]/C_GPS_M)*(svPosVel.v[0]/C_GPS_M)
                  -2*(svPosVel.x[1]/C_GPS_M)*(svPosVel.v[1]/C_GPS_M)
                  -2*(svPosVel.x[2]/C_GPS_M)*(svPosVel.v[2]/C_GPS_M);
      return dtr;
   }

}  // namespace gpstk
