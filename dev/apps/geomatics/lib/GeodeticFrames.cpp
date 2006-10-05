#pragma ident "$Id: $"

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
 * @file GeodeticFrames.cpp
 * Implement geodetic frame computations in the GeodeticFrames class.
 * class gpstk::GeodeticFrames encapsulates frame transformations between the
 * conventional terrestrial frame and the conventional inertial frame, as defined
 * by the reference. It implements models of precession and nutation (IERS 1996) of
 * Earth's axis, as well as the precise rotation of Earth and its 'wobble'
 * as given by the Earth orientation parameters (see class EarthOrientation).
 * Reference: IERS Technical Note 21, IERS Conventions (1996),
 * by Dennis D. McCarthy, U.S. Naval Observatory.
 */

//------------------------------------------------------------------------------------
// GPSTk includes
#include "StringUtils.hpp"
#include "geometry.hpp"             // for DEG_TO_RAD
#include "icd_200_constants.hpp"    // for PI and TWO_PI
#include "GeodeticFrames.hpp"

using namespace std;

namespace gpstk
{
   //---------------------------------------------------------------------------------
   //---------------------------------------------------------------------------------
   // constants
   // epoch for CoordTransTime
   const long GeodeticFrames::JulianEpoch=2451545;

   //---------------------------------------------------------------------------------
   //---------------------------------------------------------------------------------
   // functions used internally

   //---------------------------------------------------------------------------------
   // Compute the 'coordinate transformation time' which is the time since
   // epoch J2000 = January 1 2000 12h UT = 2451545.0JD, divided by 36525 days.
   // This quantity is used throughout the terrestrial / inertial coordinate
   // transformations.
   double GeodeticFrames::CoordTransTime(DayTime t)
      throw()
   {
      double ct = (t.JD()-double(JulianEpoch))/36525.0; // day contribution
      ct += ((t.secOfDay()-43200.0)/86400.0)/36525.0;   // seconds contribution
      return ct;
   }

   //---------------------------------------------------------------------------------
   // Return mean longitude of lunar ascending node, in degrees,
   // given T, the CoordTransTime at the Epoch of interest. (Ref: F5 pg 23)
   double GeodeticFrames::Omega(double T)
      throw()
   {
      double om;
      om = -0.00005939*T;           // seconds of arc
      om = (om + 0.007702)*T;
      om = (om + 7.4722)*T;
      om /= 3600.0;                 // convert to degrees
      om = (om - 1934.136185139)*T; // 1934.136185139 = 6962890.2665/3600.0
      om += 125.04455501;
      //om = fmod(om,360.0);
      return om;
   }

   //---------------------------------------------------------------------------------
   // Return mean longitude of the moon - Omega, in degrees,
   // given T, the CoordTransTime at the Epoch of interest. (Ref: F3 pg 23)
   double GeodeticFrames::F(double T)
      throw()
   {
      double f;
      f = 0.00000417*T;             // seconds of arc
      f = (f - 0.001037)*T;
      f = (f - 12.7512)*T;
      f /= 3600.0;                  // convert to degrees
      f = (f + 483202.01745772)*T;  // 483202.01745772 = 1739527262.8478/3600.0
      f += 93.27209062;
      //f = fmod(f,360.0);
      return 0.0;
   }

   //---------------------------------------------------------------------------------
   // Return mean elongation of the moon from the sun, in degrees,
   // given T, the CoordTransTime at the Epoch of interest. (Ref: F4 pg 23)
   double GeodeticFrames::D(double T)
      throw()
   {
      double d;
      d = -0.00003169*T;            // seconds of arc
      d = (d + 0.006593)*T;
      d = (d - 6.3706)*T;
      d /= 3600.0;                  // convert to degrees
      d = (d + 445267.111446944)*T; // 445267.111446944 = 1602961601.2090 / 3600.0
      d += 297.85019547;
      //d = fmod(d,360.0);
      return d;
   }

   //---------------------------------------------------------------------------------
   // Return mean anomaly of the moon, in degrees,
   // given T, the CoordTransTime at the Epoch of interest. (Ref: F1 pg 23)
   double GeodeticFrames::L(double T)
      throw()
   {
      double l;
      l = -0.00024470*T;            // seconds of arc
      l = (l + 0.051635)*T;
      l = (l + 31.8792)*T;
      l /= 3600.0;                  // convert to degrees
      l = (l + 477198.8675605)*T;   // 477198.8675605 = 1717915923.2178 / 3600.0
      l += 134.96340251;
      //l = fmod(l,360.0);
      return l;
   }

   //---------------------------------------------------------------------------------
   // Return mean anomaly of the sun, in degrees,
   // given T, the CoordTransTime at the Epoch of interest. (Ref: F2 pg 23)
   double GeodeticFrames::Lp(double T)
      throw()
   {
      double lp;
      lp = -0.00001149*T;           // seconds of arc
      lp = (lp + 0.000136)*T;
      lp = (lp - 0.5532)*T;
      lp /= 3600.0;                 // convert to degrees
      lp = (lp + 35999.050291139)*T;// 35999.050291139 = 129596581.0481 / 3600.0
      lp += 357.52910918;
      //lp = fmod(lp,360.0);
      return lp;
   }

   //---------------------------------------------------------------------------------
   // Compute eps, the obliquity of the ecliptic, in degrees,
   // given T, the CoordTransTime at the time of interest. IAU76
   double GeodeticFrames::Obliquity(double T)
      throw()
   {
      double ep;
      // seconds of arc
      ep = T*(-46.8150 + T*(-0.00059 + T*0.001813));
      ep /= 3600.0;                       // convert to degrees
      // degrees
      ep += 23.43929111;                  // = 84381.448/3600.0

      return ep;
   }

