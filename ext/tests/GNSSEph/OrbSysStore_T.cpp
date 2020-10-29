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

 /*********************************************************************
*
*  Test program for gpstk/ext/lib/GNSSEph/OrbDataSys* and /OrbSysStore*
*
*********************************************************************/
#include <iostream>
#include <fstream>

#include "CivilTime.hpp"
#include "Exception.hpp"
#include "GPSWeekSecond.hpp"
#include "OrbDataSys.hpp"
#include "OrbSysGpsL_56.hpp"
#include "OrbSysGpsC_33.hpp"
#include "OrbSysStore.hpp"
#include "SystemTime.hpp"
#include "TimeString.hpp"
#include "TimeSystem.hpp"

#include "build_config.h"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

class OrbSysStore_T
{
public:
   OrbSysStore_T();

   void init();

   unsigned createAndDump_LNAV();
   unsigned createAndDump_CNAV();
   void setUpLNAV();
   void setUpCNAV();
   void setUpBDS();
   void setUpGLO();
      /**
       * @throw InvalidParameter */
   gpstk::PackedNavBits getPnbLNav(const gpstk::ObsID& oidr,
                                   const std::string& str);
      /**
       * @throw InvalidParameter */
   gpstk::PackedNavBits getPnbCNav(const gpstk::ObsID& oidr,
                                   const std::string& str);

      // Methods above exist to set up the following
      // members
   list<PackedNavBits> dataList;
   string typeDesc;
   CommonTime initialCT;
   CommonTime finalCT;
   int msgsExpectedToBeAdded;

   ofstream out;

      // For testing the test
   int debugLevel;
};

OrbSysStore_T::
OrbSysStore_T()
{
   debugLevel = 0;
   init();
}

