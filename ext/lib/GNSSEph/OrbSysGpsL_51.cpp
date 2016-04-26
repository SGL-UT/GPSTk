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
 * @file OrbSysGpsL_51.cpp
 * OrbSysGpsL_51 data encapsulated in engineering terms
 */
#include <iomanip>

#include "OrbSysGpsL_51.hpp"
#include "GPSWeekSecond.hpp"
#include "MJD.hpp"
#include "StringUtils.hpp"
#include "TimeConstants.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   const unsigned short hBit[] =
   {
        0,                          // Index 0 is not used
       90,  96, 102, 108,    // Word  4
      120, 126, 132, 138,    // Word  5
      150, 156, 162, 168,    // Word  6
      180, 186, 192, 198,    // Word  7
      210, 216, 222, 228,    // Word  8
      240, 246, 252, 258     // Word  9
   };

   OrbSysGpsL_51::OrbSysGpsL_51()
      :OrbSysGpsL(), toa(0), WNa(0)
   {
      for (int i=0;i< 9;i++) { health[i] = 0; }
      ctToa = MJD(GPS_EPOCH_MJD,TimeSystem::GPS); 
   }

   OrbSysGpsL_51::OrbSysGpsL_51(const PackedNavBits& msg)
      throw( InvalidParameter):
      OrbSysGpsL()
   {
      loadData(msg);
   }

   OrbSysGpsL_51* OrbSysGpsL_51::clone() const
   {
      return new OrbSysGpsL_51 (*this); 
   }

   bool OrbSysGpsL_51::isSameData(const OrbData* right) const      
   {
         // First, test whether the test object is actually a OrbSysGpsL_51 object.
      const OrbSysGpsL_51* p = dynamic_cast<const OrbSysGpsL_51*>(right);
      if (p==0) return false; 

         // Establish if it refers to the same SV and UID. 
      if (!OrbSysGpsL::isSameData(right)) return false;
       
         // Finally, examine the contents
      for (int i=1; i<=24; i++)
      {
         if (health[i] != p->health[i]) return false;
      }
      return true;      
   }
   
   void OrbSysGpsL_51::loadData(const PackedNavBits& msg)
      throw(InvalidParameter)
   {
      setUID(msg);
      if (UID!=51)
      {
         stringstream ss;
         ss << "Expected GPS Subframe 5, Page 25, SVID 51 (525).  Found unique ID ";
         ss << StringUtils::asString(UID);
         InvalidParameter exc(ss.str());
         GPSTK_THROW(exc);    
      } 

         // Clear any existing data 
      for (int i=0;i<=24;i++) { health[i] = 0; }

      obsID        = msg.getobsID();
      satID        = msg.getsatSys();
      beginValid   = msg.getTransmitTime();

      toa = msg.asUnsignedLong(68,8,4096);
      WNa = (unsigned short) msg.asUnsignedLong(76,8,1);

      for (int i=1;i<=24;i++)
      {
         health[i] = (unsigned short) msg.asUnsignedLong(hBit[i],6,1);
      }

      unsigned short currXMitWeek = static_cast<GPSWeekSecond>(beginValid).week;
      unsigned short currXMitWeek8bit = currXMitWeek & 0x00FF; 
      unsigned short currXMitWeekMSBs = currXMitWeek & 0xFF00;
      unsigned short WNaFull = currXMitWeekMSBs + WNa; 
      short diff8 = (short) WNa - (short) currXMitWeek8bit;
      if (diff8 < -127) WNaFull += 128; 
      if (diff8 >  127) WNaFull -= 128;
      ctToa = GPSWeekSecond(WNaFull, toa, TimeSystem::GPS);

      dataLoadedFlag = true;   
   } // end of loadData()

   void OrbSysGpsL_51::dumpTerse(std::ostream& s) const
         throw(InvalidRequest)
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      string ssys = SatID::convertSatelliteSystemToString(satID.system); 
      s << setw(7) << ssys;
      s << " " << setw(2) << satID.id;

      string tform="%02m/%02d/%04Y %02H:%02M:%02S";
      s << "  51";      // UID
      s << " " << printTime(beginValid,tform) << "  ";
      s << "toa: " << printTime(ctToa,tform) << "  ";

         // We are going to be cute here, count the number of unique
         // health bit combinations and summarize them by frequency.
      map<unsigned short, unsigned short> fMap;
      map<unsigned short, unsigned short>::iterator it;
      for (int i=1;i<=24;i++)
      {
         it = fMap.find(health[i]);
         if (it==fMap.end())
         {
            unsigned short count = 0; 
            map<unsigned short, unsigned short>::value_type p(health[i],count);
            fMap.insert(p);
            it = fMap.find(health[i]);
         }
         (it->second)++; 
      }
      for (it=fMap.begin();it!=fMap.end();it++)
      {
         if (it!=fMap.begin()) s << ", ";
         s << "Hlt 0x" 
           << hex << setfill('0') << setw(2) << it->first
           << dec << setfill(' ') << ":#" << it->second;
      }
   } // end of dumpTerse()

   void OrbSysGpsL_51::dumpBody(ostream& s) const
      throw( InvalidRequest )
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s.setf(ios::fixed, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(0);
      s.fill(' ');

      s << endl;
      s << " toa, WNa : " << toa << ", " << WNa << endl;
      string tform2 = "%02m/%02d/%04Y %02H:%02M:%02S  %4F %6.0g";
      s << " Full Toa : " << printTime(ctToa,tform2); 

      s << endl;
      s << "SV Health" << endl;
      s << " PRN  hex  dec   PRN  hex dec   PRN  hex dec   PRN  hex dec" << endl;      
      for (int i=1; i<=24; i++)
      {
         unsigned prnNum = i;
         s << "  " << setw(2) << prnNum << ": 0x";
         s << hex << setw(2) << health[i] << "  " 
           << dec << setw(2) << health[i] << " ";
         if (i%4==0) s << endl; 
      }      
   } // end of dumpBody()   

} // end namespace gpstk
