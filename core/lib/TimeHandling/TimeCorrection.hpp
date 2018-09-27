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

#ifndef GPSTK_TIMECORRECTION_HPP
#define GPSTK_TIMECORRECTION_HPP

namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

      /** Implement a correction for errors in week-rollover,
       * i.e. when the GPS 10-bit week changes from 1023 to 0.
       * @param[in] toCorrectWeek The suspect week from data that
       *   might be off by 1024 weeks.
       * @param[in,out] refWeek The known full week that will be used
       *   as a reference.  If this value is zero (the default), the
       *   local system clock will be consulted to obtain a reference
       *   week.  It is recommended for the best performance that the
       *   calling application retain this reference week for
       *   subsequent calls to this function as each time the function
       *   is called with a reference week of zero, system calls are
       *   performed to obtain the system clock, and conversions are
       *   performed to turn that time into GPS week-second time.  The
       *   week according to the system clock should be valid for at
       *   least a single week epoch, i.e. 1024 weeks (~19 years), so
       *   unless you're planning on leaving your software running for
       *   more than 19 years straight, it should be valid.
       * @return The value of toCorrectWeek having been adjusted by as
       *   many epochs as necessary to bring it to within half a GPS
       *   week epoch (i.e. 512 weeks) of the reference time.
       */
   long timeAdjustWeekRollover(long toCorrectWeek, long& refWeek);
      //@}
}

#endif // GPSTK_TIMECORRECTION_HPP
