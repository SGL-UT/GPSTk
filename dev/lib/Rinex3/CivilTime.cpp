#pragma ident "$Id: CivilTime.cpp 1162 2008-03-27 21:18:13Z snelsen $"

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
#include "CivilTime.hpp"
#include "TimeConverters.hpp"

using namespace gpstk;

namespace Rinex3
{
      /// Long month names for converstion from numbers to strings
   const char * CivilTime::MonthNames[] = 
   {
      "Error",
      "January","February", "March", "April",
      "May", "June","July", "August",
      "September", "October", "November", "December"
   };
      
      /// Short month names for converstion from numbers to strings
   const char * CivilTime::MonthAbbrevNames[] = 
   {
      "err", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
   };
   
   CivilTime& CivilTime::operator=( const CivilTime& right )
      throw()
   {
      year       = right.year;
      month      = right.month;
      day        = right.day;
      hour       = right.hour;
      minute     = right.minute;
      second     = right.second;
      timeSystem = right.timeSystem;
      return *this;
   }
   
   CommonTime CivilTime::convertToCommonTime() const
      throw( InvalidRequest )
   {
      try
      {
            // get the julian day
         long jday = convertCalendarToJD( year, month, day );
            // get the second of day
         double sod = convertTimeToSOD( hour, minute, second );
            // make a CommonTime with jd, whole sod, and 
            // fractional second of day
         return CommonTime(  jday, static_cast<long>(sod) ,
                            (sod - static_cast<long>(sod)),
                            timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ir);
      }
   }
   
   void CivilTime::convertFromCommonTime( const CommonTime& ct )
      throw()
   {
      long jday, sod;
      double fsod;
      TimeSystem timeSys;
         // get the julian day, second of day, and fractional second of day
      ct.get( jday, sod, fsod, timeSys );
         // convert the julian day to calendar "year/month/day of month"
      convertJDtoCalendar( jday, year, month, day );
         // convert the (whole) second of day to "hour/minute/second"
      convertSODtoTime( static_cast<double>( sod ), hour, minute, second );
         // add the fractional second of day to "second"
      second += fsod;
      timeSystem = timeSys;
   }
   
   std::string CivilTime::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv = fmt;
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "Y",
                              "Yd", year );
         rv = formattedPrint( rv, getFormatPrefixInt() + "y",
                              "yd", static_cast<short>( year % 100 ) );
         rv = formattedPrint( rv, getFormatPrefixInt() + "m",
                              "mu", month );
         rv = formattedPrint( rv, getFormatPrefixInt() + "b",
                              "bs", MonthAbbrevNames[month] );
         rv = formattedPrint( rv, getFormatPrefixInt() + "B",
                              "Bs", MonthNames[month] );
         rv = formattedPrint( rv, getFormatPrefixInt() + "d",
                              "du", day );
         rv = formattedPrint( rv, getFormatPrefixInt() + "H",
                              "Hu", hour );
         rv = formattedPrint( rv, getFormatPrefixInt() + "M",
                              "Mu", minute );
         rv = formattedPrint( rv, getFormatPrefixInt() + "S", 
                              "Su", static_cast<short>( second ) );
         rv = formattedPrint( rv, getFormatPrefixFloat() + "f",
                              "ff", second );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Pu", timeSystem );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& exc )
      {
         GPSTK_RETHROW( exc );
      }
   }

   std::string CivilTime::printError( const std::string& fmt) const
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
         rv = formattedPrint( rv, getFormatPrefixInt() + "m",
                              "ms", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "b",
                              "bs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "B",
                              "Bs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "d",
                              "ds", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "H",
                              "Hs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "M",
                              "Ms", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "S", 
                              "Ss", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixFloat() + "f",
                              "fs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& exc )
      {
         GPSTK_RETHROW( exc );
      }
   }

   bool CivilTime::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;

      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
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
            
            case 'm':
               month = asInt( i->second );
               break;
               
            case 'b':
            case 'B':
            {
               std::string thisMonth( i->second );
               lowerCase(thisMonth);
               
               if (isLike(thisMonth, "jan.*")) month = 1;               
               else if (isLike(thisMonth, "feb.*")) month = 2;
               else if (isLike(thisMonth, "mar.*")) month = 3;
               else if (isLike(thisMonth, "apr.*")) month = 4;
               else if (isLike(thisMonth, "may.*")) month = 5;
               else if (isLike(thisMonth, "jun.*")) month = 6;
               else if (isLike(thisMonth, "jul.*")) month = 7;
               else if (isLike(thisMonth, "aug.*")) month = 8;
               else if (isLike(thisMonth, "sep.*")) month = 9;
               else if (isLike(thisMonth, "oct.*")) month = 10;
               else if (isLike(thisMonth, "nov.*")) month = 11;
               else if (isLike(thisMonth, "dec.*")) month = 12;
               else
               {
                  return false;
               }
            }
               break;

            case 'd':
               day = asInt( i->second );
               break;
               
            case 'H':
               hour = asInt( i->second );
               break;
               
            case 'M':
               minute = asInt( i->second );
               break;
               
            case 'S':
            case 'f':
               second = asDouble( i->second );
               if (i->first == 'S')
                  second = floor(second);
               break;
               
            default:
                  // do nothing
               break;
         };
      }

      return true;
   }

   bool CivilTime::isValid() const
      throw()
   {
      CivilTime temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }
   
   void CivilTime::reset()
      throw()
   {
      year = 0;
      month = day = 1;
      hour = minute = 0;
      second = 0.0;
      timeSystem = Unknown;
   }

   bool CivilTime::operator==( const CivilTime& right ) const
      throw()
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != Any && right.timeSystem != Any) &&
           timeSystem != right.timeSystem)
         return false;

      if( year   == right.year    &&
          month  == right.month   &&
          day    == right.day     &&
          hour   == right.hour    &&
          minute == right.minute  &&
          fabs(second - right.second) < CommonTime::eps )
      {
         return true;
      }
      return false;
   }

   bool CivilTime::operator!=( const CivilTime& right ) const
      throw()
   {
      return (! operator==( right ) );
   }

   bool CivilTime::operator<( const CivilTime& right ) const
      throw()
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != Any && right.timeSystem != Any) &&
           timeSystem != right.timeSystem)
         return false;

      if( year < right.year )
      {
         return true;
      }
      if( year > right.year )
      {
         return false;
      }
      if( month < right.month )
      {
         return true;
      }
      if( month > right.month )
      {
         return false;
      }
      if( day < right.day )
      {
         return true;
      }
      if( day > right.day )
      {
         return false;
      }
      if( hour < right.hour )
      {
         return true;
      }
      if( hour > right.hour )
      {
         return false;
      }
      if( minute < right.minute )
      {
         return true;
      }
      if( minute > right.minute )
      {
         return false;
      }
      if( second < right.second )
      {
         return true;
      }

      return false;
   }

   bool CivilTime::operator>( const CivilTime& right ) const
      throw()
   {
      return (! operator<=( right ) );
   }

   bool CivilTime::operator<=( const CivilTime& right ) const
      throw()
   {
      return ( operator<( right ) || operator==( right ) );
   } 

   bool CivilTime::operator>=( const CivilTime& right ) const
      throw()
   {
      return (! operator<( right ) );
   }
   
} // namespace
