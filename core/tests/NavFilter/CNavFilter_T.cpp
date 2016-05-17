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
#include "CNavFilterData.hpp"
#include "CNavParityFilter.hpp"
#include "CNavCookFilter.hpp"
#include "CNavEmptyFilter.hpp"
#include "CNavTOWFilter.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "NavFilter.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"
#include "TimeSystem.hpp"

using namespace std;
using namespace gpstk;

/*
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
*/

class CNavFilter_T
{
public:
   CNavFilter_T();

   void init();

   unsigned loadData();

      /// Test to make sure that with no filters, no data is removed
   unsigned noFilterTest();
      /// Cook the subframes
   unsigned testCNavCook();
      /// Test the LNAV parity filter
   unsigned testCNavParity();
      /// Test the LNAV empty subframe filter
   unsigned testCNavEmpty();
      /// Test the TLM and HOW filter
   unsigned testCNavTOW();
      /// Test the combination of parity, empty and TOW filters
   unsigned testCNavCombined();

   list<PackedNavBits*> messageList;
   list<CNavFilterData> cNavList;  

};

CNavFilter_T ::
CNavFilter_T()
{
   init();
}

void CNavFilter_T ::
init()
{
   TUDEF("CNavFilter", "initialize");
   string fs = getFileSep();
   string dp(gpstk::getPathData() + fs);
   string tf(gpstk::getPathTestTemp() + fs);

//   inputFileLNAV   = dp + "test_input_CNavFilter.txt";
//   outputFile = tf + "test_output_NavFilter.txt";
}


