#include "LNavEmptyFilter.hpp"
#include "LNavFilterData.hpp"

namespace gpstk
{
   LNavEmptyFilter ::
   LNavEmptyFilter()
   {
   }

   void LNavEmptyFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*i);
         bool blank =
               // all zeroes
            ((fd->sf[0] == 0) && (fd->sf[1] == 0) && (fd->sf[2] == 0) &&
             (fd->sf[3] == 0) && (fd->sf[4] == 0) && (fd->sf[5] == 0) &&
             (fd->sf[6] == 0) && (fd->sf[7] == 0) && (fd->sf[8] == 0) && 
             (fd->sf[9] == 0)) ||
               // or subframe 4 or 5 with alternating 1s and 0s
            ((((fd->sf[1] & 0x700) == 0x400) ||
              ((fd->sf[1] & 0x700) == 0x500)) &&
             ((fd->sf[2] & 0x3ffc0) == 0x2aa80) &&
             ((fd->sf[3] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[4] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[5] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[6] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[7] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[8] & 0x3ffffc0) == 0x2aaaa80) &&
             ((fd->sf[9] & 0x3ffffc0) == 0x2aaaa80));
         if (blank)
            reject(fd);
         else
            accept(fd, msgBitsOut);
      }
   }
}
