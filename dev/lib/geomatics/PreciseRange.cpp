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

/**
 * @file PreciseRange.cpp
 * Implement computation of range and associated quantities from EphemerisStore,
 * including earth orientation corrections, given receiver position and time,
 * in class PreciseRange.
 */

// -----------------------------------------------------------------------------------
// GPSTk includes
#include "PreciseRange.hpp"
#include "MiscMath.hpp"
#include "GPSGeoid.hpp"
#include "icd_200_constants.hpp"
#include "geometry.hpp"
#include "GeodeticFrames.hpp"

// Geomatic lib includes
#include "GSatID.hpp"

using namespace std;

namespace gpstk
{
   double PreciseRange::ComputeAtReceiveTime(const DayTime& tr_nom,
                                             const Position& Rx,
                                             const int prn,
                                             const XvtStore<SatID>& Eph,
                                             const EarthOrientation& EO)
   throw(InvalidRequest)
   {
   try {
      int nit,i;
      double tof,tof_old,wt,sx,sy;
      GPSGeoid geoid;
      static bool first=false;

      nit = 0;
      tof = 0.07;       // time of flight, initial guess 70ms
      do {
            // best estimate of transmit time
         transmit = tr_nom;
         transmit -= tof;
         tof_old = tof;

            // get SV position
         try {
            GSatID sat(prn,GSatID::systemGPS);
            Xvt svPosVel = Eph.getXvt(sat,transmit);
            SVR = Position(svPosVel.x[0],svPosVel.x[1],svPosVel.x[2]);// default is
            SVV = Position(svPosVel.v[0],svPosVel.v[1],svPosVel.v[2]);// Cartesian
            SVdtime = svPosVel.dtime;
            SVdrift = svPosVel.ddtime;
         }
         catch(InvalidRequest& e)
         {
            GPSTK_RETHROW(e);
         }

            // compute new time of flight
         tof = range(SVR,Rx);
         tof /= geoid.c();

            // correct for Earth rotation
         double rr1,rr2;
         Matrix<double> Rot1,Rot2;
         Vector<double> Rterr1(3),Rcel(3),Rterr2(3);

         wt = geoid.angVelocity()*tof;
         Rot1 = rotation<double>(wt,3);
         if(first) {
            cout << "Simple rotation\n"
               << fixed << setw(13) << setprecision(9) << Rot1 << endl;
         }
         Matrix<double> W=GeodeticFrames::PolarMotion(EO.xp,EO.yp);
         Matrix<double> Srx =
            rotation<double>(
               (GeodeticFrames::GMST(transmit,0.0,false)
               -GeodeticFrames::GMST(tr_nom,EO.UT1mUTC,false)),3);
         Rot2 = transpose(W)*Srx;
         if(first) {
            cout << "Complex rotation\n"
                << fixed << setw(13) << setprecision(9) << Rot2 << endl;
         }

         for(i=0; i<3; i++) Rcel(i)=SVR[i];
         Rterr1 = Rot1*Rcel;
         Rterr2 = Rot2*Rcel;
         for(i=0; i<3; i++) SVR[i]=Rterr1(i);
         //for(i=0; i<3; i++) SVR[i]=Rterr2(i);

         rr1 = range(SVR,Rx);
         rr2 = range(SVR,Rx);

         if(first) {
            cout << "Simple result "
               << fixed << setw(13) << setprecision(9) << Rterr1 << endl;
            cout << "Complex result "
               << fixed << setw(13) << setprecision(9) << Rterr2 << endl;
            cout << "Position difference "
               << fixed << setw(13) << setprecision(9) << (Rterr1-Rterr2) << endl;
            cout << "Range difference "
               << fixed << setw(13) << setprecision(9) << (rr1-rr2) << endl;
            first = false;
         }

            // update raw range
         rawrange = rr1;
         //rawrange = rr2;

         tof = rawrange/geoid.c();

         // dont forget velocity - used for relativity
         for(i=0; i<3; i++) Rcel(i)=SVV[i];
         Rterr1 = Rot1*Rcel;
         for(i=0; i<3; i++) SVV[i]=Rterr1(i);
         //Rterr2 = Rot2*Rcel;
         //for(i=0; i<3; i++) SVV[i]=Rterr2(i);

      } while(ABS(tof-tof_old)>1.e-13 && ++nit<5);

      relativity = PreciseRelativityCorrection(SVR,SVV) * C_GPS_M;
      // relativity correction is added to dtime by the
      // EphemerisStore::getPrnXvt routines...

      svclkbias = SVdtime*C_GPS_M - relativity;
      svclkdrift = SVdrift * C_GPS_M;

      cosines[0] = (Rx.X()-SVR.X())/rawrange;
      cosines[1] = (Rx.Y()-SVR.Y())/rawrange;
      cosines[2] = (Rx.Z()-SVR.Z())/rawrange;

      elevation = Rx.elevation(SVR);
      azimuth = Rx.azimuth(SVR);

      return (rawrange-svclkbias-relativity);

      }
      catch(gpstk::Exception& e) {
         GPSTK_RETHROW(e);
      }
   }  // end PreciseRange::ComputeAtReceiveTime

   double PreciseRelativityCorrection(const Position& R, const Position& V)
   {
      // relativity correction is added to dtime by the
      // EphemerisStore::getPrnXvt routines...
      // dtr = -2*dot(R,V)/(c*c) = -4.4428e-10(s/sqrt(m)) * ecc * sqrt(A(m)) * sinE
      // compute it separately here, in units seconds.
      double dtr = -2*(R.X()/C_GPS_M)*(V.X()/C_GPS_M)
                   -2*(R.Y()/C_GPS_M)*(V.Y()/C_GPS_M)
                   -2*(R.Z()/C_GPS_M)*(V.Z()/C_GPS_M);

      // TD add Sagnac and second order terms

      return dtr;
   }

}  // namespace gpstk
