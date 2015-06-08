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
 * @file OrbElemBase.cpp
 * OrbElemBase data encapsulated in engineering terms
 */

#include "OrbElemBase.hpp"
#include "StringUtils.hpp"
#include "GPSEllipsoid.hpp"
#include "YDSTime.hpp"
#include "CivilTime.hpp"
#include "TimeSystem.hpp"
#include "TimeString.hpp"
#include "MathBase.hpp"


namespace gpstk
{
   using namespace std;
   using namespace gpstk;
    OrbElemBase::OrbElemBase()
      :dataLoadedFlag(false),
       ctToe(CommonTime::BEGINNING_OF_TIME),
       beginValid(CommonTime::BEGINNING_OF_TIME),
       endValid(CommonTime::BEGINNING_OF_TIME)
   {
      healthy = false;
   }

    bool OrbElemBase::isValid(const CommonTime& ct) const
      throw(InvalidRequest)
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      if (ct >= beginValid && ct <= endValid) return(true);
      return(false);
   }


   bool OrbElemBase::dataLoaded() const
   {
      return(dataLoadedFlag);
   }

   bool OrbElemBase::isHealthy() const
      throw(InvalidRequest)
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return(healthy);
   }

      // The base dump method exists in order that a dump() with
      // minimum functionality is guaranteed to exist.  The
      // test that the data are loaded is implemented, the header,
      // body, and footer are called, and any output formats are
      // preserved. 
   void OrbElemBase::dump(ostream& s) const
      throw( InvalidRequest )
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      const ios::fmtflags oldFlags = s.flags();
      
      dumpHeader(s);
      dumpBody(s);
      dumpFooter(s);
      
      s.flags(oldFlags);
   }

} // namespace
