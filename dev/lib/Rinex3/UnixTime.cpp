#pragma ident "$Id: UnixTime.cpp 1162 2008-03-27 21:18:13Z snelsen $"



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

#include "UnixTime.hpp"
#include "TimeConstants.hpp"

using namespace gpstk;

namespace Rinex3
{
   UnixTime& UnixTime::operator=( const UnixTime& right )
      throw()
   {
      tv.tv_sec  = right.tv.tv_sec ;
      tv.tv_usec = right.tv.tv_usec;
      return *this;
   }
   
   CommonTime UnixTime::convertToCommonTime() const
      throw( InvalidRequest )
   {
      try
      {
         return CommonTime( ( MJD_JDAY + UNIX_MJD + tv.tv_sec / SEC_PER_DAY ),
                            ( tv.tv_sec % SEC_PER_DAY ),
                            ( static_cast<double>( tv.tv_usec ) * 1e-6 ),
                            timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ip);
      }
   }
   
   void UnixTime::convertFromCommonTime( const CommonTime& ct )
      throw( InvalidRequest )
   {
         /// This is the earliest CommonTime for which UnixTimes are valid.
     static const CommonTime MIN_CT = UnixTime(0, 0, Any);
         /// This is the latest CommonTime for which UnixTimes are valid.
         /// (2^31 - 1) s and 999999 us
       static const CommonTime MAX_CT = UnixTime(2147483647, 999999, Any);

      if ( ct < MIN_CT || ct > MAX_CT )
      {
         InvalidRequest ir("Unable to convert given CommonTime to UnixTime.");
         GPSTK_THROW(ir);
      }
                             
      long jday, sod;
      double fsod;
      TimeSystem timeSys;
      ct.get( jday, sod, fsod, timeSys );
      
      tv.tv_sec = (jday - MJD_JDAY - UNIX_MJD) * SEC_PER_DAY + sod;
      
         // round to the nearest microsecond
      tv.tv_usec = static_cast<time_t>( fsod * 1e6 + 0.5 ) ;
      
      if (tv.tv_usec >= 1000000) 
      {
         tv.tv_usec -= 1000000; 
         ++tv.tv_sec; 
      }

      timeSystem = timeSys;
   }
   
   std::string UnixTime::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint(rv, getFormatPrefixInt() + "U",
                             "Ulu", tv.tv_sec);
         rv = formattedPrint(rv, getFormatPrefixInt() + "u",
                             "ulu", tv.tv_usec);         
         return rv;         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   std::string UnixTime::printError( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint(rv, getFormatPrefixInt() + "U",
                             "Us", getError().c_str());
         rv = formattedPrint(rv, getFormatPrefixInt() + "u",
                             "us", getError().c_str());         
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
      
      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'U':
               tv.tv_sec = asInt( i->second );
               break;
               
            case 'u':
               tv.tv_usec = asInt( i->second );
               break;
               
            default:
                  // do nothing
               break;
         };
      }
      
      return true;
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

   void UnixTime::reset()
      throw()
   {
      tv.tv_sec = tv.tv_usec = 0;
      timeSystem = Unknown;
   }
   
   bool UnixTime::operator==( const UnixTime& right ) const
      throw()
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != Any && right.timeSystem != Any) &&
           timeSystem != right.timeSystem)
         throw InvalidRequest("CommonTime objects not in same time system, cannot be compared");

      if( tv.tv_sec  == right.tv.tv_sec  &&
          abs(tv.tv_usec - right.tv.tv_usec) < CommonTime::eps )
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
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != Any && right.timeSystem != Any) &&
           timeSystem != right.timeSystem)
         throw InvalidRequest("CommonTime objects not in same time system, cannot be compared");

      if( tv.tv_sec  <  right.tv.tv_sec )
      {
         return true;
      }
      if( tv.tv_sec  == right.tv.tv_sec  &&
          tv.tv_usec <  right.tv.tv_usec   )
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