unsigned CNavFilter_T ::
loadData()
{
   static const int MSG_COUNT_CNAV = 12; 
   string sv63_CNAV[] =
   {
      "365,12/31/2015,00:00:00,1877,345600,1,63,10, 0x8B04A708, 0x10EAA60A, 0x6A49007A, 0x2E3FFDAE, 0x42EEB000, 0x81B983C7, 0x9A881433, 0x89C04F25, 0xB9F60DD4, 0xED600000",
      "365,12/31/2015,00:00:12,1877,345612,1,63,11, 0x8B04B708, 0x22490999, 0x6E1AC9D0, 0xF0CB205F, 0xFFEE3FFA, 0xBFFC9FF7, 0x94201BB3, 0x6022E5FF, 0x8B9802D4, 0xA5600000",
      "365,12/31/2015,00:00:24,1877,345624,1,63,30, 0x8B05E708, 0x320A6A79, 0x240214AC, 0x80084000, 0x0B0FFB7E, 0x5019E0D4, 0x0DFFFF02, 0x39F1FF11, 0x5500081A, 0x33500000",
      "365,12/31/2015,00:00:36,1877,345636,1,63,33, 0x8B061708, 0x420A6A79, 0x240214AC, 0x80084001, 0xFFCE0000, 0x02298EE7, 0x5539D988, 0x80000000, 0x00000486, 0xBF400000",
      "365,12/31/2015,02:00:00,1877,352800,1,63,10, 0x8B04A72D, 0x90EAA60A, 0x6A550077, 0x2A400140, 0xF2F62FFF, 0xC4FC3082, 0x7A701435, 0xE9084F1E, 0xA9CA02EF, 0x1B800000",
      "365,12/31/2015,02:00:12,1877,352812,1,63,11, 0x8B04B72D, 0xA2550999, 0x22EA09D0, 0xF0770055, 0x8FA57FED, 0xA007DFF6, 0x9B001BF7, 0xA0224CFF, 0x7D980CAA, 0xA4200000",
      "365,12/31/2015,02:00:24,1877,352824,1,63,30, 0x8B05E72D, 0xB20A6A79, 0x54021520, 0x80084000, 0x0B0FFB7E, 0x5019E0D4, 0x0DFFFF02, 0x39F1FF11, 0x55000016, 0xEC400000",
      "365,12/31/2015,02:00:36,1877,352836,1,63,33, 0x8B06172D, 0xC20A6A79, 0x54021520, 0x80084001, 0xFFCE0000, 0x02298EE7, 0x5539D988, 0x80000000, 0x00000C8A, 0x60500000",
      "365,12/31/2015,04:00:00,1877,360000,1,63,10, 0x8B04A753, 0x10EAA60A, 0x6A610078, 0xE17FFE9C, 0xA30F7800, 0x64BEDD4C, 0xDBB81434, 0x9B6C4F13, 0x67480568, 0xC7C00000",
      "365,12/31/2015,04:00:12,1877,360012,1,63,11, 0x8B04B753, 0x22610998, 0xD6B2C9D0, 0xEF01800F, 0xFF5EE007, 0xA00B3FF5, 0xEA001B8B, 0xE022CCFF, 0x755807EB, 0xB3300000",
      "365,12/31/2015,04:00:24,1877,360024,1,63,30, 0x8B05E753, 0x320A6A79, 0x84021595, 0x00084000, 0x0B0FFB7E, 0x5019E0D4, 0x0DFFFF02, 0x39F1FF11, 0x55000411, 0xB1F00000",
      "365,12/31/2015,04:00:36,1877,360036,1,63,33, 0x8B061753, 0x420A6A79, 0x84021595, 0x00084001, 0xFFCE0000, 0x02298EE7, 0x5539D988, 0x80000000, 0x0000088D, 0x3DE00000"
   };
   static const string sv50_CNAV[] =
   {
      "365,12/31/2015,00:00:00,1877,345600,5,50,10, 0x8B14A708, 0x10EAA605, 0xEA4900F8, 0x87BFFFCF, 0xE33EE000, 0x0DD1110C, 0x796012CA, 0x8EB84F34, 0xC1F0049B, 0x24900000",
      "365,12/31/2015,00:00:12,1877,345612,5,50,11, 0x8B14B708, 0x22491E9A, 0xBFDBC9A2, 0xACF8E007, 0xC0DE000E, 0xC002C003, 0x510016F7, 0x4028AB00, 0x2FF00D9E, 0x66400000",
      "365,12/31/2015,00:00:24,1877,345624,5,50,30, 0x8B15E708, 0x3205E5F9, 0x25D6111F, 0x801FB001, 0xE90FFC83, 0x1E001000, 0x0DFFFF02, 0x39F1FF11, 0x55000952, 0x83B00000",
      "365,12/31/2015,00:00:36,1877,345636,5,50,33, 0x8B161708, 0x4205E5F9, 0x25D6111F, 0x801FB001, 0xFFCE0000, 0x022979C7, 0x5539D988, 0x80000000, 0x00000F0F, 0x80A00000",
      "365,12/31/2015,02:00:00,1877,352800,5,50,10, 0x8B14A72D, 0x90EAA605, 0xEA5500F1, 0x10800003, 0x134AF000, 0x16E3BDCD, 0x7D8012C9, 0x8F304F2A, 0xDF9A0CA4, 0xD5A00000",
      "365,12/31/2015,02:00:12,1877,352812,5,50,11, 0x8B14B72D, 0xA2551E9A, 0x72C749A2, 0xAE352013, 0x70829FFF, 0xFFF80002, 0xEE80181B, 0x0026F400, 0x28580984, 0x56000000",
      "365,12/31/2015,02:00:24,1877,352824,5,50,30, 0x8B15E72D, 0xB205E5F9, 0x55D612DD, 0x001FB001, 0xE90FFC83, 0x1E001000, 0x0DFFFF02, 0x39F1FF11, 0x550008B6, 0x7E600000",
      "365,12/31/2015,02:00:36,1877,352836,5,50,33, 0x8B16172D, 0xC205E5F9, 0x55D612DD, 0x001FB001, 0xFFCE0000, 0x022979C7, 0x5539D988, 0x80000000, 0x00000EEB, 0x7D700000",
      "365,12/31/2015,04:00:00,1877,360000,5,50,10, 0x8B14A753, 0x10EAA605, 0xEA6100F8, 0x27400088, 0x334447FF, 0xE2966A43, 0x63D812C8, 0xAE484F33, 0xD3FC0C2A, 0x5C400000",
      "365,12/31/2015,04:00:12,1877,360012,5,50,11, 0x8B14B753, 0x22611E9A, 0x24C149A2, 0xAF354020, 0x8088FFF9, 0xFFFB4001, 0xF5601886, 0x6026A800, 0x1A800B07, 0xF9800000",
      "365,12/31/2015,04:00:24,1877,360024,5,50,30, 0x8B15E753, 0x3205E5F9, 0x85D6149A, 0x801FA801, 0xE90FFC83, 0x1E001000, 0x0DFFFF02, 0x39F1FF11, 0x55000781, 0x8FB00000",
      "365,12/31/2015,04:00:36,1877,360036,5,50,33, 0x8B161753, 0x4205E5F9, 0x85D6149A, 0x801FA801, 0xFFCE0000, 0x022979C7, 0x5539D988, 0x80000000, 0x000001DC, 0x8CA00000"
   };

   ObsID oidCNAV(ObsID::otNavMsg, ObsID::cbL2, ObsID::tcC2LM); 
   for (int i1=0; i1<MSG_COUNT_CNAV; i1++)
   {
      vector<string> words = StringUtils::split(sv63_CNAV[i1],',');

      int gpsWeek = StringUtils::asInt(words[3]);
      double sow = StringUtils::asInt(words[4]);
      CommonTime ct = GPSWeekSecond(gpsWeek, sow, TimeSystem::GPS);
      
      int prn = StringUtils::asInt(words[5]);
      SatID sid(prn, SatID::systemGPS);

      string rxString = "unk";

      PackedNavBits* pnb = new PackedNavBits(sid,oidCNAV,rxString,ct);

         // Load the raw data
         // Words 0-8 have 32 bits.
         // Word 9 has 12 bits and they'll be "left-justified"
         // in the sense that the string will look like 0xXXX00000;
         // that is to say, 12 bits plus 20 bits of zero padding.
      int offset = 8;
      for (int i=0; i<10; i++)
      {
         int ndx = i + offset;
         string hexStr = StringUtils::strip(words[ndx]);
         string::size_type n = hexStr.find("x"); 
         hexStr = hexStr.substr(n+1);
         unsigned long bits = StringUtils::x2uint(hexStr);
         if (i<9) pnb->addUnsignedLong(bits,32,1);
         else
         {
            bits >>= 20;
            pnb->addUnsignedLong(bits,12,1); 
         }
      }
      pnb->trimsize();

      messageList.push_back(pnb);

      CNavFilterData cnavFilt(pnb);
      cNavList.push_back(cnavFilt);     
   }

   return 0;
}


