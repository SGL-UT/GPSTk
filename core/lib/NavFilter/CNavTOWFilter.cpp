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
#include "CNavTOWFilter.hpp"
#include "CNavFilterData.hpp"

namespace gpstk
{
   CNavTOWFilter::
   CNavTOWFilter()
   {
   }

   void CNavTOWFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         CNavFilterData *fd = dynamic_cast<CNavFilterData*>(*i);
         uint32_t preamble = (uint32_t) fd->pnb->asUnsignedLong(0,8,1);
         uint32_t msgType  = (uint32_t) fd->pnb->asUnsignedLong(14,6,1);
         uint32_t TOWCount = (uint32_t) fd->pnb->asUnsignedLong(20,17,1);

         bool valid =
               // check TLM preamble
            ( preamble == 0x0000008b &&
               // < 604800 sow or < 100800 TOW counts
              TOWCount < 100800 &&
               // subframe ID
             (  msgType==0 ||
               (msgType >= 10 && msgType <= 15 ) || 
               (msgType >= 30 && msgType <= 39 ) ) );
         if (valid)
            accept(fd, msgBitsOut);
         else
            reject(fd);
      }
   }
}
