/** @warning This code is automatically generated.
 *
 *  DO NOT EDIT THIS CODE BY HAND.
 *
 *  Refer to the documenation in the toolkit_docs gitlab project.
 */

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

#ifndef GPSTK_SATELLITESYSTEM_HPP
#define GPSTK_SATELLITESYSTEM_HPP

#include <string>
#include "EnumIterator.hpp"

namespace gpstk
{
      /// Supported satellite systems
   enum class SatelliteSystem
   {
      Unknown,
      GPS,
      Galileo,
      Glonass,
      Geosync,
      LEO,
      Transit,
      BeiDou,      ///< aka Compass
      QZSS,
      IRNSS,       ///< Official name changed from IRNSS to NavIC
      Mixed,
      UserDefined,
      Last,        ///< Used to verify that all items are described at compile time
   }; // enum class SatelliteSystem

      /** Define an iterator so C++11 can do things like
       * for (SatelliteSystem i : SatelliteSystemIterator()) */
   typedef EnumIterator<SatelliteSystem, SatelliteSystem::Unknown, SatelliteSystem::Last> SatelliteSystemIterator;

   namespace StringUtils
   {
         /// Convert a SatelliteSystem to a whitespace-free string name.
      std::string asString(SatelliteSystem e) throw();
         /// Convert a string name to an SatelliteSystem
      SatelliteSystem asSatelliteSystem(const std::string& s) throw();
   }

      /** Translate system enumeration to its string representation.
       * @note The string representation is being used in file
       *   formats, e.g. RawNavCSVHeader.  The string values should
       *   not be changed if at all possible, as that would break
       *   the ability to read older files.
       * @note Any new systems should not contain spaces in the
       *   string values.
       * @note The translations here should precisely match those
       *   in convertStringToSatelliteSystem.
       * @param[in] s The system to get the string name of.
       * @return A space-free string containing the name of the GNSS.
       */
   inline std::string convertSatelliteSystemToString(SatelliteSystem s)
   { return StringUtils::asString(s); }

      /** Translate GNSS names as strings into system enumeration
       * equivalents.
       * @see convertSatelliteSystemToString
       * @param[in] s The GNSS name to convert to enumeration.
       * @return An enumeration equivalent of the given string.
       *   Unknown is returned for any names that do not
       *   exactly match known values.
       */
   inline SatelliteSystem convertStringToSatelliteSystem(
      const std::string& s)
   { return StringUtils::asSatelliteSystem(s); }

} // namespace gpstk

#endif // GPSTK_SATELLITESYSTEM_HPP
