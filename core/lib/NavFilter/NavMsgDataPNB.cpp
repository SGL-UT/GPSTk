#include <NavMsgDataPNB.hpp>

namespace gpstk
{
   uint32_t NavMsgDataPNB ::
   getBits(unsigned start, unsigned num) const
   {
      return sf->asUnsignedLong(start-1, num, 1);
   }


   void NavMsgDataPNB ::
   dump(std::ostream& s, unsigned totalBits) const
   {
         /** @todo eventually we'll likely want to make this a bit
          * more configurable.  This applies to GPS and BeiDou at the
          * very least, but probably not GLONASS.  Configuration
          * shouldn't involve the use of variables in this class if it
          * can be avoided, since there can be a large number of these
          * objects when processing data.  It would be better to
          * figure out a way to make the nav code-specific derived
          * classes set the configuration somehow via methods or
          * static data or some such. */
      sf->outputPackedBits(s, 1000, ' ', 30);
   }
} // namespace gpstk
