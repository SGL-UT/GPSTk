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
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//============================================================================
 /*********************************************************************
*
*  Test program for gpstk/ext/lib/GNSSEph/OrbAlmStore
*
*********************************************************************/
#include <iostream>
#include <fstream>

#include "CivilTime.hpp"
#include "Exception.hpp"
#include "GPSWeekSecond.hpp"
#include "NavID.hpp"
#include "OrbAlm.hpp"
#include "OrbAlmGen.hpp"
#include "OrbAlmStore.hpp"
#include "SystemTime.hpp"
#include "TimeString.hpp"
#include "TimeSystem.hpp"

#include "build_config.h"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;


class OrbAlmStore_T
{
public:

      // Following structure is used by both the tests for the find( )
      // method and the deriveLastXmit( ) method.
      //
      // For find( ), the names of the members are pretty self-explantory
      //  subjID - The satellite of interest for the find( )
      //  expectPass - True if the find( ) is expected to return with 
      //               a response (as opposed to throwing an exception)
      //  testTime - The time of interest for the find( )
      //  considerEff - Take the almanac period of effectivity into
      //             account.
      //  expBV - The beginValid time that the return should have. 
      //          This is only relevant if expectPass==true.
      //  xmitID - In the case of the find( ) method for a particular
      //          transmitting SV, this is the transmitting satellite
      //          of interest.  If left as invalid the simpler find( )
      //          that looks among unique almanacs for all transmitting
      //          SVs is used.
      //
      // For deriveLastXmit( ), the definitions vary as follows:
      //  subjID - The satellite of interest.
      //  expectPass - True if the deriveLastXmit( ) is expected to return with 
      //               a response (as opposed to throwing an exception)
      //  testTime - The time of interest for the find( ).  The 
      //             subjID and the testTime are used in order to 
      //             determine the particular almanac for which the
      //             last transmit time is to be derived.
      //  considerEff - Alway true for these tests.
      //  expBV - The lastXmitTime that should be returned. 
      //          This is only relevant if expectPass==true.
      //  xmitID - Unused.
   typedef struct PassFailData
   {
      SatID subjID;
      bool expectPass;
      CommonTime testTime;
      bool considerEff;
      CommonTime expBV;       // beginValid time for expected return
      SatID xmitID;

      PassFailData(): 
         subjID(SatID()),
         expectPass(false), 
         testTime(CommonTime::END_OF_TIME), 
         considerEff(false), 
         expBV(CommonTime::END_OF_TIME),
         xmitID(SatID())
         { }

         // Expect to pass, so include expected return.
      PassFailData(const SatID& subj, const CommonTime& testT, const bool eff, 
                   const CommonTime& exp, const SatID& sidr=SatID()):
         subjID(subj),
         expectPass(true),
         testTime(testT),
         considerEff(eff),
         expBV(exp),
         xmitID(sidr)
         { }

         // Expect to fail, so do NOT include expected return.
      PassFailData(const SatID& subj, const CommonTime& testT, const bool eff, const SatID& sidr=SatID()):
         subjID(subj),
         expectPass(false),
         testTime(testT),
         considerEff(eff),
         expBV(CommonTime::END_OF_TIME),
         xmitID(sidr)
         { }

   } PassFailData;

   OrbAlmStore_T();

   void init();

   unsigned findEmptyTest();
   unsigned createAndDump();
   void testFind(const PassFailData& pfd, 
                       OrbAlmStore& oas,
                       TestUtil& testFramework);
   void testFindExpectingPass(const PassFailData& pfd, 
                                    OrbAlmStore& oas,
                                    TestUtil& testFramework);
   void testFindExpectingFail(const PassFailData& pfd,
                                    OrbAlmStore& oas,
                                    TestUtil& testFramework);
   void testLastXmit(const PassFailData& pfd, 
                       OrbAlmStore& oas,
                       TestUtil& testFramework);
   void testLastXmitExpectingPass(const PassFailData& pfd, 
                                    OrbAlmStore& oas,
                                    TestUtil& testFramework);
   void testLastXmitExpectingFail(const PassFailData& pfd,
                                    OrbAlmStore& oas,
                                    TestUtil& testFramework);

   void setUpLNAV();
   void setUpCNAV();
   void setUpBDS();
   void setUpGLO();
   gpstk::PackedNavBits getPnbLNav(const gpstk::ObsID& oidr,
                                   const std::string& str)
             throw(gpstk::InvalidParameter);
   gpstk::PackedNavBits getPnbCNav(const gpstk::ObsID& oidr,
                                   const std::string& str)
             throw(gpstk::InvalidParameter);

      // The setUpXXX() methods above exist to set up the following
      // members
   list<PackedNavBits> dataList; 
   string typeDesc;
   CommonTime initialCT;
   CommonTime finalCT;

      // There are three size( ) values to be tested.
      //   sizeTotal - The total number of almanac objects expected, both in the
      //               subject SV store and the transmit store.
      //   sizeSubj - The number of almanac objects expected in the subject
      //               almanac table.
      //   sizeXmit - The number of almanac object expected in the transmit
      //                almanac table. 
      // It is not enough to simply count the number of object read in as some
      // will be redundant and not stored. 
      // 
   unsigned int sizeTotal;
   unsigned int sizeSubj;
   unsigned int sizeXmit;

   ofstream out; 

      // For testing the test
   int debugLevel;   

      // List of PassFailData objects that contain definitions for sepcific find( ) tests.
   list<PassFailData> pfList;

      // List of PassFailData object that contain definition for specific 
      // deriveLastXmit( ) tests.
   list<PassFailData> lastXmitList;
};

OrbAlmStore_T::
OrbAlmStore_T()
{
   debugLevel = 0; 
   init();
}

