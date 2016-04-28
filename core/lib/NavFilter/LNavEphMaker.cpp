#include "LNavEphMaker.hpp"
#include "EngNav.hpp"

namespace gpstk
{
   LNavEphMaker ::
   LNavEphMaker()
   {
   }


   void LNavEphMaker ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator nmli;
      completeEphs.clear();
      for (nmli = msgBitsIn.begin(); nmli != msgBitsIn.end(); nmli++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
         uint32_t sfid = EngNav::getSFID(fd->sf[1]);
         if ((sfid < 1) || (sfid > 3))
         {
               // ignore non-ephemeris data
            accept(fd, msgBitsOut);
            continue;
         }
         if (ephemerides.find(fd) == ephemerides.end())
         {
               // set up us the ephemeris
            ephemerides[fd].resize(3, NULL);
         }
         EphGroup &ephGrp(ephemerides[fd]);
         ephGrp[sfid-1] = fd;

            // determine if a complete, valid ephemeris is available

            // check that all subframes are present
         if ((ephGrp[0] == NULL) || (ephGrp[1] == NULL) || (ephGrp[2] == NULL))
         {
            continue;
         }
            // check that TOW counts are consecutive
         if ((EngNav::getTOW(ephGrp[1]->sf[1]) -
              EngNav::getTOW(ephGrp[0]->sf[1]) != 1) ||
             (EngNav::getTOW(ephGrp[2]->sf[1]) -
              EngNav::getTOW(ephGrp[1]->sf[1]) != 1))
         {
            continue;
         }
            // check that the IODE/IODC are consistent
         if (((ephGrp[0]->sf[7] >> 22) != (ephGrp[1]->sf[2] >> 22)) ||
             ((ephGrp[0]->sf[7] >> 22) != (ephGrp[2]->sf[9] >> 22)))
         {
            continue;
         }
            // everything checks out, put the data where the user can find it
         completeEphs.push_back(&ephGrp);
      }
   }


   void LNavEphMaker ::
   finalize(NavMsgList& msgBitsOut)
   {
         // Don't really need to do anything but clear out our own
         // data structures.  Any valid ephemerides would have been
         // immediately produced as soon as they had been determined
         // valid, and all subframes are passed.
      ephemerides.clear();
      completeEphs.clear();
   }

}
