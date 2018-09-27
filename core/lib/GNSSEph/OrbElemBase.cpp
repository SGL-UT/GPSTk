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

      // Compare the data contents of two OrbElemBase objects.
      //
      // 1.) The beginValid comparison is omitted since beginValid
      // is fequently derived from the transmit time.  Therefore,
      // two otherwise identical sets of data could disagree
      // on beginValid and still be the same data set. 
      //
      // 2.) The obsID comparison is omitted due to the fact that
      // in most cases, data should be identical across different
      // carriers/codes. This may be address by keeping separate 
      // stores of messages at a higher level. 
      //
   bool OrbElemBase::isSameData(const OrbElemBase* right) const
   {
      if (dataLoadedFlag != right->dataLoadedFlag) return false;
      if (satID          != right->satID)          return false;
      //if (obsID          != right->obsID)          return false;
      if (ctToe          != right->ctToe)          return false;
      if (healthy        != right->healthy)        return false;
      //if (beginValid     != right->beginValid)     return false;
      if (endValid       != right->endValid)       return false;
      return true;
   }

      // Compare, but also return a list of all differences found
   std::list<std::string> OrbElemBase::compare(const OrbElemBase* right) const
   {
      std::list<std::string> retList; 
      if (dataLoadedFlag != right->dataLoadedFlag) retList.push_back("dataLoadedFlag");
      if (satID          != right->satID)          retList.push_back("satID");
      if (obsID          != right->obsID)          retList.push_back("obsID");
      if (ctToe          != right->ctToe)          retList.push_back("ctToe");
      if (healthy        != right->healthy)        retList.push_back("healthy");
      if (beginValid     != right->beginValid)     retList.push_back("beginValid");
      if (endValid       != right->endValid)       retList.push_back("endValid");
      return retList; 
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


   Rinex3NavData OrbElemBase::makeRinex3NavData() const
         throw( InvalidRequest )
   {
      InvalidRequest ir("Method makeRinex3NavData() not implemented.");
      GPSTK_THROW(ir); 
   }
   


} // namespace
