#pragma ident "$Id: //depot/sgl/gpstk/dev/src/CivilTime.cpp#6 $"

#include "CivilTime.hpp"
#include "TimeConverters.hpp"

namespace gpstk
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
      "err", "Jan", "Feb", "Mar", "Apr", "May", "Jun","Jul",
      "Aug", "Sep", "Oct", "Nov", "Dec"
   };
   
   CivilTime& CivilTime::operator=( const CivilTime& right )
      throw()
   {
      year = right.year;
      month = right.month;
      day = right.day;
      hour = right.hour;
      minute = right.minute;
      second = right.second;
      return *this;
   }
   
   CommonTime CivilTime::convertToCommonTime() const
   {
         // get the julian day
      long jday = convertCalendarToJD( year, month, day );
         // get the second of day
      double sod = convertTimeToSOD( hour, minute, second );
         // make a CommonTime with jd, whole sod, and fractional second of day
      return CommonTime( jday, static_cast<long>( sod ),
                         ( sod - static_cast<long>( sod ) ) );
   }
   
   void CivilTime::convertFromCommonTime( const CommonTime& ct )
   {
      long jday, sod;
      double fsod;
         // get the julian day, second of day, and fractional second of day
      ct.get( jday, sod, fsod );
         // convert the julian day to calendar "year/month/day of month"
      convertJDtoCalendar( jday, year, month, day );
         // convert the (whole) second of day to "hour/minute/second"
      convertSODtoTime( static_cast<double>( sod ), hour, minute, second );
         // add the fractional second of day to "second"
      second += fsod;
   }
   
   std::string CivilTime::printf(const std::string& fmt) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv = fmt;
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "Y",
                              "Yhd", year );
         rv = formattedPrint( rv, getFormatPrefixInt() + "y",
                              "yhd", static_cast<short>( year % 100 ) );
         rv = formattedPrint( rv, getFormatPrefixInt() + "m",
                              "mhd", month );
         rv = formattedPrint( rv, getFormatPrefixInt() + "b",
                              "bs", MonthAbbrevNames[month] );
         rv = formattedPrint( rv, getFormatPrefixInt() + "B",
                              "Bs", MonthNames[month] );
         rv = formattedPrint( rv, getFormatPrefixInt() + "d",
                              "dhd", day );
         rv = formattedPrint( rv, getFormatPrefixInt() + "H",
                              "Hhd", hour );
         rv = formattedPrint( rv, getFormatPrefixInt() + "M",
                              "Mhd", minute );
         rv = formattedPrint( rv, getFormatPrefixInt() + "S", 
                              "Sd", static_cast<short>( second ) );
         rv = formattedPrint( rv, getFormatPrefixFloat() + "f",
                              "ff", second );
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
      
      bool hyear( false ), hmonth( false ), hday( false ), 
         hhour( false ), hmin( false ), hsec( false );
      int iyear( 0 ), imonth( 1 ), iday( 1 ), ihour( 0 ), imin( 0 );
      double isec( 0. );

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
            
            case 'm':
               imonth = asInt( i->second );
               hmonth = true;
               break;
               
            case 'b':
            case 'B':
            {
               std::string thisMonth( i->second );
               lowerCase(thisMonth);
               
               if (isLike(thisMonth, "jan.*")) imonth = 1;               
               else if (isLike(thisMonth, "feb.*")) imonth = 2;
               else if (isLike(thisMonth, "mar.*")) imonth = 3;
               else if (isLike(thisMonth, "apr.*")) imonth = 4;
               else if (isLike(thisMonth, "may.*")) imonth = 5;
               else if (isLike(thisMonth, "jun.*")) imonth = 6;
               else if (isLike(thisMonth, "jul.*")) imonth = 7;
               else if (isLike(thisMonth, "aug.*")) imonth = 8;
               else if (isLike(thisMonth, "sep.*")) imonth = 9;
               else if (isLike(thisMonth, "oct.*")) imonth = 10;
               else if (isLike(thisMonth, "nov.*")) imonth = 11;
               else if (isLike(thisMonth, "dec.*")) imonth = 12;
               else
               {
                  return false;
                     //InvalidRequest ir("Invalid month entry for readTime");
                     //GPSTK_THROW( ir );
               }
               hmonth = true;
            }
               break;

            case 'd':
               iday = asInt( i->second );
               hday = true;
               break;
               
            case 'H':
               ihour = asInt( i->second );
               hhour = true;
               break;
               
            case 'M':
               imin = asInt( i->second );
               hmin = true;
               break;
               
            case 'S':
               isec = asDouble( i->second );
               hsec = true;
               break;
               
            default:
                  // do nothing
               break;
         };
      }

      if( hyear )
      {
         year = iyear;
         month = imonth;
         day = iday;
         hour = ihour;
         minute = imin;
         second = isec;
         return true;
      }
      
      return false;
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
   
   bool CivilTime::operator==( const CivilTime& right ) const
      throw()
   {
      if( year == right.year &&
          month == right.month && 
          day == right.day &&
          hour == right.hour &&
          minute == right.minute &&
          second == right.second )
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
