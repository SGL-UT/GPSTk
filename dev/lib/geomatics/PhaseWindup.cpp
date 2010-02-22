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
 * @file PhaseWindup.cpp
 * Implement computations of phase windup, solar ephemeris, satellite attitude
 * and eclipse at the satellite.
 */
 
// -----------------------------------------------------------------------------------
// GPSTk includes
#include "Matrix.hpp"
#include "geometry.hpp"             // DEG_TO_RAD
#include "icd_200_constants.hpp"    // TWO_PI
// geomatics
#include "PhaseWindup.hpp"
#include "SunEarthSatGeometry.hpp"

using namespace std;

namespace gpstk
{

// -----------------------------------------------------------------------------------
// Compute the phase windup, in cycles, given the time, the unit vector from receiver
// to transmitter, and the west and north unit vectors at the receiver, all in ECEF.
// YR is the West unit vector, XR is the North unit vector, at the receiver.
// shadow is the fraction of the sun's area not visible at the satellite.
// Previous value is needed to ensure continuity and prevent 1-cycle ambiguities.
double PhaseWindup(double& prev,         // previous return value
                   DayTime& tt,          // epoch of interest
                   Position& SV,         // satellite position
                   Position& Rx2Tx,      // unit vector from receiver to satellite
                   Position& YR,         // west unit vector at receiver
                   Position& XR,         // north unit vector at receiver
                   SolarSystem& SSEph,   // solar system ephemeris
                   EarthOrientation& EO, // earth orientation at tt
                   double& shadow,       // fraction of sun not visible at satellite
                   bool isBlockR)        // true for Block IIR satellites
   throw(Exception)
{
try {
   double d,windup;
   Position DR,DT;
   Position TR = -1.0 * Rx2Tx;         // transmitter to receiver

   // get satellite attitude
   Position XT,YT,ZT;
   Matrix<double> Att = SatelliteAttitude(tt, SV, SSEph, EO, shadow);
   XT = Position(Att(0,0),Att(0,1),Att(0,2));      // Cartesian is default
   YT = Position(Att(1,0),Att(1,1),Att(1,2));
   ZT = Position(Att(2,0),Att(2,1),Att(2,2));

   // NB. Block IIR has X (ie the effective dipole orientation) in the -XT direction.
   // Ref. Kouba(2009) GPS Solutions 13, pp1-12.
   // In fact it should be a rotation by pi about Z, producing a constant offset.
   //if(isBlockR) {
   //   XT = Position(-Att(0,0),-Att(0,1),-Att(0,2));
   //   YT = Position(-Att(1,0),-Att(1,1),-Att(1,2));
   //}

   // compute effective dipoles at receiver and transmitter
   // Ref Kouba(2009) Using IGS Products; note sign diff. <=> East(ref) West(here)
   // NB. switching second sign between the two eqns <=> overall sign windup
   DR = XR - TR * TR.dot(XR) + Position(TR.cross(YR));
   DT = XT - TR * TR.dot(XT) - Position(TR.cross(YT));

   // normalize
   d = 1.0/DR.mag();
   DR = d * DR;
   d = 1.0/DT.mag();
   DT = d * DT;

   windup = ::acos(DT.dot(DR)) / TWO_PI;             // cycles
   if(TR.dot(DR.cross(DT)) < 0.) windup *= -1.0;

   // adjust by 2pi if necessary
   d = windup-prev;
   windup -= int(d + (d < 0.0 ? -0.5 : 0.5));

   return windup;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

// -----------------------------------------------------------------------------------
// Version without JPL solar system ephemeris - uses a lower quality solar position.
// Compute the phase windup, in cycles, given the time, the unit vector from receiver
// to transmitter, and the west and north unit vectors at the receiver, all in ECEF.
// YR is the West unit vector, XR is the North unit vector, at the receiver.
// shadow is the fraction of the sun's area not visible at the satellite.
double PhaseWindup(double& prev,       // previous return value
                   DayTime& tt,        // epoch of interest
                   Position& SV,       // satellite position
                   Position& Rx2Tx,    // unit vector from receiver to satellite
                   Position& YR,       // west unit vector at receiver
                   Position& XR,       // north unit vector at receiver
                   double& shadow,     // fraction of sun not visible at satellite
                   bool isBlockR)        // true for Block IIR satellites
   throw(Exception)
{
try {
   double d,windup=0.0;
   Position DR,DT;
   Position TR = -1.0 * Rx2Tx;         // transmitter to receiver

   // get satellite attitude
   Position XT,YT,ZT;
   Matrix<double> Att = SatelliteAttitude(tt, SV, shadow);
   XT = Position(Att(0,0),Att(0,1),Att(0,2));      // Cartesian is default
   YT = Position(Att(1,0),Att(1,1),Att(1,2));
   ZT = Position(Att(2,0),Att(2,1),Att(2,2));

   // NB. Block IIR has X (ie the effective dipole orientation) in the -XT direction.
   // Ref. Kouba(2009) GPS Solutions 13, pp1-12.
   if(isBlockR) XT = Position(-Att(0,0),-Att(0,1),-Att(0,2));

   // compute effective dipoles at receiver and transmitter
   // Ref Kouba(2009) Using IGS Products; note sign diff. <=> East(ref) West(here)
   // NB. switching second sign between the two eqns <=> overall sign windup
   DR = XR - TR * TR.dot(XR) + Position(TR.cross(YR));
   DT = XT - TR * TR.dot(XT) - Position(TR.cross(YT));

   // normalize
   d = 1.0/DR.mag();
   DR = d * DR;
   d = 1.0/DT.mag();
   DT = d * DT;

   windup = ::acos(DT.dot(DR)) / TWO_PI;
   if(TR.dot(DR.cross(DT)) < 0.) windup *= -1.0;

   // adjust by 2pi if necessary
   d = windup-prev;
   windup -= int(d + (d < 0.0 ? -0.5 : 0.5));

   return windup;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

} // end namespace gpstk
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
