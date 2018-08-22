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
//  Copyright 2018, The University of Texas at Austin
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

#include "TimeCorrection.hpp"
#include "SystemTime.hpp"
#include "GPSWeekSecond.hpp"

namespace gpstk
{

   long timeAdjustWeekRollover(long toCorrectWeek, long& refWeek)
   {
         // gotta use signed ints so that the weeks can actually go
         // negative when doing subtraction and comparison.
      long rv = toCorrectWeek;
      if (refWeek == 0)
      {
            // One might be inclined to try to cache the refWeek value
            // from the clock internally, but that would mean that
            // applications would be unable to process data from
            // different epochs.  An unusual situation, to be sure,
            // but I'd rather not make assumptions about everyone's
            // use cases.
         GPSWeekSecond ref = SystemTime().convertToCommonTime();
         refWeek = ref.week;
      }
         // GPS_WEEK_PER_EPOCH >> 1 is the same as /2, i.e. half-epoch.
         // The while loops allow us to correct data that's more than
         // one "epoch" out.
      while (refWeek - rv > (GPS_WEEK_PER_EPOCH>>1))
      {
         rv += GPS_WEEK_PER_EPOCH;
      }
      while (refWeek - rv < -(GPS_WEEK_PER_EPOCH>>1))
      {
         rv -= GPS_WEEK_PER_EPOCH;
      }
      return rv;
   }

} // namespace gpstk
