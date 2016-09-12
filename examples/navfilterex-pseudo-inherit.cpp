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

/// Combine the nav filter store and the application store into a single class
class MyNavFilterData : public gpstk::LNavFilterData
{
public:
   NavSubframe appData;
};

/// function to read a single nav subframe
MyNavFilterData* readNav()
{
   if (!strm)
      return NULL;
   MyNavFilterData *rv = new MyNavFilterData();
   strm >> rv->appData;
   retun rv;
}

void filterEX()
{
      // Filter manager, where the work is done
   gpstk::NavFilterMgr mgr;
      // Individual filters being applied
   gpstk::LNavCookFilter filtCook;
   gpstk::LNavCrossSourceFilter filtVote;
   gpstk::NavFilter::NavMsgList::const_iterator nmli;

      // Tell the manager what filters to use
   mgr.addFilter(&filtCook);
   mgr.addFilter(&filtVote);

   while (strm)
   {
      MyNavFilterData *fd = readNav();

         // validate the subframe
      gpstk::NavFilter::NavMsgList l = mgr.validate(fd);

         // process the results
      for (nmli = l.begin(); nmli != l.end(); nmli++)
      {
         MyNavFilterData *ofd = dynamic_cast<MyNavFilterData*>(*nmli);
            // do something with fd (nav message that passed the filters)

            // Then free the memory.  Note that this would also free
            // the application's data, so you may want to delay this
            // depending on what you're trying to do.
         delete ofd;
      }
   }

      // Finalize the filters.  Probably not necessary when using only
      // depth 1 filters, but good practice in any case.
   gpstk::NavFilter::NavMsgList l = mgr.finalize();

      // process the results
   for (nmli = l.begin(); nmli != l.end(); nmli++)
   {
      MyNavFilterData *ofd = dynamic_cast<MyNavFilterData*>(*nmli);
         // do something with fd (nav message that passed the filters)

         // Then free the memory.  Note that this would also free
         // the application's data, so you may want to delay this
         // depending on what you're trying to do.
      delete ofd;
   }
}
