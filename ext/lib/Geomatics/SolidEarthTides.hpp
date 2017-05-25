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

/// @file SolidEarthTides.hpp
/// Implement the formula for the displacement of a point fixed to the solid Earth
/// due to the solid Earth tides resulting from the influence of the Sun and Moon.
/// Reference IERS Conventions (1996) found in IERS Technical Note 21 (IERS).
/// NB. Currently only the largest terms are implemented, yielding a result accurate
/// to the millimeter level. Specifically, IERS pg 61 eq 8 and IERS pg 65 eq 17.
/// The second equation referenced here is the permanent component.
/// Class SolarSystem may be used to get Solar and Lunar ephemeris information,
/// including position and mass ratios.

//------------------------------------------------------------------------------------
#ifndef SOLID_EARTH_TIDES_INCLUDE
#define SOLID_EARTH_TIDES_INCLUDE

//------------------------------------------------------------------------------------
// system
// GPSTk
#include "Exception.hpp"
#include "EphTime.hpp"
#include "Position.hpp"
#include "Triple.hpp"
#include "IERSConvention.hpp"

namespace gpstk
{

   //---------------------------------------------------------------------------------
   /// Compute the site displacement due to solid Earth tides for the given Position
   /// (assumed to be fixed to the solid Earth) at the given time, given the position
   /// of the site of interest, positions and mass ratios of the sun and moon.
   /// Return a Triple containing the site displacement in ECEF XYZ coordinates with
   /// units meters.
   /// Reference IERS Conventions (1996) found in IERS Technical Note 21
   ///       and IERS Conventions (2003) found in IERS Technical Note 32
   ///       and IERS Conventions (2010) found in IERS Technical Note 36.
   /// NB. Currently only the largest terms are implemented, yielding a result
   /// accurate to the millimeter level. Specifically, TN21 pg 61 eq 8 and
   /// TN21 pg 65 eq 17.
   /// @param Position site  Nominal position of the site of interest.
   /// @param EphTime time   Time of interest.
   /// @param Position Sun   Position of the Sun at time
   /// @param Position Moon  Position of the Moon at time
   /// @param double EMRAT   Earth-to-Moon mass ratio (default to DE405 value)
   /// @param double SERAT   Sun-to-Earth mass ratio (default to DE405 value)
   /// @param IERSConvention IERS convention to use (default IERS2010)
   /// @return Triple        Displacement vector, ECEF XYZ in meters.
   Triple computeSolidEarthTides(const Position site,
                                 const EphTime time,
                                 const Position Sun,
                                 const Position Moon,
                                 const double EMRAT=81.30056,
                                 const double SERAT=332946.050894783285912,
                                 const IERSConvention iers=IERSConvention::IERS2010)
      throw(Exception);

   //---------------------------------------------------------------------------------
   /// Compute the site displacement due to rotational deformation due to polar motion
   /// for the given Position (assumed to fixed to the solid Earth) at the given time,
   /// given the polar motion angles at time (cf.EarthOrientation) and the IERS
   /// convention to use. Return a Triple containing the site displacement in
   /// ECEF XYZ coordinates with units meters.
   /// Reference IERS Conventions (1996) found in IERS Technical Note 21, ch. 7 pg 67.
   /// @param Position site  Nominal position of the site of interest.
   /// @param EphTime time   Time of interest.
   /// @param double xp,yp   Polar motion angles in arcsec (cf. EarthOrientation)
   /// @param IERSConvention IERS convention to use (default IERS2010)
   /// @return Triple disp   Displacement vector, ECEF XYZ in meters.
   Triple computePolarTides(const Position site, const EphTime time,
                            const double xp, const double yp,
                            const IERSConvention iers=IERSConvention::IERS2010)
      throw(Exception);

}  // end namespace gpstk

#endif // SOLID_EARTH_TIDES_INCLUDE
// nothing below this

