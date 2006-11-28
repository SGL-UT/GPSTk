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

#include "GPSZcount32.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   GPSZcount32& GPSZcount32::operator=( const GPSZcount32& right )
      throw()
   {
      zcount = right.zcount;
      return *this;
   }
   
   CommonTime GPSZcount32::convertToCommonTime() const
   {
         // What we have in the zcount data member is the following:
         // a 13-bit week number, and a 19-bit zcount.  Split 'em apart.
      int wk( (zcount >> 19) & 0x1FFF ), zc( zcount & 0x7FFFF );
      int jday = GPS_EPOCH_JDAY + ( 7 * wk ) 
         + static_cast<int>( zc * DAY_PER_ZCOUNT );
      double sod = ( zc % ZCOUNT_PER_DAY ) * 1.5;

      return CommonTime( jday, 
                         static_cast<long>( sod ),
                         sod - static_cast<long>( sod ) );
   }
   
   void GPSZcount32::convertFromCommonTime( const CommonTime& ct )
   {
      long day, sod;
      double fsod;
      ct.get( day, sod, fsod );
      
               // find the number of days since the beginning of the GPS Epoch
      day -= GPS_EPOCH_JDAY;
         // find out how many weeks that is
      int wk( static_cast<int>( day / 7 ) );
         // find out what the day of week is
      day %= 7;

      zcount = static_cast<long>( day * ZCOUNT_PER_DAY ) 
         + static_cast<long>( static_cast<double>( sod + fsod ) / 1.5 );      
      
      zcount |= ( wk << 19 );
   }
   
   std::string GPSZcount32::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "C",
                              "Cd", zcount );

         return rv;         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
   bool GPSZcount32::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;
      
      IdToValue::const_iterator itr = info.find('C');
      if( itr != info.end() )
      {
         zcount = gpstk::StringUtils::asInt( itr->second );
      }
      
      return true;
   }
   
   bool GPSZcount32::isValid() const
      throw()
   {
      GPSZcount32 temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }

   void GPSZcount32::reset()
      throw()
   {
      zcount = 0;
   }
   
   bool GPSZcount32::operator==( const GPSZcount32& right ) const
      throw()
   {
      if( zcount == right.zcount )
      {
         return true;
      }
      return false;
   }

   bool GPSZcount32::operator!=( const GPSZcount32& right ) const
      throw()
   {
      return ( !operator==( right ) );
   }

   bool GPSZcount32::operator<( const GPSZcount32& right ) const
      throw()
   {
      if( zcount < right.zcount )
      {
         return true;
      }
      return false;
   }

   bool GPSZcount32::operator>( const GPSZcount32& right ) const
      throw()
   {
      return ( !operator<=( right ) );
   }

   bool GPSZcount32::operator<=( const GPSZcount32& right ) const
      throw()
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool GPSZcount32::operator>=( const GPSZcount32& right ) const
      throw()
   {
      return ( !operator<( right ) );
   }

} // namespace
