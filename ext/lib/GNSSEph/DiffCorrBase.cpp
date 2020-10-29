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
 * DiffCorrBase.cpp.
 * A single CNAV/CNAV-2 Clokc Differential Correction (CDC) packet
 * in engineering units.
 *
 */
#include <iomanip>

#include "DiffCorrBase.hpp"
#include "GPSWeekSecond.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"
#include "YDSTime.hpp"

using namespace std;

namespace gpstk
{
   //----------------------------------------------------------------
   DiffCorrBase::DiffCorrBase()
   {
      dcDataType = dtUnknown;
      dataLoadedFlag = false;
   }

   //----------------------------------------------------------------
   DiffCorrBase::DiffCorrBase(const PackedNavBits& msg,
                            const unsigned startBit)
   {
      try
      {
         loadData(msg,startBit);
      }
      catch (InvalidParameter ip)
      {
         GPSTK_RETHROW(ip); 
      }
   }

   //----------------------------------------------------------------
   void DiffCorrBase::loadData(const PackedNavBits& msg, 
                            const unsigned startBit)
   {
      dcDataType = dtUnknown;
      dataLoadedFlag = false;

      unsigned startBit_topD = 0;
      unsigned startBit_tOD = 0; 
      unsigned startBit_prn = 0;
      unsigned prnBitLength = 0; 

         // Determine type of the message
      int numBits = msg.getNumBits();
         // CNAV-2, subframe 3, page 5
      if (numBits==274)
      {
         unsigned pageNo = msg.asUnsignedLong(8,6,1);
         if (pageNo==5)
         {
            startBit_topD = 14;
            startBit_tOD = 25;
            startBit_prn = 0;
            prnBitLength = 8;
         }
      }
         // CNAV
      else if (numBits==300)
      {
         unsigned mt = msg.asUnsignedLong(14,6,1);
         switch (mt)
         {
            case 13: { startBit_topD = 38; startBit_tOD= 49; startBit_prn = 8; prnBitLength = 6; break; }
            case 14: { startBit_topD = 38; startBit_tOD= 49; startBit_prn = 8; prnBitLength = 6; break; }
            case 34: { startBit_topD =127; startBit_tOD=138; startBit_prn = 8; prnBitLength = 6; break; }
         }
      }
         // If the message length and/or message type do not match any of the 
         // messages of interest, throw an error. 
      if (startBit_topD==0)
      {
         stringstream ss; 
         ss << "DiffCorrBase.loadData().  Invalid message length: " << numBits;
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip); 
      }

         // Obtain the times in terms of continuous GPS time
      unsigned sow_scale = 300;    // See IS-GPS-200 30.3.3.7.2.1. and 30.3.3.7.2.2.
      unsigned topD_sow = msg.asUnsignedLong(startBit_topD,11,sow_scale);
      unsigned tOD_sow = msg.asUnsignedLong(startBit_tOD,11,sow_scale);

         // Determine the correct week number.   
         // The time of prediction (top).  CNAV and CNAV-2 do not have
         // any extended nav mode.   Therefore, top shoudl be within a 
         // half week of the transmit time.   
      unsigned xmitWeek = static_cast<GPSWeekSecond>(msg.getTransmitTime()).week;
      double xmitSOW = static_cast<GPSWeekSecond>(msg.getTransmitTime()).sow;
      double diff = xmitSOW - topD_sow; 
      unsigned topD_week = xmitWeek;
      if (diff < -HALFWEEK)
         topD_week--;
      else if (diff > HALFWEEK)
         topD_week++; 
      topD = GPSWeekSecond(topD_week,topD_sow);
      topD.setTimeSystem(TimeSystem::GPS);

      cout << " topD: " << printTime(topD,"%02m/%02d/%04Y %02H:%02M:%02S %P ") << endl;

          // Same check for the reference time.
      diff = xmitSOW - topD_sow;
      unsigned tOD_week = xmitWeek;
      if (diff < -HALFWEEK)
        tOD_week--;
      else if (diff > HALFWEEK)
        tOD_week++;
      tOD = GPSWeekSecond(tOD_week,tOD_sow);
      tOD.setTimeSystem(TimeSystem::GPS);
      cout << " tOD:  " << printTime(topD,"%02m/%02d/%04Y %02H:%02M:%02S %P ") << endl;

         // Obtain the transmit PRN ID.
      unsigned prnID = msg.asUnsignedLong(startBit_prn,prnBitLength,1);
      xmitSv = SatID(prnID, SatelliteSystem::GPS); 

   } 
  
   //----------------------------------------------------------------
   bool DiffCorrBase::isSameData(const DiffCorrBase& right) const
   {
      if (topD           != right.topD)           return false;
      if (tOD            != right.tOD)            return false;
      if (subjSv         != right.subjSv)          return false;
      if (xmitSv         != right.xmitSv)         return false;
      if (dcDataType     != right.dcDataType)     return false; 
      if (dataLoadedFlag != right.dataLoadedFlag) return false;
      return true;
   }

   //----------------------------------------------------------------
   void DiffCorrBase::dump(std::ostream& s) const
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s << endl
        << "           CLOCK DIFFERENTIAL CORRECTION PARAMETERS"
        << endl
        << "Parameter              Value" << endl;

      string tform="  %02m/%02d/%04Y %02H:%02M:%02S  Week %F  SOW %6.0g";

      s << "top-D        " << printTime(topD,tform) << endl;
      s << "tOD          " << printTime(tOD,tform) << endl;
      s << "subjSv                   " << subjSv << endl;
      s << "xmitSv                   " << xmitSv << endl;
      s << "DC Data Type " << setw(18);
      switch(dcDataType)
      {
         case dtUnknown: { s << "dtUnknown"; break;}
         case dtLNAV:    { s << "LNAV";      break;}
         case dtCNAV:    { s << "CNAV";      break;}
         case dtCNAV2:   { s << "CNAV-2";    break;}
         default: s << "dtUnknown";
      }
      s << endl;
   }
}


