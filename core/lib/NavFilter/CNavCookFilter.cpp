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

#include "CNavCookFilter.hpp"

namespace gpstk
{
   CNavCookFilter ::
   CNavCookFilter()
   {
   }


   void CNavCookFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         CNavFilterData *fd = dynamic_cast<CNavFilterData*>(*i);
         cookSubframe(fd);
         msgBitsOut.push_back(fd);
      }
   }

   void CNavCookFilter ::
   cookSubframe(CNavFilterData* fd)
   {
      unsigned word = fd->pnb->asUnsignedLong(0,8,1.0);
      if (word!=0x8b) fd->pnb->invert();
   }
}
