#include "SatelliteSystem.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(SatelliteSystem e) throw()
      {
         switch (e)
         {
            case SatelliteSystem::Unknown:     return "Unknown";
            case SatelliteSystem::GPS:         return "GPS";
            case SatelliteSystem::Galileo:     return "Galileo";
            case SatelliteSystem::Glonass:     return "GLONASS";
            case SatelliteSystem::Geosync:     return "Geostationary";
            case SatelliteSystem::LEO:         return "LEO";
            case SatelliteSystem::Transit:     return "Transit";
            case SatelliteSystem::BeiDou:      return "BeiDou";
            case SatelliteSystem::QZSS:        return "QZSS";
            case SatelliteSystem::IRNSS:       return "IRNSS";
            case SatelliteSystem::Mixed:       return "Mixed";
            case SatelliteSystem::UserDefined: return "UserDefined";
            default:                           return "???";
         } // switch (e)
      } // asString(SatelliteSystem)


      SatelliteSystem asSatelliteSystem(const std::string& s) throw()
      {
         if (s == "Unknown")
            return SatelliteSystem::Unknown;
         if (s == "GPS")
            return SatelliteSystem::GPS;
         if (s == "Galileo")
            return SatelliteSystem::Galileo;
         if (s == "GLONASS")
            return SatelliteSystem::Glonass;
         if (s == "Geostationary")
            return SatelliteSystem::Geosync;
         if (s == "LEO")
            return SatelliteSystem::LEO;
         if (s == "Transit")
            return SatelliteSystem::Transit;
         if (s == "BeiDou")
            return SatelliteSystem::BeiDou;
         if (s == "QZSS")
            return SatelliteSystem::QZSS;
         if (s == "IRNSS")
            return SatelliteSystem::IRNSS;
         if (s == "Mixed")
            return SatelliteSystem::Mixed;
         if (s == "UserDefined")
            return SatelliteSystem::UserDefined;
         return SatelliteSystem::Unknown;
      } // asSatelliteSystem(string)
   } // namespace StringUtils
} // namespace gpstk
