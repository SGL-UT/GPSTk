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

#include "NavOrderFilter.hpp"
#include "GPSWeekZcount.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   NavOrderFilter ::
   NavOrderFilter(unsigned depth, unsigned epoch)
         : procDepth(depth),
           epochInSeconds(epoch)
   {
   }


   void NavOrderFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator nmli;
      CommonTime oldestAllowed; // default to super old i.e. any data is ok
      oldestAllowed.setTimeSystem(gpstk::TimeSystem::Any);
      if (!orderedNav.empty())
      {
         SubframeSet::reverse_iterator rssi = orderedNav.rbegin();
         CommonTime newest = (*rssi)->timeStamp;
         oldestAllowed = newest - (epochInSeconds * procDepth);
      }
      for (nmli = msgBitsIn.begin(); nmli != msgBitsIn.end(); nmli++)
      {
            // Do not add anything that's too old.  We do the checking
            // here instead of in examineSubframes so that if the nav
            // timestamp jumps forward more than one epoch, it doesn't
            // cause perfectly valid nav data to be rejected.
         if ((*nmli)->timeStamp < oldestAllowed)
         {
            reject(*nmli);
            continue;
         }
         NavFilterKey *fd = dynamic_cast<NavFilterKey*>(*nmli);
         orderedNav.insert(fd);
      }
      examineSubframes(msgBitsOut);
   }


   void NavOrderFilter ::
   finalize(NavMsgList& msgBitsOut)
   {
      std::copy(orderedNav.begin(), orderedNav.end(),
                std::back_inserter(msgBitsOut));
      orderedNav.clear();
   }


   void NavOrderFilter ::
   examineSubframes(NavMsgList& msgBitsOut)
   {
      using gpstk::printTime;
      if (orderedNav.empty())
         return; // nothing to do

      SubframeSet::iterator ssi, oldIt = orderedNav.end();
      SubframeSet::reverse_iterator rssi = orderedNav.rbegin();
      CommonTime newest = (*rssi)->timeStamp;
      CommonTime oldestAllowed = newest - (epochInSeconds * procDepth);
      SubframeSet::iterator oldestIt = orderedNav.begin();
      GPSWeekZcount oldestTime = (*oldestIt)->timeStamp;
         /*
      std::cerr << "NavOrderFilter"
                << "  n=" << printTime(newest,"%Fw%Zz")
                << "  o=" << printTime(oldestTime,"%Fw%Zz")
                << "  oa=" << printTime(oldestAllowed,"%Fw%Zz")
                << std::endl;
         */

      for (ssi = orderedNav.begin(); ssi != orderedNav.end(); ssi++)
      {
         if ((*ssi)->timeStamp <= oldestAllowed)
         {
            accept(*ssi, msgBitsOut);
               // This subframe is being accepted, so include it as
               // one to be erased from the set when we're done.
            oldIt = ssi;
         }
         else
         {
               // Since we're processing in time order and we've
               // reached data newer than the oldest allowed, we don't
               // have any more processing to do.
            break;
         }
      }
         // if oldIt is end() then we don't have anything to erase.
      if (oldIt != orderedNav.end())
      {
            // erase is up to BUT NOT INCLUDING the second iterator,
            // so increment oldIt to make sure to erase the last
            // element we need to erase.
         orderedNav.erase(orderedNav.begin(), ++oldIt);
      }
   }

}
