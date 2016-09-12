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
 * @file OrbSysGpsL_63.cpp
 * OrbSysGpsL_63 data encapsulated in engineering terms
 */
#include <iomanip>

#include "OrbSysGpsL_63.hpp"
#include "GPSWeekSecond.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   const unsigned short cBit[] =
   {
        0,                          // Index 0 is not used
       68,  72,  76,  80,           // Word 3
       90,  94,  98, 102, 106, 110, // Word 4
      120, 124, 128, 132, 136, 140, // Word 5
      150, 154, 158, 162, 166, 170, // Word 6
      180, 184, 188, 192, 196, 200, // Word 7
      210, 214, 218, 222            // Word 8
   };
   const unsigned short hBit[] =
   {
        0,                          // Index 0 is not used
      228,                   // Word  8
      240, 246, 252, 258,    // Word  9
      270, 276, 282          // Word 10
   };

   OrbSysGpsL_63::OrbSysGpsL_63()
      :OrbSysGpsL()
   {
      for (int i=0;i< 9;i++) { health[i] = 0; }
      for (int i=0;i<33;i++) { config[i] = 0; }
   }

   OrbSysGpsL_63::OrbSysGpsL_63(const PackedNavBits& msg)
      throw( InvalidParameter):
      OrbSysGpsL()
   {
      loadData(msg);
   }

   OrbSysGpsL_63* OrbSysGpsL_63::clone() const
   {
      return new OrbSysGpsL_63 (*this); 
   }

   bool OrbSysGpsL_63::isSameData(const OrbData* right) const      
   {
         // First, test whether the test object is actually a OrbSysGpsL_63 object.
      const OrbSysGpsL_63* p = dynamic_cast<const OrbSysGpsL_63*>(right);
      if (p==0) return false; 

         // Establish if it refers to the same SV and UID. 
      if (!OrbSysGpsL::isSameData(right)) return false;
       
         // Finally, examine the contents
      for (int i=1; i<9; i++)
      {
         if (health[i] != p->health[i]) return false;
      }
      for (int i=1; i<33; i++)
      {
         if (config[i] != p->config[i]) return false;
      }
      return true;      
   }
   
   void OrbSysGpsL_63::loadData(const PackedNavBits& msg)
      throw(InvalidParameter)
   {
      setUID(msg);
      if (UID!=63)
      {
         stringstream ss;
         ss << "Expected GPS Subframe 4, Page 25, SVID 63 (425).  Found unique ID ";
         ss << StringUtils::asString(UID);
         InvalidParameter exc(ss.str());
         GPSTK_THROW(exc);    
      } 

         // Clear any existing data 
      for (int i=0;i< 9;i++) { health[i] = 0; }
      for (int i=0;i<33;i++) { config[i] = 0; }

      obsID        = msg.getobsID();
      satID        = msg.getsatSys();
      beginValid   = msg.getTransmitTime();

      for (int i=1;i<33;i++)
      {
         config[i] = (unsigned short) msg.asUnsignedLong(cBit[i],4,1);
      }
      for (int i=1;i<9;i++)
      {
         health[i] = (unsigned short) msg.asUnsignedLong(hBit[i],6,1);
      }

      dataLoadedFlag = true;   
   } // end of loadData()

   void OrbSysGpsL_63::dumpTerse(std::ostream& s) const
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
      s << "  63";      // UID
      s << " " << printTime(beginValid,tform) << "  ";

         // We are going to be cute here, count the number of unique
         // SV config and health bit combinations and summarize them 
         //by frequency.
      map<unsigned short, unsigned short> fMap;
      map<unsigned short, unsigned short>::iterator it;
      for (int i=1;i<=32;i++)
      {
         it = fMap.find(config[i]);
         if (it==fMap.end())
         {
            unsigned short count = 0; 
            map<unsigned short, unsigned short>::value_type p(config[i],count);
            fMap.insert(p);
            it = fMap.find(config[i]);
         }
         (it->second)++; 
      }
      for (it=fMap.begin();it!=fMap.end();it++)
      {
         unsigned short cfg = it->first;
         if (it!=fMap.begin()) s << ", ";
         s << "Cfg ";
         if (cfg & 0x8) s << "1"; 
           else s<< "0";
         if (cfg & 0x4) s << "1";
           else s << "0";
         if (cfg & 0x2) s << "1";
           else s << "0";
         if (cfg & 0x1) s << "1";
           else s << "0";
         s << ":#" << it->second;
      }
      s << "  ";
      
      fMap.clear(); 
      for (int i=1;i<=8;i++)
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

   void OrbSysGpsL_63::dumpBody(ostream& s) const
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
      s << "SV Configuration" << endl << setfill('0');
      for (int j=1; j<=8; j++)
          s << "  PRN     ";
      s << endl;
      for (int j=1; j<=32; j++)
      {
         s << "   " << setw(2) << j << ":";
         if (config[j] & 0x8) s << "1"; 
           else s<< "0";
         if (config[j] & 0x4) s << "1";
           else s << "0";
         if (config[j] & 0x2) s << "1";
           else s << "0";
         if (config[j] & 0x1) s << "1";
           else s << "0";
         if (j%8==0) s << endl;
      }

      s << endl;
      s << "SV Health" << endl;
      s << " PRN  hex  dec   PRN  hex dec   PRN  hex dec   PRN  hex dec" << endl;      
      for (int i=1; i<=8; i++)
      {
         unsigned prnNum = 24 + i;
         s << "  " << setw(2) << prnNum << ": 0x";
         s << hex << setw(2) << health[i] << "  " 
           << dec << setw(2) << health[i] << " ";
         if (i%4==0) s << endl; 
      }      
   } // end of dumpBody()   

} // end namespace gpstk
