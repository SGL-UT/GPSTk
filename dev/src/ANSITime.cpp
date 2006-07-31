#pragma ident "$Id: //depot/sgl/gpstk/dev/src/ANSITime.cpp#2 $"

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

#include "ANSITime.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
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

         rv = formattedPrint( rv, getFormatPrefixInt() + "K",
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
