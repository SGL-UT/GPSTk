#include "NavFilter.hpp"


namespace gpstk
{
   NavFilter ::
   NavFilter()
   {
   }

   void NavFilter ::
   reject(NavFilterKey* data)
   {
         /// @todo Consider adding some information about why the data
         /// was rejected.
      rejected.push_back(data);
   }

} // namespace gpstk
