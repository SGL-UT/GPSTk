#ifndef LNAVFILTERDATA_HPP
#define LNAVFILTERDATA_HPP

#include <stdint.h>
#include <NavFilterKey.hpp>

namespace gpstk
{
      /// Class for filtering GPS legacy nav (id 2) data
   class LNavFilterData : public NavFilterKey
   {
   public:
      LNavFilterData();
         /** Must be set before use in a filter.  The data is expected
          * to be an array of 10 uint32_t values where each array
          * index corresponds to a subframe word (sf[0] is word 1, and
          * so on).  The words are right-aligned meaning that the LSB
          * of word 1 is in the LSB of sf[0].
          *
          * This is stored as a pointer rather than array so that data
          * need not be moved in order to perform the filtering. */
      uint32_t *sf;
   };
}

#endif // LNAVFILTERDATA_HPP
