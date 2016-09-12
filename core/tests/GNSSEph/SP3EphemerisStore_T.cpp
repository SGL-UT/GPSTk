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
   void init( void )
   {
      TestUtil test0;
      std::string dataFilePath = gpstk::getPathData();
      std::string tempFilePath = gpstk::getPathTestTemp();
      std::string file_sep = "/";

      inputSP3Data             =  dataFilePath + file_sep +
         "test_input_sp3_nav_ephemerisData.sp3";
      inputAPCData             =  dataFilePath + file_sep +
         "test_input_sp3_nav_apcData.sp3";
      inputNotaFile            =  dataFilePath + file_sep + "NotaFILE";

      outputDataDump           =  tempFilePath + file_sep + "SP3_DataDump.txt";

      inputComparisonOutput1 =
         "x:(-1.51906e+07, -2.15539e+07, 3.31227e+06),"
         " v:(488.793, 118.124, 3125.01), clk bias:1.68268e-05,"
         " clk drift:1.93783e-11, relcorr:-8.45152e-09";
      inputComparisonOutput15 =
         "x:(-1.57075e+07, 1.72951e+07, 1.24252e+07),"
         " v:(408.54, -1568.11, 2651.16), clk bias:0.000411558,"
         " clk drift:3.22901e-12, relcorr:1.32734e-08";
      inputComparisonOutput31 =
         "x:(-1.69885e+07, 2.21265e+06, 2.02132e+07),"
         " v:(-1670.69, -1985.6, -1151.13), clk bias:0.000294455,"
         " clk drift:-5.8669e-11, relcorr:-1.60472e-08";
   }


//=============================================================================
// General test for the SP3EphemerisStore
// Makes sure SP3EphemerisStore can be instantiated and can load
// a file; also ensures that nonexistent files throw an exception
//=============================================================================
   int SP3ESTest(void)
   {
      TUDEF( "SP3EphemerisStore", "Constructor" );

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
/*
  fstream smoo;
  smoo.open(inputSP3Data.c_str(),std::ios::in);
  testFramework.assert(smoo, "plain file open fail", __LINE__);
  smoo.close();
  cerr << "-------------------------" << endl;
*/
         store.loadFile(inputSP3Data);
//         cerr << "-------------------------" << endl;
         TUPASS("Opening a valid file works with no exceptions");
      }
      catch (...)
      {
//         cerr << "-------------------------" << endl;
         TUFAIL("Exception thrown when opening a valid file");
      }

         // Write the dump of the loaded file
      ofstream DumpData;
      DumpData.open (outputDataDump.c_str());
      store.dump(DumpData,1);
      DumpData.close();

      return testFramework.countFails();
   }


