#ifndef LNAVFILTERDATA_HPP
#define LNAVFILTERDATA_HPP

#include <stdint.h>
#include <NavFilterKey.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Class for filtering GPS legacy nav (id 2) data.
       */
   class LNavFilterData : public NavFilterKey
   {
   public:
         /// Set data fields to reasonable defaults, i.e. sf to NULL.
      LNavFilterData();
         /** Must be set before use in a filter.  The data is expected
          * to be an array of 10 uint32_t values where each array
          * index corresponds to a subframe word (sf[0] is word 1, and
          * so on).  The words are right-aligned meaning that the LSB
          * of word 1 is in the LSB of sf[0].
          *
          * This is stored as a pointer rather than array so that data
          * need not be moved in order to perform the filtering.
          *
          * @note The data contents pointed to by sf may be modified
          *   by some filters. */
      uint32_t *sf;
   };


      /// Sort LNavFilterData pointers by navigation message bits
   struct LNavMsgSort
      : std::binary_function<LNavFilterData*,LNavFilterData*,bool>
   {
      inline bool operator()(const LNavFilterData*const& l,
                             const LNavFilterData*const& r)
         const;
   };

      //@}


   bool LNavMsgSort ::
   operator()(const LNavFilterData*const& l, const LNavFilterData*const& r)
      const
   {
      for (unsigned sfword = 0; sfword < 10; sfword++)
      {
         if (l->sf[sfword] < r->sf[sfword])
            return true;
         if (l->sf[sfword] > r->sf[sfword])
            return false;
      }
      return false;
   }

}

#endif // LNAVFILTERDATA_HPP
