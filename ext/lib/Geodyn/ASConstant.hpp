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

#ifndef GPSTK_ASCONSTANT_HPP
#define GPSTK_ASCONSTANT_HPP

#include <string>
#include <cmath>

namespace gpstk
{

      /** Astrodynamical Constants
       * 
       */
   namespace ASConstant
   {
      //* Physical Const
      //=======================================================================

         /// Speed of Light in m/s from IAU 1976.
      static const double SPEED_OF_LIGHT = 299792458.0;

         /// Astronomical Unit [m]; IAY 1976
      static const double AU = 149597870000.0;      // [m]




      //* Math Const
      //=========================================================================

      static const double PI = 3.14159265358979323846264338327950288419716939937511;

      static const double TWO_PI = 6.28318530717958647692528676655900576839433879875022;
      
      //static const double E = 2.71828182845904523536028747135266249775724709369996;


      // Time Constant
      //=================================================================================
      
      static const double JD_MJD_OFFSET = 2400000.5;    // Vallado page 187

      static const double MJD_J2000 = 51544.5;           //Modified Julian Date of the J2000 Epoch.
     
         
         /// GM of the Earth in km^3/s^2 from JGM-3.
      static const double MU = 398600.4415;
      
         /// Earth's J2 Value from JGM-3.
      static const double J2 = 0.00108263;

         /// Acceleration due to gravity near earth in m/s.
      static const double G0 = -9.780;

         /// Mean Earth Radius in km from WGS-84.
      static const double re = 6378.137;               // radius of earth in kilometers
         
         /// Earth atmosphere model parameter.
      static const double h_0 = 920000.0;              // atmosphere model parameter
         
         /// Earth atmosphere model parameter.
      static const double rho_0 = 4.36E-14;            // atmosphere model parameter
      
         /// Earth atmosphere model parameter.
      static const double gamma_0 = 5.381E-06;         // atmosphere model parameter

      /// Earth's rotation rate in rad/s.
      //static const double omega_e = 7.2921157746E-05;   // earth rotation rate
      static const double omega_e = 7.292115E-05;         // IERS 1996 conventions
      //static const double omega_e = 7.2921158553E-05;   // Vallado

         /// Equatorial radius of earth in m from WGS-84
      static const double R_Earth = 6378.137e3;      // Radius Earth [m]; WGS-84
      
         /// Flattening factor of earth from WGS-84
      static const double f_Earth = 1.0/298.257223563; // Flattening; WGS-84


         /// Mean radius of the Sun in m
      static const double R_Sun  = 696.0e6;//6.9599e8;            // [m] STK
         
         /// Mean radius of the Moon in m
      static const double R_Moon  = 1738000;//6.9599e8;            // [m] STK

      /// Solar radiation pressure at 1 AU in N/m^2
      static const double P_Sol      = 4.560E-6;          // [N/m^2] (~1367 W/m^2); IERS 96


      /// Earth gravity constant in m^3/s^2 from JGM3
      static const double GM_Earth   = 3.986004415e14;    // [m^3/s^2]; JGM3

      /// Earth gravity constant in m^3/s^2 from JGM3
      static const double GM_WGS84    = 398600.5e+9;    // [m^3/s^2]; JGM3
      /// Earth's rotation rate in rad/s.
      static const double WE_WGS84 = 7.2921151467E-05;  // earth rotation rate

      /// Sun gravity constant in m^3/s^2 from IAU 1976
      //static const double GM_Sun      = 1.32712438e+20;    // [m^3/s^2]; IAU 1976
      static const double GM_Sun      = 1.3271250e20;//1.3271220e+20;    // [m^3/s^2]; STK

      /// Moon gravity constant in m^3/s^2 from DE200
      //static const double GM_Moon     = GM_Earth/81.300587;// [m^3/s^2]; DE200
      static const double GM_Moon     = 4.9027890e12;//4.90279490e+12;    // [m^3/s^2]; STK
      //public final static double GM_Moon     = GM_Earth*0.012300034;    // [m^3/s^2]; JPL ssd

      /// Obliquity of the ecliptic, J2000 in degrees
      static const double EPS = 23.43929111;

   }  // End of 'namespace ASConstant'

}  // End of 'namespace gpstk'

#endif //GNSSTK_ASCONSTANT_HPP
