//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2017, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/// @file PosixTime.cpp

#include "PosixTime.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   PosixTime& PosixTime::operator=( const PosixTime& right )
   {
      ts.tv_sec  = right.ts.tv_sec ;
      ts.tv_nsec = right.ts.tv_nsec;
      timeSystem = right.timeSystem;
      return *this;
   }

   CommonTime PosixTime::convertToCommonTime() const
   {
      try
      {
         CommonTime ct;
         return ct.set( ( MJD_JDAY + UNIX_MJD + ts.tv_sec / SEC_PER_DAY ),
                        ( ts.tv_sec % SEC_PER_DAY ),
                        ( static_cast<double>( ts.tv_nsec ) * 1e-9 ),
                        timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ip);
      }
   }

   void PosixTime::convertFromCommonTime( const CommonTime& ct )
   {
         /// This is the earliest CommonTime for which PosixTimes are valid.
      static const CommonTime MIN_CT = PosixTime(0, 0, TimeSystem::Any);
         /// This is the latest CommonTime for which PosixTimes are valid.
         /// (2^31 - 1) s and 999999 us
      static const CommonTime MAX_CT = PosixTime(2147483647, 999999,
                                                 TimeSystem::Any);

      if ( ct < MIN_CT || ct > MAX_CT )
      {
         InvalidRequest ir("Unable to convert given CommonTime to PosixTime.");
         GPSTK_THROW(ir);
      }

      long jday, sod;
      double fsod;
      ct.get( jday, sod, fsod, timeSystem );

      ts.tv_sec = (jday - MJD_JDAY - UNIX_MJD) * SEC_PER_DAY + sod;

         // round to the nearest nanosecond
      ts.tv_nsec = static_cast<time_t>( fsod * 1e9 + 0.5 ) ;

      if (ts.tv_nsec >= 1000000000)
      {
         ts.tv_nsec -= 1000000000;
         ++ts.tv_sec;
      }
   }

   std::string PosixTime::printf( const std::string& fmt ) const
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint(rv, getFormatPrefixInt() + "W",
                             "Wlu", ts.tv_sec);
         rv = formattedPrint(rv, getFormatPrefixInt() + "N",
                             "Nlu", ts.tv_nsec);
         rv = formattedPrint(rv, getFormatPrefixInt() + "P",
                             "Ps", timeSystem.asString().c_str() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   std::string PosixTime::printError( const std::string& fmt ) const
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint(rv, getFormatPrefixInt() + "W",
                             "Ws", getError().c_str());
         rv = formattedPrint(rv, getFormatPrefixInt() + "N",
                             "Ns", getError().c_str());
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str());
         return rv;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   bool PosixTime::setFromInfo( const IdToValue& info )
   {
      using namespace gpstk::StringUtils;

      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'W':
               ts.tv_sec = asInt( i->second );
               break;

            case 'N':
               ts.tv_nsec = asInt( i->second );
               break;

            case 'P':
               timeSystem.fromString(i->second);
               break;

            default:
                  // do nothing
               break;
         };
      }

      return true;
   }

   bool PosixTime::isValid() const
   {
      PosixTime temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }

   void PosixTime::reset()
   {
      ts.tv_sec = ts.tv_nsec = 0;
      timeSystem = TimeSystem::Unknown;
   }

   bool PosixTime::operator==( const PosixTime& right ) const
   {
         /// Any (wildcard) type exception allowed, otherwise must be
         /// same time systems
      if ((timeSystem != TimeSystem::Any &&
           right.timeSystem != TimeSystem::Any) &&
          timeSystem != right.timeSystem)
         return false;

      if( ts.tv_sec == right.ts.tv_sec  &&
          abs(ts.tv_nsec - right.ts.tv_nsec) < CommonTime::eps )
      {
         return true;
      }
      return false;
   }

   bool PosixTime::operator!=( const PosixTime& right ) const
   {
      return ( !operator==( right ) );
   }

   bool PosixTime::operator<( const PosixTime& right ) const
   {
         /// Any (wildcard) type exception allowed, otherwise must be
         /// same time systems
      if ((timeSystem != TimeSystem::Any &&
           right.timeSystem != TimeSystem::Any) &&
          timeSystem != right.timeSystem)
      {
         gpstk::InvalidRequest ir("CommonTime objects not in same time system,"
                                  " cannot be compared");
         GPSTK_THROW(ir);
      }

      if( ts.tv_sec  <  right.ts.tv_sec )
      {
         return true;
      }
      if( ts.tv_sec  == right.ts.tv_sec  &&
          ts.tv_nsec <  right.ts.tv_nsec   )
      {
         return true;
      }
      return false;
   }

   bool PosixTime::operator>( const PosixTime& right ) const
   {
      return ( !operator<=( right ) );
   }

   bool PosixTime::operator<=( const PosixTime& right ) const
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool PosixTime::operator>=( const PosixTime& right ) const
   {
      return ( !operator<( right ) );
   }

} // namespace
