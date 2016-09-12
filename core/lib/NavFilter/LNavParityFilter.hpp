#ifndef LNAVPARITYFILTER_HPP
#define LNAVPARITYFILTER_HPP

#include <NavFilter.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS legacy nav messages that fail parity checks.
       * Nav message bits are assumed to be upright.
       *
       * @attention Processing depth = 1 epoch. */
   class LNavParityFilter : public NavFilter
   {
   public:
      LNavParityFilter();

         /** Check the parity of the nav subframes (per IS-GPS-200).
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

#endif // LNAVPARITYFILTER_HPP
