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
//  Copyright 2016, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
// This software developed by Applied Research Laboratories at the
// University of Texas at Austin, under contract to an agency or
// agencies within the U.S.  Department of Defense. The
// U.S. Government retains all rights to use, duplicate, distribute,
// disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file NMCTMeta.hpp
 * Provides class for containing NMCT-related data from subframe 2 (GPS LNAV).
 */

#ifndef NMCTMETA_HPP
#define NMCTMETA_HPP

#include <stdint.h>
#include <CommonTime.hpp>

namespace gpstk
{
      /** Encapsulate the time data related to the NMCT (subframe 4 pg 13). */
   class NMCTMeta
   {
   public:
         /// Initialize data members.
      NMCTMeta();
      unsigned short prn; ///< Transmitting satellite of subframe 2.
      CommonTime tot;     ///< Time of transmission of subframe 2.
      uint32_t aodo;      ///< Age-of-data offset (in seconds).
      CommonTime tnmct;   ///< NMCT validity time.
      CommonTime toe;     ///< Ephemeris validity time.
   };
}

#endif // NMCTMETA_HPP