unsigned OrbAlmStore_T::
findEmptyTest()
{
   TUDEF("OrbAlmStore", "find");
   try
   {
         // test that an exception is thrown for a completely empty OrbAlmStore
      OrbAlmStore oas;
      SatID s(1,SatID::systemGPS);
      SystemTime now;
      CommonTime ct(now);
      ct.setTimeSystem(TimeSystem::Any);
         // first try with useEffectivity on
      try
      {
         oas.find(s, ct, true);
         TUFAIL("Expected an InvalidRequest exception to be thrown");
      }
      catch (gpstk::InvalidRequest &exc)
      {
         TUPASS("Expected exception");
      }
      catch (gpstk::Exception &exc)
      {
         cerr << exc;
         TUFAIL("Unexpected gpstk exception");
      }
      catch (std::exception &exc)
      {
         cerr << exc.what() << endl;
         TUFAIL("Unexpected std c++ exception");
      }
         // then try with useEffectivity off
      try
      {
         oas.find(s, ct, false);
         TUFAIL("Expected an InvalidRequest exception to be thrown");
      }
      catch (gpstk::InvalidRequest &exc)
      {
         TUPASS("Expected exception");
      }
      catch (gpstk::Exception &exc)
      {
         cerr << exc;
         TUFAIL("Unexpected gpstk exception");
      }
      catch (std::exception &exc)
      {
         cerr << exc.what() << endl;
         TUFAIL("Unexpected std c++ exception");
      }
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   
   TURETURN();
}

unsigned OrbAlmStore_T::
createAndDump()
{
   string currMethod = typeDesc + " create/access an OrbAlmStore";
   TUDEF("OrbAlmStore",currMethod);

      // Open an output stream specific to this navigation message type
   std::string fs = getFileSep(); 
   std::string tf(getPathTestTemp()+fs);
   std::string tempFile = tf + "test_output_OrbAlmStore_T_" +
                         typeDesc+".out";
   out.open(tempFile.c_str(),std::ios::out);
   if (!out)
   {
      stringstream ss;
      ss << "Could not open file " << tempFile << " for output.";
      TUFAIL(ss.str());
      TURETURN();
   }

      // All the navigation message data will be placed here. 
   OrbAlmStore oas;
   oas.setDebugLevel(debugLevel); 

   bool passed = true;
   unsigned addSuccess = 0; 
   unsigned addXmit    = 0; 
   unsigned addSubj    = 0; 
   list<PackedNavBits>::const_iterator cit;
   for (cit=dataList.begin();cit!=dataList.end();cit++)
   {
      try
      {
         const PackedNavBits& pnbr = *cit;
         unsigned short retval = oas.addMessage(pnbr);
         if (retval & OrbAlmStore::ADD_XMIT) { addXmit++; addSuccess++; }
         if (retval & OrbAlmStore::ADD_SUBJ) { addSubj++; addSuccess++; }
      }
      catch(gpstk::InvalidParameter ir)
      {
            // Dummy almanacs are not considered errors. 
         std::stringstream serror;
         serror << ir;
         string strError(serror.str());
         if (strError.find("dummy")==string::npos)
         {
            passed = false;
            std::stringstream ss;
            ss << "Load of OrbAlmStore failed." << std::endl;
            ss << ir; 
            TUFAIL(ss.str());
         }
      }
   }
   unsigned count = oas.size();
   if (count!=sizeTotal)
   {
      stringstream ss;
      ss << "Size of ObsSysStore incorrect after loading.  Expected " 
         << sizeTotal << " actual size " << count;
      TUFAIL(ss.str());
      passed = false; 
   }
   else
   {
      TUPASS("Combined size map sub-test passed.");
   }

   unsigned countSubj = oas.size(1);
   if (countSubj!=sizeSubj)
   {
      stringstream ss;
      ss << "Size of ObsSysStore(subject SVs) incorrect after loading.  Expected " 
         << sizeSubj << " actual size " << countSubj;
      TUFAIL(ss.str());
      passed = false; 
   }
   else
   {
      TUPASS("Subject almanac map size sub-test passed.");
   }

   unsigned countXmit = oas.size(2);
   if (countXmit!=sizeXmit)
   {
      stringstream ss;
      ss << "Size of ObsSysStore(xmit SVs) incorrect after loading.  Expected " 
         << sizeXmit << " actual size " << countXmit;
      TUFAIL(ss.str());
      passed = false; 
   }
   else
   {
      TUPASS("Transmit almanac map size sub-test passed.");
   }

//--- Test the isPresent( ) method --------------------------------
   currMethod = typeDesc + " OrbAlmStore.isPresent() "; 
   TUCSM(currMethod);
   SatID sidT1(1,SatID::systemGPS);
   if (oas.isPresent(sidT1)) 
      TUPASS("");
   else
      TUFAIL("Failed to find PRN 1 in store"); 
   
   SatID sidT2(33,SatID::systemGPS);
   if (oas.isPresent(sidT2))
      TUFAIL("Reported PRN 33 as present (which is not true)");
   else 
      TUPASS("");
      
//--- Test the getXXXTime( ) methods -------------------------
   currMethod = typeDesc + " OrbAlmStore.getXxxxTime() "; 
   TUCSM(currMethod);
   try
   {
      const CommonTime& initialTestT = oas.getInitialTime();
      if (initialTestT==initialCT) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Incorrect initial time.  Expected "
            << printTime(initialCT,"%02m/%02d/%4Y %02H:%02M:%02S %P")
            << " found "
            << printTime(initialTestT,"%02m/%02d/%4Y %02H:%02M:%02S %P");
         TUFAIL(ss.str());
      }
   }
   catch (InvalidRequest ir)
   {
     stringstream ss;
     ss << "Unexpected exception." << ir;
     TUFAIL(ss.str());
   }

   try
   {
      const CommonTime& finalTestT = oas.getFinalTime();
      if (finalTestT==finalCT) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Incorrect final time.  Expected "
            << printTime(finalCT,"%02m/%02d/%4Y %02H:%02M:%02S %P")
            << " found "
            << printTime(finalTestT,"%02m/%02d/%4Y %02H:%02M:%02S %P");
         InvalidRequest ir(ss.str());
         TUFAIL(ss.str());
      }
   }
   catch (InvalidRequest ir)
   {
     stringstream ss;
     ss << "Unexpected exception." << ir;
     TUFAIL(ss.str());
   }

   //--- Test the find( ) method --------------------------------
   currMethod = typeDesc + " OrbAlmStore.find() "; 
   TUCSM(currMethod);

   list<PassFailData>::const_iterator citp;
   for (citp=pfList.begin();citp!=pfList.end();citp++)
   {
      const PassFailData& pfd = *citp;
      testFind(pfd,oas,testFramework);
   }

   //--- Test the deriveLastXmit( ) method --------------------------------
   currMethod = typeDesc + " OrbAlmStore.deriveLastXmit() "; 
   TUCSM(currMethod);

   for (citp=lastXmitList.begin();citp!=lastXmitList.end();citp++)
   {
      const PassFailData& pfd = *citp;
      testLastXmit(pfd,oas,testFramework);
   }

   //--- Dump the store ----------------------
   currMethod = typeDesc + " OrbAlmStore.dump()";
   TUCSM(currMethod);
   oas.dump(out);

      // Dump terse (one-line) summaries
   oas.dump(out,1);

      // Dump all contents
   oas.dump(out,2);

      // Dump terse in time order
   oas.dump(out,3);

   //--- Test getXVT( ) and validity 
   //  Verify that getXvt( ) will return values for times beyond the fit interval
   //  of the alamanc data.   
   //
   //  Verify that getXvtWithinValid( ) will throw an exeption in this condition. 
   //         Expect                Expect   Test             
   //         getXvt  getXvt_WithinValid()   Time                Comment
   //      1.   True                 False   12/31/15 00:00:00   Earlier than fit interval
   //      2.   True                  True   12/31/15 12:00:00   Within fit interval
   //      3.   True                 False   01/31/16 00:00:00   Later than fit interval   currMethod = typeDesc + " getXvt()";
   currMethod = typeDesc + " OrbAlmStore.getXvt()";
   TUCSM(currMethod);

   SatID sidXvt(1,SatID::systemGPS);
   CommonTime test1 = CivilTime(2015,12,31,00,00,00,TimeSystem::GPS);
   CommonTime test2 = CivilTime(2015,12,31,12,00,00,TimeSystem::GPS);
   CommonTime test3 = CivilTime(2016, 1,31,00,00,00,TimeSystem::GPS); 
   vector<CommonTime> ctVector;
   ctVector.push_back(test1);
   ctVector.push_back(test2);
   ctVector.push_back(test3);

      // First, verify getXvt( ) succeeds with out-of-fit-interval times
   Xvt saveXvt;
   for (int i=0;i<3;i++)
   {
      try
      {
         Xvt xvt1 = oas.getXvt(sidXvt, ctVector[i]);
         stringstream ss;
         ss << "getXvt() succeeded for time ";
         switch(i)
         {
            case 0: ss << " earlier than fit interval."; break;
            case 1: ss << " within fit interval."; saveXvt = xvt1; break;
            case 2: ss << " later than fit interval."; break;
         }
         TUPASS(ss.str());
      }
      catch(InvalidRequest ir)
      {
         stringstream ss;
         ss << "getXvt() failed for time ";
         switch(i)
         {
            case 0: ss << " earlier than fit interval."; break;
            case 1: ss <<" within fit interval."; break;
            case 2: ss <<" later than fit interval."; break;
         }
         ss << endl << ir;
         TUFAIL(ss.str()); 
      }
   }
      // Next verify getXvt_WithinValid( ) fails in 2-of-3 cases. 
   for (int i=0;i<3;i++)
   {
      try
      {
         Xvt xvt1 = oas.getXvt_WithinValidity(sidXvt, ctVector[i]);
         stringstream ss;
         switch(i)
         {
            case 0: 
               ss << "getXvt_WithinValidity() succeeded (correctly) for time earlier than fit interval."; 
               TUFAIL(ss.str()); 
               break;
            case 1: 
               ss << "getXvt_WithinValidity() succeeded for time within fit interval."; 
               TUPASS(ss.str());
               if (saveXvt.x[0]     != xvt1.x[0] ||
                   saveXvt.x[1]     != xvt1.x[1] ||
                   saveXvt.x[2]     != xvt1.x[2] ||
                   saveXvt.v[0]     != xvt1.v[0] ||
                   saveXvt.v[1]     != xvt1.v[1] ||
                   saveXvt.v[2]     != xvt1.v[2] ||
                   saveXvt.clkbias  != xvt1.clkbias ||
                   saveXvt.clkdrift != xvt1.clkdrift)
               {
                  TUFAIL("getXvt() and getXvt_WithinValidity() did not return matching results.");
               }
               break;
            case 2: 
               ss << "getXvt_WithinValidity() succeeded (correctly) for time later than fit interval."; 
               TUFAIL(ss.str()); 
               break;
         }
      }
      catch(InvalidRequest ir)
      {
         stringstream ss;
         ss << "getXvt() failed for time ";
         switch(i)
         {
            case 0: 
               ss << "getXvt_WithinValid() failed (correctly) for time earlier than fit interval."; 
               TUPASS(ss.str());
               break;
            case 1: 
               ss << "getXvt_WithinValid() failed for time within fit interval."; 
               TUFAIL(ss.str()); 
               break;
            case 2: 
               ss << "getXvt_WithinValid() failed (correctly) for time later than fit interval."; 
               TUPASS(ss.str());
               break;
         }
      }
   }

      /* This checks for a situation where the OrbAlmStore top-level
       * map has satellites mapped to empty maps.  Prior to addressing
       * this issue, it would seg fault due to trying to dereference
       * an "end" iterator value. */
   currMethod = typeDesc + " OrbAlmStore.edit()";
   TUCSM(currMethod);
   CivilTime editTime(2015,12,31,12,28,55,TimeSystem::GPS);
   TUCATCH(oas.edit(editTime));
   SystemTime now;
   CommonTime ct(now);
   ct.setTimeSystem(TimeSystem::Any);
   for (unsigned prn = 1; prn <= 32; prn++)
   {
      const OrbAlm *foo = (const OrbAlm*)-1;
      SatID sidee(prn,SatID::systemGPS);
      try
      {
         foo = oas.find(sidee, ct);
         TUFAIL("find should throw an exception");
      }
      catch (gpstk::InvalidRequest)
      {
         TUPASS("find should throw an exception");
      }
   }

   //--- Clear the store ----------------------
   currMethod = typeDesc + " OrbAlmStore.clear()";
   TUCSM(currMethod);

   oas.clear();
   if (oas.size()!=0)
   {
      TUFAIL("Failed to entirely clear OrbAlmStore.");
   }
   else TUPASS("");

   out.close();

   TURETURN();
}

