#ifndef GPSTK_OBSERVATIONTYPE_HPP
#define GPSTK_OBSERVATIONTYPE_HPP

#include <string>
#include "EnumIterator.hpp"

namespace gpstk
{
      /// The type of observation, mostly used by ObsID.
   enum class ObservationType
   {
      Unknown,
      Any,       ///< Used to match any observation type
      Range,     ///< pseudorange, in meters
      Phase,     ///< accumulated phase, in cycles
      Doppler,   ///< Doppler, in Hz
      SNR,       ///< Signal strength, in dB-Hz
      Channel,   ///< Channel number
      DemodStat, ///< Demodulator status
      Iono,      ///< Ionospheric delay (see RINEX3 section 5.12)
      SSI,       ///< Signal Strength Indicator (RINEX)
      LLI,       ///< Loss of Lock Indicator (RINEX)
      TrackLen,  ///< Number of continuous epochs of 'good' tracking
      NavMsg,    ///< Navigation Message data
      RngStdDev, ///< pseudorange standard deviation, in meters
      PhsStdDev, ///< phase standard deviation, in cycles
      FreqIndx,  ///< GLONASS frequency offset index [-6..7]
      Undefined, ///< Undefined
      Last       ///< Used to verify that all items are described at compile time
   };

      /** Define an iterator so C++11 can do things like
       * for (ObservationType ot : ObservationTypeIterator()) */
   typedef EnumIterator<ObservationType, ObservationType::Unknown, ObservationType::Last> ObservationTypeIterator;

   namespace StringUtils
   {
         /// Convert a ObservationType to a whitespace-free string name.
      std::string asString(ObservationType e) throw();
         /// Convert a string name to an ObservationType
      ObservationType asObservationType(const std::string& s) throw();
   } // namespace StringUtils
} // namespace gpstk

#endif // GPSTK_OBSERVATIONTYPE_HPP
