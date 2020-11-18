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

#include "LNavEmptyFilter.hpp"
#include "LNavFilterData.hpp"

namespace gpstk
{
   LNavEmptyFilter ::
   LNavEmptyFilter()
   {
   }

   void LNavEmptyFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*i);
         bool blank =
               // all zeroes
            ((fd->sf[0] == 0) && (fd->sf[1] == 0) && (fd->sf[2] == 0) &&
             (fd->sf[3] == 0) && (fd->sf[4] == 0) && (fd->sf[5] == 0) &&
             (fd->sf[6] == 0) && (fd->sf[7] == 0) && (fd->sf[8] == 0) && 
             (fd->sf[9] == 0)) ||
               // or subframe 4 or 5 with alternating 1s and 0s
            ((((fd->sf[1] & 0x700) == 0x400) ||
              ((fd->sf[1] & 0x700) == 0x500)) &&
             ((fd->sf[2] & 0x3ffc0) == 0x2aa80) &&
             ((fd->sf[3] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[4] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[5] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[6] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[7] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[8] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[9] & 0x3ffffc0) == 0x2aaaa80));
         if (blank)
            reject(fd);
         else
            accept(fd, msgBitsOut);
      }
   }
}
