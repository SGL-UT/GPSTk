#include "NavFilter.hpp"


namespace gpstk
{
   NavFilter ::
   NavFilter()
   {
   }

   void NavFilter::
   dumpRejected(std::ostream& out) const
   {
      NavMsgList::const_iterator cit;
      for (cit=rejected.begin();cit!=rejected.end();cit++)
      {
         NavFilterKey* p = *cit;
         out << *p << std::endl;
      }
   }

} // namespace gpstk
