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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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


void xSP3EphemerisStore :: setUp (void)
{
}

// General test for the SP3EphemerisStore

void xSP3EphemerisStore :: SP3Test(void)
{
	ofstream DumpData;
		DumpData.open ("DumpData.txt");

		CPPUNIT_ASSERT_NO_THROW(gpstk::SP3EphemerisStore Store);
		gpstk::SP3EphemerisStore Store;
		try
		{
		  CPPUNIT_ASSERT_THROW(Store.loadFile("NotaFILE"),gpstk::FileMissingException);
		}
		catch (gpstk::Exception& e)
		{
		  cout << "unexpected exception thrown" << endl;
		  cout << e << endl;
		}

		CPPUNIT_ASSERT_NO_THROW(Store.loadFile("igs09000.sp3"));
		Store.loadFile("igs09000.sp3");
		Store.dump(DumpData,1);
		DumpData.close();
}

// Test for getXvt

void xSP3EphemerisStore :: SP3getXvtTest (void)
{
	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN31;

	fPRN1.open ("Logs/getXvt1.txt");
	fPRN15.open ("Logs/getXvt15.txt");
	fPRN31.open ("Logs/getXvt31.txt");

	gpstk::SP3EphemerisStore Store;
	Store.loadFile("igs09000.sp3");

	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN31 = 31;
	const short PRN32 = 32;  //Top PRN (33) (Border test case);
    gpstk::SatID sid0(PRN0,gpstk::SatID::systemGPS);
    gpstk::SatID sid1(PRN1,gpstk::SatID::systemGPS);
    gpstk::SatID sid15(PRN15,gpstk::SatID::systemGPS);
    gpstk::SatID sid32(PRN31,gpstk::SatID::systemGPS);
    gpstk::SatID sid33(PRN32,gpstk::SatID::systemGPS);

	gpstk::CommonTime eTime = gpstk::CommonTime(1997,4,6,6,15,0);  //Epoch Time (Exact time of epoch)
	gpstk::CommonTime bTime = gpstk::CommonTime(1997,4,6,0,0,0);   //Border Time (Time of Border test cases)

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.getXvt(sid1,eTime));

		fPRN1 << Store.getXvt(sid1,eTime) << endl;
		fPRN15 << Store.getXvt(sid15,eTime) << endl;
		fPRN31 << Store.getXvt(sid31,eTime) << endl;

		CPPUNIT_ASSERT_THROW(Store.getXvt(sid0,bTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(Store.getXvt(sid33,bTime),gpstk::InvalidRequest);
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/getXvt1.txt",(char*)"Checks/getPrnXvt1.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/getXvt15.txt",(char*)"Checks/getPrnXvt15.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/getXvt31.txt",(char*)"Checks/getPrnXvt31.chk"));
}


// Test for getInitialTime

void xSP3EphemerisStore :: SP3getInitialTimeTest (void)
{
	gpstk::SP3EphemerisStore Store;
	Store.loadFile("igs09000.sp3");

	gpstk::CommonTime computedInitialTime = Store.getInitialTime;

	gpstk::CommonTime knownInitialTime = gpstk::CommonTime(1997,4,6,0,0,0);

	CPPUNIT_ASSERT_EQUAL(knownInitialTime,computedInitialTime);
}

// Test for getFinalTime

void xSP3EphemerisStore :: SP3getFinalTimeTest (void)
{
	gpstk::SP3EphemerisStore Store;
	Store.loadFile("igs09000.sp3");

	gpstk::CommonTime computedFinalTime = Store.getFinalTime;

	gpstk::CommonTime knownFinalTime = CommonTime(1997,4,6,23,45,0);

	CPPUNIT_ASSERT_EQUAL(knownFinalTime,computedFinalTime);
}

// Test for getPosition

void xSP3EphemerisStore :: SP3getPositionTest (void)
{
	gpstk::SP3EphemerisStore Store;
	Store.loadFile("igs09000.sp3");

	const short PRN1 = 1;
	const short PRN15 = 15;

	gpstk::CommonTime testTime(1997,4,6,2,0,0);

	gpstk::SatID sid1(PRN1,gpstk::SatID::systemGPS);
	gpstk::SatID sid15(PRN15,gpstk::SatID::systemGPS);

	gpstk::Triple computedPosition_1 = Store.getPosition(sid1,testTime);
	gpstk::Triple computedPosition_15 = Store.getPosition(sid15,testTime);

	gpstk::Triple knownPosition_1 = gpstk::Triple(-17432.922132,6688.018407,-18768.291053);
	gpstk::Triple knownPosition_15 = gpstk::Triple(2484.685581,14739.428502,-22173.216899);

	CPPUNIT_ASSERT_EQUAL(knownPosition_1,computedPosition_1);
	CPPUNIT_ASSERT_EQUAL(knownPosition_15,computedPosition_15);
}

// Test for getVelocity

void xSP3EphemerisStore :: SP3getVelocityTest (void)
{

}
