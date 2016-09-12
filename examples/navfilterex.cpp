#include <iostream>
#include <vector>
#include "NavFilterMgr.hpp"
#include "LNavFilterData.hpp"
#include "LNavParityFilter.hpp"
#include "LNavCookFilter.hpp"
#include "CommonTime.hpp"
#include "TimeString.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk;

int main()
{
   NavFilterMgr mgr;
   LNavCookFilter filtCook;
   LNavParityFilter filtParity;
   LNavFilterData navFiltData;
   std::string line, timeString, wordStr;
   CommonTime recTime;
      // Note that storing a single subframe is only valid in this
      // example because the filters in use immediately return the
      // data.
   std::vector<uint32_t> subframe(10,0);

   mgr.addFilter(&filtCook);
   mgr.addFilter(&filtParity);

      // point at what will be the first word when loaded
   navFiltData.sf = &subframe[0];

   while (cin)
   {
         // read nav data from stdin
      getline(cin, line);
      if (line[0] == '#')
         continue; // comment line
      if (line.length() == 0)
         continue; // blank line
      timeString = gpstk::StringUtils::firstWord(line, ',');
      scanTime(recTime, timeString, "%4Y %3j %02H:%02M:%04.1f");

         // copy the subframe words into subframe
      unsigned subframeIdx = 0;
      for (unsigned strWord = 6; strWord <= 15; strWord++)
      {
         wordStr = gpstk::StringUtils::word(line, strWord, ',');
         subframe[subframeIdx++] = gpstk::StringUtils::x2uint(wordStr);
      }
      navFiltData.prn = gpstk::StringUtils::asUnsigned(
         gpstk::StringUtils::word(line, 2, ','));
         // note that the test file contents use enums that probably
         // don't match ObsID's enums but that's really not important
         // for this example.
      navFiltData.carrier = (ObsID::CarrierBand)gpstk::StringUtils::asInt(
         gpstk::StringUtils::word(line, 3, ','));
      navFiltData.code = (ObsID::TrackingCode)gpstk::StringUtils::asInt(
         gpstk::StringUtils::word(line, 4, ','));

         // validate the subframe
      NavFilter::NavMsgList l = mgr.validate(&navFiltData);
      NavFilter::NavMsgList::const_iterator nmli;
         // write any valid data to stdout
      for (nmli = l.begin(); nmli != l.end(); nmli++)
      {
         cout << timeString << ", 310, " << setw(2) << (*nmli)->prn << ", "
              << setw(1) << (*nmli)->carrier << ", " << (*nmli)->code << ", 1";
         LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
         for (unsigned sfword = 0; sfword < 10; sfword++)
         {
            cout << ", " << hex << setiosflags(ios::uppercase) << setw(8)
                 << setfill('0') << fd->sf[sfword] << dec << setfill(' ');
         }
         cout << endl;
      }
   }

}
