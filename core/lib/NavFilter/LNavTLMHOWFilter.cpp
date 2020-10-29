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

#include "LNavTLMHOWFilter.hpp"
#include "LNavFilterData.hpp"

namespace gpstk
{
   LNavTLMHOWFilter ::
   LNavTLMHOWFilter()
   {
   }

   void LNavTLMHOWFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*i);
         uint32_t sfid = ((fd->sf[1] >> 8) & 0x07);
         bool valid =
               // check TLM preamble
            ((fd->sf[0] & 0x3fc00000) == 0x22c00000) &&
               // zero parity - 2 LSBs
            ((fd->sf[1] & 0x03) == 0) &&
               // < 604800 sow or < 100800 TOW counts
            (((fd->sf[1] >> 13) & 0x1ffff) < 100800) &&
               // subframe ID
            (sfid >= 1) && (sfid <= 5);
         if (valid)
            accept(fd, msgBitsOut);
         else
            reject(fd);
      }
   }
}
