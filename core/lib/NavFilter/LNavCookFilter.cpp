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

#include "LNavCookFilter.hpp"
#include "EngNav.hpp"

namespace gpstk
{
   LNavCookFilter ::
   LNavCookFilter()
   {
   }


   void LNavCookFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*i);
         cookSubframe(fd);
         msgBitsOut.push_back(fd);
      }
   }


   void LNavCookFilter ::
   cookSubframe(LNavFilterData* fd)
   {
      unsigned word;

         // Invert the first word only if it needs to be.  It appears
         // to always be the case that D30* = D39* for word 10.  Also
         // it appears you need to invert the entire subframe, if word
         // 1 is inverted.
      if ((fd->sf[0] & 0x3fc00000) == 0x1d000000)
      {
         for (word = 0; word < 10; word++)
            fd->sf[word] = fd->sf[word] ^ 0x3fffffff;
      }

         // no need to continue attempting to invert garbage data
      if ((fd->sf[0] & 0x3fc00000) != 0x22c00000)
         return;

      for (word = 1; word < 10; word++)
      {
         if (gpstk::EngNav::getd30(fd->sf[word-1]))
         {
            fd->sf[word] = fd->sf[word] ^ 0x3fffffc0;
         }
      }
   }
}
