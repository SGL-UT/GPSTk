#include "TestUtil.hpp"
#include "NavFilterMgr.hpp"
#include "LNavFilterData.hpp"
#include "LNavParityFilter.hpp"
#include "LNavCookFilter.hpp"
#include "LNavEmptyFilter.hpp"
#include "LNavTLMHOWFilter.hpp"
#include "CommonTime.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

// hard-coded expectations... is there a better way?

// Checked against mdptool.. mdptool reports 1269; this is close enough
unsigned long expLNavParity = 1265;
// Checked using mdptool -s table and grep
unsigned long expLNavEmpty = 225;
// This number has not been vetted by other means
unsigned long expLNavTLMHOW = 613;

class NavFilterMgr_T
{
public:
   NavFilterMgr_T();

   void init();

   unsigned loadData();

      /// Test to make sure that with no filters, no data is removed
   unsigned noFilterTest();
      /// Cook the subframes
   unsigned testLNavCook();
      /// Test the LNAV parity filter
   unsigned testLNavParity();
      /// Test the LNAV empty subframe filter
   unsigned testLNavEmpty();
      /// Test the TLM and HOW filter
   unsigned testLNavTLMHOW();

   string inputFile;
      /// one for each record in the input file
   vector<LNavFilterData> data;
      /// ten for each record in the input file
   vector<uint32_t> subframes;

   unsigned long dataIdx;
};


NavFilterMgr_T ::
NavFilterMgr_T()
      : dataIdx(0)
{
      // about how much a day's worth of data is
   data.resize(40000);
   subframes.resize(400000);
   init();
}


void NavFilterMgr_T ::
init()
{
   TUDEF("NavFilterMgr", "initialize");
   string fs = getFileSep();
   string dp(testFramework.getDataPath() + fs);
   string tf(testFramework.getDataPath() + fs);

   inputFile = dp + "test_input_NavFilterMgr.txt";
}


unsigned NavFilterMgr_T ::
loadData()
{
   ifstream inf(inputFile.c_str());
   string line, timeString, wordStr;
   CommonTime recTime;
   unsigned long subframeIdx = 0;

   if (!inf)
   {
      cerr << "Could not load input file \"" << inputFile << "\"" << endl;
      return 1;
   }
   while (inf)
   {
      getline(inf, line);
      if (line[0] == '#')
         continue; // comment line
      if (line.length() == 0)
         continue; // blank line
      timeString = gpstk::StringUtils::firstWord(line, ',');
      scanTime(recTime, timeString, "%4Y %3j %02H:%02M:%04.1f");

         // check to make sure we don't run off the end of our vector
      if (dataIdx >= data.size())
      {
         data.resize(data.size() + 1000);
         subframes.resize(subframes.size() + 10000);
      }

      LNavFilterData tmp;
         // point at what will be the first word when loaded
      tmp.sf = &subframes[subframeIdx];
      for (unsigned strWord = 6; strWord <= 15; strWord++)
      {
         wordStr = gpstk::StringUtils::word(line, strWord, ',');
         subframes[subframeIdx++] = gpstk::StringUtils::x2uint(wordStr);
      }
      tmp.prn = gpstk::StringUtils::asUnsigned(
         gpstk::StringUtils::word(line, 2, ','));
         // note that the test file contents use enums that probably
         // don't match ObsID's enums but that's really not important
         // for this test.
      tmp.carrier = (ObsID::CarrierBand)gpstk::StringUtils::asInt(
         gpstk::StringUtils::word(line, 3, ','));
      tmp.code = (ObsID::TrackingCode)gpstk::StringUtils::asInt(
         gpstk::StringUtils::word(line, 4, ','));

      data[dataIdx++] = tmp;
   }
   cout << "Using " << dataIdx << " subframes" << endl;
   return 0;
}


unsigned NavFilterMgr_T ::
noFilterTest()
{
   TUDEF("NavFilterMgr", "validate");

   NavFilterMgr mgr;
   unsigned long count = 0;

   for (unsigned i = 0; i < dataIdx; i++)
   {
      gpstk::NavFilter::NavMsgList l = mgr.validate(&data[i]);
         // We could do an assert for each record but that would be
         // stupid. Just compare the final counts.
      count += l.size();
   }
   TUASSERTE(unsigned long, dataIdx, count);

   return testFramework.countFails();
}


