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
      rv.push_back(msgBits);
      rejected.clear();
      for (FilterList::iterator i = filters.begin(); i != filters.end(); i++)
      {
         if (rv.empty())
            break;
         (*i)->rejected.clear();
         newrv.clear();
         (*i)->validate(rv, newrv);
         if (!(*i)->rejected.empty())
            rejected.insert(*i);
         rv = newrv;
      }
      return rv;
   }


   NavFilter::NavMsgList NavFilterMgr ::
   finalize()
   {
         // final and intermediate return values
      NavFilter::NavMsgList rv, rv1, rv2;
         // current and next filter
      FilterList::iterator fliCur, fliNxt;
      rejected.clear();
         // touch ALL filters
      for (fliCur = filters.begin(); fliCur != filters.end(); fliCur++)
      {
            // finalize the data in the current filter
         (*fliCur)->rejected.clear();
         rv2.clear();
         (*fliCur)->finalize(rv2);
         
            // If the filter returned some data, we need to push it
            // into the next filter using validate.
         if (!rv2.empty())
         {
            fliNxt = fliCur;
            fliNxt++;
               // cascade the data through the end.
            rv1 = rv2;
            while ((fliNxt != filters.end()) && !rv1.empty())
            {
               (*fliNxt)->rejected.clear();
               rv2.clear();
               (*fliNxt)->validate(rv1, rv2);
               rv1 = rv2;
               fliNxt++;
            }
               // If the filter cascade got some data that passed all
               // filters, add it to the final return value.
            if (!rv1.empty())
            {
               std::copy(rv1.begin(), rv1.end(),
                         std::back_insert_iterator<NavFilter::NavMsgList>(rv));
            }
         }
      }
      return rv;
   }
}
