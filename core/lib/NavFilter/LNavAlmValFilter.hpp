#ifndef LNAVALMVALFILTER_HPP
#define LNAVALMVALFILTER_HPP

#include <NavFilter.hpp>
#include <LNavFilterData.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS subframes 4 and 5 based on expected value ranges.
       *
       * @attention Processing depth = 1 epoch. */
   class LNavAlmValFilter : public NavFilter
   {
   public:
      LNavAlmValFilter();

         /** For subframes 4 and 5, check the ranges of message data
          * fields.  For anything else, feed it back into
          * msgBitsOut.
          * @pre LNavFilterData::sf is set
          * @param[in,out] msgBitsIn A list of LNavFilterData* objects
          *   containing GPS legacy navigation messages (id 2).
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);

         /// Filter stores no data, therefore this does nothing.
      virtual void finalize(NavMsgList& msgBitsOut)
      {}

         /// No internal storage of subframe data so return 0.
      virtual unsigned processingDepth() const throw()
      { return 0; }

         /// Return the filter name.
      virtual std::string filterName() const throw()
      { return "AlmVal"; }

         /// Specific value range checks
      static bool checkAlmValRange(LNavFilterData* fd);
   };

      //@}

}

#endif // LNAVALMVALFILTER_HPP
