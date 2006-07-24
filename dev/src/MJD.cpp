#pragma ident "$Id: //depot/sgl/gpstk/dev/src/MJD.cpp#3 $"

#include "MJD.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   MJD& MJD::operator=( const MJD& right )
      throw()
   {
      mjd = right.mjd;
      return *this;
   }
   
   CommonTime MJD::convertToCommonTime() const
   {
         // convert to Julian Day
      long double tmp( mjd + MJD_JDAY );
         // get the whole number of days
      long jday( static_cast<long>( tmp ) );
         // tmp now holds the partial days
      tmp -= static_cast<long>( tmp );
         // convert tmp to seconds of day
      tmp *= SEC_PER_DAY;
      
      return CommonTime( jday,
                         static_cast<long>( tmp ),
                         tmp - static_cast<long>( tmp ) );
   }
   
   void MJD::convertFromCommonTime( const CommonTime& ct )
   {
      long jday, sod;
      double fsod;
      ct.get( jday, sod, fsod );
     
      mjd = static_cast<long double>( jday - MJD_JDAY ) +
         ( static_cast<long double>( sod ) 
           + static_cast<long double>( fsod ) ) * DAY_PER_SEC;
   }
   
   std::string MJD::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixFloat() + "Q",
                              "QLf", mjd );
         return rv;         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
   bool MJD::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;
      
      IdToValue::const_iterator itr = info.find('Q');
      if( itr != info.end() )
      {
         mjd = gpstk::StringUtils::asLongDouble( itr->second );
         return true;
      }
      
      return false;
   }
   
   bool MJD::isValid() const
      throw()
   {
      MJD temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }
   
   bool MJD::operator==( const MJD& right ) const
      throw()
   {
      if( mjd == right.mjd )
      {
         return true;
      }
      return false;
   }

   bool MJD::operator!=( const MJD& right ) const
      throw()
   {
      return ( !operator==( right ) );
   }

   bool MJD::operator<( const MJD& right ) const
      throw()
   {
      if( mjd < right.mjd )
      {
         return true;
      }
      return false;
   }

   bool MJD::operator>( const MJD& right ) const
      throw()
   {
      return ( !operator<=( right ) );
   }

   bool MJD::operator<=( const MJD& right ) const
      throw()
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool MJD::operator>=( const MJD& right ) const
      throw()
   {
      return ( !operator<( right ) );
   }

} // namespace
