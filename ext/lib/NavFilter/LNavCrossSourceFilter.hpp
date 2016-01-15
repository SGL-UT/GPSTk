#ifndef LNAVCROSSOURCEFILTER_HPP
#define LNAVCROSSOURCEFILTER_HPP

#include <NavFilter.hpp>

namespace gpstk
{
      /** Filter GPS subframes using a voting algorithm across a
       * single epoch.  This may be used for a single receiver where
       * multiple codes can be compared against each other, or across
       * multiple receivers (with or without multiple codes). */
   class LNavCrossSourceFilter : public NavFilter
   {
   public:
      LNavCrossSourceFilter();

      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);
         /// Validation requires successive epochs
      virtual unsigned waitLength() const
      { return 2; }

   protected:
      
   };
}

#endif // LNAVCROSSOURCEFILTER_HPP
