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

#include "RinexEphemerisStore.hpp"
#include "Exception.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "RinexNavData.hpp"
#include "EngEphemeris.hpp"
#include "GPSEphemerisStore.hpp"
#include "SatID.hpp"
#include "RinexNavStream.hpp"
#include "TestUtil.hpp"


using namespace gpstk;
using namespace std;


class RinexEphemerisStore_T
{
	public:
		RinexEphemerisStore_T() {}

		/*
		**** General test for the RinexEphemerisStore (RES) class
		**** Test to assure the that RES throws its exceptions in the right place and
		**** that it loads the RINEX Nav file correctly

		**** To further check this data, please view DumpData.txt for the dumped information
		*/

		int RESTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "Constructor", __FILE__, __func__ );
			testFramework.init();

			ofstream DumpData;
			DumpData.open ("DumpData.txt");

			try {RinexEphemerisStore Store; testFramework.passTest();}
			catch (...) {testFramework.failTest();}
			RinexEphemerisStore Store; 

			//------------------------------------------------------------------------------
			// SHOULDN'T THIS FAIL?
			testFramework.changeSourceMethod("loadFile");
			try
			{
			  Store.loadFile("NotaFILE");
			  testFramework.passTest();
			}
			catch (Exception& e)
			{
			  cout << "Expected exception thrown " << endl;
			  cout << e << endl;
			  testFramework.failTest();
			}
			//------------------------------------------------------------------------------

			try {Store.loadFile("TestRinex06.031"); testFramework.passTest();}
			catch (...) {cout << "Checking for failure!!!!" << endl; testFramework.failTest();}

			// Clear the store after invoking for assert_no_throw before loading file again to avoid 
			// duplicate file name error

			RinexEphemerisStore Store2;

			try {Store2.loadFile("TestRinex06.031");}
			catch (Exception& e)
			{
			  cout << " Exception received from RinexEphemerisStore, e = " << e << endl;
			} 

			Store2.dump(DumpData,1);
			DumpData.close();
			return testFramework.countFails();

		}

		/*
		**** Test to assure the quality of GPSEphemerisStore class member findEph()

		**** This test makes sure that exceptions are thrown if there is no ephemeris data
		**** for the given PRN and also that an exception is thrown if there is no data for
		**** the PRN at the given time. Furthermore, this test finds an Ephemeris for a given
		**** CivilTime Time and PRN.

		**** To see the ephemeris information for the selected Time and PRN please see
		**** findEph.txt
		*/

