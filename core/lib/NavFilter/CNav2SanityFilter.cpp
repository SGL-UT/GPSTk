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
//
//  Implements a handful of structural sanity checks for GPS CNAV-2 messages.
//    - TOI + ITOW must equal transmit time.
//    - PRN ID in subframe 3 must equal PRN of transmitting SV.
//    - Page No. in subframe 3 must be in the valid range (1-6).
//
#include "CNav2SanityFilter.hpp"
#include "CNavFilterData.hpp"
#include "GPSWeekSecond.hpp"

namespace gpstk
{
   CNav2SanityFilter::
   CNav2SanityFilter()
   {
   }

   void CNav2SanityFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         CNavFilterData *fd = dynamic_cast<CNavFilterData*>(*i);
         uint32_t msgWeek = (uint32_t) fd->pnb->asUnsignedLong(9,13,1); 
         uint32_t TOI   = (uint32_t) fd->pnb->asUnsignedLong(0,9,1);
         uint32_t ITOW  = (uint32_t) fd->pnb->asUnsignedLong(22,8,1);
         unsigned bitOffset = 9 + 600;
         uint32_t PRN = (int32_t) fd->pnb->asUnsignedLong(bitOffset,8,1);
         bitOffset += 8; 
         uint32_t pageNum = (uint32_t) fd->pnb->asUnsignedLong(bitOffset,6,1);

         unsigned msgSOW = 7200 * ITOW + TOI * 18; 
         msgSOW -= 18;
         if (msgSOW<0)
            msgSOW += 604800; 
         unsigned xmitSOW = static_cast<GPSWeekSecond>(fd->pnb->getTransmitTime()).sow;
         unsigned xmitWeek = static_cast<GPSWeekSecond>(fd->pnb->getTransmitTime()).week;
 
         bool valid =
               // check SOW time is consistent
            ( msgSOW == xmitSOW &&
               // check week number is consistent
              msgWeek == xmitWeek &&
               // check PRN is consistent
              PRN == fd->pnb->getsatSys().id &&
               // check subframe 3 page number is valid
             (pageNum>=1 && pageNum<=6) );
         if (valid)
            accept(fd, msgBitsOut);
         else
            reject(fd);
      }
   }
}
