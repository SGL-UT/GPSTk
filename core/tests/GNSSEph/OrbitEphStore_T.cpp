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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "OrbitEphStore.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"
#include "TestUtil.hpp"
#include "GPSWeekSecond.hpp"

using namespace std;

class OrbitEphStore_T
{
public:
      /** This tests the behavior of OrbitEphStore when the store is
       * empty.  Under normal circumstances the map for a given
       * satellite would not be empty, but that can't be guaranteed as
       * soon as edit() is used. */
   unsigned doFindEphEmptyTests()
   {
      TUDEF("OrbitEphStore","findUserOrbitEph");
      try
      {
         gpstk::OrbitEphStore store;
            // this eph will be removed by edit()
         gpstk::OrbitEph baleted;
         gpstk::SatID sat(11, gpstk::SatID::systemGPS);
         gpstk::ObsID obsID(gpstk::ObsID::otNavMsg,
                            gpstk::ObsID::cbL1,
                            gpstk::ObsID::tcCA);
         baleted.dataLoadedFlag = true;
         baleted.satID = sat;
         baleted.obsID = obsID;
         baleted.ctToe = gpstk::GPSWeekSecond(1917, 576000);
         baleted.ctToc = gpstk::GPSWeekSecond(1917, 576000);
         baleted.beginValid = baleted.ctToe - 3600;
         baleted.endValid = baleted.ctToe + 3600;

         gpstk::CommonTime searchTime(baleted.ctToe);

         TUCSM("addEphemeris");
         store.addEphemeris(&baleted);

            // make sure the ephemeris is in the store
         TUCSM("size");
         TUASSERTE(unsigned, 1, store.size());

            // make sure we can find it
         TUCSM("findNearOrbitEph");
         TUASSERT(store.findNearOrbitEph(sat, searchTime) != NULL);

         TUCSM("findUserOrbitEph");
         TUASSERT(store.findUserOrbitEph(sat, searchTime) != NULL);

            // remove the ephemeris
         TUCSM("edit");
         store.edit(baleted.endValid + 604800);

            // make sure the ephemeris has been removed
         TUCSM("size");
         TUASSERTE(unsigned, 0, store.size());

            // make sure we can't find it and don't seg fault
         TUCSM("findNearOrbitEph");
         TUASSERT(store.findNearOrbitEph(sat, searchTime) == NULL);

         TUCSM("findUserOrbitEph");
         TUASSERT(store.findUserOrbitEph(sat, searchTime) == NULL);
      }
      catch (gpstk::Exception &exc)
      {
         cerr << exc << endl;
         TUFAIL("Unexpected exception");
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }
      TURETURN();
   }
};


int main(int argc, char *argv[])
{
   unsigned total = 0;
   OrbitEphStore_T testClass;
   total += testClass.doFindEphEmptyTests();

   cout << "Total Failures for " << __FILE__ << ": " << total << endl;
   return total;
}
