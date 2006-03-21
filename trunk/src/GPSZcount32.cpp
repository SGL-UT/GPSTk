#pragma ident "$Id: //depot/sgl/gpstk/dev/src/GPSZcount32.cpp#2 $"

#include "GPSZcount32.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   const std::string GPSZcount32::printChars( "C" );
   const std::string GPSZcount32::defaultFormat( "%C" );
   
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
         
         rv = formattedPrint( rv, formatPrefixInt + "C",
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
         return true;
      }
      
      return false;
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
