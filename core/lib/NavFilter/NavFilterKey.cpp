#include "NavFilterKey.hpp"

namespace gpstk
{
   NavFilterKey ::
   NavFilterKey()
         : stationID(""),
           rxID(""),
           carrier(ObsID::cbUnknown),
           code(ObsID::tcUnknown)
   {
   }


/* don't use this...
   bool NavFilterKey ::
   operator<(const NavFilterKey& right) const
   {
      if (stationID < right.stationID) return true;
      if (stationID > right.stationID) return false;
      if (rxID < right.rxID) return true;
      if (rxID > right.rxID) return false;
      if (carrier < right.carrier) return true;
      if (carrier > right.carrier) return false;
      if (code < right.code) return true;
         //if (code > right.code) 
      return false;
   }
*/
}
