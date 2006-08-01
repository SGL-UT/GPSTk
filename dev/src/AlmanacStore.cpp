#pragma ident "$Id$"



//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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






/**
 * @file AlmanacStore.cpp
 * Store almanac information, and access by satellite and time
 */

#include "AlmanacStore.hpp"
#include "StringUtils.hpp"
#include "EngAlmanac.hpp"
#include "gps_constants.hpp"

namespace gpstk
{
   Xvt AlmanacStore::getPrnXvt(short prn, const gpstk::DayTime& t)
      const throw(AlmanacStore::NoAlmanacFound)
   {
      AlmOrbit a = findAlmanac(prn, t);
      return a.svXvt(t);
   }

   short AlmanacStore::getPrnHealth(short prn, const gpstk::DayTime& t)
      const throw(AlmanacStore::NoAlmanacFound)
   {
      AlmOrbit a = findAlmanac(prn, t);
      return a.getSVHealth();
   }

   bool AlmanacStore::addAlmanac(const AlmOrbit& alm) throw()
   {
      if ((alm.getPRNID() >= 1) && (alm.getPRNID() <= MAX_PRN))
      {
         uba[alm.getPRNID()][alm.getToaTime()] = alm;
         return true;
      }
      return false;
   }

   bool AlmanacStore::addAlmanac(const EngAlmanac& alm) throw()
   {
      AlmOrbits ao = alm.getAlmOrbElems();
      AlmOrbits::const_iterator oci;
      for (oci = ao.begin();
           oci != ao.end();
           oci++)
      {
         addAlmanac((*oci).second);
      }
      return true;
   }

      /// gets the closest almanac for the given time and prn,
      /// closest being in the past or future.
   AlmOrbit AlmanacStore::findAlmanac(short prn, const gpstk::DayTime& t) 
      const throw(AlmanacStore::NoAlmanacFound)
   {
      UBAMap::const_iterator prnItr = uba.find(prn);
      if (prnItr == uba.end())
      {
         NoAlmanacFound nef("No almanacs for prn " + 
                            gpstk::StringUtils::asString(prn));
         GPSTK_THROW(nef);
      }
         
      const EngAlmMap& eam = (*prnItr).second;

         // find the closest almanac BEFORE t, if any.
      EngAlmMap::const_iterator nextItr = eam.begin(),
         almItr = eam.end();
         
      while ( (nextItr != eam.end()) &&
              ((*nextItr).first < t) )
      {
         almItr = nextItr;
         nextItr++;
      }

      if (almItr == eam.end())
      {
         if (nextItr == eam.end()) 
         {
            NoAlmanacFound nef("No almanacs for time " + t.asString());
            GPSTK_THROW(nef);
         }
         else
         {
            almItr = nextItr;
         }
      }

         // check the next almanac (the first one after t's time)
         // to see if it's closer than the one before t
      if (nextItr != eam.end())
      {
         if ( ((*nextItr).first - t) < (t - (*almItr).first))
            almItr = nextItr;
      }
      return (*almItr).second;
   }


   AlmOrbits AlmanacStore::findAlmanacs(const gpstk::DayTime& t) 
      const
   {
      AlmOrbits ao;
      UBAMap::const_iterator prnItr = uba.begin();
      while (prnItr != uba.end())
      {
         try
         {
            AlmOrbit a = findAlmanac((*prnItr).first, t);
            ao[(*prnItr).first] = a;
         }
            /// who cares about exceptions - the map will
            /// be empty if there are no alms...
         catch(...)
         {}

         prnItr++;
      }
      return ao;
   }
   
   gpstk::DayTime AlmanacStore::getInitialTime() 
      const
   {
      DayTime retDT = DayTime::END_OF_TIME;
      UBAMap::const_iterator prnItr = uba.begin();
      while (prnItr != uba.end())
      {
         const EngAlmMap& eam = (*prnItr).second;

         EngAlmMap::const_iterator nextItr;
         for (nextItr=eam.begin(); nextItr!=eam.end(); ++nextItr)
         {
            const AlmOrbit& ao = (*nextItr).second;
            try 
            {
               DayTime testT = ao.getToaTime();
               if (testT<retDT) retDT = testT;
            }
               // Not to worry, worst case method return 'END_OF_TIME'
            catch(...)
            {}
         }
         prnItr++;
      }
      return(retDT);
   }
   
}
