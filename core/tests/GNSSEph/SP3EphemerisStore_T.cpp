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

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "SatID.hpp"
#include "Exception.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "SP3EphemerisStore.hpp"
#include "TestUtil.hpp"

using namespace gpstk;
using namespace std;

class SP3EphemerisStore_T
{
public:
   SP3EphemerisStore_T()
   {
      epsilon = 1E-12;
   }

//=============================================================================
// Initialize Test Data Filenames
//=============================================================================
   void init()
   {
      std::string dataFilePath = gpstk::getPathData();
      std::string tempFilePath = gpstk::getPathTestTemp();
      std::string fileSep = gpstk::getFileSep();

      inputSP3Data = dataFilePath + fileSep +
         "test_input_sp3_nav_ephemerisData.sp3";
      inputSP3cData = dataFilePath + fileSep +
         "test_input_SP3c.sp3";
      inputAPCData = dataFilePath + fileSep +
         "test_input_sp3_nav_apcData.sp3";
      inputSixNinesData = dataFilePath + fileSep +
         "inputs" + fileSep + "igs" + fileSep + "igr20354.sp3";
      inputNotaFile = dataFilePath + fileSep + "NotaFILE";
      outputDataDump = tempFilePath + fileSep + "SP3_DataDump.txt";

      inputComparisonOutput1 =
         "x:(-1.51906e+07, -2.15539e+07, 3.31227e+06),"
         " v:(488.793, 118.124, 3125.01), clk bias:1.68268e-05,"
         " clk drift:1.93783e-11, relcorr:-8.45152e-09,"
         " health:Unused";
      inputComparisonOutput15 =
         "x:(-1.57075e+07, 1.72951e+07, 1.24252e+07),"
         " v:(408.54, -1568.11, 2651.16), clk bias:0.000411558,"
         " clk drift:3.22901e-12, relcorr:1.32734e-08,"
         " health:Unused";
      inputComparisonOutput31 =
         "x:(-1.69885e+07, 2.21265e+06, 2.02132e+07),"
         " v:(-1670.69, -1985.6, -1151.13), clk bias:0.000294455,"
         " clk drift:-5.8669e-11, relcorr:-1.60472e-08,"
         " health:Unused";
   }


//=============================================================================
// General test for the SP3EphemerisStore
// Makes sure SP3EphemerisStore can be instantiated and can load
// a file; also ensures that nonexistent files throw an exception
//=============================================================================
   unsigned SP3ESTest()
   {
      TUDEF("SP3EphemerisStore", "Constructor");

         // Verify the consturctor builds the SP3EphemerisStore object
      try
      {
         SP3EphemerisStore store;
         TUPASS("SP3EphemerisStore object successfully created");
      }
      catch (...)
      {
         TUFAIL("SP3EphemerisStore object could not be created");
      }

      SP3EphemerisStore store;

         // Verify opening an empty file throws an error
      try
      {
         store.loadFile(inputNotaFile);
         TUFAIL("Opening an empty file did not throw an exception");
      }
      catch (Exception& e)
      {
         TUPASS("Opening an empty file threw the correct exception");
      }
      catch (...)
      {
         TUFAIL("Opening an empty file caused an unexpected exception");
      }

         // Verify opening a file works with no errors
      try
      {
         store.loadFile(inputSP3Data);
         TUPASS("Opening a valid file works with no exceptions");
      }
      catch (...)
      {
         TUFAIL("Exception thrown when opening a valid file");
      }

         // Write the dump of the loaded file
      ofstream dumpData;
      dumpData.open (outputDataDump.c_str());
      store.dump(dumpData,1);
      dumpData.close();

      TURETURN();
   }


