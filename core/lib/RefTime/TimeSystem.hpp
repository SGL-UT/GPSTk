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

/// @file TimeSystem.hpp

#ifndef GPSTK_TIMESYSTEM_HPP
#define GPSTK_TIMESYSTEM_HPP

#include <iostream>
#include <string>

namespace gpstk
{
      /// Definition of various time systems.
   enum class TimeSystem
   {
         // Add new systems BEFORE count, then
         // add to asString() in TimeSystem.cpp and make parallel
         // to this enum.

         // Unknown MUST BE FIRST, and must = 0
      Unknown = 0, ///< unknown time frame; for legacy code compatibility
      Any,         ///< wildcard; allows comparison with any other type
      GPS,         ///< GPS system time
      GLO,         ///< GLONASS system time
      GAL,         ///< Galileo system time
      QZS,         ///< QZSS system Time
      BDT,         ///< BeiDou system Time
      IRN,         ///< IRNSS system Time
      UTC,         ///< Coordinated Universal Time (e.g., from NTP)
      TAI,         ///< International Atomic Time
      TT,          ///< Terrestrial time (used in IERS conventions)
      TDB,         ///< Barycentric dynamical time (JPL ephemeris); very near TT
         // Last MUST BE LAST
      Last        ///< Used to verify that all items are described at compile time
   };


      /** Return the number of leap seconds between UTC and TAI, that
       * is the difference in time scales UTC-TAI, at an epoch defined
       * by year/month/day.
       * @note Input day in a floating quantity and thus any epoch may
       *   be represented; this is relevant the period 1960 to 1972,
       *   when UTC-TAI was not integral.
       * @note GPS = TAI-19sec and so GPS-UTC = getLeapSeconds()-19 == dtLS.
       * @note GLO = UTC = GPS - dtLS. but not incl. RINEX::TIME
       *   SYSTEM CORR::GPUT
       * @note GLO is actually UTC(SU) Moscow
       * @note GAL = GPS = UTC + dtLS this does not incl. RINEX::TIME
       *   SYSTEM CORR::GAUT
       * @note BDT = GPS - 15 but this does not include RINEX::TIME
       *   SYSTEM CORR::BDUT
       * @note BDT is actually UTC(NTSC) China
       *
       * @note The table 'leaps' must be modified when a new leap
       *   second is announced.
       *
       * @param[in] yr year of interest
       * @param[in] mon month of interest
       * @param[in] day day of interest
       */
   double getLeapSeconds(const int yr, const int mon, const double day);

      /** Compute the conversion (in seconds) from one time system
       * (inTS) to another (outTS), given the year and month of the
       * time to be converted.  Result is to be added to the first
       * time (inTS) to yield the second (outTS), that is t(outTS) =
       * t(inTS) + correction(inTS,outTS).
       * @note caller must not forget to change to outTS after adding
       *   this correction.
       * @param[in] inTS input system
       * @param[in] outTS output system
       * @param[in] year year of the time to be converted.
       * @param[in] month month (1-12) of the time to be converted.
       * @return correction (sec) to be added to t(in) to yield t(out).
       * @throw if input system(s) are invalid or Unknown.
       */
   double getTimeSystemCorrection(
      const TimeSystem inTS, const TimeSystem outTS,
      const int year, const int month, const double day);

      /** Write name (asString()) of a TimeSystem to an output stream.
       * @param[in,out] os The output stream
       * @param[in] ts The TimeSystem to be written
       * @return reference to the output stream
       */
   std::ostream& operator<<(std::ostream& os, const TimeSystem ts);

   namespace StringUtils
   {
         /// Convert a TimeSystem enum to its string representation.
      std::string asString(TimeSystem ts);
         /// Convert a string representation of TimeSystem to an enum.
      TimeSystem asTimeSystem(const std::string& s);
   }

}   // end namespace

#endif // GPSTK_TIMESYSTEM_HPP