   //---------------------------------------------------------------------------------
   // Nutation of the obliquity (deps) and of the longitude (dpsi), IERS 1996
   // model (ref pg 26), given
   // @param T,    the coordinate transformation time at the time of interest
   // @param deps, nutation of the obliquity, in arc seconds (output)
   // @param dpsi, nutation of the longitude, in arc seconds (output)
   void GeodeticFrames::NutationAngles(double T,
                                       double& deps,
                                       double& dpsi)
      throw()
   {
		//-----------------------------------------------------------------------
		// Code to implement Table 5.2 of IERS Conventions 1996 series for
		// nutation in longitude dpsi and obliquity deps in arc seconds.
		// (Generated by perl script Table52.pl)
		double arg;
		dpsi = deps = 0.0;
	
		// double T must be defined as time in Julian centuries from epoch J2000.0
		// Also define (all doubles):
		double o = Omega(T); // = mean longitude of lunar ascending node, in degrees,
		double f = F(T)    ; // = mean longitude of the moon - Omega, in degrees,
		double d = D(T)    ; // = mean elongation of the moon from the sun, in degrees,
		double l = L(T)    ; // = mean anomaly of the moon, in degrees, and
		double lp = Lp(T)  ; // = mean anomaly of the sun, in degrees.
	   o *= DEG_TO_RAD;
	   f *= DEG_TO_RAD;
	   d *= DEG_TO_RAD;
	   l *= DEG_TO_RAD;
	   lp *= DEG_TO_RAD;
	
		// line 1 of Table 5.2, period = -6798.38 days
		arg = o;
		dpsi += (-17.206277 - 0.017419*T) * ::sin(arg) + 0.003645 * ::cos(arg);
		deps += (+9.205356 + 0.000886*T) * ::cos(arg) + 0.001553 * ::sin(arg);
		// line 2 of Table 5.2, period = 182.62 days
		arg = 2*f-2*d+2*o;
		dpsi += (-1.317014 - 0.000156*T) * ::sin(arg) - 0.001400 * ::cos(arg);
		deps += (+0.573058 - 0.000306*T) * ::cos(arg) - 0.000464 * ::sin(arg);
		// line 3 of Table 5.2, period = 13.66 days
		arg = 2*f+2*o;
		dpsi += (-0.227720 - 0.000023*T) * ::sin(arg) + 0.000269 * ::cos(arg);
		deps += (+0.097864 - 0.000048*T) * ::cos(arg) + 0.000136 * ::sin(arg);
		// line 4 of Table 5.2, period = -3399.18 days
		arg = 2*o;
		dpsi += (+0.207429 + 0.000021*T) * ::sin(arg) - 0.000071 * ::cos(arg);
		deps += (-0.089747 + 0.000047*T) * ::cos(arg) - 0.000029 * ::sin(arg);
		// line 5 of Table 5.2, period = -365.26 days
		arg = -lp;
		dpsi += (-0.147538 + 0.000364*T) * ::sin(arg) + 0.001121 * ::cos(arg);
		deps += (+0.007388 - 0.000019*T) * ::cos(arg) + 0.000198 * ::sin(arg);
		// line 6 of Table 5.2, period = 121.75 days
		arg = lp+2*f-2*d+2*o;
		dpsi += (-0.051687 + 0.000123*T) * ::sin(arg) - 0.000054 * ::cos(arg);
		deps += (+0.022440 - 0.000068*T) * ::cos(arg) - 0.000018 * ::sin(arg);
		// line 7 of Table 5.2, period = 27.55 days
		arg = l;
		dpsi += (+0.071118 + 0.000007*T) * ::sin(arg) - 0.000094 * ::cos(arg);
		deps -= 0.000687 * ::cos(arg) + 0.000039 * ::sin(arg);
		// line 8 of Table 5.2, period = 13.63 days
		arg = 2*f+o;
		dpsi += (-0.038752 - 0.000037*T) * ::sin(arg) + 0.000034 * ::cos(arg);
		deps += (+0.020076 + 0.000002*T) * ::cos(arg) + 0.000032 * ::sin(arg);
		// line 9 of Table 5.2, period = 9.13 days
		arg = l+2*f+2*o;
		dpsi += (-0.030137 - 0.000004*T) * ::sin(arg) + 0.000077 * ::cos(arg);
		deps += (+0.012896 - 0.000006*T) * ::cos(arg) + 0.000035 * ::sin(arg);
		// line 10 of Table 5.2, period = 365.22 days
		arg = -lp+2*f-2*d+2*o;
		dpsi += (+0.021583 - 0.000049*T) * ::sin(arg) + 0.000006 * ::cos(arg);
		deps += (-0.009591 + 0.000030*T) * ::cos(arg) + 0.000012 * ::sin(arg);
		// line 11 of Table 5.2, period = 177.84 days
		arg = 2*f-2*d+o;
		dpsi += (+0.012820 + 0.000014*T) * ::sin(arg) + 0.000018 * ::cos(arg);
		deps += (-0.006897 - 0.000001*T) * ::cos(arg) + 0.000004 * ::sin(arg);
		// line 12 of Table 5.2, period = 27.09 days
		arg = -l+2*f+2*o;
		dpsi += (+0.012353 + 0.000001*T) * ::sin(arg) + 0.000002 * ::cos(arg);
		deps += (-0.005334 + 0.000003*T) * ::cos(arg);
		// line 13 of Table 5.2, period = 31.81 days
		arg = -l+2*d;
		dpsi += (+0.015699 + 0.000001*T) * ::sin(arg) - 0.000018 * ::cos(arg);
		deps -= 0.000127 * ::cos(arg) + 0.000009 * ::sin(arg);
		// line 14 of Table 5.2, period = 27.67 days
		arg = l+o;
		dpsi += (+0.006314 + 0.000006*T) * ::sin(arg) + 0.000003 * ::cos(arg);
		deps -= 0.003323 * ::cos(arg) - 0.000001 * ::sin(arg);
		// line 15 of Table 5.2, period = -27.44 days
		arg = -l+o;
		dpsi += (-0.005797 - 0.000006*T) * ::sin(arg) - 0.000019 * ::cos(arg);
		deps += 0.003141 * ::cos(arg) - 0.000008 * ::sin(arg);
		// line 16 of Table 5.2, period = 9.56 days
		arg = -l+2*f+2*d+2*o;
		dpsi += (-0.005965 - 0.000001*T) * ::sin(arg) + 0.000014 * ::cos(arg);
		deps += (+0.002554 - 0.000001*T) * ::cos(arg) + 0.000007 * ::sin(arg);
		// line 17 of Table 5.2, period = 9.12 days
		arg = l+2*f+o;
		dpsi += (-0.005163 - 0.000004*T) * ::sin(arg) + 0.000012 * ::cos(arg);
		deps += 0.002635 * ::cos(arg) + 0.000008 * ::sin(arg);
		// line 18 of Table 5.2, period = 1305.48 days
		arg = -2*l+2*f+o;
		dpsi += (+0.004590 + 0.000005*T) * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += (-0.002424 - 0.000001*T) * ::cos(arg) + 0.000001 * ::sin(arg);
		// line 19 of Table 5.2, period = 14.77 days
		arg = 2*d;
		dpsi += (+0.006336 + 0.000001*T) * ::sin(arg) - 0.000015 * ::cos(arg);
		deps -= 0.000125 * ::cos(arg) + 0.000003 * ::sin(arg);
		// line 20 of Table 5.2, period = 7.10 days
		arg = 2*f+2*d+2*o;
		dpsi -= 0.003854 * ::sin(arg) + 0.000015 * ::cos(arg);
		deps += 0.001643 * ::cos(arg) + 0.000006 * ::sin(arg);
		// line 21 of Table 5.2, period = -205.89 days
		arg = -2*l+2*d;
		dpsi -= 0.004774 * ::sin(arg) - 0.000002 * ::cos(arg);
		deps += 0.000048 * ::cos(arg) - 0.000003 * ::sin(arg);
		// line 22 of Table 5.2, period = 6.86 days
		arg = 2*l+2*f+2*o;
		dpsi -= 0.003102 * ::sin(arg) + 0.000012 * ::cos(arg);
		deps += (+0.001323 - 0.000001*T) * ::cos(arg) + 0.000005 * ::sin(arg);
		// line 23 of Table 5.2, period = 23.94 days
		arg = l+2*f-2*d+2*o;
		dpsi += 0.002863 * ::sin(arg);
		deps += (-0.001235 + 0.000001*T) * ::cos(arg);
		// line 24 of Table 5.2, period = 26.98 days
		arg = -l+2*f+o;
		dpsi += (+0.002044 + 0.000002*T) * ::sin(arg) + 0.000001 * ::cos(arg);
		deps -= 0.001076 * ::cos(arg);
		// line 25 of Table 5.2, period = 13.78 days
		arg = 2*l;
		dpsi += 0.002923 * ::sin(arg) - 0.000008 * ::cos(arg);
		deps -= 0.000062 * ::cos(arg) + 0.000001 * ::sin(arg);
		// line 26 of Table 5.2, period = 13.61 days
		arg = 2*f;
		dpsi += 0.002585 * ::sin(arg) - 0.000007 * ::cos(arg);
		deps -= 0.000056 * ::cos(arg) + 0.000001 * ::sin(arg);
		// line 27 of Table 5.2, period = 386.00 days
		arg = lp+o;
		dpsi += (-0.001406 - 0.000003*T) * ::sin(arg) + 0.000008 * ::cos(arg);
		deps += 0.000857 * ::cos(arg) - 0.000004 * ::sin(arg);
		// line 28 of Table 5.2, period = 31.96 days
		arg = -l+2*d+o;
		dpsi += (+0.001517 + 0.000001*T) * ::sin(arg) + 0.000001 * ::cos(arg);
		deps -= 0.000801 * ::cos(arg);
		// line 29 of Table 5.2, period = 91.31 days
		arg = 2*lp+2*f-2*d+2*o;
		dpsi += (-0.001578 + 0.000007*T) * ::sin(arg) - 0.000002 * ::cos(arg);
		deps += (+0.000685 - 0.000004*T) * ::cos(arg) - 0.000001 * ::sin(arg);
		// line 30 of Table 5.2, period = -173.31 days
		arg = -2*f+2*d;
		dpsi += 0.002178 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps -= 0.000015 * ::cos(arg) + 0.000001 * ::sin(arg);
		// line 31 of Table 5.2, period = -31.66 days
		arg = l-2*d+o;
		dpsi += (-0.001286 - 0.000001*T) * ::sin(arg) - 0.000004 * ::cos(arg);
		deps += 0.000694 * ::cos(arg) - 0.000002 * ::sin(arg);
		// line 32 of Table 5.2, period = -346.64 days
		arg = -lp+o;
		dpsi += (-0.001269 + 0.000001*T) * ::sin(arg) + 0.000006 * ::cos(arg);
		deps += (+0.000642 + 0.000001*T) * ::cos(arg) + 0.000002 * ::sin(arg);
		// line 33 of Table 5.2, period = 9.54 days
		arg = -l+2*f+2*d+o;
		dpsi += (-0.001022 - 0.000001*T) * ::sin(arg) + 0.000002 * ::cos(arg);
		deps += 0.000522 * ::cos(arg) + 0.000001 * ::sin(arg);
		// line 34 of Table 5.2, period = -182.63 days
		arg = -2*lp;
		dpsi += (-0.001671 + 0.000008*T) * ::sin(arg) - 0.000001 * ::cos(arg);
		deps += 0.000014 * ::cos(arg) - 0.000001 * ::sin(arg);
		// line 35 of Table 5.2, period = 5.64 days
		arg = l+2*f+2*d+2*o;
		dpsi -= 0.000768 * ::sin(arg) + 0.000004 * ::cos(arg);
		deps += 0.000325 * ::cos(arg) + 0.000002 * ::sin(arg);
		// line 36 of Table 5.2, period = 1095.18 days
		arg = -2*l+2*f;
		dpsi -= 0.001102 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps += 0.000010 * ::cos(arg);
/*
		// line 37 of Table 5.2, period = 13.17 days
		arg = lp+2*f+2*o;
		dpsi += (+0.000757 - 0.000002*T) * ::sin(arg) - 0.000001 * ::cos(arg);
		deps += (-0.000326 - 0.000002*T) * ::cos(arg);
		// line 38 of Table 5.2, period = 7.09 days
		arg = 2*f+2*d+o;
		dpsi += (-0.000664 - 0.000001*T) * ::sin(arg) + 0.000002 * ::cos(arg);
		deps += (+0.000335 - 0.000001*T) * ::cos(arg) + 0.000001 * ::sin(arg);
		// line 39 of Table 5.2, period = 14.19 days
		arg = -lp+2*f+2*o;
		dpsi += (-0.000714 + 0.000002*T) * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += (+0.000307 + 0.000002*T) * ::cos(arg);
		// line 40 of Table 5.2, period = 14.80 days
		arg = 2*d+o;
		dpsi += (-0.000631 - 0.000001*T) * ::sin(arg);
		deps += 0.000327 * ::cos(arg);
		// line 41 of Table 5.2, period = 23.86 days
		arg = l+2*f-2*d+o;
		dpsi += (+0.000580 + 0.000001*T) * ::sin(arg);
		deps -= 0.000307 * ::cos(arg);
		// line 42 of Table 5.2, period = 12.81 days
		arg = 2*l+2*f-2*d+2*o;
		dpsi += 0.000643 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps -= 0.000277 * ::cos(arg);
		// line 43 of Table 5.2, period = -199.84 days
		arg = -2*l+2*d+o;
		dpsi += (-0.000579 - 0.000001*T) * ::sin(arg) - 0.000001 * ::cos(arg);
		deps += 0.000304 * ::cos(arg);
		// line 44 of Table 5.2, period = 6.85 days
		arg = 2*l+2*f+o;
		dpsi -= 0.000533 * ::sin(arg) + 0.000002 * ::cos(arg);
		deps += 0.000269 * ::cos(arg) + 0.000001 * ::sin(arg);
		// line 45 of Table 5.2, period = 346.60 days
		arg = -lp+2*f-2*d+o;
		dpsi += (-0.000477 - 0.000001*T) * ::sin(arg);
		deps += (+0.000271 - 0.000001*T) * ::cos(arg);
		// line 46 of Table 5.2, period = -14.73 days
		arg = -2*d+o;
		dpsi += (-0.000493 - 0.000001*T) * ::sin(arg) - 0.000002 * ::cos(arg);
		deps += 0.000272 * ::cos(arg) - 0.000001 * ::sin(arg);
		// line 47 of Table 5.2, period = 34.85 days
		arg = -l-lp+2*d;
		dpsi += 0.000735 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps -= 0.000005 * ::cos(arg);
		// line 48 of Table 5.2, period = 212.32 days
		arg = 2*l-2*d+o;
		dpsi += 0.000405 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps -= 0.000220 * ::cos(arg);
		// line 49 of Table 5.2, period = 9.61 days
		arg = l+2*d;
		dpsi += 0.000657 * ::sin(arg) - 0.000002 * ::cos(arg);
		deps -= 0.000020 * ::cos(arg);
		// line 50 of Table 5.2, period = 119.61 days
		arg = lp+2*f-2*d+o;
		dpsi += 0.000361 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps -= 0.000194 * ::cos(arg);
		// line 51 of Table 5.2, period = 29.80 days
		arg = l-lp;
		dpsi += 0.000471 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps -= 0.000004 * ::cos(arg);
		// line 52 of Table 5.2, period = 1615.76 days
		arg = -2*l+2*f+2*o;
		dpsi -= 0.000311 * ::sin(arg);
		deps += 0.000131 * ::cos(arg);
		// line 53 of Table 5.2, period = 5.49 days
		arg = 3*l+2*f+2*o;
		dpsi -= 0.000289 * ::sin(arg) + 0.000002 * ::cos(arg);
		deps += 0.000124 * ::cos(arg) + 0.000001 * ::sin(arg);
		// line 54 of Table 5.2, period = 15.39 days
		arg = -lp+2*d;
		dpsi += 0.000435 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps -= 0.000009 * ::cos(arg);
		// line 55 of Table 5.2, period = 9.37 days
		arg = l-lp+2*f+2*o;
		dpsi -= 0.000287 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += 0.000123 * ::cos(arg);
		// line 56 of Table 5.2, period = 9.81 days
		arg = -l-lp+2*f+2*d+2*o;
		dpsi -= 0.000282 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += 0.000122 * ::cos(arg);
		// line 57 of Table 5.2, period = 29.53 days
		arg = d;
		dpsi -= 0.000422 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += 0.000003 * ::cos(arg);
		// line 58 of Table 5.2, period = 26.88 days
		arg = -l+2*f;
		dpsi -= 0.000404 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += 0.000004 * ::cos(arg);
		// line 59 of Table 5.2, period = 7.24 days
		arg = -lp+2*f+2*d+2*o;
		dpsi -= 0.000264 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += 0.000114 * ::cos(arg);
		// line 60 of Table 5.2, period = -13.75 days
		arg = -2*l+o;
		dpsi -= 0.000228 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps += 0.000126 * ::cos(arg);
		// line 61 of Table 5.2, period = 8.91 days
		arg = l+lp+2*f+2*o;
		dpsi += 0.000246 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps -= 0.000106 * ::cos(arg);
		// line 62 of Table 5.2, period = 13.81 days
		arg = 2*l+o;
		dpsi += 0.000218 * ::sin(arg);
		deps -= 0.000114 * ::cos(arg);
		// line 63 of Table 5.2, period = 3232.87 days
		arg = -l+lp+d;
		dpsi += 0.000327 * ::sin(arg);
		deps -= 0.000001 * ::cos(arg);
		// line 64 of Table 5.2, period = 25.62 days
		arg = l+lp;
		dpsi -= 0.000338 * ::sin(arg);
		deps += 0.000004 * ::cos(arg);
		// line 65 of Table 5.2, period = 9.11 days
		arg = l+2*f;
		dpsi += 0.000334 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps -= 0.000011 * ::cos(arg);
		// line 66 of Table 5.2, period = -32.61 days
		arg = -l+2*f-2*d+o;
		dpsi -= 0.000199 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps += 0.000107 * ::cos(arg);
		// line 67 of Table 5.2, period = 27.78 days
		arg = l+2*o;
		dpsi -= 0.000197 * ::sin(arg);
		deps += 0.000085 * ::cos(arg);
		// line 68 of Table 5.2, period = -411.78 days
		arg = -l+d;
		dpsi += 0.000405 * ::sin(arg) - 0.000035 * ::cos(arg);
		deps -= 0.000055 * ::cos(arg) - 0.000014 * ::sin(arg);
		// line 69 of Table 5.2, period = 9.34 days
		arg = 2*f+d+2*o;
		dpsi += 0.000165 * ::sin(arg);
		deps -= 0.000072 * ::cos(arg);
		// line 70 of Table 5.2, period = 5.80 days
		arg = -l+2*f+4*d+2*o;
		dpsi -= 0.000151 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += 0.000066 * ::cos(arg);
		// line 71 of Table 5.2, period = 6786.31 days
		arg = -2*lp+2*f-2*d+o;
		dpsi -= 0.000130 * ::sin(arg);
		deps += 0.000069 * ::cos(arg);
		// line 72 of Table 5.2, period = 6164.17 days
		arg = -l+lp+d+o;
		dpsi += 0.000132 * ::sin(arg);
		deps -= 0.000068 * ::cos(arg);
		// line 73 of Table 5.2, period = 5.64 days
		arg = l+2*f+2*d+o;
		dpsi -= 0.000133 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += 0.000066 * ::cos(arg);
		// line 74 of Table 5.2, period = 14.63 days
		arg = -2*l+2*f+2*d+2*o;
		dpsi += 0.000139 * ::sin(arg);
		deps -= 0.000060 * ::cos(arg);
		// line 75 of Table 5.2, period = -27.33 days
		arg = -l+2*o;
		dpsi += 0.000139 * ::sin(arg);
		deps -= 0.000060 * ::cos(arg);
		// line 76 of Table 5.2, period = 22.47 days
		arg = l+lp+2*f-2*d+2*o;
		dpsi += 0.000128 * ::sin(arg);
		deps -= 0.000055 * ::cos(arg);
		// line 77 of Table 5.2, period = 7.35 days
		arg = -2*l+2*f+4*d+2*o;
		dpsi -= 0.000121 * ::sin(arg);
		deps += 0.000052 * ::cos(arg);
		// line 78 of Table 5.2, period = 9.06 days
		arg = -l+4*f+2*o;
		dpsi += 0.000115 * ::sin(arg);
		deps -= 0.000049 * ::cos(arg);
		// line 79 of Table 5.2, period = 12.79 days
		arg = 2*l+2*f-2*d+o;
		dpsi += 0.000101 * ::sin(arg);
		deps -= 0.000054 * ::cos(arg);
		// line 80 of Table 5.2, period = 4.68 days
		arg = 2*l+2*f+2*d+2*o;
		dpsi -= 0.000108 * ::sin(arg) + 0.000001 * ::cos(arg);
		deps += 0.000047 * ::cos(arg);
		// line 81 of Table 5.2, period = 9.63 days
		arg = l+2*d+o;
		dpsi -= 0.000095 * ::sin(arg);
		deps += 0.000049 * ::cos(arg);
		// line 82 of Table 5.2, period = 9.18 days
		arg = 3*l;
		dpsi += 0.000157 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps -= 0.000005 * ::cos(arg);
		// line 83 of Table 5.2, period = 8.75 days
		arg = 3*l+2*f-2*d+2*o;
		dpsi += 0.000094 * ::sin(arg);
		deps -= 0.000040 * ::cos(arg);
		// line 84 of Table 5.2, period = 12.66 days
		arg = 4*f-2*d+2*o;
		dpsi += 0.000091 * ::sin(arg);
		deps -= 0.000039 * ::cos(arg);
		// line 85 of Table 5.2, period = -169.00 days
		arg = -2*f+2*d+o;
		dpsi += 0.000087 * ::sin(arg);
		deps -= 0.000044 * ::cos(arg);
		// line 86 of Table 5.2, period = 13.14 days
		arg = lp+2*f+o;
		dpsi += 0.000081 * ::sin(arg);
		deps -= 0.000042 * ::cos(arg);
		// line 87 of Table 5.2, period = 187.66 days
		arg = 2*f-2*d+3*o;
		dpsi += 0.000123 * ::sin(arg);
		deps -= 0.000020 * ::cos(arg);
		// line 88 of Table 5.2, period = 10.08 days
		arg = -l+4*d;
		dpsi += 0.000133 * ::sin(arg);
		deps -= 0.000004 * ::cos(arg);
		// line 89 of Table 5.2, period = -943.23 days
		arg = 2*l-2*f+o;
		dpsi += 0.000071 * ::sin(arg);
		deps -= 0.000038 * ::cos(arg);
		// line 90 of Table 5.2, period = -15.91 days
		arg = 2*l-4*d;
		dpsi -= 0.000128 * ::sin(arg);
		deps += 0.000001 * ::cos(arg);
		// line 91 of Table 5.2, period = 35.03 days
		arg = -l-lp+2*d+o;
		dpsi += 0.000075 * ::sin(arg);
		deps -= 0.000039 * ::cos(arg);
		// line 92 of Table 5.2, period = -131.67 days
		arg = -2*l-lp+2*d;
		dpsi -= 0.000115 * ::sin(arg);
		deps += 0.000001 * ::cos(arg);
		// line 93 of Table 5.2, period = 14.16 days
		arg = -lp+2*f+o;
		dpsi -= 0.000066 * ::sin(arg);
		deps += 0.000035 * ::cos(arg);
		// line 94 of Table 5.2, period = -388.27 days
		arg = -l+d+o;
		dpsi += 0.000101 * ::sin(arg) - 0.000003 * ::cos(arg);
		deps -= 0.000049 * ::cos(arg) - 0.000001 * ::sin(arg);
		// line 95 of Table 5.2, period = -13.58 days
		arg = -2*f+o;
		dpsi -= 0.000068 * ::sin(arg);
		deps += 0.000036 * ::cos(arg);
		// line 96 of Table 5.2, period = 409.23 days
		arg = lp+2*o;
		dpsi += 0.000069 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps -= 0.000033 * ::cos(arg);
		// line 97 of Table 5.2, period = 25.42 days
		arg = 2*f-d+2*o;
		dpsi -= 0.000074 * ::sin(arg);
		deps += 0.000031 * ::cos(arg);
		// line 98 of Table 5.2, period = 4.79 days
		arg = 2*f+4*d+2*o;
		dpsi -= 0.000069 * ::sin(arg);
		deps += 0.000029 * ::cos(arg);
		// line 99 of Table 5.2, period = -34.67 days
		arg = l+lp-2*d+o;
		dpsi -= 0.000061 * ::sin(arg);
		deps += 0.000032 * ::cos(arg);
		// line 100 of Table 5.2, period = 29.26 days
		arg = -l+lp+2*d;
		dpsi -= 0.000094 * ::sin(arg);
		// line 101 of Table 5.2, period = 5.73 days
		arg = l-lp+2*f+2*d+2*o;
		dpsi -= 0.000059 * ::sin(arg);
		deps += 0.000025 * ::cos(arg);
		// line 102 of Table 5.2, period = 29.93 days
		arg = l-lp+o;
		dpsi += 0.000051 * ::sin(arg);
		deps -= 0.000027 * ::cos(arg);
		// line 103 of Table 5.2, period = -329.79 days
		arg = lp-2*f+2*d;
		dpsi -= 0.000090 * ::sin(arg);
		deps += 0.000003 * ::cos(arg);
		// line 104 of Table 5.2, period = 5.49 days
		arg = 3*l+2*f+o;
		dpsi -= 0.000050 * ::sin(arg);
		deps += 0.000025 * ::cos(arg);
		// line 105 of Table 5.2, period = 9.31 days
		arg = -l+lp+2*f+2*d+2*o;
		dpsi += 0.000056 * ::sin(arg);
		deps -= 0.000024 * ::cos(arg);
		// line 106 of Table 5.2, period = 6.96 days
		arg = lp+2*f+2*d+2*o;
		dpsi += 0.000054 * ::sin(arg);
		deps -= 0.000022 * ::cos(arg);
		// line 107 of Table 5.2, period = -9.60 days
		arg = -l-2*d+o;
		dpsi -= 0.000050 * ::sin(arg);
		deps += 0.000027 * ::cos(arg);
		// line 108 of Table 5.2, period = 66079.30 days
		arg = -l+lp+d+2*o;
		dpsi -= 0.000052 * ::sin(arg);
		deps += 0.000023 * ::cos(arg);
		// line 109 of Table 5.2, period = 7.23 days
		arg = -lp+2*f+2*d+o;
		dpsi -= 0.000044 * ::sin(arg);
		deps += 0.000024 * ::cos(arg);
		// line 110 of Table 5.2, period = -38.74 days
		arg = l+2*f-4*d+o;
		dpsi -= 0.000047 * ::sin(arg);
		deps += 0.000024 * ::cos(arg);
		// line 111 of Table 5.2, period = -23.77 days
		arg = -l-2*f+2*d;
		dpsi += 0.000077 * ::sin(arg);
		// line 112 of Table 5.2, period = 9.80 days
		arg = -l-lp+2*f+2*d+o;
		dpsi -= 0.000046 * ::sin(arg);
		deps += 0.000024 * ::cos(arg);
		// line 113 of Table 5.2, period = -329.82 days
		arg = -lp+2*o;
		dpsi += 0.000059 * ::sin(arg);
		deps -= 0.000025 * ::cos(arg);
		// line 114 of Table 5.2, period = 6.99 days
		arg = 2*l-lp+2*f+2*o;
		dpsi -= 0.000048 * ::sin(arg);
		deps += 0.000021 * ::cos(arg);
		// line 115 of Table 5.2, period = 9.35 days
		arg = l-lp+2*f+o;
		dpsi -= 0.000042 * ::sin(arg);
		deps += 0.000022 * ::cos(arg);
		// line 116 of Table 5.2, period = 14.83 days
		arg = 2*d+2*o;
		dpsi -= 0.000046 * ::sin(arg);
		deps += 0.000020 * ::cos(arg);
		// line 117 of Table 5.2, period = 14.19 days
		arg = lp+2*d;
		dpsi -= 0.000067 * ::sin(arg);
		// line 118 of Table 5.2, period = 25.22 days
		arg = -l+lp+2*f+2*o;
		dpsi += 0.000047 * ::sin(arg);
		deps -= 0.000020 * ::cos(arg);
		// line 119 of Table 5.2, period = 73.05 days
		arg = 3*lp+2*f-2*d+2*o;
		dpsi -= 0.000044 * ::sin(arg);
		deps += 0.000019 * ::cos(arg);
		// line 120 of Table 5.2, period = -117.54 days
		arg = -lp-2*f+2*d;
		dpsi += 0.000066 * ::sin(arg);
		// line 121 of Table 5.2, period = 29.66 days
		arg = d+o;
		dpsi -= 0.000037 * ::sin(arg);
		deps += 0.000020 * ::cos(arg);
		// line 122 of Table 5.2, period = -9.53 days
		arg = l-2*f-2*d;
		dpsi -= 0.000064 * ::sin(arg);
		deps += 0.000001 * ::cos(arg);
		// line 123 of Table 5.2, period = 8.90 days
		arg = l+lp+2*f+o;
		dpsi += 0.000036 * ::sin(arg);
		deps -= 0.000018 * ::cos(arg);
		// line 124 of Table 5.2, period = 6.73 days
		arg = 2*l+lp+2*f+2*o;
		dpsi += 0.000040 * ::sin(arg);
		deps -= 0.000017 * ::cos(arg);
		// line 125 of Table 5.2, period = 27.32 days
		arg = lp+d;
		dpsi += 0.000057 * ::sin(arg);
		// line 126 of Table 5.2, period = 32.76 days
		arg = l-2*f+2*d;
		dpsi -= 0.000058 * ::sin(arg);
		// line 127 of Table 5.2, period = 25.72 days
		arg = l+lp+o;
		dpsi -= 0.000034 * ::sin(arg);
		deps += 0.000019 * ::cos(arg);
		// line 128 of Table 5.2, period = -7.13 days
		arg = -2*l-2*d;
		dpsi -= 0.000059 * ::sin(arg);
		deps += 0.000001 * ::cos(arg);
		// line 129 of Table 5.2, period = 32.11 days
		arg = -l+2*d+2*o;
		dpsi -= 0.000038 * ::sin(arg);
		deps += 0.000017 * ::cos(arg);
		// line 130 of Table 5.2, period = -29.40 days
		arg = -d+o;
		dpsi += 0.000033 * ::sin(arg);
		deps -= 0.000018 * ::cos(arg);
		// line 131 of Table 5.2, period = -15.35 days
		arg = lp-2*d+o;
		dpsi -= 0.000033 * ::sin(arg);
		deps += 0.000018 * ::cos(arg);
		// line 132 of Table 5.2, period = -32.45 days
		arg = -l+2*f-2*d+2*o;
		dpsi += 0.000036 * ::sin(arg);
		deps -= 0.000016 * ::cos(arg);
		// line 133 of Table 5.2, period = -29.67 days
		arg = -l+lp+o;
		dpsi -= 0.000031 * ::sin(arg);
		deps += 0.000017 * ::cos(arg);
		// line 134 of Table 5.2, period = 6.98 days
		arg = l+2*f+d+2*o;
		dpsi += 0.000033 * ::sin(arg);
		deps -= 0.000014 * ::cos(arg);
		// line 135 of Table 5.2, period = -7.38 days
		arg = -4*d;
		dpsi -= 0.000048 * ::sin(arg);
		deps += 0.000001 * ::cos(arg);
		// line 136 of Table 5.2, period = 9.33 days
		arg = 2*f+d+o;
		dpsi += 0.000027 * ::sin(arg);
		deps -= 0.000014 * ::cos(arg);
		// line 137 of Table 5.2, period = -31.52 days
		arg = l-2*d+2*o;
		dpsi += 0.000032 * ::sin(arg);
		deps -= 0.000014 * ::cos(arg);
		// line 138 of Table 5.2, period = 13.22 days
		arg = l+2*f-d+2*o;
		dpsi -= 0.000033 * ::sin(arg);
		deps += 0.000013 * ::cos(arg);
		// line 139 of Table 5.2, period = 9.87 days
		arg = l-lp+2*d;
		dpsi += 0.000048 * ::sin(arg);
		// line 140 of Table 5.2, period = 5.80 days
		arg = -l+2*f+4*d+o;
		dpsi -= 0.000026 * ::sin(arg);
		deps += 0.000013 * ::cos(arg);
		// line 141 of Table 5.2, period = -7.08 days
		arg = -2*f-2*d;
		dpsi -= 0.000041 * ::sin(arg);
		deps += 0.000001 * ::cos(arg);
		// line 142 of Table 5.2, period = -26.77 days
		arg = l-2*f+o;
		dpsi += 0.000027 * ::sin(arg);
		deps -= 0.000014 * ::cos(arg);
		// line 143 of Table 5.2, period = 313.04 days
		arg = -l+2*f-d+o;
		dpsi -= 0.000023 * ::sin(arg);
		deps += 0.000014 * ::cos(arg);
		// line 144 of Table 5.2, period = 22.40 days
		arg = l+lp+2*f-2*d+o;
		dpsi += 0.000023 * ::sin(arg);
		deps -= 0.000012 * ::cos(arg);
		// line 145 of Table 5.2, period = 4.58 days
		arg = 4*l+2*f+2*o;
		dpsi -= 0.000026 * ::sin(arg);
		deps += 0.000011 * ::cos(arg);
		// line 146 of Table 5.2, period = 9.11 days
		arg = lp+2*f+d+2*o;
		dpsi -= 0.000024 * ::sin(arg);
		deps += 0.000010 * ::cos(arg);
		// line 147 of Table 5.2, period = -6.85 days
		arg = -2*l-2*f;
		dpsi -= 0.000036 * ::sin(arg);
		deps += 0.000001 * ::cos(arg);
		// line 148 of Table 5.2, period = 12.38 days
		arg = 2*l+lp+2*f-2*d+2*o;
		dpsi += 0.000025 * ::sin(arg);
		deps -= 0.000010 * ::cos(arg);
		// line 149 of Table 5.2, period = 14.32 days
		arg = 2*l-lp;
		dpsi += 0.000038 * ::sin(arg);
		// line 150 of Table 5.2, period = -25.53 days
		arg = -l-lp+o;
		dpsi += 0.000021 * ::sin(arg);
		deps -= 0.000012 * ::cos(arg);
		// line 151 of Table 5.2, period = 14.60 days
		arg = -2*l+2*f+2*d+o;
		dpsi += 0.000022 * ::sin(arg);
		deps -= 0.000011 * ::cos(arg);
		// line 152 of Table 5.2, period = -2266.12 days
		arg = 3*o;
		dpsi -= 0.000022 * ::sin(arg);
		deps += 0.000010 * ::cos(arg);
		// line 153 of Table 5.2, period = 8.68 days
		arg = l+4*f-2*d+2*o;
		dpsi += 0.000023 * ::sin(arg);
		deps -= 0.000009 * ::cos(arg);
		// line 154 of Table 5.2, period = 4.68 days
		arg = 2*l+2*f+2*d+o;
		dpsi -= 0.000019 * ::sin(arg);
		deps += 0.000010 * ::cos(arg);
		// line 155 of Table 5.2, period = 7.34 days
		arg = -2*l+2*f+4*d+o;
		dpsi -= 0.000020 * ::sin(arg);
		deps += 0.000010 * ::cos(arg);
		// line 156 of Table 5.2, period = 14.22 days
		arg = lp+2*d+o;
		dpsi += 0.000018 * ::sin(arg);
		deps -= 0.000009 * ::cos(arg);
		// line 157 of Table 5.2, period = 14.25 days
		arg = l+d;
		dpsi -= 0.000033 * ::sin(arg);
		// line 158 of Table 5.2, period = 10.10 days
		arg = -l+4*d+o;
		dpsi -= 0.000018 * ::sin(arg);
		deps += 0.000009 * ::cos(arg);
		// line 159 of Table 5.2, period = 9.05 days
		arg = -l+4*f+o;
		dpsi += 0.000019 * ::sin(arg);
		deps -= 0.000009 * ::cos(arg);
		// line 160 of Table 5.2, period = -35.23 days
		arg = 2*f-3*d+2*o;
		dpsi -= 0.000020 * ::sin(arg);
		deps += 0.000008 * ::cos(arg);
		// line 161 of Table 5.2, period = 6.82 days
		arg = 4*f+2*o;
		dpsi += 0.000019 * ::sin(arg);
		deps -= 0.000008 * ::cos(arg);
		// line 162 of Table 5.2, period = 13.28 days
		arg = 2*l+lp;
		dpsi -= 0.000028 * ::sin(arg);
		// line 163 of Table 5.2, period = -16.10 days
		arg = 2*f-4*d+o;
		dpsi -= 0.000016 * ::sin(arg);
		deps += 0.000009 * ::cos(arg);
		// line 164 of Table 5.2, period = 5.90 days
		arg = -l-lp+2*f+4*d+2*o;
		dpsi -= 0.000017 * ::sin(arg);
		deps += 0.000007 * ::cos(arg);
		// line 165 of Table 5.2, period = 38.52 days
		arg = -l-2*lp+2*d;
		dpsi += 0.000027 * ::sin(arg);
		// line 166 of Table 5.2, period = 7.39 days
		arg = 4*d+o;
		dpsi -= 0.000016 * ::sin(arg);
		deps += 0.000007 * ::cos(arg);
		// line 167 of Table 5.2, period = 15.42 days
		arg = -lp+2*d+o;
		dpsi -= 0.000014 * ::sin(arg);
		deps += 0.000007 * ::cos(arg);
		// line 168 of Table 5.2, period = 4.08 days
		arg = l+2*f+4*d+2*o;
		dpsi -= 0.000016 * ::sin(arg);
		deps += 0.000007 * ::cos(arg);
		// line 169 of Table 5.2, period = -194.13 days
		arg = -2*l+2*d+2*o;
		dpsi += 0.000018 * ::sin(arg);
		deps -= 0.000008 * ::cos(arg);
		// line 170 of Table 5.2, period = 1616.44 days
		arg = -2*l+2*lp+2*d;
		dpsi -= 0.000022 * ::sin(arg);
		// line 171 of Table 5.2, period = -507.16 days
		arg = -2*l-lp+2*f+o;
		dpsi += 0.000009 * ::sin(arg);
		deps -= 0.000005 * ::cos(arg);
		// line 172 of Table 5.2, period = -9.17 days
		arg = -3*l+o;
		dpsi -= 0.000014 * ::sin(arg);
		deps += 0.000007 * ::cos(arg);
		// line 173 of Table 5.2, period = 13.69 days
		arg = 2*f+3*o;
		dpsi += 0.000020 * ::sin(arg);
		// line 174 of Table 5.2, period = 4.79 days
		arg = 2*f+4*d+o;
		dpsi -= 0.000012 * ::sin(arg);
		deps += 0.000006 * ::cos(arg);
		// line 175 of Table 5.2, period = 12.64 days
		arg = 4*f-2*d+o;
		dpsi += 0.000012 * ::sin(arg);
		deps -= 0.000007 * ::cos(arg);
		// line 176 of Table 5.2, period = 16.06 days
		arg = -2*lp+2*d;
		dpsi += 0.000021 * ::sin(arg);
		// line 177 of Table 5.2, period = 438.33 days
		arg = l-d+o;
		dpsi += 0.000017 * ::sin(arg) - 0.000003 * ::cos(arg);
		deps -= 0.000005 * ::cos(arg) + 0.000001 * ::sin(arg);
		// line 178 of Table 5.2, period = 5.56 days
		arg = l+lp+2*f+2*d+2*o;
		dpsi += 0.000015 * ::sin(arg);
		deps -= 0.000006 * ::cos(arg);
		// line 179 of Table 5.2, period = 8.73 days
		arg = 3*l+2*f-2*d+o;
		dpsi += 0.000012 * ::sin(arg);
		deps -= 0.000007 * ::cos(arg);
		// line 180 of Table 5.2, period = 29.26 days
		arg = -l-lp+2*f+2*o;
		dpsi -= 0.000016 * ::sin(arg);
		deps += 0.000006 * ::cos(arg);
		// line 181 of Table 5.2, period = -129.17 days
		arg = -2*l-lp+2*d+o;
		dpsi -= 0.000013 * ::sin(arg);
		deps += 0.000007 * ::cos(arg);
		// line 182 of Table 5.2, period = -14.70 days
		arg = -2*d+2*o;
		dpsi += 0.000013 * ::sin(arg);
		deps -= 0.000005 * ::cos(arg);
		// line 183 of Table 5.2, period = 7.38 days
		arg = -2*lp+2*f+2*d+2*o;
		dpsi -= 0.000013 * ::sin(arg);
		deps += 0.000005 * ::cos(arg);
		// line 184 of Table 5.2, period = -10.07 days
		arg = l-4*d+o;
		dpsi -= 0.000012 * ::sin(arg);
		deps += 0.000006 * ::cos(arg);
		// line 185 of Table 5.2, period = 29.39 days
		arg = -l+lp+2*d+o;
		dpsi -= 0.000010 * ::sin(arg);
		deps += 0.000006 * ::cos(arg);
		// line 186 of Table 5.2, period = 15.94 days
		arg = -2*l+4*d+o;
		dpsi += 0.000011 * ::sin(arg);
		deps -= 0.000006 * ::cos(arg);
		// line 187 of Table 5.2, period = 25.33 days
		arg = 2*f-d+o;
		dpsi -= 0.000010 * ::sin(arg);
		deps += 0.000005 * ::cos(arg);
		// line 188 of Table 5.2, period = 187.67 days
		arg = 2*lp+o;
		dpsi -= 0.000009 * ::sin(arg);
		deps += 0.000005 * ::cos(arg);
		// line 189 of Table 5.2, period = 90.10 days
		arg = 2*lp+2*f-2*d+o;
		dpsi += 0.000008 * ::sin(arg);
		deps -= 0.000005 * ::cos(arg);
		// line 190 of Table 5.2, period = 7.13 days
		arg = 2*l+2*d+o;
		dpsi -= 0.000009 * ::sin(arg);
		deps += 0.000005 * ::cos(arg);
		// line 191 of Table 5.2, period = -15.87 days
		arg = 2*l-4*d+o;
		dpsi -= 0.000011 * ::sin(arg);
		deps += 0.000005 * ::cos(arg);
		// line 192 of Table 5.2, period = 95.42 days
		arg = 2*l+2*f-4*d+o;
		dpsi += 0.000010 * ::sin(arg);
		deps -= 0.000005 * ::cos(arg);
		// line 193 of Table 5.2, period = -9.10 days
		arg = -l-2*f+o;
		dpsi -= 0.000010 * ::sin(arg);
		deps += 0.000005 * ::cos(arg);
		// line 194 of Table 5.2, period = 25.13 days
		arg = -l+lp+2*f+o;
		dpsi += 0.000009 * ::sin(arg);
		deps -= 0.000005 * ::cos(arg);
		// line 195 of Table 5.2, period = -35.80 days
		arg = -l+lp+2*f-2*d+o;
		dpsi -= 0.000011 * ::sin(arg);
		deps += 0.000005 * ::cos(arg);
		// line 196 of Table 5.2, period = 10.37 days
		arg = -l-lp+4*d;
		dpsi += 0.000015 * ::sin(arg);
		// line 197 of Table 5.2, period = 37.63 days
		arg = -3*l+4*d;
		dpsi += 0.000016 * ::sin(arg);
		// line 198 of Table 5.2, period = 4.00 days
		arg = 3*l+2*f+2*d+2*o;
		dpsi -= 0.000014 * ::sin(arg);
		// line 199 of Table 5.2, period = 471.89 days
		arg = 2*l-lp-2*d;
		dpsi -= 0.000009 * ::sin(arg) - 0.000001 * ::cos(arg);
		deps += 0.000001 * ::cos(arg);
		// line 200 of Table 5.2, period = -3396.16 days
		arg = 2*lp-2*f+2*d;
		dpsi -= 0.000009 * ::sin(arg);
		// line 201 of Table 5.2, period = 4.86 days
		arg = -lp+2*f+4*d+2*o;
		dpsi -= 0.000009 * ::sin(arg);
		// line 202 of Table 5.2, period = 27.32 days
		arg = -lp+2*f-d+2*o;
		dpsi += 0.000009 * ::sin(arg);
		// line 203 of Table 5.2, period = 9.37 days
		arg = l+lp+2*d;
		dpsi -= 0.000010 * ::sin(arg);
		// line 204 of Table 5.2, period = 219.17 days
		arg = 2*l-2*d+2*o;
		dpsi -= 0.000011 * ::sin(arg);
		// line 205 of Table 5.2, period = 4.74 days
		arg = 2*l-lp+2*f+2*d+2*o;
		dpsi -= 0.000009 * ::sin(arg);
		// line 206 of Table 5.2, period = 6.89 days
		arg = 4*l;
		dpsi += 0.000009 * ::sin(arg);
		// line 207 of Table 5.2, period = 6.64 days
		arg = 4*l+2*f-2*d+2*o;
		dpsi += 0.000012 * ::sin(arg);
		// line 208 of Table 5.2, period = 15.31 days
		arg = -l+3*d;
		dpsi -= 0.000010 * ::sin(arg);
		// line 209 of Table 5.2, period = 23.43 days
		arg = -l+4*f-2*d+2*o;
		dpsi -= 0.000009 * ::sin(arg);
		// line 210 of Table 5.2, period = 10.08 days
		arg = -l-2*lp+2*f+2*d+2*o;
		dpsi -= 0.000009 * ::sin(arg);
		// line 211 of Table 5.2, period = 16.63 days
		arg = -2*l-lp+4*d;
		dpsi += 0.000012 * ::sin(arg);
		// line 212 of Table 5.2, period = 7.50 days
		arg = -2*l-lp+2*f+4*d+2*o;
		dpsi -= 0.000012 * ::sin(arg);
		// line 213 of Table 5.2, period = 6.95 days
		arg = lp+2*f+2*d+o;
		dpsi += 0.000007 * ::sin(arg);
		// line 214 of Table 5.2, period = 12.71 days
		arg = 2*lp+2*f+2*o;
		dpsi += 0.000007 * ::sin(arg);
		// line 215 of Table 5.2, period = 14.77 days
		arg = -2*lp+2*f+2*o;
		dpsi -= 0.000008 * ::sin(arg);
		// line 216 of Table 5.2, period = 5.82 days
		arg = l+4*d;
		dpsi += 0.000008 * ::sin(arg);
		// line 217 of Table 5.2, period = 5.63 days
		arg = l+2*f+2*d;
		dpsi += 0.000008 * ::sin(arg);
		// line 218 of Table 5.2, period = -38.52 days
		arg = l+2*f-4*d+2*o;
		dpsi += 0.000007 * ::sin(arg);
		// line 219 of Table 5.2, period = 5.73 days
		arg = l-lp+2*f+2*d+o;
		dpsi -= 0.000008 * ::sin(arg);
		// line 220 of Table 5.2, period = 25.62 days
		arg = l-lp+2*f-2*d+2*o;
		dpsi -= 0.000007 * ::sin(arg);
		// line 221 of Table 5.2, period = 32.45 days
		arg = l-2*lp;
		dpsi += 0.000008 * ::sin(arg);
		// line 222 of Table 5.2, period = 13.83 days
		arg = 2*l+2*o;
		dpsi -= 0.000008 * ::sin(arg);
		// line 223 of Table 5.2, period = 134.27 days
		arg = 2*l+lp-2*d+o;
		dpsi += 0.000008 * ::sin(arg);
		// line 224 of Table 5.2, period = 9.20 days
		arg = 3*l+o;
		dpsi += 0.000007 * ::sin(arg);
		// line 225 of Table 5.2, period = 14.13 days
		arg = -l+2*f+d+2*o;
		dpsi += 0.000008 * ::sin(arg);
		// line 226 of Table 5.2, period = 7.22 days
		arg = -l+2*f+3*d+2*o;
		dpsi += 0.000008 * ::sin(arg);
		// line 227 of Table 5.2, period = 38.96 days
		arg = -l-2*f+4*d;
		dpsi -= 0.000007 * ::sin(arg);
		// line 228 of Table 5.2, period = 9.30 days
		arg = -l+lp+2*f+2*d+o;
		dpsi += 0.000007 * ::sin(arg);
		// line 229 of Table 5.2, period = 27.09 days
		arg = -l+2*lp+2*d;
		dpsi -= 0.000008 * ::sin(arg);
		// line 230 of Table 5.2, period = 2189.73 days
		arg = -l-lp+2*f-d+o;
		dpsi += 0.000007 * ::sin(arg);
		// line 231 of Table 5.2, period = -14.93 days
		arg = -2*l+2*f-2*d+o;
		dpsi -= 0.000008 * ::sin(arg);
		// line 232 of Table 5.2, period = 13.49 days
		arg = -2*l+4*f+2*o;
		dpsi -= 0.000007 * ::sin(arg);
		// line 233 of Table 5.2, period = -12.76 days
		arg = -2*l-2*f+2*d;
		dpsi += 0.000008 * ::sin(arg);
		// line 234 of Table 5.2, period = 285.41 days
		arg = -2*l+lp+2*f+o;
		dpsi += 0.000009 * ::sin(arg);
		// line 235 of Table 5.2, period = -28.15 days
		arg = -3*l+2*f+o;
		dpsi -= 0.000008 * ::sin(arg);
		// line 236 of Table 5.2, period = 27.43 days
		arg = lp+d+o;
		dpsi += 0.000005 * ::sin(arg);
		// line 237 of Table 5.2, period = 7.53 days
		arg = -lp+4*d;
		dpsi += 0.000006 * ::sin(arg);
		// line 238 of Table 5.2, period = -14.16 days
		arg = -lp-2*d+o;
		dpsi += 0.000005 * ::sin(arg);
		// line 239 of Table 5.2, period = -177.85 days
		arg = -2*lp+o;
		dpsi -= 0.000006 * ::sin(arg);
		// line 240 of Table 5.2, period = 6.97 days
		arg = l+2*f+d+o;
		dpsi += 0.000005 * ::sin(arg);
		// line 241 of Table 5.2, period = 126.51 days
		arg = l+2*f-3*d+2*o;
		dpsi -= 0.000006 * ::sin(arg);
		// line 242 of Table 5.2, period = -299.26 days
		arg = l-2*f+d;
		dpsi -= 0.000007 * ::sin(arg);
		// line 243 of Table 5.2, period = 13.72 days
		arg = l+lp+d;
		dpsi += 0.000005 * ::sin(arg);
		// line 244 of Table 5.2, period = -29.14 days
		arg = l-lp-2*d+o;
		dpsi += 0.000006 * ::sin(arg);
		// line 245 of Table 5.2, period = 8.93 days
		arg = 2*l+2*f-d+2*o;
		dpsi -= 0.000006 * ::sin(arg);
		// line 246 of Table 5.2, period = 6.73 days
		arg = 2*l+lp+2*f+o;
		dpsi += 0.000005 * ::sin(arg);
		// line 247 of Table 5.2, period = 6.98 days
		arg = 2*l-lp+2*f+o;
		dpsi -= 0.000006 * ::sin(arg);
		// line 248 of Table 5.2, period = 13.28 days
		arg = 2*l-lp+2*f-2*d+2*o;
		dpsi += 0.000005 * ::sin(arg);
		// line 249 of Table 5.2, period = 5.66 days
		arg = 3*l+2*d;
		dpsi += 0.000005 * ::sin(arg);
		// line 250 of Table 5.2, period = 5.58 days
		arg = 3*l-lp+2*f+2*o;
		dpsi -= 0.000005 * ::sin(arg);
		// line 251 of Table 5.2, period = 29.14 days
		arg = -l-lp+2*f+o;
		dpsi -= 0.000006 * ::sin(arg);
		// line 252 of Table 5.2, period = -13.72 days
		arg = -2*l+2*o;
		dpsi += 0.000006 * ::sin(arg);
		// line 253 of Table 5.2, period = 34.48 days
		arg = -2*l+3*d;
		dpsi -= 0.000005 * ::sin(arg);
		// line 254 of Table 5.2, period = -7.12 days
		arg = -2*l-2*d+o;
		dpsi -= 0.000005 * ::sin(arg);
		// line 255 of Table 5.2, period = 14.57 days
		arg = -2*l+2*f+2*d;
		dpsi -= 0.000006 * ::sin(arg);
		// line 256 of Table 5.2, period = -548.04 days
		arg = -2*l-lp+2*f;
		dpsi -= 0.000005 * ::sin(arg);
		// line 257 of Table 5.2, period = 15.24 days
		arg = -2*l-lp+2*f+2*d+2*o;
		dpsi += 0.000006 * ::sin(arg);
		// line 258 of Table 5.2, period = 27.21 days
		arg = f;
		dpsi  += 0.000008 * ::cos(arg);
		// line 259 of Table 5.2, period = 27.32 days
		arg = f+o;
		dpsi  -= 0.000016 * ::cos(arg);
		deps  -= 0.000014 * ::sin(arg);
		// line 260 of Table 5.2, period = 2190.35 days
		arg = -l+f;
		dpsi  += 0.000033 * ::cos(arg);
		// line 261 of Table 5.2, period = 3231.51 days
		arg = -l+f+o;
		dpsi  -= 0.000105 * ::cos(arg);
		deps  -= 0.000089 * ::sin(arg);
		// line 262 of Table 5.2, period = 6159.22 days
		arg = -l+f+2*o;
		dpsi  += 0.000036 * ::cos(arg);
		deps  += 0.000018 * ::sin(arg);
		// line 263 of Table 5.2, period = 65514.10 days
		arg = -l+f+3*o;
		dpsi  -= 0.000006 * ::cos(arg);
*/
		//End Code implementing Table 5.2 IERS Conventions 1996 nutation series.
		//-----------------------------------------------------------------------
   }

