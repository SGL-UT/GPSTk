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

#include "GPSEpochWeekSecond.hpp"
#include "TimeConstants.hpp"
//#include "TimeConverters.hpp"

namespace gpstk
{
   GPSEpochWeekSecond& 
   GPSEpochWeekSecond::operator=( const GPSEpochWeekSecond& right )
      throw()
   {
      epoch = right.epoch;
      week = right.week;
      sow = right.sow;
      return *this;
   }
   
   CommonTime GPSEpochWeekSecond::convertToCommonTime() const
   {
      int fullweek( epoch * WEEKS_PER_EPOCH + week );
      int dow = static_cast<int>( sow * DAY_PER_SEC );
      int jday = GPS_EPOCH_JDAY + ( 7 * fullweek ) + dow;
      double sod(  sow - SEC_PER_DAY * dow );
      return CommonTime( jday, 
                         static_cast<long>( sod ),
                         sod - static_cast<long>( sod ) );
   }
   
   void GPSEpochWeekSecond::convertFromCommonTime( const CommonTime& ct )
   {
      long day, sod;
      double fsod;
      ct.get( day, sod, fsod );

         // find the number of days since the beginning of the GPS Epoch
      day -= GPS_EPOCH_JDAY;
         // find out how many weeks that is
      int fullweek( static_cast<int>( day / 7 ) );
         // find out what the day of week is
      day -= fullweek * 7;

      epoch = fullweek / WEEKS_PER_EPOCH;
      week = fullweek % WEEKS_PER_EPOCH;
      sow = static_cast<double>( day * SEC_PER_DAY + sod ) + fsod;
   }
   
   std::string GPSEpochWeekSecond::printf(const std::string& fmt) const
      throw( gpstk::StringUtils::StringException )
      {
         try
         {
            using gpstk::StringUtils::formattedPrint;
            std::string rv = fmt;
               
            rv = formattedPrint( rv, getFormatPrefixInt() + "E",
                                 "Ehd", epoch );
            rv = formattedPrint( rv, getFormatPrefixInt() + "G", 
                                 "Ghd", week );
            rv = formattedPrint( rv, getFormatPrefixFloat() + "g",
                                 "gf", sow);
            return rv;
         }
         catch( gpstk::StringUtils::StringException& exc )
         {
            GPSTK_RETHROW( exc );
         }
      }
      
   bool GPSEpochWeekSecond::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;
      
      bool hepoch( false ), hweek( false ), hsow( false );
      int iepoch( 0 ), iweek( 0 );
      double isow( 0. );
      
      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
            // based on the character, we know what to do...
         switch ( i->first ) 
         {
            case 'E':
               iepoch = asInt( i->second );
               hepoch = true;
               break;
            case 'F':
               iweek = asInt( i->second );
               hweek = true;
               break;
            case 'g':
               isow = asDouble( i->second );
               hsow = true;
               break;
            default:
                  // do nothing
               break;
         };
         
      } // end of for loop
      
      if( hepoch )
      {
         epoch = iepoch;
         week = iweek;
         sow = isow;
         return true;
      }

      return false;
   }

   bool GPSEpochWeekSecond::isValid() const
      throw()
   {
      GPSEpochWeekSecond temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }
   
   bool 
   GPSEpochWeekSecond::operator==( const GPSEpochWeekSecond& right ) const
      throw()
   {
      if( epoch == right.epoch &&
          week == right.week &&
          sow == right.sow )
      {
         return true;
      }
      return false;
   }

   bool 
   GPSEpochWeekSecond::operator!=( const GPSEpochWeekSecond& right ) const
      throw()
   {
      return (! operator==( right ) );
   }

   bool 
   GPSEpochWeekSecond::operator<( const GPSEpochWeekSecond& right ) const
      throw()
   {
      if( epoch < right.epoch )
      {
         return true;
      }
      if( epoch > right.epoch )
      {
         return false;
      }
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
   GPSEpochWeekSecond::operator>( const GPSEpochWeekSecond& right ) const
      throw()
   {
      return (! operator<=( right ) );
   }

   bool 
   GPSEpochWeekSecond::operator<=( const GPSEpochWeekSecond& right ) const
      throw()
   {
      return ( operator<( right ) || operator==( right ) );
   }

   bool 
   GPSEpochWeekSecond::operator>=( const GPSEpochWeekSecond& right ) const
      throw()
   {
      return (! operator<( right ) );
   }

} // namespace
