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
 * @file PhaseWindup.hpp
 * Include file for the phase windup routines.
 */

#ifndef PHASE_WINDUP_INCLUDE
#define PHASE_WINDUP_INCLUDE

#include "CommonTime.hpp"
#include "Position.hpp"
#include "SolarSystem.hpp"
#include "EarthOrientation.hpp"

namespace gpstk {

/// Compute the phase windup, in cycles, given the time, the unit vector from receiver
/// to transmitter, and the west and north unit vectors at the receiver, all in ECEF.
/// YR is the West unit vector, XR is the North unit vector, at the receiver.
/// shadow is the fraction of the sun's area visible at the satellite.
/// Previous value is needed to ensure continuity and prevent 1-cycle ambiguities.
/// NB. Block IIR has X (ie the effective dipole orientation) in the -XT direction.
/// Ref. Kouba(2009) GPS Solutions 13, pp1-12.
/// This should not matter to phase windup b/c rotating the antenna by a fixed
/// amount yields a constant offset in the windup.
/// NB. This assumes the transmitter and receiver boresights (Z or 'up') lie in a
/// plane; this is true as long as the SV points to Earth center; cf. Beyerle (2009)
/// GPS Solutions 13, pp191-198; in practice differences are small (<1mm).
/// @param double& prev         return value on previous call (zero initially) (input)
/// @param CommonTime& tt          the epoch of interest (input)
/// @param Position& SV         the satellite position (input)
/// @param Position& Rx2Tx      unit vector from receiver to satellite (input)
/// @param Position& YR         west unit vector at receiver (input)
/// @param Position& XR         north unit vector at receiver (input)
/// @param SolarSystem SSEph    Solar system ephemeris
/// @param EarthOrientation EO  Earth orientation parameters appropriate for time
/// @param double& shadow       fraction of sun visible at satellite (output)
double PhaseWindup(double& prev,
                   CommonTime& tt,
                   Position& SV,
                   Position& Rx2Tx,
                   Position& RxW,
                   Position& RxN,
                   SolarSystem& SSEph,
                   EarthOrientation& EO,
                   double& shadow,
                   bool isBlockR=false)
   throw(Exception);

/// Version without SolarSystem ephemeris; uses a lower quality solar position routine
double PhaseWindup(double& prev,
                   CommonTime& tt,
                   Position& SV,
                   Position& Rx2Tx,
                   Position& RxW,
                   Position& RxN,
                   double& shadow,
                   bool isBlockR=false)
   throw(Exception);

}  // end namespace gpstk

#endif // PHASE_WINDUP_INCLUDE
