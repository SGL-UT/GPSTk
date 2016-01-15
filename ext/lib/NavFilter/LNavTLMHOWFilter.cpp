#include "LNavTLMHOWFilter.hpp"
#include "LNavFilterData.hpp"

namespace gpstk
{
   LNavTLMHOWFilter ::
   LNavTLMHOWFilter()
   {
   }

   void LNavTLMHOWFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator i;
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*i);
         uint32_t sfid = ((fd->sf[1] >> 8) & 0x07);
         bool valid =
               // check TLM preamble
            ((fd->sf[0] & 0x3fc00000) == 0x22c00000) &&
               // zero parity - 2 LSBs
            ((fd->sf[1] & 0x03) == 0) &&
               // < 604800 sow or < 100800 TOW counts
            (((fd->sf[1] >> 13) & 0x1ffff) < 100800) &&
               // subframe ID
            (sfid >= 1) && (sfid <= 5);
         if (valid)
            accept(fd, msgBitsOut);
         else
            reject(fd);
      }
   }
}
