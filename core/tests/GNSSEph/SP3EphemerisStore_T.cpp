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
		SP3EphemerisStore_T() {epsilon = 1E-12;}

/* =========================================================================================================================
	Initialize Test Data Filenames
========================================================================================================================= */
		void init( void )
		{
			dataFilePath = __FILE__;
			dataFilePath = dataFilePath.substr(0, dataFilePath.find_last_of("\\/"));

			inputSP3Data             =  dataFilePath + "/" + "igs09000.sp3";
			inputAPCData             =  dataFilePath + "/" + "apc01000";

			outputDataDump           =  dataFilePath + "/Logs/" + "SP3_DataDump.txt";

			inputNotaFile            =  dataFilePath + "/" + "NotaFILE";

			inputComparisonOutput1 = "x:(-1.51906e+07, -2.15539e+07, 3.31227e+06), v:(488.793, 118.124, 3125.01), clk bias:1.68268e-05, clk drift:1.93783e-11, relcorr:-8.45152e-09";
			inputComparisonOutput15 = "x:(-1.57075e+07, 1.72951e+07, 1.24252e+07), v:(408.54, -1568.11, 2651.16), clk bias:0.000411558, clk drift:3.22901e-12, relcorr:1.32734e-08";
			inputComparisonOutput31 = "x:(-1.69885e+07, 2.21265e+06, 2.02132e+07), v:(-1670.69, -1985.6, -1151.13), clk bias:0.000294455, clk drift:-5.8669e-11, relcorr:-1.60472e-08";
		}


