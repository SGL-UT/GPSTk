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
//  Copyright 2004, The University of Texas at Austin
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

/// @file JulianDate.cpp

#include <cmath>
#include <cstdlib>            // for strtol
#include "JulianDate.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   const unsigned int JulianDate::JDLEN(17);                   // < # dec digits ulong
   const double JulianDate::JDFACT(1.0e-17);                   // 1.0e-JDLEN
   //const uint64_t JulianDate::JDHALFDAY(500000000L);   // JDLEN=9 digits
   //const uint64_t JulianDate::JDHALFDAY(500000000000L);   // JDLEN=12 digits
   const uint64_t JulianDate::JDHALFDAY(50000000000000000L);   // JDLEN=17 digits

   JulianDate& JulianDate::operator=( const JulianDate& right )
   {
      jday = right.jday;
      dday = right.dday;
      fday = right.fday;
      timeSystem = right.timeSystem;
      return *this;
   }

   CommonTime JulianDate::convertToCommonTime() const
   {
      try
      {
         // fraction of day
         double frod = static_cast<double>(dday)*JDFACT;
         frod += static_cast<double>(fday)*JDFACT*JDFACT;
         long sod = static_cast<long>(frod*SEC_PER_DAY);       // truncate
         frod -= static_cast<double>(sod)/SEC_PER_DAY;
         // fractional seconds of day
         double frsod = frod*SEC_PER_DAY;
         
         CommonTime ct;
         return ct.set( jday, sod, frsod, timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ir);
      }
   }

   void JulianDate::convertFromCommonTime( const CommonTime& ct )
   {
      long isod;
      double fsod;
      ct.get( jday, isod, fsod, timeSystem );

      double frac((static_cast<double>(isod)+fsod)/SEC_PER_DAY);
      dday = static_cast<uint64_t>(frac/JDFACT);
      fday = static_cast<uint64_t>((frac/JDFACT-static_cast<double>(dday))/JDFACT);
   }

   std::string JulianDate::printf( const std::string& fmt ) const
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint( rv, getFormatPrefixFloat() + "J",
                              "JLf", JD() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", timeSystem.asString().c_str() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   std::string JulianDate::printError( const std::string& fmt ) const
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint( rv, getFormatPrefixFloat() + "J",
                              "Js", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   bool JulianDate::setFromInfo( const IdToValue& info )
   {
      using namespace gpstk::StringUtils;

      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'J':
               this->fromString(i->second);
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

   bool JulianDate::isValid() const
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
   {
      jday = 0L;
      dday = fday = static_cast<uint64_t>(0);
      timeSystem = TimeSystem::Unknown;
   }

   bool JulianDate::operator==( const JulianDate& right ) const
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if((timeSystem != TimeSystem::Any && right.timeSystem != TimeSystem::Any)
            && timeSystem != right.timeSystem)
      {
         return false;
      //   gpstk::InvalidRequest ir("CommonTime objects not in same time system, "
      //                                 "cannot be compared");
      //   GPSTK_THROW(ir);
      }
      //std::cout << std::scientific << std::setprecision(4)
      //<< " dday " << (dday > right.dday ? dday-right.dday : right.dday-dday)*JDFACT
      //<< " fday " << (fday > right.fday ? fday-right.fday : right.fday-fday)
      //*JDFACT*JDFACT << " eps " << CommonTime::eps << std::endl;
      if(jday == right.jday
         && std::abs(dday*JDFACT - right.dday*JDFACT
                   + fday*JDFACT*JDFACT - right.fday*JDFACT*JDFACT) < CommonTime::eps)
      {
         return true;
      }

      return false;
   }

   bool JulianDate::operator!=( const JulianDate& right ) const
   {
      return ( !operator==( right ) );
   }

   bool JulianDate::operator<( const JulianDate& right ) const
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != TimeSystem::Any &&
           right.timeSystem != TimeSystem::Any) &&
          timeSystem != right.timeSystem)
      {
         gpstk::InvalidRequest ir("CommonTime objects not in same time system,"
                                       " cannot be compared");
         GPSTK_THROW(ir);
      }

      if( jday < right.jday || dday < right.dday || fday < right.fday ) return true;
      return false;
   }

   bool JulianDate::operator>( const JulianDate& right ) const
   {
      return ( !operator<=( right ) );
   }

   bool JulianDate::operator<=( const JulianDate& right ) const
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool JulianDate::operator>=( const JulianDate& right ) const
   {
      return ( !operator<( right ) );
   }

} // namespace