//---------------------------------------------------------------------------------
void OrbAlmStore_T::
testFind(const PassFailData& pfd, 
               OrbAlmStore& oas,
               TestUtil& testFramework)
{
   if (pfd.expectPass) 
      testFindExpectingPass(pfd,oas,testFramework);
   else 
      testFindExpectingFail(pfd,oas,testFramework);
}

//---------------------------------------------------------------------------------
// Test OrbAlmStore.find( ) with a set of arguments that are expected to pass
void OrbAlmStore_T::
testFindExpectingPass(const PassFailData& pfd, 
                            OrbAlmStore& oas,
                            TestUtil& testFramework)
{
   try
   {
      const OrbAlm* p = oas.find(pfd.subjID, pfd.testTime, pfd.considerEff); 
      if (p)
      {
         if (p->beginValid==pfd.expBV) 
            TUPASS("");
         else
         {
            stringstream ss;
            ss << "Wrong object found.  Expected xmit time "
               << printTime(pfd.expBV,"%02H:%02M:%02S")
               << " found time " 
               << printTime(p->beginValid,"%02H:%02M:%02S"); 
            TUFAIL(ss.str());
         }
      }
      else
      {
         stringstream ss;
         ss << "Returned without a valid pointer.";
         TUFAIL(ss.str());         
      }
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << "Unexpected exception" << endl;
      ss << ir << endl;
      TUFAIL(ss.str());
   }
   return; 
}

//---------------------------------------------------------------------------------
// Test OrbAlmStore.find( ) with a set of arguments that are expected to fail
void OrbAlmStore_T::
testFindExpectingFail(const PassFailData& pfd, 
                            OrbAlmStore& oas,
                            TestUtil& testFramework)
{
   try
   {
      const OrbAlm* p = oas.find(pfd.subjID,pfd.testTime,pfd.considerEff); 
      stringstream ss;
      ss << "Failed to throw exception for time after all endValid times";
      TUFAIL(ss.str());
   }
   catch (InvalidRequest)
   {
      TUPASS("");
   }
   return;
}

