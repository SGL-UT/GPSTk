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
 * @file OrbSysGpsL.cpp
 */

#include "OrbSysGpsL.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
      // Almanac period is 12.5 minutes long = 750 seconds. 
   static const unsigned long ALMANAC_PERIOD = 750;
   static const unsigned long FRAME_PERIOD = 30;

   OrbSysGpsL::OrbSysGpsL():
      OrbDataSys()
   { }

   bool OrbSysGpsL::isSameData(const OrbData* right) const
   {
         // First, test whether the test object is actually a OrbSysGpsL object.
      const OrbSysGpsL* p = dynamic_cast<const OrbSysGpsL*>(right);
      if (p==0) return false; 

      if (!OrbDataSys::isSameData(right))  return false; 
      return true;
   }

   std::list<std::string> OrbSysGpsL::compare(const OrbSysGpsL* right) const
   {
      std::list<std::string> retList = OrbDataSys::compare(right);
      return retList;
   }

   void OrbSysGpsL::dumpHeader(std::ostream& s) const
         throw( InvalidRequest )
   {
      s << "*******************************************************" << endl;
      s << " GPS System-level navigation message data.  UID: " << UID << endl;
      s << " Transmitting SV : " << satID << endl;
      s << " Transmit Time   : " 
        << printTime(beginValid,"%02m/%02d/%4Y DOY %03j %02H:%02M:%02S  %F %g")
        << endl;
   }

   void OrbSysGpsL::setUID(const PackedNavBits& pnb)
   {
      unsigned long sfNum = pnb.asUnsignedLong(49, 3, 1);
      if (sfNum==4 || sfNum==5) 
      {
         UID = pnb.asUnsignedLong(62,6,1);
      }
   }  

      // Of the system-level data, only Data ID 51 is in subframe 5.
      // All the others are in subframe 4
   unsigned short OrbSysGpsL::getSubframe() const
   {
      if (UID==51) return 5;
      return 4; 
   }

      // The page ID for some Data ID is dependent on where in the 
      // subframe cycle they are located.   For those Data IDs, the
      // transmit time (beginValid) must be referenced.
   unsigned short OrbSysGpsL::getPageID() const
   {
      unsigned short retVal = 0; 
      switch (UID)
      {
         case 51: { retVal=25; break;}
         case 52: { retVal=13; break;}
         case 53: { retVal=14; break;}
         case 54: { retVal=15; break;}
         case 55: { retVal=17; break;}
         case 56: { retVal=18; break;}

         case 58: { retVal=19; break;}
         case 59: { retVal=20; break;}

         case 60: { retVal=22; break;}
         case 61: { retVal=23; break;}
         case 63: { retVal=25; break;}

         case 57:
         case 62: 
         { 
            double sow = static_cast<GPSWeekSecond>(beginValid).sow;
            unsigned long lsow = (unsigned long) sow; 
            unsigned long secInAlmPeriod = lsow % ALMANAC_PERIOD;
            unsigned long page = secInAlmPeriod / FRAME_PERIOD; 
            retVal = (unsigned short) page; 
            break;
         }
         default: break;
      }
      return retVal;
   }

} // namespace
