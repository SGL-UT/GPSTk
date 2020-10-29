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

#include "LNavCrossSourceFilter.hpp"

namespace gpstk
{
   LNavCrossSourceFilter ::
   LNavCrossSourceFilter()
   {
   }

   void LNavCrossSourceFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator nmli;
      for (nmli = msgBitsIn.begin(); nmli != msgBitsIn.end(); nmli++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
         if (fd->timeStamp != currentTime)
         {
               // different time, so check out what we have
            examineSubframes(msgBitsOut);
            groupedNav.clear();
            currentTime = fd->timeStamp;
         }
            // add the subframe to our collection
         groupedNav[fd->prn][fd].push_back(*nmli);
      }
   }

   void LNavCrossSourceFilter ::
   finalize(NavMsgList& msgBitsOut)
   {
      examineSubframes(msgBitsOut);
      groupedNav.clear();
      currentTime.reset();
   }

   void LNavCrossSourceFilter ::
   examineSubframes(NavMsgList& msgBitsOut)
   {
      NavMap::const_iterator nmi;
      SubframeMap::const_iterator smi;
         // loop over each PRN/SV
      for (nmi = groupedNav.begin(); nmi != groupedNav.end(); nmi++)
      {
            // count of unique messages
         size_t uniqueBits = nmi->second.size();
            // count of total messages
         size_t msgCount = 0;
            // store the vote winner here
         LNavFilterData *winner = NULL;
            // store the largest number of "votes" for a subframe here
         size_t voteCount = 0;
         for (smi = nmi->second.begin(); smi != nmi->second.end(); smi++)
         {
            size_t msgs = smi->second.size();
            msgCount += msgs;
               // minimum # of useful votes
            if ((msgs > voteCount) && (msgs >= 2))
            {
               voteCount = msgs;
               winner = smi->first;
            }
         }
         if (msgCount < 3)
            winner = NULL; // not enough messages to have a useful vote

            // If winner is NULL, i.e. there is no winner, all
            // messages will be rejected below.  Otherwise only the
            // winners will be accepted.
         for (smi = nmi->second.begin(); smi != nmi->second.end(); smi++)
         {
            if (smi->first == winner)
               accept(smi->second, msgBitsOut);
            else
               reject(smi->second);
         }
      }
   }
}