//---------------------------------------------------------------------------------
void OrbAlmStore_T::
testLastXmit(const PassFailData& pfd, 
               OrbAlmStore& oas,
               TestUtil& testFramework)
{
   if (pfd.expectPass) 
      testLastXmitExpectingPass(pfd,oas,testFramework);
   else 
      testLastXmitExpectingFail(pfd,oas,testFramework);
}

//---------------------------------------------------------------------------------
// Test OrbAlmStore.deriveLastXmit( ) with a set of arguments that are expected to pass
void OrbAlmStore_T::
testLastXmitExpectingPass(const PassFailData& pfd, 
                                OrbAlmStore& oas,
                                TestUtil& testFramework)
{
   try
   {
         // First find the subject SV almanac
      const OrbAlm* p = oas.find(pfd.subjID, pfd.testTime); 
      if (p)
      {
         const CommonTime ct = oas.deriveLastXmit(p);
         if (ct==pfd.expBV) 
            TUPASS("");
         else
         {
            stringstream ss;
            ss << "Expected lastXmit time "
               << printTime(pfd.expBV,"%02m/%02d/%4Y %02H:%02M:%02S")
               << " found time " 
               << printTime(ct,"%02m/%02d/%4Y %02H:%02M:%02S"); 
            TUFAIL(ss.str());
         }
      }
      else
      {
         stringstream ss;
         ss << "Returned without a valid pointer.";
         TUFAIL(ss.str());         
      }
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << "Unexpected exception" << endl;
      ss << ir << endl;
      TUFAIL(ss.str());
   }
   return; 
}

//---------------------------------------------------------------------------------
// Test OrbAlmStore.deriveLastXmit( ) with a set of arguments that are expected to fail
void OrbAlmStore_T::
testLastXmitExpectingFail(const PassFailData& pfd, 
                                OrbAlmStore& oas,
                                TestUtil& testFramework)
{
   try
   {
         // Could fail on find( ) call.  If there's no object for
         // this subject SV, then can't find one. 
      const OrbAlm* p = oas.find(pfd.subjID,pfd.testTime);

         // IF an object is found, we SHOULD be able to 
         // find the lastXmit time.
      const CommonTime ct = oas.deriveLastXmit(p); 
      stringstream ss;
      ss << "Failed to throw expected exception for deriveLastXmit( ) test";
      TUFAIL(ss.str());
   }
   catch (InvalidRequest)
   {
      TUPASS("");
   }
   return;
}

//---------------------------------------------------------------------------------
void OrbAlmStore_T::
init()
{
   dataList.clear();
} 

