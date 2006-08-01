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

#include "GPSWeekZcount.hpp"
#include "TimeConstants.hpp"
#include "TimeConverters.hpp"

namespace gpstk
{
   GPSWeekZcount& GPSWeekZcount::operator=( const GPSWeekZcount& right )
      throw()
   {
      week = right.week;
      zcount = right.zcount;
      return *this;
   }
   
   CommonTime GPSWeekZcount::convertToCommonTime() const
   {
      int dow = zcount / ZCOUNT_PER_DAY;
      int jday = GPS_EPOCH_JDAY + ( 7 * week ) + dow;
      double sod = static_cast<double>( zcount % ZCOUNT_PER_DAY ) * 1.5;
      return CommonTime( jday,
                         static_cast<long>( sod ),
                         sod - static_cast<long>( sod ) );
   }
   
   void GPSWeekZcount::convertFromCommonTime( const CommonTime& ct ) 
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

      zcount = static_cast<long>( day * ZCOUNT_PER_DAY ) 
         + static_cast<long>( static_cast<double>( sod + fsod ) / 1.5 );      
   }

   std::string GPSWeekZcount::printf( const std::string& fmt ) const
         throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint( rv, getFormatPrefixInt() + "F", "Fhd", week );
         rv = formattedPrint( rv, getFormatPrefixInt() + "z", "zd", zcount );
         rv = formattedPrint( rv, getFormatPrefixInt() + "Z", "Zd", zcount );
         
         return rv;         
      }
      catch( gpstk::StringUtils::StringException& exc )
      {
         GPSTK_RETHROW( exc );
      }
   }
   
   bool GPSWeekZcount::setFromInfo( const IdToValue& info )
         throw()
   {
      using namespace gpstk::StringUtils;

      bool hfullweek( false ), hzcount( false );
      int ifullweek( 0 ), izcount( 0 );
      
      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
            // based on the character, we know what to do...
         switch( i->first )
         {
            case 'F':
               ifullweek = asInt( i->second );
               hfullweek = true;
               break;
            
            case 'z':
            case 'Z':
               izcount = asInt( i->second );
               hzcount = true;
               break;
               
            default:
                  // do nothing
               break;
         };
      }
      
      if( hfullweek )
      {
         week = ifullweek;
         zcount = izcount;
         return true;
      }
      
      return false;
   }

   bool GPSWeekZcount::isValid() const
         throw()
   {
      GPSWeekZcount temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }

   bool GPSWeekZcount::operator==( const GPSWeekZcount& right ) const
      throw()
   {
      if( week == right.week &&
          zcount == right.zcount )
      {
         return true;
      }
      return false;         
   }

   bool GPSWeekZcount::operator!=( const GPSWeekZcount& right ) const
      throw()
   {
      return ( !operator==( right ) );
   }

   bool GPSWeekZcount::operator<( const GPSWeekZcount& right ) const
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
      if( week < right.zcount )
      {
         return true;
      }
      return false;
   }

   bool GPSWeekZcount::operator>( const GPSWeekZcount& right ) const
      throw()
   {
      return ( !operator<=( right ) );
   }

   bool GPSWeekZcount::operator<=( const GPSWeekZcount& right ) const
      throw()
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool GPSWeekZcount::operator>=( const GPSWeekZcount& right ) const
      throw()
   {
      return ( !operator<( right ) );
   }
   
} // namespace
