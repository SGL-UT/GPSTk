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

/**
 * @file FFData.cpp
 * Formatted File Data base class
 */

#include "FFData.hpp"
#include "FFStream.hpp"

namespace gpstk
{
   void FFData::putRecord(FFStream& s) const 
      throw(FFStreamError, gpstk::StringUtils::StringException)
   { 
      s.tryFFStreamPut(*this); 
   }
   
   void FFData::getRecord(FFStream& s)
      throw(FFStreamError, gpstk::StringUtils::StringException)
   { 
      s.tryFFStreamGet(*this); 
   }
   
   std::ostream& operator<<(FFStream& o, const FFData& f)
         throw(FFStreamError, gpstk::StringUtils::StringException)
   {
      f.putRecord(o);
      return o;
   }

   std::istream& operator>>(FFStream& i, FFData& f)
         throw(FFStreamError, gpstk::StringUtils::StringException)
   {
      f.getRecord(i);
      return i;
   }
}