   //------------------------------------------------------------------------------
   // Zonal tide terms for corrections of UT1mUTC when that quantity does not
   // include tides (e.g. NGA EOP), ref. IERS 1996 Ch. 8, table 8.1 pg 74.
   // @param T,    the coordinate transformation time at the time of interest
   // @param UT1mUT1R, the correction to UT1mUTC (seconds)
   // @param dlodR, the correction to the length of day (seconds)
   // @param domegaR, the correction to the Earth rotation rate, rad/second.
   void GeodeticFrames::UT1mUTCTidalCorrections(double T,
                                                double& UT1mUT1R,
                                                double& dlodR,
                                                double& domegaR)
         throw()
   {
		//-----------------------------------------------------------------------
		// Code to implement Table 8.1 of IERS Conventions 1996 series for
		// Zonal Tide terms for UT1R, length of the day and omega(Earth).
		// Units for UT1R, length of the day and omega(Earth) are:
		//        seconds,    seconds        and radians/sec.
		// (Generated by perl script Table81.pl)
		double arg;
		UT1mUT1R = dlodR = domegaR = 0.0;
	
		// Define (all doubles) and all in RADIANS:
		double o = Omega(T); // = mean longitude of lunar ascending node, in degrees,
		double f = F(T)    ; // = mean longitude of the moon - Omega, in degrees,
		double d = D(T)    ; // = mean elongation of the moon from the sun, in degrees,
		double l = L(T)    ; // = mean anomaly of the moon, in degrees, and
		double lp = Lp(T)  ; // = mean anomaly of the sun, in degrees.
	   o *= DEG_TO_RAD;
	   f *= DEG_TO_RAD;
	   d *= DEG_TO_RAD;
	   l *= DEG_TO_RAD;
	   lp *= DEG_TO_RAD;
	
		// line 1 of Table 8.1, period = 5.64 days
		arg = l+2*f+2*d+2*o;
		UT1mUT1R -= 0.02e-4 * ::sin(arg);
		dlodR += 0.3e-5 * ::cos(arg);
		domegaR -= 0.2e-14 * ::cos(arg);
		// line 2 of Table 8.1, period = 6.85 days
		arg = 2*l+2*f+o;
		UT1mUT1R -= 0.04e-4 * ::sin(arg);
		dlodR += 0.4e-5 * ::cos(arg);
		domegaR -= 0.3e-14 * ::cos(arg);
		// line 3 of Table 8.1, period = 6.86 days
		arg = 2*l+2*f+2*o;
		UT1mUT1R -= 0.10e-4 * ::sin(arg);
		dlodR += 0.9e-5 * ::cos(arg);
		domegaR -= 0.8e-14 * ::cos(arg);
		// line 4 of Table 8.1, period = 7.09 days
		arg = 2*f+2*d+o;
		UT1mUT1R -= 0.05e-4 * ::sin(arg);
		dlodR += 0.4e-5 * ::cos(arg);
		domegaR -= 0.4e-14 * ::cos(arg);
		// line 5 of Table 8.1, period = 7.10 days
		arg = 2*f+2*d+2*o;
		UT1mUT1R -= 0.12e-4 * ::sin(arg);
		dlodR += 1.1e-5 * ::cos(arg);
		domegaR -= 0.9e-14 * ::cos(arg);
		// line 6 of Table 8.1, period = 9.11 days
		arg = l+2*f;
		UT1mUT1R -= 0.04e-4 * ::sin(arg);
		dlodR += 0.3e-5 * ::cos(arg);
		domegaR -= 0.2e-14 * ::cos(arg);
		// line 7 of Table 8.1, period = 9.12 days
		arg = l+2*f+o;
		UT1mUT1R -= 0.41e-4 * ::sin(arg);
		dlodR += 2.8e-5 * ::cos(arg);
		domegaR -= 2.4e-14 * ::cos(arg);
		// line 8 of Table 8.1, period = 9.13 days
		arg = l+2*f+2*o;
		UT1mUT1R -= 0.99e-4 * ::sin(arg);
		dlodR += 6.8e-5 * ::cos(arg);
		domegaR -= 5.8e-14 * ::cos(arg);
		// line 9 of Table 8.1, period = 9.18 days
		arg = 3*l;
		UT1mUT1R -= 0.02e-4 * ::sin(arg);
		dlodR += 0.1e-5 * ::cos(arg);
		domegaR -= 0.1e-14 * ::cos(arg);
		// line 10 of Table 8.1, period = 9.54 days
		arg = -l+2*f+2*d+o;
		UT1mUT1R -= 0.08e-4 * ::sin(arg);
		dlodR += 0.5e-5 * ::cos(arg);
		domegaR -= 0.5e-14 * ::cos(arg);
		// line 11 of Table 8.1, period = 9.56 days
		arg = -l+2*f+2*d+2*o;
		UT1mUT1R -= 0.20e-4 * ::sin(arg);
		dlodR += 1.3e-5 * ::cos(arg);
		domegaR -= 1.1e-14 * ::cos(arg);
		// line 12 of Table 8.1, period = 9.61 days
		arg = l+2*d;
		UT1mUT1R -= 0.08e-4 * ::sin(arg);
		dlodR += 0.5e-5 * ::cos(arg);
		domegaR -= 0.4e-14 * ::cos(arg);
		// line 13 of Table 8.1, period = 12.81 days
		arg = 2*l+2*f-2*d+2*o;
		UT1mUT1R += 0.02e-4 * ::sin(arg);
		dlodR -= 0.1e-5 * ::cos(arg);
		domegaR += 0.1e-14 * ::cos(arg);
		// line 14 of Table 8.1, period = 13.17 days
		arg = lp+2*f+2*o;
		UT1mUT1R += 0.03e-4 * ::sin(arg);
		dlodR -= 0.1e-5 * ::cos(arg);
		domegaR += 0.1e-14 * ::cos(arg);
		// line 15 of Table 8.1, period = 13.61 days
		arg = 2*f;
		UT1mUT1R -= 0.30e-4 * ::sin(arg);
		dlodR += 1.4e-5 * ::cos(arg);
		domegaR -= 1.2e-14 * ::cos(arg);
		// line 16 of Table 8.1, period = 13.63 days
		arg = 2*f+o;
		UT1mUT1R -= 3.21e-4 * ::sin(arg);
		dlodR += 14.8e-5 * ::cos(arg);
		domegaR -= 12.5e-14 * ::cos(arg);
		// line 17 of Table 8.1, period = 13.66 days
		arg = 2*f+2*o;
		UT1mUT1R -= 7.76e-4 * ::sin(arg);
		dlodR += 35.7e-5 * ::cos(arg);
		domegaR -= 30.1e-14 * ::cos(arg);
		// line 18 of Table 8.1, period = 13.75 days
		arg = 2*l-o;
		UT1mUT1R += 0.02e-4 * ::sin(arg);
		dlodR -= 0.1e-5 * ::cos(arg);
		domegaR += 0.1e-14 * ::cos(arg);
		// line 19 of Table 8.1, period = 13.78 days
		arg = 2*l;
		UT1mUT1R -= 0.34e-4 * ::sin(arg);
		dlodR += 1.5e-5 * ::cos(arg);
		domegaR -= 1.3e-14 * ::cos(arg);
		// line 20 of Table 8.1, period = 13.81 days
		arg = 2*l+o;
		UT1mUT1R += 0.02e-4 * ::sin(arg);
		dlodR -= 0.1e-5 * ::cos(arg);
		domegaR += 0.1e-14 * ::cos(arg);
		// line 21 of Table 8.1, period = 14.19 days
		arg = -lp+2*f+2*o;
		UT1mUT1R -= 0.02e-4 * ::sin(arg);
		dlodR += 0.1e-5 * ::cos(arg);
		domegaR -= 0.1e-14 * ::cos(arg);
		// line 22 of Table 8.1, period = 14.73 days
		arg = 2*d-o;
		UT1mUT1R += 0.05e-4 * ::sin(arg);
		dlodR -= 0.2e-5 * ::cos(arg);
		domegaR += 0.2e-14 * ::cos(arg);
		// line 23 of Table 8.1, period = 14.77 days
		arg = 2*d;
		UT1mUT1R -= 0.73e-4 * ::sin(arg);
		dlodR += 3.1e-5 * ::cos(arg);
		domegaR -= 2.6e-14 * ::cos(arg);
		// line 24 of Table 8.1, period = 14.80 days
		arg = 2*d+o;
		UT1mUT1R -= 0.05e-4 * ::sin(arg);
		dlodR += 0.2e-5 * ::cos(arg);
		domegaR -= 0.2e-14 * ::cos(arg);
		// line 25 of Table 8.1, period = 15.39 days
		arg = -lp+2*d;
		UT1mUT1R -= 0.05e-4 * ::sin(arg);
		dlodR += 0.2e-5 * ::cos(arg);
		domegaR -= 0.2e-14 * ::cos(arg);
		// line 26 of Table 8.1, period = 23.86 days
		arg = l+2*f-2*d+o;
		UT1mUT1R += 0.05e-4 * ::sin(arg);
		dlodR -= 0.1e-5 * ::cos(arg);
		domegaR += 0.1e-14 * ::cos(arg);
		// line 27 of Table 8.1, period = 23.94 days
		arg = l+2*f-2*d+2*o;
		UT1mUT1R += 0.10e-4 * ::sin(arg);
		dlodR -= 0.3e-5 * ::cos(arg);
		domegaR += 0.2e-14 * ::cos(arg);
		// line 28 of Table 8.1, period = 25.62 days
		arg = l+lp;
		UT1mUT1R += 0.04e-4 * ::sin(arg);
		dlodR -= 0.1e-5 * ::cos(arg);
		domegaR += 0.1e-14 * ::cos(arg);
		// line 29 of Table 8.1, period = 26.88 days
		arg = -l+2*f;
		UT1mUT1R += 0.05e-4 * ::sin(arg);
		dlodR -= 0.1e-5 * ::cos(arg);
		domegaR += 0.1e-14 * ::cos(arg);
		// line 30 of Table 8.1, period = 26.98 days
		arg = -l+2*f+o;
		UT1mUT1R += 0.18e-4 * ::sin(arg);
		dlodR -= 0.4e-5 * ::cos(arg);
		domegaR += 0.3e-14 * ::cos(arg);
		// line 31 of Table 8.1, period = 27.09 days
		arg = -l+2*f+2*o;
		UT1mUT1R += 0.44e-4 * ::sin(arg);
		dlodR -= 1.0e-5 * ::cos(arg);
		domegaR += 0.9e-14 * ::cos(arg);
		// line 32 of Table 8.1, period = 27.44 days
		arg = l-o;
		UT1mUT1R += 0.53e-4 * ::sin(arg);
		dlodR -= 1.2e-5 * ::cos(arg);
		domegaR += 1.0e-14 * ::cos(arg);
		// line 33 of Table 8.1, period = 27.56 days
		arg = l;
		UT1mUT1R -= 8.26e-4 * ::sin(arg);
		dlodR += 18.8e-5 * ::cos(arg);
		domegaR -= 15.9e-14 * ::cos(arg);
		// line 34 of Table 8.1, period = 27.67 days
		arg = l+o;
		UT1mUT1R += 0.54e-4 * ::sin(arg);
		dlodR -= 1.2e-5 * ::cos(arg);
		domegaR += 1.0e-14 * ::cos(arg);
		// line 35 of Table 8.1, period = 29.53 days
		arg = d;
		UT1mUT1R += 0.05e-4 * ::sin(arg);
		dlodR -= 0.1e-5 * ::cos(arg);
		domegaR += 0.1e-14 * ::cos(arg);
		// line 36 of Table 8.1, period = 29.80 days
		arg = l-lp;
		UT1mUT1R -= 0.06e-4 * ::sin(arg);
		dlodR += 0.1e-5 * ::cos(arg);
		domegaR -= 0.1e-14 * ::cos(arg);
		// line 37 of Table 8.1, period = 31.66 days
		arg = -l+2*d-o;
		UT1mUT1R += 0.12e-4 * ::sin(arg);
		dlodR -= 0.2e-5 * ::cos(arg);
		domegaR += 0.2e-14 * ::cos(arg);
		// line 38 of Table 8.1, period = 31.81 days
		arg = -l+2*d;
		UT1mUT1R -= 1.82e-4 * ::sin(arg);
		dlodR += 3.6e-5 * ::cos(arg);
		domegaR -= 3.0e-14 * ::cos(arg);
		// line 39 of Table 8.1, period = 31.96 days
		arg = -l+2*d+o;
		UT1mUT1R += 0.13e-4 * ::sin(arg);
		dlodR -= 0.3e-5 * ::cos(arg);
		domegaR += 0.2e-14 * ::cos(arg);
		// line 40 of Table 8.1, period = 32.61 days
		arg = l-2*f+2*d-o;
		UT1mUT1R += 0.02e-4 * ::sin(arg);
		// line 41 of Table 8.1, period = 34.85 days
		arg = -l-lp+2*d;
		UT1mUT1R -= 0.09e-4 * ::sin(arg);
		dlodR += 0.2e-5 * ::cos(arg);
		domegaR -= 0.1e-14 * ::cos(arg);
	
		//End Code implementing Table 8.1 IERS Conventions 1996 UT1R tide series.
		//-----------------------------------------------------------------------
	}

