//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
// This software developed by Applied Research Laboratories at the
// University of Texas at Austin, under contract to an agency or
// agencies within the U.S.  Department of Defense. The
// U.S. Government retains all rights to use, duplicate, distribute,
// disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================
#include "TestUtil.hpp"
#include "NavFilterMgr.hpp"
#include "LNavFilterData.hpp"
#include "LNavParityFilter.hpp"
#include "LNavCookFilter.hpp"
#include "LNavEmptyFilter.hpp"
#include "LNavTLMHOWFilter.hpp"
#include "LNavEphMaker.hpp"
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
// This number represents the union of subframes stripped by parity,
// empty and TLM/HOW checks.
unsigned long expLNavCombined = 1488;
// This number was vetted by getting a rough count of ephemerides in
// the source file (which was 5526, which is in the same ball park).
// /usr/bin/tail +109 test_input_NavFilterMgr.txt | head -27513 | grep ':[03]0.0, ' | wc -l
unsigned long expLNavEphs = 5210;

// define some classes for exercising NavFilterMgr
class BunkFilterData : public NavFilterKey
{
public:
   BunkFilterData()
         : data(NULL)
   {}
   uint32_t *data; // point to a single uint32_t
};
// filter by bit pattern
class BunkFilter1 : public NavFilter
{
public:
   BunkFilter1() {}
   virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator nmli;
      for (nmli = msgBitsIn.begin(); nmli != msgBitsIn.end(); nmli++)
      {
            // copy data with an arbitrary bit pattern
         BunkFilterData *fd = dynamic_cast<BunkFilterData*>(*nmli);
         if ((*(fd->data) & 0xff) == 0x000000d1)
            msgBitsOut.push_back(*nmli);
      }
   }
   virtual void finalize(NavMsgList& msgBitsOut)
   {}
};
// filter with cache
class BunkFilter2 : public NavFilter
{
public:
   BunkFilter2() {}
   virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator nmli;
      std::copy(msgBitsIn.begin(), msgBitsIn.end(),
                std::back_insert_iterator<NavMsgList>(cache));
      while (cache.size() > 4)
      {
         msgBitsOut.push_back(cache.front());
         cache.pop_front();
      }
   }
   virtual void finalize(NavMsgList& msgBitsOut)
   {
      std::copy(cache.begin(), cache.end(),
                std::back_insert_iterator<NavMsgList>(msgBitsOut));
      cache.clear();
   }
   NavMsgList cache;
};

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
      /// Test the ephemeris maker
   unsigned testLNavEphMaker();
      /// Test the combination of parity, empty and TLM/HOW filters
   unsigned testLNavCombined();

      /// test a simple bit pattern filter
   unsigned testBunk1();
      /// test a filter with behavior like multiple input epochs
   unsigned testBunk2();

   string inputFileLNAV;
   string inputFileBunk;
   string refFileBunk1, refFileBunk2;
   string outputFileBunk1, outputFileBunk2;
      /// one for each record in the input file
   vector<LNavFilterData> dataLNAV;
      /// ten for each record in the input file
   vector<uint32_t> subframesLNAV;
      /// "subframes" for the "bunk" test classes
   vector<uint32_t> subframesBunk;
   vector<BunkFilterData> dataBunk;

   unsigned long dataIdxLNAV, dataIdxBunk;
};


NavFilterMgr_T ::
NavFilterMgr_T()
      : dataIdxLNAV(0),
        dataIdxBunk(0)
{
      // about how much a day's worth of data is
   dataLNAV.resize(40000);
   subframesLNAV.resize(400000);
   dataBunk.resize(40000);
   subframesBunk.resize(400000, 0xdabbad00);
   init();
}


