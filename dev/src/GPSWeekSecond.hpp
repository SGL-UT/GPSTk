/// @file GPSWeekSecond.hpp  Define GPS week and seconds-of-week; inherits WeekSecond

#ifndef GPSTK_GPSWEEKSECOND_HPP
#define GPSTK_GPSWEEKSECOND_HPP

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

#include "WeekSecond.hpp"

namespace gpstk
{
   /// This class handles GPS Week and Seconds-of-week. It inherits WeekSecond
   class GPSWeekSecond : public WeekSecond
   {
   public:

      /// Constructor.
      GPSWeekSecond(unsigned int w = 0,
                       double s = 0.,
                       TimeSystem ts = TimeSystem::GPS)
         : WeekSecond(w,s)
      { timeSystem = ts; }

      /// Constructor from CommonTime
      GPSWeekSecond( const CommonTime& right )
      {
         convertFromCommonTime( right );
         timeSystem = TimeSystem::GPS;
      }

      /// Destructor.
      ~GPSWeekSecond() {}
      
      /// Return the number of bits in the bitmask used to get the ModWeek from the
      /// full week.
      int Nbits(void) const
      {
         static const int n=10;
         return n;
      }

      /// Return the bitmask used to get the ModWeek from the full week.
      int bitmask(void) const
      {
         static const int bm=0x3FF;
         return bm;
      }

      /// Return the Julian Day (JDAY) of epoch for this system.
      long JDayEpoch(void) const
      {
         static const long e=GPS_EPOCH_JDAY;
         return e;
      }

   }; // end class GPSWeekSecond

} // namespace

#endif // GPSTK_GPSWEEKSECOND_HPP
