//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file OrbSysGpsL_55.cpp
 * OrbSysGpsL_55 data encapsulated in engineering terms
 */
#include <iomanip>

#include "OrbSysGpsL_55.hpp"
#include "GPSWeekSecond.hpp"
#include "MJD.hpp"
#include "StringUtils.hpp"
#include "TimeConstants.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   OrbSysGpsL_55::OrbSysGpsL_55()
      :OrbSysGpsL()
   { }

   OrbSysGpsL_55::OrbSysGpsL_55(const PackedNavBits& msg)
      :OrbSysGpsL()
   {
      loadData(msg);
   }

   OrbSysGpsL_55* OrbSysGpsL_55::clone() const
   {
      return new OrbSysGpsL_55 (*this); 
   }

   bool OrbSysGpsL_55::isSameData(const OrbData* right) const      
   {
         // First, test whether the test object is actually a OrbSysGpsL_55 object.
      const OrbSysGpsL_55* p = dynamic_cast<const OrbSysGpsL_55*>(right);
      if (p==0) return false; 

         // Establish if it refers to the same SV and UID. 
      if (!OrbSysGpsL::isSameData(right)) return false;
       
         // Finally, examine the contents
      if (reserved!=p->reserved) return false; 
      if (textMsg.compare(p->textMsg)!=0) return false;
      return true;      
   }
   
   void OrbSysGpsL_55::loadData(const PackedNavBits& msg)
   {
      setUID(msg);

      if (UID!=55)
      {
         stringstream ss;
         ss << "Expected GPS Subframe 4, Page 17, SV ID 55.  Found unique ID ";
         ss << StringUtils::asString(UID);
         InvalidParameter exc(ss.str());
         GPSTK_THROW(exc);    
      } 

         // Why isn't this in OrbDataSys and why is there no virtual
         // loadData( ) method in the ancestors? 
      obsID        = msg.getobsID();
      satID        = msg.getsatSys();
      beginValid   = msg.getTransmitTime();

      unsigned long uword;
      unsigned short ubyte;
      long ndx = 68;
      short mNdx = 0; 
      char msgAr[23];

      while (mNdx<22)
      {
         uword = msg.asUnsignedLong(ndx,8,1);
         ubyte = (unsigned short) uword;
         char c = validChar(ubyte);
         msgAr[mNdx] = c;

         ndx += 8;
            // Skip over parity bits as needed.   Specifically, after
            // mNdx 1, 4, 7, 10, 13, 16, & 19.  
         if ((mNdx%3)==1) 
            ndx += 6;  

            // Increment the index in the character array
         mNdx++;
      }
      msgAr[22] = '\0'; 
      textMsg = string(msgAr);

      ndx = 286;
      reserved = msg.asUnsignedLong(ndx,6,1);
 
      dataLoadedFlag = true;   
   } // end of loadData()

   //--------------------------------------------------------------
   // According to IS-GPS-200 20.3.3.5.1.8, only certain characters
   // are valid.  If the unsigned short converts to a valid 
   // character, return that character; otherwise return a blank.
   char OrbSysGpsL_55::validChar(const unsigned short testShort)
   {
      bool valid = false;
         // Test for A-Z
         // NOTE OCTAL NOTATION (Used because that's what is used
         // in the IS-GPS-200).
      if (testShort>=0101 && testShort<=0132)
         valid = true;

         // Test for 0-9
      else if (testShort>=060 && testShort<=071)
         valid = true;

         // Test for remaining valid characters. 
      else if (testShort== 053 ||    // +
               testShort== 055 ||    // -
               testShort== 056 ||    // decimal point
               testShort== 047 ||    // minute mark
               testShort==0370 ||    // degree sign
               testShort== 057 ||    // forward slash
               testShort== 040 ||    // Space
               testShort== 072 ||    // :
               testShort== 042 ) valid = true;

      char retVal = '_';   // Underscore is NOT a valid character, but it is printable. 
      if (valid)
      {
         retVal = (char) testShort;

         // degree sign is allowable, but doesn't always print in reduced ASCII.
         // Since only upper case A-Z are valid, we will use a small 'd'
         // to stand in for the degree sign.
         if (testShort==0370)
            retVal = 'd';        
      } 
      return retVal;  
   }

   void OrbSysGpsL_55::dumpTerse(std::ostream& s) const
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      string ssys = convertSatelliteSystemToString(satID.system); 
      s << setw(7) << ssys;
      s << " " << setw(2) << satID.id;

      string tform="%02m/%02d/%04Y %03j %02H:%02M:%02S";
      s << "  55";
      s << " " << printTime(beginValid,tform);
      s << "  Text message ";
      s << "[" << textMsg << "]" ; 
      if (textMsg.find('_')!=string::npos)
         s << " (underscores represent invalid characters)"; 
   } // end of dumpTerse()

   void OrbSysGpsL_55::dumpBody(ostream& s) const
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s << " Text message ";
      if (textMsg.find('_')!=string::npos)
         s << "(underscores represent invalid characters):";
      s << endl;
      s << "[" << textMsg << "]" << endl;

      s.setf(ios::uppercase);
      s.precision(0);
      s.fill('0');
      s << hex; 

      s << "Reserved bits (6 bits, right-justified): 0x" 
        << setw(2) << reserved;
      s << endl;
   } // end of dumpBody()   

} // end namespace gpstk
