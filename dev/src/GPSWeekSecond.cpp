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
      week = right.week;
      sow = right.sow;
      return *this;
   }
   
   CommonTime GPSWeekSecond::convertToCommonTime() const
   {
      int dow = static_cast<int>( sow * DAY_PER_SEC );
      int jday = GPS_EPOCH_JDAY + ( 7 * week ) + dow;
      double sod(  sow - SEC_PER_DAY * dow );
      return CommonTime( jday, 
                         static_cast<long>( sod ),
                         sod - static_cast<long>( sod ) );
   }
   
   void GPSWeekSecond::convertFromCommonTime( const CommonTime& ct )
   {
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
   
   std::string GPSWeekSecond::printf(const std::string& fmt) const
      throw( gpstk::StringUtils::StringException )
      {
         try
         {
            using gpstk::StringUtils::formattedPrint;
            std::string rv = fmt;
               
            rv = formattedPrint( rv, getFormatPrefixInt() + "F", 
                                 "Fhd", week );
            rv = formattedPrint( rv, getFormatPrefixInt() + "w",
                                 "whd", static_cast<int>(sow / SEC_PER_DAY) );
            rv = formattedPrint( rv, getFormatPrefixFloat() + "g",
                                 "gf", sow);
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

      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
            // based on the character, we know what to do...
         switch ( i->first ) 
         {
            case 'F':
               week = asInt( i->second );
               break;
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
      GPSWeekSecond temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }

   void GPSWeekSecond::reset()
      throw()
   {
      week = 0;
      sow = 0.0;
   }

   bool 
   GPSWeekSecond::operator==( const GPSWeekSecond& right ) const
      throw()
   {
      if( week == right.week &&
          sow == right.sow )
      {
         return true;
      }
      return false;
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
      if( week < right.week )
      {
         return true;
      }
      if( week > right.week )
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
