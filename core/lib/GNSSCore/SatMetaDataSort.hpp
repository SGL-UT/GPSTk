#ifndef GPSTK_SATMETADATASORT_HPP
#define GPSTK_SATMETADATASORT_HPP

#include <functional>
#include "SatMetaData.hpp"

namespace gpstk
{
      /** Provide a sorting algorithm for SatMetaData to be used by
       * SatMetaDataStore and derivatives to minimize the amount of
       * time spent looking for data.
       * @note This sort algorithm assumes that it is sorting
       *   satellites of the same system, which is how they're stored
       *   in SatMetaDataStore. */
   struct SatMetaDataSort
   {
      bool operator()(const SatMetaData& lhs, const SatMetaData& rhs) const
      {
         if (lhs.prn < rhs.prn)
            return true;
         if (lhs.prn > rhs.prn)
            return false;
         if (lhs.chl < rhs.chl)
            return true;
         if (lhs.chl > rhs.chl)
            return false;
         if (lhs.slotID < rhs.slotID)
            return true;
         if (lhs.slotID > rhs.slotID)
            return false;
         if (lhs.startTime < rhs.startTime)
            return true;
         return false;
      }
   };

} // namespace gpstk

#endif // GPSTK_SATMETADATASORT_HPP
