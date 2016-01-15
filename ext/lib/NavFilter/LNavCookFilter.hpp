#ifndef LNAVCOOKFILTER_HPP
#define LNAVCOOKFILTER_HPP

#include <NavFilter.hpp>
#include <LNavFilterData.hpp>

namespace gpstk
{
      /// "Cook" GPS legacy nav messages by turning words upright.
   class LNavCookFilter : public NavFilter
   {
   public:
      LNavCookFilter();

         /** @todo Implement this.  I thought the cook function was in
          * the gpstk but it apparently is not. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);
      virtual unsigned waitLength() const
      { return 1; }

      static void cookSubframe(LNavFilterData* fd);
   };
}

#endif // LNAVCOOKFILTER_HPP
