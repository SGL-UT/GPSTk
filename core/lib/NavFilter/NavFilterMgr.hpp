#ifndef NAVFILTERMGR_HPP
#define NAVFILTERMGR_HPP

#include <list>
#include <set>
#include <NavFilter.hpp>

namespace gpstk
{
      /** @defgroup NavFilter Navigation Data Filtering
       *
       * The NavFilter classes provide a framework for applying
       * filters to navigation data.  The primary purpose is to
       * perform validity checks on raw navigation data and remove
       * suspect data from a data stream.
       *
       * The filter is implemented using a cascading filter approach,
       * meaning that the data output by one filter becomes the input
       * for the next.  The order and choice of filters is entirely up
       * to the developer.
       *
       * To use the filter, the developer will instantiate a
       * NavFilterMgr object, one per navigation structure type
       * (e.g. one for GPS LNAV, one for GPS CNAV, and so on).
       * Filters for that particular navigation message structure are
       * instantiated and added to the manager using
       * NavFilterMgr::addFilter().  Data is processed and returned
       * using NavFilterMgr::validate().
       *
       * Data is added to the NavFilterMgr using child classes of
       * NavFilterKey.  These child classes will have data members
       * pointing to pre-existing storage of the navigation messages
       * themselves.  Using pointers to pre-existing storage allows
       * the algorithm to operate quickly where it would otherwise
       * have to make copies of blocks of memory.  Most filters will
       * not change the contents of the navigation message data, but
       * that is not guaranteed and in fact some filters are intended
       * to do just that (e.g. LNavCookFilter).
       *
       * @see NavFilterMgr for a list of examples.
       *
       * @section NavFilterStore Storing NavFilterKey Data
       *
       * For performance reasons, the filter data is passed around
       * using pointers rather than copying the messages themselves.
       * The down side to this approach is that the user is
       * subsequently required to manage the memory used to store the
       * data being filtered.  There are a number of ways to do this,
       * including:
       *
       *   1. Keeping the data in a container of the application's
       *      data structure of choice, then discarding the data when
       *      appropriate.  This approach is most appropriate when
       *      using filters of depth 1.
       *   2. Creating new NavFilterKey classes that inherit from the
       *      filter data classes appropriate to the nav messages
       *      being process (e.g. LNavFilterData).  The data in the
       *      application's data structure of choice could be stored
       *      as a new field in this child class or inheriting from
       *      both the NavFilter and data structure.  This has the
       *      additional advantage that the application's data can be
       *      created dynamically on the heap and freed as the data is
       *      either rejected or accepted.  This approach is most
       *      useful when using filters of depth 2 and larger.
       *
       * @see NavFilterMgr for a list of examples.
       *
       * @section GPSLNAV GPS Legacy Nav Filters
       *
       * Filters in this group use the data class LNavFilterData,
       * which contains a pointer to an array of 10 uint32_t values in
       * host byte order.  Each of the 10 values contains one subframe
       * word each, with the 30 bits of the word in the 30 LSBs of the
       * 32-bit value.
       *
       * | Class                 | Filter Depth | Modifies Msg |
       * | :-------------------- | -----------: | :----------- |
       * | LNavFilterData        |          n/a | no           |
       * | LNavAlmValFilter      |            1 | no           |
       * | LNavCookFilter        |            1 | yes          |
       * | LNavCrossSourceFilter |            2 | no           |
       * | LNavEmptyFilter       |            1 | no           |
       * | LNavParityFilter      |            1 | no           |
       * | LNavTLMHOWFilter      |            1 | no           |
       */

      /// @ingroup NavFilter
      //@{

      /** @example navfilterex.cpp
       * Example of how to use NavFilterMgr on GPS LNAV data.
       */

      /** @example navfilterex-pseudo.cpp
       * Pseudo-code example (minimal extra code) of how to use
       * NavFilterMgr on GPS LNAV data.
       */

      /** @example navfilterex-pseudo-container.cpp
       * Pseudo-code example of using a container of application data
       * structures for storing the navigation messages processed by
       * the filter.
       */

      /** @example navfilterex-pseudo-inherit.cpp
       * Pseudo-code example of using inheritance for the filter data
       * for storing the navigation messages processed by the filter.
       */

      /** Provides the top-level filtering for navigation messages.  A
       * single instance of this class should be created for each
       * navigation message structure to be processed.  Desired
       * filters are added via addFilter() in the desired order of
       * precedence.  Navigation messages are validated using the
       * validate() method.
       */
   class NavFilterMgr
   {
   public:
         /// A list of navigation data filters.
      typedef std::list<NavFilter*> FilterList;
         /// A set of unique filter pointers.
      typedef std::set<NavFilter*> FilterSet;

         /// Do-nothing default constructor.
      NavFilterMgr();

         /** Add a desired navigation message data filter to the list.
          * Filters are expected to be for the same type of navigation
          * message data, but this is not enforced.
          * @param[in] filt The filter to be added.
          *
          * @warning Do not add the exact same filter object more than
          *   once.  Multiple instances of the same filter class may
          *   be added, but if the same instance is added more than
          *   once, memory allocated for rejected subframes will
          *   likely be leaked. */
      void addFilter(NavFilter* filt);

         /** Validate a single navigation message.
          * @param[in] msgBits The navigation message to
          *   validate/filter.  This should not be a NavFilterKey
          *   pointer but rather should be a child class specific to
          *   the type of navigation message data being processed.
          *   Behavior is undefined if msgBits is not the expected
          *   type.
          * @return Any messages that have successfully passed all
          *   configured filters. */
      NavFilter::NavMsgList validate(NavFilterKey* msgBits);

         /** Flush the stored data for all known filters.  This method
          * should be called by the user after all data has been added
          * to the filter manager via validate().
          * @return The remaining messages successfully passing the
          *   filters. */
      virtual NavFilter::NavMsgList finalize();

         /** This set contains any filters with rejected data after a
          * validate() or finalize() call.  The set will be cleared at
          * the beginning of the validate() or finalize() call so that
          * only filters with rejected data from the most recent call
          * will be present in the set.  The rejected data itself can
          * be accessed via the NavFilter::rejected data member. */
      FilterSet rejected;

   private:
         /// The collection of navigation message filters to apply.
      FilterList filters;
   };

      //@}
}

#endif // NAVFILTERMGR_HPP
