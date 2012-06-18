#pragma ident "$Id$"



#ifndef GPSTK_TIMESTRING_HPP
#define GPSTK_TIMESTRING_HPP

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

#include "TimeTag.hpp"
#include "CommonTime.hpp"

namespace gpstk
{
      /**
       * The TimeTag classes are the "interface" for CommonTime, so
       * when printing a CommonTime object, each of the TimeTag printf()
       * functions are called to handle the print identifiers that it 
       * recognizes.  The following is a list of these identifiers and 
       * the meaning for each:
       *
       * - ANSITime:
       *   - K     integer seconds since Unix Epoch (00:00, Jan 1, 1970 UTC)
       *
       * - CivilTime:
       *   - Y     integer 4-digit year
       *   - y     integer 2-digit year
       *   - m     integer month
       *   - b     abbreviated month name string (e.g. "Jan")
       *   - B     full month name string (e.g. "January")
       *   - d     integer day-of-month
       *   - H     integer hour-of-day
       *   - M     integer minute-of-hour
       *   - S     integer second-of-minute
       *   - f     float second-of-minute
       *
       * - GPSWeek:
       *   - E     integer GPS Epoch
       *   - F     integer full (13-bit) GPS Week
       *   - G     integer 10-bit GPS Week
       * 
       * - GPSWeekSecond:
       *   - w     integer GPS day-of-week
       *   - g     float GPS second-of-week
       *
       * - GPSWeekZcount:
       *   - w     integer GPS day-of-week
       *   - z     integer GPS Z-count
       *   - Z     integer GPS Z-count
       *   - c     integer 29-bit Z-count
       *   - C     integer 32-bit Z-count
       *
       * - JulianDate:
       *   - J     float Julian Date
       *
       * - MJD:
       *   - Q     float Modified Julian Date
       *
       * - UnixTime:
       *   - U     integer seconds since Unix Epoch (00:00, Jan 1, 1970 UTC)
       *   - u     integer microseconds
       *
       * - YDSTime:
       *   - Y     integer 4-digit year
       *   - y     integer 2-digit year
       *   - j     integer day-of-year
       *   - s     integer second-of-day
       *
       * - Common Identifiers:
       *   - P     Print the object's TimeSystem as a string.
       */
   std::string printTime( const CommonTime& t,
                          const std::string& fmt )
      throw( gpstk::StringUtils::StringException );

      /// This function converts the given CommonTime into the templatized
      /// TimeTag object, before calling the TimeTag's printf(fmt).  If 
      /// there's an error in conversion, it instead calls printf(fmt, true)
      /// to signal a conversion error.
   template <class TimeTagType>
   std::string printAs( const CommonTime& t,
                        const std::string& fmt )
      throw( gpstk::StringUtils::StringException )
   {
      TimeTagType ttt;
      try
      {
         ttt.convertFromCommonTime(t);
         return ttt.printf(fmt);
      }
      catch (InvalidRequest& ir)
      {
         return ttt.printError(fmt);
      }
   }
   
      /// Fill the TimeTag object \a btime with time information found in
      /// string \a str formatted according to string \a fmt.
   void scanTime( TimeTag& btime,
                  const std::string& str,
                  const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException );
   
   void scanTime( CommonTime& t,
                  const std::string& str,
                  const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException );

      /** This function is like the other scanTime functions except that
       *  it allows mixed time formats. 
       *  i.e. Year / 10-bit GPS week / seconds-of-week
       *  The time formats are filled in the following order: GPS Epoch,
       *  year, month, GPS Full Week, GPS 10-bit Week, day-of-week,
       *  day-of-month, day-of-year, 29-bit Zcount, 19-bit Zcount, hour,
       *  minute, second-of-week, second-of-day, second-of-minute.
       *  @note MJD, Julian Date, ANSI time, Unix time, and 32-bit Zcount are
       *  treated as stand-alone types and are not mixed with others if
       *  detected.
       */
   void mixedScanTime( CommonTime& t,
                       const std::string& str,
                       const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException );
} // namespace

#endif // GPSTK_TIMESTRING_HPP
