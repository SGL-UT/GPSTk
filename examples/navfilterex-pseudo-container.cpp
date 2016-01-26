/** @file navfilterex-pseudo-container.cpp Pseudo-code for a
 * NavFilterMgr example where the application data is stored in a
 * container.  Refer to navfilterex.cpp for a functional example. */

/// application's navigation message storage class
class NavSubframe
{
public:
      /** The Navigation Subframe. 10 4-byte words.  There are 11
       * elements to facilitate access to elements 1-10. */
   std::vector<uint32_t> subframe;
};

/// use pointers because performance
typedef std::list<NavSubframe*> NavSubframeList;

/// function to store a single epoch's worth of NavSubframe data
void readNavEpoch(NavSubframeList& nsl)
{
   NavSubframe *nsf;
   while (sameEpoch)
   {
      nsf = new NavSubframe();
      strm >> *nsf;
      if (strm)
         nsl.push_back(nsf);
   }
}

void filterEX()
{
      // Filter manager, where the work is done
   gpstk::NavFilterMgr mgr;
      // Individual filters being applied
   gpstk::LNavCookFilter filtCook;
   gpstk::LNavParityFilter filtParity;
      // Data being passed to the filter
   gpstk::LNavFilterData navFiltData;
   gpstk::NavFilter::NavMsgList::const_iterator nmli;
      // application's nav storage
   NavSubframeList nsl;
   NavSubframeList::iterator nsli;

      // Tell the manager what filters to use
   mgr.addFilter(&filtCook);
   mgr.addFilter(&filtParity);

   while (strm)
   {
      readNavEpoch(nsl);

         // process each subframe read from the input stream
      for (nsli = nsl.begin(); nsli != nsl.end(); nsli++)
      {
         NavSubframe *nsf = *nsli;
            // Point the filter data to the LNAV message
         navFiltData.sf = &nsf->subframe[1];

            // validate the subframe
         gpstk::NavFilter::NavMsgList l = mgr.validate(&navFiltData);

            // process the results
         for (nmli = l.begin(); nmli != l.end(); nmli++)
         {
            gpstk::LNavFilterData *fd =
               dynamic_cast<gpstk::LNavFilterData*>(*nmli);
               // do something with fd (nav message that passed the filters)
         }
            // Because the filters are depth 1, we can immediately
            // free the application data storage.  We set the value in
            // the list to NULL as "good practice".  Erase the list at
            // the end so that we don't end up doing any weird
            // unnecessary memory management internal to the list
            // implementation.
         delete nsf;
         *nsli = NULL;
      }
      nsl.clear();
   }

      // Finalize the filters.  Probably not necessary when using only
      // depth 1 filters, but good practice in any case.
   gpstk::NavFilter::NavMsgList l = mgr.finalize();

      // process the results
   for (nmli = l.begin(); nmli != l.end(); nmli++)
   {
      gpstk::LNavFilterData *fd =
         dynamic_cast<gpstk::LNavFilterData*>(*nmli);
         // do something with fd (nav message that passed the filters)
   }
}
