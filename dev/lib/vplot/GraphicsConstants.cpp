#pragma ident "$Id: //depot/msn/r5.3/wonky/gpstkplot/lib/draw/GraphicsConstants.cpp#2 $"

/// @file GraphicsConstants.cpp Defines constants. 

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

#include <iomanip>
#include "GraphicsConstants.hpp"

namespace vplot
{

   const double PI = 3.14159265358979323846264;
   const double DEGREES_TO_RADIANS = PI/180.;
   const double RADIANS_TO_DEGREES = 1/DEGREES_TO_RADIANS;

  /// Units conversion from points to inches
  const double PTS_PER_INCH=72;
  /// Units conversion from points to centimeters
  const double PTS_PER_CM=72/2.54;

  /// U.S. Customary page size width in points
  const double US_LETTER_WIDTH_PTS=612;
  /// U.S. Customary page size height in points;
  const double US_LETTER_HEIGHT_PTS=792;

  // For the rest of the world... there's ISO 216-series sizes
  /// A3 page size width in points
  const double A3_WIDTH_PTS=841.9;
  /// A3 page size height in points
  const double A3_HEIGHT_PTS=1190.6;

  // A4 and US customary letter are of comparable size
  /// A4 page size width in points
  const double A4_WIDTH_PTS=595.3;
  /// A4 page size height in points 
  const double A4_HEIGHT_PTS=841.9;

  /// A5 page size width in points
  const double A5_WIDTH_PTS=419.5;
  /// A5 page size height in points
  const double A5_HEIGHT_PTS=595.3;

} // namespace vplot