   //---------------------------------------------------------------------------------
   // Compute the Greenwich hour angle of the true vernal equinox, or
   // Greenwich Apparent Sidereal Time (GAST) in radians,
   // given the (UT) time of interest t, and, where T = CoordTransTime(t),
   // o  = Omega(T) = mean longitude of lunar ascending node, in degrees,
   // eps = the obliquity of the ecliptic, in degrees,
   // dpsi = nutation in longitude (counted in the ecliptic),
   //                in seconds of arc.
   //
   // GAST = Greenwich hour angle of the true vernal equinox
   // GAST = GMST + dpsi*cos(eps) + 0.00264" * sin(Omega) +0.000063" * sin(2*Omega)
   //    (these terms account for the accumulated precession and nutation in
   //       right ascension and minimize any discontinuity in UT1)
   //
   // GMST = Greenwich hour angle of the mean vernal equinox
   //      = Greenwich Mean Sideral Time
   //      = GMST0 + r*[UTC + (UT1-UTC)]
   // r    = is the ratio of universal to sidereal time
   //      = 1.002737909350795 + 5.9006E-11*T' - 5.9e-15*T'^2
   // T'   = days'/36525
   // days'= number of days elapsed since the Julian Epoch t0 (J2000)
   //      = +/-(integer+0.5)
   //   and
   // (UT1-UTC) (seconds) is taken from the IERS bulletin 
   //
   // GMST0 = GMST at 0h UT1
   //      = 6h 41min (50.54841+8640184.812866*T'+0.093104*T'^2-6.2E-6*T'^3)sec
   //
   // see pg 21 of the Reference (IERS 1996).
   double GeodeticFrames::gast(DayTime t,
                               double om,
                               double eps,
                               double dpsi,
                               double UT1mUTC)
      throw()
   {
      double G = GMST(t,UT1mUTC);
         // add dpsi, eps and Omega terms
      om *= DEG_TO_RAD;
      eps *= DEG_TO_RAD;
      G += (       dpsi * ::cos(eps)
             + 0.00264  * ::sin(om)
             + 0.000063 * ::sin(2.0*om) ) * DEG_TO_RAD / 3600.0;

      return G;
   }

