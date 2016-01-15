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
      filters.push_back(filt);
   }


   NavFilter::NavMsgList NavFilterMgr ::
   validate(NavFilterKey* msgBits)
   {
      NavFilter::NavMsgList rv, newrv;
      NavFilter::NavMsgList::iterator j;
      rv.push_back(msgBits);
      for (FilterList::iterator i = filters.begin(); i != filters.end(); i++)
      {
         if (rv.empty())
            break;
         (*i)->rejected.clear();
         newrv.clear();
         (*i)->validate(rv, newrv);
         std::cerr << "NavFilterMgr::validate called " << typeid(*(*i)).name()
                   << std::endl;
         rv = newrv;
      }
      return rv;
   }
}