// this should be executed before any other filter tests are used as
// it will upright all the data in memory.
unsigned NavFilterMgr_T ::
testLNavCook()
{
   TUDEF("LNavCookFilter", "validate");

   NavFilterMgr mgr;
   unsigned long count = 0;
   LNavCookFilter filtCook;

   mgr.addFilter(&filtCook);

   for (unsigned i = 0; i < dataIdx; i++)
   {
/*
      cout << "------------------" << endl
           << "   idx: " << i << endl;
      cout << "   before:";
      for (unsigned sfword = 0; sfword < 10; sfword++)
         cout << " " << hex << setw(8) << data[i].sf[sfword] << dec;
      cout << endl;
*/
      gpstk::NavFilter::NavMsgList l = mgr.validate(&data[i]);
         // We could do an assert for each record but that would be
         // stupid. Just compare the final counts.
/*
      cout << "   after: ";
      for (unsigned sfword = 0; sfword < 10; sfword++)
         cout << " " << hex << setw(8) << data[i].sf[sfword] << dec;
      cout << endl;
*/
      count += l.size();

/*
      gpstk::NavFilter::NavMsgList::const_iterator nmli;
         for (nmli = l.begin(); nmli != l.end(); nmli++)
         {
            LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
            cout << "   tow: " << ((fd->sf[1] >> 13) * 6) << endl;
         }
*/
   }
   TUASSERTE(unsigned long, dataIdx, count);

   return testFramework.countFails();
}


unsigned NavFilterMgr_T ::
testLNavParity()
{
   TUDEF("LNavParityFilter", "validate");

   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   LNavParityFilter filtParity;

   mgr.addFilter(&filtParity);

   for (unsigned i = 0; i < dataIdx; i++)
   {
      gpstk::NavFilter::NavMsgList l = mgr.validate(&data[i]);
      gpstk::NavFilter::NavMsgList::const_iterator nmli;
         /*
      if (!filtParity.rejected.empty())
      {
         for (nmli = filtParity.rejected.begin();
              nmli != filtParity.rejected.end();
              nmli++)
         {
            LNavFilterData *fd = dynamic_cast<LNavFilterData*>(*nmli);
            for (unsigned sfword = 0; sfword < 10; sfword++)
            {
               if ((sfword % 5) == 0)
                  cout << "nav" << (sfword+1) << ": ";
               cout << hex << setiosflags(ios::uppercase) << setw(8)
                    << setfill('0') << fd->sf[sfword] << dec << setfill(' ')
                    << "  ";
               if ((sfword % 5) == 4)
                  cout << endl;
            }
         }
      }
         */
      rejectCount += filtParity.rejected.size();
   }
   TUASSERTE(unsigned long, expLNavParity, rejectCount);

   return testFramework.countFails();
}


unsigned NavFilterMgr_T ::
testLNavEmpty()
{
   TUDEF("LNavEmptyFilter", "validate");

   uint32_t emptySF[10] = { 0,0,0,0,0,0,0,0,0,0 };
   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   LNavEmptyFilter filtEmpty;

   mgr.addFilter(&filtEmpty);

   for (unsigned i = 0; i < dataIdx; i++)
   {
      gpstk::NavFilter::NavMsgList l = mgr.validate(&data[i]);
      rejectCount += filtEmpty.rejected.size();
   }
   TUASSERTE(unsigned long, expLNavEmpty, rejectCount);
   LNavFilterData fd;
   fd.sf = emptySF;
   gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
   TUASSERTE(unsigned long, 0, l.size());
   TUASSERTE(unsigned long, 1, filtEmpty.rejected.size());

   return testFramework.countFails();
}


unsigned NavFilterMgr_T ::
testLNavTLMHOW()
{
   TUDEF("LNavTLMHOWFilter", "validate");

   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   LNavTLMHOWFilter filtTLMHOW;

   mgr.addFilter(&filtTLMHOW);

   for (unsigned i = 0; i < dataIdx; i++)
   {
         /*
      uint32_t sfid = ((data[i].sf[1] >> 8) & 0x07);
      if ((data[i].sf[0] & 0x3fc00000) != 0x22c00000)
         cout << (i+1) << " invalid preamble " << hex << (data[i].sf[0] & 0x3fc00000) << dec << endl;
      else if ((data[i].sf[1] & 0x03) != 0)
         cout << (i+1) << " invalid parity bits " << hex << (data[i].sf[1] & 0x03) << dec << endl;
      else if (((data[i].sf[1] >> 13) & 0x1ffff) >= 100800)
         cout << (i+1) << " invalid TOW count " << hex << ((data[i].sf[1] >> 13) & 0x1ffff) << dec << endl;
      else if ((sfid < 1) || (sfid > 5))
         cout << (i+1) << " invalid SF ID " << sfid << endl;
         */
      gpstk::NavFilter::NavMsgList l = mgr.validate(&data[i]);
      rejectCount += filtTLMHOW.rejected.size();
   }
   TUASSERTE(unsigned long, expLNavTLMHOW, rejectCount);

   return testFramework.countFails();
}


int main()
{
   unsigned errorTotal = 0;

   NavFilterMgr_T testClass;

   errorTotal += testClass.loadData();
   errorTotal += testClass.noFilterTest();
   errorTotal += testClass.testLNavCook();
   errorTotal += testClass.testLNavParity();
   errorTotal += testClass.testLNavEmpty();
   errorTotal += testClass.testLNavTLMHOW();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
