//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "TestUtil.hpp"
#include "NavFilterMgr.hpp"
#include "CNavFilterData.hpp"
#include "CNavCrossSourceFilter.hpp"
#include "CNav2SanityFilter.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "NavFilter.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"
#include "TimeSystem.hpp"

using namespace std;
using namespace gpstk;


class CNav2Filter_T
{
public:
   CNav2Filter_T();
   ~CNav2Filter_T();

   unsigned loadData();

      /// Test to make sure that with no filters, no data is removed
   unsigned noFilterTest();
      /// Test the CNAV-2 sanity filter
   unsigned testCNav2Sanity();
      /// Test the combination of sanity,and cross-source filters
   unsigned testCNav2Combined();

      // This is a list of the PackedNavBit messages that are created from the
      // static strings contained in the loadData( ) method.
   list<PackedNavBits*> messageList;

      // This is a parallel list of CNavFilterData objects created from 
      // the PackedNavBit objects.   These are all believed to be valid. 
   list<CNavFilterData> cNavList;  
};

//-------------------------------------------------------------------
CNav2Filter_T ::
CNav2Filter_T()
{
   // In the case of this test.  This method doesn't have to do anything.
}


CNav2Filter_T ::
~CNav2Filter_T()
{
   for (auto& mli : messageList)
   {
      delete mli;
   }
}


//-------------------------------------------------------------------
// At the time of the original writing, all these examples are
// hand-generated as no validated CNAV-2 data are available at this
// time.  The following assumputions are made:
//   1.) Message data are stored one frame to a PackedNavBits message.
//   2.) The TOI, ITOW, week number, PRN, and page number are generated
//       via the test algorithm.  The remaining data are zero except where
//       modified to exercise the change detection. 
//   3.) The messages cycle through subframe 3 page 1 - subframe 3 page 6.
//       NOTE:  There is no reason to expect that this will be the
//       operational pattern.  The goal is to exercise all the
//       valid subframes. 
//
unsigned CNav2Filter_T ::
loadData()
{
   ObsID oidCNAV2(ObservationType::NavMsg, CarrierBand::L1, TrackingCode::L1CDP); 
   unsigned PRN = 1;
   SatID sid(PRN,SatelliteSystem::GPS);
   CommonTime ct = GPSWeekSecond(2000,86400.0);
   double MESSAGE_INTERVAL = 18.0;      // CNAV-2 message rate
   size_t MAX_PAGE_NUMBER = 6;
   unsigned pageNum = 1; 
   size_t NUMBER_MESSAGES = 12; 
   string RX_STRING = "unk";
   unsigned TWO_HOURS = 7200;

   for (unsigned i=0; i<NUMBER_MESSAGES; i++)
   {

      PackedNavBits* pnb = new PackedNavBits(sid,oidCNAV2,RX_STRING,ct);

      unsigned week = static_cast<GPSWeekSecond>(ct).week;
      double dSOW = static_cast<GPSWeekSecond>(ct).sow;
      unsigned SOW = (unsigned) dSOW;
      SOW += 18;
      if (SOW>=FULLWEEK)
         SOW -= FULLWEEK;
      unsigned ITOW = SOW / TWO_HOURS;
      unsigned TOI  = SOW - (ITOW * TWO_HOURS);
      TOI = TOI / 18;     
      unsigned long zeroes = 0x00000000L;  

      pnb->addUnsignedLong(TOI,9,1);     // Subframe 1
                                         // Subframe 2
      pnb->addUnsignedLong(week,13,1);   // Week
      pnb->addUnsignedLong(ITOW,8,1);    // ITOW
      for (int i1=0;i1<18;i1++)          // Add 579 zeroes to pad data space.
      {                                  // That's 18 32 bits words plus three bits.
         pnb->addUnsignedLong(zeroes,32,1);
      }
      pnb->addUnsignedLong(zeroes,3,1);
                                         // Subframe 3
      pnb->addUnsignedLong(PRN,8,1);     // PRN ID
      pnb->addUnsignedLong(pageNum,6,1); // subframe ID
      for (int i1=0;i1<8;i1++)  // Add 260 zeroes to pad data space.
      {                                  // That's 8 32 bits, plus 4 bits 
         pnb->addUnsignedLong(zeroes,32,1);
      }
      pnb->addUnsignedLong(zeroes,4,1);

      pnb->trimsize();

      messageList.push_back(pnb);

      CNavFilterData cnavFilt(pnb);
      cNavList.push_back(cnavFilt);     

      pageNum++;
      if (pageNum>MAX_PAGE_NUMBER)
         pageNum = 1; 
      ct += MESSAGE_INTERVAL; 
   }

   return 0;
}

