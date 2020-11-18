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

#ifndef GPSTK_OBSERVATIONTYPE_HPP
#define GPSTK_OBSERVATIONTYPE_HPP

#include <string>
#include "EnumIterator.hpp"

namespace gpstk
{
      /// The type of observation, mostly used by ObsID.
   enum class ObservationType
   {
      Unknown,
      Any,       ///< Used to match any observation type
      Range,     ///< pseudorange, in meters
      Phase,     ///< accumulated phase, in cycles
      Doppler,   ///< Doppler, in Hz
      SNR,       ///< Signal strength, in dB-Hz
      Channel,   ///< Channel number
      DemodStat, ///< Demodulator status
      Iono,      ///< Ionospheric delay
      SSI,       ///< Signal Strength Indicator (RINEX)
      LLI,       ///< Loss of Lock Indicator (RINEX)
      TrackLen,  ///< Number of continuous epochs of 'good' tracking
      NavMsg,    ///< Navigation Message data
      RngStdDev, ///< pseudorange standard deviation, in meters
      PhsStdDev, ///< phase standard deviation, in cycles
      FreqIndx,  ///< GLONASS frequency offset index
      Undefined, ///< Observation type is known to be undefined (as opposed to unknown)
      Last,      ///< Used to verify that all items are described at compile time
   }; // enum class ObservationType

      /** Define an iterator so C++11 can do things like
       * for (ObservationType i : ObservationTypeIterator()) */
   typedef EnumIterator<ObservationType, ObservationType::Unknown, ObservationType::Last> ObservationTypeIterator;

   namespace StringUtils
   {
         /// Convert a ObservationType to a whitespace-free string name.
      std::string asString(ObservationType e) throw();
         /// Convert a string name to an ObservationType
      ObservationType asObservationType(const std::string& s) throw();
   }
} // namespace gpstk

#endif // GPSTK_OBSERVATIONTYPE_HPP
