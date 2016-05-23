#include "NavFilterKey.hpp"
#include "TimeString.hpp"

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

   void NavFilterKey::dump(std::ostream& s) const
   {
         // Use civil time format to accommodate multi-GNSS
      s << gpstk::printTime(timeStamp,"%02m/%02d/%4Y %02H:%02M:%04.1f ");
      s << std::setw(3) << prn << " " << stationID;
      if (rxID.length())
      {
         s << "/" << rxID;
      }
      s << " ";
      s << gpstk::ObsID::cbDesc[carrier] << ", " << gpstk::ObsID::tcDesc[code] << " "; 
   }

   std::ostream& operator<<(std::ostream& s, const NavFilterKey& nfk)
   {
      nfk.dump(s);
      return s; 
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
