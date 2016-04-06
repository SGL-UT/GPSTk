#ifndef NAVFILTER_HPP
#define NAVFILTER_HPP

#include <iterator>
#include <stdint.h>
#include <list>
#include <ObsID.hpp>
#include <NavFilterKey.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Base class for all navigation message filters used by NavFilterMgr.
       */
   class NavFilter
   {
   public:
      typedef std::list<NavFilterKey*> NavMsgList;

      NavFilter();

         /** Validate/filter navigation messages.
          * This method is called by NavFilterMgr.  Messages are
          * ingested and filtered results returned, but not
          * necessarily all in a single call to this method. Multiple
          * calls may be required to accumulate sufficient data to
          * give meaningful results.  This behavior is
          * filter-specific.
          * @param[in,out] msgBitsIn A list of NavFilterKey* objects
          *   containing navigation messages.
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter.  The data from msgBitsIn will not appear
          *   here until it successfully passes the filter, which may
          *   require multiple calls of validate with multiple epochs
          *   of data. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut) = 0;

         /** Flush the filter's stored data, if any.  This method is
          * called by NavFilterMgr::finalize() which is in turn called
          * by the user.  This method allows any lingering data stored
          * internally by filters to be output.
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter. */
      virtual void finalize(NavMsgList& msgBitsOut) = 0;

         /// Add a validated nav msg to the output list.
      inline void accept(NavFilterKey* data, NavMsgList& msgBitsOut);

         /// Add a list of validated nav msg to the output list.
      inline void accept(const NavMsgList& valid, NavMsgList& msgBitsOut);

         /// Add an invalid nav message to the reject list.
      inline void reject(NavFilterKey* data);

         /// Add a list of invalid nav messages to the reject list.
      inline void reject(const NavMsgList& invalid);

         /** Rejected nav messages go here.  If using NavFilterMgr,
          * this list will be cleared prior to the validate message
          * being called (to prevent memory bloat).
          *
          * Filter users may want to examine the contents of this list
          * after each NavFilterMgr::validate() call.
          *
          * @warning If you are NOT using NavFilterMgr, be aware that
          *   you will need to manage the rejected list yourself to
          *   avoid it growing unbounded. */
      NavMsgList rejected;
   };

      //@}

   void NavFilter ::
   accept(NavFilterKey* data, NavMsgList& msgBitsOut)
   {
      msgBitsOut.push_back(data);
   }

   void NavFilter ::
   accept(const NavMsgList& valid, NavMsgList& msgBitsOut)
   {
      std::copy(valid.begin(), valid.end(),
                std::back_insert_iterator<NavMsgList>(msgBitsOut));
   }

   void NavFilter ::
   reject(NavFilterKey* data)
   {
      rejected.push_back(data);
   }

   void NavFilter ::
   reject(const NavMsgList& invalid)
   {
      std::copy(invalid.begin(), invalid.end(),
                std::back_insert_iterator<NavMsgList>(rejected));
   }

} // namepace gpstk

#endif // NAVFILTER_HPP