/*		int BCESfindEphTest (void)
		{
			TestUtil test2( "RinexEphemerisStore", "Error Handling", __FILE__, __func__ );
			ofstream fPRN1;
			ofstream fPRN15;
			ofstream fPRN32;
			fPRN1.open ("Rinex_Logs/findEph1.txt");
			fPRN15.open ("Rinex_Logs/findEph15.txt");
			fPRN32.open ("Rinex_Logs/findEph32.txt");

			RinexEphemerisStore Store;
			Store.loadFile("TestRinex06.031");

			std::list<RinexNavData> R3NList;
			GPSEphemerisStore GStore;
			std::list<RinexNavData>::const_iterator it;
			const SatID sidNeg((short)-1, SatID::systemGPS);
			Store.addToList(R3NList,sidNeg);
			for (it = R3NList.begin(); it != R3NList.end(); ++it)
			  GStore.addEphemeris(EngEphemeris(*it));

			// debug dump of GStore

			//ofstream GDumpData;
			//GDumpData.open("GDumpData.txt");
			//GStore.dump(GDumpData,1);

			const short PRN0 = 0; // Zero PRN (Border test case)
			const short PRN1 = 1;
			const short PRN15 = 15;
			const short PRN32 = 32;
			const short PRN33 = 33;  //Top PRN (33) (Border test case);

			const SatID sid0(PRN0,SatID::systemGPS);
			const SatID sid1(PRN1,SatID::systemGPS);
			const SatID sid15(PRN15,SatID::systemGPS);
			const SatID sid32(PRN32,SatID::systemGPS);
			const SatID sid33(PRN33,SatID::systemGPS);

			CivilTime Time(2006,1,31,11,45,0,2);
			CivilTime bTime(2006,1,31,2,0,0,2); //Border Time (Time of Border test cases)
			const CommonTime ComTime = (CommonTime)Time;
			const CommonTime CombTime = (CommonTime)bTime;

			try
			{
				CivilTime crazy(1950,1,31,2,0,0,2);
				const CommonTime Comcrazy = (CommonTime)crazy;

				try {GStore.findEphemeris(sid1,ComTime); test2.print(); test2.next();}
				catch (...) {test2.fail(); test2.print(); test2.next();}

				fPRN1 << GStore.findEphemeris(sid1,ComTime);
				fPRN15 << GStore.findEphemeris(sid15,ComTime);
				fPRN32 << GStore.findEphemeris(sid32,ComTime);

				try {GStore.findEphemeris(sid0,CombTime); test2.fail(); test2.print(); test2.next();}
				catch (InvalidRequest) {test2.print(); test2.next();}
				catch (...) {test2.fail(); test2.print(); test2.next();}

				try {GStore.findEphemeris(sid33,CombTime; test2.fail(); test2.print(); test2.next();}
				catch (InvalidRequest) {test2.print(); test2.next();}
				catch (...) {test2.fail(); test2.print(); test2.next();}

				try {GStore.findEphemeris(sid32,Comcrazy); test2.fail(); test2.print(); test2.next();}
				catch (InvalidRequest) {test2.print(); test2.next();}
				catch (...) {test2.fail(); test2.print(); test2.next();}
			}
			catch (Exception& e)
			{
				//cout << e;
			}

			test2.assert(fileEqualTest((char*)"Rinex_Logs/findEph1.txt",(char*)"Rinex_Checks/findEph1.chk"));
			test2.print();
			test2.next();
			test2.assert(fileEqualTest((char*)"Rinex_Logs/findEph15.txt",(char*)"Rinex_Checks/findEph15.chk"));
			test2.print();
			test2.next();
			test2.assert(fileEqualTest((char*)"Rinex_Logs/findEph32.txt",(char*)"Rinex_Checks/findEph32.chk"));
			test2.print();
			test2.next();
			return test1.countFails();
		}


/*
**** Test to assure the quality of GPSEphemerisStore class member getXvt()

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test finds an Xvt for a given
**** CivilTime Time and PRN.

**** To see the Xvt information for the selected Time and PRN please see
**** getXvt.txt

void RinexEphemerisStore_T :: BCESgetXvtTest (void)
{
	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN32;

	fPRN1.open ("Rinex_Logs/getXvt1.txt");
	fPRN15.open ("Rinex_Logs/getXvt15.txt");
	fPRN32.open ("Rinex_Logs/getXvt32.txt");

	RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");

	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
   SatID sid0(PRN0,SatID::systemGPS);
   SatID sid1(PRN1,SatID::systemGPS);
   SatID sid15(PRN15,SatID::systemGPS);
   SatID sid32(PRN32,SatID::systemGPS);
   SatID sid33(PRN33,SatID::systemGPS);

	CivilTime Time(2006,1,31,11,45,0,2);
	CivilTime bTime(2006,1,31,2,0,0,2); //Border Time (Time of Border test cases)
        const CommonTime ComTime = (CommonTime)Time;
        const CommonTime CombTime = (CommonTime)bTime;

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.getXvt(sid1,ComTime));

		fPRN1 << Store.getXvt(sid1,ComTime) << endl;
		fPRN15 << Store.getXvt(sid15,ComTime) << endl;
		fPRN32 << Store.getXvt(sid32,ComTime) << endl;

		CPPUNIT_ASSERT_THROW(Store.getXvt(sid0,CombTime),InvalidRequest);
		CPPUNIT_ASSERT_THROW(Store.getXvt(sid33,CombTime),InvalidRequest);
	}
	catch (Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/getXvt1.txt",(char*)"Rinex_Checks/getPrnXvt1.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/getXvt15.txt",(char*)"Rinex_Checks/getPrnXvt15.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/getXvt32.txt",(char*)"Rinex_Checks/getPrnXvt32.chk"));
}


/*
**** Test to assure the quality of GPSEphemerisStore class member getXvt()
**** This test differs from the previous in that this getXvt() has another parameter
**** for the IODC

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test finds an Xvt for a given
**** CivilTime Time and PRN and IODC.

**** To see the Xvt information for the selected Time and PRN please see
**** getXvt2.txt

void RinexEphemerisStore_T :: BCESgetXvt2Test (void)
{

	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN32;

	fPRN1.open ("Rinex_Logs/getXvt2_1.txt");
	fPRN15.open ("Rinex_Logs/getXvt2_15.txt");
	fPRN32.open ("Rinex_Logs/getXvt2_32.txt");

	RinexEphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<RinexNavData> R3NList;
        GPSEphemerisStore GStore;
        list<RinexNavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(EngEphemeris(*it));
        
	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
   SatID sid0(PRN0,SatID::systemGPS);
   SatID sid1(PRN1,SatID::systemGPS);
   SatID sid15(PRN15,SatID::systemGPS);
   SatID sid32(PRN32,SatID::systemGPS);
   SatID sid33(PRN33,SatID::systemGPS);

	short IODC0 = 89;
	short IODC1 = 372;
	short IODC15 = 455;
	short IODC32 = 441;
	short IODC33 = 392;

	CivilTime Time(2006,1,31,11,45,0,2);
	CivilTime bTime(2006,1,31,2,0,0,2); //Border Time (Time of Border test cases)
        const CommonTime ComTime = (CommonTime)Time;
        const CommonTime CombTime = (CommonTime)bTime;

	try
	{
		fPRN1 << GStore.getXvt(sid1,ComTime,IODC1) << endl;
		fPRN15 << GStore.getXvt(sid15,ComTime,IODC15) << endl;
		fPRN32 << GStore.getXvt(sid32,ComTime,IODC32) << endl;

		CPPUNIT_ASSERT_THROW(GStore.getXvt(sid0,CombTime,IODC0),InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.getXvt(sid33,CombTime,IODC33),InvalidRequest);
	}
	catch (Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/getXvt2_1.txt",(char*)"Rinex_Checks/getPrnXvt1.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/getXvt2_15.txt",(char*)"Rinex_Checks/getPrnXvt15.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/getXvt2_32.txt",(char*)"Rinex_Checks/getPrnXvt32.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member getSatHealth()

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test assures that for a specific PRN
**** and Time, that SV is as we expect it, health (0).


void RinexEphemerisStore_T :: BCESgetSatHealthTest (void)
{
	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
   SatID sid0(PRN0,SatID::systemGPS);
   SatID sid1(PRN1,SatID::systemGPS);
   SatID sid15(PRN15,SatID::systemGPS);
   SatID sid32(PRN32,SatID::systemGPS);
   SatID sid33(PRN33,SatID::systemGPS);

	RinexEphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<RinexNavData> R3NList;
        GPSEphemerisStore GStore;
        list<RinexNavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(EngEphemeris(*it));

	CivilTime Time(2006,1,31,11,45,0,2);
	CivilTime bTime(2006,1,31,2,0,0,2); //Border Time (Time of Border test cases)
        const CommonTime ComTime = (CommonTime)Time;
        const CommonTime CombTime = (CommonTime)bTime;


	try
	{


		CPPUNIT_ASSERT_NO_THROW(GStore.getSatHealth(sid1,ComTime));

		CPPUNIT_ASSERT_EQUAL((short) 0,GStore.getSatHealth(sid1,Time));
		CPPUNIT_ASSERT_EQUAL((short) 0,GStore.getSatHealth(sid15,Time));
		CPPUNIT_ASSERT_EQUAL((short) 0,GStore.getSatHealth(sid32,Time));

		CPPUNIT_ASSERT_THROW(GStore.getSatHealth(sid0,bTime),InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.getSatHealth(sid33,bTime),InvalidRequest);
	}
	catch (Exception& e)
	{
		//cout << e;
	}
}

/*
**** Test to assure the quality of GPSEphemerisStore class member dump()

**** This test makes sure that dump() behaves as expected.  With paramters from
**** 0-2 with each giving more and more respective information, this information is
**** then put into txt files.

**** To see the dump with paramter 0, please view DumpData0.txt
**** To see the dump with paramter 1, pleave view DumpData1.txt
**** To see the dump with paramter 2, please uncomment the test and view the command
**** line output (cout).


void RinexEphemerisStore_T :: BCESdumpTest (void)
{
	ofstream DumpData0;
	ofstream DumpData1;
	ofstream DumpData2;
	DumpData0.open ("Rinex_Logs/DumpData0.txt");
	DumpData1.open ("Rinex_Logs/DumpData1.txt");
	DumpData2.open ("Rinex_Logs/DumpData2.txt");

	RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.dump(DumpData0,0));
		CPPUNIT_ASSERT_NO_THROW(Store.dump(DumpData1,1));
		//Code outputs to cout but does pass, just dont want to run that every time
		//CPPUNIT_ASSERT_NO_THROW(Store.dump(2,DumpData2));
		//Store.dump(2,DumpData2);

	}
	catch (Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/DumpData0.txt",(char*)"Rinex_Checks/DumpData0.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/DumpData1.txt",(char*)"Rinex_Checks/DumpData1.chk"));
	//CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/DumpData2.txt",(char*)"Rinex_Checks/DumpData2.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member addEphemeris()

**** This test assures that no exceptions are thrown when a an ephemeris, taken from Store
**** is added to a blank BCES Object.  Then the test makes sure that only that Ephemeris
**** is in the object by checking the start and end times of the Object

**** Question:  Why does this eph data begin two hours earlier than it does on the output?


void RinexEphemerisStore_T :: BCESaddEphemerisTest (void)
{

	ofstream DumpData;
	DumpData.open ("Rinex_Logs/addEphemerisTest.txt");

	GPSEphemerisStore Blank;
     //cout << " On construction, Blank.getInitialTime: " << Blank.getInitialTime() << endl;
     //cout << " On construction, Blank.getFinalTime:   " << Blank.getFinalTime() << endl;

	RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");

        std::list<RinexNavData> R3NList;
        GPSEphemerisStore GStore;
        std::list<RinexNavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(EngEphemeris(*it));

	short PRN = 1;
        SatID sid(PRN,SatID::systemGPS);

	CivilTime Time(2006,1,31,11,45,0,2);
	CivilTime TimeB(2006,1,31,9,59,44,2);
        CivilTime TimeE(2006,1,31,13,59,44,2);

        const CommonTime ComTime = (CommonTime)Time;
        const CommonTime ComTimeB = (CommonTime)TimeB;
        const CommonTime ComTimeE = (CommonTime)TimeE;

	const EngEphemeris eph = GStore.findEphemeris(sid,ComTime);

     //cout << " ComTime: " << ComTime << " ComTimeB: " << ComTimeB << " ComTimeE: " << ComTimeE << endl;
     //cout << " eph follows: " << endl;
     //cout << eph << endl;

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Blank.addEphemeris(eph));
     //cout << " After assert_no_throw: " << endl;
     //cout << " Blank.getInitialTime: " << Blank.getInitialTime() << endl;
     //cout << " Blank.getFinalTime:   " << Blank.getFinalTime() << endl;

                Blank.clear();
     //cout << " After clear: " << endl;
     //cout << " Blank.getInitialTime: " << Blank.getInitialTime() << endl;
     //cout << " Blank.getFinalTime:   " << Blank.getFinalTime() << endl;

		Blank.addEphemeris(eph);
     //cout << " After addEphemeris(eph): " << endl;
     //cout << " Blank.getInitialTime: " << Blank.getInitialTime() << endl;
     //cout << " Blank.getFinalTime:   " << Blank.getFinalTime() << endl;

		CPPUNIT_ASSERT_EQUAL(ComTimeB,Blank.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(ComTimeE,Blank.getFinalTime());

		Blank.dump(DumpData,1);
	}
	catch (Exception& e)
	{
		cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/addEphemerisTest.txt",(char*)"Rinex_Checks/addEphemerisTest.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member edit()

**** This test assures that no exceptions are thrown when we edit a RES object
**** then after we edit the RES Object, we test to make sure that our edit time
**** parameters are now the time endpoints of the object.

**** For further inspection of the edit, please view editTest.txt


void RinexEphemerisStore_T :: BCESeditTest (void)
{
	ofstream DumpData;
	DumpData.open ("Rinex_Logs/editTest.txt");

	RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");

	CivilTime TimeMax(2006,1,31,15,45,0,2);
	CivilTime TimeMin(2006,1,31,3,0,0,2);

        //cout << "TimeMax: " << TimeMax << "\n" << "TimeMin: " << TimeMin << "\n";

        const CommonTime ComTMax = (CommonTime)TimeMax;
        const CommonTime ComTMin = (CommonTime)TimeMin;

	//cout << "ComTMax: " << ComTMax << "\n" << "ComTMin: " << ComTMin << "\n";

	try
	{
                CPPUNIT_ASSERT_NO_THROW(Store.edit(ComTMin, ComTMax));
		Store.edit(ComTMin, ComTMax);
		CPPUNIT_ASSERT_EQUAL(ComTMin,Store.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(ComTMax,Store.getFinalTime());
		Store.dump(DumpData,1);

	}
	catch (Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/editTest.txt",(char*)"Rinex_Checks/editTest.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member wiper()

**** This test assures that no exceptions are thrown when we wiper a RES object
**** then after we wiper the RES Object, we test to make sure that our wiper time
**** parameter in now the time endpoint of the object.

**** For further inspection of the edit, please view wiperTest.txt

**** Please note that this test also indirectly tests size


void RinexEphemerisStore_T :: BCESwiperTest (void)
{
	ofstream DumpData1;
	ofstream DumpData2;
	DumpData1.open ("Rinex_Logs/wiperTest.txt");
	DumpData2.open ("Rinex_Logs/wiperTest2.txt");


	RinexEphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<RinexNavData> R3NList;
        GPSEphemerisStore GStore;
        list<RinexNavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(EngEphemeris(*it));

	CivilTime Time(2006,1,31,11,45,0,2);
        const CommonTime ComTime = (CommonTime)Time;

	try
	{
		//Make sure it doesn't fail but dont wipe anything
		CPPUNIT_ASSERT_NO_THROW(GStore.wiper(CommonTime::BEGINNING_OF_TIME));
		//Wipe everything outside interval and make sure that we did wipe all the data
		GStore.wiper(ComTime);
		GStore.dump(DumpData1,1);

		CPPUNIT_ASSERT_EQUAL(ComTime,GStore.getInitialTime());

		//Wipe everything, return size (should be zero)
		GStore.wiper(CommonTime::END_OF_TIME);
		unsigned int Num = GStore.ubeSize();

		CPPUNIT_ASSERT_EQUAL((unsigned) 0,Num);

		GStore.dump(DumpData2,1);

		CPPUNIT_ASSERT_EQUAL(CommonTime::END_OF_TIME,GStore.getInitialTime());


	}
	catch (Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/wiperTest.txt",(char*)"Rinex_Checks/wiperTest.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/wiperTest.txt",(char*)"Rinex_Checks/wiperTest.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member clear()

**** This test assures that no exceptions are thrown when we clear a RES object
**** then after we clear the RES Object, we test to make sure that END_OF_TIME is our
**** initial time and BEGINNING_OF_TIME is our final time

**** For further inspection of the edit, please view clearTest.txt


void RinexEphemerisStore_T :: BCESclearTest (void)
{
	ofstream DumpData;
	DumpData.open ("Rinex_Logs/clearTest.txt");

	RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.clear());

		CPPUNIT_ASSERT_EQUAL(CommonTime::END_OF_TIME,Store.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(CommonTime::BEGINNING_OF_TIME,Store.getFinalTime());
		Store.dump(DumpData,1);

	}
	catch (Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/clearTest.txt",(char*)"Rinex_Checks/clearTest.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member findUserEphemeris()

**** findUserEphemeris find the ephemeris which a) is within the fit tinterval for the
**** given time of interest and 2) is the last ephemeris transmitted before the time of
**** interest (i.e. min(toi-HOW time))

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Store is then cleared and the ephemeris data is readded
**** for output purposes.

**** For further inspection of the find, please view findUserTest.txt


void RinexEphemerisStore_T :: BCESfindUserTest (void)
{
	ofstream DumpData;
	DumpData.open ("Rinex_Logs/findUserTest.txt");

	RinexEphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<RinexNavData> R3NList;
        GPSEphemerisStore GStore;
        list<RinexNavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(EngEphemeris(*it));

	CivilTime Time(2006,1,31,13,0,1,2);
        const CommonTime ComTime = (CommonTime)Time;

	short PRN0 = 0;
	short PRN1 = 1;
	short PRN15 = 15;
	short PRN32 = 32;
	short PRN33 = 33;
   SatID sid0(PRN0,SatID::systemGPS);
   SatID sid1(PRN1,SatID::systemGPS);
   SatID sid15(PRN15,SatID::systemGPS);
   SatID sid32(PRN32,SatID::systemGPS);
   SatID sid33(PRN33,SatID::systemGPS);

	try
	{
		CPPUNIT_ASSERT_THROW(GStore.findUserEphemeris(sid0,ComTime),InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findUserEphemeris(sid33,ComTime),InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findUserEphemeris(sid1,CommonTime::END_OF_TIME),
					InvalidRequest);

		CPPUNIT_ASSERT_NO_THROW(GStore.findUserEphemeris(sid1, ComTime));

		const EngEphemeris Eph1 = GStore.findUserEphemeris(sid1, ComTime);
		const EngEphemeris Eph15 = GStore.findUserEphemeris(sid15, ComTime);
		const EngEphemeris Eph32 = GStore.findUserEphemeris(sid32, ComTime);

		GStore.clear();

		GStore.addEphemeris(Eph1);
		GStore.addEphemeris(Eph15);
		GStore.addEphemeris(Eph32);

		GStore.dump(DumpData,1);

	}
	catch (Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/findUserTest.txt",(char*)"Rinex_Checks/findUserTest.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member findNearEphemeris()
**** findNearEphemeris finds the ephemeris with the HOW time closest to the time t, i.e
**** with the smalles fabs(t-HOW), but still within the fit interval

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Store is then cleared and the epeheris data is readded
**** for output purposes.

**** For further inspection of the find, please view findNearTest.txt


void RinexEphemerisStore_T :: BCESfindNearTest (void)
{
	ofstream DumpData;
	DumpData.open ("Rinex_Logs/findNearTest.txt");

	RinexEphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<RinexNavData> R3NList;
        GPSEphemerisStore GStore;
        list<RinexNavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(EngEphemeris(*it));

	CivilTime Time(2006,1,31,13,0,1,2);
        const CommonTime ComTime = (CommonTime)Time;

	short PRN0 = 0;
	short PRN1 = 1;
	short PRN15 = 15;
	short PRN32 = 32;
	short PRN33 = 33;
   SatID sid0(PRN0,SatID::systemGPS);
   SatID sid1(PRN1,SatID::systemGPS);
   SatID sid15(PRN15,SatID::systemGPS);
   SatID sid32(PRN32,SatID::systemGPS);
   SatID sid33(PRN33,SatID::systemGPS);

	try
	{
		CPPUNIT_ASSERT_THROW(GStore.findNearEphemeris(sid0,ComTime),InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findNearEphemeris(sid33,ComTime),InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findNearEphemeris(sid1,CommonTime::END_OF_TIME),
					InvalidRequest);

		CPPUNIT_ASSERT_NO_THROW(GStore.findNearEphemeris(sid1, ComTime));

		const EngEphemeris Eph1 = GStore.findNearEphemeris(sid1, ComTime);
		const EngEphemeris Eph15 = GStore.findNearEphemeris(sid15, ComTime);
		const EngEphemeris Eph32 = GStore.findNearEphemeris(sid32, ComTime);

		GStore.clear();

		GStore.addEphemeris(Eph1);
		GStore.addEphemeris(Eph15);
		GStore.addEphemeris(Eph32);

		GStore.dump(DumpData,1);

	}
	catch (Exception& e)
	{
                e.addLocation(FILE_LOCATION);
		cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/findNearTest.txt",(char*)"Rinex_Checks/findNearTest.chk"));
}



/*
**** Test to assure the quality of GPSEphemerisStore class member addToList()

**** This test creats a list of EngEphemeris and then adds all of the ephemeris
**** members to that list.  After that of the List and Store are checked to be
**** equal.

**** For further inspection of the add, please view addToListTest.txt


void RinexEphemerisStore_T :: BCESaddToListTest (void)
{
	ofstream DumpData;
	DumpData.open ("Rinex_Logs/addToListTest.txt");


	RinexEphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<RinexNavData> R3NList;
        GPSEphemerisStore GStore;
        list<RinexNavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(EngEphemeris(*it));


	try
	{
		std::list<EngEphemeris> EphList; // Empty Ephemeris List

		//Assert that the number of added members equals the size of Store (all members added)
		CPPUNIT_ASSERT_EQUAL(GStore.ubeSize(),(unsigned) GStore.addToList(EphList));
		CPPUNIT_ASSERT_EQUAL((unsigned) EphList.size(),GStore.ubeSize());

		typedef list<EngEphemeris>::const_iterator LI;
		for (LI i = EphList.begin();i!=EphList.end();i++)
		{
			const EngEphemeris& e = *i;
			DumpData << e;
		}

	}
	catch (Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Rinex_Logs/addToListTest.txt",(char*)"Rinex_Checks/addToListTest.chk"));
}

*/
	bool fileEqualTest (char* handle1, char* handle2)
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
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	RinexEphemerisStore_T testClass;

	check = testClass.RESTest();
	errorCounter += check;
/*
	check = testClass.BCESfindEphTest();
	errorCounter += check;

	check = testClass.operatorTest();
	errorCounter += check;

	check = testClass.setFromInfoTest();
	errorCounter += check;

	check = testClass.resetTest();
	errorCounter += check;

	check = testClass.timeSystemTest();
	errorCounter += check;

	check = testClass.toFromCommonTimeTest();
	errorCounter += check;

	check = testClass.printfTest();
	errorCounter += check;
*/	
	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
