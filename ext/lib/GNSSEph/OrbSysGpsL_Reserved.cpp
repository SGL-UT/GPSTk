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
 * @file OrbSysGpsL_Reserved.cpp
 * OrbSysGpsL_Reserved data encapsulated in engineering terms
 */
#include <iomanip>

#include "OrbSysGpsL_Reserved.hpp"
#include "GPSWeekSecond.hpp"
#include "MJD.hpp"
#include "StringUtils.hpp"
#include "TimeConstants.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   OrbSysGpsL_Reserved::OrbSysGpsL_Reserved()
      :OrbSysGpsL()
   { }

   OrbSysGpsL_Reserved::OrbSysGpsL_Reserved(const PackedNavBits& msg)
      throw( InvalidParameter)
      :OrbSysGpsL()
   {
      loadData(msg);
   }

   OrbSysGpsL_Reserved* OrbSysGpsL_Reserved::clone() const
   {
      return new OrbSysGpsL_Reserved (*this); 
   }

   bool OrbSysGpsL_Reserved::isSameData(const OrbData* right) const      
   {
         // First, test whether the test object is actually a OrbSysGpsL_Reserved object.
      const OrbSysGpsL_Reserved* p = dynamic_cast<const OrbSysGpsL_Reserved*>(right);
      if (p==0) return false; 

         // Establish if it refers to the same SV and UID. 
      if (!OrbSysGpsL::isSameData(right)) return false;
       
         // Finally, examine the contents
      if (!pnb.matchBits(p->pnb)) return false; 
      return true;      
   }
   
   void OrbSysGpsL_Reserved::loadData(const PackedNavBits& msg)
      throw(InvalidParameter)
   {
      setUID(msg);

      if (UID!=53 && UID!=54 &&
          (UID<57 && UID>62) )
      {
         stringstream ss;
         ss << "Expected GPS Subframe 4, Reserved Page.  Found unique ID ";
         ss << StringUtils::asString(UID);
         InvalidParameter exc(ss.str());
         GPSTK_THROW(exc);    
      } 

         // Why isn't this in OrbDataSys and why is there no virtual
         // loadData( ) method in the ancestors? 
      obsID        = msg.getobsID();
      satID        = msg.getsatSys();
      beginValid   = msg.getTransmitTime();

         // Set the meta-data equivalent to the meta-data in the
         // message. Then load the appropriate
         // bits from the message array into the reserved bit array.
      pnb = PackedNavBits(msg.getsatSys(),msg.getobsID(),
                          msg.getRxID(), msg.getTransmitTime());  

      unsigned long uword = msg.asUnsignedLong(68,16,1);
      pnb.addUnsignedLong(uword,16,1);
      for (int i=4; i<=9; i++)   // Word 4-10
      {
         long ndx = (i-1) * 30;
         uword = msg.asUnsignedLong(ndx,24,1);
         pnb.addUnsignedLong(uword,24,1);
      }
      uword = msg.asUnsignedLong(270,22,1);
      pnb.addUnsignedLong(uword,22,1);
      pnb.trimsize();

      dataLoadedFlag = true;   
   } // end of loadData()

   void OrbSysGpsL_Reserved::dumpTerse(std::ostream& s) const
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

      string tform="%02m/%02d/%04Y %03j %02H:%02M:%02S";
      s << "  " << setw(2) << UID;
      s << " " << printTime(beginValid,tform) << "  ";
      s << "Reserved bits "; 
   } // end of dumpTerse()

   void OrbSysGpsL_Reserved::dumpBody(ostream& s) const
      throw( InvalidRequest )
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s.setf(ios::uppercase);
      s.precision(0);
      s.fill('0');
      s << hex; 
      s << " Contents of Reserved Bits in Words 4-10 (with parity removed)" << endl;
      s << " 182 bits, left-justified in 32 bit words" << endl;
      unsigned long uword = 0; 
      long ndx;
      for (int i=0;i<4;i++)
      {
         ndx = i * 32; 
         uword = pnb.asUnsignedLong(ndx,32,1);
         s << " 0x" << setw(8) << uword;
      }
      ndx = 4 * 32;
      uword = pnb.asUnsignedLong(ndx,22,1);
      uword <<= 10; 
      s << " 0x" << setw(8) << uword; 
      s << endl;
   } // end of dumpBody()   

} // end namespace gpstk
