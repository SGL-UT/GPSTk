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

/** @file GPSWeekSecond.hpp Define GPS week and seconds-of-week;
 * inherits WeekSecond */

#ifndef GPSWEEKSECOND_HPP
#define GPSWEEKSECOND_HPP

#include "WeekSecond.hpp"

namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

      /// This class handles GPS Week and Seconds-of-week. It inherits
      /// WeekSecond
      /// The GPS week is specified by 10-bit ModWeek, rollover at
      /// 1024, bitmask 0x3FF and epoch GPS_EPOCH_MJD
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
      }

         /// Destructor.
      ~GPSWeekSecond() {}
      
         /// Return the number of bits in the bitmask used to get the
         /// ModWeek from the full week.
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

         /// Return the Modified Julian Date (MJD) of epoch for this system.
      long MJDEpoch(void) const
      {
         static const long e=GPS_EPOCH_MJD;
         return e;
      }

         /** @name WeekSecond Comparison Operators
          * All comparison operators have a parameter "right" which
          * corresponds to the GPSWeekSecond object to the right of
          * the symbol.  All comparison operators are const and return
          * true on success and false on failure. */
         //@{
      inline bool operator==( const GPSWeekSecond& right ) const
      {
         return WeekSecond::operator==( right );
      }
      inline bool operator!=( const GPSWeekSecond& right ) const
      {
         return WeekSecond::operator!=( right );
      }
      inline bool operator<( const GPSWeekSecond& right ) const
      {
         return WeekSecond::operator<( right );
      }
      inline bool operator>( const GPSWeekSecond& right ) const
      {
         return WeekSecond::operator>( right );
      }
      inline bool operator<=( const GPSWeekSecond& right ) const
      {
         return WeekSecond::operator<=( right );
      }
      inline bool operator>=( const GPSWeekSecond& right ) const
      {
         return WeekSecond::operator>=( right );
      }
         //@}

         /// Return a string containing the characters that this class
         /// understands when printing times.
      virtual std::string getPrintChars() const
      {
         return "EFGwgP";
      }

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
      {
         return "%F %g %P";
      }

         /// This function formats this time to a string.  The exceptions
         /// thrown would only be due to problems parsing the fmt string.
      virtual std::string printf(const std::string& fmt) const
      {
         try {
            using gpstk::StringUtils::formattedPrint;

            std::string rv = fmt;
            rv = formattedPrint( rv, getFormatPrefixInt() + "E",
                                 "Eu", getEpoch() );
            rv = formattedPrint( rv, getFormatPrefixInt() + "F",
                                 "Fu", week );
            rv = formattedPrint( rv, getFormatPrefixInt() + "G",
                                 "Gu", getModWeek() );
            rv = formattedPrint( rv, getFormatPrefixInt() + "w",
                                 "wu", getDayOfWeek() );
            rv = formattedPrint( rv, getFormatPrefixFloat() + "g",
                                 "gf", sow );
            rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                                 "Ps", timeSystem.asString().c_str() );
            return rv;
         }
         catch(gpstk::StringUtils::StringException& e)
         { GPSTK_RETHROW(e); }
      }

         /// This function works similarly to printf.  Instead of filling
         /// the format with data, it fills with error messages.
      virtual std::string printError(const std::string& fmt) const
      {
         try {
            using gpstk::StringUtils::formattedPrint;
            std::string rv = fmt;

            rv = formattedPrint( rv, getFormatPrefixInt() + "E",
                                 "Es", "BadGPSepoch");
            rv = formattedPrint( rv, getFormatPrefixInt() + "F",
                                 "Fs", "BadGPSfweek");
            rv = formattedPrint( rv, getFormatPrefixInt() + "G",
                                 "Gs", "BadGPSmweek");
            rv = formattedPrint( rv, getFormatPrefixInt() + "w",
                                 "ws", "BadGPSdow");
            rv = formattedPrint( rv, getFormatPrefixFloat() + "g",
                                 "gs", "BadGPSsow");
            rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                                 "Ps", "BadGPSsys");
            return rv;
         }
         catch(gpstk::StringUtils::StringException& e)
         { GPSTK_RETHROW(e); }
      }

         /// Set this object using the information provided in \a info.
         /// @param info the IdToValue object to which this object shall be set.
         /// @return true if this object was successfully set using the
         ///  data in \a info, false if not.
      bool setFromInfo( const IdToValue& info )
      {
         for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
         {
               // based on the character, we know what to do...
            switch ( i->first )
            {
               case 'E':
                  setEpoch( gpstk::StringUtils::asInt( i->second ) );
                  break;
               case 'F':
                  week = gpstk::StringUtils::asInt( i->second );
                  break;
               case 'G':
                  setModWeek( gpstk::StringUtils::asInt( i->second ) );
                  break;
               case 'w':
                  sow = static_cast<double>(gpstk::StringUtils::asInt(i->second))*SEC_PER_DAY;
                  break;
               case 'g':
                  sow = gpstk::StringUtils::asDouble( i->second );
                  break;
               case 'P':
                  timeSystem.fromString(i->second);
                  break;
               default:
                     // do nothing
                  break;
            };

         } // end of for loop

         return true;
      }

   }; // end class GPSWeekSecond

      //@}

} // namespace

#endif // GPSWEEKSECOND_HPP