void NavFilterMgr_T ::
init()
{
   TUDEF("NavFilterMgr", "initialize");
   string fs = getFileSep();
   string dp(gpstk::getPathData() + fs);
   string tf(gpstk::getPathTestTemp() + fs);

   inputFileLNAV   = dp + "test_input_NavFilterMgr.txt";
   inputFileBunk   = dp + "test_input_NavFilterMgr_bunk.txt";
   refFileBunk1    = dp + "test_output_NavFilterMgr_bunk1.txt";
   refFileBunk2    = dp + "test_output_NavFilterMgr_bunk2.txt";
   outputFileBunk1 = tf + "test_output_NavFilterMgr_bunk1.txt";
   outputFileBunk2 = tf + "test_output_NavFilterMgr_bunk2.txt";
}


unsigned NavFilterMgr_T ::
loadData()
{
   ifstream inf(inputFileLNAV.c_str());
   string line, timeString, wordStr;
   CommonTime recTime;
   unsigned long subframeIdx = 0;

   if (!inf)
   {
      cerr << "Could not load input file \"" << inputFileLNAV << "\"" << endl;
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
      if (dataIdxLNAV >= dataLNAV.size())
      {
         dataLNAV.resize(dataLNAV.size() + 1000);
         subframesLNAV.resize(subframesLNAV.size() + 10000);
      }

      LNavFilterData tmp;
         // point at what will be the first word when loaded
      tmp.sf = &subframesLNAV[subframeIdx];
      for (unsigned strWord = 6; strWord <= 15; strWord++)
      {
         wordStr = gpstk::StringUtils::word(line, strWord, ',');
         subframesLNAV[subframeIdx++] = gpstk::StringUtils::x2uint(wordStr);
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

      dataLNAV[dataIdxLNAV++] = tmp;
   }
   inf.close();
   cout << "Using " << dataIdxLNAV << " LNAV subframes" << endl;

      // load "bunk" data
   BunkFilterData bunkKey;
   subframeIdx = 0;
   inf.open(inputFileBunk.c_str());
   if (!inf)
   {
      cerr << "Could not load input file \"" << inputFileBunk << "\"" << endl;
      return 1;
   }
   while (inf)
   {
      getline(inf, line);
      if (line[0] == '#')
         continue; // comment line
      if (line.length() == 0)
         continue; // blank line
      for (unsigned strWord = 1; strWord <= 4; strWord++)
      {
         wordStr = gpstk::StringUtils::word(line, strWord, ' ');
         bunkKey.data = &subframesBunk[subframeIdx];
         subframesBunk[subframeIdx++] = gpstk::StringUtils::x2uint(wordStr);
            // we don't really care waht the prn, carrier or code are
            // for this test
         dataBunk[dataIdxBunk++] = bunkKey;
      }
   }
   inf.close();
   cout << "Using " << dataIdxBunk << " \"Bunk\" subframes" << endl;

   return 0;
}


unsigned NavFilterMgr_T ::
noFilterTest()
{
   TUDEF("NavFilterMgr", "validate");

   NavFilterMgr mgr;
   unsigned long count = 0;

   for (unsigned i = 0; i < dataIdxLNAV; i++)
   {
      gpstk::NavFilter::NavMsgList l = mgr.validate(&dataLNAV[i]);
         // We could do an assert for each record but that would be
         // stupid. Just compare the final counts.
      count += l.size();
   }
   TUASSERTE(unsigned long, dataIdxLNAV, count);

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

   for (unsigned i = 0; i < dataIdxLNAV; i++)
   {
/*
      cout << "------------------" << endl
           << "   idx: " << i << endl;
      cout << "   before:";
      for (unsigned sfword = 0; sfword < 10; sfword++)
         cout << " " << hex << setw(8) << dataLNAV[i].sf[sfword] << dec;
      cout << endl;
*/
      gpstk::NavFilter::NavMsgList l = mgr.validate(&dataLNAV[i]);
         // We could do an assert for each record but that would be
         // stupid. Just compare the final counts.
/*
      cout << "   after: ";
      for (unsigned sfword = 0; sfword < 10; sfword++)
         cout << " " << hex << setw(8) << dataLNAV[i].sf[sfword] << dec;
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
   TUASSERTE(unsigned long, dataIdxLNAV, count);

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

   for (unsigned i = 0; i < dataIdxLNAV; i++)
   {
      gpstk::NavFilter::NavMsgList l = mgr.validate(&dataLNAV[i]);
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
/*
      if (!filtParity.rejected.empty())
         cerr << "filter " << i << " parity" << endl;
*/
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

   for (unsigned i = 0; i < dataIdxLNAV; i++)
   {
      gpstk::NavFilter::NavMsgList l = mgr.validate(&dataLNAV[i]);
      rejectCount += filtEmpty.rejected.size();
/*
      if (!filtEmpty.rejected.empty())
         cerr << "filter " << i << " empty" << endl;
*/
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

   for (unsigned i = 0; i < dataIdxLNAV; i++)
   {
         /*
      uint32_t sfid = ((dataLNAV[i].sf[1] >> 8) & 0x07);
      if ((dataLNAV[i].sf[0] & 0x3fc00000) != 0x22c00000)
         cout << (i+1) << " invalid preamble " << hex << (dataLNAV[i].sf[0] & 0x3fc00000) << dec << endl;
      else if ((dataLNAV[i].sf[1] & 0x03) != 0)
         cout << (i+1) << " invalid parity bits " << hex << (dataLNAV[i].sf[1] & 0x03) << dec << endl;
      else if (((dataLNAV[i].sf[1] >> 13) & 0x1ffff) >= 100800)
         cout << (i+1) << " invalid TOW count " << hex << ((dataLNAV[i].sf[1] >> 13) & 0x1ffff) << dec << endl;
      else if ((sfid < 1) || (sfid > 5))
         cout << (i+1) << " invalid SF ID " << sfid << endl;
         */
      gpstk::NavFilter::NavMsgList l = mgr.validate(&dataLNAV[i]);
      rejectCount += filtTLMHOW.rejected.size();
/*
      if (!filtTLMHOW.rejected.empty())
         cerr << "filter " << i << " tlm/how" << endl;
*/
   }
   TUASSERTE(unsigned long, expLNavTLMHOW, rejectCount);

   return testFramework.countFails();
}


// make sure the eph maker produces the expected number of complete ephemerides
unsigned NavFilterMgr_T ::
testLNavEphMaker()
{
   TUDEF("LNavTLMHOWFilter", "validate");

   NavFilterMgr mgr;
   unsigned long ephCount = 0;
   LNavEphMaker filtEph;

   mgr.addFilter(&filtEph);

   for (unsigned i = 0; i < dataIdxLNAV; i++)
   {
      gpstk::NavFilter::NavMsgList l = mgr.validate(&dataLNAV[i]);
      ephCount += filtEph.completeEphs.size();
   }
   TUASSERTE(unsigned long, expLNavEphs, ephCount);

   return testFramework.countFails();
}


unsigned NavFilterMgr_T ::
testLNavCombined()
{
   TUDEF("NavFilterMgr", "validate");

   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   LNavParityFilter filtParity;
   LNavEmptyFilter filtEmpty;
   LNavTLMHOWFilter filtTLMHOW;

   mgr.addFilter(&filtParity);
   mgr.addFilter(&filtEmpty);
   mgr.addFilter(&filtTLMHOW);

   for (unsigned i = 0; i < dataIdxLNAV; i++)
   {
/*
      cerr << "checking " << i << endl;
      if (i == 2196)
      {
         for (unsigned sfword = 0; sfword < 10; sfword++)
            cerr << " " << hex << setw(8) << dataLNAV[i].sf[sfword] << dec;
         cerr << endl;
      }
*/
      gpstk::NavFilter::NavMsgList l = mgr.validate(&dataLNAV[i]);
         // if l is empty, the subframe was rejected.. 
      rejectCount += l.empty();
/*
      if (l.empty())
         cerr << "filter " << i << " combined" << endl;
*/
   }
   TUASSERTE(unsigned long, expLNavCombined, rejectCount);

   return testFramework.countFails();
}


unsigned NavFilterMgr_T ::
testBunk1()
{
   TUDEF("NavFilterMgr", "validate");

   ofstream outs(outputFileBunk1.c_str());
   NavFilterMgr mgr;
   BunkFilter1 filt1;
   gpstk::NavFilter::NavMsgList l;
   gpstk::NavFilter::NavMsgList::const_iterator nmli;

   if (!outs)
   {
      TUFAIL("Could not open \"" + outputFileBunk1 + "\" for output");
      return testFramework.countFails();
   }

   mgr.addFilter(&filt1);
   for (unsigned i = 0; i < dataIdxBunk; i++)
   {
      l = mgr.validate(&dataBunk[i]);
      for (nmli = l.begin(); nmli != l.end(); nmli++)
      {
         BunkFilterData *fd = dynamic_cast<BunkFilterData*>(*nmli);
         outs << hex << setw(8) << setfill('0') << *(fd->data) << setfill(' ')
              << dec << endl;
      }
   }
   l = mgr.finalize();
   for (nmli = l.begin(); nmli != l.end(); nmli++)
   {
      BunkFilterData *fd = dynamic_cast<BunkFilterData*>(*nmli);
      outs << hex << setw(8) << setfill('0') << *(fd->data) << setfill(' ')
           << dec << endl;
   }
   outs.close();
   testFramework.assert_files_equal(__LINE__, refFileBunk1, outputFileBunk1,
                                    "Files differ");

   return testFramework.countFails();
}


unsigned NavFilterMgr_T ::
testBunk2()
{
      // the filter uses cached data because we're more interested in
      // testing finalize here.
   TUDEF("NavFilterMgr", "finalize");

   ofstream outs(outputFileBunk2.c_str());
   NavFilterMgr mgr;
   BunkFilter2 filt2;
   gpstk::NavFilter::NavMsgList l;
   gpstk::NavFilter::NavMsgList::const_iterator nmli;

   if (!outs)
   {
      TUFAIL("Could not open \"" + outputFileBunk2 + "\" for output");
      return testFramework.countFails();
   }

   mgr.addFilter(&filt2);
      // count of wrong filter results before and after reaching the cache size
   unsigned wrongEarly = 0, wrongLate = 0;
   for (unsigned i = 0; i < dataIdxBunk; i++)
   {
      l = mgr.validate(&dataBunk[i]);
/*
      if ((i < 3) && (l.size() != 0))
      {
         TUFAIL("Filter expected to return no data before 4 messages input");
      }
      else if ((i >= 3) && (l.size() != 1))
      {
         TUFAIL("Filter expected to return 1 message after 4 messages input");
      }
*/
      if (i <= 3)
      {
         if (l.size() != 0)
            wrongEarly++;
      }
      else
      {
         if (l.size() != 1)
            wrongLate++;
      }

      for (nmli = l.begin(); nmli != l.end(); nmli++)
      {
         BunkFilterData *fd = dynamic_cast<BunkFilterData*>(*nmli);
         outs << hex << setw(8) << setfill('0') << *(fd->data) << setfill(' ')
              << dec << endl;
      }
   }
      // just do all the assertions at once
   testFramework.changeSourceMethod("validate");
   TUASSERTE(unsigned,0,wrongEarly);
   TUASSERTE(unsigned,0,wrongLate);

   l = mgr.finalize();
      // cache of size 4 so finalize should return the last four messages
   testFramework.changeSourceMethod("finalize");
   TUASSERTE(size_t,4,l.size());
   for (nmli = l.begin(); nmli != l.end(); nmli++)
   {
      BunkFilterData *fd = dynamic_cast<BunkFilterData*>(*nmli);
      outs << hex << setw(8) << setfill('0') << *(fd->data) << setfill(' ')
           << dec << endl;
   }
   outs.close();
   testFramework.assert_files_equal(__LINE__, refFileBunk2, outputFileBunk2,
                                    "Files differ");

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
   errorTotal += testClass.testLNavEphMaker();
   errorTotal += testClass.testLNavCombined();
   errorTotal += testClass.testBunk1();
   errorTotal += testClass.testBunk2();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
