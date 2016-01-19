#ifndef LNAVCOOKFILTER_HPP
#define LNAVCOOKFILTER_HPP

#include <NavFilter.hpp>
#include <LNavFilterData.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** "Cook" GPS legacy nav messages by turning words upright.
       * Data in LNavFilterData::sf are modified.
       */
   class LNavCookFilter : public NavFilter
   {
   public:
      LNavCookFilter();

         /** Turn words in a GPS LNAV message upright.  This results
          * in the expected 0x8b preamble in the TLM and subsequent
          * bits as expected per the IS-GPS-200.
          * @param[in,out] msgBitsIn a list of LNavFilterData* objects
          *   containing GPS LNAV data.
          * @param[out] msgBitsOut the "cooked" subframes.  All
          *   contents of msgBitsIn will be immediately seen in
          *   msgBitsOut. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);
         /// No aggregation of data is required, filter is immediate.
      virtual unsigned waitLength() const
      { return 1; }

         /// Turn an LNAV subframe data upright.
      static void cookSubframe(LNavFilterData* fd);
   };

      //@}
}

#endif // LNAVCOOKFILTER_HPP