unsigned CNavFilter_T ::
noFilterTest()
{
   TUDEF("CNav NoFilter", "validate");

   NavFilterMgr mgr;
   unsigned long count = 0;

   list<CNavFilterData>::iterator it; 
   for (it=cNavList.begin(); it!=cNavList.end(); it++)
   {
      CNavFilterData& fd = *it;
      gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
         // We could do an assert for each record but that would be
         // stupid. Just compare the final counts.
      count += l.size();
   }
   int expected = cNavList.size();
   TUASSERTE(unsigned long, expected, count);

   TURETURN();
}


// this should be executed before any other filter tests are used as
// it will upright all the data in memory.
unsigned CNavFilter_T ::
testCNavCook()
{
   TUDEF("CNavCookFilter", "validate");

   NavFilterMgr mgr;
   unsigned long count = 0;
   CNavCookFilter filtCook;

   mgr.addFilter(&filtCook);

   list<CNavFilterData>::iterator it; 
   for (it=cNavList.begin(); it!=cNavList.end(); it++)
   {
      CNavFilterData& fd = *it;
      gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
         // We could do an assert for each record but that would be
         // stupid. Just compare the final counts.
      count += l.size();
   }
   int expected = cNavList.size();
   TUASSERTE(unsigned long, expected, count);

   TURETURN();
}


