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
#include "CNavFilterData.hpp"
#include "CNavEmptyFilter.hpp"

namespace gpstk
{
   CNavEmptyFilter ::
   CNavEmptyFilter()
   {
   }

      // Bits 39-276 are a total of 238 bits.   That's 7 * 32 with 
      // a remainder of 14.
   void CNavEmptyFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      unsigned long word[8];

      NavMsgList::const_iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         CNavFilterData *fd = dynamic_cast<CNavFilterData*>(*i);

         int startBit = 38; 
         for (int n=0;n<7;n++)
         {
            word[n] = fd->pnb->asUnsignedLong(startBit,32,1);
            startBit += 32;
         }
         word[7] = fd->pnb->asUnsignedLong(startBit,14,1); 

         bool blank =
            ( (word[0]==0) &&
              (word[1]==0) &&
              (word[2]==0) &&
              (word[3]==0) &&
              (word[4]==0) &&
              (word[5]==0) &&
              (word[6]==0) &&
              (word[7]==0) ) ||

            ( (word[0]==0x55555555) &&
              (word[1]==0x55555555) &&
              (word[2]==0x55555555) &&
              (word[3]==0x55555555) &&
              (word[4]==0x55555555) &&
              (word[5]==0x55555555) &&
              (word[6]==0x55555555) &&
              (word[7]==0x00001555) ) /*||

            ( (word[0]==0xAAAAAAAA) &&         This is valid for default nav
              (word[1]==0xAAAAAAAA) &&
              (word[2]==0xAAAAAAAA) &&
              (word[3]==0xAAAAAAAA) &&
              (word[4]==0xAAAAAAAA) &&
              (word[5]==0xAAAAAAAA) &&
              (word[6]==0xAAAAAAAA) &&
              (word[7]==0x00002AAA) ) */;

         if (blank)
            reject(fd);
         else
            accept(fd, msgBitsOut);
      }
   }
}
