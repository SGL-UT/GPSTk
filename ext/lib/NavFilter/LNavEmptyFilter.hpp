#ifndef LNAVEMPTYFILTER_HPP
#define LNAVEMPTYFILTER_HPP

#include <NavFilter.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS nav subframes with empty contents (either all
       * zeroes, or the standard IS-GPS-200 blank almanac subframe bit
       * pattern). */
   class LNavEmptyFilter : public NavFilter
   {
   public:
      LNavEmptyFilter();

      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);
         /// Immediate validation
      virtual unsigned waitLength() const
      { return 1; }
   };

      //@}

}

#endif // LNAVEMPTYFILTER_HPP
