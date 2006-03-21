#pragma ident "$Id: //depot/sgl/gpstk/dev/src/GPSEpochWeekSecond.cpp#5 $"

#include "GPSEpochWeekSecond.hpp"
#include "TimeConstants.hpp"
//#include "TimeConverters.hpp"

namespace gpstk
{
   const std::string GPSEpochWeekSecond::printChars( "EGg" );
   const std::string GPSEpochWeekSecond::defaultFormat( "%E %04G %g" );
   
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
               
            rv = formattedPrint( rv, formatPrefixInt + "E",
                                 "Ehd", epoch );
            rv = formattedPrint( rv, formatPrefixInt + "G", 
                                 "Ghd", week );
            rv = formattedPrint( rv, formatPrefixFloat + "g",
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
