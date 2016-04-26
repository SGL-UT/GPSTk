#include "LNavCookFilter.hpp"
#include "EngNav.hpp"

namespace gpstk
{
   LNavCookFilter ::
   LNavCookFilter()
   {
   }


   void LNavCookFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*i);
         cookSubframe(fd);
         msgBitsOut.push_back(fd);
      }
   }


   void LNavCookFilter ::
   cookSubframe(LNavFilterData* fd)
   {
      unsigned word;

         // Invert the first word only if it needs to be.  It appears
         // to always be the case that D30* = D39* for word 10.  Also
         // it appears you need to invert the entire subframe, if word
         // 1 is inverted.
      if ((fd->sf[0] & 0x3fc00000) == 0x1d000000)
      {
         for (word = 0; word < 10; word++)
            fd->sf[word] = fd->sf[word] ^ 0x3fffffff;
      }

         // no need to continue attempting to invert garbage data
      if ((fd->sf[0] & 0x3fc00000) != 0x22c00000)
         return;

      for (word = 1; word < 10; word++)
      {
         if (gpstk::EngNav::getd30(fd->sf[word-1]))
         {
            fd->sf[word] = fd->sf[word] ^ 0x3fffffc0;
         }
      }
   }
}
