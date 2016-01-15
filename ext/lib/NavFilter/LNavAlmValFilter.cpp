#include "LNavAlmValFilter.hpp"
#include "EngNav.hpp"

namespace gpstk
{
   LNavAlmValFilter ::
   LNavAlmValFilter()
   {
   }


   void LNavAlmValFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator i;

      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*i);
         short sfid = EngNav::getSFID(fd->sf[1]);
         switch (sfid)
         {
            case 4:
            case 5:
               if (checkAlmValRange(fd))
                  accept(*i, msgBitsOut);
               else
                  reject(*i);
               break;
            default:
               accept(*i, msgBitsOut);
               break;
         }
      }
   }


   bool LNavAlmValFilter ::
   checkAlmValRange(LNavFilterData* fd)
   {
         /// @todo implement this
      return true;
   }
}
