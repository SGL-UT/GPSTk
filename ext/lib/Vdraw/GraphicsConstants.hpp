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

/// @file GraphicsConstants.hpp Declares constants. 

#ifndef VDRAW_GRAPHIC_CONSTANTS_H
#define VDRAW_GRAPHIC_CONSTANTS_H


namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /// The value of pie to 24 decimal places
   extern const double PI;
      /// pi / 2
   extern const double HALF_PI;
      /// pi * 2
   extern const double TWO_PI;
      /// pi / 180
   extern const double DEGREES_TO_RADIANS; 
      /// 180 / pi
   extern const double RADIANS_TO_DEGREES;

      /// Units conversion from points to inches
   extern const double PTS_PER_INCH;

      /// Units conversion from points to centimeters
   extern const double PTS_PER_CM;

      /// U.S. Customary page size width given in points
   extern const double US_LETTER_WIDTH_PTS;
      /// U.S. Customary page size height given in points
   extern const double US_LETTER_HEIGHT_PTS;

      // For the rest of the world... there's ISO 216-series sizes
      /// A3 page size width given in points
   extern const double A3_WIDTH_PTS;
      /// A3 page size height given in points
   extern const double A3_HEIGHT_PTS;

      // A4 and US customary letter are of comparable size
      /// A4 page size width given in points
   extern const double A4_WIDTH_PTS;
      /// A4 page size height given in points 
   extern const double A4_HEIGHT_PTS;

      /// A5 page size width given in points
   extern const double A5_WIDTH_PTS;
      /// A5 page size height given in points
   extern const double A5_HEIGHT_PTS;

      //@}

} // namespace vdraw

#endif //VDRAW_GRAPHIC_CONSTANTS_H
