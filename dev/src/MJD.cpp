#pragma ident "$Id$"



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
      throw(InvalidRequest)
   {
      try
      {
            // convert to Julian Day
         long double tmp( mjd + MJD_JDAY );
            // get the whole number of days
         long jday( static_cast<long>( tmp ) );
            // tmp now holds the partial days
         tmp -= static_cast<long>( tmp );
            // convert tmp to seconds of day
         tmp *= SEC_PER_DAY;
            // Lose excess precision in 'tmp' because it may cause rounding
            // problems in the conversion to CommonTime.
         double dTmp( static_cast<double>( tmp ) );

         return CommonTime( jday,
                            static_cast<long>( dTmp ),
                            dTmp - static_cast<long>( dTmp ) );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ip);
      }
      
   }
   
   void MJD::convertFromCommonTime( const CommonTime& ct )
      throw()
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
   
   std::string MJD::printError( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixFloat() + "Q",
                              "Qs", getError().c_str() );
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
      }
      
      return true;
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
   
   void MJD::reset()
      throw()
   {
      mjd = 0.0;
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
