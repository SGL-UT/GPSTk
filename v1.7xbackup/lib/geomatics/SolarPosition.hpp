#pragma ident "$Id$"

/**
 * @file SolarPosition.hpp  Compute solar and lunar positions with a simple algorithm.
 */

// ======================================================================
// This software was developed by Applied Research Laboratories, The
// University of Texas at Austin under contract to an agency or agencies
// within the U.S. Department of Defense. The U.S. Government retains all
// rights to use, duplicate, distribute, disclose, or release this
// software.
// 
// Copyright 2008 The University of Texas at Austin
// ======================================================================

//------------------------------------------------------------------------------------
#ifndef SOLAR_POSITION_INCLUDE
#define SOLAR_POSITION_INCLUDE

//------------------------------------------------------------------------------------
// includes
// system
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// GPSTk
#include "DayTime.hpp"
#include "Position.hpp"

// geomatics

namespace gpstk {

   /// Compute the Position of the Sun in WGS84 ECEF coordinates.
   /// Ref. Astronomical Almanac pg C24, as presented on USNO web site; claimed
   /// accuracy is about 1 arcminute, when t is within 2 centuries of 2000.
   /// @param DayTime t  Input epoch of interest
   /// @param double AR  Output apparent angular radius of sun as seen at Earth (deg)
   /// @return Position  Position (ECEF) of the Sun at t
   Position SolarPosition(DayTime t, double& AR) throw();

   /// Compute the latitude and longitude of the Sun using a very simple algorithm.
   /// Adapted from sunpos by D. Coco ARL:UT 12/15/94
   /// @param DayTime t  Input epoch of interest
   /// @param double lat Output latitude of the Sun at t
   /// @param double lon Output longitude of the Sun at t
   void CrudeSolarPosition(DayTime t, double& lat, double& lon) throw();

   /// Compute the Position of the Moon in WGS84 ECEF coordinates.
   /// Ref. Astronomical Almanac 1990 D46
   /// @param DayTime t  Input epoch of interest
   /// @param double AR  Output apparent angular radius of moon as seen at Earth (deg)
   /// @return Position  Position (ECEF) of the Moon at t
   Position LunarPosition(DayTime t, double& AR) throw();

   /// Compute the fraction of the area of the Sun covered by the Earth as seen from
   /// another body (e.g. satellite).
   /// @param double Rearth  Apparent angular radius of Earth.
   /// @param double Rsun    Apparent angular radius of Sun.
   /// @param double dES     Angular separation of Sun and Earth.
   /// @return double factor Fraction (0 <= factor <= 1) of Sun area covered by Earth
   double shadowFactor(double Rearth, double Rsun, double dES) throw();

}  // end namespace gpstk

#endif // SOLAR_POSITION_INCLUDE
// nothing below this
