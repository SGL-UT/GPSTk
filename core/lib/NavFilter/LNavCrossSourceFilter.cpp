#include "LNavCrossSourceFilter.hpp"

namespace gpstk
{
   LNavCrossSourceFilter ::
   LNavCrossSourceFilter()
   {
   }

   void LNavCrossSourceFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator nmli;
      for (nmli = msgBitsIn.begin(); nmli != msgBitsIn.end(); nmli++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
         if (fd->timeStamp != currentTime)
         {
               // different time, so check out what we have
            examineSubframes(msgBitsOut);
            groupedNav.clear();
            currentTime = fd->timeStamp;
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
      currentTime.reset();
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
            winner = NULL; // not enough messages to have a useful vote

            // If winner is NULL, i.e. there is no winner, all
            // messages will be rejected below.  Otherwise only the
            // winners will be accepted.
         for (smi = nmi->second.begin(); smi != nmi->second.end(); smi++)
         {
            if (smi->first == winner)
               accept(smi->second, msgBitsOut);
            else
               reject(smi->second);
         }
      }
   }
}
