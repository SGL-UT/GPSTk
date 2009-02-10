#pragma ident "$Id: TimeString.hpp 1162 2008-03-27 21:18:13Z snelsen $"



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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

#include "TimeTag.hpp"
#include "CommonTime.hpp"

namespace gpstk
{
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
