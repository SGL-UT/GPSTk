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
       *
       * @attention Processing depth = 1 epoch. */
   class LNavCookFilter : public NavFilter
   {
   public:
      LNavCookFilter();

         /** Turn words in a GPS LNAV message upright.  This results
          * in the expected 0x8b preamble in the TLM and subsequent
          * bits as expected per the IS-GPS-200.
          * @pre LNavFilterData::sf is set
          * @param[in,out] msgBitsIn a list of LNavFilterData* objects
          *   containing GPS LNAV data.
          * @param[out] msgBitsOut the "cooked" subframes.  All
          *   contents of msgBitsIn will be immediately seen in
          *   msgBitsOut. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);

         /// Filter stores no data, therefore this does nothing.
      virtual void finalize(NavMsgList& msgBitsOut)
      {}

         /// No internal storage of subframe data so return 0.
      virtual unsigned processingDepth() const throw()
      { return 0; }

         /// Turn an LNAV subframe data upright.
      static void cookSubframe(LNavFilterData* fd);

         /// Return the filter name.
      virtual std::string filterName() const throw()
      { return "Cook"; }
   };

      //@}
}

#endif // LNAVCOOKFILTER_HPP
