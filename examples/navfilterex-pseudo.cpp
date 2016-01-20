/** @file navfilterex-pseudo.cpp Pseudo-code for a NavFilterMgr
 * example.  Refer to navfilterex.cpp for a functional example. */

void filterEX()
{
      // Filter manager, where the work is done
   NavFilterMgr mgr;
      // Individual filters being applied
   LNavCookFilter filtCook;
   LNavParityFilter filtParity;
      // Data being passed to the filter
   LNavFilterData navFiltData;
      // Generic LNAV message storage
   std::vector<uint32_t> subframe(10,0);

      // Tell the manager what filters to use
   mgr.addFilter(&filtCook);
   mgr.addFilter(&filtParity);

      // Point the filter data to the LNAV message
   navFiltData.sf = &subframe[0];

      // validate the subframe
   NavFilter::NavMsgList l = mgr.validate(&navFiltData);

      // process the results
   NavFilter::NavMsgList::const_iterator nmli;
   for (nmli = l.begin(); nmli != l.end(); nmli++)
   {
      LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
         // do something with fd (nav message that passed the filters)
   }
}
