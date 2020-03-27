#ifndef GPSTK_SATMETADATA_HPP
#define GPSTK_SATMETADATA_HPP

#include <cstdint>
#include "SatID.hpp"
#include "CommonTime.hpp"

namespace gpstk
{
      /** Store information about a GNSS satellite's configuration.
       * @note This class only implements the data storage and does
       *   not implement any I/O.  The I/O is implemented in
       *   SatMetaDataStore, where polymorphism can be used to provide
       *   transparent access to other external storage mechanisms.
       * @note The signals value is expected to correlate to a named
       *   SatSignalsSet object.
       */
   class SatMetaData
   {
   public:
         /// Maximum number of clocks on a satellite.
      static const unsigned NUMCLOCKS = 4;
         /// Enumeration for the satellite usage states.
      enum class Status
      {
         Unknown,
         Operational,
         Decommissioned,
         Test
      };
         /// Enumeration for the various types of frequency standards.
      enum class ClockType
      {
         Unknown,
         Crystal,
         Cesium,
         Rubidium,
         Hydrogen,
         USNO
      };
         /// Initialize data fields to reasonable defaults.
      SatMetaData();

         /// Convert Status to string
      static std::string asString(Status s);
         /// Convert string to Status
      static Status asStatus(const std::string& s);
         /// Convert ClockType to string
      static std::string asString(ClockType s);
         /// Convert string to ClockType
      static ClockType asClockType(const std::string& s);

      uint32_t prn;    ///< Pseudo-Random Number, identifies CDMA satellites.
      std::string svn; ///< Space vehicle number, a unique GNSS satellite ID.
      int32_t norad;   ///< NORAD-assigned ID for this satellite.
      int32_t chl;     ///< Channel ID for FDMA satellites.
      uint32_t slotID; ///< Slot ID for FDMA satellites.
      SatID::SatelliteSystem sys;   ///< Which GNSS this satellite is from.
      gpstk::CommonTime launchTime; ///< When the satellite was launched.
      gpstk::CommonTime startTime;  ///< When the satellite became operational.
      gpstk::CommonTime endTime;    ///< When the satellite ceased operation.
      std::string plane;            ///< Satellite plane identifier.
      std::string slot;             ///< Slot within the plane.
      std::string type;             ///< Typically block number.
      std::string signals;          ///< Name of broadcast signal set.
      std::string mission;          ///< Mission number.
      Status status;                ///< Current satellite state.
      ClockType clocks[NUMCLOCKS];  ///< Types of frequency standards available.
      uint8_t activeClock;          ///< Which of the above 4 clocks is active.
   }; // class SatMetaData

      /** Stream output for SatMetaData
       * @warning not expected to be consistent between releases,
       * don't use for purposes other than debugging. */
   std::ostream& operator<<(std::ostream& s, const SatMetaData& smd);

   inline std::ostream& operator<<(std::ostream& s, SatMetaData::Status status)
   {
      s << SatMetaData::asString(status);
      return s;
   }

   inline std::ostream& operator<<(std::ostream& s, SatMetaData::ClockType ct)
   {
      s << SatMetaData::asString(ct);
      return s;
   }

} // namespace gpstk

#endif // GPSTK_SATMETADATA_HPP
