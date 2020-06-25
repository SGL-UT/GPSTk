#ifndef GPSTK_SATELLITESYSTEM_HPP
#define GPSTK_SATELLITESYSTEM_HPP

#include <string>

namespace gpstk
{
      /** Supported satellite systems
       * @note any additions to this enumeration list should also
       *   be added to convertSatelliteSystemToString() and
       *   convertStringToSatelliteSystem() and to SatID_T. */
#if !defined(SWIG) || SWIG_VERSION >= 0x030000 
   enum class SatelliteSystem
#else
   enum SatelliteSystem
#endif
   {
      GPS = 1,
      Galileo,
      Glonass,
      Geosync,
      LEO,
      Transit,
      BeiDou,
      QZSS,
      IRNSS,
      Mixed,
      UserDefined,
      Unknown
   };

      /** Translate system enumeration to its string representation.
       * @note The string representation is being used in file
       *   formats, e.g. RawNavCSVHeader.  The string values should
       *   not be changed if at all possible, as that would break
       *   the ability to read older files.
       * @note Any new systems should not contain spaces in the
       *   string values.
       * @note The translations here should precisely match those
       *   in convertStringToSatelliteSystem.
       * @param[in] s The system to get the string name of.
       * @return A space-free string containing the name of the GNSS.
       */
   inline std::string convertSatelliteSystemToString(SatelliteSystem s);

      /** Translate GNSS names as strings into system enumeration
       * equivalents.
       * @see convertSatelliteSystemToString
       * @param[in] s The GNSS name to convert to enumeration.
       * @return An enumeration equivalent of the given string.
       *   systemUnknown is returned for any names that do not
       *   exactly match known values.
       */
   inline SatelliteSystem convertStringToSatelliteSystem(
      const std::string& s);


   namespace StringUtils
   {
         /// @ingroup StringUtils
         //@{

         /// SatelliteSystem as a string
      inline std::string asString(SatelliteSystem s)
      {
         return convertSatelliteSystemToString(s);
      }

         ///@}
   }


   std::string
   convertSatelliteSystemToString(SatelliteSystem s)
   {
      switch(s)
      {
         case SatelliteSystem::GPS:         return "GPS";           break;
         case SatelliteSystem::Galileo:     return "Galileo";       break;
         case SatelliteSystem::Glonass:     return "GLONASS";       break;
         case SatelliteSystem::Geosync:     return "Geostationary"; break;
         case SatelliteSystem::LEO:         return "LEO";           break;
         case SatelliteSystem::Transit:     return "Transit";       break;
         case SatelliteSystem::BeiDou:      return "BeiDou";        break;
         case SatelliteSystem::QZSS:        return "QZSS";          break;
         case SatelliteSystem::IRNSS:       return "IRNSS";         break;
         case SatelliteSystem::Mixed:       return "Mixed";         break;
         case SatelliteSystem::UserDefined: return "UserDefined";   break;
         case SatelliteSystem::Unknown:     return "Unknown";       break;
         default:                           return "??";            break;
      }
   }

   SatelliteSystem
   convertStringToSatelliteSystem(const std::string& s)
   {
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
   }

} // namespace gpstk

#endif // GPSTK_SATELLITESYSTEM_HPP
