#include "NavType.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(NavType e) throw()
      {
         switch (e)
         {
            case NavType::GPSLNAV:   return "GPS_LNAV";
            case NavType::GPSCNAVL2: return "GPS_CNAV_L2";
            case NavType::GPSCNAVL5: return "GPS_CNAV_L5";
            case NavType::GPSCNAV2:  return "GPS_CNAV2";
            case NavType::GPSMNAV:   return "GPS_MNAV";
            case NavType::BeiDou_D1: return "Beidou_D1";
            case NavType::BeiDou_D2: return "Beidou_D2";
            case NavType::GloCivilF: return "GloCivilF";
            case NavType::GloCivilC: return "GloCivilC";
            case NavType::GalFNAV:   return "GalFNAV";
            case NavType::GalINAV:   return "GalINAV";
            case NavType::IRNSS_SPS: return "IRNSS_SPS";
            case NavType::Unknown:   return "Unknown";
            default:                 return "???";
         } // switch (e)
      } // asString(NavType)


      NavType asNavType(const std::string& s) throw()
      {
         if (s == "GPS_LNAV")
            return NavType::GPSLNAV;
         if (s == "GPS_CNAV_L2")
            return NavType::GPSCNAVL2;
         if (s == "GPS_CNAV_L5")
            return NavType::GPSCNAVL5;
         if (s == "GPS_CNAV2")
            return NavType::GPSCNAV2;
         if (s == "GPS_MNAV")
            return NavType::GPSMNAV;
         if (s == "Beidou_D1")
            return NavType::BeiDou_D1;
         if (s == "Beidou_D2")
            return NavType::BeiDou_D2;
         if (s == "GloCivilF")
            return NavType::GloCivilF;
         if (s == "GloCivilC")
            return NavType::GloCivilC;
         if (s == "GalFNAV")
            return NavType::GalFNAV;
         if (s == "GalINAV")
            return NavType::GalINAV;
         if (s == "IRNSS_SPS")
            return NavType::IRNSS_SPS;
         if (s == "Unknown")
            return NavType::Unknown;
         return NavType::Unknown;
      } // asNavType(string)
   } // namespace StringUtils
} // namespace gpstk