//---------------------------------------------------------------------------------
void OrbAlmStore_T::
setUpLNAV()
{
   init();

      // Define state variables for creating a LNAV store
   typeDesc = "GPS_LNAV";
   initialCT = CivilTime(2015,12,31,00,00,24,TimeSystem::GPS);
   finalCT   = CivilTime(2016, 1, 5,21,50,24,TimeSystem::GPS);

      // Literals for LNAV test data 
   const unsigned short LNavExCount = 53;
   const std::string LNavEx[] =
   {
      "365,12/31/2015,00:00:00,1877,345600,1,63,100, 0x22C3550A, 0x1C2029AC, 0x35540023, 0x0EA56C31, 0x16E4B88E, 0x37CECD3F, 0x171242FF, 0x09D588A2, 0x0000023F, 0x00429930",
      "365,12/31/2015,00:00:06,1877,345606,1,63,200, 0x22C3550A, 0x1C204A3C, 0x09FDB732, 0x0BC06889, 0x3C5827D1, 0x3E08808B, 0x21A678CF, 0x0472285B, 0x0350F3B4, 0x15889F94",
      "365,12/31/2015,00:00:12,1877,345612,1,63,300, 0x22C3550A, 0x1C206BB4, 0x3FFAC4D5, 0x0CAD96FA, 0x3FFA09D3, 0x10F0C405, 0x06D1C4E4, 0x31C1B694, 0x3FEA6E36, 0x09FFA5F4",
      "365,12/31/2015,00:00:18,1877,345618,1,63,421, 0x22C3550A, 0x1C208C44, 0x1E7181C9, 0x1C2E68A2, 0x0F4507DA, 0x247093F0, 0x26C720E5, 0x07E00109, 0x0196E4A3, 0x1D588110",
      "365,12/31/2015,00:00:24,1877,345624,1,63,521, 0x22C3550A, 0x1C20ADCC, 0x156ED525, 0x1EFEDF83, 0x3F4DC035, 0x2843463D, 0x047D1075, 0x2D2F1B44, 0x3814F871, 0x2FBFF920",
      "365,12/31/2015,00:11:18,1877,346278,1,63,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054", 
      "365,12/31/2015,00:00:54,1877,345654,1,63,522, 0x22C3550A, 0x1C214D60, 0x158F6074, 0x1EFCBBAC, 0x3F4DC00A, 0x28461943, 0x0F1D81A5, 0x2B5870BF, 0x277908D8, 0x0DBFFC14",
      "365,12/31/2015,00:01:18,1877,345678,1,63,423, 0x22C3550A, 0x1C21CC74, 0x1F7F6C90, 0x051BB19F, 0x0448D0CA, 0x0DBF8BB3, 0x22743371, 0x094AC152, 0x326CF461, 0x1B130084",
      "365,12/31/2015,00:01:24,1877,345684,1,63,523, 0x22C3550A, 0x1C21EDFC, 0x15D54CB3, 0x1EC0B78B, 0x3F4F0019, 0x284362C1, 0x1A101F0F, 0x25558BFB, 0x1263ADDC, 0x3B3FF85C",
      "365,12/31/2015,00:01:48,1877,345708,1,63,424, 0x22C3550A, 0x1C226CB8, 0x1F9C515F, 0x17E7DFD9, 0x01EFC299, 0x394C2501, 0x103A6022, 0x268D815A, 0x38D116AB, 0x2AAAAAD0",
      "365,12/31/2015,00:01:54,1877,345714,1,63,524, 0x22C3550A, 0x1C228D84, 0x16083F59, 0x1EC174C3, 0x3F4E4029, 0x28432CBF, 0x24835394, 0x034BD570, 0x0C287E8D, 0x3F8005FC",
      "365,12/31/2015,00:02:18,1877,345738,1,63,425, 0x22C3550A, 0x1C230C58, 0x1FEE6CC4, 0x2AEAEEC0, 0x26A66A75, 0x2A666666, 0x26EEEE53, 0x2AEA4013, 0x0000003F, 0x0000006C",
      "365,12/31/2015,00:02:24,1877,345744,1,63,525, 0x22C3550A, 0x1C232DD0, 0x1CDED544, 0x00000FDE, 0x00000029, 0x00000016, 0x00000029, 0x00000016, 0x00000029, 0x000000E0",
      "365,12/31/2015,00:02:48,1877,345768,1,63,401, 0x22C3550A, 0x1C23ACC4, 0x1E7181C9, 0x1C2E68A2, 0x0F4507DA, 0x247093F0, 0x26C720E5, 0x07E00109, 0x0196E4A3, 0x3CD37120",
      "365,12/31/2015,00:02:54,1877,345774,1,63,501, 0x22C3550A, 0x1C23CDB4, 0x104A1B03, 0x1EC3752A, 0x3F52C00A, 0x284334F8, 0x04C97D73, 0x04F1B747, 0x0917642F, 0x004000C4",
      "365,12/31/2015,00:03:18,1877,345798,1,63,402, 0x22C3550A, 0x1C244CC8, 0x164A03ED, 0x1EC5DBEA, 0x3F56803C, 0x28431268, 0x2F65B770, 0x0716C3D8, 0x37EDFFB5, 0x3CFFF7D4",
      "365,12/31/2015,00:03:24,1877,345804,1,63,502, 0x22C3550A, 0x1C246D40, 0x109F1A3A, 0x1EFFFE0A, 0x3F4E0031, 0x2843466E, 0x045B1909, 0x29A9E68A, 0x0E9523D3, 0x13800480",
      "365,12/31/2015,00:03:48,1877,345828,1,63,403, 0x22C3550A, 0x1C24EC54, 0x16814A22, 0x1EC2EE78, 0x3F52802D, 0x2843484D, 0x2F550185, 0x3D2ED632, 0x3421C34D, 0x39BFDF68",
      "365,12/31/2015,00:03:54,1877,345834,1,63,503, 0x22C3550A, 0x1C250DA0, 0x10C0AB1E, 0x1EC2AA09, 0x3F55C00E, 0x28437DE4, 0x0F6412DF, 0x1D6C36C0, 0x2620982A, 0x3FFFFA68",
      "365,12/31/2015,00:04:18,1877,345858,1,63,404, 0x22C3550A, 0x1C258CB4, 0x16C601E8, 0x1EC47843, 0x3F528012, 0x284353B2, 0x3A04AC17, 0x025C17C2, 0x1F2AA62A, 0x01800C94",
      "365,12/31/2015,00:04:24,1877,345864,1,63,504, 0x22C3550A, 0x1C25AD3C, 0x102AAAAC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC",
      "365,12/31/2015,00:11:24,1877,346284,1,63,518, 0x22C3550A, 0x1C2E6D4C, 0x14A1B3B8, 0x1EFD15DB, 0x3F4E4029, 0x2843301D, 0x0F1B6C25, 0x2C6E2942, 0x2EFBFAA5, 0x0F400B20",
      "365,12/31/2015,12:28:48,1877,390528,1,63,418, 0x22C3550A, 0x1FC82C44, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x0000014E, 0x00000029, 0x00641562, 0x044EC0EB, 0x044000D8",
      "365,12/31/2015,12:28:54,1877,390534,1,63,518, 0x22C3550A, 0x1FC84D34, 0x14A1B582, 0x243D154A, 0x3F4DC023, 0x28432F8B, 0x0F198ACA, 0x2C6EA741, 0x2EC76168, 0x0F400C54",
      "365,12/31/2015,00:02:18,1877,345738,1,63,425, 0x22C3550A, 0x1C230C58, 0x1FEE6CC4, 0x2AEAEEC0, 0x26A66A75, 0x2A666666, 0x26EEEE53, 0x2AEA4013, 0x0000003F, 0x0000006C",
           //
           // Next two records are designed to verify that the EARLIEST transmit time is being retained.
           // The first is hand-edited to show a later transmit time, but the same data as the original.
           // The original record (transmitted at 00:04:54) is then provided as the second record.   It should
           // be used in place of the first record.
      "365,12/31/2015,00:54:54,1877,348894,1,63,505, 0x22C3550A, 0x1C64CD08, 0x114964A0, 0x1EC0910D, 0x3F52803B, 0x28434E0D, 0x0F4A2471, 0x04F2B1F2, 0x274ABF25, 0x3AC009BC",
      "365,12/31/2015,00:04:54,1877,345894,1,63,505, 0x22C3550A, 0x1C264D08, 0x114964A0, 0x1EC0910D, 0x3F52803B, 0x28434E0D, 0x0F4A2471, 0x04F2B1F2, 0x274ABF25, 0x3AC009BC",
           //
           // Next record is designed to verify that a later re-transmission found in the loaded data will
           // NOT overwrite an earlier transmission of the same record already stored in the store.   The record
           // has been hand-modified to have a LATER transmit time than the PRN 5 almanac already stored.
      "365,12/31/2015,01:44:54,1877,351894,1,63,505, 0x22C3550A, 0x1CA34D08, 0x114964A0, 0x1EC0910D, 0x3F52803B, 0x28434E0D, 0x0F4A2471, 0x04F2B1F2, 0x274ABF25, 0x3AC009BC",
           //        
      "365,12/31/2015,00:02:24,1877,345744,1,63,525, 0x22C3550A, 0x1C232DD0, 0x1CDED544, 0x00000FDE, 0x00000029, 0x00000016, 0x00000029, 0x00000016, 0x00000029, 0x000000E0",
      "365,12/31/2015,12:19:54,1877,389994,1,63,525, 0x22C3550A, 0x1FBD0DB0, 0x1CE4157D, 0x00000FC8, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000",
      "365,12/31/2015,12:20:24,1877,390024,1,63,501, 0x22C3550A, 0x1FBDAD2C, 0x104A1BA6, 0x24037521, 0x3F52803B, 0x284333DF, 0x04C7ADAD, 0x04F16DE7, 0x08E35CE8, 0x004001F0",
      "365,12/31/2015,00:11:18,1877,346278,2,61,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x0000014E, 0x00000029, 0x005ED55B, 0x044EC0FD, 0x04400054",
      "365,12/31/2015,18:43:48,1877,413028,2,61,418, 0x22C3550A, 0x219CECF0, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFE7B, 0x3FFFFFFC, 0x3F641555, 0x044EC0D4, 0x044000B4",
      "365,12/31/2015,00:00:48,1877,345648,2,61,422, 0x22C3550A, 0x1C212C10, 0x1F0C570F, 0x2B529DC0, 0x1B335311, 0x39DBF915, 0x2172300B, 0x18B8C176, 0x2A90AA18, 0x149B4EF8",
      "365,12/31/2015,00:00:54,1877,345654,2,61,522, 0x22C3550A, 0x1C214D60, 0x158F6074, 0x1EFCBBAC, 0x3F4DC00A, 0x28461943, 0x0F1D81A5, 0x2B5870BF, 0x277908D8, 0x0DBFFC14",
      "365,12/31/2015,00:01:18,1877,345678,2,61,423, 0x22C3550A, 0x1C21CC74, 0x1F7F6C90, 0x051BB19F, 0x0448D0CA, 0x0DBF8BB3, 0x22743371, 0x094AC152, 0x326CF461, 0x1B130084",
      "365,12/31/2015,00:01:24,1877,345684,2,61,523, 0x22C3550A, 0x1C21EDFC, 0x15D54CB3, 0x1EC0B78B, 0x3F4F0019, 0x284362C1, 0x1A101F0F, 0x25558BFB, 0x1263ADDC, 0x3B3FF85C",
      "365,12/31/2015,00:01:48,1877,345708,2,61,424, 0x22C3550A, 0x1C226CB8, 0x1F9C515F, 0x17E7DFD9, 0x01EFC299, 0x394C2501, 0x103A6022, 0x268D815A, 0x38D116AB, 0x2AAAAAD0",
      "365,12/31/2015,00:01:54,1877,345714,2,61,524, 0x22C3550A, 0x1C228D84, 0x16083F59, 0x1EC174C3, 0x3F4E4029, 0x28432CBF, 0x24835394, 0x034BD570, 0x0C287E8D, 0x3F8005FC",
      "365,12/31/2015,00:02:18,1877,345738,2,61,425, 0x22C3550A, 0x1C230C58, 0x1FEE6CC4, 0x2AEAEEC0, 0x26A66A75, 0x2A666666, 0x26EEEE53, 0x2AEA4013, 0x0000003F, 0x0000006C",
      "365,12/31/2015,00:02:24,1877,345744,2,61,525, 0x22C3550A, 0x1C232DD0, 0x1CDED544, 0x00000FDE, 0x00000029, 0x00000016, 0x00000029, 0x00000016, 0x00000029, 0x055555F8",
      "365,12/31/2015,00:02:48,1877,345768,2,61,401, 0x22C3550A, 0x1C23ACC4, 0x1E7181C9, 0x1C2E68A2, 0x0F4507DA, 0x247093F0, 0x26C720E5, 0x07E00109, 0x0196E4A3, 0x01334EF4",
      "365,12/31/2015,00:02:54,1877,345774,2,61,501, 0x22C3550A, 0x1C23CDB4, 0x104A1B03, 0x1EC3752A, 0x3F52C00A, 0x284334F8, 0x04C97D73, 0x04F1B747, 0x0917642F, 0x004000C4",
      "365,12/31/2015,00:03:18,1877,345798,2,61,402, 0x22C3550A, 0x1C244CC8, 0x164A03ED, 0x1EC5DBEA, 0x3F56803C, 0x28431268, 0x2F65B770, 0x0716C3D8, 0x37EDFFB5, 0x3CFFF7D4",
      "365,12/31/2015,00:03:24,1877,345804,2,61,502, 0x22C3550A, 0x1C246D40, 0x109F1A3A, 0x1EFFFE0A, 0x3F4E0031, 0x2843466E, 0x045B1909, 0x29A9E68A, 0x0E9523D3, 0x13800480",
      "365,12/31/2015,00:03:48,1877,345828,2,61,403, 0x22C3550A, 0x1C24EC54, 0x16814A22, 0x1EC2EE78, 0x3F52802D, 0x2843484D, 0x2F550185, 0x3D2ED632, 0x3421C34D, 0x39BFDE30",
      "365,12/31/2015,00:03:54,1877,345834,2,61,503, 0x22C3550A, 0x1C250DA0, 0x10C0AB1E, 0x1EC2AA09, 0x3F55C00E, 0x28437DE4, 0x0F6412DF, 0x1D6C36C0, 0x2620982A, 0x3FFFFA68",
      "365,12/31/2015,00:04:18,1877,345858,2,61,404, 0x22C3550A, 0x1C258CB4, 0x16C601E8, 0x1EC47843, 0x3F528012, 0x284353B2, 0x3A04AC17, 0x025C17C2, 0x1F2AA62A, 0x01800C94",
      "365,12/31/2015,00:04:24,1877,345864,2,61,504, 0x22C3550A, 0x1C25AD3C, 0x102AAAAC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC, 0x2AAAAABC",
      "365,12/31/2015,00:11:18,1877,346278,3,69,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054",
      "365,12/31/2015,18:35:24,1877,412524,2,61,501, 0x22C3550A, 0x21926DD0, 0x104A1BA6, 0x24037521, 0x3F52803B, 0x284333DF, 0x04C7ADAD, 0x04F16DE7, 0x08E35CE8, 0x004001F0",
      "365,12/31/2015,00:08:48,1877,346128,1,63,413, 0x22C3550A, 0x1C2B2C1C, 0x1D163D8D, 0x0374F72B, 0x0B190095, 0x08F95CEE, 0x0B5F0864, 0x24F97F6B, 0x2B9382F3, 0x2B0D72A8",
      "365,12/31/2015,12:26:18,1877,390378,1,63,413, 0x22C3550A, 0x1FC50CF0, 0x1D1FE70B, 0x31715EBB, 0x1B9122BA, 0x0329194A, 0x18EC680E, 0x074229DF, 0x08E88416, 0x2A2445A4",
      "365,12/31/2015,00:08:48,1877,346128,2,61,413, 0x22C3550A, 0x1C2B2C1C, 0x1D1F18D0, 0x17B5F2ED, 0x3CE0889C, 0x1B176553, 0x129C8100, 0x32321ECF, 0x092F8292, 0x018C79A0"
   };

      // Convert the LNAV strings to PNB
   if (debugLevel) std::cout << "Building PNB from strings" << std::endl;
   gpstk::ObsID currObsID(gpstk::ObsID::otNavMsg,
                          gpstk::ObsID::cbL1,
                          gpstk::ObsID::tcCA);
   gpstk::PackedNavBits msg;
   for (unsigned short i=0; i<LNavExCount; i++)
   {
      msg = getPnbLNav(currObsID,LNavEx[i]);
      dataList.push_back(msg);
   }

      // Have to hand-derive these.   From the total list of messages above,
      //   Count only the messages that contain almanacs.
      //   Remove the default almanacs
      //   Remove the redundant almanacs 
   sizeSubj  = 15; 
   sizeXmit  = 24;
   sizeTotal = sizeSubj + sizeXmit; 

   /*
    *  Define several test of find( ) for Sat ID GPS 1
    *     Expect   Test             Consider          Expected Return
    *       Pass   Time             Effectivity       BeginValid         Test
    *  1.  False   12/31 00:00:00    T            n/a                    Too early
    *  2.  False   12/31 00:02:54    T            n/a                    Too early by 1 s
    *  3.   True   12/31 00:02:55    T            12/31 00:02:54         Ealiest possible
    *  4.   True   12/31 12:20:24    T            12/31 00:02:54         1s prior to cutover
    *  5.   True   12/31 12:20:25    T            12/31 12:20:25         1s after cutover
    *  6.  False    1/31/16 00:00    T            n/a                    Too late
    *  7.   True   12/31 00:00:00    F            12/31 00:02:54         Early, but no eff test
    *  8.   True    1/31/16 00:00    F            12/31 12:29:25         Late, but no eff. test
    */
   pfList.clear();
   SatID sidTest(1,SatID::systemGPS);
   CommonTime begValFirstAlmPRN1 = CivilTime(2015,12,31,00,02,54,TimeSystem::GPS);
   CommonTime begValSecondAlmPRN1 = CivilTime(2015,12,31,12,20,24,TimeSystem::GPS);
   CommonTime testTime = CivilTime(2015,12,31,00,00,00,TimeSystem::GPS); 

   PassFailData pfd(sidTest,testTime,true);   // 1
   pfList.push_back(pfd);

   testTime = CivilTime(2015,12,31,00,02,54,TimeSystem::GPS);  // 2
   pfd = PassFailData(sidTest, testTime,true); 
   pfList.push_back(pfd); 

   testTime = CivilTime(2015,12,31,00,02,55,TimeSystem::GPS);  // 3
   pfd = PassFailData(sidTest, testTime,true,begValFirstAlmPRN1); 
   pfList.push_back(pfd);

   testTime = CivilTime(2015,12,31,12,20,24,TimeSystem::GPS);  // 4
   pfd = PassFailData(sidTest, testTime,true,begValFirstAlmPRN1); 
   pfList.push_back(pfd);
   
   testTime = CivilTime(2015,12,31,12,20,25,TimeSystem::GPS);  // 5
   pfd = PassFailData(sidTest, testTime,true,begValSecondAlmPRN1); 
   pfList.push_back(pfd);

   testTime = CivilTime(2016, 1,31,00,00,00,TimeSystem::GPS);  // 6
   pfd = PassFailData(sidTest, testTime,true); 
   pfList.push_back(pfd); 

   testTime = CivilTime(2015,12,31,00,00,00,TimeSystem::GPS);  // 7
   pfd = PassFailData(sidTest, testTime,false,begValFirstAlmPRN1); 
   pfList.push_back(pfd);

   testTime = CivilTime(2016, 1,31,00,00,00,TimeSystem::GPS);  // 8
   pfd = PassFailData(sidTest, testTime,false,begValSecondAlmPRN1); 
   pfList.push_back(pfd);

   // Now search among the almanacs collected from a specific SV.
   // 
   //     Expect   Test             Consider          Expected Return
   //       Pass   Time             Effectivity       BeginValid         Test
   //  1.  False   12/31 00:00:00    T            n/a                    Too early
   //  2.   True   12/31 00:02:55    T            12/31 00:02:54         Ealiest possible
   //  3.  False    1/31/16 00:00    T            n/a                    Too late
   //  4.   True   12/31 00:00:00    F            12/31 00:02:54         Early, but no eff test
   //  5.   True    1/31/16 00:00    F            12/31 12:29:25         Late, but no eff. test
   //  6.  False   12/31 00:02:55    T            12/31 00:02:54         As 2, but using a SatID not in the maps
   SatID xmitID(2,SatID::systemGPS);

   testTime = CivilTime(2015,12,31,00,00,00,TimeSystem::GPS);  // 1
   pfd = PassFailData(sidTest, testTime,true,xmitID); 
   pfList.push_back(pfd); 

   testTime = CivilTime(2015,12,31,00,02,55,TimeSystem::GPS);  // 2
   pfd = PassFailData(sidTest, testTime,true,begValFirstAlmPRN1,xmitID); 
   pfList.push_back(pfd);

   testTime = CivilTime(2016, 1,31,00,00,00,TimeSystem::GPS);  // 3
   pfd = PassFailData(sidTest, testTime,true,xmitID); 
   pfList.push_back(pfd); 

   testTime = CivilTime(2015,12,31,00,00,00,TimeSystem::GPS);  // 4
   pfd = PassFailData(sidTest, testTime,false,begValFirstAlmPRN1,xmitID); 
   pfList.push_back(pfd);

   testTime = CivilTime(2016, 1,31,00,00,00,TimeSystem::GPS);  // 5
   pfd = PassFailData(sidTest, testTime,false,begValSecondAlmPRN1,xmitID); 
   pfList.push_back(pfd);

   testTime = CivilTime(2015,12,31,00,02,55,TimeSystem::GPS);  // 6
   SatID xmitID_32(32,SatID::systemGPS);
   pfd = PassFailData(sidTest, testTime,true,begValFirstAlmPRN1,xmitID_32);
   pfList.push_back(pfd);

   // PRN 5 is a special case.   There are three messages FROM PRN 1
   // that contain almanac data for PRN 5 (Subframe 5, page 5).  They
   // contain the smae data, but different transmit times.  We want 
   // to verify that the correct almanac message was retained.
   SatID subjID_5(5,SatID::systemGPS);
   testTime = CivilTime(2015,12,31,01,00,00,TimeSystem::GPS);
   CommonTime begValidPRN5 = CivilTime(2015,12,31,00,04,54,TimeSystem::GPS);
   pfd = PassFailData(subjID_5,testTime,true,begValidPRN5);
   pfList.push_back(pfd);

   // Set up tests for deriveLastXmit( ).
   // 
   //     Expect  Subj   Test             Consider   Expected Return
   //       Pass    SV   Time             Eff.       lastXMit         Test
   //  1.   True     1   12/31 00:03:00   T          12/31 18:35:24   PRN 1 stopped at 12:20:24
   //                                                                 however PRN stopped at 18:35:24
   //                                                                 The fact the TWO almanacs were
   //                                                                 available for six hours is 
   //                                                                 exactly what we want to know
   //  2.   True     1   12/31 13:00:00   T          END_OF_TIME      
   //  x.  False    32   12/31 13:00:00   T          NONE             Subject SV not present
   SatID subjID_1(1,SatID::systemGPS);
   testTime = CivilTime(2015,12,31,00,03,00,TimeSystem::GPS);
   CommonTime lastXmit = CivilTime(2015,12,31,18,35,24,TimeSystem::GPS);
   pfd = PassFailData(subjID_1,testTime,true,lastXmit);
   lastXmitList.push_back(pfd); 

   testTime = CivilTime(2015,12,31,13,00,00,TimeSystem::GPS);
   lastXmit = CommonTime::END_OF_TIME;
   pfd = PassFailData(subjID_1,testTime,true,lastXmit);
   lastXmitList.push_back(pfd); 

   pfd = PassFailData(xmitID_32,testTime,false);
   lastXmitList.push_back(pfd);

   return;
}

