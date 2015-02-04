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

/// @file BDSEphemeris.cpp Encapsulates the GPS legacy broadcast ephemeris and clock.
/// Inherits OrbitEph, which does most of the work; this class adds health and
/// accuracy information, fit interval, ionospheric correction terms and data
/// flags.

#include <string>
#include "Exception.hpp"
#include "BDSWeekSecond.hpp"

#include "BDSEphemeris.hpp"
#include "GPSWeekSecond.hpp"
#include "WGS84Ellipsoid.hpp"   
#include "TimeString.hpp"
#include "Matrix.hpp"

using namespace std;

namespace gpstk
{
   // Returns true if the time, ct, is within the period of validity of
   // this OrbitEph object.
   // @throw Invalid Request if the required data has not been stored.
   bool BDSEphemeris::isValid(const CommonTime& ct) const
   {
      try {
         if(ct >= beginValid && ct <= endValid) return true;
         return false;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // This function returns the health status of the SV.
   bool BDSEphemeris::isHealthy(void) const
   {
      try {
         OrbitEph::isHealthy();     // ignore the return value; for dataLoaded check
         if(health == 0) return true;
         return false;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // adjustBeginningValidity determines the beginValid and endValid times.
   // Note that this is currently a "best guess" based on observation of Beidou
   // operation. The concept of a fit interval is mentioned in the ICD, but the 
   // fit interval is undefined. 
   //   - It appears the Toe is aligned with the beginning of transmit.
   //   - It is assumed data should not be used prior to transmit.  
   //   - The transmission period appears to be one hour.
   //   - It is assumed that the data will be good for another hour
   //     in order to support SV position determination for 
   //     users that cannot collect navigation message continuously.
   // @throw Invalid Request if the required data has not been stored.
   void BDSEphemeris::adjustValidity(void)
   {
      try {
         OrbitEph::adjustValidity();   // for dataLoaded check
         
         beginValid = ctToe;  // Default case

            // If elements were updated during the hour, then
            // we want to use the later time. 
         if (transmitTime>beginValid) beginValid = transmitTime;
         endValid = ctToe + 3600.0;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }
      
   // Dump the orbit, etc information to the given output stream.
   // @throw Invalid Request if the required data has not been stored.
   void BDSEphemeris::dumpBody(std::ostream& os) const
   {
      try {
         OrbitEph::dumpBody(os);

         os << "           BeiDou-SPECIFIC PARAMETERS\n"
            << scientific << setprecision(8)
            << "Tgd (B1/B3) : " << setw(16) << Tgd13 << " meters" << endl
            << "Tgd (B2/B3) : " << setw(16) << Tgd23 << " meters" << endl
            << "HOW time    : " << setw(6) << HOWtime << " (sec of BDS week "
               << setw(4) << static_cast<BDSWeekSecond>(ctToe).getWeek() << ")"
            << "   fitDuration: " << setw(2) << fitDuration << " hours" << endl
            << "TransmitTime: " << OrbitEph::timeDisplay(transmitTime) << endl
            << "Accuracy    : " << fixed << setprecision(2)
            << getAccuracy() << " meters" << endl
            << "IODC: " << IODC << "   IODE: " << IODE << "   health: " << health
            << endl;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   void BDSEphemeris::dumpTerse(std::ostream& os) const
   {
      string tform = "%03j %02H:%02M:%02S";
      try
      {
	 os << " " << setw(3) << satID.id << " ! ";     
         os << printTime(transmitTime,tform) << " ! " 
	    << printTime(ctToe,tform) << " ! "
	    << printTime(endValid,tform) << " !"
	    << fixed << setprecision(2)
	    << setw(6) << getAccuracy() << "!"
	    << setw(4) << IODC << "!"
	    << setw(4) << IODE << "!"
	    << setw(6) << health << "!" << endl;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //  BDS is different in that some satellites are in GEO orbits.
   //  According to the ICD, the 
   //  SV position derivation for MEO and IGSO is identical to 
   //  that for other kepler+perturbation systems (e.g. GPS); however,
   //  the position derivation for the GEO SVs is different.
   //  According to the ICD, the GEO SVs are those with PRNs 1-5.
   //  The following method overrides OrbitEph.svXvt( ).  It uses
   //  OrbitEph::svXvt( ) for PRNs above 5, but implements a different
   //  algorithm for PRNs 1-5.
   Xvt BDSEphemeris::svXvt(const CommonTime& t) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));
      
         // If the PRN ID is greatet than 5, assume this
         // is a MEO or IGSO SV and use the standard OrbitEph
         // version of svXvt
      if (satID.id>5) return(OrbitEph::svXvt(t));

         // If PRN ID is in the range 1-5, treat this as a GEO
         // 
         // The initial calculations are identical to the standard
         // Kepler+preturbation model
      Xvt sv;
      double ea;              // eccentric anomaly
      double delea;           // delta eccentric anomaly during iteration
      double elapte;          // elapsed time since Toe
      //double elaptc;          // elapsed time since Toc
      //double dtc,dtr;
      double q,sinea,cosea;
      double GSTA,GCTA;
      double amm;
      double meana;           // mean anomaly
      double F,G;             // temporary real variables
      double alat,talat,c2al,s2al,du,dr,di,U,R,truea,AINC;
      double ANLON,cosu,sinu,xip,yip,can,san,cinc,sinc;
      double dek,dlk,div,duv,drv;
      double dxp,dyp;
      double xGK,yGK,zGK;

      WGS84Ellipsoid ell;
      double sqrtgm = SQRT(ell.gm());
      double twoPI = 2.0e0 * PI;
      double lecc;            // eccentricity
      double tdrinc;          // dt inclination
      double Ahalf = SQRT(A); // A is semi-major axis of orbit
      double ToeSOW = GPSWeekSecond(ctToe).sow;    // SOW is time-system-independent

      lecc = ecc;
      tdrinc = idot;

      // Compute time since ephemeris & clock epochs
      elapte = t - ctToe;

      // Compute mean motion
      amm  = (sqrtgm / (A*Ahalf)) + dn;

      // In-plane angles
      //     meana - Mean anomaly
      //     ea    - Eccentric anomaly
      //     truea - True anomaly
      meana = M0 + elapte * amm;
      meana = fmod(meana, twoPI);
      ea = meana + lecc * ::sin(meana);

      int loop_cnt = 1;
      do  {
         F = meana - (ea - lecc * ::sin(ea));
         G = 1.0 - lecc * ::cos(ea);
         delea = F/G;
         ea = ea + delea;
         loop_cnt++;
      } while ((fabs(delea) > 1.0e-11) && (loop_cnt <= 20));

      // Compute clock corrections
      sv.relcorr = svRelativity(t);
      sv.clkbias = svClockBias(t);
      sv.clkdrift = svClockDrift(t);
      sv.frame = ReferenceFrame::WGS84;

      // Compute true anomaly
      q     = SQRT(1.0e0 - lecc*lecc);
      sinea = ::sin(ea);
      cosea = ::cos(ea);
      G     = 1.0e0 - lecc * cosea;

      //  G*SIN(TA) AND G*COS(TA)
      GSTA  = q * sinea;
      GCTA  = cosea - lecc;

      //  True anomaly
      truea = atan2 (GSTA, GCTA);

      // Argument of lat and correction terms (2nd harmonic)
      alat  = truea + w;
      talat = 2.0e0 * alat;
      c2al  = ::cos(talat);
      s2al  = ::sin(talat);

      du  = c2al * Cuc +  s2al * Cus;
      dr  = c2al * Crc +  s2al * Crs;
      di  = c2al * Cic +  s2al * Cis;

      // U = updated argument of lat, R = radius, AINC = inclination
      U    = alat + du;
      R    = A*G  + dr;
      AINC = i0 + tdrinc * elapte  +  di;

      // At this point, the ICD formulation diverges to something 
      // different. 
      //  Longitude of ascending node (ANLON)
      ANLON = OMEGA0 + OMEGAdot * elapte
                     - ell.angVelocity() * ToeSOW;

      // In plane location
      cosu = ::cos(U);
      sinu = ::sin(U);
      xip  = R * cosu;
      yip  = R * sinu;

      //  Angles for rotation
      can  = ::cos(ANLON);
      san  = ::sin(ANLON);
      cinc = ::cos(AINC);
      sinc = ::sin(AINC);

      // GEO satellite coordinates in user-defined inertial system
      xGK  =  xip*can  -  yip*cinc*san;
      yGK  =  xip*san  +  yip*cinc*can;
      zGK  =              yip*sinc;

      // Rz matrix
      double angleZ = ell.angVelocity() * elapte;
      double cosZ = ::cos(angleZ);
      double sinZ = ::sin(angleZ); 

         // Initiailize 3X3 with all 0.0
      gpstk::Matrix<double> matZ(3,3); 
      // Row,Col
      matZ(0,0) =  cosZ;
      matZ(0,1) =  sinZ;
      matZ(0,2) =   0.0;
      matZ(1,0) = -sinZ;
      matZ(1,1) =  cosZ; 
      matZ(1,2) =   0.0;
      matZ(2,0) =   0.0;
      matZ(2,1) =   0.0;
      matZ(2,2) =   1.0; 

      // Rx matrix
      double angleX = -5.0 * PI/180.0;    /// This is a constant.  Should set it once
      double cosX = ::cos(angleX);
      double sinX = ::sin(angleX); 
      gpstk::Matrix<double> matX(3,3);
      matX(0,0) =   1.0;
      matX(0,1) =   0.0;
      matX(0,2) =   0.0;
      matX(1,0) =   0.0;
      matX(1,1) =  cosX;
      matX(1,2) =  sinX;
      matX(2,0) =   0.0;
      matX(2,1) = -sinX;
      matX(2,2) =  cosX;

      // Matrix (single column) of xGK, yGK, zGK
      gpstk::Matrix<double> inertialPos(3,1);
      inertialPos(0,0) = xGK;
      inertialPos(1,0) = yGK;
      inertialPos(2,0) = zGK;

      gpstk::Matrix<double> result(3,1);
      result = matZ * matX * inertialPos;

      sv.x[0] = result(0,0);
      sv.x[1] = result(1,0);
      sv.x[2] = result(2,0);

      // derivatives of true anamoly and arg of latitude
      dek = amm / G; 
      dlk =  Ahalf * q * sqrtgm / (R*R);

      // in-plane, cross-plane, and radial derivatives
      div = tdrinc - 2.0e0 * dlk * (Cis  * c2al - Cic * s2al);
      duv = dlk*(1.e0+ 2.e0 * (Cus*c2al - Cuc*s2al));
      drv = A * lecc * dek * sinea + 2.e0 * dlk * (Crs * c2al - Crc * s2al);

      //
      dxp = drv*cosu - R*sinu*duv;
      dyp = drv*sinu + R*cosu*duv;

      // Time-derivative of Rz matrix
      gpstk::Matrix<double> dmatZ(3,3);
      // Row,Col
      dmatZ(0,0) =  sinZ * -ell.angVelocity();
      dmatZ(0,1) = -cosZ * -ell.angVelocity();
      dmatZ(0,2) =   0.0;
      dmatZ(1,0) =  cosZ * -ell.angVelocity();
      dmatZ(1,1) =  sinZ * -ell.angVelocity();
      dmatZ(1,2) =   0.0;
      dmatZ(2,0) =   0.0;
      dmatZ(2,1) =   0.0;
      dmatZ(2,2) =   0.0;

      // Time-derivative of X,Y,Z in interial form
      gpstk::Matrix<double> dIntPos(3,1);
      dIntPos(0,0) = - xip * san * OMEGAdot
                     + dxp * can
                     - yip * (cinc * can * OMEGAdot
                             -sinc * san * div )
                     - dyp * cinc * san;
      dIntPos(1,0) =   xip * can * OMEGAdot
                     + dxp * san
                     - yip * (cinc * san * OMEGAdot
                             +sinc * can * div )
                     + dyp * cinc * can;
      dIntPos(2,0) = yip * cinc * div + dyp * sinc;

      /*
      cout << "dIntPos : " << dIntPos(0,0) 
                           << ", " << dIntPos(1,0)
                           << ", " << dIntPos(2,0) << endl;
      double vMag = ::sqrt(dIntPos(0,0)*dIntPos(0,0) + 
                           dIntPos(1,0)*dIntPos(1,0) +                    
                           dIntPos(2,0)*dIntPos(2,0) );
      cout << " dIntPos Mag: " << vMag << endl;     
      cout << "dmatZ : " << dmatZ(0,0) 
                   << ", " << dmatZ(0,1)
                   << ", " << dmatZ(0,2) << endl; 
      cout << "dmatZ : " << dmatZ(1,0) 
                   << ", " << dmatZ(1,1)
                   << ", " << dmatZ(1,2) << endl; 
      cout << "dmatZ : " << dmatZ(2,0) 
                   << ", " << dmatZ(2,1)
                   << ", " << dmatZ(2,2) << endl; 
      */
      gpstk::Matrix<double> vresult(3,1);
      vresult =  matZ * matX * dIntPos + 
                dmatZ * matX * inertialPos;

      /* debug
      gpstk::Matrix<double> firstHalf(3,1);
      firstHalf = matZ * matX * dIntPos;
      gpstk::Matrix<double> secondHalf(3,1);
      secondHalf = dmatZ * matX * inertialPos;

      cout << "firstHalf: " << firstHalf(0,0)
                    << ", " << firstHalf(1,0)
                    << ", " << firstHalf(2,0) << endl;
      cout << "secondHalf: " << secondHalf(0,0)
                    << ", " << secondHalf(1,0)
                    << ", " << secondHalf(2,0) << endl;
      end debug */
     
      // Move results into output variables
      sv.v[0] = vresult(0,0);
      sv.v[1] = vresult(1,0);
      sv.v[2] = vresult(2,0);
      
      return sv;
   }

} // end namespace
