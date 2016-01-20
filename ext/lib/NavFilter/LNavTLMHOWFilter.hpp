#ifndef LNAVTLMHOWFILTER_HPP
#define LNAVTLMHOWFILTER_HPP

#include <NavFilter.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS nav subframes with
       * 1. a bad preamble
       * 2. an invalid TOW count
       * 3. an invalid subframe ID, or
       * 4. non-zero parity bits in the HOW bits 29-30.
       * Input data is assumed to be upright. */
   class LNavTLMHOWFilter : public NavFilter
   {
   public:
      LNavTLMHOWFilter();

      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);
         /// Immediate validation
      virtual unsigned waitLength() const
      { return 1; }
   };

      //@}

}

#endif // LNAVTLMHOWFILTER_HPP
