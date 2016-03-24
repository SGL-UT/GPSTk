#ifndef LNAVCROSSOURCEFILTER_HPP
#define LNAVCROSSOURCEFILTER_HPP

#include <NavFilterMgr.hpp>
#include <NavFilter.hpp>
#include <LNavFilterData.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS subframes using a voting algorithm across a
       * single epoch.  This may be used for a single receiver where
       * multiple codes can be compared against each other, or across
       * multiple receivers (with or without multiple codes).
       *
       * @attention Processing depth = 2 epochs. */
   class LNavCrossSourceFilter : public NavFilter
   {
   public:
      LNavCrossSourceFilter();

         /** Add LNAV messages to the voting collection (groupedNav).
          * @pre NavFilterKey::timeStamp is set to either the HOW time
          *   of the subframe, or the time of transmission of the
          *   subframe.
          * @pre NavFilterKey::prn is set
          * @pre LNavFilterData::sf is set
          * @param[in,out] msgBitsIn A list of LNavFilterData* objects
          *   containing GPS legacy navigation messages (id 2).
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter.  The contents of msgBitsOut will always be
          *   one epoch behind msgBitsIn (meaning data from prevous,
          *   but not current calls to validate will be here). */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);

         /** Flush the remaining contents of groupedNav.
          * @param[out] msgBitsOut Any remaining valid (by vote) nav
          *   messages are stored here on return. */
      virtual void finalize(NavMsgList& msgBitsOut);

   protected:
         /// Map from subframe data to source list
      typedef std::map<LNavFilterData*, NavMsgList, LNavMsgSort> SubframeMap;
         /// Map from PRN to SubframeMap
      typedef std::map<uint32_t, SubframeMap> NavMap;

         /// Nav subframes grouped by prn and unique nav bits
      NavMap groupedNav;
         /// Most recent time
      gpstk::CommonTime currentTime;


         /** Filter by vote.
          * @note Bare minimum for producing output is 2 out of 3
          *   matching subframes.  If there are no matching subframes,
          *   or fewer than 3 subframes are present in groupedNav, no
          *   output will be produced.
          * @param[out] msgBitsOut Nav messages passing the voting
          *   algorithm are stored here. */
      void examineSubframes(NavMsgList& msgBitsOut);
   };

      //@}

}

#endif // LNAVCROSSOURCEFILTER_HPP
