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

#include "ObservationType.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(ObservationType e) throw()
      {
         switch (e)
         {
            case ObservationType::Unknown:   return "Unknown";
            case ObservationType::Any:       return "Any";
            case ObservationType::Range:     return "Range";
            case ObservationType::Phase:     return "Phase";
            case ObservationType::Doppler:   return "Doppler";
            case ObservationType::SNR:       return "SNR";
            case ObservationType::Channel:   return "Channel";
            case ObservationType::DemodStat: return "DemodStat";
            case ObservationType::Iono:      return "Iono";
            case ObservationType::SSI:       return "SSI";
            case ObservationType::LLI:       return "LLI";
            case ObservationType::TrackLen:  return "TrackLen";
            case ObservationType::NavMsg:    return "NavMsg";
            case ObservationType::RngStdDev: return "RngStdDev";
            case ObservationType::PhsStdDev: return "PhsStdDev";
            case ObservationType::FreqIndx:  return "FreqIndx";
            case ObservationType::Undefined: return "Undefined";
            default:                         return "???";
         } // switch (e)
      } // asString(ObservationType)


      ObservationType asObservationType(const std::string& s) throw()
      {
         if (s == "Unknown")
            return ObservationType::Unknown;
         if (s == "Any")
            return ObservationType::Any;
         if (s == "Range")
            return ObservationType::Range;
         if (s == "Phase")
            return ObservationType::Phase;
         if (s == "Doppler")
            return ObservationType::Doppler;
         if (s == "SNR")
            return ObservationType::SNR;
         if (s == "Channel")
            return ObservationType::Channel;
         if (s == "DemodStat")
            return ObservationType::DemodStat;
         if (s == "Iono")
            return ObservationType::Iono;
         if (s == "SSI")
            return ObservationType::SSI;
         if (s == "LLI")
            return ObservationType::LLI;
         if (s == "TrackLen")
            return ObservationType::TrackLen;
         if (s == "NavMsg")
            return ObservationType::NavMsg;
         if (s == "RngStdDev")
            return ObservationType::RngStdDev;
         if (s == "PhsStdDev")
            return ObservationType::PhsStdDev;
         if (s == "FreqIndx")
            return ObservationType::FreqIndx;
         if (s == "Undefined")
            return ObservationType::Undefined;
         return ObservationType::Unknown;
      } // asObservationType(string)
   } // namespace StringUtils
} // namespace gpstk
