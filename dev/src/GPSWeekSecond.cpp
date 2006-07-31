#pragma ident "$Id: //depot/sgl/gpstk/dev/src/GPSWeekSecond.cpp#3 $"

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
//#include "TimeConverters.hpp"

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

         // This class can parse GPS full week and GPS seconds of week.
      bool hfullWeek( false ), hsow( false );
      int ifullWeek( 0 );
      double isow( 0. );
      
      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
            // based on the character, we know what to do...
         switch ( i->first ) 
         {
            case 'F':
            {
               ifullWeek = asInt( i->second );
               hfullWeek = true;
            }
            break;
            
            case 'g':
            {
               isow = asDouble( i->second );
               hsow = true;
            }
            break;
            
            default:
            {
                  // do nothing
            }
            break;
         };
         
      } // end of for loop
      
      if( hfullWeek )
      {
         week = ifullWeek;
         sow = isow;
         return true;
      }

      return false;
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