unsigned CNavFilter_T ::
testCNavParity()
{
   TUDEF("CNavParityFilter", "validate");

   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   unsigned long acceptCount = 0;
   CNavParityFilter filtParity;

   mgr.addFilter(&filtParity);

   list<CNavFilterData>::iterator it; 
   for (it=cNavList.begin(); it!=cNavList.end(); it++)
   {
      CNavFilterData& fd = *it;
      gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
      gpstk::NavFilter::NavMsgList::const_iterator nmli;

      acceptCount += l.size();
      rejectCount += filtParity.rejected.size();
   }
   int expected = cNavList.size();
   TUASSERTE(unsigned long, expected, acceptCount);
   TUASSERTE(unsigned long, 0, rejectCount);

   TURETURN();
}


unsigned CNavFilter_T ::
testCNavEmpty()
{
   TUDEF("CNavEmptyFilter", "validate");

   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   unsigned long acceptCount = 0;
   CNavEmptyFilter filtEmpty;

   mgr.addFilter(&filtEmpty);

   list<CNavFilterData>::iterator it; 
   for (it=cNavList.begin(); it!=cNavList.end(); it++)
   {
      CNavFilterData& fd = *it;
      gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
      acceptCount += l.size();
      rejectCount += filtEmpty.rejected.size();
   }
   int expected = cNavList.size();
   TUASSERTE(unsigned long, expected, acceptCount);
   TUASSERTE(unsigned long, 0, rejectCount);

      // Now test an empty message.   Create an empty message
      // by cloning the first message in the list (which is
      // known to be valid), then zeroing out the "payload".
      // Since CNavFilterData does not pay attention to 
      // whether the CRC is good or bad, we don't need to 
      // worry about that. 
      // The goal is to retain bits 1-38 and bits 277-300 
      // for a valid message while zeroing out bits 39-276.
   acceptCount = 0;
   rejectCount = 0; 
   CNavFilterData fd;
   list<PackedNavBits*>::iterator it2 = messageList.begin();
   PackedNavBits* p = *it2; 
   PackedNavBits* pnbEmptyMsg = p->clone();

   unsigned long bits_1_32 = p->asUnsignedLong(0,32,1);
   unsigned long bits_33_38 = p->asUnsignedLong(32,6,1);
   unsigned long bits_277_300 = p->asUnsignedLong(276,24,1);
   unsigned long alternating = 0xAAAAAAAA; 

   pnbEmptyMsg->reset_num_bits();
   pnbEmptyMsg->addUnsignedLong(bits_1_32,32,1);
   pnbEmptyMsg->addUnsignedLong(bits_33_38,6,1);
   unsigned long alternating26 = alternating >> 6; 
   pnbEmptyMsg->addUnsignedLong(alternating26,26,1);  // bits  39- 64
   pnbEmptyMsg->addUnsignedLong(alternating,32,1);  // bits  65- 96
   pnbEmptyMsg->addUnsignedLong(alternating,32,1);  // bits  97-128
   pnbEmptyMsg->addUnsignedLong(alternating,32,1);  // bits 129-160
   pnbEmptyMsg->addUnsignedLong(alternating,32,1);  // bits 161-192
   pnbEmptyMsg->addUnsignedLong(alternating,32,1);  // bits 193-224
   pnbEmptyMsg->addUnsignedLong(alternating,32,1);  // bits 225-256
   unsigned long alternating20 = alternating >> 12; 
   pnbEmptyMsg->addUnsignedLong(alternating20,20,1);  // bits 257-276
   pnbEmptyMsg->addUnsignedLong(bits_277_300,24,1);

   pnbEmptyMsg->trimsize();

   CNavFilterData fdEmpty(pnbEmptyMsg);
   gpstk::NavFilter::NavMsgList l = mgr.validate(&fdEmpty);
   acceptCount += l.size();
   rejectCount += filtEmpty.rejected.size();

      // Now build a message with zeroes in the payload
   PackedNavBits* pnbZeroMsg = p->clone();
   unsigned long allZero = 0x00000000;
   pnbZeroMsg->reset_num_bits();
   pnbZeroMsg->addUnsignedLong(bits_1_32,32,1);
   pnbZeroMsg->addUnsignedLong(bits_33_38,6,1);
   pnbZeroMsg->addUnsignedLong(allZero,26,1);  // bits  39- 64
   pnbZeroMsg->addUnsignedLong(allZero,32,1);  // bits  65- 96
   pnbZeroMsg->addUnsignedLong(allZero,32,1);  // bits  97-128
   pnbZeroMsg->addUnsignedLong(allZero,32,1);  // bits 129-160
   pnbZeroMsg->addUnsignedLong(allZero,32,1);  // bits 161-192
   pnbZeroMsg->addUnsignedLong(allZero,32,1);  // bits 193-224
   pnbZeroMsg->addUnsignedLong(allZero,32,1);  // bits 225-256
   pnbZeroMsg->addUnsignedLong(allZero,20,1);  // bits 257-276
   pnbZeroMsg->addUnsignedLong(bits_277_300,24,1);

   pnbZeroMsg->trimsize();

   CNavFilterData fdZero(pnbZeroMsg);
   l = mgr.validate(&fdZero);
   acceptCount += l.size();
   rejectCount += filtEmpty.rejected.size();

   TUASSERTE(unsigned long, 0, acceptCount);
   TUASSERTE(unsigned long, 2, rejectCount);
   TURETURN();
}