unsigned OrbSysStore_T::
createAndDump_LNAV()
{
   string currMethod = typeDesc + " create/store OrbDataSys objects";
   TUDEF("OrbSysStore",currMethod);

      // Open an output stream specific to this navigation message type
   std::string fs = getFileSep();
   std::string tf(getPathTestTemp()+fs);
   std::string tempFile = tf + "test_output_OrbSysStore_T_" +
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
   OrbSysStore oss;
   oss.setDebugLevel(debugLevel);

   bool passed = true;
   unsigned long addSuccess = 0;
   list<PackedNavBits>::const_iterator cit;
   for (cit=dataList.begin();cit!=dataList.end();cit++)
   {
      try
      {
         const PackedNavBits& pnbr = *cit;
         bool retval = oss.addMessage(pnbr);
         if (retval) addSuccess++;
      }
      catch(gpstk::InvalidRequest ir)
      {
         passed = false;
         std::stringstream ss;
         ss << "Load of OrbSysStore failed." << std::endl;
         ss << ir;
         TUFAIL(ss.str());
      }
   }
   unsigned long count = oss.size();
   if (count!=msgsExpectedToBeAdded)
   {
      stringstream ss;
      ss << "Size of ObsSysStore incorrect after loading.  Expected "
         << msgsExpectedToBeAdded << " actual size " << count;
      TUFAIL(ss.str());
      passed = false;
   }
   if (passed) TUPASS("Successfully loaded data to store.");

//--- Test the isPresent( ) method --------------------------------
   currMethod = typeDesc + " OrbSysStore.isPresent() ";
   TUCSM(currMethod);
   SatID sidT1(1,SatelliteSystem::GPS);
   if (oss.isPresent(sidT1))
      TUPASS("");
   else
      TUFAIL("Failed to find PRN 1 in store");

   SatID sidT2(33,SatelliteSystem::GPS);
   if (oss.isPresent(sidT2))
      TUFAIL("Reported PRN 33 as present (which is not true)");
   else
      TUPASS("");

//--- Test the getXXXTime( ) methods -------------------------
   currMethod = typeDesc + " OrbSysStore.getXxxxTime() ";
   TUCSM(currMethod);
   try
   {
      const CommonTime& initialTestT = oss.getInitialTime();
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
      const CommonTime& finalTestT = oss.getFinalTime();
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
   currMethod = typeDesc + " OrbSysStore.find() ";
   TUCSM(currMethod);
   SatID sidTest(1,SatelliteSystem::GPS);
   NavID nidTest(NavType::GPSLNAV);
   unsigned long UID = 56;

      // TOO EARLY
   CommonTime testTime = CivilTime(2015,12,31,00,00,00,TimeSystem::GPS);
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime);
      stringstream ss;
      ss << "Failed to throw exception for time prior to all data";
      TUFAIL(ss.str());
   }
   catch (InvalidRequest)
   {
      TUPASS("");
   }

      // Right on time (which is still too early)
   testTime = CivilTime(2015,12,31,00,11,18,TimeSystem::GPS);
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime);
      stringstream ss;
      ss << "Failed to throw exception for time prior to all data";
      TUFAIL(ss.str());
   }
   catch (InvalidRequest)
   {
      TUPASS("");
   }

      // Should return object with xMit of 00:11:31
   CommonTime expTime = testTime;
   testTime = CivilTime(2015,12,31,02,00,00,TimeSystem::GPS);
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime);
      if (p->beginValid==expTime) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Wrong object found.  Expected xmit time "
            << printTime(expTime,"%02H:%02M:%02S")
            << " found time "
            << printTime(p->beginValid,"%02H:%02M:%02S");
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

      // Should return object with xMit of 00:11:31
   testTime = CivilTime(2015,12,31,12,28,48,TimeSystem::GPS);
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime);
      if (p->beginValid==expTime) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Wrong object found.  Expected xmit time "
            << printTime(expTime,"%02H:%02M:%02S")
            << " found time "
            << printTime(p->beginValid,"%02H:%02M:%02S");
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

      // Should return object with xMit of 12:28:48
   expTime =  CivilTime(2015,12,31,12,28,48,TimeSystem::GPS);
   testTime = CivilTime(2015,12,31,14,00,00,TimeSystem::GPS);
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime);
      if (p->beginValid==expTime) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Wrong object found.  Expected xmit time "
            << printTime(expTime,"%02H:%02M:%02S")
            << " found time "
            << printTime(p->beginValid,"%02H:%02M:%02S");
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

      // Test find() for most recent GPS UTC information (418)
      // Should return object with xMit of 12:28:48
   currMethod = typeDesc + " OrbSysStore.find() across all SVs";
   TUCSM(currMethod);
   expTime =  CivilTime(2015,12,31,18,43,48,TimeSystem::GPS);
   testTime = CivilTime(2015,12,31,23,59,59,TimeSystem::GPS);
   UID = 56;
   try
   {
      const OrbDataSys* p = oss.find(nidTest,UID,testTime);
      if (p->beginValid==expTime) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Wrong object found.  Expected xmit time "
            << printTime(expTime,"%02H:%02M:%02S")
            << " found time "
            << printTime(p->beginValid,"%02H:%02M:%02S");
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

   expTime =  CivilTime(2015,12,31,12,28,48,TimeSystem::GPS);
   testTime = CivilTime(2015,12,31,13,00,00,TimeSystem::GPS);
   UID = 56;
   try
   {
      const OrbDataSys* p = oss.find(nidTest,UID,testTime);
      if (p->beginValid==expTime) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Wrong object found.  Expected xmit time "
            << printTime(expTime,"%02H:%02M:%02S")
            << " found time "
            << printTime(p->beginValid,"%02H:%02M:%02S");
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

      // Test findList(NavID,UID)
   currMethod = typeDesc + " OrbSysStore.findList(NavID,UID)";
   TUCSM(currMethod);
   UID = 56;
   try
   {
      list<const OrbDataSys*> pList = oss.findList(nidTest,UID);
      TUASSERTE(int,5,pList.size());
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << "Unexpected exception" << endl;
      ss << ir << endl;
      TUFAIL(ss.str());
   }

      // Test getSatIDList()
   currMethod = typeDesc + " OrbSysStore.getSatIDList()";
   TUCSM(currMethod);
   list<SatID> satIDList = oss.getSatIDList();
   TUASSERTE(int,3,satIDList.size());
   SatID sid = satIDList.front();
   TUASSERTE(SatID,sid,SatID(1,SatelliteSystem::GPS));

      // Test getNavIDList()
   currMethod = typeDesc + " OrbSysStore.getNavIDList()";
   TUCSM(currMethod);
   list<NavID> navIDList = oss.getNavIDList();
   TUASSERTE(int,1,navIDList.size());
   NavID nid = navIDList.front();
   NavID expNid = NavID(NavType::GPSLNAV);
   TUASSERTE(NavID,nid,expNid);

      // Test getSatSysList()
   /*
   currMethod = typeDesc + " OrbSysStore.getSatSysList()";
   TUCSM(currMethod);
   const list<SatelliteSystem>& satSysList = oss.getSatSysList();
   TUASSERTE(int,1,satSysList.size());
   SatelliteSystem ss = satSysList.front();
   TUASSERTE(SatelliteSystem,ss,SatelliteSystem::GPS);
   cout << "Length of satSysList: " << satSysList.size() << endl;
   list<SatelliteSystem>::const_iterator tt;
   for (tt=satSysList.begin(); tt!=satSysList.end(); tt++)
   {
      cout << " " << *tt << endl;
   }
  */
  //
//-------------------------------------------------------------------------------
//  Test hasSignal() method
//
      // Test data has a Subframe 4, page 25 from 12/31/2015 a little after 0200.
      // Therefore, we'll set the test time-of-interest to 12/31/2015 0300.
   CommonTime ctHS = CivilTime(2015,12,31,3,0,0.0,TimeSystem::Any);
//
//  Test Cases
//
//   SV Type   Signal     hasSignal      Exception
//   -------   ---------  ---------      ---------
//     IIA      L1 C/A         Y             N
//     IIA      L1 P           Y             N
//     IIA      L2 Y           Y             N
//     IIA      L2 CM          N             N
//     IIA      L1 M           N             N
//     IIA      L5 I           N             N
//     IIA      L1C            N             N
//     IIR      L1 C/A         Y             N
//     IIR      L1 P           Y             N
//     IIR      L2 Y           Y             N
//     IIR      L2 CM          N             N
//     IIR      L1 M           N             N
//     IIR      L5 I           N             N
//     IIR      L1C            N             N
//     IIR-M    L1 C/A         Y             N
//     IIR-M    L1 P           Y             N
//     IIR-M    L2 Y           Y             N
//     IIR-M    L2 CM          Y             N
//     IIR-M    L1 M           Y             N
//     IIR-M    L5 I           N             N
//     IIR-M    L1C            N             N
//     IIF      L1 C/A         Y             N
//     IIF      L1 P           Y             N
//     IIF      L2 Y           Y             N
//     IIF      L2 CM          Y             N
//     IIF      L1 M           Y             N
//     IIF      L5 I           Y             N
//     IIF      L1C            N             N
//     III      L1 C/A         Y             N
//     III      L1 P           Y             N
//     III      L2 Y           Y             N
//     III      L2 CM          Y             N
//     III      L1 M           Y             N
//     III      L5 I           Y             N
//     III      L1C            Y             N
//  PRN==99     L1 C/A         -             Y
//  Beidou      L1 C/A         -             Y
//
   currMethod = typeDesc + " OrbSysStore.hasSignal()";
   TUCSM(currMethod);

      // Define test control structure
   struct hsTestData
   {
      int svNdx;
      int obsNdx;
      bool isPass;
      bool throwException;
   };
   hsTestData hsTestArray[] =
   {
      // Block IIA tests
      { 0, 0,  true, false },
      { 0, 1,  true, false },
      { 0, 2,  true, false },
      { 0, 3, false, false },
      { 0, 4, false, false },
      { 0, 5, false, false },
      { 0, 6, false, false },
      // Block IIR tests
      { 1, 0,  true, false },
      { 1, 1,  true, false },
      { 1, 2,  true, false },
      { 1, 3, false, false },
      { 1, 4, false, false },
      { 1, 5, false, false },
      { 1, 6, false, false },
      // Block IIR-M tests
      { 2, 0,  true, false },
      { 2, 1,  true, false },
      { 2, 2,  true, false },
      { 2, 3,  true, false },
      { 2, 4,  true, false },
      { 2, 5, false, false },
      { 2, 6, false, false },
      // Block IIF tests
      { 3, 0,  true, false },
      { 3, 1,  true, false },
      { 3, 2,  true, false },
      { 3, 3,  true, false },
      { 3, 4,  true, false },
      { 3, 5,  true, false },
      { 3, 6, false, false },
      // GPS III tests
      { 4, 0,  true, false },
      { 4, 1,  true, false },
      { 4, 2,  true, false },
      { 4, 3,  true, false },
      { 4, 4,  true, false },
      { 4, 5,  true, false },
      { 4, 6,  true, false },
      // Invalid cases
      { 5,  0, false,  true },
      { 6,  0, false,  true }
   };
   int NCASES = 37;

      // Define SVs of interest
   SatID sidAr[] =
   {
      SatID(18,SatelliteSystem::GPS),    // Block IIA.  None in system on 12/13/2015.  Had to hand-edit data
      SatID(19,SatelliteSystem::GPS),    // Block IIR.  SVN 59/PRN 19
      SatID(17,SatelliteSystem::GPS),    // Block IIR-M.  SVN 53/PRN 17
      SatID( 9,SatelliteSystem::GPS),    // Block IIF.  SVN 68/PRN 9
      SatID( 1,SatelliteSystem::GPS),    // GPS III.  None in system on 12/13/2015.  Had to hand-edit data
      SatID(99,SatelliteSystem::GPS),    // Invalid PRN ID.
      SatID( 1,SatelliteSystem::BeiDou), // Invalid System
   };

      // Define signals of interest
   ObsID oidAr[] =
   {
      ObsID(ObservationType::Unknown, CarrierBand::L1, TrackingCode::CA),    // L1 C/A  - 0
      ObsID(ObservationType::Unknown, CarrierBand::L1, TrackingCode::P),     // L1 P    - 1
      ObsID(ObservationType::Unknown, CarrierBand::L1, TrackingCode::Y),     // L2 Y    - 2
      ObsID(ObservationType::Unknown, CarrierBand::L2, TrackingCode::L2CML),  // L2 CMCL - 3
      ObsID(ObservationType::Unknown, CarrierBand::L1, TrackingCode::MDP),     // L1 M    - 4
      ObsID(ObservationType::Unknown, CarrierBand::L5, TrackingCode::L5I),    // L5 I    - 5
      ObsID(ObservationType::Unknown, CarrierBand::L1, TrackingCode::L1CD),   // L1C     - 6
   };

      // Execute the test cases
   for (int ndx=0;ndx<NCASES;ndx++)
   {
      hsTestData& currTest = hsTestArray[ndx];
      try
      {
         bool retVal = oss.hasSignal(sidAr[currTest.svNdx],
                           ctHS,
                           oidAr[currTest.obsNdx]);
         TUASSERTE(bool,retVal,currTest.isPass);
      }
         // hasSignal() threw an exception
      catch(InvalidRequest exc)
      {
         if (currTest.throwException)
         {
            stringstream ss;
            ss << "Threw expected exception for case ";
            ss << ndx;
            TUPASS(ss.str());
         }
         else
         {
            stringstream ss;
            ss << "Unexpectedly threw exception for case ";
            ss << ndx;
            ss << "," << exc;
            TUFAIL(ss.str());
         }
      }
   }

   //
   // Test findUtcData() method
   //
   CommonTime ctUtc = CivilTime(2015,12,31,13,0,0.0,TimeSystem::GPS);   // 01:00:00 of day.

   double TOLERANCE = 1e-12;

      // Calculate an assmed truth value.
   double A0Assumed =  9.31322575e-10;
   double A1Assumed =  4.44089210e-15;
   CommonTime totAssumed = CivilTime(2016,1,2,19,50,24,TimeSystem::GPS);
   double assumedOffset = A0Assumed + A1Assumed * (ctUtc - totAssumed);

   NavID nidUtc(NavType::GPSLNAV);
   try
   {
      const OrbDataUTC* odu = oss.findUtcData(nidUtc,ctUtc);
      double testOffset = odu->getUtcOffsetModLeapSec(ctUtc);
      TUASSERTFEPS(assumedOffset, testOffset, TOLERANCE);
         //
      const OrbSysGpsL_56* p56 = dynamic_cast<const OrbSysGpsL_56*>(odu);
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << "Failed to find when expected to success.  ";
      ss << ir;
      TUFAIL(ss.str());
   }

      // Test a too early value.  Should throw an exception
   ctUtc = CivilTime(2015,12,30,0,0,0.0,TimeSystem::GPS);   // Time too early
   try
   {
       const OrbDataUTC* odu = oss.findUtcData(nidUtc,ctUtc);
       TUFAIL("Returned a value when time is prior to data.");
   }
   catch (InvalidRequest)
   {
      TUPASS("Threw expected exception when time is prior to data.");
   }

      // Dump the store
   currMethod = typeDesc + " OrbSysStore.dump()";
   TUCSM(currMethod);
   oss.dump(out);

      // Dump terse (one-line) summaries
   oss.dump(out,1);

      // Dump all contents
   oss.dump(out,2);

      // Dump terese in time order
   oss.dump(out,3);

      // Clear the store
   currMethod = typeDesc + " OrbSysStore.clear()";
   TUCSM(currMethod);

   oss.clear();
   if (oss.size()!=0)
   {
      TUFAIL("Failed to entirely clear OrbSysStore.");
   }
   else TUPASS("");

   out.close();

   TURETURN();
}

unsigned OrbSysStore_T::
createAndDump_CNAV()
{
   string currMethod = typeDesc + " create/store OrbDataSys objects";
   TUDEF("OrbSysStore",currMethod);

      // Open an output stream specific to this navigation message type
   std::string fs = getFileSep();
   std::string tf(getPathTestTemp()+fs);
   std::string tempFile = tf + "test_output_OrbSysStore_T_" +
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
   OrbSysStore oss;
   oss.setDebugLevel(debugLevel);

   bool passed = true;
   unsigned long addSuccess = 0;
   list<PackedNavBits>::const_iterator cit;
   for (cit=dataList.begin();cit!=dataList.end();cit++)
   {
      try
      {
         const PackedNavBits& pnbr = *cit;
         bool retval = oss.addMessage(pnbr);
         if (retval) addSuccess++;
      }
      catch(gpstk::InvalidRequest ir)
      {
         passed = false;
         std::stringstream ss;
         ss << "Load of OrbSysStore failed." << std::endl;
         ss << ir;
         TUFAIL(ss.str());
      }
   }
   unsigned long count = oss.size();
   if (count!=msgsExpectedToBeAdded)
   {
      stringstream ss;
      ss << "Size of ObsSysStore incorrect after loading.  Expected "
         << msgsExpectedToBeAdded << " actual size " << count;
      TUFAIL(ss.str());
      passed = false;
   }
   if (passed) TUPASS("Successfully loaded data to store.");

    //
   // Test findUtcData() method
   //
   CommonTime ctUtc = CivilTime(2016,3,7,1,0,0.0,TimeSystem::GPS);   // 00:00:00 of day.

   double TOLERANCE = 1e-12;

      // Calculate an assmed truth value.
   double A0Assumed =  5.8207660913e-10;
   double A1Assumed =  0.00;
   CommonTime totAssumed = CivilTime(2016,3,6,12,45,04,TimeSystem::GPS);
   double assumedOffset = A0Assumed + A1Assumed * (ctUtc - totAssumed);

   NavID nidUtcL2(NavType::GPSCNAVL2);
   try
   {
      const OrbDataUTC* odu = oss.findUtcData(nidUtcL2,ctUtc);
      double testOffset = odu->getUtcOffsetModLeapSec(ctUtc);
      TUASSERTFEPS(assumedOffset, testOffset, TOLERANCE);
         //
      const OrbSysGpsC_33* p33 = dynamic_cast<const OrbSysGpsC_33*>(odu);
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << "Failed to find when expected to success.  ";
      ss << ir;
      TUFAIL(ss.str());
   }

        //L5 check
   NavID nidUtcL5(NavType::GPSCNAVL5);
   try
   {
      const OrbDataUTC* odu = oss.findUtcData(nidUtcL5,ctUtc);
      double testOffset = odu->getUtcOffsetModLeapSec(ctUtc);
      TUASSERTFEPS(assumedOffset, testOffset, TOLERANCE);
         //
      const OrbSysGpsC_33* p33 = dynamic_cast<const OrbSysGpsC_33*>(odu);
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << "Failed to find when expected to success.  ";
      ss << ir;
      TUFAIL(ss.str());
   }

      // Test a too early value.  Should throw an exception
   ctUtc = CivilTime(2016,3,6,0,0,0.0,TimeSystem::GPS);   // Time too early
   try
   {
       const OrbDataUTC* odu = oss.findUtcData(nidUtcL2,ctUtc);
       TUFAIL("Returned a value when time is prior to data.");
   }
   catch (InvalidRequest)
   {
      TUPASS("Threw expected exception when time is prior to data.");
   }

      // Dump the store
   currMethod = typeDesc + " OrbSysStore.dump()";
   TUCSM(currMethod);
   oss.dump(out);

      // Dump terse (one-line) summaries
   oss.dump(out,1);

      // Dump all contents
   oss.dump(out,2);

      // Dump terese in time order
   oss.dump(out,3);

      // Clear the store
   currMethod = typeDesc + " OrbSysStore.clear()";
   TUCSM(currMethod);

   oss.clear();
   if (oss.size()!=0)
   {
      TUFAIL("Failed to entirely clear OrbSysStore.");
   }
   else TUPASS("");

   out.close();

   TURETURN();
}

void OrbSysStore_T::
init()
{
   dataList.clear();
}

void OrbSysStore_T::
setUpLNAV()
{
   init();

      // Define state variables for creating a LNAV store
   typeDesc = "GPS_LNAV";
   initialCT = CivilTime(2015,12,31,00,00,18,TimeSystem::GPS);
   finalCT   = CivilTime(2015,12,31,18,43,48,TimeSystem::GPS);
   msgsExpectedToBeAdded = 11;

      // Literals for LNAV test data
   const unsigned short LNavExCount = 20;
   const std::string LNavEx[] =
   {
      "365,12/31/2015,00:00:00,1877,345600,1,63,100, 0x22C3550A, 0x1C2029AC, 0x35540023, 0x0EA56C31, 0x16E4B88E, 0x37CECD3F, 0x171242FF, 0x09D588A2, 0x0000023F, 0x00429930", // not added
      "365,12/31/2015,00:00:06,1877,345606,1,63,200, 0x22C3550A, 0x1C204A3C, 0x09FDB732, 0x0BC06889, 0x3C5827D1, 0x3E08808B, 0x21A678CF, 0x0472285B, 0x0350F3B4, 0x15889F94", // not added
      "365,12/31/2015,00:00:12,1877,345612,1,63,300, 0x22C3550A, 0x1C206BB4, 0x3FFAC4D5, 0x0CAD96FA, 0x3FFA09D3, 0x10F0C405, 0x06D1C4E4, 0x31C1B694, 0x3FEA6E36, 0x09FFA5F4", // not added
      "365,12/31/2015,00:00:18,1877,345618,1,63,421, 0x22C3550A, 0x1C208C44, 0x1E7181C9, 0x1C2E68A2, 0x0F4507DA, 0x247093F0, 0x26C720E5, 0x07E00109, 0x0196E4A3, 0x1D588110", // added
      "365,12/31/2015,00:00:24,1877,345624,1,63,521, 0x22C3550A, 0x1C20ADCC, 0x156ED525, 0x1EFEDF83, 0x3F4DC035, 0x2843463D, 0x047D1075, 0x2D2F1B44, 0x3814F871, 0x2FBFF920", // not added
      "365,12/31/2015,00:11:18,1877,346278,1,63,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054", // added
      "365,12/31/2015,00:11:24,1877,346284,1,63,518, 0x22C3550A, 0x1C2E6D4C, 0x14A1B3B8, 0x1EFD15DB, 0x3F4E4029, 0x2843301D, 0x0F1B6C25, 0x2C6E2942, 0x2EFBFAA5, 0x0F400B20", // not added
      "365,12/31/2015,12:28:48,1877,390528,1,63,418, 0x22C3550A, 0x1FC82C44, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x0000014E, 0x00000029, 0x00641562, 0x044EC0EB, 0x044000D8", // added
      "365,12/31/2015,12:28:54,1877,390534,1,63,518, 0x22C3550A, 0x1FC84D34, 0x14A1B582, 0x243D154A, 0x3F4DC023, 0x28432F8B, 0x0F198ACA, 0x2C6EA741, 0x2EC76168, 0x0F400C54", // not added
                                             // Hand edited this column from 0x1FEE6CC4, to 0x1FF26CC4
      "365,12/31/2015,00:02:18,1877,345738,1,63,425, 0x22C3550A, 0x1C230C58, 0x1FF26CC4, 0x2AEAEEC0, 0x26A66A75, 0x2A666666, 0x26EEEE53, 0x2AEA4013, 0x0000003F, 0x0000006C", // added
      "365,12/31/2015,00:02:24,1877,345744,1,63,525, 0x22C3550A, 0x1C232DD0, 0x1CDED544, 0x00000FDE, 0x00000029, 0x00000016, 0x00000029, 0x00000016, 0x00000029, 0x000000E0", // added
      "365,12/31/2015,00:11:18,1877,346278,2,61,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x0000014E, 0x00000029, 0x005ED55B, 0x044EC0FD, 0x04400054", // added
      "365,12/31/2015,18:43:48,1877,413028,2,61,418, 0x22C3550A, 0x219CECF0, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFE7B, 0x3FFFFFFC, 0x3F641555, 0x044EC0D4, 0x044000B4", // added
      "365,12/31/2015,00:11:18,1877,346278,3,69,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054", // added and then removed
      "365,12/31/2015,00:08:48,1877,346128,1,63,413, 0x22C3550A, 0x1C2B2C1C, 0x1D163D8D, 0x0374F72B, 0x0B190095, 0x08F95CEE, 0x0B5F0864, 0x24F97F6B, 0x2B9382F3, 0x2B0D72A8", // added
      "365,12/31/2015,12:26:18,1877,390378,1,63,413, 0x22C3550A, 0x1FC50CF0, 0x1D1FE70B, 0x31715EBB, 0x1B9122BA, 0x0329194A, 0x18EC680E, 0x074229DF, 0x08E88416, 0x2A2445A4", // added
      "365,12/31/2015,00:08:48,1877,346128,2,61,413, 0x22C3550A, 0x1C2B2C1C, 0x1D1F18D0, 0x17B5F2ED, 0x3CE0889C, 0x1B176553, 0x129C8100, 0x32321ECF, 0x092F8292, 0x018C79A0", // added
      "365,12/31/2015,00:11:00,1877,346260,3,69,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054", // added
      "365,12/31/2015,00:22:48,1877,347028,3,69,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FEB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054", // not added
      "365,12/31/2015,00:11:18,1877,346278,3,69,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FEB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054"  // not added
   };

      // Convert the LNAV strings to PNB
   if (debugLevel) std::cout << "Building PNB from strings" << std::endl;
   gpstk::ObsID currObsID(gpstk::ObservationType::NavMsg,
                          gpstk::CarrierBand::L1,
                          gpstk::TrackingCode::CA);
   gpstk::PackedNavBits msg;
   for (unsigned short i=0; i<LNavExCount; i++)
   {
      msg = getPnbLNav(currObsID,LNavEx[i]);
      dataList.push_back(msg);
   }
   return;
}

void OrbSysStore_T::
setUpCNAV()
{
   init();

      // Define state variables for writing an CNAV data
   gpstk::ObsID L2ObsID(gpstk::ObservationType::NavMsg,
                    gpstk::CarrierBand::L2,
                    gpstk::TrackingCode::L2CML);
   gpstk::ObsID L5ObsID(gpstk::ObservationType::NavMsg,
                    gpstk::CarrierBand::L5,
                    gpstk::TrackingCode::L5I);
   msgsExpectedToBeAdded = 4;
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
      msg = getPnbCNav(L2ObsID, CNavEx[i]);
      dataList.push_back(msg);
      msg = getPnbCNav(L5ObsID, CNavEx[i]);
      dataList.push_back(msg);
   }
   return;
}

void OrbSysStore_T::
setUpBDS()
{

}

void OrbSysStore_T::
setUpGLO()
{

}

   //---------------------------------------------------------------
   gpstk::PackedNavBits
   OrbSysStore_T::
   getPnbLNav(const gpstk::ObsID& oidr, const std::string& str)
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
         SatID sid(prn,SatelliteSystem::GPS);

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
   OrbSysStore_T::
   getPnbCNav(const gpstk::ObsID& oidr, const std::string& str)
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
         SatID sid(prn,SatelliteSystem::GPS);

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

int main()
{
  unsigned errorTotal = 0;

  OrbSysStore_T testClass;

  testClass.setUpLNAV();
  errorTotal += testClass.createAndDump_LNAV();

  testClass.setUpCNAV();
  errorTotal += testClass.createAndDump_CNAV();

  testClass.setUpBDS();
  //errorTotal += testClass.writeReadTest();

  testClass.setUpGLO();
  //errorTotal += testClass.writeReadTest();

  cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
  return errorTotal; // Return the total number of errors
}


