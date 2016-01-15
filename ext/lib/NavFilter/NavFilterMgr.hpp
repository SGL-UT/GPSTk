#ifndef NAVFILTERMGR_HPP
#define NAVFILTERMGR_HPP

#include <list>
#include <NavFilter.hpp>

namespace gpstk
{
   class NavFilterMgr
   {
   public:
      typedef std::list<NavFilter*> FilterList;

      NavFilterMgr();

      void addFilter(NavFilter* filt);
      NavFilter::NavMsgList validate(NavFilterKey* msgBits);

   private:
      FilterList filters;
   };
}

#endif // NAVFILTERMGR_HPP
