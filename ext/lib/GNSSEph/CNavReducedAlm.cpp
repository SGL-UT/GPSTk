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
 * @file CNavReducedAlm.cpp
 * CNavReducedAlm data encapsulated in engineering terms
 *
 */

#include <iomanip>

#include "CNavReducedAlm.hpp"
#include "GPSWeekSecond.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   const double CNavReducedAlm::A_ref = 26559710.0;

   CNavReducedAlm::CNavReducedAlm()
      :L1HEALTH(1),
       L2HEALTH(1), 
       L5HEALTH(1),
       dataLoadedFlag(0)
   {
   }

   CNavReducedAlm::CNavReducedAlm(const AlmType almType, const CommonTime& ctAlm, 
                                  const PackedNavBits& pnb, const unsigned int startBit)
      :L1HEALTH(1),
       L2HEALTH(1), 
       L5HEALTH(1),
       dataLoadedFlag(0)
   {
      loadData(atCNAV2, ctAlm, pnb, startBit);
   }

   bool CNavReducedAlm::isSameData(const CNavReducedAlm& right) const      
   {
            // Finally, examine the contents
      if (ctAlmEpoch  != right.ctAlmEpoch) return false;
      if (subjSv      != right.subjSv)     return false;
      if (A           != right.A)          return false;
      if (OMEGA0      != right.OMEGA0)     return false;
      if (Psi0        != right.Psi0)       return false;
      if (L1HEALTH    != right.L1HEALTH)   return false;
      if (L2HEALTH    != right.L2HEALTH)   return false;
      if (L5HEALTH    != right.L5HEALTH)   return false;
      return true; 
   }


   void CNavReducedAlm::loadData(const AlmType almType, 
                                 const CommonTime& ctAlm, 
                                 const PackedNavBits& pnb, 
                                 const unsigned int startBit)
   {
         // Verify that the PackedNavBits contains an appropriate data set
      const ObsID& oidr = pnb.getobsID();
         // Check CNAV-2 case
      if (almType==atCNAV2)
      {
         unsigned pageID = pnb.asUnsignedLong(8,6,1);
         if (pageID!=3) 
         {
            stringstream ss;
            ss << "CNavReducedAlm::loadData().  Expected CNAV-2, Subframe 3, Page 3.   Found page " << pageID;
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip); 
         }
      }
         // Check CNAV case
      else
      {
         unsigned mt = pnb.asUnsignedLong(14,6,1);
         if (mt!=31 && mt!=12)
         {
            stringstream ss;
            ss << "CNavReducedAlm::loadData().  Expected CNAV, MT 12 or MT 21.   Found MT " << mt;
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip); 
         }
      }

         // Verify PackedNavBits object has sufficient size
      unsigned endBit = startBit + 31;
      if (almType==atCNAV2) endBit += 2;
      if (endBit>pnb.getNumBits())
      {
         stringstream ss;
         ss << "Requested packet from bits " << startBit << "-" << (endBit-1) << " but there are only " 
            << pnb.getNumBits() << " in the PackedNavBits object."; 
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip);
      }

      unsigned prnLen = 6;
      unsigned offset = 0;
      if (almType==atCNAV2) { offset+=2; prnLen=8; }
      
      unsigned prnId = pnb.asUnsignedLong(startBit, prnLen, 1);
      if (prnId==0)
      {
         stringstream ss;
         ss << "Reduced almanac packet starting at bit " << startBit << " has PRN of 0."
            << "  It does not contain data. ";
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip);
      }

      ctAlmEpoch = ctAlm;

      subjSv = SatID(prnId, SatelliteSystem::GPS);
      unsigned nextStart = startBit + 6 + offset;
      deltaA = pnb.asSignedDouble(nextStart, 8, 9);
      A = deltaA + A_ref;

      nextStart = startBit + 14 + offset;
      OMEGA0 = pnb.asDoubleSemiCircles(nextStart, 7, -6); 

      nextStart = startBit + 21 + offset;
      Psi0 = pnb.asDoubleSemiCircles(nextStart, 7, -6); 

      nextStart = startBit + 28 + offset;
      L1HEALTH = (unsigned short) pnb.asUnsignedLong(nextStart,1,1);
      L2HEALTH = (unsigned short) pnb.asUnsignedLong(nextStart+1,1,1);
      L5HEALTH = (unsigned short) pnb.asUnsignedLong(nextStart+2,1,1);

      dataLoadedFlag = true;   
   } // end of loadData()

         /** Output the contents of this orbit data to the given stream.
          * @throw Invalid Request if the required data has not been stored.
          */
   void CNavReducedAlm::dumpTerse(std::ostream& s) const
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      string ssys = convertSatelliteSystemToString(subjSv.system); 
      s << setw(7) << ssys;
      s << ":" << setw(2) << setfill('0') << subjSv.id << setfill(' ');
   }

   void CNavReducedAlm::dumpHeader(std::ostream& s)
   {
      s << "PRN         deltaA(m)       OMEGA0(rad)         Psi0(rad)  L1 L2 L5" << endl;
   }


   void CNavReducedAlm::dumpBody(std::ostream& s) const
   {
      s << setw(2) << setfill('0') << subjSv.id;
      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(10);
      s.fill(' ');
      s << " " << setw(17) << deltaA << " " << setw(17) << OMEGA0 << " " << setw(17) << Psi0;

      s.setf(ios::fixed, ios::floatfield);
      s.precision(0);
      s << "   " << L1HEALTH;
      s << "  " << L2HEALTH;
      s << "  " << L5HEALTH;
      s << endl;
   }


}
