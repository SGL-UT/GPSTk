#pragma ident "$Id$"
// file: xRinexEphemerisStore.cpp

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

#include <list>
#include <string>

#include "xRinexEphemerisStore.hpp"
#include "Exception.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "Rinex3NavData.hpp"
#include "EngEphemeris.hpp"
#include "GPSEphemerisStore.hpp"
#include "SatID.hpp"
#include "Rinex3NavStream.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xRinexEphemerisStore);


void xRinexEphemerisStore :: setUp (void)
{
}

/*
**** General test for the Rinex3EphemerisStore (RES) class
**** Test to assure the that RES throws its exceptions in the right place and
**** that it loads the RINEX Nav file correctly

**** To further check this data, please view DumpData.txt for the dumped information
*/

void xRinexEphemerisStore :: RESTest (void)
{
	ofstream DumpData;
	DumpData.open ("DumpData.txt");

	CPPUNIT_ASSERT_NO_THROW(gpstk::Rinex3EphemerisStore Store);
	gpstk::Rinex3EphemerisStore Store;

	try
	{
	  CPPUNIT_ASSERT_EQUAL(Store.loadFile("NotaFILE"), int(-1));
	}
	catch (gpstk::Exception& e)
	{
	  cout << "Unexpected exception thrown" << endl;
	  cout << e << endl;
	}

	CPPUNIT_ASSERT_NO_THROW(Store.loadFile("TestRinex06.031"));

        // Clear the store after invoking for assert_no_throw before loading file again to avoid 
        // duplicate file name error

        Store.clear();

	try {Store.loadFile("TestRinex06.031");}
        catch (gpstk::Exception& e)
        {
          cout << " Exception received from Rinex3EphemerisStore, e = " << e << endl;
        } 

	Store.dump(DumpData,1);
	DumpData.close();

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

void xRinexEphemerisStore :: BCESfindEphTest (void)
{
	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN32;
	fPRN1.open ("Logs/findEph1.txt");
	fPRN15.open ("Logs/findEph15.txt");
	fPRN32.open ("Logs/findEph32.txt");

	gpstk::Rinex3EphemerisStore Store;
	Store.loadFile("TestRinex06.031");

        std::list<gpstk::Rinex3NavData> R3NList;
        gpstk::GPSEphemerisStore GStore;
        std::list<gpstk::Rinex3NavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(gpstk::EngEphemeris(*it));

        // debug dump of GStore

        //ofstream GDumpData;
        //GDumpData.open("GDumpData.txt");
        //GStore.dump(GDumpData,1);

	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);

        const gpstk::SatID sid0(PRN0,gpstk::SatID::systemGPS);
        const gpstk::SatID sid1(PRN1,gpstk::SatID::systemGPS);
        const gpstk::SatID sid15(PRN15,gpstk::SatID::systemGPS);
        const gpstk::SatID sid32(PRN32,gpstk::SatID::systemGPS);
        const gpstk::SatID sid33(PRN33,gpstk::SatID::systemGPS);

	gpstk::CivilTime Time(2006,1,31,11,45,0,2);
	gpstk::CivilTime bTime(2006,1,31,2,0,0,2); //Border Time (Time of Border test cases)
        const gpstk::CommonTime ComTime = (gpstk::CommonTime)Time;
        const gpstk::CommonTime CombTime = (gpstk::CommonTime)bTime;

	try
	{
		gpstk::CivilTime crazy(1950,1,31,2,0,0,2);
                const gpstk::CommonTime Comcrazy = (gpstk::CommonTime)crazy;

		CPPUNIT_ASSERT_NO_THROW(GStore.findEphemeris(sid1,ComTime));

		fPRN1 << GStore.findEphemeris(sid1,ComTime);
		fPRN15 << GStore.findEphemeris(sid15,ComTime);
		fPRN32 << GStore.findEphemeris(sid32,ComTime);

		CPPUNIT_ASSERT_THROW(GStore.findEphemeris(sid0,CombTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findEphemeris(sid33,CombTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findEphemeris(sid32,Comcrazy),gpstk::InvalidRequest);
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/findEph1.txt",(char*)"Checks/findEph1.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/findEph15.txt",(char*)"Checks/findEph15.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/findEph32.txt",(char*)"Checks/findEph32.chk"));

}


/*
**** Test to assure the quality of GPSEphemerisStore class member getXvt()

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test finds an Xvt for a given
**** CivilTime Time and PRN.

**** To see the Xvt information for the selected Time and PRN please see
**** getXvt.txt
*/
void xRinexEphemerisStore :: BCESgetXvtTest (void)
{
	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN32;

	fPRN1.open ("Logs/getXvt1.txt");
	fPRN15.open ("Logs/getXvt15.txt");
	fPRN32.open ("Logs/getXvt32.txt");

	gpstk::Rinex3EphemerisStore Store;
	Store.loadFile("TestRinex06.031");

	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
   gpstk::SatID sid0(PRN0,gpstk::SatID::systemGPS);
   gpstk::SatID sid1(PRN1,gpstk::SatID::systemGPS);
   gpstk::SatID sid15(PRN15,gpstk::SatID::systemGPS);
   gpstk::SatID sid32(PRN32,gpstk::SatID::systemGPS);
   gpstk::SatID sid33(PRN33,gpstk::SatID::systemGPS);

	gpstk::CivilTime Time(2006,1,31,11,45,0,2);
	gpstk::CivilTime bTime(2006,1,31,2,0,0,2); //Border Time (Time of Border test cases)
        const gpstk::CommonTime ComTime = (gpstk::CommonTime)Time;
        const gpstk::CommonTime CombTime = (gpstk::CommonTime)bTime;

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.getXvt(sid1,ComTime));

		fPRN1 << Store.getXvt(sid1,ComTime) << endl;
		fPRN15 << Store.getXvt(sid15,ComTime) << endl;
		fPRN32 << Store.getXvt(sid32,ComTime) << endl;

		CPPUNIT_ASSERT_THROW(Store.getXvt(sid0,CombTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(Store.getXvt(sid33,CombTime),gpstk::InvalidRequest);
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/getXvt1.txt",(char*)"Checks/getPrnXvt1.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/getXvt15.txt",(char*)"Checks/getPrnXvt15.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/getXvt32.txt",(char*)"Checks/getPrnXvt32.chk"));
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
*/
void xRinexEphemerisStore :: BCESgetXvt2Test (void)
{

	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN32;

	fPRN1.open ("Logs/getXvt2_1.txt");
	fPRN15.open ("Logs/getXvt2_15.txt");
	fPRN32.open ("Logs/getXvt2_32.txt");

	gpstk::Rinex3EphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<gpstk::Rinex3NavData> R3NList;
        gpstk::GPSEphemerisStore GStore;
        list<gpstk::Rinex3NavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(gpstk::EngEphemeris(*it));
        
	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
   gpstk::SatID sid0(PRN0,gpstk::SatID::systemGPS);
   gpstk::SatID sid1(PRN1,gpstk::SatID::systemGPS);
   gpstk::SatID sid15(PRN15,gpstk::SatID::systemGPS);
   gpstk::SatID sid32(PRN32,gpstk::SatID::systemGPS);
   gpstk::SatID sid33(PRN33,gpstk::SatID::systemGPS);

	short IODC0 = 89;
	short IODC1 = 372;
	short IODC15 = 455;
	short IODC32 = 441;
	short IODC33 = 392;

	gpstk::CivilTime Time(2006,1,31,11,45,0,2);
	gpstk::CivilTime bTime(2006,1,31,2,0,0,2); //Border Time (Time of Border test cases)
        const gpstk::CommonTime ComTime = (gpstk::CommonTime)Time;
        const gpstk::CommonTime CombTime = (gpstk::CommonTime)bTime;

	try
	{
		fPRN1 << GStore.getXvt(sid1,ComTime,IODC1) << endl;
		fPRN15 << GStore.getXvt(sid15,ComTime,IODC15) << endl;
		fPRN32 << GStore.getXvt(sid32,ComTime,IODC32) << endl;

		CPPUNIT_ASSERT_THROW(GStore.getXvt(sid0,CombTime,IODC0),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.getXvt(sid33,CombTime,IODC33),gpstk::InvalidRequest);
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/getXvt2_1.txt",(char*)"Checks/getPrnXvt1.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/getXvt2_15.txt",(char*)"Checks/getPrnXvt15.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/getXvt2_32.txt",(char*)"Checks/getPrnXvt32.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member getSatHealth()

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test assures that for a specific PRN
**** and Time, that SV is as we expect it, health (0).
*/

void xRinexEphemerisStore :: BCESgetSatHealthTest (void)
{
	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
   gpstk::SatID sid0(PRN0,gpstk::SatID::systemGPS);
   gpstk::SatID sid1(PRN1,gpstk::SatID::systemGPS);
   gpstk::SatID sid15(PRN15,gpstk::SatID::systemGPS);
   gpstk::SatID sid32(PRN32,gpstk::SatID::systemGPS);
   gpstk::SatID sid33(PRN33,gpstk::SatID::systemGPS);

	gpstk::Rinex3EphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<gpstk::Rinex3NavData> R3NList;
        gpstk::GPSEphemerisStore GStore;
        list<gpstk::Rinex3NavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(gpstk::EngEphemeris(*it));

	gpstk::CivilTime Time(2006,1,31,11,45,0,2);
	gpstk::CivilTime bTime(2006,1,31,2,0,0,2); //Border Time (Time of Border test cases)
        const gpstk::CommonTime ComTime = (gpstk::CommonTime)Time;
        const gpstk::CommonTime CombTime = (gpstk::CommonTime)bTime;


	try
	{


		CPPUNIT_ASSERT_NO_THROW(GStore.getSatHealth(sid1,ComTime));

		CPPUNIT_ASSERT_EQUAL((short) 0,GStore.getSatHealth(sid1,Time));
		CPPUNIT_ASSERT_EQUAL((short) 0,GStore.getSatHealth(sid15,Time));
		CPPUNIT_ASSERT_EQUAL((short) 0,GStore.getSatHealth(sid32,Time));

		CPPUNIT_ASSERT_THROW(GStore.getSatHealth(sid0,bTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.getSatHealth(sid33,bTime),gpstk::InvalidRequest);
	}
	catch (gpstk::Exception& e)
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
*/

void xRinexEphemerisStore :: BCESdumpTest (void)
{
	ofstream DumpData0;
	ofstream DumpData1;
	ofstream DumpData2;
	DumpData0.open ("Logs/DumpData0.txt");
	DumpData1.open ("Logs/DumpData1.txt");
	DumpData2.open ("Logs/DumpData2.txt");

	gpstk::Rinex3EphemerisStore Store;
	Store.loadFile("TestRinex06.031");

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.dump(DumpData0,0));
		CPPUNIT_ASSERT_NO_THROW(Store.dump(DumpData1,1));
		//Code outputs to cout but does pass, just dont want to run that every time
		//CPPUNIT_ASSERT_NO_THROW(Store.dump(2,DumpData2));
		//Store.dump(2,DumpData2);

	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/DumpData0.txt",(char*)"Checks/DumpData0.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/DumpData1.txt",(char*)"Checks/DumpData1.chk"));
	//CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/DumpData2.txt",(char*)"Checks/DumpData2.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member addEphemeris()

**** This test assures that no exceptions are thrown when a an ephemeris, taken from Store
**** is added to a blank BCES Object.  Then the test makes sure that only that Ephemeris
**** is in the object by checking the start and end times of the Object

**** Question:  Why does this eph data begin two hours earlier than it does on the output?
*/

void xRinexEphemerisStore :: BCESaddEphemerisTest (void)
{

	ofstream DumpData;
	DumpData.open ("Logs/addEphemerisTest.txt");

	gpstk::GPSEphemerisStore Blank;
     //cout << " On construction, Blank.getInitialTime: " << Blank.getInitialTime() << endl;
     //cout << " On construction, Blank.getFinalTime:   " << Blank.getFinalTime() << endl;

	gpstk::Rinex3EphemerisStore Store;
	Store.loadFile("TestRinex06.031");

        std::list<gpstk::Rinex3NavData> R3NList;
        gpstk::GPSEphemerisStore GStore;
        std::list<gpstk::Rinex3NavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(gpstk::EngEphemeris(*it));

	short PRN = 1;
        gpstk::SatID sid(PRN,gpstk::SatID::systemGPS);

	gpstk::CivilTime Time(2006,1,31,11,45,0,2);
	gpstk::CivilTime TimeB(2006,1,31,9,59,44,2);
        gpstk::CivilTime TimeE(2006,1,31,13,59,44,2);

        const gpstk::CommonTime ComTime = (gpstk::CommonTime)Time;
        const gpstk::CommonTime ComTimeB = (gpstk::CommonTime)TimeB;
        const gpstk::CommonTime ComTimeE = (gpstk::CommonTime)TimeE;

	const gpstk::EngEphemeris eph = GStore.findEphemeris(sid,ComTime);

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
	catch (gpstk::Exception& e)
	{
		cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/addEphemerisTest.txt",(char*)"Checks/addEphemerisTest.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member edit()

**** This test assures that no exceptions are thrown when we edit a RES object
**** then after we edit the RES Object, we test to make sure that our edit time
**** parameters are now the time endpoints of the object.

**** For further inspection of the edit, please view editTest.txt
*/

void xRinexEphemerisStore :: BCESeditTest (void)
{
	ofstream DumpData;
	DumpData.open ("Logs/editTest.txt");

	gpstk::Rinex3EphemerisStore Store;
	Store.loadFile("TestRinex06.031");

	gpstk::CivilTime TimeMax(2006,1,31,15,45,0,2);
	gpstk::CivilTime TimeMin(2006,1,31,3,0,0,2);

        //cout << "TimeMax: " << TimeMax << "\n" << "TimeMin: " << TimeMin << "\n";

        const gpstk::CommonTime ComTMax = (gpstk::CommonTime)TimeMax;
        const gpstk::CommonTime ComTMin = (gpstk::CommonTime)TimeMin;

	//cout << "ComTMax: " << ComTMax << "\n" << "ComTMin: " << ComTMin << "\n";

	try
	{
                CPPUNIT_ASSERT_NO_THROW(Store.edit(ComTMin, ComTMax));
		Store.edit(ComTMin, ComTMax);
		CPPUNIT_ASSERT_EQUAL(ComTMin,Store.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(ComTMax,Store.getFinalTime());
		Store.dump(DumpData,1);

	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/editTest.txt",(char*)"Checks/editTest.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member wiper()

**** This test assures that no exceptions are thrown when we wiper a RES object
**** then after we wiper the RES Object, we test to make sure that our wiper time
**** parameter in now the time endpoint of the object.

**** For further inspection of the edit, please view wiperTest.txt

**** Please note that this test also indirectly tests size
*/

void xRinexEphemerisStore :: BCESwiperTest (void)
{
	ofstream DumpData1;
	ofstream DumpData2;
	DumpData1.open ("Logs/wiperTest.txt");
	DumpData2.open ("Logs/wiperTest2.txt");


	gpstk::Rinex3EphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<gpstk::Rinex3NavData> R3NList;
        gpstk::GPSEphemerisStore GStore;
        list<gpstk::Rinex3NavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(gpstk::EngEphemeris(*it));

	gpstk::CivilTime Time(2006,1,31,11,45,0,2);
        const gpstk::CommonTime ComTime = (gpstk::CommonTime)Time;

	try
	{
		//Make sure it doesn't fail but dont wipe anything
		CPPUNIT_ASSERT_NO_THROW(GStore.wiper(gpstk::CommonTime::BEGINNING_OF_TIME));
		//Wipe everything outside interval and make sure that we did wipe all the data
		GStore.wiper(ComTime);
		GStore.dump(DumpData1,1);

		CPPUNIT_ASSERT_EQUAL(ComTime,GStore.getInitialTime());

		//Wipe everything, return size (should be zero)
		GStore.wiper(gpstk::CommonTime::END_OF_TIME);
		unsigned int Num = GStore.ubeSize();

		CPPUNIT_ASSERT_EQUAL((unsigned) 0,Num);

		GStore.dump(DumpData2,1);

		CPPUNIT_ASSERT_EQUAL(gpstk::CommonTime::END_OF_TIME,GStore.getInitialTime());


	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/wiperTest.txt",(char*)"Checks/wiperTest.chk"));
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/wiperTest.txt",(char*)"Checks/wiperTest.chk"));
}

/*
**** Test to assure the quality of GPSEphemerisStore class member clear()

**** This test assures that no exceptions are thrown when we clear a RES object
**** then after we clear the RES Object, we test to make sure that END_OF_TIME is our
**** initial time and BEGINNING_OF_TIME is our final time

**** For further inspection of the edit, please view clearTest.txt
*/

void xRinexEphemerisStore :: BCESclearTest (void)
{
	ofstream DumpData;
	DumpData.open ("Logs/clearTest.txt");

	gpstk::Rinex3EphemerisStore Store;
	Store.loadFile("TestRinex06.031");

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.clear());

		CPPUNIT_ASSERT_EQUAL(gpstk::CommonTime::END_OF_TIME,Store.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(gpstk::CommonTime::BEGINNING_OF_TIME,Store.getFinalTime());
		Store.dump(DumpData,1);

	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/clearTest.txt",(char*)"Checks/clearTest.chk"));
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
*/

void xRinexEphemerisStore :: BCESfindUserTest (void)
{
	ofstream DumpData;
	DumpData.open ("Logs/findUserTest.txt");

	gpstk::Rinex3EphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<gpstk::Rinex3NavData> R3NList;
        gpstk::GPSEphemerisStore GStore;
        list<gpstk::Rinex3NavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(gpstk::EngEphemeris(*it));

	gpstk::CivilTime Time(2006,1,31,13,0,1,2);
        const gpstk::CommonTime ComTime = (gpstk::CommonTime)Time;

	short PRN0 = 0;
	short PRN1 = 1;
	short PRN15 = 15;
	short PRN32 = 32;
	short PRN33 = 33;
   gpstk::SatID sid0(PRN0,gpstk::SatID::systemGPS);
   gpstk::SatID sid1(PRN1,gpstk::SatID::systemGPS);
   gpstk::SatID sid15(PRN15,gpstk::SatID::systemGPS);
   gpstk::SatID sid32(PRN32,gpstk::SatID::systemGPS);
   gpstk::SatID sid33(PRN33,gpstk::SatID::systemGPS);

	try
	{
		CPPUNIT_ASSERT_THROW(GStore.findUserEphemeris(sid0,ComTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findUserEphemeris(sid33,ComTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findUserEphemeris(sid1,gpstk::CommonTime::END_OF_TIME),
					gpstk::InvalidRequest);

		CPPUNIT_ASSERT_NO_THROW(GStore.findUserEphemeris(sid1, ComTime));

		const gpstk::EngEphemeris Eph1 = GStore.findUserEphemeris(sid1, ComTime);
		const gpstk::EngEphemeris Eph15 = GStore.findUserEphemeris(sid15, ComTime);
		const gpstk::EngEphemeris Eph32 = GStore.findUserEphemeris(sid32, ComTime);

		GStore.clear();

		GStore.addEphemeris(Eph1);
		GStore.addEphemeris(Eph15);
		GStore.addEphemeris(Eph32);

		GStore.dump(DumpData,1);

	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/findUserTest.txt",(char*)"Checks/findUserTest.chk"));
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
*/

void xRinexEphemerisStore :: BCESfindNearTest (void)
{
	ofstream DumpData;
	DumpData.open ("Logs/findNearTest.txt");

	gpstk::Rinex3EphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<gpstk::Rinex3NavData> R3NList;
        gpstk::GPSEphemerisStore GStore;
        list<gpstk::Rinex3NavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(gpstk::EngEphemeris(*it));

	gpstk::CivilTime Time(2006,1,31,13,0,1,2);
        const gpstk::CommonTime ComTime = (gpstk::CommonTime)Time;

	short PRN0 = 0;
	short PRN1 = 1;
	short PRN15 = 15;
	short PRN32 = 32;
	short PRN33 = 33;
   gpstk::SatID sid0(PRN0,gpstk::SatID::systemGPS);
   gpstk::SatID sid1(PRN1,gpstk::SatID::systemGPS);
   gpstk::SatID sid15(PRN15,gpstk::SatID::systemGPS);
   gpstk::SatID sid32(PRN32,gpstk::SatID::systemGPS);
   gpstk::SatID sid33(PRN33,gpstk::SatID::systemGPS);

	try
	{
		CPPUNIT_ASSERT_THROW(GStore.findNearEphemeris(sid0,ComTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findNearEphemeris(sid33,ComTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(GStore.findNearEphemeris(sid1,gpstk::CommonTime::END_OF_TIME),
					gpstk::InvalidRequest);

		CPPUNIT_ASSERT_NO_THROW(GStore.findNearEphemeris(sid1, ComTime));

		const gpstk::EngEphemeris Eph1 = GStore.findNearEphemeris(sid1, ComTime);
		const gpstk::EngEphemeris Eph15 = GStore.findNearEphemeris(sid15, ComTime);
		const gpstk::EngEphemeris Eph32 = GStore.findNearEphemeris(sid32, ComTime);

		GStore.clear();

		GStore.addEphemeris(Eph1);
		GStore.addEphemeris(Eph15);
		GStore.addEphemeris(Eph32);

		GStore.dump(DumpData,1);

	}
	catch (gpstk::Exception& e)
	{
                e.addLocation(FILE_LOCATION);
		cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/findNearTest.txt",(char*)"Checks/findNearTest.chk"));
}



/*
**** Test to assure the quality of GPSEphemerisStore class member addToList()

**** This test creats a list of EngEphemeris and then adds all of the ephemeris
**** members to that list.  After that of the List and Store are checked to be
**** equal.

**** For further inspection of the add, please view addToListTest.txt
*/

void xRinexEphemerisStore :: BCESaddToListTest (void)
{
	ofstream DumpData;
	DumpData.open ("Logs/addToListTest.txt");


	gpstk::Rinex3EphemerisStore Store;
        int nr;
	nr = Store.loadFile("TestRinex06.031");

        std::list<gpstk::Rinex3NavData> R3NList;
        gpstk::GPSEphemerisStore GStore;
        list<gpstk::Rinex3NavData>::const_iterator it;
        Store.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris(gpstk::EngEphemeris(*it));


	try
	{
		std::list<gpstk::EngEphemeris> EphList; // Empty Ephemeris List

		//Assert that the number of added members equals the size of Store (all members added)
		CPPUNIT_ASSERT_EQUAL(GStore.ubeSize(),(unsigned) GStore.addToList(EphList));
		CPPUNIT_ASSERT_EQUAL((unsigned) EphList.size(),GStore.ubeSize());

		typedef list<gpstk::EngEphemeris>::const_iterator LI;
		for (LI i = EphList.begin();i!=EphList.end();i++)
		{
			const gpstk::EngEphemeris& e = *i;
			DumpData << e;
		}

	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/addToListTest.txt",(char*)"Checks/addToListTest.chk"));
}


bool xRinexEphemerisStore :: fileEqualTest (char* handle1, char* handle2)
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
