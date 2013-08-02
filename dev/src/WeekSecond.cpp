/// @file WeekSecond.cpp  Implement full week, mod week and seconds-of-week time
/// representation.

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
#include "TimeConstants.hpp"
#include "JulianDate.hpp"

namespace gpstk
{
   WeekSecond& WeekSecond::operator=( const WeekSecond& right )
   {
      Week::operator=(right);
      sow = right.sow;
      return *this;
   }

   CommonTime WeekSecond::convertToCommonTime() const
   {
      try
      {
	      //int dow = static_cast<int>( sow * DAY_PER_SEC );
         // Appears to have rounding issues on 32-bit platforms

         int dow = static_cast<int>( sow / SEC_PER_DAY );
         int jday = JDayEpoch() + ( 7 * week ) + dow;
         double sod(  sow - SEC_PER_DAY * dow );
         CommonTime ct;
         return ct.set( jday,
                        static_cast<long>( sod ),
                        sod - static_cast<long>( sod ),
                        timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ir);
      }
   }

   void WeekSecond::convertFromCommonTime( const CommonTime& ct )
   {
      if(static_cast<JulianDate>(ct).jd < JDayEpoch())
      {
         InvalidRequest ir("Unable to convert to Week/Second - before Epoch.");
         GPSTK_THROW(ir);
      }

      long day, sod;
      double fsod;
      ct.get( day, sod, fsod, timeSystem );
         // find the number of days since the beginning of the Epoch
      day -= JDayEpoch();
         // find out how many weeks that is
      week = static_cast<int>( day / 7 );
         // find out what the day of week is
      day %= 7;

      sow = static_cast<double>( day * SEC_PER_DAY + sod ) + fsod;
   }

   std::string WeekSecond::printf( const std::string& fmt ) const
   {
      try
      {
         using StringUtils::formattedPrint;

         std::string rv = Week::printf( fmt );

         rv = formattedPrint( rv, getFormatPrefixInt() + "w",
                              "wu", getDayOfWeek() );
         rv = formattedPrint( rv, getFormatPrefixFloat() + "g",
                              "gf", sow);
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", timeSystem.asString().c_str() );
         return rv;
      }
      catch( StringUtils::StringException& exc )
      {
         GPSTK_RETHROW( exc );
      }
   }

   std::string WeekSecond::printError( const std::string& fmt ) const
   {
      try
      {
         using StringUtils::formattedPrint;

         std::string rv = Week::printError( fmt );

         rv = formattedPrint( rv, getFormatPrefixInt() + "w",
                              "ws", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixFloat() + "g",
                              "gs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str() );
         return rv;
      }
      catch( StringUtils::StringException& exc )
      {
         GPSTK_RETHROW( exc );
      }
   }

   bool WeekSecond::setFromInfo( const IdToValue& info )
   {
      using namespace StringUtils;

      Week::setFromInfo( info );

      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
            // based on the character, we know what to do...
         switch ( i->first )
         {
            case 'w':
               sow = static_cast<double>( asInt( i->second ) ) * SEC_PER_DAY;
               break;
            case 'g':
               sow = asDouble( i->second );
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

   bool WeekSecond::isValid() const
   {
      return ( Week::isValid() &&
               sow < FULLWEEK );
   }

   void WeekSecond::reset()
   {
      Week::reset();
      sow = 0.0;
   }

   bool WeekSecond::operator==( const WeekSecond& right ) const
   {
      return ( Week::operator==(right) &&
               sow == right.sow );
   }

   bool WeekSecond::operator!=( const WeekSecond& right ) const
   {
      return ( !operator==( right ) );
   }

   bool WeekSecond::operator<( const WeekSecond& right ) const
   {
      if( Week::operator<(right) )
      {
         return true;
      }
      if( Week::operator>(right) )
      {
         return false;
      }
      if( sow < right.sow )
      {
         return true;
      }
      return false;
   }

   bool WeekSecond::operator>( const WeekSecond& right ) const
   {
      return ( !operator<=( right ) );
   }

   bool WeekSecond::operator<=( const WeekSecond& right ) const
   {
      return ( operator<( right ) || operator==( right ) );
   }

   bool WeekSecond::operator>=( const WeekSecond& right ) const
   {
      return ( !operator<( right ) );
   }

} // namespace
