#pragma ident "$Id$"



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

#include "GPSWeekSecond.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   GPSWeekSecond& 
   GPSWeekSecond::operator=( const GPSWeekSecond& right )
      throw()
   {
      GPSWeek::operator=(right);
      sow = right.sow;
      return *this;
   }
   
   CommonTime GPSWeekSecond::convertToCommonTime() const
      throw(InvalidRequest)
   {
      try
      {
         int dow = static_cast<int>( sow * DAY_PER_SEC );
         int jday = GPS_EPOCH_JDAY + ( 7 * week ) + dow;
         double sod(  sow - SEC_PER_DAY * dow );
         return CommonTime( jday, 
                            static_cast<long>( sod ),
                            sod - static_cast<long>( sod ) );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ir);
      }
   }
   
   void GPSWeekSecond::convertFromCommonTime( const CommonTime& ct )
      throw(InvalidRequest)
   {
         /// This is the earliest CommonTime convertible to GPSWeekSecond.
      static const CommonTime MIN_CT = GPSWeekSecond();

      if (ct < MIN_CT)
      {
         InvalidRequest ir("Unable to convert CommonTime to GPSWeekSecond.");
         GPSTK_THROW(ir);
      }
      
      long day, sod;
      double fsod;
      ct.get( day, sod, fsod );

         // find the number of days since the beginning of the GPS Epoch
      day -= GPS_EPOCH_JDAY;
         // find out how many weeks that is
      week = static_cast<int>( day / 7 );
         // find out what the day of week is
      day %= 7;

      sow = static_cast<double>( day * SEC_PER_DAY + sod ) + fsod;
   }
   
   std::string GPSWeekSecond::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         
         std::string rv = GPSWeek::printf( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "w",
                              "wu", getDayOfWeek() );
         rv = formattedPrint( rv, getFormatPrefixFloat() + "g",
                              "gf", sow);
         return rv;
      }
      catch( gpstk::StringUtils::StringException& exc )
      {
         GPSTK_RETHROW( exc );
      }
   }
      
   std::string GPSWeekSecond::printError( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         
         std::string rv = GPSWeek::printError( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "w",
                              "ws", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixFloat() + "g",
                              "gs", getError().c_str() );               
         return rv;
      }
      catch( gpstk::StringUtils::StringException& exc )
      {
         GPSTK_RETHROW( exc );
      }
   }
   
   bool GPSWeekSecond::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;

      GPSWeek::setFromInfo( info );

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
            default:
                  // do nothing
               break;
         };
         
      } // end of for loop
      
      return true;
   }

   bool GPSWeekSecond::isValid() const
      throw()
   {
      return ( GPSWeek::isValid() &&
               sow < FULLWEEK );
   }

   void GPSWeekSecond::reset()
      throw()
   {
      GPSWeek::reset();
      sow = 0.0;
   }

   bool 
   GPSWeekSecond::operator==( const GPSWeekSecond& right ) const
      throw()
   {
      return ( GPSWeek::operator==(right) &&
               sow == right.sow );
   }

   bool 
   GPSWeekSecond::operator!=( const GPSWeekSecond& right ) const
      throw()
   {
      return (! operator==( right ) );
   }

   bool 
   GPSWeekSecond::operator<( const GPSWeekSecond& right ) const
      throw()
   {
      if( GPSWeek::operator<(right) )
      {
         return true;
      }
      if( GPSWeek::operator>(right) )
      {
         return false;
      }
      if( sow < right.sow )
      {
         return true;
      }
      return false;
   }

   bool 
   GPSWeekSecond::operator>( const GPSWeekSecond& right ) const
      throw()
   {
      return (! operator<=( right ) );
   }

   bool 
   GPSWeekSecond::operator<=( const GPSWeekSecond& right ) const
      throw()
   {
      return ( operator<( right ) || operator==( right ) );
   }

   bool 
   GPSWeekSecond::operator>=( const GPSWeekSecond& right ) const
      throw()
   {
      return (! operator<( right ) );
   }

} // namespace
