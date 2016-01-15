#ifndef NAVFILTERMGR_HPP
#define NAVFILTERMGR_HPP

#include <set>
#include <NavFilter.hpp>

namespace gpstk
{
   class NavFilterMgr
   {
   public:
      typedef std::set<NavFilter*,NavFilterSort> FilterSet;

      NavFilterMgr();

      void addFilter(NavFilter* filt);
      NavFilter::NavMsgList validate(NavFilterKey* msgBits);

   private:
      FilterSet filters;
   };
}

#endif // NAVFILTERMGR_HPP
