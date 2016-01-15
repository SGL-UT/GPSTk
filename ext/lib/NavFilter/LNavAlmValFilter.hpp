#ifndef LNAVALMVALFILTER_HPP
#define LNAVALMVALFILTER_HPP

#include <NavFilter.hpp>
#include <LNavFilterData.hpp>

namespace gpstk
{
      /// Filter GPS subframes 4 and 5 based on expected value ranges
   class LNavAlmValFilter : public NavFilter
   {
   public:
      LNavAlmValFilter();

         /** For subframes 4 and 5, check the ranges of message data
          * fields.  For anything else, feed it back into
          * msgBitsOut. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);
         /// Validation is immediate
      virtual unsigned waitLength() const
      { return 1; }
         /// Specific value range checks
      static bool checkAlmValRange(LNavFilterData* fd);
   };
}

#endif // LNAVALMVALFILTER_HPP
