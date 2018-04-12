#include "LNavOrderFilter.hpp"
#include "GPSWeekZcount.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   LNavOrderFilter ::
   LNavOrderFilter(unsigned depth, unsigned epoch)
         : procDepth(depth),
           epochInSeconds(epoch)
   {
   }


   void LNavOrderFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::const_iterator nmli;
      CommonTime oldestAllowed; // default to super old i.e. any data is ok
      oldestAllowed.setTimeSystem(gpstk::TimeSystem::Any);
      if (!orderedNav.empty())
      {
         SubframeSet::reverse_iterator rssi = orderedNav.rbegin();
         CommonTime newest = (*rssi)->timeStamp;
         oldestAllowed = newest - (epochInSeconds * procDepth);
      }
      for (nmli = msgBitsIn.begin(); nmli != msgBitsIn.end(); nmli++)
      {
//         std::cerr << "OrderAdd " << *(*nmli) << std::endl;
            // Do not add anything that's too old.  We do the checking
            // here instead of in examineSubframes so that if the nav
            // timestamp jumps forward more than one epoch, it doesn't
            // cause perfectly valid nav data to be rejected.
         if ((*nmli)->timeStamp < oldestAllowed)
         {
//            std::cerr << "OrderReject " << *(*nmli) << std::endl;
            reject(*nmli);
            continue;
         }
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
         orderedNav.insert(fd);
      }
      examineSubframes(msgBitsOut);
   }


   void LNavOrderFilter ::
   finalize(NavMsgList& msgBitsOut)
   {
      std::copy(orderedNav.begin(), orderedNav.end(),
                std::back_inserter(msgBitsOut));
      orderedNav.clear();
   }


   void LNavOrderFilter ::
   examineSubframes(NavMsgList& msgBitsOut)
   {
      using gpstk::printTime;
      if (orderedNav.empty())
         return; // nothing to do

      SubframeSet::iterator ssi, oldIt = orderedNav.end();
      SubframeSet::reverse_iterator rssi = orderedNav.rbegin();
      CommonTime newest = (*rssi)->timeStamp;
      CommonTime oldestAllowed = newest - (epochInSeconds * procDepth);
      SubframeSet::iterator oldestIt = orderedNav.begin();
      GPSWeekZcount oldestTime = (*oldestIt)->timeStamp;
         /*
           std::cerr << "LNavOrderFilter"
           << "  n=" << printTime(newest,"%Fw%Zz")
           << "  o=" << printTime(oldestTime,"%Fw%Zz")
           << "  oa=" << printTime(oldestAllowed,"%Fw%Zz")
           << std::endl;
         */

      for (ssi = orderedNav.begin(); ssi != orderedNav.end(); ssi++)
      {
         if ((*ssi)->timeStamp <= oldestAllowed)
         {
//            std::cerr << "OrderAccept " << *(*ssi) << std::endl;
            accept(*ssi, msgBitsOut);
               // This subframe is being accepted, so include it as
               // one to be erased from the set when we're done.
            oldIt = ssi;
         }
         else
         {
               // Since we're processing in time order and we've
               // reached data newer than the oldest allowed, we don't
               // have any more processing to do.
            break;
         }
      }
         // if oldIt is end() then we don't have anything to erase.
      if (oldIt != orderedNav.end())
      {
            // erase is up to BUT NOT INCLUDING the second iterator,
            // so increment oldIt to make sure to erase the last
            // element we need to erase.
         orderedNav.erase(orderedNav.begin(), ++oldIt);
      }
   }

}