unsigned CNavFilter_T ::
testCNavTOW()
{
   TUDEF("CNavTOWFilter", "validate");

   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   unsigned long acceptCount = 0; 
   CNavTOWFilter filtTOW;

   mgr.addFilter(&filtTOW);

   list<CNavFilterData>::iterator it; 
   for (it=cNavList.begin(); it!=cNavList.end(); it++)
   {
      CNavFilterData& fd = *it;
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
      gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
      rejectCount += filtTOW.rejected.size();
      acceptCount += l.size();
/*
      if (!filtTLMHOW.rejected.empty())
         cerr << "filter " << i << " tlm/how" << endl;
*/
   }
   int expected = cNavList.size();
   TUASSERTE(unsigned long, expected, acceptCount);
   TUASSERTE(unsigned long, 0,  rejectCount);

   TURETURN();
}


unsigned CNavFilter_T ::
testCNavCombined()
{
   TUDEF("CNavFilter", "validate");

   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   CNavParityFilter filtParity;
   CNavEmptyFilter filtEmpty;
   CNavTOWFilter filtTOW;

   mgr.addFilter(&filtParity);
   mgr.addFilter(&filtEmpty);
   mgr.addFilter(&filtTOW);

   list<CNavFilterData>::iterator it; 
   for (it=cNavList.begin(); it!=cNavList.end(); it++)
   {
      CNavFilterData& fd = *it;
/*
      cerr << "checking " << i << endl;
      if (i == 2196)
      {
         for (unsigned sfword = 0; sfword < 10; sfword++)
            cerr << " " << hex << setw(8) << dataLNAV[i].sf[sfword] << dec;
         cerr << endl;
      }
*/
      gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
         // if l is empty, the subframe was rejected.. 
      rejectCount += l.empty();
/*
      if (l.empty())
         cerr << "filter " << i << " combined" << endl;
*/
   }
   int expected = cNavList.size();
   TUASSERTE(unsigned long, 0, rejectCount);

   TURETURN();
}


int main()
{
   unsigned errorTotal = 0;

   CNavFilter_T testClass;

   errorTotal += testClass.loadData();
   errorTotal += testClass.noFilterTest();
   errorTotal += testClass.testCNavCook();
   errorTotal += testClass.testCNavParity();
   errorTotal += testClass.testCNavEmpty();
   errorTotal += testClass.testCNavTOW();
   errorTotal += testClass.testCNavCombined();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
