#pragma ident "$Id: //depot/sgl/gpstk/dev/src/UnixTime.cpp#3 $"

#include "UnixTime.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   const std::string UnixTime::printChars( "Uu" );
   const std::string UnixTime::defaultFormat( "%U %u" );
   
   UnixTime& UnixTime::operator=( const UnixTime& right )
      throw()
   {
      tv.tv_sec = right.tv.tv_sec;
      tv.tv_usec = right.tv.tv_usec;
      return *this;
   }
   
   CommonTime UnixTime::convertToCommonTime() const
   {
      return CommonTime( ( MJD_JDAY + UNIX_MJD + tv.tv_sec / SEC_PER_DAY ),
                         ( tv.tv_sec % SEC_PER_DAY ),
                         ( static_cast<double>( tv.tv_usec ) * 1e-6 ) );
   }
   
   void UnixTime::convertFromCommonTime( const CommonTime& ct )
   {
      long jday, sod;
      double fsod;
      ct.get( jday, sod, fsod );
      
      tv.tv_sec = 
         static_cast<time_t>((jday - MJD_JDAY - UNIX_MJD) * SEC_PER_DAY + sod);
      
         // round to the nearest microsecond
      tv.tv_usec = static_cast<time_t>( fsod * 1e6 + 0.5 ) ;
      
      if (tv.tv_usec >= 1000000) 
      {
         tv.tv_usec -= 1000000; 
         ++tv.tv_sec; 
      }
   }
   
   std::string UnixTime::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint(rv, formatPrefixInt + "U",
                             "Ud", tv.tv_sec);
         rv = formattedPrint(rv, formatPrefixInt + "u",
                             "ud", tv.tv_usec);         

         return rv;         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
   bool UnixTime::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;
      
      bool hunixsec( false ), hunixusec( false );
      int iunixsec( 0 ), iunixusec( 0 );
      
      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'U':
               iunixsec = asInt( i->second );
               hunixsec = true;
               break;
               
            case 'u':
               iunixusec = asInt( i->second );
               hunixusec = true;
               break;
         };
      }
      
      if( hunixsec && hunixusec )
      {
         tv.tv_sec = iunixsec;
         tv.tv_usec = iunixusec;
         return true;
      }
      return false;
   }
   
   bool UnixTime::isValid() const
      throw()
   {
      UnixTime temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }
   
   bool UnixTime::operator==( const UnixTime& right ) const
      throw()
   {
      if( tv.tv_sec == right.tv.tv_sec &&
          tv.tv_usec == right.tv.tv_usec )
      {
         return true;
      }
      return false;
   }

   bool UnixTime::operator!=( const UnixTime& right ) const
      throw()
   {
      return ( !operator==( right ) );
   }

   bool UnixTime::operator<( const UnixTime& right ) const
      throw()
   {
      if( tv.tv_sec < right.tv.tv_sec )
      {
         return true;
      }
      if( tv.tv_sec == right.tv.tv_sec &&
          tv.tv_usec < right.tv.tv_usec )
      {
         return true;
      }
      return false;
   }

   bool UnixTime::operator>( const UnixTime& right ) const
      throw()
   {
      return ( !operator<=( right ) );
   }

   bool UnixTime::operator<=( const UnixTime& right ) const
      throw()
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool UnixTime::operator>=( const UnixTime& right ) const
      throw()
   {
      return ( !operator<( right ) );
   }

} // namespace
