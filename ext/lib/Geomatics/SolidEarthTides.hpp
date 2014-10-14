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
 * @file SolidEarthTides.hpp
 * Implement the formula for the displacement of a point fixed to the solid Earth
 * due to the solid Earth tides resulting from the influence of the Sun and Moon.
 * Reference IERS Conventions (1996) found in IERS Technical Note 21 (IERS).
 * NB. Currently only the largest terms are implemented, yielding a result accurate
 * to the millimeter level. Specifically, IERS pg 61 eq 8 and IERS pg 65 eq 17.
 * The second equation referenced here is the permanent component.
 * Class SolarSystem is used to get Solar and Lunar ephemeris information, as well
 * as GM for sun and moon.
 */

#ifndef SOLID_EARTH_TIDES_INCLUDE
#define SOLID_EARTH_TIDES_INCLUDE

#include "Exception.hpp"
#include "CommonTime.hpp"
#include "Position.hpp"
#include "Triple.hpp"

#include "SolarSystem.hpp"

#include "TimeString.hpp"

//------------------------------------------------------------------------------------
/// Compute the site displacement due to solid Earth tides for the given Position
/// (assumed to be fixed to the solid Earth) at the given time. Return a Triple
/// containing the site displacement in WGS84 ECEF XYZ coordinates with units meters.
/// Reference IERS Conventions (1996) found in IERS Technical Note 21 (IERS).
/// NB. Currently only the largest terms are implemented, yielding a result accurate
/// to the millimeter level. Specifically, IERS pg 61 eq 8 and IERS pg 65 eq 17.
/// @param  Position site    Nominal position of the site of interest.
/// @param  CommonTime time     Time of interest.
/// @param  SolarSystem sse  Reference to solar system ephemeris (class SolarSystem);
///                           must be initialized appropriately for time
///                           (by calling SolarSystem::initializeWithBinaryFile() ).
/// @param  EarthOrientation eo  Earth orientation parameters appropriate for time.
/// @return Triple disp      Displacement vector, WGS84 ECEF XYZ meters.
/// @throw if solar system ephemeris is not valid.
gpstk::Triple computeSolidEarthTides(gpstk::Position site,
                                     gpstk::CommonTime time,
                                     gpstk::SolarSystem& sse,
                                     gpstk::EarthOrientation& eo)
   throw(gpstk::Exception);

/// Compute the site displacement due to rotational deformation due to polar motion
/// for the given Position (assumed to fixed to the solid Earth) at the given time.
/// Return a Triple containing the site displacement in WGS84 ECEF XYZ coordinates
/// with units meters. Reference IERS Conventions (1996) found in IERS Technical
/// Note 21 (IERS), ch. 7 page 67.
/// @param  Position site        Nominal position of the site of interest.
/// @param  CommonTime time         Time of interest.
/// @param  EarthOrientation eo  Earth orientation parameters appropriate for time.
/// @return Triple disp          Displacement vector, WGS84 ECEF XYZ meters.
gpstk::Triple computePolarTides(gpstk::Position site,
                                gpstk::CommonTime time,
                                gpstk::EarthOrientation& eo)
   throw(gpstk::Exception);

#endif // SOLID_EARTH_TIDES_INCLUDE
