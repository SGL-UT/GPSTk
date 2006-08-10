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

#include "gpstkplatform.h"
#include "SystemTime.hpp"
#include "ANSITime.hpp"

namespace gpstk
{
   SystemTime& SystemTime::operator=( const SystemTime& right )
      throw()
   {
      UnixTime::operator=( right );
      return *this;
   }

   SystemTime& SystemTime::check()
      throw()
   {
#if defined(ANSI_ONLY)
      time_t t;
      time( &t );
      *this = ANSITime( t );
#elif defined(WIN32)
      _timeb t;
      _ftime( &t );
      tv.tv_sec = t.time;
      tv.tv_usec = t.millitm * 1000;
#else
      gettimeofday( &tv, NULL );
#endif

      return *this;
   }

} // namespace
