#pragma ident "$Id: //depot/sgl/gpstk/dev/src/ANSITime.cpp#2 $"

#include "ANSITime.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   const std::string ANSITime::printChars( "K" );
   const std::string ANSITime::defaultFormat( "%K" );
   
   ANSITime& ANSITime::operator=( const ANSITime& right )
      throw()
   {
      time = right.time;
      return *this;
   }
   
   CommonTime ANSITime::convertToCommonTime() const
   {
      return CommonTime( ( MJD_JDAY + UNIX_MJD + time / SEC_PER_DAY ),
                         ( time % SEC_PER_DAY ),
                         0 );
   }
   
   void ANSITime::convertFromCommonTime( const CommonTime& ct )
   {
      long jday, sod;
      double fsod;
      ct.get( jday, sod, fsod );
      
      time = 
         static_cast<time_t>((jday - MJD_JDAY - UNIX_MJD) * SEC_PER_DAY + sod);
   }
   
   std::string ANSITime::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint( rv, formatPrefixInt + "K",
                              "Kd", time );

         return rv;         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
   bool ANSITime::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;
      
      IdToValue::const_iterator i = info.find('K');
      if( i != info.end() )
      {
         time = asInt( i->second );
         return true;
      }

      return false;
   }
   
   bool ANSITime::isValid() const
      throw()
   {
      ANSITime temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }
   
   bool ANSITime::operator==( const ANSITime& right ) const
      throw()
   {
      if( time == right.time )
      {
         return true;
      }
      return false;
   }

   bool ANSITime::operator!=( const ANSITime& right ) const
      throw()
   {
      return ( !operator==( right ) );
   }

   bool ANSITime::operator<( const ANSITime& right ) const
      throw()
   {
      return ( time < right.time );
   }

   bool ANSITime::operator>( const ANSITime& right ) const
      throw()
   {
      return ( !operator<=( right ) );
   }

   bool ANSITime::operator<=( const ANSITime& right ) const
      throw()
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool ANSITime::operator>=( const ANSITime& right ) const
      throw()
   {
      return ( !operator<( right ) );
   }

} // namespace
