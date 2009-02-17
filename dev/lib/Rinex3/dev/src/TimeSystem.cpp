#pragma ident "$Id: TimeSystem.cpp 1649 2009-01-27 23:53:20Z raindave $"


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

#include "TimeSystem.hpp"

namespace gpstk
{
   // Static initialization of const std::strings for asString().
   // Must parallel enum SystemsEnum in TimeSystem.hpp.
   // NB: DO NOT use std::map here; on some systems initialization fails.
   const std::string TimeSystem::Strings[TAI+1] =
     {
       std::string("Unknown"),
       std::string("Any"),
       std::string("GPS"),
       std::string("GLO"),
       std::string("GAL"),
       std::string("UTC"),
       std::string("TAI")
     };

   void TimeSystem::setTimeSystem( const SystemsEnum& sys )
      throw()
   {
      if (sys < 0 || sys > TAI)
      {
         system = Unknown;
      }
      else
      {
         system = sys;
      }
   }

   void TimeSystem::fromString(const std::string str)
      throw()
   {
      system = Unknown;
      for(int i=0; i<=TAI; i++) {
         if(Strings[i] == str) {
            system = static_cast<SystemsEnum>(i);
            break;
         }
      }
   }

}   // end namespace
