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

#include "GPSZcount29.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   GPSZcount29& GPSZcount29::operator=( const GPSZcount29& right )
      throw()
   {
      zcount = right.zcount;
      return *this;
   }
   
   CommonTime GPSZcount29::convertToCommonTime() const
   {
         // What we have in the zcount data member is the following:
         // a 10-bit week number, and a 19-bit zcount.  Split 'em apart.
      long wk( epoch * WEEKS_PER_EPOCH + ( (zcount >> 19) & 0x3FF ) );
      long zc( zcount & 0x7FFFF );
      long jday = GPS_EPOCH_JDAY + ( 7 * wk ) 
         + ( zc / ZCOUNT_PER_DAY );
      double sod = ( zc % ZCOUNT_PER_DAY ) * 1.5;

      return CommonTime( jday, 
                         static_cast<long>( sod ),
                         sod - static_cast<long>( sod ) );
   }
   
   void GPSZcount29::convertFromCommonTime( const CommonTime& ct )
   {
      long day, sod;
      double fsod;
      ct.get( day, sod, fsod );
      
         // find the number of days since the beginning of the GPS Epoch
      day -= GPS_EPOCH_JDAY;

         // find out how many weeks that is
      int wk( static_cast<int>( day / 7 ) );

         // ... and chop off the epochs
      epoch = wk / WEEKS_PER_EPOCH;
      wk %= WEEKS_PER_EPOCH;

         // find out what the day of week is
      day %= 7;

      zcount = static_cast<long>( day * ZCOUNT_PER_DAY ) 
         + static_cast<long>( static_cast<double>( sod + fsod ) / 1.5 );      
      
      zcount |= ( wk << 19 );
   }
   
   std::string GPSZcount29::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "E",
                              "Ehd", epoch );
         rv = formattedPrint( rv, getFormatPrefixInt() + "c",
                              "chd", zcount );
         return rv;         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
   bool GPSZcount29::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;
      
      bool hepoch( false ), hzcount( false );
      int iepoch( 0 ), izcount( 0 );
      
      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'E':
               hepoch = true;
               iepoch = gpstk::StringUtils::asInt( i->second );
               break;
            case 'c':
               hzcount = true;
               izcount = gpstk::StringUtils::asInt( i->second );
               break;
            default:
                  // do nothing
               break;
         };
      }
      
      if( hepoch && hzcount )
      {
         epoch = iepoch;
         zcount = izcount;
         return true;
      }

      return false;
   }
   
   bool GPSZcount29::isValid() const
      throw()
   {
      GPSZcount29 temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }
   
   bool GPSZcount29::operator==( const GPSZcount29& right ) const
      throw()
   {
      if( zcount == right.zcount )
      {
         return true;
      }
      return false;
   }

   bool GPSZcount29::operator!=( const GPSZcount29& right ) const
      throw()
   {
      return ( !operator==( right ) );
   }

   bool GPSZcount29::operator<( const GPSZcount29& right ) const
      throw()
   {
      if( zcount < right.zcount )
      {
         return true;
      }
      return false;
   }

   bool GPSZcount29::operator>( const GPSZcount29& right ) const
      throw()
   {
      return ( !operator<=( right ) );
   }

   bool GPSZcount29::operator<=( const GPSZcount29& right ) const
      throw()
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool GPSZcount29::operator>=( const GPSZcount29& right ) const
      throw()
   {
      return ( !operator<( right ) );
   }

} // namespace
