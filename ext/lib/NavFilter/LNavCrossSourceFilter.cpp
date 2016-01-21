#include "LNavCrossSourceFilter.hpp"

namespace gpstk
{
   LNavCrossSourceFilter ::
   LNavCrossSourceFilter()
         : currentTOW(-1)
   {
   }

   void LNavCrossSourceFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
         /** @todo figure out the best way to handle the situation
          * where the HOW is corrupt and as such the time stamp is
          * likely also wrong. */
      NavMsgList::const_iterator nmli;
      for (nmli = msgBitsIn.begin(); nmli != msgBitsIn.end(); nmli++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
         uint32_t tow = fd->sf[1] >> 13;
         if (tow != currentTOW)
         {
               // different TOW, so check out what we have
            examineSubframes(msgBitsOut);
            groupedNav.clear();
            currentTOW = tow;
         }
            // add the subframe to our collection
         groupedNav[fd->prn][fd].push_back(*nmli);
      }
   }

   void LNavCrossSourceFilter ::
   finalize(NavMsgList& msgBitsOut)
   {
      examineSubframes(msgBitsOut);
      groupedNav.clear();
      currentTOW = -1;
   }

   void LNavCrossSourceFilter ::
   examineSubframes(NavMsgList& msgBitsOut)
   {
      NavMap::const_iterator nmi;
      SubframeMap::const_iterator smi;
         // loop over each PRN/SV
      for (nmi = groupedNav.begin(); nmi != groupedNav.end(); nmi++)
      {
            // count of unique messages
         size_t uniqueBits = nmi->second.size();
            // count of total messages
         size_t msgCount = 0;
            // store the vote winner here
         LNavFilterData *winner = NULL;
            // store the largest number of "votes" for a subframe here
         size_t voteCount = 0;
         for (smi = nmi->second.begin(); smi != nmi->second.end(); smi++)
         {
            size_t msgs = smi->second.size();
            msgCount += msgs;
               // minimum # of useful votes
            if ((msgs > voteCount) && (msgs >= 2))
            {
               voteCount = msgs;
               winner = smi->first;
            }
         }
         if (msgCount < 3)
            continue; // not enough messages to have a useful vote
         if (winner)
         {
               // we have a valid vote winner, so copy the subframes to output
            smi = nmi->second.find(winner);
            std::copy(smi->second.begin(), smi->second.end(),
                      std::back_insert_iterator<NavMsgList>(msgBitsOut));
         }
      }
   }
}
