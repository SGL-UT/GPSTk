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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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
 * @file IERSConventions2003.hpp
 * IERSConventions2003
 */

#ifndef GPSTK_IERSCONVENTIONS_HPP
#define GPSTK_IERSCONVENTIONS_HPP

#include <iostream>
#include <cmath>
#include <string>
#include <map>

#include "CommonTime.hpp"
#include "Triple.hpp"
#include "Matrix.hpp"
#include "EOPDataStore.hpp"

namespace gpstk
{
      /// @ingroup ephemcalc 
      //@{

      /** Relation of different Time Systems and Reference System
       *
       * 
       *  The Chart of Different Time System                               
       *-------------------------------------------------------------------
       *
       *          -14s
       *    -----------------> BDT(Compass Time)
       *    |                                                                    
       *    |         +19s             +32.184s           +rel.effects       
       *   GPST ------------> TAI ----------------> TT -----------------> TDB   
       *                      T |                                         
       *           -(UT1-TAI) | |    -leap seconds                          
       *   UT1 ---------------| |--------------------> UTC                
       *    |                                                              
       *    |   earth rotation                                            
       *    ---------------------> GAST                                   
       *
       *
       *   Ritrf = POM * Theta * N * P * Ricrf
       */

   // IERS Data Handling
   //--------------------------------------------------------------------------
   
      /// 'finals.data' from http://maia.usno.navy.mil/
   void LoadIERSFile(const std::string& fileName);
   
      /// ERP data file from IGS
   void LoadIGSFile(const std::string& fileName);

      /// ERP data file from STK
   void LoadSTKFile(const std::string& fileName);

      /// Request EOP Data
   EOPDataStore::EOPData EOPData(const CommonTime& UTC)
      throw(InvalidRequest);
   
      /// in arcsecond
   double PolarMotionX(const CommonTime& UTC);

      /// in arcsecond
   double PolarMotionY(const CommonTime& UTC);

      /// in second
   double UT1mUTC(const CommonTime& UTC);

      /// in arcsecond
   double NutationDPsi(const CommonTime& UTC);

      /// in arcsecond
   double NutationDEps(const CommonTime& UTC);


      /// ftp://maia.usno.navy.mil/ser7/leapsec.dat
   int TAImUTC(const CommonTime& UTC)
      throw(InvalidRequest);


   double TTmTAI();


   double TAImGPST();


   // Time System Handling
   //--------------------------------------------------------------------------
   // @TODO: This enum needs to be replaced in favor of the class in TimeSystem.hpp
   enum TimeSystemEnum
   {
      TS_Unknown = 0,
      TS_UTC,
      TS_UT1,
      TS_GPST,
      TS_TAI,
      TS_TT
   };
   
   CommonTime ConvertTimeSystem(const CommonTime& time, TimeSystemEnum from, TimeSystemEnum to);

   CommonTime GPST2UTC(const CommonTime& GPST);
   CommonTime UTC2GPST(const CommonTime& UTC);

   CommonTime UT12UTC(const CommonTime& UT1);
   CommonTime UTC2UT1(const CommonTime& UTC);

   CommonTime UT12UTC(const CommonTime& UT1,double ut1mutc);
   CommonTime UTC2UT1(const CommonTime& UTC,double ut1mutc);

   CommonTime TT2UTC(const CommonTime& TT);
   CommonTime UTC2TT(const CommonTime& UTC);

   CommonTime TAI2UTC(const CommonTime& TAI);
   CommonTime UTC2TAI(const CommonTime& UTC);

   CommonTime BDT2UTC(const CommonTime& BDT);
   CommonTime UTC2BDT(const CommonTime& UTC);
   
   // Reference System Handling
   //--------------------------------------------------------------------------

   /// Rotate a matrix about the x-axis.
   Matrix<double> Rx(const double& angle);

   /// Rotate a matrix about the y-axis.
   Matrix<double> Ry(const double& angle);

   /// Rotate a matrix about the z-axis.
   Matrix<double> Rz(const double& angle);

   /// Precession matrix by IAU 1976 model
   Matrix<double> iauPmat76(const CommonTime& TT);

   double iauNut80Args(const CommonTime& TT,double& eps, double& dpsi,double& deps)
      throw(Exception);

   // IAU1976/1980 model (IERS conventions 1996)
   void J2kToECEFMatrix(const CommonTime& UTC, 
                        const EOPDataStore::EOPData& ERP,
                        Matrix<double>& POM, 
                        Matrix<double>& Theta, 
                        Matrix<double>& NP)
      throw(Exception);

      /// Convert position from J2000 to ECEF
   Triple J2kPosToECEF(const Triple& j2kPos, const CommonTime& time, TimeSystemEnum sys = TS_GPST);

      /// Convert position from ECEF to J2000
   Triple ECEFPosToJ2k(const Triple& ecefPos, const CommonTime& time, TimeSystemEnum sys = TS_GPST);

      /// ECI to ECF transform matrix, POM * Theta * NP 
   Matrix<double> J2kToECEFMatrix(const CommonTime& UTC,const EOPDataStore::EOPData& ERP);
      

      /// Convert position from J2000 to ECEF.
   Vector<double> J2kPosToECEF(const CommonTime& UTC, const Vector<double>& j2kPos)
      throw(Exception);


      /// Convert position from ECEF to J2000.
   Vector<double> ECEFPosToJ2k(const CommonTime& UTC, const Vector<double>& ecefPos)
      throw(Exception);   


      /// Convert position and velocity from J2000 to ECEF.
   Vector<double> J2kPosVelToECEF(const CommonTime& UTC, const Vector<double>& j2kPosVel)
      throw(Exception);


      /// Convert position and velocity from ECEF to J2000.
   Vector<double> ECEFPosVelToJ2k(const CommonTime& UTC, const Vector<double>& ecefPosVel)
      throw(Exception);   
 
 
      /// sun position in J2000 
   Vector<double> sunJ2kPosition(const CommonTime& TT);
      

      /// moon position in J2000
   Vector<double> moonJ2kPosition(const CommonTime& TT);

      
   //////////////////////////////////////////////////////////////////////////

      /// Normalize angle into the range -pi <= a < +pi.
   double normalizeAngle(double a);
   

      /// Nutation angles by IAU 1980 model
   void nutationAngles(const CommonTime& TT, double& dpsi, double& deps);

      /// Mean obliquity of the ecliptic by IAU 1980 model
   double meanObliquity(const CommonTime& TT);

      /// Equation of the equinoxes by IAU 1994 model
   double iauEqeq94(const CommonTime& TT,double eps, double dPsi);

      /// Greenwich mean sidereal time by IAU 1982 model
   double iauGmst82(const CommonTime& UT1);


      /// Nutation matrix from nutation angles
   Matrix<double> iauNmat(const double& epsa, 
                          const double& dpsi, 
                          const double& deps);

      /// earth rotation angle
   double earthRotationAngle(const CommonTime& UT1);

      /**Earth rotation angle first order rate.
       *  @param TT         Modified Julian Date in TT
       *  @return              d(GAST)/d(t) in [rad]
       */
   double earthRotationAngleRate1(const CommonTime& TT);


      //@}
   
} // namespace gpstk

#endif  // GPSTK_IERSCONVENTIONS2003_HPP