//-------------------------------------------------------------------
unsigned CNav2Filter_T ::
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

//-------------------------------------------------------------------
unsigned CNav2Filter_T ::
testCNav2Sanity()
{
   TUDEF("CNav2SanityFilter", "validate");

   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   unsigned long acceptCount = 0;
   CNav2SanityFilter filtSanity;

   mgr.addFilter(&filtSanity);

      // Test with valid data
   list<CNavFilterData>::iterator it; 
   for (it=cNavList.begin(); it!=cNavList.end(); it++)
   {
      CNavFilterData& fd = *it;
      gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
      gpstk::NavFilter::NavMsgList::const_iterator nmli;

      acceptCount += l.size();
      rejectCount += filtSanity.rejected.size();
   }
   int expected = cNavList.size();
   TUASSERTE(unsigned long, expected, acceptCount);
   TUASSERTE(unsigned long, 0, rejectCount);

      // Clone a valid message, modify the ITOW, and 
      // verify that the filter rejects the data.
   acceptCount = 0;
   rejectCount = 0; 
   list<PackedNavBits*>::iterator it2 = messageList.begin();
   PackedNavBits* p = *it2; 

      // There are 84 two-hour epochs in a week.  That's 54 base 16.
      // An ITOW of a solid 7 bits would be too large.
   PackedNavBits* pnb = p->clone();
   unsigned long wordBad = 0x0000003F;
   pnb->insertUnsignedLong(wordBad,22,8); 
   CNavFilterData fd(pnb);
   gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
   acceptCount = l.size();
   rejectCount = filtSanity.rejected.size();
   TUASSERTE(unsigned long, 0, acceptCount);
   TUASSERTE(unsigned long, 1, rejectCount);
   delete pnb;

      // There are 400 18s intervals in a two-hour period.
      // 0x190.
      // An TOI of a 0x1FF is too large.
   PackedNavBits* pnb2 = p->clone();
   wordBad = 0x000001FF;
   pnb2->insertUnsignedLong(wordBad,0,9); 
   CNavFilterData fd2(pnb2);
   gpstk::NavFilter::NavMsgList l2 = mgr.validate(&fd2);
   acceptCount = l2.size();
   rejectCount = filtSanity.rejected.size();
   TUASSERTE(unsigned long, 0, acceptCount);
   TUASSERTE(unsigned long, 1, rejectCount);
   delete pnb2;

      // The current week is 2000. Reset it to 1999
      // and verify a failure.
      // 2000 = 0x7CF.
   PackedNavBits* pnb3 = p->clone();
   wordBad = 0x000007CF;
   pnb3->insertUnsignedLong(wordBad,9,13); 
   CNavFilterData fd3(pnb3);
   gpstk::NavFilter::NavMsgList l3 = mgr.validate(&fd3);
   acceptCount = l3.size();
   rejectCount = filtSanity.rejected.size();
   TUASSERTE(unsigned long, 0, acceptCount);
   TUASSERTE(unsigned long, 1, rejectCount);
   delete pnb3;

      // A PRN of 0 doesn't match the test value.
   PackedNavBits* pnb4 = p->clone();
   wordBad = 0x00000000;
   unsigned long bitOffset = 609;
   pnb4->insertUnsignedLong(wordBad,bitOffset,8); 
   CNavFilterData fd4(pnb4);
   gpstk::NavFilter::NavMsgList l4 = mgr.validate(&fd4);
   acceptCount = l4.size();
   rejectCount = filtSanity.rejected.size();
   TUASSERTE(unsigned long, 0, acceptCount);
   TUASSERTE(unsigned long, 1, rejectCount);
   delete pnb4;

      // A page number of 0 doesn't match the test value.
   PackedNavBits* pnb5 = p->clone();
   bitOffset = 609 + 8;
   pnb5->insertUnsignedLong(wordBad,bitOffset,6); 
   CNavFilterData fd5(pnb5);
   gpstk::NavFilter::NavMsgList l5 = mgr.validate(&fd5);
   acceptCount = l5.size();
   rejectCount = filtSanity.rejected.size();
   TUASSERTE(unsigned long, 0, acceptCount);
   TUASSERTE(unsigned long, 1, rejectCount);
   delete pnb5;

   TURETURN();
}

