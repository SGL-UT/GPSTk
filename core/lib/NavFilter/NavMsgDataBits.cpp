#include <NavMsgDataBits.hpp>

namespace gpstk
{
   uint32_t NavMsgDataBits ::
   getBits(unsigned start, unsigned num) const
   {
      uint32_t rv = 0;
      for (unsigned i = start-1; i < (start+num-1); i++)
      {
         rv <<= 1;
         rv |= sf[i];
      }
      return rv;
   }


   void NavMsgDataBits ::
   dump(std::ostream& s, unsigned totalBits) const
   {
      for (unsigned i = 0; i < totalBits; i++)
         s << (int)sf[i];
   }
} // namespace gpstk