//---------------------------------------------------------------------------------
void OrbAlmStore_T::
setUpCNAV()
{
   init();

      // Define state variables for writing an CNAV data
   gpstk::ObsID currObsID(gpstk::ObsID::otNavMsg, 
                    gpstk::ObsID::cbL2, 
                    gpstk::ObsID::tcC2LM);
   typeDesc = "GPS_CNAV";

      // Literals for CNAV test data 
   const unsigned short CNavExCount = 8;
   const std::string CNavEx[] =
   {
     "067,03/07/2016,00:00:12,1887,086412,1,63,11, 0x8B04B1C2, 0x2099701F, 0xCD37C9D1, 0xBACE000D, 0xFED7C008, 0x8003BFF3, 0x2D2018DA, 0x4027AAFF, 0x4D180E4F, 0x7C300000",
     "067,03/07/2016,00:00:24,1887,086424,1,63,30, 0x8B05E1C2, 0x304CEA72, 0x640393DD, 0x00094000, 0x0B0FFB7E, 0x5019E0D4, 0x1100FE00, 0x37FFFC01, 0x5F000B16, 0x6FF00000",
     "067,03/07/2016,00:00:36,1887,086436,1,63,33, 0x8B0611C2, 0x404CEA72, 0x640393DD, 0x00094000, 0x00280000, 0x022166A7, 0x5F39D988, 0x80000000, 0x00000502, 0xCA100000",
     "067,03/07/2016,00:00:48,1887,086448,1,63,10, 0x8B04A1C2, 0x50EBE44C, 0xE899007E, 0xF4400186, 0xD2F057FF, 0xC94FB679, 0x2D0014F5, 0x70B04AC8, 0x5FD807FD, 0xA7700000",
     "067,03/07/2016,02:00:00,1887,093600,1,63,10, 0x8B04A1E7, 0x90EBE44C, 0xE8A50073, 0x88BFFE1F, 0x93079000, 0x80226376, 0xB44814F4, 0x518C4AB0, 0x475C0E58, 0x3A700000",
     "067,03/07/2016,02:00:12,1887,093612,1,63,11, 0x8B04B1E7, 0xA0A5701F, 0x7F6BC9D1, 0xB8C2E015, 0xCEBEC007, 0xDFFCBFF1, 0x8F201A17, 0x202586FF, 0x38100791, 0x85000000",
     "067,03/07/2016,02:00:24,1887,093624,1,63,30, 0x8B05E1E7, 0xB04CEA72, 0x9403945F, 0x00094000, 0x0B0FFB7E, 0x5019E0D4, 0x1100FE00, 0x37FFFC01, 0x5F000C5A, 0xF4A00000",
     "067,03/07/2016,02:00:36,1887,093636,1,63,33, 0x8B0611E7, 0xC04CEA72, 0x9403945F, 0x00094000, 0x00280000, 0x022166A7, 0x5F39D988, 0x80000000, 0x0000024E, 0x51400000"
   };

      // Convert the CNAV strings to PNB
   gpstk::PackedNavBits msg;
   for (unsigned short i=0; i<CNavExCount; i++)
   {
      msg = getPnbCNav(currObsID, CNavEx[i]);
      dataList.push_back(msg);
   }
   return;
}