//=============================================================================
// Test for getXvt.
// Tests the getXvt method in SP3EphemerisStore by comparing known
// results with the method's output for various time stamps in an
// SP3 file; also ensures nonexistent SatIDs throw an exception
//=============================================================================
   int getXvtTest (void)
   {
      TUDEF( "SP3EphemerisStore", "getXvt" );

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

         SatID sid0(PRN0,SatID::systemGPS);
         SatID sid1(PRN1,SatID::systemGPS);
         SatID sid15(PRN15,SatID::systemGPS);
         SatID sid31(PRN31,SatID::systemGPS);
         SatID sid32(PRN32,SatID::systemGPS);

         CivilTime eTime_civ(1997,4,6,6,15,0); // Time stamp of one epoch
         CommonTime eTime = eTime_civ.convertToCommonTime();
         CivilTime bTime_civ(1997,4,6,0,0,0); // Time stamp of first epoch
         CommonTime bTime = bTime_civ.convertToCommonTime();

         try
         {
               // Verify that an InvalidRequest exception is thrown when SatID is not in the data
            try
            {
               store.getXvt(sid0,bTime);
               TUFAIL("No exception thrown when getXvt looks for an invalid"
                      " SatID");
            }
            catch (InvalidRequest& e)
            {
               TUPASS("Expected exception thrown when getXvt looks for an invalid"
                      " SatID");
            }
            catch (...)
            {
               TUFAIL("Unexpected exception thrown when getXvt looks for an"
                      " invalid SatID");
            }

               // Verify that an InvalidRequest exception is thrown when SatID is not in the data
            try
            {
               store.getXvt(sid32,bTime);
               TUFAIL("No exception thrown when getXvt looks for an invalid"
                      " SatID");
            }
            catch (InvalidRequest& e)
            {
               TUPASS("Expected exception thrown when getXvt looks for an invalid"
                      " SatID");
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
               TUPASS("No exception thrown when getXvt looks for a valid SatID");
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

            //--------------------------------------------------------------------
            // Were the values set to expectation using the explicit constructor?
            //--------------------------------------------------------------------
         TUASSERTE(std::string, inputComparisonOutput1, outputStream1.str());
         TUASSERTE(std::string, inputComparisonOutput15, outputStream15.str());
         TUASSERTE(std::string, inputComparisonOutput31, outputStream31.str());
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      return testFramework.countFails();
   }


//=============================================================================
// Test for getInitialTime
// Tests getInitialTime method in SP3EphemerisStore by ensuring that
// the method outputs the initial time stamp in an SP3 file
//=============================================================================
   int getInitialTimeTest (void)
   {
      TUDEF( "SP3EphemerisStore", "getInitialTime" );

      try
      {
         SP3EphemerisStore store;
         store.loadFile(inputSP3Data);

         CommonTime computedInitialTime = store.getInitialTime();

         CivilTime knownInitialTime_civ(1997,4,6,0,0,0);
         CommonTime knownInitialTime = knownInitialTime_civ.convertToCommonTime();

            // Check that the function returns the initial time from the file
         TUASSERTE(CommonTime, knownInitialTime, computedInitialTime);
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      return testFramework.countFails();
   }


//=============================================================================
// Test for getFinalTime
// Tests getFinalTime method in SP3EphemerisStore by ensuring that
// the method outputs the final time stamp in an SP3 file
//=============================================================================
   int getFinalTimeTest (void)
   {
      TUDEF( "SP3EphemerisStore", "getFinalTime" );

      try
      {
         SP3EphemerisStore store;
         store.loadFile(inputSP3Data);

         CommonTime computedFinalTime = store.getFinalTime();

         CivilTime knownFinalTime_civ(1997,4,6,23,45,0);
         CommonTime knownFinalTime = knownFinalTime_civ.convertToCommonTime();

            // Check that the function returns the initial time from the file
         TUASSERTE(CommonTime, knownFinalTime, computedFinalTime);
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      return testFramework.countFails();
   }
//=============================================================================
// Test for getPosition
// Tests getPosition method in SP3EphemerisStore by comparing the outputs
// of the method to known values in two SP3 files--one with position and
// velocity values and one with only position values
//=============================================================================
   int getPositionTest (void)
   {
      TUDEF( "SP3EphemerisStore", "getPosition" );

      try
      {
         SP3EphemerisStore igsStore;
         igsStore.loadFile(inputSP3Data);

         const short PRN1 = 1;
         const short PRN31 = 31;

         CivilTime igsTime_civ(1997,4,6,2,0,0);
         CommonTime igsTime = igsTime_civ.convertToCommonTime();

         SatID sid1(PRN1,SatID::systemGPS);
         SatID sid31(PRN31,SatID::systemGPS);

         Triple computedPosition_igs1  = igsStore.getPosition(sid1,igsTime);
         Triple computedPosition_igs31 = igsStore.getPosition(sid31,igsTime);

         Triple knownPosition_igs1(-17432922.132,6688018.407,-18768291.053);
         Triple knownPosition_igs31(-5075919.490,25101160.691,-6633797.696);

         double relativeError;
         std::stringstream testMessageStream;
         std::string testMessageP1 = "getPosition obtained the wrong position in the ";
         std::string testMessageP2 = " direction for SatID 1";
            //--------------------------------------------------------------------
            // Check that the computed position matches the known value for SatID 1
            //--------------------------------------------------------------------
         for (int i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError  = fabs(knownPosition_igs1[i]  - computedPosition_igs1[i]) /fabs(
               knownPosition_igs1[i] );
            testFramework.assert( relativeError < epsilon , testMessageStream.str() ,
                                  __LINE__);
            testMessageStream.str(std::string());
         }

            //--------------------------------------------------------------------
            // Check that the computed position matches the known value for SatID 31
            //--------------------------------------------------------------------
         testMessageP2 = " direction for SatID 31";
         for (int i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError  = fabs(knownPosition_igs31[i]  -
                                  computedPosition_igs31[i]) /fabs(knownPosition_igs31[i] );
            testFramework.assert( relativeError < epsilon , testMessageStream.str() ,
                                  __LINE__);
            testMessageStream.str(std::string());
         }

         SP3EphemerisStore apcStore;
         apcStore.loadFile(inputAPCData);

         CivilTime apcTime_civ(2001,7,22,2,0,0);
         CommonTime apcTime = apcTime_civ.convertToCommonTime();

         Triple computedPosition_apc1 = apcStore.getPosition(sid1,apcTime);
         Triple computedPosition_apc31 = apcStore.getPosition(sid31,apcTime);

         Triple knownPosition_apc1(-5327654.053,-16633919.811,20164748.602);
         Triple knownPosition_apc31(2170451.938,-22428932.839,-14059088.503);

            //--------------------------------------------------------------------
            // Check that the computed position matches the known value for SatID 1
            //--------------------------------------------------------------------
         testMessageP2 = " direction for SatID 1";
         for (int i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError = fabs(knownPosition_apc1[i]  - computedPosition_apc1[i]) /fabs(
               knownPosition_apc1[i] );
            testFramework.assert( relativeError < epsilon , testMessageStream.str() ,
                                  __LINE__);
            testMessageStream.str(std::string());
         }

            //--------------------------------------------------------------------
            // Check that the computed position matches the known value for SatID 31
            //--------------------------------------------------------------------
         testMessageP2 = " direction for SatID 31";
         for (int i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError = fabs(knownPosition_apc31[i]  -
                                 computedPosition_apc31[i]) /fabs(knownPosition_apc31[i] );
            testFramework.assert( relativeError < epsilon , testMessageStream.str() ,
                                  __LINE__);
            testMessageStream.str(std::string());
         }
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      return testFramework.countFails();
   }


//=============================================================================
// Test for getVelocity
// Tests getPosition method in SP3EphemerisStore by comparing the outputs
// of the method to known values in an SP3 files with position and
// velocity values
//=============================================================================
   int getVelocityTest (void)
   {
      TUDEF( "SP3EphemerisStore", "getVelocity" );

      try
      {
         SP3EphemerisStore Store;
         Store.loadFile(inputAPCData);

         const short PRN1 = 1;
         const short PRN31 = 31;

         CivilTime testTime_civ(2001,7,22,2,0,0);
         CommonTime testTime = testTime_civ.convertToCommonTime();

         SatID sid1(PRN1,SatID::systemGPS);
         SatID sid31(PRN31,SatID::systemGPS);

         Triple computedVelocity_1 = Store.getVelocity(sid1,testTime);
         Triple computedVelocity_31 = Store.getVelocity(sid31,testTime);

         Triple knownVelocity_1(1541.6040306,-2000.8516260,-1256.4479944);
         Triple knownVelocity_31(1165.3672035,-1344.4254143,2399.1497704);

         double relativeError;
         std::stringstream testMessageStream;
         std::string testMessageP2,
            testMessageP1 = "getVelocity obtained the wrong velocity in the ";
            //--------------------------------------------------------------------
            // Check that the computed position matches the known value for SatID 1
            //--------------------------------------------------------------------
         testMessageP2 = " direction for SatID 1";
         for (int i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError = fabs(knownVelocity_1[i]  - computedVelocity_1[i]) /fabs(
               computedVelocity_1[i] );
            testFramework.assert( relativeError < epsilon , testMessageStream.str() ,
                                  __LINE__);
            testMessageStream.str(std::string());
         }

            //--------------------------------------------------------------------
            // Check that the computed position matches the known value for SatID 1
            //--------------------------------------------------------------------
         testMessageP2 = " direction for SatID 31";
         for (int i = 0; i < 3; i++)
         {
            testMessageStream << testMessageP1 << i << testMessageP2;
            relativeError = fabs(knownVelocity_31[i] - computedVelocity_31[i])/fabs(
               computedVelocity_31[i]);
            testFramework.assert( relativeError < epsilon , testMessageStream.str() ,
                                  __LINE__);
            testMessageStream.str(std::string());
         }
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }

      return testFramework.countFails();
   }

private:
   double epsilon; // Floating point error threshold
   std::string dataFilePath;

   std::string inputSP3Data;
   std::string inputAPCData;

   std::string outputDataDump;

   std::string inputNotaFile;

   std::string inputComparisonOutput1;
   std::string inputComparisonOutput15;
   std::string inputComparisonOutput31;
};


int main() // Main function to initialize and run all tests above
{
   int errorTotal = 0;
   SP3EphemerisStore_T testClass;
   testClass.init();

   errorTotal += testClass.SP3ESTest();
   errorTotal += testClass.getXvtTest();
   errorTotal += testClass.getInitialTimeTest();
   errorTotal += testClass.getFinalTimeTest();
   errorTotal += testClass.getPositionTest();
   errorTotal += testClass.getVelocityTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
