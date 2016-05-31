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
 * @file OrbSysGpsL_52.cpp
 * OrbSysGpsL_52 data encapsulated in engineering terms
 */
#include <iomanip>

#include "OrbSysGpsL_52.hpp"
#include "GPSWeekSecond.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   const int OrbSysGpsL_52::max_erd = 30; 

   OrbSysGpsL_52::OrbSysGpsL_52()
      :OrbSysGpsL(),
       availIndicator(2),
       erd(max_erd),
       erdAvail(max_erd)
   {}

   OrbSysGpsL_52::OrbSysGpsL_52(const PackedNavBits& msg)
      throw( InvalidParameter):
      OrbSysGpsL(),
      erd(max_erd),
      erdAvail(max_erd)

   {
      loadData(msg);
   }

   OrbSysGpsL_52* OrbSysGpsL_52::clone() const
   {
      return new OrbSysGpsL_52 (*this); 
   }

   bool OrbSysGpsL_52::isSameData(const OrbData* right) const      
   {
         // First, test whether the test object is actually a OrbSysGpsL_52 object.
      const OrbSysGpsL_52* p = dynamic_cast<const OrbSysGpsL_52*>(right);
      if (p==0) return false; 

         // Establish if it refers to the same SV and UID. 
      if (!OrbSysGpsL::isSameData(right)) return false;
       
      if (availIndicator != p->availIndicator) return false;
      
      for (int i=0; i<max_erd; i++)
      {
         if (erd[i] != p->erd[i]) return false;
         if (erdAvail[i] != p->erdAvail[i]) return false;
      }
       
      return true;      
   }
   
   void OrbSysGpsL_52::loadData(const PackedNavBits& msg)
      throw(InvalidParameter)
   {
      setUID(msg);
      if (UID!=52)
      {
         char errStr[80];
         std::string msgString("Expected GPS Subframe 4, Page 13, SVID 52 (413).  Found unique ID ");
         msgString += StringUtils::asString(UID);
         InvalidParameter exc(msgString);
         GPSTK_THROW(exc);    
      } 
      obsID          = msg.getobsID();
      satID          = msg.getsatSys();
      beginValid     = msg.getTransmitTime();
      availIndicator = (unsigned short)msg.asUnsignedLong(68, 2, 1);
      
      int startBit   = 70;
      int numBits    = 6;
      int offset     = 1;
      int ndx        = 0;
      double scale   = 0.3;
      double unscaled = 0.0; 
      unsigned long erdAsBits;
      unsigned int start2[2];
      unsigned int num2[]     = {2,4};
      int parityLen  = 6;
      
      while (ndx<max_erd)
      {
         if (offset<3)
         {
            erdAsBits = msg.asUnsignedLong(startBit, numBits, 1); 
            unscaled  = msg.asSignedDouble(startBit, numBits, 0.0);
            startBit += numBits;
            offset++;
         }
         else
         {
            start2[0] = startBit;
            start2[1] = startBit + num2[0] + parityLen;

               // The lsb of this quantity is NOT a power of 2, but
               // a linear scale factor. 
            erdAsBits = msg.asUnsignedLong(start2, num2, 2, 1); 
            unscaled  = msg.asSignedDouble(start2, num2, 2, 0.0);
            startBit  = startBit + numBits + parityLen;
            offset    = 0;
         }

         if (erdAsBits==0x20)
         {
            erdAvail[ndx] = false;
            erd[ndx] = 0.0;
         }
         else
         {
            erdAvail[ndx] = true;
            erd[ndx]  = unscaled * scale;
         }
         ndx++;
      }
      dataLoadedFlag = true;
   }
   
   void OrbSysGpsL_52::dumpTerse(std::ostream& s = std::cout) const
         throw( InvalidRequest )
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
      s << "  52";
      s << " " << printTime(beginValid,tform) << "  ";      

      s << "NMCT ";       
         //Correlate availIndicator with NMCT table in IS-GPS-200
      string value;
      switch(availIndicator) {
         case 0 : value = "0 - Unencrypted";   break;
         case 1 : value = "1 - Encrypted";     break;
         case 2 : value = "2 - Not Available"; break;
         case 3 : value = "3 - Reserved";      break; 
         } 
         s << value;
   }
         
   void OrbSysGpsL_52::dumpBody(ostream& s) const
      throw( InvalidRequest )
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s << endl
        << "           GPS NMCT Data"
        << endl
        << "Parameter              Value" << endl;

      s.setf(ios::fixed, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(1);
      s.fill(' ');
      string value;
      
         //Correlate availIndicator with NMCT table in IS-GPS-200
      switch(availIndicator) {
         case 0 : value = "0 - Unencrypted";   break;
         case 1 : value = "1 - Encrypted";     break;
         case 2 : value = "2 - Not Available"; break;
         case 3 : value = "3 - Reserved";      break; 
         } 
         s << "availIndicator " << setw(17) << value << endl;
      if (availIndicator == 0)
      {
         for (size_t i=0; i<max_erd; i++)
         {
               s << "ERD[" << setw(2) << i << "]         " 
                 << setw(12) << erd[i] << " meters";
               if (erdAvail[i]) s << "   valid";
                  else          s << " INVALID";
               s << endl;
         }
      }
      else
      {
         s << "NMCT data is encrypted, not available, or reserved. No"
           << " ERD output will be displayed." << endl;
      }
      
   } // end of dumpBody()
   
   double OrbSysGpsL_52::getERD(const SatID& sidr) const
      throw(InvalidRequest)
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      if (availIndicator != 0)
      {
         InvalidRequest exc("NMCT data is not unencrypted.");
         GPSTK_THROW(exc);
      }
      if (sidr.id <1 | sidr.id >=max_erd+1)
      {
         InvalidRequest exc("SatID out of range");
         GPSTK_THROW(exc);
      }
      if (sidr == satID)
      {
         InvalidRequest exc("SatID cannot match the transmitting SatID.");
         GPSTK_THROW(exc);
      }
      
      int ndx = sidr.id - 1;
      if (sidr.id > satID.id) ndx--;
      if (ndx > 30)
      {
         InvalidRequest exc("NMCT only valid for PRN 1-31.");
         GPSTK_THROW(exc);
      }
      if (!erdAvail[ndx])
      {
         stringstream ss;
         ss << "No valid erd for " << sidr; 
         InvalidRequest exc(ss.str());
         GPSTK_THROW(exc);
      }   
       
      return erd[ndx];
   }

} // end namespace gpstk