   //---------------------------------------------------------------------------------
   // Compute the precession matrix, a 3x3 rotation matrix, given T,
   // the coordinate transformation time at the time of interest
   Matrix<double> GeodeticFrames::PrecessionMatrix(double T)
      throw(InvalidRequest)
   {
      try {
            // IAU76 - ref McCarthy - seconds of arc
         double zeta  = T*(2306.2181 + T*(0.30188 + T*0.017998));
         double theta = T*(2004.3109 - T*(0.42665 + T*0.041833));
         double z     = T*(2306.2181 + T*(1.09468 + T*0.018203));

            // convert to degrees
         zeta  /= 3600.0;
         theta /= 3600.0;
         z     /= 3600.0;

         Matrix<double> R1 = rotation(-zeta*DEG_TO_RAD, 3);
         Matrix<double> R2 = rotation(theta*DEG_TO_RAD, 2);
         Matrix<double> R3 = rotation(-z*DEG_TO_RAD, 3);
         Matrix<double> P = R3*R2*R1;

         return P;
      }
      catch(InvalidRequest& ire) {
         GPSTK_RETHROW(ire);
      }
   }

   //---------------------------------------------------------------------------------
   // Compute the nutation matrix, given
   // eps,  the obliquity of the ecliptic, in degrees,
   // dpsi, the nutation in longitude (counted in the ecliptic),
   // in seconds of arc, and
   // deps, the nutation in obliquity, in seconds of arc.
   Matrix<double> GeodeticFrames::NutationMatrix(double eps,
                                                 double dpsi,
                                                 double deps)
      throw(InvalidRequest)
   {
      Matrix<double> N;
      try {
         Matrix<double> R1 = rotation(-eps*DEG_TO_RAD, 1);
         Matrix<double> R2 = rotation(dpsi*DEG_TO_RAD/3600.0, 3);
         Matrix<double> R3 = rotation((eps+deps/3600.0)*DEG_TO_RAD, 1);
         N = R1*R2*R3;
         return N;
      }
      catch(InvalidRequest& ire) {
         GPSTK_RETHROW(ire);
      }
   }