      /// test loading of SP3c data
   unsigned sp3cTest()
   {
      TUDEF("SP3EphemerisStore", "whatever");
      SP3EphemerisStore store;
      TUCATCH(store.loadFile(inputSP3cData));
      TUASSERTE(size_t, 750, store.size());
      TURETURN();
   }


//=============================================================================
// Test for getXvt.
// Tests the getXvt method in SP3EphemerisStore by comparing known
// results with the method's output for various time stamps in an
// SP3 file; also ensures nonexistent SatIDs throw an exception
//=============================================================================
   unsigned getXvtTest()
   {
      TUDEF("SP3EphemerisStore", "getXvt");

      try
      {
         SP3EphemerisStore store;
         store.loadFile(inputSP3Data);

         stringstream outputStream1;
         stringstream outputStream15;
         stringstream outputStream31;

         const short PRN0 = 0; // Nonexistent in SP3 file
         const short PRN1 = 1;
         const short PRN15 = 15;
         const short PRN31 = 31;
         const short PRN32 = 32; // Nonexistent in SP3 file

         SatID sid0(PRN0,SatelliteSystem::GPS);
         SatID sid1(PRN1,SatelliteSystem::GPS);
         SatID sid15(PRN15,SatelliteSystem::GPS);
         SatID sid31(PRN31,SatelliteSystem::GPS);
         SatID sid32(PRN32,SatelliteSystem::GPS);

         CivilTime eTimeCiv(1997,4,6,6,15,0); // Time stamp of one epoch
         CommonTime eTime = eTimeCiv.convertToCommonTime();
         CivilTime bTimeCiv(1997,4,6,0,0,0); // Time stamp of first epoch
         CommonTime bTime = bTimeCiv.convertToCommonTime();

         try
         {
               // Verify that an InvalidRequest exception is thrown
               // when SatID is not in the data
            try
            {
               store.getXvt(sid0,bTime);
               TUFAIL("No exception thrown when getXvt looks for an invalid"
                      " SatID");
            }
            catch (InvalidRequest& e)
            {
               TUPASS("Expected exception thrown when getXvt looks for an"
                      " invalid SatID");
            }
            catch (...)
            {
               TUFAIL("Unexpected exception thrown when getXvt looks for an"
                      " invalid SatID");
            }

               // Verify that an InvalidRequest exception is thrown
               // when SatID is not in the data
            try
            {
               store.getXvt(sid32,bTime);
               TUFAIL("No exception thrown when getXvt looks for an invalid"
                      " SatID");
            }
            catch (InvalidRequest& e)
            {
               TUPASS("Expected exception thrown when getXvt looks for an"
                      " invalid SatID");
            }
            catch (...)
            {
               TUFAIL("Unexpected exception thrown when getXvt looks for an"
                      " invalid SatID");
            }

               // Verify that no exception is thrown for SatID in the data set
            try
            {
               store.getXvt(sid1,eTime);
               TUPASS("No exception thrown when getXvt looks for a valid"
                      " SatID");
            }
            catch (...)
            {
               TUFAIL("Exception thrown when getXvt looks for a valid SatID");
            }

            outputStream1 << store.getXvt(sid1,eTime);
            outputStream15 << store.getXvt(sid15,eTime);
            outputStream31 << store.getXvt(sid31,eTime);
         }

         catch (Exception& e)
         {
            cout << e;
         }

            // Were the values set to expectation using the explicit
            //constructor?
         TUASSERTE(std::string, inputComparisonOutput1, outputStream1.str());
         TUASSERTE(std::string, inputComparisonOutput15, outputStream15.str());
         TUASSERTE(std::string, inputComparisonOutput31, outputStream31.str());
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      TURETURN();
   }


//=============================================================================
// Test for computeXvt.
// Tests the computeXvt method in SP3EphemerisStore by comparing known
// results with the method's output for various time stamps in an
// SP3 file; also ensures nonexistent SatIDs throw an exception
//=============================================================================
   unsigned computeXvtTest()
   {
      TUDEF("SP3EphemerisStore", "computeXvt");

      try
      {
         SP3EphemerisStore store;
         stringstream outputStream1;
         stringstream outputStream15;
         stringstream outputStream31;
         Xvt rv;
         SatID sid0(0, SatelliteSystem::GPS);
         SatID sid1(1, SatelliteSystem::GPS);
         SatID sid15(15, SatelliteSystem::GPS);
         SatID sid31(31, SatelliteSystem::GPS);
         SatID sid32(32, SatelliteSystem::GPS);
         CommonTime eTime = CivilTime(1997,4,6,6,15,0,gpstk::TimeSystem::GPS);
         CommonTime bTime = CivilTime(1997,4,6,0,0,0,gpstk::TimeSystem::GPS);

         store.rejectBadPositions(false);
         store.rejectBadClocks(false);
         store.rejectPredPositions(false);
         store.rejectPredClocks(false);
         store.loadFile(inputSP3Data);

         TUCATCH(rv = store.computeXvt(sid0,bTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unavailable,rv.health);
         TUCATCH(rv = store.computeXvt(sid32,bTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unavailable,rv.health);
         TUCATCH(rv = store.computeXvt(sid1,eTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unused, rv.health);
         outputStream1 << store.computeXvt(sid1,eTime);
         outputStream15 << store.computeXvt(sid15,eTime);
         outputStream31 << store.computeXvt(sid31,eTime);

            // Were the values set to expectation using the explicit
            //constructor?
         TUASSERTE(std::string, inputComparisonOutput1, outputStream1.str());
         TUASSERTE(std::string, inputComparisonOutput15, outputStream15.str());
         TUASSERTE(std::string, inputComparisonOutput31, outputStream31.str());
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      TURETURN();
   }


//=============================================================================
// Test for getSVHealth.
// Tests the getSVHealth method in SP3EphemerisStore by comparing known
// results with the method's output for various time stamps in an
// SP3 file; also ensures nonexistent SatIDs throw an exception
//=============================================================================
   unsigned getSVHealthTest()
   {
      TUDEF("SP3EphemerisStore", "getSVHealth");

      try
      {
            // These are the same test queries used in computeXvt but
            // the health results expected are different given that
            // SP3 can provide Xvt data but not health data.
         SP3EphemerisStore store;
         Xvt::HealthStatus rv;
         SatID sid0(0, SatelliteSystem::GPS);
         SatID sid1(1, SatelliteSystem::GPS);
         SatID sid15(15, SatelliteSystem::GPS);
         SatID sid27(27, SatelliteSystem::GPS);
         SatID sid31(31, SatelliteSystem::GPS);
         SatID sid32(32, SatelliteSystem::GPS);
         CommonTime eTime = CivilTime(1997,4,6,6,15,0,gpstk::TimeSystem::GPS);
         CommonTime bTime = CivilTime(1997,4,6,0,0,0,gpstk::TimeSystem::GPS);

         store.rejectBadPositions(false);
         store.rejectBadClocks(false);
         store.rejectPredPositions(false);
         store.rejectPredClocks(false);
         store.loadFile(inputSP3Data);

         TUCATCH(rv = store.getSVHealth(sid0,bTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unused, rv);
         TUCATCH(rv = store.getSVHealth(sid32,bTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unused, rv);
         TUCATCH(rv = store.getSVHealth(sid1,eTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unused, rv);

         store.clear();
         TUASSERTE(int, 0, store.size());
         store.loadFile(inputSixNinesData);
         SatID sid4(4, SatelliteSystem::GPS);
         CommonTime cTime = CivilTime(2019,1,10,1,5,0,gpstk::TimeSystem::GPS);
            // PRN 4 has clock bias of 999999.999999 but a valid position
         TUCATCH(rv = store.getSVHealth(sid4, cTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unused, rv);
            // PRN 27 has an invalid position and clock bias
         TUCATCH(rv = store.getSVHealth(sid27, cTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unused, rv);
            // PRN 1 should be fine
         TUCATCH(rv = store.getSVHealth(sid1, cTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unused, rv);
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }
      TURETURN();
   }


//=============================================================================
// Test for getInitialTime
// Tests getInitialTime method in SP3EphemerisStore by ensuring that
// the method outputs the initial time stamp in an SP3 file
//=============================================================================
   unsigned getInitialTimeTest()
   {
      TUDEF("SP3EphemerisStore", "getInitialTime");

      try
      {
         SP3EphemerisStore store;
         store.loadFile(inputSP3Data);

         CommonTime computedInitialTime = store.getInitialTime();

         CivilTime knownInitialTimeCiv(1997,4,6,0,0,0);
         CommonTime knownInitialTime =
            knownInitialTimeCiv.convertToCommonTime();

            // Check that the function returns the initial time from the file
         TUASSERTE(CommonTime, knownInitialTime, computedInitialTime);
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      TURETURN();
   }


//=============================================================================
// Test for getFinalTime
// Tests getFinalTime method in SP3EphemerisStore by ensuring that
// the method outputs the final time stamp in an SP3 file
//=============================================================================
   unsigned getFinalTimeTest()
   {
      TUDEF("SP3EphemerisStore", "getFinalTime");

      try
      {
         SP3EphemerisStore store;
         store.loadFile(inputSP3Data);

         CommonTime computedFinalTime = store.getFinalTime();

         CivilTime knownFinalTimeCiv(1997,4,6,23,45,0);
         CommonTime knownFinalTime = knownFinalTimeCiv.convertToCommonTime();

            // Check that the function returns the initial time from the file
         TUASSERTE(CommonTime, knownFinalTime, computedFinalTime);
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      TURETURN();
   }
//=============================================================================
// Test for getPosition
// Tests getPosition method in SP3EphemerisStore by comparing the outputs
// of the method to known values in two SP3 files--one with position and
// velocity values and one with only position values
//=============================================================================
   unsigned getPositionTest()
   {
      TUDEF("SP3EphemerisStore", "getPosition");

      try
      {
         SP3EphemerisStore igsStore;
         igsStore.loadFile(inputSP3Data);

         const short PRN1 = 1;
         const short PRN31 = 31;

         CivilTime igsTimeCiv(1997,4,6,2,0,0);
         CommonTime igsTime = igsTimeCiv.convertToCommonTime();

         SatID sid1(PRN1,SatelliteSystem::GPS);
         SatID sid31(PRN31,SatelliteSystem::GPS);

         Triple computedPosition_igs1  = igsStore.getPosition(sid1,igsTime);
         Triple computedPosition_igs31 = igsStore.getPosition(sid31,igsTime);

         Triple knownPosition_igs1(-17432922.132,6688018.407,-18768291.053);
         Triple knownPosition_igs31(-5075919.490,25101160.691,-6633797.696);

         double relativeError;
         std::stringstream testMessageStream;
         std::string testMessageP1 =
            "getPosition obtained the wrong position in the ";
         std::string testMessageP2 = " direction for SatID 1";
            // Check that the computed position matches the known
            // value for SatID 1
         for (unsigned i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError  =
               fabs(knownPosition_igs1[i] - computedPosition_igs1[i]) /
               fabs(knownPosition_igs1[i]);
            testFramework.assert(relativeError < epsilon,
                                 testMessageStream.str(),
                                 __LINE__);
            testMessageStream.str(std::string());
         }

            //------------------------------------------------------------------
            // Check that the computed position matches the known
            // value for SatID 31
            //------------------------------------------------------------------
         testMessageP2 = " direction for SatID 31";
         for (unsigned i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError  = fabs(knownPosition_igs31[i]  -
                                  computedPosition_igs31[i]) /
               fabs(knownPosition_igs31[i]);
            testFramework.assert(relativeError < epsilon ,
                                 testMessageStream.str() ,
                                 __LINE__);
            testMessageStream.str(std::string());
         }

         SP3EphemerisStore apcStore;
         apcStore.loadFile(inputAPCData);

         CivilTime apcTimeCiv(2001,7,22,2,0,0);
         CommonTime apcTime = apcTimeCiv.convertToCommonTime();

         Triple computedPosition_apc1 = apcStore.getPosition(sid1,apcTime);
         Triple computedPosition_apc31 = apcStore.getPosition(sid31,apcTime);

         Triple knownPosition_apc1(-5327654.053,-16633919.811,20164748.602);
         Triple knownPosition_apc31(2170451.938,-22428932.839,-14059088.503);

            //------------------------------------------------------------------
            // Check that the computed position matches the known
            // value for SatID 1
            //------------------------------------------------------------------
         testMessageP2 = " direction for SatID 1";
         for (unsigned i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError = fabs(knownPosition_apc1[i]  -
                                 computedPosition_apc1[i]) /
               fabs(knownPosition_apc1[i]);
            testFramework.assert(relativeError < epsilon ,
                                 testMessageStream.str() ,
                                 __LINE__);
            testMessageStream.str(std::string());
         }

            //------------------------------------------------------------------
            // Check that the computed position matches the known
            // value for SatID 31
            //------------------------------------------------------------------
         testMessageP2 = " direction for SatID 31";
         for (unsigned i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError = fabs(knownPosition_apc31[i]  -
                                 computedPosition_apc31[i]) /
               fabs(knownPosition_apc31[i]);
            testFramework.assert(relativeError < epsilon ,
                                 testMessageStream.str() ,
                                 __LINE__);
            testMessageStream.str(std::string());
         }


            //------------------------------------------------------------------
            // Check that getSatList() and getIndexSet() return expected values
            // The data set has data for 29 SVs with PRN 12, PRN 16,
            // and PRN 32 missing
            //------------------------------------------------------------------
         set<SatID> expectedSet;
         for (unsigned i=1;i<=31;i++)
         {
            if (i!=12 && i!=16 && i!=32)
            {
               SatID sid(i,SatelliteSystem::GPS);
               expectedSet.insert(sid);
            }
         }
     
         vector<SatID> loadedList = apcStore.getSatList();
         set<SatID> loadedSet = apcStore.getIndexSet();
         TUASSERTE(unsigned,expectedSet.size(),loadedSet.size());
         TUASSERTE(unsigned,expectedSet.size(),loadedList.size());

         set<SatID>::const_iterator cit;
         for (cit=expectedSet.begin();cit!=expectedSet.end();cit++)
         {
            bool found = false;
            const SatID& sidr = *cit;
            if (loadedSet.find(sidr)!=loadedSet.end())
               found = true;
            TUASSERTE(bool,true,found);
         }

         vector<SatID>::const_iterator citl;
         for (citl=loadedList.begin();citl!=loadedList.end();citl++)
         {
            bool found = false;
            const SatID& sidr = *citl;
            if (expectedSet.find(sidr)!=expectedSet.end())
               found = true;
            TUASSERTE(bool,true,found);
         }

      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      TURETURN();
   }

//=============================================================================
// Test for getVelocity
// Tests getPosition method in SP3EphemerisStore by comparing the outputs
// of the method to known values in an SP3 files with position and
// velocity values
//=============================================================================
   unsigned getVelocityTest()
   {
      TUDEF("SP3EphemerisStore", "getVelocity");

      try
      {
         SP3EphemerisStore Store;
         Store.loadFile(inputAPCData);

         const short PRN1 = 1;
         const short PRN31 = 31;

         CivilTime testTimeCiv(2001,7,22,2,0,0);
         CommonTime testTime = testTimeCiv.convertToCommonTime();

         SatID sid1(PRN1,SatelliteSystem::GPS);
         SatID sid31(PRN31,SatelliteSystem::GPS);

         Triple computedVelocity_1 = Store.getVelocity(sid1,testTime);
         Triple computedVelocity_31 = Store.getVelocity(sid31,testTime);

         Triple knownVelocity_1(1541.6040306,-2000.8516260,-1256.4479944);
         Triple knownVelocity_31(1165.3672035,-1344.4254143,2399.1497704);

         double relativeError;
         std::stringstream testMessageStream;
         std::string testMessageP2,
            testMessageP1 = "getVelocity obtained the wrong velocity in the ";
            //------------------------------------------------------------------
            // Check that the computed position matches the known
            // value for SatID 1
            //------------------------------------------------------------------
         testMessageP2 = " direction for SatID 1";
         for (unsigned i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError = fabs(knownVelocity_1[i]  - computedVelocity_1[i]) /
               fabs(computedVelocity_1[i]);
            testFramework.assert(relativeError < epsilon ,
                                 testMessageStream.str() ,
                                 __LINE__);
            testMessageStream.str(std::string());
         }

            //------------------------------------------------------------------
            // Check that the computed position matches the known
            // value for SatID 1
            //------------------------------------------------------------------
         testMessageP2 = " direction for SatID 31";
         for (unsigned i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError = fabs(knownVelocity_31[i] - computedVelocity_31[i])/
               fabs(computedVelocity_31[i]);
            testFramework.assert(relativeError < epsilon ,
                                 testMessageStream.str() ,
                                 __LINE__);
            testMessageStream.str(std::string());
         }
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      TURETURN();
   }

private:
   double epsilon; // Floating point error threshold
   std::string dataFilePath;

   std::string inputSP3Data;
   std::string inputSP3cData;
   std::string inputAPCData;
   std::string inputSixNinesData;

   std::string outputDataDump;

   std::string inputNotaFile;

   std::string inputComparisonOutput1;
   std::string inputComparisonOutput15;
   std::string inputComparisonOutput31;
};


int main() // Main function to initialize and run all tests above
{
   unsigned errorTotal = 0;
   SP3EphemerisStore_T testClass;
   testClass.init();

   errorTotal += testClass.SP3ESTest();
   errorTotal += testClass.sp3cTest();
   errorTotal += testClass.getXvtTest();
   errorTotal += testClass.computeXvtTest();
   errorTotal += testClass.getSVHealthTest();
   errorTotal += testClass.getInitialTimeTest();
   errorTotal += testClass.getFinalTimeTest();
   errorTotal += testClass.getPositionTest();
   errorTotal += testClass.getVelocityTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
