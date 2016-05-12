#include "LNavParityFilter.hpp"
#include "LNavFilterData.hpp"
#include "EngNav.hpp"

namespace gpstk
{
   LNavParityFilter ::
   LNavParityFilter()
   {
   }


   void LNavParityFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator i;
         // check parity of each subframe and put the valid ones in
         // the output
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*i);
         if (EngNav::checkParity(fd->sf))
            accept(*i, msgBitsOut);
         else
            reject(*i);
      }
   }
}
