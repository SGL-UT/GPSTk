// file: xSP3EphemerisStore.cpp

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "xSP3EphemerisStore.hpp"
#include "SatID.hpp"
#include "Exception.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "SP3EphemerisStore.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xSP3EphemerisStore);

using namespace gpstk;

void xSP3EphemerisStore :: setUp (void)
{
}

/*
 * General test for the SP3EphemerisStore
 * -- Makes sure SP3EphemerisStore can be instantiated and can load
 * -- a file; also ensures that nonexistent files throw an exception
 */

void xSP3EphemerisStore :: SP3Test(void)
{
	ofstream DumpData;
	DumpData.open ("Logs/DumpData.txt");
	CPPUNIT_ASSERT_NO_THROW(SP3EphemerisStore Store);
	SP3EphemerisStore Store;
	CPPUNIT_ASSERT_THROW(Store.loadFile("NotaFILE"),Exception);
	CPPUNIT_ASSERT_NO_THROW(Store.loadFile("igs09000.sp3"));
	Store.dump(DumpData,1);
	DumpData.close();
}

/*
 * Test for getXvt
 * -- Tests the getXvt method in SP3EphemerisStore by comparing known
 * -- results with the method's output for various time stamps in an
 * -- SP3 file; also ensures nonexistent SatIDs throw an exception
 */

void xSP3EphemerisStore :: SP3getXvtTest (void)
{
	SP3EphemerisStore Store;
	Store.loadFile("igs09000.sp3");

	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN31;

	fPRN1.open ("Logs/findXvt1.txt");
	fPRN15.open ("Logs/findXvt15.txt");
	fPRN31.open ("Logs/findXvt31.txt");

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
		CPPUNIT_ASSERT_NO_THROW(Store.getXvt(sid1,eTime));

		fPRN1 << Store.getXvt(sid1,eTime);
		fPRN15 << Store.getXvt(sid15,eTime);
		fPRN31 << Store.getXvt(sid31,eTime);

		fPRN1.close();
		fPRN15.close();
		fPRN31.close();

		CPPUNIT_ASSERT_THROW(Store.getXvt(sid0,bTime),InvalidRequest);
		CPPUNIT_ASSERT_THROW(Store.getXvt(sid32,bTime),InvalidRequest);
	}

	catch (Exception& e)
	{
		cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/findXvt1.txt",(char*)"Checks/findXvt1.txt"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/findXvt15.txt",(char*)"Checks/findXvt15.txt"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/findXvt31.txt",(char*)"Checks/findXvt31.txt"));

}

/*
 * Test for getInitialTime
 * -- Tests getInitialTime method in SP3EphemerisStore by ensuring that
 * -- the method outputs the initial time stamp in an SP3 file
 */

void xSP3EphemerisStore :: SP3getInitialTimeTest (void)
{
	SP3EphemerisStore Store;
	Store.loadFile("igs09000.sp3");

	CommonTime computedInitialTime = Store.getInitialTime();

	CivilTime knownInitialTime_civ(1997,4,6,0,0,0);
	CommonTime knownInitialTime = knownInitialTime_civ.convertToCommonTime();

	CPPUNIT_ASSERT_EQUAL(knownInitialTime,computedInitialTime);
}

/*
 * Test for getFinalTime
 * -- Tests getFinalTime method in SP3EphemerisStore by ensuring that
 * -- the method outputs the final time stamp in an SP3 file
 */

void xSP3EphemerisStore :: SP3getFinalTimeTest (void)
{
	SP3EphemerisStore Store;
	Store.loadFile("igs09000.sp3");

	CommonTime computedFinalTime = Store.getFinalTime();

	CivilTime knownFinalTime_civ(1997,4,6,23,45,0);
	CommonTime knownFinalTime = knownFinalTime_civ.convertToCommonTime();

	CPPUNIT_ASSERT_EQUAL(knownFinalTime,computedFinalTime);
}

/*
 * Test for getPosition
 * -- Tests getPosition method in SP3EphemerisStore by comparing the outputs
 * -- of the method to known values in two SP3 files--one with position and
 * -- velocity values and one with only position values
 */

void xSP3EphemerisStore :: SP3getPositionTest (void)
{

	SP3EphemerisStore igsStore;
	igsStore.loadFile("igs09000.sp3");

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

	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_igs1[0],computedPosition_igs1[0],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_igs1[1],computedPosition_igs1[1],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_igs1[2],computedPosition_igs1[2],0.001);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_igs31[0],computedPosition_igs31[0],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_igs31[1],computedPosition_igs31[1],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_igs31[2],computedPosition_igs31[2],0.001);

	SP3EphemerisStore apcStore;
	apcStore.loadFile("apc01000");

	CivilTime apcTime_civ(2001,7,22,2,0,0);
	CommonTime apcTime = apcTime_civ.convertToCommonTime();

	Triple computedPosition_apc1 = apcStore.getPosition(sid1,apcTime);
	Triple computedPosition_apc31 = apcStore.getPosition(sid31,apcTime);

	Triple knownPosition_apc1(-5327654.053,-16633919.811,20164748.602);
	Triple knownPosition_apc31(2170451.938,-22428932.839,-14059088.503);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_apc1[0],computedPosition_apc1[0],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_apc1[1],computedPosition_apc1[1],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_apc1[2],computedPosition_apc1[2],0.001);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_apc31[0],computedPosition_apc31[0],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_apc31[1],computedPosition_apc31[1],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownPosition_apc31[2],computedPosition_apc31[2],0.001);

}

/*
 * Test for getVelocity
 * -- Tests getPosition method in SP3EphemerisStore by comparing the outputs
 * -- of the method to known values in an SP3 files with position and
 * -- velocity values
 */

void xSP3EphemerisStore :: SP3getVelocityTest (void)
{
	SP3EphemerisStore Store;
	Store.loadFile("apc01000");

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

	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownVelocity_1[0],computedVelocity_1[0],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownVelocity_1[1],computedVelocity_1[1],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownVelocity_1[2],computedVelocity_1[2],0.001);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownVelocity_31[0],computedVelocity_31[0],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownVelocity_31[1],computedVelocity_31[1],0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(knownVelocity_31[2],computedVelocity_31[2],0.001);

}

/*
 * Can compare two files--necessary for getXvt test
 */
bool xSP3EphemerisStore :: fileEqualTest (char* handle1, char* handle2)
{
	bool isEqual = false;

	ifstream File1;
	ifstream File2;

	std::string File1Line;
	std::string File2Line;

	File1.open(handle1);
	File2.open(handle2);

	while (!File1.eof())
	{
		if (File2.eof())
			return isEqual;
		getline (File1, File1Line);
		getline (File2, File2Line);

		if (File1Line != File2Line)
			return isEqual;
	}
	if (!File2.eof())
		return isEqual;
	else
		return isEqual = true;
}
