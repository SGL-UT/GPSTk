#ifndef GPSTK_CARRIERBAND_HPP
#define GPSTK_CARRIERBAND_HPP

#include <string>
#include "EnumIterator.hpp"

namespace gpstk
{
   enum class CarrierBand
   {
      Unknown,   ///< Uninitialized value
      Any,       ///< Used to match any carrier band
      L1,        ///< GPS L1, Galileo E1, SBAS L1, QZSS L1, BeiDou L1
      L2,        ///< GPS L2, QZSS L2
      L5,        ///< GPS L5, Galileo E5a, SBAS L5, QZSS L5, BeiDou B2a, NavIC L5
      G1,        ///< GLONASS G1
      G1a,       ///< GLONASS G1a
      G2a,       ///< GLONASS G2a
      G2,        ///< GLONASS G2
      G3,        ///< GLONASS G3
      E5b,       ///< Galileo E5b
      E5ab,      ///< Galileo E5, BeiDou B2
      E6,        ///< Galileo E6, QZSS L6
      B1,        ///< BeiDou B1
      B2,        ///< BeiDou B2b
      B3,        ///< BeiDou B3
      I9,        ///< NavIC S
      L1L2,      ///< GPS L1+L2
      Undefined, ///< Code is known to be undefined (as opposed to unknown)
      Last,      ///< Used to verify that all items are described at compile time
   };

      /** Define an iterator so C++11 can do things like
       * for (CarrierBand cb : CarrierBandIterator()) */
   typedef EnumIterator<CarrierBand, CarrierBand::Unknown, CarrierBand::Last> CarrierBandIterator;

   namespace StringUtils
   {
         /// Convert a CarrierBand to a whitespace-free string name.
      std::string asString(CarrierBand e) throw();
         /// Convert a string name to an CarrierBand
      CarrierBand asCarrierBand(const std::string& s) throw();
   }

} // namespace gpstk

#endif // GPSTK_CARRIERBAND_HPP
