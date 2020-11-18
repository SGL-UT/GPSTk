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

#include "LNavEphMaker.hpp"
#include "EngNav.hpp"

namespace gpstk
{
   LNavEphMaker ::
   LNavEphMaker()
   {
   }


   void LNavEphMaker ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator nmli;
      completeEphs.clear();
      for (nmli = msgBitsIn.begin(); nmli != msgBitsIn.end(); nmli++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
         uint32_t sfid = EngNav::getSFID(fd->sf[1]);
         if ((sfid < 1) || (sfid > 3))
         {
               // ignore non-ephemeris data
            accept(fd, msgBitsOut);
            continue;
         }
         if (ephemerides.find(fd) == ephemerides.end())
         {
               // set up us the ephemeris
            ephemerides[fd].resize(3, NULL);
         }
         EphGroup &ephGrp(ephemerides[fd]);
         ephGrp[sfid-1] = fd;

            // determine if a complete, valid ephemeris is available

            // check that all subframes are present
         if ((ephGrp[0] == NULL) || (ephGrp[1] == NULL) || (ephGrp[2] == NULL))
         {
            continue;
         }
            // check that TOW counts are consecutive
         if ((EngNav::getTOW(ephGrp[1]->sf[1]) -
              EngNav::getTOW(ephGrp[0]->sf[1]) != 1) ||
             (EngNav::getTOW(ephGrp[2]->sf[1]) -
              EngNav::getTOW(ephGrp[1]->sf[1]) != 1))
         {
            continue;
         }
            // check that the IODE/IODC are consistent
         if (((ephGrp[0]->sf[7] >> 22) != (ephGrp[1]->sf[2] >> 22)) ||
             ((ephGrp[0]->sf[7] >> 22) != (ephGrp[2]->sf[9] >> 22)))
         {
            continue;
         }
            // everything checks out, put the data where the user can find it
         completeEphs.push_back(&ephGrp);
      }
   }


   void LNavEphMaker ::
   finalize(NavMsgList& msgBitsOut)
   {
         // Don't really need to do anything but clear out our own
         // data structures.  Any valid ephemerides would have been
         // immediately produced as soon as they had been determined
         // valid, and all subframes are passed.
      ephemerides.clear();
      completeEphs.clear();
   }

}
