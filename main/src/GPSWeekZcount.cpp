#pragma ident "$Id: //depot/sgl/gpstk/dev/src/GPSWeekZcount.cpp#3 $"

#include "GPSWeekZcount.hpp"
#include "TimeConstants.hpp"
#include "TimeConverters.hpp"

namespace gpstk
{
   const std::string GPSWeekZcount::printChars( "FzZ" );
   const std::string GPSWeekZcount::defaultFormat( "%04F %06Z" );

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

         rv = formattedPrint( rv, formatPrefixInt + "F", "Fhd", week );
         rv = formattedPrint( rv, formatPrefixInt + "z", "zd", zcount );
         rv = formattedPrint( rv, formatPrefixInt + "Z", "Zd", zcount );
         
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
