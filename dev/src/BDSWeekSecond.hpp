/// @file BDSWeekSecond.hpp  Define BDS week and seconds-of-week; inherits WeekSecond

#ifndef GPSTK_BDSWEEKSECOND_HPP
#define GPSTK_BDSWEEKSECOND_HPP

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
   /// This class handles the week portion of the BDS TimeTag classes.
   /// The BDS week is specified by
   /// 13-bit ModWeek, rollover at 8192, bitmask 0x1FFF and epoch BDS_EPOCH_JDAY
   class BDSWeekSecond : public WeekSecond
   {
   public:

      /// Constructor.
      BDSWeekSecond(unsigned int w = 0,
                       double s = 0.,
                       TimeSystem ts = TimeSystem::BDS)
         : WeekSecond(w,s)
      { timeSystem = ts; }

      /// Constructor from CommonTime
      BDSWeekSecond( const CommonTime& right )
      {
         convertFromCommonTime( right );
      }

      /// Destructor.
      ~BDSWeekSecond() throw() {}

      // TD do we want to do this?
      // TD throw here? this would require changing throw spec on TimeTag version
      /// Override routine in TimeTag, allowing only BDS time system
      void setTimeSystem(const TimeSystem& timeSys)
      {
         // ?? if(timeSys != TimeSystem::BDS) GPSTK_THROW(InvalidRequest(""));
         timeSystem = TimeSystem::BDS;
      }

      // the rest define the week rollover and starting time

      /// Return the number of bits in the bitmask used to get the ModWeek from the
      /// full week.
      int Nbits(void) const
      {
         static const int n=13;
         return n;
      }

      /// Return the bitmask used to get the ModWeek from the full week.
      int bitmask(void) const
      {
         static const int bm=0x1FFF;
         return bm;
      }

      /// Return the Julian Day (JDAY) of epoch for this system.
      long JDayEpoch(void) const
      {
         static const long e=BDS_EPOCH_JDAY;
         return e;
      }

   }; // end class BDSWeekSecond

} // namespace

#endif // GPSTK_BDSWEEKSECOND_HPP
