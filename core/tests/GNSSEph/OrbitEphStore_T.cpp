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