/*
//-------------------------------------------------------------------
unsigned CNav2Filter_T ::
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
      gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
      rejectCount += filtTOW.rejected.size();
      acceptCount += l.size();
   }
   int expected = cNavList.size();
   TUASSERTE(unsigned long, expected, acceptCount);
   TUASSERTE(unsigned long, 0,  rejectCount);

   // --- NOW GENERATE SOME INVALID MESSAGES AND VERIFY THAT 
   // --- THEY ARE REJECTED
      // Modify a message to have an invalid TOW count.
   acceptCount = 0;
   rejectCount = 0; 
   CNavFilterData fd;
   list<PackedNavBits*>::iterator it2 = messageList.begin();
   PackedNavBits* p = *it2; 

      // Message with invalid too large) TOW
   PackedNavBits* pnbBadTOWMsg = p->clone();
   unsigned long badTOW = 604800; 
   pnbBadTOWMsg->insertUnsignedLong(badTOW, 20, 17, 6);

      // Message with invalid preamble
   PackedNavBits* pnbBadPreamble = p->clone();
   unsigned long badPre = 0;
   pnbBadPreamble->insertUnsignedLong(badPre, 0, 8);

   acceptCount = 0; 
   rejectCount = 0; 
   CNavFilterData fdBadTOW(pnbBadTOWMsg);
   gpstk::NavFilter::NavMsgList l = mgr.validate(&fdBadTOW);
   rejectCount += filtTOW.rejected.size();
   acceptCount += l.size();
   
   CNavFilterData fdBadPreamble(pnbBadPreamble);
   l = mgr.validate(&fdBadPreamble);
   rejectCount += filtTOW.rejected.size();
   acceptCount += l.size();

      // Bad Message Type tests
      // Test the invalid MT immediately above/below the valid ranges.
   unsigned long badMT[] = { 9, 16, 29, 40}; 
   int badMTCount = 4; 
   PackedNavBits* pnbBadMT = p->clone();
   for (int i=0; i<badMTCount; i++)
   {
      pnbBadMT->insertUnsignedLong(badMT[i], 14, 6);
      CNavFilterData fdBadMT(pnbBadMT);
      l = mgr.validate(&fdBadMT);
      rejectCount += filtTOW.rejected.size();
      acceptCount += l.size();
   }

   unsigned long expReject = 2 + badMTCount; 
   TUASSERTE(unsigned long, 0, acceptCount);
   TUASSERTE(unsigned long, expReject,  rejectCount);

   TURETURN();
}
*/
//-------------------------------------------------------------------
unsigned CNav2Filter_T ::
testCNav2Combined()
{
   TUDEF("CNavFilter-Combined", "validate");

   NavFilterMgr mgr;
   unsigned long rejectCount = 0;
   CNav2SanityFilter filtSanity;

   mgr.addFilter(&filtSanity);

   list<CNavFilterData>::iterator it; 
   for (it=cNavList.begin(); it!=cNavList.end(); it++)
   {
      CNavFilterData& fd = *it;
      gpstk::NavFilter::NavMsgList l = mgr.validate(&fd);
         // if l is empty, the subframe was rejected. 
      rejectCount += l.empty();
   }
   int expected = cNavList.size();
   TUASSERTE(unsigned long, 0, rejectCount);
   TURETURN();
}

//-------------------------------------------------------------------
int main()
{
   unsigned errorTotal = 0;

   CNav2Filter_T testClass;

   errorTotal += testClass.loadData();
   errorTotal += testClass.testCNav2Sanity();
   errorTotal += testClass.testCNav2Combined();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
