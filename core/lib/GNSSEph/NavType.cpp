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

#include "NavType.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(NavType e) throw()
      {
         switch (e)
         {
            case NavType::GPSLNAV:   return "GPS_LNAV";
            case NavType::GPSCNAVL2: return "GPS_CNAV_L2";
            case NavType::GPSCNAVL5: return "GPS_CNAV_L5";
            case NavType::GPSCNAV2:  return "GPS_CNAV2";
            case NavType::GPSMNAV:   return "GPS_MNAV";
            case NavType::BeiDou_D1: return "Beidou_D1";
            case NavType::BeiDou_D2: return "Beidou_D2";
            case NavType::GloCivilF: return "GloCivilF";
            case NavType::GloCivilC: return "GloCivilC";
            case NavType::GalFNAV:   return "GalFNAV";
            case NavType::GalINAV:   return "GalINAV";
            case NavType::IRNSS_SPS: return "IRNSS_SPS";
            case NavType::Unknown:   return "Unknown";
            default:                 return "???";
         } // switch (e)
      } // asString(NavType)


      NavType asNavType(const std::string& s) throw()
      {
         if (s == "GPS_LNAV")
            return NavType::GPSLNAV;
         if (s == "GPS_CNAV_L2")
            return NavType::GPSCNAVL2;
         if (s == "GPS_CNAV_L5")
            return NavType::GPSCNAVL5;
         if (s == "GPS_CNAV2")
            return NavType::GPSCNAV2;
         if (s == "GPS_MNAV")
            return NavType::GPSMNAV;
         if (s == "Beidou_D1")
            return NavType::BeiDou_D1;
         if (s == "Beidou_D2")
            return NavType::BeiDou_D2;
         if (s == "GloCivilF")
            return NavType::GloCivilF;
         if (s == "GloCivilC")
            return NavType::GloCivilC;
         if (s == "GalFNAV")
            return NavType::GalFNAV;
         if (s == "GalINAV")
            return NavType::GalINAV;
         if (s == "IRNSS_SPS")
            return NavType::IRNSS_SPS;
         if (s == "Unknown")
            return NavType::Unknown;
         return NavType::Unknown;
      } // asNavType(string)
   } // namespace StringUtils
} // namespace gpstk
