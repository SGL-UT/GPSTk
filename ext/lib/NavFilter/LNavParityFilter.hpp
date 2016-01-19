#ifndef LNAVPARITYFILTER_HPP
#define LNAVPARITYFILTER_HPP

#include <NavFilter.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS legacy nav messages that fail parity checks.
       * Nav message bits are assumed to be upright. */
   class LNavParityFilter : public NavFilter
   {
   public:
      LNavParityFilter();

      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);
         /// Validation is immediate
      virtual unsigned waitLength() const
      { return 1; }
   };

      //@}

}

#endif // LNAVPARITYFILTER_HPP
