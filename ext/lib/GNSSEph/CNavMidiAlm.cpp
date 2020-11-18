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
 * @file CNavMidiAlm.cpp
 * CNavMidiAlm data encapsulated in engineering terms
 *
 */

#include <iomanip>

#include "CNavMidiAlm.hpp"
#include "GPSWeekSecond.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   CNavMidiAlm::CNavMidiAlm()
      :OrbDataSys(),
       L1HEALTH(1),
       L2HEALTH(1), 
       L5HEALTH(1)
   {
      almType = matUnknown;
   }

   CNavMidiAlm::CNavMidiAlm(const PackedNavBits& pnb)
      :OrbDataSys(),
       L1HEALTH(1),
       L2HEALTH(1), 
       L5HEALTH(1)
   {
      almType = matUnknown;
      try
      {
         loadData(pnb);
      }
      catch (InvalidParameter ip)
      {
         GPSTK_RETHROW(ip); 
      }
   }

  //----------------------------------------------------------------
   CNavMidiAlm* CNavMidiAlm::clone() const
   {
      return new CNavMidiAlm (*this);
   }

   bool CNavMidiAlm::isSameData(const gpstk::OrbData* right) const      
   {
         // First, test whether the test object is actually a CNavMidiAlm object.
      const CNavMidiAlm* p = dynamic_cast<const CNavMidiAlm*>(right);
      if (p==0) return false;

            // Finally, examine the contents
      if (almType     != p->almType)    return false;
      if (ctAlmEpoch  != p->ctAlmEpoch) return false;
      if (xmitSv      != p->xmitSv)     return false;
      if (subjSv      != p->subjSv)     return false;
      if (e           != p->e)          return false;
      if (di          != p->di)         return false;
      if (OMEGAdot    != p->OMEGAdot)   return false;
      if (sqrtA       != p->sqrtA)      return false;
      if (OMEGA0      != p->OMEGA0)     return false;
      if (w           != p->w)          return false;
      if (M0          != p->M0)         return false;
      if (af0         != p->af0)        return false;
      if (af1         != p->af1)        return false;
      if (L1HEALTH    != p->L1HEALTH)   return false;
      if (L2HEALTH    != p->L2HEALTH)   return false;
      if (L5HEALTH    != p->L5HEALTH)   return false;
      return true; 
   }

   void CNavMidiAlm::loadData(const PackedNavBits& pnb)
   {
         // Verify that the PackedNavBits contains an appropriate data set
      const ObsID& oidr = pnb.getobsID();

         // Check CNAV-2 case
      if (pnb.getNumBits()==274)
      {
         unsigned pageID = pnb.asUnsignedLong(8,6,1);
         if (pageID!=4) 
         {
            stringstream ss;
            ss << "CNavMidiAlm::loadData().  Expected CNAV-2, Subframe 3, Page 4.   Found page " << pageID;
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip); 
         }
         almType = matCNAV2;
      }
         // Check CNAV case
      else
      {
         unsigned mt = pnb.asUnsignedLong(14,6,1);
         if (mt!=37)
         {
            stringstream ss;
            ss << "CNavMidiAlm::loadData().  Expected CNAV, MT 37.   Found MT " << mt;
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip); 
         }
         almType = matCNAV;
      }

      beginValid = pnb.getTransmitTime();
      obsID = pnb.getobsID();
      satID = pnb.getsatSys();

         // CNAV and CNAV-2 use the same bit layout, but different starting
         // locations. (This is true except for the transmit (xmit) SV.)  Therefore, 
         // we'll use the CNAV-2 starting bit indices that begin at bit 14 
         // (zero index).  If CNAV, then we will apply an offset that 
         // represents the difference between the CNAV (127, index 0) and
         // CNAV-2 starting locations.
      unsigned offsetBits = 0;                  
      if (almType==matCNAV) offsetBits = 127 - 14;

      unsigned prnLen = 6;
      unsigned prnStart = 8;
      if (almType==matCNAV2) { prnStart=0; prnLen=8; }
      unsigned long prnId = pnb.asUnsignedLong(prnStart, prnLen, 1);
      xmitSv = SatID(prnId,SatelliteSystem::GPS); 

      unsigned long wn = pnb.asUnsignedLong(14+offsetBits,13,1);
      unsigned long toaSOW = pnb.asUnsignedLong(27+offsetBits,8,4096);
      ctAlmEpoch = GPSWeekSecond(wn,(double) toaSOW);
      ctAlmEpoch.setTimeSystem(TimeSystem::GPS);

      prnId = pnb.asUnsignedLong(35+offsetBits,8,1);
      subjSv = SatID(prnId, SatelliteSystem::GPS);

      unsigned nextStart = 43 + offsetBits;
      L1HEALTH = (unsigned short) pnb.asUnsignedLong(nextStart,1,1);
      L2HEALTH = (unsigned short) pnb.asUnsignedLong(nextStart+1,1,1);
      L5HEALTH = (unsigned short) pnb.asUnsignedLong(nextStart+2,1,1);

      e        = pnb.asUnsignedDouble(46+offsetBits,11,-16);
      di       = pnb.asDoubleSemiCircles(57+offsetBits,11,-14);
      OMEGAdot = pnb.asDoubleSemiCircles(68+offsetBits,11,-33);
      sqrtA    = pnb.asSignedDouble(79+offsetBits,17,-4);
      OMEGA0   = pnb.asDoubleSemiCircles(96+offsetBits,16,-15);
      w        = pnb.asDoubleSemiCircles(112+offsetBits,16,-15);
      M0       = pnb.asDoubleSemiCircles(128+offsetBits,16,-15);
      af0      = pnb.asSignedDouble(144+offsetBits,11,-20);
      af1      = pnb.asSignedDouble(155+offsetBits,10,-37); 

      dataLoadedFlag = true;   
   } // end of loadData()

   void CNavMidiAlm::dumpHeader(std::ostream& s) const
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      s << "*********************************************************" << endl;
      s << " GPS CNAV System-level navigation message data.  Midi Almanac: " << endl;
      s << " Transmit Time   : "
        << printTime(beginValid,"%02m/%02d/%4Y DOY %03j %02H:%02M:%02S  %F %6.0g")
        << endl;
   }

         /** Output the contents of this orbit data to the given stream.
          * @throw Invalid Request if the required data has not been stored.
          */
   void CNavMidiAlm::dumpTerse(std::ostream& s) const
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      if (almType==matCNAV)
         s << "MT 37. ";
      else
         s << "UID 304. "; 
      s << " xmit PRN: " << xmitSv.id;
      s << " subject PRN: " << subjSv.id;
      s << " t_oa: " << printTime(ctAlmEpoch,"%02m/%02d/%04Y %02H:%02M:%02S %P") << endl;
   }

   void CNavMidiAlm::dumpBody(std::ostream& s) const
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s << endl
        << "           MIDI ALMANAC PARAMETERS"
        << endl
        << "Parameter              Value" << endl;

      string tform="  %02m/%02d/%04Y %02H:%02M:%02S  Week %F  SOW %6.0g";

      s << "t_oa         " << printTime(ctAlmEpoch,tform) << endl;
      switch(almType)
      {
         case matCNAV:  { s << "Message type  " << setw(17) << "CNAV"; break;}
         case matCNAV2: { s << "Message type  " << setw(17) << "CNAV-2"; break;}
      }
      s << endl;

      s << "Transmit SV" << setw(17) << xmitSv << endl;
      s << "Subject SV " << setw(17) << subjSv << endl;
 
      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(10);
      s.fill(' ');
      s << "e             " << setw(17) << e << " dimensionless" << endl;
      s << "di            " << setw(17) << di << " rad (ref to i0)" << endl;
      s << "OMEGAdot      " << setw(17) << OMEGAdot << " rad" << endl;
      s << "sqrtA         " << setw(17) << sqrtA << " m**0.5" << endl;
      s << "OMEGA0        " << setw(17) << OMEGA0 << " rad" << endl;
      s << "w             " << setw(17) << w << " rad" << endl;
      s << "M0            " << setw(17) << M0 << " rad" << endl;      s << "af0           " << setw(17) << af0 << " sec" << endl;
      s << "af1           " << setw(17) << af1 << " sec/sec" << endl;

      s.setf(ios::fixed, ios::floatfield);
      s.precision(0);
      s << "L1 Health     " << setw(17) << L1HEALTH << goodBad(L1HEALTH,s) << endl;
      s << "L2 Health     " << setw(17) << L2HEALTH << goodBad(L2HEALTH,s) << endl;
      s << "L5 Health     " << setw(17) << L5HEALTH << goodBad(L5HEALTH,s) << endl;
      s << endl;
   }

   std::string CNavMidiAlm::goodBad(const unsigned val, std::ostream& s) const
   {
      stringstream ss;
      if (val==0) ss << " Healthy";
       else ss << " Unhealthy";
      return ss.str();
   }

}