   //---------------------------------------------------------------------------------
   // public functions

   //---------------------------------------------------------------------------------
   // Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
   // the mean vernal equinox, given the coordinate time of interest,
   // and UT1-UTC (sec), which comes from the IERS bulletin.
   // @param t DayTime epoch of the rotation.
   // @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
   // @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
   //                 'no tides', as is the case with the NGA EOPs (default=F).
   double GeodeticFrames::GMST(DayTime t,
                               double UT1mUTC,
                               bool reduced)
         throw()
   {
         // days' since epoch = +/-(integer+0.5)
      double days = double(t.JD() - JulianEpoch) - 1.0 + t.secOfDay()/86400.0;
      int d=int(days);
      if(d < 0 && days==double(d)) d++;
      days = d + (days<0.0 ? -0.5 : 0.5);
      double Tp = days/36525.0;

         // Compute GMST in radians
         // First compute GMST0
      double G;
      //G = 24060.0 + 50.54841 + 8640184.812866*Tp;  // seconds (24060s = 6h 41min)
      //G /= 86400.0; // instead, divide the above equation by 86400.0 manually...
      G = 0.27847222 + 0.00058505104167 + 100.0021390378009*Tp;
      G += (0.093104 - 6.2e-6*Tp)*Tp*Tp/86400.0;      // seconds/86400 = circles

         // if reduced, compute tidal terms
      if(reduced) {
         double dlodR,domegaR,UT1mUT1R;
         UT1mUTCTidalCorrections(CoordTransTime(t), UT1mUT1R, dlodR, domegaR);
         UT1mUTC = UT1mUT1R-UT1mUTC;
      }

         // now get GMST
      double r=1.002737909350795 + (5.9006e-11 - 5.9e-15*Tp)*Tp;
      G += r*(UT1mUTC + t.secOfDay() - 13.0)/86400.0;       // circles
      G *= TWO_PI;                                   // radians

      return G;
   }

   //---------------------------------------------------------------------------------
   // Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
   // the true vernal equinox, given the coordinate time of interest,
   // and UT1-UTC (sec), which comes from the IERS bulletin.
   // @param t DayTime epoch of the rotation.
   // @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
   double GeodeticFrames::GAST(DayTime t,
                               double UT1mUTC,
                               bool reduced)
      throw()
   {
	   double T = CoordTransTime(t);
	   double o = Omega(T);
	   double eps = Obliquity(T);
      double deps,dpsi;

      NutationAngles(T,deps,dpsi);        // deps is not used...

      // if reduced (NGA), correct for tides
      double UT1mUT1R,dlodR,domegaR;
      if(reduced)
         UT1mUTCTidalCorrections(T, UT1mUT1R, dlodR, domegaR);

	   return gast(t, o, eps, dpsi, reduced ? UT1mUT1R-UT1mUTC : UT1mUTC);
   }

   //---------------------------------------------------------------------------------
   // Generate transformation matrix (3X3 rotation) due to polar motion (xp,yp)
   // xp and yp are in arc seconds, as found in the IERS bulletin
   Matrix<double> GeodeticFrames::PolarMotion(double xp,
                                              double yp)
      throw(InvalidRequest)
   {
      try {
	      xp *= DEG_TO_RAD/3600.0;
	      yp *= DEG_TO_RAD/3600.0;
         Matrix<double> R1,R2;
         R1 = rotation(yp,1);
         R2 = rotation(xp,2);
	      return (R1*R2);
      }
      catch(InvalidRequest& ire) {
         GPSTK_RETHROW(ire);
      }
   }

   //---------------------------------------------------------------------------------
   // Generate precise transformation matrix (3X3 rotation) due to Earth rotation
   // at Greenwich hour angle of the true vernal equinox and which accounts for
   // precession and nutation in right ascension, given the UT time of interest
   // and the UT1-UTC correction (in sec), obtained from the IERS bulletin.
   Matrix<double> GeodeticFrames::PreciseEarthRotation(DayTime t,
                                                       double UT1mUTC,
                                                       bool reduced)
      throw(InvalidRequest)
   {
      try {
         return (rotation(-GAST(t,UT1mUTC,reduced),3));
      }
      catch(InvalidRequest& ire) {
         GPSTK_RETHROW(ire);
      }
   }

   //---------------------------------------------------------------------------------
   // Generate an Earth Nutation Matrix (3X3 rotation) at the given DayTime.
   // @param t DayTime epoch of the rotation.
   Matrix<double> GeodeticFrames::Nutation(DayTime t)
      throw(InvalidRequest)
   {
      try {
	      double T=CoordTransTime(t);
	      double eps=Obliquity(T);								// degrees
         double deps,dpsi;

         NutationAngles(T,deps,dpsi);

	      return NutationMatrix(eps,dpsi,deps);
      }
      catch(InvalidRequest& ire) {
         GPSTK_RETHROW(ire);
      }
   }

   //---------------------------------------------------------------------------------
   // Generate the full transformation matrix (3x3 rotation) relating the ECEF
   // frame to the conventional inertial frame.
   // throw(); Input is the time of interest,
   // the polar motion angles xp and yp (arcseconds), and UT1-UTC (seconds)
   // (xp,yp and UT1-UTC are just as found in the IERS bulletin).
   Matrix<double> GeodeticFrames::ECEFtoInertial(DayTime t,
                                                 double xp,
                                                 double yp,
                                                 double UT1mUTC,
                                                 bool reduced)
      throw(InvalidRequest)
   {
      try {
	      Matrix<double> P,N,W,S;

	      double T=CoordTransTime(t);
         P = PrecessionMatrix(T);

	      double eps=Obliquity(T);								// degrees
         double deps,dpsi;
         NutationAngles(T,deps,dpsi);
         N = NutationMatrix(eps,dpsi,deps);

         // PolarMotion converts xp, yp to radians
	      W = PolarMotion(xp, yp);

         // if reduced (NGA), correct UT1mUTC for tides
         double UT1mUT1R,dlodR,domegaR;
         if(reduced)
            UT1mUTCTidalCorrections(T, UT1mUT1R, dlodR, domegaR);

	      double omega = Omega(T);
	      double g = gast(t, omega, eps, dpsi, reduced ? UT1mUT1R-UT1mUTC : UT1mUTC);
         S = rotation(-g,3);

	      return (P*N*W*S);
      }
      catch(InvalidRequest& ire) {
         GPSTK_RETHROW(ire);
      }
   }

