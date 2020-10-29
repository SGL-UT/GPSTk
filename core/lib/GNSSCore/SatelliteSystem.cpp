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

#include "SatelliteSystem.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(SatelliteSystem e) throw()
      {
         switch (e)
         {
            case SatelliteSystem::Unknown:     return "Unknown";
            case SatelliteSystem::GPS:         return "GPS";
            case SatelliteSystem::Galileo:     return "Galileo";
            case SatelliteSystem::Glonass:     return "GLONASS";
            case SatelliteSystem::Geosync:     return "Geostationary";
            case SatelliteSystem::LEO:         return "LEO";
            case SatelliteSystem::Transit:     return "Transit";
            case SatelliteSystem::BeiDou:      return "BeiDou";
            case SatelliteSystem::QZSS:        return "QZSS";
            case SatelliteSystem::IRNSS:       return "IRNSS";
            case SatelliteSystem::Mixed:       return "Mixed";
            case SatelliteSystem::UserDefined: return "UserDefined";
            default:                           return "???";
         } // switch (e)
      } // asString(SatelliteSystem)


      SatelliteSystem asSatelliteSystem(const std::string& s) throw()
      {
         if (s == "Unknown")
            return SatelliteSystem::Unknown;
         if (s == "GPS")
            return SatelliteSystem::GPS;
         if (s == "Galileo")
            return SatelliteSystem::Galileo;
         if (s == "GLONASS")
            return SatelliteSystem::Glonass;
         if (s == "Geostationary")
            return SatelliteSystem::Geosync;
         if (s == "LEO")
            return SatelliteSystem::LEO;
         if (s == "Transit")
            return SatelliteSystem::Transit;
         if (s == "BeiDou")
            return SatelliteSystem::BeiDou;
         if (s == "QZSS")
            return SatelliteSystem::QZSS;
         if (s == "IRNSS")
            return SatelliteSystem::IRNSS;
         if (s == "Mixed")
            return SatelliteSystem::Mixed;
         if (s == "UserDefined")
            return SatelliteSystem::UserDefined;
         return SatelliteSystem::Unknown;
      } // asSatelliteSystem(string)
   } // namespace StringUtils
} // namespace gpstk
