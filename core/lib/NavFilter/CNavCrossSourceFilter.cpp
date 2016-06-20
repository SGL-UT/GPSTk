#include "CNavCrossSourceFilter.hpp"

namespace gpstk
{
   CNavCrossSourceFilter ::
   CNavCrossSourceFilter() : minIdentical(2)
   {
   }

   void CNavCrossSourceFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator nmli;
      for (nmli = msgBitsIn.begin(); nmli != msgBitsIn.end(); nmli++)
      {
         CNavFilterData *fd = dynamic_cast<CNavFilterData*>(*nmli);
         if (fd->timeStamp != currentTime)
         {
               // different time, so check out what we have
            examineMessages(msgBitsOut);
            groupedNav.clear();
            currentTime = fd->timeStamp;
         }
            // add the message to our collection
         groupedNav[fd->prn][fd].push_back(*nmli);
      }
   }

   void CNavCrossSourceFilter ::
   finalize(NavMsgList& msgBitsOut)
   {
      examineMessages(msgBitsOut);
      groupedNav.clear();
      currentTime.reset();
   }

   void CNavCrossSourceFilter ::
   examineMessages(NavMsgList& msgBitsOut)
   {
      NavMap::const_iterator nmi;
      MessageMap::const_iterator smi;
         // loop over each PRN/SV
      for (nmi = groupedNav.begin(); nmi != groupedNav.end(); nmi++)
      {
            // count of unique messages
         size_t uniqueBits = nmi->second.size();
            // count of total messages
         size_t msgCount = 0;
            // store the vote winner here
         CNavFilterData *winner = NULL;
            // store the largest number of "votes" for a subframe here
         size_t voteCount = 0;
         for (smi = nmi->second.begin(); smi != nmi->second.end(); smi++)
         {
            size_t msgs = smi->second.size();
            msgCount += msgs;
               // minimum # of useful votes
            if ((msgs > voteCount) && (msgs >= minIdentical))
            {
               voteCount = msgs;
               winner = smi->first;
            }
         }
         if (msgCount < minIdentical)
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

   void CNavCrossSourceFilter::
   dump(std::ostream& s) const
   {
      s << "#--------------------------------" << std::endl;
      s << "  Dump of CNavCrossSourceFilter::NavMap" << std::endl; 
      NavMap::const_iterator cit;
      for (cit=groupedNav.begin(); cit!=groupedNav.end(); cit++)
      {
         s << "PRN " << std::setw(2) << (unsigned short) cit->first << std::endl;

         const MessageMap& mm = cit->second;
         MessageMap::const_iterator cit2;
         for (cit2=mm.begin(); cit2!=mm.end(); cit2++)
         {
            const CNavFilterData* cfdp = cit2->first;
            const NavMsgList& nml = cit2->second;

            s << "  Key  ptr: " << cfdp << ": " << *cfdp << std::endl;
            NavMsgList::const_iterator cit3;
            for (cit3=nml.begin(); cit3!=nml.end(); cit3++)
            {
               const NavFilterKey* nfkp = *cit3;
               s << "       ptr: " << nfkp << ": " << *nfkp  << std::endl; 
            }
         }
      }
   }

}
