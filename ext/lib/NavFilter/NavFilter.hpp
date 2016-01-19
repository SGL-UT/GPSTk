#ifndef NAVFILTER_HPP
#define NAVFILTER_HPP

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
          * @param[in,out] msgBitsIn A list of NavFilterData* objects
          *   containing navigation messages.
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter.  The data from msgBitsIn will not appear
          *   here until it successfully passes the filter, which may
          *   require multiple calls of validate with multiple epochs
          *   of data. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut) = 0;

         /** Returns the number of epochs worth of data (it
          * is assumed the navigation data being compared is of a
          * constant cadence) that the child class must accumulate
          * before a determination of validity can be made.
          *
          * A return value of 1 means that the filter will immediately
          * return valid messages.  2 means that two successive epochs
          * must be accumulated before returning any valid data, and
          * so on. */
      virtual unsigned waitLength() const = 0;

      bool operator==(const NavFilter& r) const throw()
      { return waitLength() == r.waitLength(); }

      bool operator<(const NavFilter& r) const throw()
      { return waitLength() < r.waitLength(); }

         /// Add a validated nav msg to the output list.
      inline void accept(NavFilterKey* data, NavMsgList& msgBitsOut)
      { msgBitsOut.push_back(data); }

         /// Add an invalid nav message to the reject list.
      void reject(NavFilterKey* data);

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


      /// Sort NavFilter pointers by contents rather than pointer value.
   struct NavFilterSort : std::binary_function<NavFilter*,NavFilter*,bool>
   {
      bool operator()(const NavFilter*const& l, const NavFilter*const& r) const
      { return l->operator<(*r); }
   };

      //@}

} // namepace gpstk

#endif // NAVFILTER_HPP
