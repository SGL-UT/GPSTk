#ifndef LNAVEMPTYFILTER_HPP
#define LNAVEMPTYFILTER_HPP

#include <NavFilter.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS nav subframes with empty contents (either all
       * zeroes, or the standard IS-GPS-200 blank almanac subframe bit
       * pattern).
       *
       * @attention Processing depth = 1 epoch. */
   class LNavEmptyFilter : public NavFilter
   {
   public:
      LNavEmptyFilter();

         /** Filter subframes in msgBitsIn that are empty.
          * @pre LNavFilterData::sf is set
          * @param[in,out] msgBitsIn A list of LNavFilterData* objects
          *   containing GPS legacy navigation messages (id 2).
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);

         /// Filter stores no data, therefore this does nothing.
      virtual void finalize(NavMsgList& msgBitsOut)
      {}
   };

      //@}

}

#endif // LNAVEMPTYFILTER_HPP
