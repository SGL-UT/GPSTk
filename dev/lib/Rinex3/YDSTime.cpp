#pragma ident "$Id: YDSTime.cpp 1162 2008-03-27 21:18:13Z snelsen $"



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

#include <cmath>
#include "YDSTime.hpp"
#include "TimeConverters.hpp"

using namespace gpstk;

namespace Rinex3
{
   YDSTime& YDSTime::operator=( const YDSTime& right )
      throw()
   {
      year = right.year;
      doy  = right.doy;
      sod  = right.sod;
      timeSystem = right.timeSystem;
      return *this;
   }
   
   CommonTime YDSTime::convertToCommonTime() const
      throw(InvalidRequest)
   {
      try
      {
         long jday = convertCalendarToJD( year, 1, 1 ) + doy - 1;
         return CommonTime( jday, sod, timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ir);
      }
   }
   
   void YDSTime::convertFromCommonTime( const CommonTime& ct )
      throw()
   {
      long jday, secDay;
      double fsecDay;
      TimeSystem timeSys;
      ct.get( jday, secDay, fsecDay, timeSys );
      sod = static_cast<double>( secDay ) + fsecDay;

      int month, day;
      convertJDtoCalendar( jday, year, month, day );
      doy = jday - convertCalendarToJD( year, 1, 1 ) + 1;

      timeSystem = timeSys;
   }
   
   std::string YDSTime::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv = fmt;
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "Y",
                              "Yd", year );
         rv = formattedPrint( rv, getFormatPrefixInt() + "y",
                              "yd", static_cast<short>(year % 100) );
         rv = formattedPrint( rv, getFormatPrefixInt() + "j",
                              "ju", doy );
         rv = formattedPrint( rv, getFormatPrefixFloat() + "s",
                              "sf", sod );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Pu", timeSystem );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& exc)
      {
         GPSTK_RETHROW( exc );
      }
   }

   std::string YDSTime::printError( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv = fmt;
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "Y",
                              "Ys", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "y",
                              "ys", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "j",
                              "js", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixFloat() + "s",
                              "ss", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str() );
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

      for( IdToValue::const_iterator i = info.begin();
           i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'Y':
               year = asInt( i->second );
               break;
            
            case 'y':
               switch( i->second.length() )
               {
                  case 2:
                     year = asInt( i->second ) + 1900;
                     if( year < 1980 )
                        year += 100;
                     break;
                  case 3:
                     year = asInt( i->second ) + 1000;
                     if( year < 1980 )
                        year += 100;
                     break;
                  default:
                     year = asInt( i->second );
                     break;
               };
               break;

            case 'j':
               doy = asInt( i->second );
               break;

            case 's':
               sod = asDouble( i->second );
               break;

            case 'P':
               timeSystem = (TimeSystem) asInt( i->second );
               break;
            
            default:
                  // do nothing
               break;
         };
      }
      
      return true;
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

   void YDSTime::reset()
      throw()
   {
      year = doy = 0;
      sod = 0.0;
      timeSystem = Unknown;
   }

   bool YDSTime::operator==( const YDSTime& right ) const
      throw()
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != Any && right.timeSystem != Any) &&
           timeSystem != right.timeSystem)
         return false;

      if( year == right.year &&
          doy  == right.doy  &&
          fabs(sod - right.sod) < CommonTime::eps )
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
      throw(InvalidRequest)
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != Any && right.timeSystem != Any) &&
           timeSystem != right.timeSystem)
         throw InvalidRequest("CommonTime objects not in same time system, cannot be compared");

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
