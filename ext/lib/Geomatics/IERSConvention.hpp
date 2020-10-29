//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file IERSConvention.hpp
/// Include file defining the IERSConvention class.

#ifndef CLASS_IERSCONVENTION_INCLUDE
#define CLASS_IERSCONVENTION_INCLUDE

#include <iostream>
#include <string>
#include "Exception.hpp"

namespace gpstk
{

      /** This enum encapsulates the choice of IERS Convention, which
       * applies directly to the operation of class EarthOrientation,
       * and is used in class SolarSystem and in the functions defined
       * in SolidEarthTides.cpp.  The IERS convention determines the
       * precise form of frame transformations between the
       * conventional terrestrial frame and the conventional inertial
       * frame, as well as the solid earth tides.
       *
       * References:
       * IERS1996: IERS Technical Note 21, "IERS Conventions (1996),"
       *   Dennis D. McCarthy, U.S. Naval Observatory, 1996.
       * IERS2003: IERS Technical Note 32, "IERS Conventions (2003),"
       *   Dennis D. McCarthy and Gerard Petit eds., U.S. Naval Observatory and
       *   Bureau International des Poids et Mesures, 2004.
       * IERS2010: IERS Technical Note 36, "IERS Conventions (2010),"
       *   Gerard Petit and Brian Luzum eds., Bureau International des
       *   Poids et Mesures and U.S. Naval Observatory, 2010.
       */
   enum class IERSConvention
   {
      Unknown = 0,      // 0 MUST be first
      IERS1996,
      IERS2003,
      IERS2010,
      Last          // the number of conventions; this must be last
   };

   namespace StringUtils
   {
         /// Convert a IERSConvention enum to its string representation.
      std::string asString(IERSConvention e);
         /// Convert a string representation of IERSConvention to an enum.
      IERSConvention asIERSConvention(const std::string& s);
   }

      /** Write name (asString()) of a Convention to an output stream.
       * @param[in,out] os The output stream
       * @param[in] cv The Convention to be written
       * @return reference to the output stream */
   inline std::ostream& operator<<(std::ostream& os, IERSConvention cv)
   { return os << StringUtils::asString(cv); }

} // end namespace gpstk

#endif  // define CLASS_IERSCONVENTION_INCLUDE