//---------------------------------------------------------------------------------
void OrbAlmStore_T::
setUpBDS()
{

}

//---------------------------------------------------------------------------------
void OrbAlmStore_T::
setUpGLO()
{

}

   //---------------------------------------------------------------
   gpstk::PackedNavBits
   OrbAlmStore_T::
   getPnbLNav(const gpstk::ObsID& oidr, const std::string& str)
                          throw(gpstk::InvalidParameter)
   {      
      try
      {
            // Split the line into words separated by commas.
            // There should be 18 words
         vector<string> words = StringUtils::split(str,',');
         unsigned short numWords = words.size();
         /*
         cout << "numWords : " << numWords
              << ", [0]: '" << words[0]
              << "', [numWords-1]: '" << words[numWords-1] << "'." << endl;
         */
         if (numWords!=18) 
         {
            stringstream ss;
            ss << "Line format problem. ";
            ss << "  Should be at least 18 items.";
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip);
         }

            // Convert the time information into a CommonTime
         int week = gpstk::StringUtils::asInt(words[3]);
         double sow = gpstk::StringUtils::asDouble(words[4]);
         CommonTime ct = GPSWeekSecond(week,sow,TimeSystem::GPS);
         
            // Convert the PRN to a SatID
         int prn = StringUtils::asInt(words[5]);
         SatID sid(prn,SatID::systemGPS);

            // Get the message ID
         int msgID = StringUtils::asInt(words[7]); 

         PackedNavBits pnb(sid,oidr,ct); 

            // Load the raw data
         int offset = 8;
         for (int i=0; i<10; i++)
         {
            int ndx = i + offset;
            string hexStr = StringUtils::strip(words[ndx]);
            string::size_type n = hexStr.find("x"); 
            hexStr = hexStr.substr(n+1);
            unsigned long bits = StringUtils::x2uint(hexStr);
            pnb.addUnsignedLong(bits,30,1);
         }
         pnb.trimsize();
         return pnb; 
      }
      catch (StringUtils::StringException)
      {
         stringstream ss;
         ss << "String conversion error:'" << str << "'.";
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip);
      }  
   }

   
   //-------------------------------------------------
   gpstk::PackedNavBits 
   OrbAlmStore_T::
   getPnbCNav(const gpstk::ObsID& oidr, const std::string& str)
             throw(gpstk::InvalidParameter)
   {
      try
      {
            // Split the line into words separated by commas.
            // There should be 18 words
         vector<string> words = StringUtils::split(str,',');
         unsigned short numWords = words.size();
         if (numWords!=18) 
         {
            stringstream ss;
            ss << "Line format problem. ";
            ss << "  Should be at least 18 items.";
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip);
         }

            // Convert the time information into a CommonTime
         int week = gpstk::StringUtils::asInt(words[3]);
         double sow = gpstk::StringUtils::asDouble(words[4]);
         CommonTime ct = GPSWeekSecond(week,sow,TimeSystem::GPS);
         
            // Convert the PRN to a SatID
         int prn = StringUtils::asInt(words[5]);
         SatID sid(prn,SatID::systemGPS);

            // Get the message ID
         int msgID = StringUtils::asInt(words[7]); 

         PackedNavBits pnb(sid,oidr,ct); 

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
            if (offset<9) pnb.addUnsignedLong(bits,32,1);
            else
            {
               bits >>= 20;
               pnb.addUnsignedLong(bits,12,1);
            }
         }
         pnb.trimsize();
         return pnb;
      }
      catch (StringUtils::StringException)
      {
         stringstream ss;
         ss << "String conversion error:'" << str << "'.";
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip);
      }  
   }


//---------------------------------------------------------------------------------
int main()
{
  unsigned errorTotal = 0;
  
  OrbAlmStore_T testClass;

  testClass.setUpLNAV();
  errorTotal += testClass.createAndDump();
  errorTotal += testClass.findEmptyTest();
  
  testClass.setUpCNAV();
  //errorTotal += testClass.createAndDump();
  
  testClass.setUpBDS();
  //errorTotal += testClass.writeReadTest();
  
  testClass.setUpGLO();
  //errorTotal += testClass.writeReadTest();

  cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
  return errorTotal; // Return the total number of errors
}