/* =========================================================================================================================
	General test for the SP3EphemerisStore
	Makes sure SP3EphemerisStore can be instantiated and can load
	a file; also ensures that nonexistent files throw an exception
========================================================================================================================= */
		int SP3ESTest(void)
		{
			TestUtil testFramework( "SP3EphemerisStore", "Constructor", __FILE__, __LINE__ );
			testFramework.init();



//--------------SP3EphemerisStore_SP3ESTest_1 - Verify the consturctor builds the SP3EphemerisStore object
			try {SP3EphemerisStore Store; testFramework.passTest();}
			catch (...) {testFramework.failTest();}

			SP3EphemerisStore Store;

//--------------SP3EphemerisStore_SP3ESTest_2 - Verify opening an empty file throws an error
			try {Store.loadFile(inputNotaFile); testFramework.failTest();}
			catch (Exception& e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}

//--------------SP3EphemerisStore_SP3ESTest_3 - Verify opening a file works with no errors
			try {Store.loadFile(inputSP3Data); testFramework.passTest();}
			catch (...) {testFramework.failTest();}

			//Write the dump of the loaded file
			ofstream DumpData;
			DumpData.open (outputDataDump.c_str());
			Store.dump(DumpData,1);
			DumpData.close();

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test for getXvt.
	Tests the getXvt method in SP3EphemerisStore by comparing known
	results with the method's output for various time stamps in an
	SP3 file; also ensures nonexistent SatIDs throw an exception
========================================================================================================================= */
		int getXvtTest (void)
		{
			TestUtil testFramework( "SP3EphemerisStore", "getXvt", __FILE__, __LINE__ );
			testFramework.init();

			SP3EphemerisStore Store;
			Store.loadFile(inputSP3Data);

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
//--------------SP3EphemerisStore_getXvtTest_1 - Verify that an InvalidRequest exception is thrown when SatID is not in the data
				try {Store.getXvt(sid0,bTime); testFramework.failTest();}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------SP3EphemerisStore_getXvtTest_2 - Verify that an InvalidRequest exception is thrown when SatID is not in the data
				try {Store.getXvt(sid32,bTime); testFramework.failTest();}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------SP3EphemerisStore_getXvtTest_3 - Verify that no exception is thrown for SatID in the data set
				try {Store.getXvt(sid1,eTime); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

				outputStream1 << Store.getXvt(sid1,eTime);
				outputStream15 << Store.getXvt(sid15,eTime);
				outputStream31 << Store.getXvt(sid31,eTime);
			}

			catch (Exception& e)
			{
				cout << e;
			}

//--------------SP3EphemerisStore_getXvtTest_4 - Check getXvt output with pre-determined standard
			testFramework.assert( outputStream1.str() == inputComparisonOutput1 );
			testFramework.next();

//--------------SP3EphemerisStore_getXvtTest_5 - Check getXvt output with pre-determined standard
			testFramework.assert( outputStream15.str() == inputComparisonOutput15 );
			testFramework.next();

//--------------SP3EphemerisStore_getXvtTest_6 - Check getXvt output with pre-determined standard
			testFramework.assert( outputStream31.str() == inputComparisonOutput31 );
			testFramework.next();

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test for getInitialTime
	Tests getInitialTime method in SP3EphemerisStore by ensuring that
	the method outputs the initial time stamp in an SP3 file
========================================================================================================================= */
		int getInitialTimeTest (void)
		{
			TestUtil testFramework( "SP3EphemerisStore", "getInitialTime", __FILE__, __LINE__ );
			testFramework.init();

			SP3EphemerisStore Store;
			Store.loadFile(inputSP3Data);

			CommonTime computedInitialTime = Store.getInitialTime();

			CivilTime knownInitialTime_civ(1997,4,6,0,0,0);
			CommonTime knownInitialTime = knownInitialTime_civ.convertToCommonTime();

//--------------SP3EphemerisStore_getInitialTimeTest_1 - Check that the function returns the initial time from the file
			testFramework.assert(knownInitialTime == computedInitialTime);

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test for getFinalTime
	Tests getFinalTime method in SP3EphemerisStore by ensuring that
	the method outputs the final time stamp in an SP3 file
========================================================================================================================= */
		int getFinalTimeTest (void)
		{
			TestUtil testFramework( "SP3EphemerisStore", "getFinalTime", __FILE__, __LINE__ );
			testFramework.init();

			SP3EphemerisStore Store;
			Store.loadFile(inputSP3Data);

			CommonTime computedFinalTime = Store.getFinalTime();

			CivilTime knownFinalTime_civ(1997,4,6,23,45,0);
			CommonTime knownFinalTime = knownFinalTime_civ.convertToCommonTime();

//--------------SP3EphemerisStore_getFinalTimeTest_1 - Check that the function returns the initial time from the file
			testFramework.assert(knownFinalTime == computedFinalTime);

			return testFramework.countFails();
		}
/* =========================================================================================================================
	Test for getPosition
	Tests getPosition method in SP3EphemerisStore by comparing the outputs
	of the method to known values in two SP3 files--one with position and
	velocity values and one with only position values
========================================================================================================================= */
		int getPositionTest (void)
		{
			TestUtil testFramework( "SP3EphemerisStore", "getPosition", __FILE__, __LINE__ );
			testFramework.init();

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

//--------------SP3EphemerisStore_getPositionTest_1 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_igs1[0] - computedPosition_igs1[0])/fabs(knownPosition_igs1[0]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_2 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_igs1[1] - computedPosition_igs1[1])/fabs(knownPosition_igs1[1]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_3 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_igs1[2] - computedPosition_igs1[2])/fabs(knownPosition_igs1[2]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_4 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_igs31[0] - computedPosition_igs31[0])/fabs(knownPosition_igs31[0]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_5 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_igs31[1] - computedPosition_igs31[1])/fabs(knownPosition_igs31[1]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_6 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_igs31[2] - computedPosition_igs31[2])/fabs(knownPosition_igs31[2]) < epsilon);
			testFramework.next();

			SP3EphemerisStore apcStore;
			apcStore.loadFile(inputAPCData);

			CivilTime apcTime_civ(2001,7,22,2,0,0);
			CommonTime apcTime = apcTime_civ.convertToCommonTime();

			Triple computedPosition_apc1 = apcStore.getPosition(sid1,apcTime);
			Triple computedPosition_apc31 = apcStore.getPosition(sid31,apcTime);

			Triple knownPosition_apc1(-5327654.053,-16633919.811,20164748.602);
			Triple knownPosition_apc31(2170451.938,-22428932.839,-14059088.503);

//--------------SP3EphemerisStore_getPositionTest_7 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_apc1[0] - computedPosition_apc1[0])/fabs(knownPosition_apc1[0]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_8 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_apc1[1] - computedPosition_apc1[1])/fabs(knownPosition_apc1[1]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_9 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_apc1[2] - computedPosition_apc1[2])/fabs(knownPosition_apc1[2]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_10 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_apc31[0] - computedPosition_apc31[0])/fabs(knownPosition_apc31[0]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_11 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_apc31[1] - computedPosition_apc31[1])/fabs(knownPosition_apc31[1]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getPositionTest_12 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownPosition_apc31[2] - computedPosition_apc31[2])/fabs(knownPosition_apc31[2]) < epsilon);
			testFramework.next();

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test for getVelocity
	Tests getPosition method in SP3EphemerisStore by comparing the outputs
	of the method to known values in an SP3 files with position and
	velocity values
========================================================================================================================= */
		int getVelocityTest (void)
		{
			TestUtil testFramework( "SP3EphemerisStore", "getVelocity", __FILE__, __LINE__ );
			testFramework.init();

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

//--------------SP3EphemerisStore_getVelocityTest_1 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownVelocity_1[0] - computedVelocity_1[0])/fabs(knownVelocity_1[0]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getVelocityTest_2 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownVelocity_1[1] - computedVelocity_1[1])/fabs(knownVelocity_1[1]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getVelocityTest_3 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownVelocity_1[2] - computedVelocity_1[2])/fabs(knownVelocity_1[2]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getVelocityTest_4 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownVelocity_31[0] - computedVelocity_31[0])/fabs(knownVelocity_31[0]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getVelocityTest_5 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownVelocity_31[1] - computedVelocity_31[1])/fabs(knownVelocity_31[1]) < epsilon);
			testFramework.next();

//--------------SP3EphemerisStore_getVelocityTest_6 - Check that the computed position matches the known value
			testFramework.assert(fabs(knownVelocity_31[2] - computedVelocity_31[2])/fabs(knownVelocity_31[2]) < epsilon);
			testFramework.next();

			return testFramework.countFails();
		}

    private:
	double epsilon; //Floating point error threshold
        std::string dataFilePath;

        std::string inputSP3Data;
        std::string inputAPCData;

        std::string outputDataDump;

        std::string inputNotaFile;

	std::string inputComparisonOutput1;
	std::string inputComparisonOutput15;
	std::string inputComparisonOutput31;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	SP3EphemerisStore_T testClass;
	testClass.init();

	check = testClass.SP3ESTest();
	errorCounter += check;

	check = testClass.getXvtTest();
	errorCounter += check;

	check = testClass.getInitialTimeTest();
	errorCounter += check;

	check = testClass.getFinalTimeTest();
	errorCounter += check;

	check = testClass.getPositionTest();
	errorCounter += check;

	check = testClass.getVelocityTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
