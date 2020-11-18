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

#include "Exception.hpp"
#include "GloEphemerisStore.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"
#include "TestUtil.hpp"
#include "GPSWeekSecond.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavData.hpp"
#include "SatelliteSystem.hpp"

using namespace std;

namespace gpstk
{
   ostream& operator<<(ostream& s, const gpstk::SatelliteSystem sys)
   {
      s << gpstk::StringUtils::asString(sys);
      return s;
   }
}

class OrbElemStore_T
{
public:
   std::string inputRN3Data;

   OrbElemStore_T()
   {
      std::string dataFilePath = gpstk::getPathData();
      std::string tempFilePath = gpstk::getPathTestTemp();
      std::string fileSep = gpstk::getFileSep();

      inputRN3Data = dataFilePath + fileSep + "mixed.06n";
   }


      /** This tests the behavior of OrbElemStore when the store is
       * empty.  Under normal circumstances the map for a given
       * satellite would not be empty, but that can't be guaranteed as
       * soon as edit() is used. */
   unsigned doFindEphEmptyTests()
   {
      TUDEF("OrbElemStore","Empty Store Tests");
      try
      {
         gpstk::GloEphemerisStore store;
         gpstk::Rinex3NavData nd = loadNav(store, testFramework, true);
         TUASSERTE(gpstk::SatelliteSystem,
                   gpstk::SatelliteSystem::Glonass, nd.sat.system);
         gpstk::CommonTime searchTime(nd.time);
         gpstk::SatID sat(nd.sat);

            // make sure the ephemeris is in the store
         TUCSM("size");
         TUASSERTE(unsigned, 1, store.size());

            // make sure we can find it
         TUCSM("findNearEphemeris");
         TUCATCH(store.findNearEphemeris(sat, searchTime));

         TUCSM("findEphemeris");
         TUCATCH(store.findEphemeris(sat, searchTime));

            // remove the ephemeris
         TUCSM("edit");
         store.edit(store.getFinalTime() + 604800);

            // make sure the ephemeris has been removed
         TUCSM("size");
         TUASSERTE(unsigned, 0, store.size());

            // make sure we can't find it and don't seg fault
         TUCSM("findNearEphemeris");
         try 
         {
            const gpstk::GloEphemeris& ge =
               store.findNearEphemeris(sat, searchTime); 
            TUFAIL("Called findNearEphemeris for empty store and FAILED to"
                   " throw InvalidRequest");
         }
         catch (gpstk::InvalidRequest)
         {
            TUPASS("Called findNearEphemeris for empty store and received"
                   " InvalidRequest as expected.");
         }

         TUCSM("findEphemeris");
         try 
         {
            const gpstk::GloEphemeris& ge =
               store.findEphemeris(sat, searchTime); 
            TUFAIL("Called findEphemeris for empty store and FAILED to"
                   " throw InvalidRequest");
         }
         catch (gpstk::InvalidRequest)
         {
            TUPASS("Called findEphemeris for empty store and received"
                   " InvalidRequest as expected.");
         }
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


   unsigned computeXvtTest()
   {
      TUDEF("GloEphemerisStore", "computeXvt");
      try
      {
         gpstk::GloEphemerisStore store;
         gpstk::Rinex3NavData nd = loadNav(store, testFramework, false);
         gpstk::Xvt rv;
         gpstk::SatID fake(933, gpstk::SatelliteSystem::Glonass);
         TUCATCH(rv = store.computeXvt(nd.sat, nd.time));
         TUASSERTE(gpstk::Xvt::HealthStatus,
                   gpstk::Xvt::HealthStatus::Healthy, rv.health);
         TUCATCH(rv = store.computeXvt(fake, nd.time));
         TUASSERTE(gpstk::Xvt::HealthStatus,
                   gpstk::Xvt::HealthStatus::Unavailable, rv.health);
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }
      TURETURN();
   }


   unsigned getSVHealthTest()
   {
      TUDEF("GloEphemerisStore", "getSVHealth");
      try
      {
         gpstk::GloEphemerisStore store;
         gpstk::Rinex3NavData nd = loadNav(store, testFramework, false);
         gpstk::Xvt::HealthStatus rv;
         gpstk::SatID fake(933, gpstk::SatelliteSystem::Glonass);
         TUCATCH(rv = store.getSVHealth(nd.sat, nd.time));
         TUASSERTE(gpstk::Xvt::HealthStatus,
                   gpstk::Xvt::HealthStatus::Healthy, rv);
         TUCATCH(rv = store.getSVHealth(fake, nd.time));
         TUASSERTE(gpstk::Xvt::HealthStatus,
                   gpstk::Xvt::HealthStatus::Unavailable, rv);
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }
      TURETURN();
   }


   gpstk::Rinex3NavData loadNav(gpstk::GloEphemerisStore& store,
                                gpstk::TestUtil& testFramework,
                                bool firstOnly)
   {
      gpstk::Rinex3NavStream ns(inputRN3Data.c_str());
      gpstk::Rinex3NavHeader nh;
      gpstk::Rinex3NavData nd;
      TUASSERT(ns.good());
      ns >> nh;
      TUASSERT(ns.good());
      ns >> nd;
      if (firstOnly)
      {
         while ((nd.sat.system != gpstk::SatelliteSystem::Glonass) && ns)
         {
            ns >> nd;
         }
         if (nd.sat.system != gpstk::SatelliteSystem::Glonass)
         {
               // somehow got through the source file without any GLONASS data
            TUFAIL("input file did not contain GLONASS data");
            return nd;
         }
         TUASSERT(store.addEphemeris(nd));
      }
      else
      {
         while (ns)
         {
            ns >> nd;
            if (nd.sat.system == gpstk::SatelliteSystem::Glonass)
            {
               TUASSERT(store.addEphemeris(nd));
            }
         }
      }
      return nd;
   }

};


int main(int argc, char *argv[])
{
   unsigned total = 0;
   OrbElemStore_T testClass;
   total += testClass.doFindEphEmptyTests();
   total += testClass.computeXvtTest();
   total += testClass.getSVHealthTest();

   cout << "Total Failures for " << __FILE__ << ": " << total << endl;
   return total;
}
