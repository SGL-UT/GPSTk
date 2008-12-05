#pragma ident "$Id: JulianDate.cpp 1162 2008-03-27 21:18:13Z snelsen $"



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

#include "JulianDate.hpp"
#include "TimeConstants.hpp"

using namespace gpstk;

namespace Rinex3
{
   JulianDate& JulianDate::operator=( const JulianDate& right )
      throw()
   {
      jd = right.jd;
      timeSystem = right.timeSystem;
      return *this;
   }
   
   CommonTime JulianDate::convertToCommonTime() const
      throw(InvalidRequest)
   {
      try
      {
         long double temp_jd( jd + 0.5 );
         long jday( static_cast<long>( temp_jd ) );
         long double sod = 
            ( temp_jd - static_cast<long double>( jday ) ) * SEC_PER_DAY;
         
         return CommonTime( jday, 
                            static_cast<long>( sod ),
                            static_cast<double>( sod - static_cast<long>( sod ) ),
                            timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ir);
      }
   }
   
   void JulianDate::convertFromCommonTime( const CommonTime& ct )
      throw()
   {
      long jday, sod;
      double fsod;
      TimeSystem timeSys;
      ct.get( jday, sod, fsod, timeSys );
     
      jd =   static_cast<long double>( jday ) +
           (   static_cast<long double>( sod ) 
             + static_cast<long double>( fsod ) ) * DAY_PER_SEC 
           - 0.5;

      timeSystem = timeSys;
   }
   
   std::string JulianDate::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixFloat() + "J",
                              "JLf", jd );
         return rv;         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
   std::string JulianDate::printError( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixFloat() + "J",
                              "Js", getError().c_str() );
         return rv;         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   bool JulianDate::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;
      
      IdToValue::const_iterator itr = info.find('J');
      if( itr != info.end() )
      {
         jd = gpstk::StringUtils::asLongDouble( itr->second );
      }
      
      return true;
   }
   
   bool JulianDate::isValid() const
      throw()
   {
      JulianDate temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }
   
   void JulianDate::reset()
      throw()
   {
      jd = 0.0;
   }

   bool JulianDate::operator==( const JulianDate& right ) const
      throw()
   {
      if( timeSystem == right.timeSystem &&
          jd == right.jd )
      {
         return true;
      }
      return false;
   }

   bool JulianDate::operator!=( const JulianDate& right ) const
      throw()
   {
      return ( !operator==( right ) );
   }

   bool JulianDate::operator<( const JulianDate& right ) const
      throw()
   {
      if( timeSystem == right.timeSystem &&
          jd < right.jd )
      {
         return true;
      }
      return false;
   }

   bool JulianDate::operator>( const JulianDate& right ) const
      throw()
   {
      return ( !operator<=( right ) );
   }

   bool JulianDate::operator<=( const JulianDate& right ) const
      throw()
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool JulianDate::operator>=( const JulianDate& right ) const
      throw()
   {
      return ( !operator<( right ) );
   }

} // namespace
