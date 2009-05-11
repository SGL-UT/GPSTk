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
 * @file BELogEntry.cpp
 */
#include "DayTime.hpp"
#include "BELogEntry.hpp"

namespace gpstk
{
   using namespace std;
   using namespace gpstk;

      // Initialize the static const.
   const std::string BELogEntry::header = "PRN Earliest SF 1 HOW !                 Toe                      IODC #Collected";
   
   BELogEntry::BELogEntry( const gpstk::EngEphemeris ee )
   {
      HOWSF1 = ee.getTransmitTime();
      Toe = ee.getEpochTime();
      PRN_ID = ee.getPRNID();
      IODC = ee.getIODC();
      count = 1;
      
      long shortweek = (long) ee.getFullWeek();
      shortweek &= 0x0000001F;
      long sixteenSecCount = long (Toe.GPSsow() / 16.0);
      
         // The purpose of the key is to enable placing these objects
         // into SV-specific maps ordered by 
         //    Primary :week
         //    Secondary :SOW
         //    Tertiary: IODC
         // The latter (IODC) is actually only a part of the key so as to
         // enforce uniqueness
      key = shortweek << 26 |
            sixteenSecCount << 10 | IODC;
   }
   
   unsigned long BELogEntry::getKey() const { return(key); }
   DayTime BELogEntry::getHOW() const { return(HOWSF1); }
   void BELogEntry::increment()    { count++; }
   
   std::string BELogEntry::getStr() const
   {
      std::string timeFmt1 = "%02m/%02d/%02y %02H:%02M:%02S";
      std::string timeFmt2 = "%02m/%02d/%02y %02H:%02M:%02S %03j %5.0s %04F %6.0g";
      char line[100];
      sprintf( line, " %02d %s ! %s 0x%03X %4d",
         PRN_ID, HOWSF1.printf( timeFmt1 ).c_str(),
                 Toe.printf(timeFmt2).c_str(),
                 IODC,count);
      std::string retStr(line);
      return(retStr);
   }
   
}   // namespace
