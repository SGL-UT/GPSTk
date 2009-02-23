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

#include <cmath>
#include "ANSITime.hpp"
#include "TimeConstants.hpp"

using namespace gpstk;

namespace Rinex3
{
   ANSITime& ANSITime::operator=( const ANSITime& right )
      throw()
   {
      time = right.time;
      timeSystem = right.timeSystem;
      return *this;
   }
   
   CommonTime ANSITime::convertToCommonTime() const
      throw( gpstk::InvalidRequest )
   {
      try
      {
         return CommonTime( ( MJD_JDAY + UNIX_MJD + time / SEC_PER_DAY ),
                            ( time % SEC_PER_DAY ),
                            0.,
                            timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ir);
      }
   }
   
   void ANSITime::convertFromCommonTime( const CommonTime& ct )
      throw( gpstk::InvalidRequest )
   {
         /// This is the earliest CommonTime for which ANSITimes are valid.
      static const CommonTime MIN_CT = ANSITime(0, TimeSys::Any);
         /// This is the latest CommonTime for which ANSITimes are valid.
         /// 2^31 - 1 seconds
      static const CommonTime MAX_CT = ANSITime(2147483647, TimeSys::Any);

      if ( ct < MIN_CT || ct > MAX_CT )
      {
         InvalidRequest ir("Unable to convert given CommonTime to ANSITime.");
         GPSTK_THROW(ir);
      }

      long jday, sod;
      double fsod;
      ct.get( jday, sod, fsod, timeSystem );
      
      time = 
         static_cast<time_t>((jday - MJD_JDAY - UNIX_MJD) * SEC_PER_DAY + sod);
   }
   
   std::string ANSITime::printf( const std::string& fmt) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "K",
                              "Klu", time );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", timeSystem.asString().c_str() );
         return rv;         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
   std::string ANSITime::printError( const std::string& fmt) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "K",
                              "Ks", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str() );
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
      
      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'K':
               time = asInt( i->second );
               break;

            case 'P':
               timeSystem = static_cast<TimeSys::Systems>(asInt( i->second ));
               break;

            default:
                  // do nothing
               break;
         };
      }
      
      return true;
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
   
   void ANSITime::reset() 
      throw()
   {
      time = 0;
      timeSystem = TimeSys::Unknown;
   }

   bool ANSITime::operator==( const ANSITime& right ) const
      throw()
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
     if ((timeSystem.getTimeSystem() != TimeSys::Any &&
          right.timeSystem.getTimeSystem() != TimeSys::Any) &&
	 timeSystem != right.timeSystem)
         return false;

      if( fabs(double(time - right.time)) < CommonTime::eps )
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
      throw( gpstk::InvalidRequest )
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
     if ((timeSystem.getTimeSystem() != TimeSys::Any &&
          right.timeSystem.getTimeSystem() != TimeSys::Any) &&
         timeSystem != right.timeSystem)
      {
         gpstk::InvalidRequest ir("CommonTime objects not in same time system, cannot be compared");
         GPSTK_THROW(ir);
      }

      return ( time < right.time );
   }

   bool ANSITime::operator>( const ANSITime& right ) const
      throw( gpstk::InvalidRequest )
   {
      return ( !operator<=( right ) );
   }

   bool ANSITime::operator<=( const ANSITime& right ) const
      throw( gpstk::InvalidRequest )
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool ANSITime::operator>=( const ANSITime& right ) const
      throw( gpstk::InvalidRequest )
   {
      return ( !operator<( right ) );
   }

} // namespace
