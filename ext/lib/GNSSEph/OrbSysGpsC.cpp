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
 * @file OrbSysGpsC.cpp
 */

#include "OrbSysGpsC.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   OrbSysGpsC::OrbSysGpsC():
      OrbDataSys()
   { }

   bool OrbSysGpsC::isSameData(const OrbData* right) const
   {
         // First, test whether the test object is actually a OrbSysGpsC object.
      const OrbSysGpsC* p = dynamic_cast<const OrbSysGpsC*>(right);
      if (p==0) return false; 

      if (!OrbDataSys::isSameData(right))  return false; 
      return true;
   }

   std::list<std::string> OrbSysGpsC::compare(const OrbSysGpsC* right) const
   {
      std::list<std::string> retList = OrbDataSys::compare(right);
      return retList;
   }

   void OrbSysGpsC::dumpHeader(std::ostream& s) const
         throw( InvalidRequest )
   {
      s << "*********************************************************" << endl;
      s << " GPS CNAV System-level navigation message data.  UID: " << UID << endl;
      s << " Transmitting SV : " << satID << endl;
      s << " Transmit Time   : " 
        << printTime(beginValid,"%02m/%02d/%4Y DOY %03j %02H:%02M:%02S  %F %6.0g")
        << endl;
   }

   void OrbSysGpsC::setUID(const PackedNavBits& pnb)
   {
      UID = pnb.asUnsignedLong(14,6,1);
   }  

      // Of the system-level data, only Data ID 51 is in subframe 5.
      // All the others are in subframe 4
   unsigned short OrbSysGpsC::getMT() const
   {
      return UID;
   }

} // namespace
