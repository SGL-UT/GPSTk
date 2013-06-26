#pragma ident "$Id$"
#ifndef GPSTK_SATID_HPP
#define GPSTK_SATID_HPP
#include <iostream>
#include <iomanip>
#include <sstream>
#include "gps_constants.hpp"

namespace gpstk
{
   class SatID;

   class SatID
   {
   public:

      // left in because RinexSatId uses them, remove this in the future if possible
      enum SatelliteSystem
      {
         systemGPS = 1,
         systemGalileo,
         systemGlonass,
         systemGeosync,
         systemLEO,
         systemTransit,
         systemCompass,
         systemMixed,
         systemUserDefined,
         systemUnknown
      };

      SatID() { id=-1; system=systemGPS; }
      // SatID(int p, SatelliteSystem s) { id=p; system=s; }
      // static std::string convertSatelliteSystemToString(SatelliteSystem s)
      // {
      //    switch(s)
      //    {
      //       case systemGPS:     return "GPS";           break;
      //       case systemGalileo: return "Galileo";       break;
      //       case systemGlonass: return "GLONASS";       break;
      //       case systemGeosync: return "Geostationary"; break;
      //       case systemLEO:     return "LEO";           break;
      //       case systemTransit: return "Transit";       break;
      //       case systemCompass: return "Compass";       break;
      //       case systemMixed:   return "Mixed";         break;
      //       case systemUserDefined:   return "UserDefined";         break;
      //       case systemUnknown: return "Unknown";       break;
      //       default:            return "??";            break;
      //    };
      // }

      void dump(std::ostream& s) const
      {
         s << convertSatelliteSystemToString(system) << " " << id;
      }
      bool operator==(const SatID& right) const
      { return ((system == right.system) && (id == right.id)); }

      bool operator!=(const SatID& right) const
      { return !(operator==(right)); }

      bool operator<(const SatID& right) const
      {
         if (system==right.system)
            return (id<right.id);
         return (system<right.system);
      }

      bool operator>(const SatID& right) const
      {  return (!operator<(right) && !operator==(right)); }

      bool operator<=(const SatID& right) const
      { return (operator<(right) || operator==(right)); }

      bool operator>=(const SatID& right) const
      { return !(operator<(right)); }

      bool isValid() const
      {
         switch(system)
         {
            case systemGPS: return (id > 0 && id <= MAX_PRN_GPS);
            //case systemGalileo:
            //case systemGlonass:
            //case systemGeosync:
            //case systemLEO:
            //case systemTransit:
            default: return (id > 0 && id < 100);
         }
      }

      int id;
      // SatelliteSystem system;

   }; // class SatID
   
   namespace StringUtils
   {
      inline std::string asString(const SatID& p)
      {
         std::ostringstream oss;
         p.dump(oss);
         return oss.str();
      }
   }
   inline std::ostream& operator<<(std::ostream& s, const SatID& p)
   {
      p.dump(s);
      return s;
   }   
}

#endif


%extend gpstk::SatID {
   SatID(int p) throw() {
      gpstk::SatID helper;
      return new gpstk::SatID(p, helper.systemGPS);
   }
   SatID(int p, int sys) throw() {
      gpstk::SatID helper;
      if(sys == 1) return new gpstk::SatID(p, helper.systemGPS);
      if(sys == 2) return new gpstk::SatID(p, helper.systemGalileo);
      if(sys == 3) return new gpstk::SatID(p, helper.systemGlonass);
      if(sys == 4) return new gpstk::SatID(p, helper.systemGeosync);
      if(sys == 5) return new gpstk::SatID(p, helper.systemLEO);
      if(sys == 6) return new gpstk::SatID(p, helper.systemTransit);
      if(sys == 7) return new gpstk::SatID(p, helper.systemCompass);
      if(sys == 8) return new gpstk::SatID(p, helper.systemMixed);
      if(sys == 9) return new gpstk::SatID(p, helper.systemUserDefined);
      else         return new gpstk::SatID(p, helper.systemUnknown);
   }
};


%pythoncode %{
import __builtin__
SatID.__str__ = lambda self: asString(self)

class SatSystems:   
    (GPS, Galileo, Glonass, Geosync, LEO, Transit, 
     Compass, Mixed, UserDefined, Unknown) = range(1, 11)

%}