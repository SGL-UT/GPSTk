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

#include "YDSTime.hpp"
#include "TimeConverters.hpp"

namespace gpstk
{
   YDSTime& YDSTime::operator=( const YDSTime& right )
      throw()
   {
      year = right.year;
      doy = right.doy;
      sod = right.sod;
      return *this;
   }
   
   CommonTime YDSTime::convertToCommonTime() const
   {
      long jday = convertCalendarToJD( year, 1, 1 ) + doy - 1;
      return CommonTime( jday, sod );
   }
   
   void YDSTime::convertFromCommonTime( const CommonTime& ct )
   {
      long jday, secDay;
      double fsecDay;
      ct.get( jday, secDay, fsecDay );
      sod = static_cast<double>( secDay ) + fsecDay;

      int month, day;
      convertJDtoCalendar( jday, year, month, day );
      doy = jday - convertCalendarToJD( year, 1, 1 ) + 1;
   }
   
   std::string YDSTime::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv = fmt;
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "Y",
                              "Yhd", year);
         rv = formattedPrint(rv, getFormatPrefixInt() + "y",
                             "yhd", static_cast<short>(year % 100));
         rv = formattedPrint( rv, getFormatPrefixInt() + "j",
                              "jhd", doy);
         rv = formattedPrint( rv, getFormatPrefixFloat() + "s",
                              "sf", sod);
         return rv;
      }
      catch( gpstk::StringUtils::StringException& exc)
      {
         GPSTK_RETHROW( exc );
      }
   }

   bool YDSTime::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;

      bool hyear( false ), hdoy( false ), hsod( false );
      int iyear( 0 ), idoy( 0 );
      double isod( 0. );

      for( IdToValue::const_iterator i = info.begin();
           i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'Y':
               iyear = asInt( i->second );
               hyear = true;
               break;
            
            case 'y':
               switch( i->second.length() )
               {
                  case 2:
                     iyear = asInt( i->second ) + 1900;
                     if( iyear < 1980 )
                        iyear += 100;
                     break;
                  case 3:
                     iyear = asInt( i->second ) + 1000;
                     if( iyear < 1980 )
                        iyear += 100;
                     break;
                  default:
                     iyear = asInt( i->second );
                     break;
               };
               hyear = true;
               break;

            case 'j':
               idoy = asInt( i->second );
               hdoy = true;
               break;

            case 's':
               isod = asDouble( i->second );
               hsod = true;
               break;
            
            default:
                  // do nothing
               break;
         };
      }
      
      if( hyear )
      {
         year = iyear;
         doy = idoy;
         sod = isod;
         return true;
      }

      return false;
   }

   bool YDSTime::isValid() const
      throw()
   {
      YDSTime temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }

   bool YDSTime::operator==( const YDSTime& right ) const
      throw()
   {
      if( year == right.year && 
          doy == right.doy &&
          sod == right.sod )
      {
         return true;
      }
      return false;          
   }

   bool YDSTime::operator!=( const YDSTime& right ) const
      throw()
   {
      return (! operator==( right ) );
   }

   bool YDSTime::operator<( const YDSTime& right ) const
      throw()
   {
      if( year < right.year )
      {
         return true;
      }
      if( year > right.year )
      {
         return false;
      }
      if( doy < right.doy )
      { 
         return true;
      }
      if( doy > right.doy )
      {
         return false;
      }
      if( sod < right.sod )
      {
         return true;
      }
      return false;
   }

   bool YDSTime::operator>( const YDSTime& right ) const
      throw()
   {
      return (! operator<=( right ) );
   }

   bool YDSTime::operator<=( const YDSTime& right ) const
      throw()
   {
      return ( operator<( right ) || operator==( right ) );
   }

   bool YDSTime::operator>=( const YDSTime& right ) const
      throw()
   {
      return (! operator<( right ) );
   }
   
} // namespace
