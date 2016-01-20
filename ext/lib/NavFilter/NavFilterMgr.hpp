#ifndef NAVFILTERMGR_HPP
#define NAVFILTERMGR_HPP

#include <list>
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
       * @see navfilterex.cpp for an example use.
       *
       * @section GPSLNAV GPS Legacy Nav Filters
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

         /// Do-nothing default constructor.
      NavFilterMgr();

         /** Add a desired navigation message data filter to the list.
          * Filters are expected to be for the same type of navigation
          * message data, but this is not enforced.
          * @param[in] filt The filter to be added. */
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

   private:
         /// The collection of navigation message filters to apply.
      FilterList filters;
   };

      //@}
}

#endif // NAVFILTERMGR_HPP
