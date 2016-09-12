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
       * Input data is assumed to be upright.
       *
       * @attention Processing depth = 1 epoch. */
   class LNavTLMHOWFilter : public NavFilter
   {
   public:
      LNavTLMHOWFilter();

         /** Check the TLM and HOW of GPS legacy nav messages
          * (i.e. data fields common to all subframes).
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

#endif // LNAVTLMHOWFILTER_HPP