   //---------------------------------------------------------------------------------
   // Given a rotation matrix R (3x3), inverse(R)=transpose(R),
   // find the Euler angles (theta,phi,psi) which produce this rotation,
   // and also determine the magnitude (alpha) and direction (nhat = unit 3-vector)
   // of the net rotation.
   // Throw InvalidRequest if the matrix is not a rotation matrix.
   //
   // Euler angles (this is one convention - there are others):
   //   Let R1 = rotation about z through angle phi
   //       R2 = rotation about x through angle theta ( 0 <= theta <= pi)
   //       R3 = rotation about z through angle psi
   //   Any rotation matrix can be expressed as the product of these rotations:
   //   R = R3*R2*R1. In particular, by definition
   //
   //           [  cos(phi) sin(phi)  0 ]
   //      R1 = [ -sin(phi) cos(phi)  0 ]
   //           [     0        0      1 ]
   //
   //           [ cos(theta) 0 -sin(theta) ]
   //      R2 = [      0     1     0       ]
   //           [ sin(theta) 0  cos(theta) ]
   //
   //           [  cos(psi) sin(psi)  0 ]
   //      R3 = [ -sin(psi) cos(psi)  0 ]
   //           [     0        0      1 ]
   //
   //   and if we define
   //          [ r11 r12 r13 ]
   //      R = [ r21 r22 r23 ]
   //          [ r31 r32 r33 ]
   //
   //   then
   //      r11 =  cos(phi)cos(psi) - cos(theta)sin(phi)sin(psi)
   //      r12 =  sin(phi)cos(psi) + cos(theta)cos(phi)sin(psi)
   //      r13 =  sin(psi)sin(theta)
   //      r21 = -cos(phi)sin(psi) - cos(theta)sin(phi)cos(psi)
   //      r22 = -sin(phi)sin(psi) + cos(theta)cos(phi)cos(psi)
   //      r23 =  cos(psi)sin(theta)
   //      r31 =  sin(phi)sin(theta)
   //      r32 = -cos(phi)sin(theta)
   //      r33 =  cos(theta)
   //
   //  This can be inverted to get theta,phi,psi by
   //     cos(theta) = r33
   //     sin(theta) = +sqrt(1-cos^2(theta))   [recall 0 <= theta <= pi]
   //     cos(phi)   = -r32/sin(theta)         [if sin(theta) != 0]
   //     sin(phi)   =  r31/sin(theta)
   //     cos(psi)   =  r23/sin(theta)
   //     sin(psi)   =  r13/sin(theta)
   //        or better
   //     theta = acos(r33)
   //     phi = atan2(r31,-r32)
   //     psi = atan2(r13,r23)
   //
   //  If sin(theta) == 0 then cos(theta) = {+1 OR -1}, theta = {0 OR pi},
   //  and (refer to R3*R2*R1 above), R becomes
   //         [  cos(phi+psi) sin(phi+psi)  0 ] OR [ cos(phi-psi)  sin(phi-psi)  0 ]
   //     R = [ -sin(phi+psi) cos(phi+psi)  0 ] OR [ sin(phi-psi) -cos(phi-psi)  0 ]
   //         [     0        0              1 ] OR [    0         0             -1 ]
   //  and thus
   //     cos(phi+psi) = r11 =  r22             OR  cos(phi-psi) = r11 = -r22
   //     sin(phi+psi) = r12 = -r21             OR  sin(phi-psi) = r12 =  r21
   //  
   //  Now let E = e0,e1,e2,e3 = (e0,e) = quaternion [scalar (e0) and 3-vector (e)].
   //  E describes a rotation (through angle alpha) about axis (unit vector) nhat as
   //     cos(alpha) = 2e0^2-1 = e0^2-dot(e,e) = e0^2 - sum_squares(ei,i=1,2,3).
   //     nhat * sin(alpha) = 2e*e0 or
   //     nhat = (e1,e2,e3)/dot(e,e)
   //  [e0^2 + dot(e,e) = sum_squares(ei,i=1,2,3,4) = 1 by identity].
   //  It can be shown that the Euler rotation is equal to the quaternion
   //     e0 = cos[(phi+psi)/2]*cos(theta/2)
   //     e1 = sin[(phi-psi)/2]*sin(theta/2)
   //     e2 = cos[(phi-psi)/2]*sin(theta/2)
   //     e3 = sin[(phi+psi)/2]*cos(theta/2)
   //  and
   //     rij = dij(e0^2-ekek)+2eiej+2(epsijk)e0ek
   //     where dij is the Kroncker delta and epsijk is the permutation symbol;
   //     r11 = e0^2 + e1^2 - e2^2 - e3^2
   //     r12 = 2(e1e2 + e0e3)
   //     r13 = 2(e1e3 - e0e2)
   //     r21 = 2(e1e2 - e0e3)
   //     r22 = e0^2 - e1^2 + e2^2 - e3^2
   //     r23 = 2(e2e3 + e0e1)
   //     r31 = 2(e1e3 + e0e2)
   //     r32 = 2(e2e3 - e0e1)
   //     r33 = e0^2 - e1^2 - e2^2 + e3^2
   //
   //  If theta=0 this reduces trivially to a simple rotation about z thru phi+psi
   //     e0 = cos[(phi+psi)/2]
   //     e1 = e2 = 0
   //     e3 = sin[(phi+psi)/2]
   //     alpha = phi+psi (phi and psi cannot be separated)
   //     nhat = (0,0,1)
   //  while if theta=pi it reduces to
   //     e0 = e3 = 0
   //     e1 = sin[(phi-psi)/2]
   //     e2 = cos[(phi-psi)/2]
   //     alpha = pi
   //     nhat = (e1,e2,0)  (dot(e,e)=1)
   void GeodeticFrames::ResolveRotation(const Matrix<double>& R,
                                        double& theta,
                                        double& phi,
                                        double& psi,
                                        double& alpha,
                                        Vector<double>& nhat)
      throw(InvalidRequest)
   {
      if(R.rows() != 3 || R.cols() != 3) {
         InvalidRequest ir(string("Input matrix has dimension ")
                           + StringUtils::asString<int>(R.rows()) + string(",")
                           + StringUtils::asString<int>(R.cols()));
         GPSTK_THROW(ir);
      }

      const double tol=1.e-12;   // tolerance  TD use limits
      Matrix<double> T=transpose(R)*R-ident<double>(3);
      if(normF(T) > tol) {       // RSS of elements
         InvalidRequest ir(string("Input matrix is not a rotation"));
         GPSTK_THROW(ir);
      }

      // first find the Euler angles
      double st = SQRT(1.0-R(2,2)*R(2,2));
      if(st < tol) {          // theta is 0 or pi
         if(R(2,2) > 0.0) theta = 0.0;
         else             theta = PI;
         psi = 0.0;           // arbitrary, since only phi +/- psi can be known
         // tan(phi) = r12/r11 = sin(phi+/-psi) / cos(phi+/-psi)
         phi = atan2(R(0,1),R(0,0));
      }
      else {
         theta = ::acos(R(2,2));
         // tan(psi) = r13/r23 = sin(psi)sin(theta)/cos(psi)sin(theta)
         psi = ::atan2(R(0,2),R(1,2));
         // tan(phi) = r31/-r32 = sin(phi)sin(theta)/cos(phi)sin(theta)
         phi = ::atan2(R(2,0),-R(2,1));
      }

      // now find the rotation angle alpha and the axis of rotation
      nhat.resize(3);
      if(theta == 0.0) {
         alpha = phi;
         nhat(0) = nhat(1) = 0.0;
         nhat(2) = 1.0;
      }
      else if(theta == PI) {
         alpha = PI;
         nhat(0) = ::sin(phi/2.0);
         nhat(1) = ::cos(phi/2.0);
         nhat(2) = 0.0;
      }
      else {
         double e0 = ::cos((phi+psi)/2.0) * ::cos(theta/2.0);
         alpha = ::acos(2.0*e0*e0-1.0);
         // construct e, then normalize
         nhat(0) = ::sin((phi-psi)/2.0) * ::sin(theta/2.0);
         nhat(1) = ::cos((phi-psi)/2.0) * ::sin(theta/2.0);
         nhat(2) = ::sin((phi+psi)/2.0) * ::cos(theta/2.0);
         e0 = norm(nhat);
         nhat /= e0;
      }
   }

} // end namespace gpstk
/*
# Table 5.2.  IERS 1996 series for nutation in longitude Dpsi and obliquity Deps,
# referred to the mean equator and equinox of date, with t measured in Julian
# centuries from epoch J2000.0. The signs of the fundamental arguments, periods,
# and coefficients may differ from the original publication.  These
# have been changed to be consistent with other portions of this chapter.
#        263
# Dpsi = sum { (A_i+A'_i*t)*sin(ARGUMENT) + A''_i*cos(ARGUMENT) }
#        i=1
#
#        263
# Deps = sum { (B_i+B'_i*t)*cos(ARGUMENT) + B''_i*sin(ARGUMENT) }
#        i=1
#                                LONGITUDE      OBLIQUITY
#MULTIPLIERS OF:  PERIOD        (0.001 mas)    (0.001 mas)
# l  l' F  D  Om  (days)          A      A'        B    B'   A''   B''
  0  0  0  0  1 -6798.38  -17206277  -17419  9205356   886  3645  1553
  0  0  2 -2  2   182.62   -1317014    -156   573058  -306 -1400  -464
  0  0  2  0  2    13.66    -227720     -23    97864   -48   269   136
  0  0  0  0  2 -3399.18     207429      21   -89747    47   -71   -29
  0 -1  0  0  0  -365.26    -147538     364     7388   -19  1121   198
  0  1  2 -2  2   121.75     -51687     123    22440   -68   -54   -18
  1  0  0  0  0    27.55      71118       7     -687     0   -94    39
  0  0  2  0  1    13.63     -38752     -37    20076     2    34    32
  1  0  2  0  2     9.13     -30137      -4    12896    -6    77    35
  0 -1  2 -2  2   365.22      21583     -49    -9591    30     6    12
  0  0  2 -2  1   177.84      12820      14    -6897    -1    18     4
 -1  0  2  0  2    27.09      12353       1    -5334     3     2     0
 -1  0  0  2  0    31.81      15699       1     -127     0   -18     9
  1  0  0  0  1    27.67       6314       6    -3323     0     3    -1
 -1  0  0  0  1   -27.44      -5797      -6     3141     0   -19    -8
 -1  0  2  2  2     9.56      -5965      -1     2554    -1    14     7
  1  0  2  0  1     9.12      -5163      -4     2635     0    12     8
 -2  0  2  0  1  1305.48       4590       5    -2424    -1     1     1
  0  0  0  2  0    14.77       6336       1     -125     0   -15     3
  0  0  2  2  2     7.10      -3854       0     1643     0    15     6
 -2  0  0  2  0  -205.89      -4774       0       48     0    -2    -3
  2  0  2  0  2     6.86      -3102       0     1323    -1    12     5
  1  0  2 -2  2    23.94       2863       0    -1235     1     0     0
 -1  0  2  0  1    26.98       2044       2    -1076     0     1     0
  2  0  0  0  0    13.78       2923       0      -62     0    -8     1
  0  0  2  0  0    13.61       2585       0      -56     0    -7     1
  0  1  0  0  1   386.00      -1406      -3      857     0     8    -4
 -1  0  0  2  1    31.96       1517       1     -801     0     1     0
  0  2  2 -2  2    91.31      -1578       7      685    -4    -2    -1
  0  0 -2  2  0  -173.31       2178       0      -15     0     1     1
  1  0  0 -2  1   -31.66      -1286      -1      694     0    -4    -2
  0 -1  0  0  1  -346.64      -1269       1      642     1     6     2
 -1  0  2  2  1     9.54      -1022      -1      522     0     2     1
  0 -2  0  0  0  -182.63      -1671       8       14     0    -1    -1
  1  0  2  2  2     5.64       -768       0      325     0     4     2
 -2  0  2  0  0  1095.18      -1102       0       10     0    -1     0
  0  1  2  0  2    13.17        757      -2     -326    -2    -1     0
  0  0  2  2  1     7.09       -664      -1      335    -1     2     1
  0 -1  2  0  2    14.19       -714       2      307     2     1     0
  0  0  0  2  1    14.80       -631      -1      327     0     0     0
  1  0  2 -2  1    23.86        580       1     -307     0     0     0
  2  0  2 -2  2    12.81        643       0     -277     0    -1     0
 -2  0  0  2  1  -199.84       -579      -1      304     0    -1     0
  2  0  2  0  1     6.85       -533       0      269     0     2     1
  0 -1  2 -2  1   346.60       -477      -1      271    -1     0     0
  0  0  0 -2  1   -14.73       -493      -1      272     0    -2    -1
 -1 -1  0  2  0    34.85        735       0       -5     0    -1     0
  2  0  0 -2  1   212.32        405       0     -220     0     1     0
  1  0  0  2  0     9.61        657       0      -20     0    -2     0
  0  1  2 -2  1   119.61        361       0     -194     0     1     0
  1 -1  0  0  0    29.80        471       0       -4     0    -1     0
 -2  0  2  0  2  1615.76       -311       0      131     0     0     0
  3  0  2  0  2     5.49       -289       0      124     0     2     1
  0 -1  0  2  0    15.39        435       0       -9     0    -1     0
  1 -1  2  0  2     9.37       -287       0      123     0     1     0
 -1 -1  2  2  2     9.81       -282       0      122     0     1     0
  0  0  0  1  0    29.53       -422       0        3     0     1     0
 -1  0  2  0  0    26.88       -404       0        4     0     1     0
  0 -1  2  2  2     7.24       -264       0      114     0     1     0
 -2  0  0  0  1   -13.75       -228       0      126     0    -1     0
  1  1  2  0  2     8.91        246       0     -106     0    -1     0
  2  0  0  0  1    13.81        218       0     -114     0     0     0
 -1  1  0  1  0  3232.87        327       0       -1     0     0     0
  1  1  0  0  0    25.62       -338       0        4     0     0     0
  1  0  2  0  0     9.11        334       0      -11     0    -1     0
 -1  0  2 -2  1   -32.61       -199       0      107     0    -1     0
  1  0  0  0  2    27.78       -197       0       85     0     0     0
 -1  0  0  1  0  -411.78        405       0      -55     0   -35   -14
  0  0  2  1  2     9.34        165       0      -72     0     0     0
 -1  0  2  4  2     5.80       -151       0       66     0     1     0
  0 -2  2 -2  1  6786.31       -130       0       69     0     0     0
 -1  1  0  1  1  6164.17        132       0      -68     0     0     0
  1  0  2  2  1     5.64       -133       0       66     0     1     0
 -2  0  2  2  2    14.63        139       0      -60     0     0     0
 -1  0  0  0  2   -27.33        139       0      -60     0     0     0
  1  1  2 -2  2    22.47        128       0      -55     0     0     0
 -2  0  2  4  2     7.35       -121       0       52     0     0     0
 -1  0  4  0  2     9.06        115       0      -49     0     0     0
  2  0  2 -2  1    12.79        101       0      -54     0     0     0
  2  0  2  2  2     4.68       -108       0       47     0     1     0
  1  0  0  2  1     9.63        -95       0       49     0     0     0
  3  0  0  0  0     9.18        157       0       -5     0    -1     0
  3  0  2 -2  2     8.75         94       0      -40     0     0     0
  0  0  4 -2  2    12.66         91       0      -39     0     0     0
  0  0 -2  2  1  -169.00         87       0      -44     0     0     0
  0  1  2  0  1    13.14         81       0      -42     0     0     0
  0  0  2 -2  3   187.66        123       0      -20     0     0     0
 -1  0  0  4  0    10.08        133       0       -4     0     0     0
  2  0 -2  0  1  -943.23         71       0      -38     0     0     0
  2  0  0 -4  0   -15.91       -128       0        1     0     0     0
 -1 -1  0  2  1    35.03         75       0      -39     0     0     0
 -2 -1  0  2  0  -131.67       -115       0        1     0     0     0
  0 -1  2  0  1    14.16        -66       0       35     0     0     0
 -1  0  0  1  1  -388.27        101       0      -49     0    -3    -1
  0  0 -2  0  1   -13.58        -68       0       36     0     0     0
  0  1  0  0  2   409.23         69       0      -33     0    -1     0
  0  0  2 -1  2    25.42        -74       0       31     0     0     0
  0  0  2  4  2     4.79        -69       0       29     0     0     0
  1  1  0 -2  1   -34.67        -61       0       32     0     0     0
 -1  1  0  2  0    29.26        -94       0        0     0     0     0
  1 -1  2  2  2     5.73        -59       0       25     0     0     0
  1 -1  0  0  1    29.93         51       0      -27     0     0     0
  0  1 -2  2  0  -329.79        -90       0        3     0     0     0
  3  0  2  0  1     5.49        -50       0       25     0     0     0
 -1  1  2  2  2     9.31         56       0      -24     0     0     0
  0  1  2  2  2     6.96         54       0      -22     0     0     0
 -1  0  0 -2  1    -9.60        -50       0       27     0     0     0
 -1  1  0  1  2 66079.30        -52       0       23     0     0     0
  0 -1  2  2  1     7.23        -44       0       24     0     0     0
  1  0  2 -4  1   -38.74        -47       0       24     0     0     0
 -1  0 -2  2  0   -23.77         77       0        0     0     0     0
 -1 -1  2  2  1     9.80        -46       0       24     0     0     0
  0 -1  0  0  2  -329.82         59       0      -25     0     0     0
  2 -1  2  0  2     6.99        -48       0       21     0     0     0
  1 -1  2  0  1     9.35        -42       0       22     0     0     0
  0  0  0  2  2    14.83        -46       0       20     0     0     0
  0  1  0  2  0    14.19        -67       0        0     0     0     0
 -1  1  2  0  2    25.22         47       0      -20     0     0     0
  0  3  2 -2  2    73.05        -44       0       19     0     0     0
  0 -1 -2  2  0  -117.54         66       0        0     0     0     0
  0  0  0  1  1    29.66        -37       0       20     0     0     0
  1  0 -2 -2  0    -9.53        -64       0        1     0     0     0
  1  1  2  0  1     8.90         36       0      -18     0     0     0
  2  1  2  0  2     6.73         40       0      -17     0     0     0
  0  1  0  1  0    27.32         57       0        0     0     0     0
  1  0 -2  2  0    32.76        -58       0        0     0     0     0
  1  1  0  0  1    25.72        -34       0       19     0     0     0
 -2  0  0 -2  0    -7.13        -59       0        1     0     0     0
 -1  0  0  2  2    32.11        -38       0       17     0     0     0
  0  0  0 -1  1   -29.40         33       0      -18     0     0     0
  0  1  0 -2  1   -15.35        -33       0       18     0     0     0
 -1  0  2 -2  2   -32.45         36       0      -16     0     0     0
 -1  1  0  0  1   -29.67        -31       0       17     0     0     0
  1  0  2  1  2     6.98         33       0      -14     0     0     0
  0  0  0 -4  0    -7.38        -48       0        1     0     0     0
  0  0  2  1  1     9.33         27       0      -14     0     0     0
  1  0  0 -2  2   -31.52         32       0      -14     0     0     0
  1  0  2 -1  2    13.22        -33       0       13     0     0     0
  1 -1  0  2  0     9.87         48       0        0     0     0     0
 -1  0  2  4  1     5.80        -26       0       13     0     0     0
  0  0 -2 -2  0    -7.08        -41       0        1     0     0     0
  1  0 -2  0  1   -26.77         27       0      -14     0     0     0
 -1  0  2 -1  1   313.04        -23       0       14     0     0     0
  1  1  2 -2  1    22.40         23       0      -12     0     0     0
  4  0  2  0  2     4.58        -26       0       11     0     0     0
  0  1  2  1  2     9.11        -24       0       10     0     0     0
 -2  0 -2  0  0    -6.85        -36       0        1     0     0     0
  2  1  2 -2  2    12.38         25       0      -10     0     0     0
  2 -1  0  0  0    14.32         38       0        0     0     0     0
 -1 -1  0  0  1   -25.53         21       0      -12     0     0     0
 -2  0  2  2  1    14.60         22       0      -11     0     0     0
  0  0  0  0  3 -2266.12        -22       0       10     0     0     0
  1  0  4 -2  2     8.68         23       0       -9     0     0     0
  2  0  2  2  1     4.68        -19       0       10     0     0     0
 -2  0  2  4  1     7.34        -20       0       10     0     0     0
  0  1  0  2  1    14.22         18       0       -9     0     0     0
  1  0  0  1  0    14.25        -33       0        0     0     0     0
 -1  0  0  4  1    10.10        -18       0        9     0     0     0
 -1  0  4  0  1     9.05         19       0       -9     0     0     0
  0  0  2 -3  2   -35.23        -20       0        8     0     0     0
  0  0  4  0  2     6.82         19       0       -8     0     0     0
  2  1  0  0  0    13.28        -28       0        0     0     0     0
  0  0  2 -4  1   -16.10        -16       0        9     0     0     0
 -1 -1  2  4  2     5.90        -17       0        7     0     0     0
 -1 -2  0  2  0    38.52         27       0        0     0     0     0
  0  0  0  4  1     7.39        -16       0        7     0     0     0
  0 -1  0  2  1    15.42        -14       0        7     0     0     0
  1  0  2  4  2     4.08        -16       0        7     0     0     0
 -2  0  0  2  2  -194.13         18       0       -8     0     0     0
 -2  2  0  2  0  1616.44        -22       0        0     0     0     0
 -2 -1  2  0  1  -507.16          9       0       -5     0     0     0
 -3  0  0  0  1    -9.17        -14       0        7     0     0     0
  0  0  2  0  3    13.69         20       0        0     0     0     0
  0  0  2  4  1     4.79        -12       0        6     0     0     0
  0  0  4 -2  1    12.64         12       0       -7     0     0     0
  0 -2  0  2  0    16.06         21       0        0     0     0     0
  1  0  0 -1  1   438.33         17       0       -5     0    -3     1
  1  1  2  2  2     5.56         15       0       -6     0     0     0
  3  0  2 -2  1     8.73         12       0       -7     0     0     0
 -1 -1  2  0  2    29.26        -16       0        6     0     0     0
 -2 -1  0  2  1  -129.17        -13       0        7     0     0     0
  0  0  0 -2  2   -14.70         13       0       -5     0     0     0
  0 -2  2  2  2     7.38        -13       0        5     0     0     0
  1  0  0 -4  1   -10.07        -12       0        6     0     0     0
 -1  1  0  2  1    29.39        -10       0        6     0     0     0
 -2  0  0  4  1    15.94         11       0       -6     0     0     0
  0  0  2 -1  1    25.33        -10       0        5     0     0     0
  0  2  0  0  1   187.67         -9       0        5     0     0     0
  0  2  2 -2  1    90.10          8       0       -5     0     0     0
  2  0  0  2  1     7.13         -9       0        5     0     0     0
  2  0  0 -4  1   -15.87        -11       0        5     0     0     0
  2  0  2 -4  1    95.42         10       0       -5     0     0     0
 -1  0 -2  0  1    -9.10        -10       0        5     0     0     0
 -1  1  2  0  1    25.13          9       0       -5     0     0     0
 -1  1  2 -2  1   -35.80        -11       0        5     0     0     0
 -1 -1  0  4  0    10.37         15       0        0     0     0     0
 -3  0  0  4  0    37.63         16       0        0     0     0     0
  3  0  2  2  2     4.00        -14       0        0     0     0     0
  2 -1  0 -2  0   471.89         -9       0        1     0    -1     0
  0  2 -2  2  0 -3396.16         -9       0        0     0     0     0
  0 -1  2  4  2     4.86         -9       0        0     0     0     0
  0 -1  2 -1  2    27.32          9       0        0     0     0     0
  1  1  0  2  0     9.37        -10       0        0     0     0     0
  2  0  0 -2  2   219.17        -11       0        0     0     0     0
  2 -1  2  2  2     4.74         -9       0        0     0     0     0
  4  0  0  0  0     6.89          9       0        0     0     0     0
  4  0  2 -2  2     6.64         12       0        0     0     0     0
 -1  0  0  3  0    15.31        -10       0        0     0     0     0
 -1  0  4 -2  2    23.43         -9       0        0     0     0     0
 -1 -2  2  2  2    10.08         -9       0        0     0     0     0
 -2 -1  0  4  0    16.63         12       0        0     0     0     0
 -2 -1  2  4  2     7.50        -12       0        0     0     0     0
  0  1  2  2  1     6.95          7       0        0     0     0     0
  0  2  2  0  2    12.71          7       0        0     0     0     0
  0 -2  2  0  2    14.77         -8       0        0     0     0     0
  1  0  0  4  0     5.82          8       0        0     0     0     0
  1  0  2  2  0     5.63          8       0        0     0     0     0
  1  0  2 -4  2   -38.52          7       0        0     0     0     0
  1 -1  2  2  1     5.73         -8       0        0     0     0     0
  1 -1  2 -2  2    25.62         -7       0        0     0     0     0
  1 -2  0  0  0    32.45          8       0        0     0     0     0
  2  0  0  0  2    13.83         -8       0        0     0     0     0
  2  1  0 -2  1   134.27          8       0        0     0     0     0
  3  0  0  0  1     9.20          7       0        0     0     0     0
 -1  0  2  1  2    14.13          8       0        0     0     0     0
 -1  0  2  3  2     7.22          8       0        0     0     0     0
 -1  0 -2  4  0    38.96         -7       0        0     0     0     0
 -1  1  2  2  1     9.30          7       0        0     0     0     0
 -1  2  0  2  0    27.09         -8       0        0     0     0     0
 -1 -1  2 -1  1  2189.73          7       0        0     0     0     0
 -2  0  2 -2  1   -14.93         -8       0        0     0     0     0
 -2  0  4  0  2    13.49         -7       0        0     0     0     0
 -2  0 -2  2  0   -12.76          8       0        0     0     0     0
 -2  1  2  0  1   285.41          9       0        0     0     0     0
 -3  0  2  0  1   -28.15         -8       0        0     0     0     0
  0  1  0  1  1    27.43          5       0        0     0     0     0
  0 -1  0  4  0     7.53          6       0        0     0     0     0
  0 -1  0 -2  1   -14.16          5       0        0     0     0     0
  0 -2  0  0  1  -177.85         -6       0        0     0     0     0
  1  0  2  1  1     6.97          5       0        0     0     0     0
  1  0  2 -3  2   126.51         -6       0        0     0     0     0
  1  0 -2  1  0  -299.26         -7       0        0     0     0     0
  1  1  0  1  0    13.72          5       0        0     0     0     0
  1 -1  0 -2  1   -29.14          6       0        0     0     0     0
  2  0  2 -1  2     8.93         -6       0        0     0     0     0
  2  1  2  0  1     6.73          5       0        0     0     0     0
  2 -1  2  0  1     6.98         -6       0        0     0     0     0
  2 -1  2 -2  2    13.28          5       0        0     0     0     0
  3  0  0  2  0     5.66          5       0        0     0     0     0
  3 -1  2  0  2     5.58         -5       0        0     0     0     0
 -1 -1  2  0  1    29.14         -6       0        0     0     0     0
 -2  0  0  0  2   -13.72          6       0        0     0     0     0
 -2  0  0  3  0    34.48         -5       0        0     0     0     0
 -2  0  0 -2  1    -7.12         -5       0        0     0     0     0
 -2  0  2  2  0    14.57         -6       0        0     0     0     0
 -2 -1  2  0  0  -548.04         -5       0        0     0     0     0
 -2 -1  2  2  2    15.24          6       0        0     0     0     0
  0  0  1  0  0    27.21          0       0        0     0     8     0
  0  0  1  0  1    27.32          0       0        0     0   -16   -14
 -1  0  1  0  0  2190.35          0       0        0     0    33     0
 -1  0  1  0  1  3231.51          0       0        0     0  -105   -89
 -1  0  1  0  2  6159.22          0       0        0     0    36    18
 -1  0  1  0  3 65514.10          0       0        0     0    -6     0
# end of table 5.2
*/
