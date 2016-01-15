#include "NavFilterMgr.hpp"

namespace gpstk
{
   NavFilterMgr ::
   NavFilterMgr()
   {
   }


   void NavFilterMgr ::
   addFilter(NavFilter* filt)
   {
      filters.insert(filt);
   }


   NavFilter::NavMsgList NavFilterMgr ::
   validate(NavFilterKey* msgBits)
   {
      NavFilter::NavMsgList rv, newrv;
      NavFilter::NavMsgList::iterator j;
      rv.push_back(msgBits);
      for (FilterSet::iterator i = filters.begin(); i != filters.end(); i++)
      {
         if (rv.empty())
            break;
         (*i)->rejected.clear();
         (*i)->validate(rv, newrv);
         rv = newrv;
      }
      return rv;
   }
}
