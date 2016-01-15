#include "LNavCrossSourceFilter.hpp"

namespace gpstk
{
   LNavCrossSourceFilter ::
   LNavCrossSourceFilter()
   {
   }

   void LNavCrossSourceFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
         /** @todo figure out the best way to handle the situation
          * where the HOW is corrupt and as such the time stamp is
          * likely also wrong. */
   }
}
