#pragma ident "$Id: MJD.cpp 1162 2008-03-27 21:18:13Z snelsen $"



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
#include "MJD.hpp"
#include "TimeConstants.hpp"

using namespace gpstk;

namespace Rinex3
{
   MJD& MJD::operator=( const MJD& right )
      throw()
   {
      mjd = right.mjd;
      timeSystem = right.timeSystem;
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
      
         return CommonTime( jday,
                            static_cast<long>( tmp ),
                            tmp - static_cast<long>( tmp ),
                            timeSystem );
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
      TimeSystem timeSys;
      ct.get( jday, sod, fsod, timeSys );
     
      mjd =  static_cast<long double>( jday - MJD_JDAY ) +
           (  static_cast<long double>( sod ) 
            + static_cast<long double>( fsod ) ) * DAY_PER_SEC;

      timeSystem = timeSys;
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
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Pu", timeSystem );
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
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str() );
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
      timeSystem = Unknown;
   }

   bool MJD::operator==( const MJD& right ) const
      throw()
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != Any && right.timeSystem != Any) &&
           timeSystem != right.timeSystem)
         throw InvalidRequest("CommonTime objects not in same time system, cannot be compared");

      if( fabs(mjd - right.mjd) < CommonTime::eps )
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
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != Any && right.timeSystem != Any) &&
           timeSystem != right.timeSystem)
         throw InvalidRequest("CommonTime objects not in same time system, cannot be compared");

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
      return ( operator<(  right ) ||
               operator==( right )   );
   }

   bool MJD::operator>=( const MJD& right ) const
      throw()
   {
      return ( !operator<( right ) );
   }

} // namespace
