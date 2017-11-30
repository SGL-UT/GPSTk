#ifndef NAVORDERFILTER_HPP
#define NAVORDERFILTER_HPP

#include <NavFilterMgr.hpp>
#include <NavFilter.hpp>
#include <NavFilterKey.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{


      /** Sort NavFilterKey pointers by time and pointer.  Using
       * pointer as a sort key is the quickest and easiest way to make
       * sure that unique objects remain unique when stored in a
       * set. */
   struct NavTimeSort
      : std::binary_function<NavFilterKey*,NavFilterKey*,bool>
   {
      inline bool operator()(const NavFilterKey*const& l,
                             const NavFilterKey*const& r)
         const;
   };

      /** Order navigation message subframes by the timestamp in the
       * NavFilterKey class.  The intent is to use this when you have
       * (near) real time navigation data arriving from multiple sites
       * where the time of the data is not guaranteed to be completely
       * synchronous, but the time needs to be synchronous.
       *
       * The processing depth of this filter is configurable, but
       * defaults to 3 epochs.  The processing depth should be set
       * according to how many epochs of navigation data (i.e. unique,
       * consecutive time stamps) to collect before passing it along.
       * The default of 3 is recommended to be the minimum as anything
       * less would eliminate the function of this class by preventing
       * enough accumulation of data to occur to fix an order, however
       * if 2 is used this will behave like a filter to discard "old"
       * navigation data.
       *
       * Accepted messages are guaranteed to be in time order.
       * Rejected messages are more than processingDepth epochs older
       * than the most recent.
       *
       * As data is added to the filter via the validate method, data
       * older than, or the same age as procDepth epochs is accepted.
       * This allows data to be accepted in the case where the time
       * stamp jumps by 2 or more epochs. */
   class NavOrderFilter : public NavFilter
   {
   public:
         /** This defaults to 6 seconds, which is the LNAV subframe
          * period.  It should be changed accordingly for other
          * navigation message formats, either via the constructor or
          * directly. */
      unsigned epochInSeconds;

         /// Initialize internal structures and set the processing depth.
      NavOrderFilter(unsigned depth = 3, unsigned epoch = 6);

         /** Add nav messages to the collection (orderedNav).
          * @pre NavFilterKey::timeStamp is set to an appropriate,
          *   consistent value for the navigation data being filtered.
          * @param[in,out] msgBitsIn A list of NavFilterKey* objects
          *   containing navigation messages.
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);

         /** Flush the remaining contents of orderedNav.
          * @param[out] msgBitsOut Any remaining nav messages are
          *   stored here on return. */
      virtual void finalize(NavMsgList& msgBitsOut);

         /// Internal storage includes a user-specified number of epochs.
      virtual unsigned processingDepth() const throw()
      { return procDepth; }

         /// Return the filter name.
      virtual std::string filterName() const throw()
      { return "Order"; }

      unsigned procDepth;

   protected:
      typedef std::set<NavFilterKey*, NavTimeSort> SubframeSet;

         /// Ordered set of nav message subframes
      SubframeSet orderedNav;

         /** Filter by age.
          * @param[out] msgBitsOut Nav messages passing age filter
          *   algorithm are stored here. */
      void examineSubframes(NavMsgList& msgBitsOut);
   };

      //@}


   bool NavTimeSort ::
   operator()(const NavFilterKey*const& l, const NavFilterKey*const& r)
      const
   {
      if (l->timeStamp < r->timeStamp)
         return true;
      if (l->timeStamp > r->timeStamp)
         return false;
      return (l < r);
   }

} // namespace gpstk

#endif // LNAVORDERFILTER_HPP
