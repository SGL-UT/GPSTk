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

#include "Xvt.hpp"
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

/* =========================================================================================================================
	General test for the RinexEphemerisStore (RES) class
	Test to assure the that RES throws its exceptions in the right place and
	that it loads the RINEX Nav file correctly
========================================================================================================================= */
		int RESTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "Constructor", __FILE__, __LINE__ );
			testFramework.init();

//--------------RinexEphemerisStore_RESTest_1 - Verify the consturctor builds the RES object
			try {RinexEphemerisStore Store; testFramework.passTest();}
			catch (...) {testFramework.failTest();}
			RinexEphemerisStore Store; 

//--------------RinexEphemerisStore_RESTest_2 - Verify the ability to load nonexistant files.
			testFramework.changeSourceMethod("loadFile");
			try
			{
			  Store.loadFile(inputNotaFile.c_str());
			  testFramework.passTest();
			}
			catch (Exception& e)
			{
			  cout << "Expected exception thrown " << endl;
			  cout << e << endl;
			  testFramework.failTest();
			}

//--------------RinexEphemerisStore_RESTest_3 - Verify the ability to load existant files.
			try {Store.loadFile(inputRinexNavData.c_str()); testFramework.passTest();}
			catch (...) {cout << "Checking for failure!!!!" << endl; testFramework.failTest();}

//=================================================================================================
//   It would be nice to verify that the double name exception is indeed thrown. However the InvalidParameter exception 
//   thrown will terminate the program even with a catch-all.
/*
//--------------RinexEphemerisStore_RESTest_4 - Verify that a repeated filename returns an exception
			try 
			{
				Store.loadFile(inputRinexNavData.c_str()); 
				testFramework.failTest();
			}
			catch (Exception& e) 
			{
				testFramework.passTest(); 
				cout << "Expected exception received from RinexEphemerisStore" << endl;
			}
			catch (...) 
			{
				cout << "Expected exception received from RinexEphemerisStore!!!!!!!!!" << endl;
			}
*/
//=================================================================================================

			testFramework.changeSourceMethod("clear");
//--------------RinexEphemerisStore_RESTest_5 - Verify that once a clear() has been performed the repeated filename can be opened.
			Store.gpstk::FileStore<RinexNavHeader>::clear();
			try {Store.loadFile(inputRinexNavData.c_str()); testFramework.passTest();}
			catch (Exception& e) {cout << " Exception received from RinexEphemerisStore, e = " << e << endl; testFramework.failTest();} 

			return testFramework.countFails();

		}


/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member findEph()

	This test makes sure that exceptions are thrown if there is no ephemeris data
	for the given PRN and also that an exception is thrown if there is no data for
	the PRN at the given time. Furthermore, this test finds an Ephemeris for a given
	CivilTime Time and PRN.

	To see the ephemeris information for the selected Time and PRN please see
	findEph#.txt
========================================================================================================================= */
		int findEphTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "findEphemeris", __FILE__, __LINE__ );
			testFramework.init();
			ofstream fPRN1;
			ofstream fPRN15;
			ofstream fPRN32;

			outputTestOutput1 = outputTestOutput + "findEph1.txt";
			outputTestOutput15 = outputTestOutput + "findEph15.txt";
			outputTestOutput32 = outputTestOutput + "findEph32.txt";

			fPRN1.open (outputTestOutput1.c_str());
			fPRN15.open (outputTestOutput15.c_str());
			fPRN32.open (outputTestOutput32.c_str());

			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

			std::list<GPSEphemeris> R3NList;                         //Create a list of GPSEphemerides
			GPSEphemerisStore GStore;                                //Create a GPSEphemerisStore for testing
			list<GPSEphemeris>::const_iterator it;                   //Create an interator for the GPSEphmeris list
			Store.addToList(R3NList);                                //Add the loaded Rinex Nav Data into the list
			for (it = R3NList.begin(); it != R3NList.end(); ++it)    //Loop over the list adding the ephemerides to the GPSEphemerisStore
			{
			  GStore.addEphemeris(GPSEphemeris(*it));
			}

			// debug dump of GStore
			ofstream GDumpData;
			GDumpData.open(outputDataDump.c_str());
			GStore.dump(GDumpData,1);
			GDumpData.close();

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
			const CommonTime ComTime = Time.convertToCommonTime();
			const CommonTime CombTime = bTime.convertToCommonTime();

			try
			{
				CivilTime crazy(1950,1,31,2,0,0,2);
				const CommonTime Comcrazy = (CommonTime)crazy;

//--------------RinexEphemerisStore_findEphTest_1 - For proper input, will the method return properly?
				try {GStore.findEphemeris(sid1,ComTime); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_findEphTest_2 - For a wrong SatID (too small), will an exception be thrown?
				try {GStore.findEphemeris(sid0,CombTime); testFramework.failTest();}
				catch (InvalidRequest) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_findEphTest_3 - For a wrong SatID (too large), will an exception be thrown?
				try {GStore.findEphemeris(sid33,CombTime); testFramework.failTest();}
				catch (InvalidRequest) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_findEphTest_4 - For an improper time, will an exception be thrown?
				try {GStore.findEphemeris(sid32,Comcrazy); testFramework.failTest();}
				catch (InvalidRequest) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

				//Write out findEphemeris data to output files
				fPRN1  << GStore.findEphemeris(sid1,ComTime);
				fPRN15 << GStore.findEphemeris(sid15,ComTime);
				fPRN32 << GStore.findEphemeris(sid32,ComTime);
			}
			catch (Exception& e)
			{
				//cout << e;
			}



			fPRN1.close();
			fPRN15.close();
			fPRN32.close();

			inputComparisonOutput1  = inputComparisonOutput + "findEph1.chk";
			inputComparisonOutput15 = inputComparisonOutput + "findEph15.chk";
			inputComparisonOutput32 = inputComparisonOutput + "findEph32.chk";

			testFramework.changeSourceMethod("findEphemeris Output");

//--------------RinexEphemerisStore_findEphTest_5 - Check findEphemeris output with pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput1, inputComparisonOutput1, 0) );
			testFramework.next();

//--------------RinexEphemerisStore_findEphTest_6 - Check findEphemeris output with pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput15, inputComparisonOutput15, 0) );
			testFramework.next();

//--------------RinexEphemerisStore_findEphTest_7 - Check findEphemeris output with pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput32, inputComparisonOutput32, 0) );
			testFramework.next();

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member getXvt()

	This test makes sure that exceptions are thrown if there is no ephemeris data
	for the given PRN and also that an exception is thrown if there is no data for
	the PRN at the given time. Furthermore, this test finds an Xvt for a given
	CivilTime Time and PRN.

	To see the Xvt information for the selected Time and PRN please see the files
	getXvt#.txt
========================================================================================================================= */

		int getXvtTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "getXvt", __FILE__, __LINE__ );
			testFramework.init();
			ofstream fPRN1;
			ofstream fPRN15;
			ofstream fPRN32;

			outputTestOutput1  = outputTestOutput + "getPrnXvt1.txt";
			outputTestOutput15 = outputTestOutput + "getPrnXvt15.txt";
			outputTestOutput32 = outputTestOutput + "getPrnXvt32.txt";

			fPRN1.open ( outputTestOutput1.c_str() );
			fPRN15.open( outputTestOutput15.c_str() );
			fPRN32.open( outputTestOutput32.c_str() );

			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

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

			CivilTime Time(2006,1,31,11,45,0,1);
			CivilTime bTime(2006,1,31,2,0,0,1); //Border Time (Time of Border test cases)
			const CommonTime ComTime = (CommonTime)Time;
			const CommonTime CombTime = (CommonTime)bTime;

			Xvt xvt1;
			Xvt xvt15;
			Xvt xvt32;
			try
			{
//--------------RinexEphemerisStore_getXvtTest_1 - Does getXvt work in ideal settings?
				try 
				{
					Store.getXvt(sid1,ComTime);
					testFramework.passTest();
				}
				catch (Exception& e)
				{
					cout << "Exception thrown is " << e << endl;
					testFramework.failTest();
				}

				xvt1 = Store.getXvt(sid1,ComTime);
				xvt15 = Store.getXvt(sid15,ComTime);
				xvt32 = Store.getXvt(sid32,ComTime);

				fPRN1 << xvt1 << endl;
				fPRN15 << xvt15 << endl;
				fPRN32 << xvt32 << endl;

//--------------RinexEphemerisStore_getXvtTest_2 - Can I get an xvt for a non-real SV?
				try 
				{
					Store.getXvt(sid0,CombTime);
					testFramework.failTest();
				}
				catch (InvalidRequest& e)
				{
					testFramework.passTest();
				}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_getXvtTest_3 - Can I get an xvt for a non-real SV?
				try 
				{
					Store.getXvt(sid33,CombTime);
					testFramework.failTest();
				}
				catch (InvalidRequest& e)
				{
					testFramework.passTest();
				}
				catch (...) {testFramework.failTest();}
			}
			catch (Exception& e)
			{
				//cout << e;
			}

			fPRN1.close(); 
			fPRN15.close(); 
			fPRN32.close();

			inputComparisonOutput1  = inputComparisonOutput + "getPrnXvt1.chk";
			inputComparisonOutput15 = inputComparisonOutput + "getPrnXvt15.chk";
			inputComparisonOutput32 = inputComparisonOutput + "getPrnXvt32.chk";

			testFramework.changeSourceMethod("getXvt Output");

//--------------RinexEphemerisStore_getXvtTest_4 - Compare data for SatID 1 with pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput1, inputComparisonOutput1, 0) );
			testFramework.next();

//--------------RinexEphemerisStore_getXvtTest_5 - Compare data for SatID 15 with pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput15, inputComparisonOutput15, 0) );
			testFramework.next();

//--------------RinexEphemerisStore_getXvtTest_6 - Compare data for SatID 32 with pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput32, inputComparisonOutput32, 0) );
			testFramework.next();

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member getXvt()
	This test differs from the previous in that this getXvt() has another parameter
	for the IODC

	This test makes sure that exceptions are thrown if there is no ephemeris data
	for the given PRN and also that an exception is thrown if there is no data for
	the PRN at the given time. Furthermore, this test finds an Xvt for a given
	CivilTime Time and PRN and IODC.

	To see the Xvt information for the selected Time and PRN please see
	getXvt2.txt

NOTE: getXvt with an IODC option is now deprecated. Test is no longer necessary, but is
      being left here in case the functionality returns.
========================================================================================================================= */
/*
		int getXvt2Test (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "getXvt with IODC", __FILE__, __LINE__ );
			testFramework.init();

			ofstream fPRN1;
			ofstream fPRN15;
			ofstream fPRN32;

			string filename;
			filename = __FILE__;
			filename = filename.substr(0, filename.find_last_of("\\/"));

			fPRN1.open ((filename+"/Logs/getXvt2_1.txt").c_str());
			fPRN15.open ((filename+"/Logs/getXvt2_15.txt").c_str());
			fPRN32.open ((filename+"/Logs/getXvt2_32.txt").c_str());

			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

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
//--------------RinexEphemerisStore_getXvt2Test_1 - Does getXvt work in ideal settings?
				try 
				{
					Store.getXvt(sid1,ComTime,IODC1);
					testFramework.passTest();
				}
				catch (...) {testFramework.failTest();}

				fPRN1 << Store.getXvt(sid1,ComTime,IODC1) << endl;
				fPRN15 << Store.getXvt(sid15,ComTime,IODC15) << endl;
				fPRN32 << Store.getXvt(sid32,ComTime,IODC32) << endl;


//--------------RinexEphemerisStore_getXvt2Test_2 - Is an error thrown when SatID is too small?
				try
				{
					Store.getXvt(sid0,CombTime,IODC0);
					testFramework.failTest();
				}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_getXvt2Test_3 - Is an error thrown when SatID is too large?
				try
				{
					Store.getXvt(sid33,CombTime,IODC33);
					testFramework.failTest();
				}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}
			}
			catch (Exception& e)
			{
				//cout << e;
			}

			fPRN1.close();
			fPRN15.close();
			fPRN32.close();

			testFramework.changeSourceMethod("getXvt with IODC Output");
//--------------RinexEphemerisStore_getXvt2Test_4 - Compare data for SatID 1 with pre-determined standard
			testFramework.assert(fileEqualTest((char*)(filename+"/Logs/getXvt2_1.txt").c_str(),(char*)(filename+"/Checks/getPrnXvt1.chk").c_str()));
			testFramework.next();
//--------------RinexEphemerisStore_getXvt2Test_5 - Compare data for SatID 15 with pre-determined standard
			testFramework.assert(fileEqualTest((char*)(filename+"/Logs/getXvt2_15.txt").c_str(),(char*)(filename+"/Checks/getPrnXvt15.chk").c_str()));
			testFramework.next();
//--------------RinexEphemerisStore_getXvt2Test_6 - Compare data for SatID 32 with pre-determined standard
			testFramework.assert(fileEqualTest((char*)(filename+"/Logs/getXvt2_32.txt").c_str(),(char*)(filename+"/Checks/getPrnXvt32.chk").c_str()));
			testFramework.next();

			return testFramework.countFails();
		}
*/


/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member getSatHealth()

	This test makes sure that exceptions are thrown if there is no ephemeris data
	for the given PRN and also that an exception is thrown if there is no data for
	the PRN at the given time. Furthermore, this test assures that for a specific PRN
	and Time, that SV is as we expect it, health (0).
========================================================================================================================= */
		int getSatHealthTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "getSatHealth", __FILE__, __LINE__ );
			testFramework.init();

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
			Store.loadFile(inputRinexNavData.c_str());

			std::list<GPSEphemeris> R3NList;
			GPSEphemerisStore GStore;
			list<GPSEphemeris>::const_iterator it;
			Store.addToList(R3NList);
			for (it = R3NList.begin(); it != R3NList.end(); ++it)
			  GStore.addEphemeris(GPSEphemeris(*it));

			CivilTime Time(2006,1,31,11,45,0,2);
			CivilTime bTime(2006,1,31,2,0,0,2); //Border Time (Time of Border test cases)
			const CommonTime ComTime = (CommonTime)Time;
			const CommonTime CombTime = (CommonTime)bTime;


			try
			{

//--------------RinexEphemerisStore_getSatHealthTest_1 - Does getSatHealth work in ideal conditions?
				try {GStore.getSatHealth(sid1,ComTime); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_getSatHealthTest_2 - Does getSatHealth return the proper value?
				testFramework.assert((short) 1 == GStore.getSatHealth(sid1,ComTime));
				testFramework.next();

//--------------RinexEphemerisStore_getSatHealthTest_3 - Does getSatHealth return the proper value?
				testFramework.assert((short) 1 == GStore.getSatHealth(sid15,ComTime));
				testFramework.next();

//--------------RinexEphemerisStore_getSatHealthTest_4 - Does getSatHealth return the proper value?
				testFramework.assert((short) 1 == GStore.getSatHealth(sid32,ComTime));
				testFramework.next();

//--------------RinexEphemerisStore_getSatHealthTest_5 - Does getSatHealth throw an error for bad SatID request?
				try {GStore.getSatHealth(sid0,CombTime); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_getSatHealthTest_6 - Does getSatHealth throw an error for bad SatID request?
				try {GStore.getSatHealth(sid33,CombTime); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_getSatHealthTest_7 - Does getSatHealth return the proper value for bad SatID?
				testFramework.assert((short) 0 == GStore.getSatHealth(sid0,ComTime));
				testFramework.next();

//--------------RinexEphemerisStore_getSatHealthTest_8 - Does getSatHealth return the proper value for bad SatID?
				testFramework.assert((short) 0 == GStore.getSatHealth(sid33,ComTime));
				testFramework.next();
			}
			catch (Exception& e)
			{
				//cout << e;
			}

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member dump()

	This test makes sure that dump() behaves as expected.  With paramters from
	1-3 with each giving more and more respective information, this information is
	then put into txt files.

	To see the dump with paramter 1, please view DumpData1.txt
	To see the dump with paramter 2, pleave view DumpData2.txt
	To see the dump with paramter 3, please view DumpData3.txt
========================================================================================================================= */
		int dumpTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "getSatHealth", __FILE__, __LINE__ );
			testFramework.init();

			ofstream DumpData0;
			ofstream DumpData1;
			ofstream DumpData2;

			outputTestOutput1  = outputTestOutput + "DumpData1.txt";
			outputTestOutput15 = outputTestOutput + "DumpData2.txt";
			outputTestOutput32 = outputTestOutput + "DumpData3.txt";

			DumpData0.open ( outputTestOutput1.c_str() );
			DumpData1.open ( outputTestOutput15.c_str() );
			DumpData2.open ( outputTestOutput32.c_str() );


			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

			try
			{
//--------------RinexEphemerisStore_dumpTest_1 - Check that dump( , detail = 1) will work with no exceptions
				try {Store.dump(DumpData0,1); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_dumpTest_2 - Check that dump( , detail = 2) will work with no exceptions
				try {Store.dump(DumpData1,2); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_dumpTest_3 - Check that dump( , detail = 3) will work with no exceptions
				try {Store.dump(DumpData2,3); testFramework.passTest();}
				catch (...) {testFramework.failTest();}


			}
			catch (Exception& e)
			{
				//cout << e;
			}

			DumpData0.close();
			DumpData1.close();
			DumpData2.close();

			inputComparisonOutput1  = inputComparisonOutput + "DumpData1.chk";
			inputComparisonOutput15 = inputComparisonOutput + "DumpData2.chk";
			inputComparisonOutput32 = inputComparisonOutput + "DumpData3.chk";

//--------------RinexEphemerisStore_dumpTest_4 - Check dump( , detail = 1) output against its pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput1, inputComparisonOutput1, 2) );
			testFramework.next();

//--------------RinexEphemerisStore_dumpTest_5 - Check dump( , detail = 2) output against its pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput15, inputComparisonOutput15, 2) );
			testFramework.next();

//--------------RinexEphemerisStore_dumpTest_6 - Check dump( , detail = 3) output against its pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput32, inputComparisonOutput32, 2) );
			testFramework.next();

			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member addEphemeris()

	This test assures that no exceptions are thrown when an ephemeris, taken from Store
	is added to a blank BCES Object.  Then the test makes sure that only that Ephemeris
	is in the object by checking the start and end times of the Object
========================================================================================================================= */

		int addEphemerisTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "addEphemeris", __FILE__, __LINE__ );
			testFramework.init();

			GPSEphemerisStore Blank;

			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

			std::list<GPSEphemeris> R3NList;
			GPSEphemerisStore GStore;
			list<GPSEphemeris>::const_iterator it;
			Store.addToList(R3NList);
			for (it = R3NList.begin(); it != R3NList.end(); ++it)
			{
			  GStore.addEphemeris(GPSEphemeris(*it));
			}

			short PRN = 1;
			SatID sid(PRN,SatID::systemGPS);

			CivilTime Time(2006,1,31,11,45,0,2);
			//CivilTime TimeB(2006,1,31,9,59,44,2);
			//CivilTime TimeE(2006,1,31,13,59,44,2);
			CivilTime TimeBeginning(2006,1,31,10,0,0,2);
			CivilTime TimeEnd(2006,1,31,14,0,0,2);
			CivilTime defaultBeginning(4713, 1, 1, 0, 0, 0, 2);
			CivilTime defaultEnd(-4713, 1, 1, 0, 0, 0, 2);
			CivilTime check;
			const CommonTime ComTime  = Time.convertToCommonTime();
			const CommonTime ComTimeB = TimeBeginning.convertToCommonTime();
			const CommonTime ComTimeE = TimeEnd.convertToCommonTime();
			const CommonTime ComDefB  = defaultBeginning.convertToCommonTime();
			const CommonTime ComDefE  = defaultEnd.convertToCommonTime();
			const GPSEphemeris eph = GStore.findEphemeris(sid,ComTime);

			try
			{

//--------------RinexEphemerisStore_addEphemeris_1 - Verify that addEphemeris runs with no errors
				try{Blank.addEphemeris(eph); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_addEphemeris_2 - Verify that addEphemeris added by checking the initial time of the GPSEphemerisStore
				testFramework.assert( ComTimeB == Blank.getInitialTime() );
				testFramework.next();

//--------------RinexEphemerisStore_addEphemeris_3 - Verify that addEphemeris added by checking the final time of the GPSEphemerisStore
				testFramework.assert( ComTimeE == Blank.getFinalTime() );
				testFramework.next();

				Blank.clear();

//--------------RinexEphemerisStore_addEphemeris_4 - Verify that clear() worked by checking the initial time of the GPSEphemerisStore
				testFramework.assert( ComDefB == Blank.getInitialTime() );
				testFramework.next();

//--------------RinexEphemerisStore_addEphemeris_5 - Verify that clear() worked by checking the initial time of the GPSEphemerisStore
				testFramework.assert( ComDefE == Blank.getFinalTime() );
				testFramework.next();
			}
			catch (Exception& e)
			{
				cout << e;
			}

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member edit()

	This test assures that no exceptions are thrown when we edit a RES object
	then after we edit the RES Object, we test to make sure that our edit time
	parameters are now the time endpoints of the object.

	For further inspection of the edit, please view editTest.txt
========================================================================================================================= */
		int editTest (void)
		{

			TestUtil testFramework( "RinexEphemerisStore", "edit", __FILE__, __LINE__ );
			testFramework.init();

			ofstream DumpData;
			outputTestOutput1 = outputTestOutput + "editTest.txt";
			DumpData.open (outputTestOutput1.c_str());

			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

			CivilTime TimeMax(2006,1,31,15,45,0,2);
			CivilTime TimeMin(2006,1,31,3,0,0,2);

			//cout << "TimeMax: " << TimeMax << "\n" << "TimeMin: " << TimeMin << "\n";

			const CommonTime ComTMax = (CommonTime)TimeMax;
			const CommonTime ComTMin = (CommonTime)TimeMin;

			//cout << "ComTMax: " << ComTMax << "\n" << "ComTMin: " << ComTMin << "\n";

			try
			{
//--------------RinexEphemerisStore_editTest_1 - Verify that the edit method runs
				try{Store.edit(ComTMin, ComTMax); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

				Store.edit(ComTMin, ComTMax);
//--------------RinexEphemerisStore_editTest_2 - Verify that the edit method changed the initial time
				testFramework.assert(ComTMin == Store.getInitialTime());
				testFramework.next();

//--------------RinexEphemerisStore_editTest_3 - Verify that the edit method changed the initial time
				testFramework.assert(ComTMax == Store.getFinalTime());
				testFramework.next();

				Store.dump(DumpData,1);

			}
			catch (Exception& e)
			{
				//cout << e;
			}
			inputComparisonOutput1 = inputComparisonOutput + "editTest.chk";
			DumpData.close();
//--------------RinexEphemerisStore_editTest_4 - Check edited output against its pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput1, inputComparisonOutput1, 0) );
			testFramework.next();

			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member wiper()

	This test assures that no exceptions are thrown when we wiper a RES object
	then after we wiper the RES Object, we test to make sure that our wiper time
	parameter in now the time endpoint of the object.

	For further inspection of the edit, please view wiperTest.txt

	Please note that this test also indirectly tests size
========================================================================================================================= */
/*		int wiperTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "wiper", __FILE__, __LINE__ );
			testFramework.init();

			ofstream DumpData1;
			ofstream DumpData2;
			outputTestOutput1 = outputTestOutput + "wiperTest.txt";
			outputTestOutput15 = outputTestOutput + "wiperTest2.txt";
			DumpData1.open (outputTestOutput1.c_str());
			DumpData2.open (outputTestOutput15.c_str());


			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

			std::list<GPSEphemeris> R3NList;
			GPSEphemerisStore GStore;
			list<GPSEphemeris>::const_iterator it;
			Store.addToList(R3NList);
			for (it = R3NList.begin(); it != R3NList.end(); ++it)
			{
			  GStore.addEphemeris(GPSEphemeris(*it));
			}

			CivilTime Time(2006,1,31,11,45,0,2);
			const CommonTime ComTime = (CommonTime)Time;

			try
			{
//--------------RinexEphemerisStore_wiperTest_1 - Verify that the wiper method runs (but shouldn't wipe anything this time)
				try {GStore.wiper(CommonTime::BEGINNING_OF_TIME); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

				//Wipe everything outside interval and make sure that we did wipe all the data
				//up to the provided time.
				GStore.wiper(ComTime);

				GStore.dump(DumpData1,1);

//--------------RinexEphemerisStore_wiperTest_2 - Verify that the new initial time is the time provided (partial wipe)
				testFramework.assert(ComTime == GStore.getInitialTime());
				testFramework.next();

				//Wipe everything, return size (should be zero)
				GStore.wiper(CommonTime::END_OF_TIME);
				unsigned int Num = GStore.gpstk::OrbitEphStore::size(); //Get the size of the GPSEphemerisStore

//--------------RinexEphemerisStore_wiperTest_3 - Verify that the store is empty (total wipe)
				testFramework.assert((unsigned int) 0 == Num);
				testFramework.next();

//--------------RinexEphemerisStore_wiperTest_4 - Verify that the initial time is the default END_OF_TIME (indicates empty GPSEphemerisStore)
				testFramework.assert(CommonTime::END_OF_TIME == GStore.getInitialTime());
				testFramework.next();

				GStore.dump(DumpData2,1);
			}
			catch (Exception& e)
			{
				//cout << e;
			}
			DumpData1.close();
			DumpData2.close();
			inputComparisonOutput1  = inputComparisonOutput + "wiperTest.chk";
			inputComparisonOutput15 = inputComparisonOutput + "wiperTest2.chk";

//--------------RinexEphemerisStore_dumpTest_5 - Check partially wiped output against its pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput1, inputComparisonOutput1, 0) );
			testFramework.next();

//--------------RinexEphemerisStore_dumpTest_6 - Check totally wiped output against its pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput15, inputComparisonOutput15, 0) );
			testFramework.next();

			return testFramework.countFails();
		}
*/

/* =========================================================================================================================
	Test to assure the quality of OrbitEphStore class member clear()

	This test assures that no exceptions are thrown when we clear a RES object
	then after we clear the RES Object, we test to make sure that END_OF_TIME is our
	initial time and BEGINNING_OF_TIME is our final time

	For further inspection of the edit, please view clearTest.txt
========================================================================================================================= */
		int clearTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "clear", __FILE__, __LINE__ );
			testFramework.init();

			ofstream DumpData;
			outputTestOutput1 = outputTestOutput + "clearTest.txt";
			DumpData.open(outputTestOutput1.c_str());

			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

			try
			{
//--------------RinexEphemerisStore_clearTest_1 - Verify the gpstk::OrbitEphStore::clear() method runs
				try {Store.gpstk::OrbitEphStore::clear(); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_clearTest_2 - Verify that clear set the initial time to END_OF_TIME
				testFramework.assert(CommonTime::END_OF_TIME == Store.getInitialTime());
				testFramework.next();

//--------------RinexEphemerisStore_clearTest_3 - Verify that clear set the initial time to END_OF_TIME
				testFramework.assert(CommonTime::BEGINNING_OF_TIME == Store.getFinalTime());
				testFramework.next();

				Store.dump(DumpData,1);
			}
			catch (Exception& e)
			{
				//cout << e;
			}
			DumpData.close();
			inputComparisonOutput1  = inputComparisonOutput + "clearTest.chk";

//--------------RinexEphemerisStore_clearTest_4 - Check partially wiped output against its pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput1, inputComparisonOutput1, 0) );
			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test to assure the quality of OrbitEphStore class member findUserOrbitEph()

	This test will be performed using OrbitEphStore's grand-child class GPSEphemerisStore

	findUserOrbitEph find the ephemeris which a) is within the fit tinterval for the
	given time of interest and 2) is the last ephemeris transmitted before the time of
	interest (i.e. min(toi-HOW time))

	This test makes sure that exceptions are thrown if there is no ephemeris data
	for the given PRN and also that an exception is thrown if there is no data for
	the PRN at the given time. Store is then cleared and the ephemeris data is readded
	for output purposes.

	For further inspection of the find, please view findUserTest.txt
========================================================================================================================= */
		int findUserOrbEphTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "findUserOrbitEph", __FILE__, __LINE__ );
			testFramework.init();

			ofstream DumpData;
			outputTestOutput1 = outputTestOutput + "findUserTest.txt";
			DumpData.open(outputTestOutput1.c_str());

			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

			OrbitEphStore orbEphStore; //Store for found ephemerides

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
//--------------RinexEphemerisStore_findUserOrbEphTest_1 - Check that a missing satID (too small) yields a thrown error
				try {Store.findUserOrbitEph(sid0,ComTime); testFramework.failTest();}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_findUserOrbEphTest_2 - Check that a missing satID (too big) yields a thrown error
				try {Store.findUserOrbitEph(sid33,ComTime); testFramework.failTest();}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_findUserOrbEphTest_3 - Check that an invalid time yields a thrown error
				try {Store.findUserOrbitEph(sid1,CommonTime::END_OF_TIME); testFramework.failTest();}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_findUserOrbEphTest_4 - Verify that for ideal conditions findUserOrbitEph runs
				try {Store.findUserOrbitEph(sid1, ComTime); testFramework.passTest();}
				catch (...) {testFramework.failTest();}

				const OrbitEph* Eph1 = Store.findUserOrbitEph(sid1, ComTime);
				const OrbitEph* Eph15 = Store.findUserOrbitEph(sid15, ComTime);
				const OrbitEph* Eph32 = Store.findUserOrbitEph(sid32, ComTime);

				orbEphStore.addEphemeris(Eph1);
				orbEphStore.addEphemeris(Eph15);
				orbEphStore.addEphemeris(Eph32);

				orbEphStore.dump(DumpData,1);

			}
			catch (Exception& e)
			{
				//cout << e;
			}

			DumpData.close();
			inputComparisonOutput1  = inputComparisonOutput + "findUserTest.chk";

//--------------RinexEphemerisStore_findUserOrbEphTest_5 - Check partially findUserOrbitEph output against its pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput1, inputComparisonOutput1, 0) );
			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member findNearOrbitEph()
	findNearOrbitEph finds the ephemeris with the HOW time closest to the time t, i.e
	with the smalles fabs(t-HOW), but still within the fit interval

	This test makes sure that exceptions are thrown if there is no ephemeris data
	for the given PRN and also that an exception is thrown if there is no data for
	the PRN at the given time. Store is then cleared and the epeheris data is readded
	for output purposes.

	For further inspection of the find, please view findNearTest.txt
========================================================================================================================= */



		int findNearOrbEphTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "findNearOrbitEph", __FILE__, __LINE__ );
			testFramework.init();

			ofstream DumpData;
			outputTestOutput1 = outputTestOutput + "findNearTest.txt";
			DumpData.open(outputTestOutput1.c_str());

			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

			OrbitEphStore orbEphStore; //Store for found ephemerides

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
//--------------RinexEphemerisStore_findNearOrbEphTest_1 - Check that a missing satID (too small) yields a thrown error
				try {Store.findNearOrbitEph(sid0,ComTime); testFramework.failTest();}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_findNearOrbEphTest_2 - Check that a missing satID (too big) yields a thrown error
				try {Store.findNearOrbitEph(sid33,ComTime); testFramework.failTest();}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_findNearOrbEphTest_3 - Check that an invalid time yields a thrown error
				try {Store.findNearOrbitEph(sid1,CommonTime::END_OF_TIME); testFramework.failTest();}
				catch (InvalidRequest& e) {testFramework.passTest();}
				catch (...) {testFramework.failTest();}

//--------------RinexEphemerisStore_findNearOrbEphTest_4 - Verify that for ideal conditions findNearOrbitEph runs
				try {Store.findNearOrbitEph(sid1, ComTime); testFramework.passTest();}
				catch (Exception& e) {cout << "Caught Exception: " << e << endl; testFramework.failTest();}
				catch (...) {testFramework.failTest();}

				const OrbitEph* Eph1 = Store.findUserOrbitEph(sid1, ComTime);
				const OrbitEph* Eph15 = Store.findUserOrbitEph(sid15, ComTime);
				const OrbitEph* Eph32 = Store.findUserOrbitEph(sid32, ComTime);

				orbEphStore.addEphemeris(Eph1);
				orbEphStore.addEphemeris(Eph15);
				orbEphStore.addEphemeris(Eph32);

				orbEphStore.dump(DumpData,1);

			}
			catch (Exception& e)
			{
				e.addLocation(FILE_LOCATION);
				cout << e;
			}
			DumpData.close();
			inputComparisonOutput1  = inputComparisonOutput + "findUserTest.chk";

//--------------RinexEphemerisStore_findNearOrbEphTest_5 - Check partially findNearOrbitEph output against its pre-determined standard
			testFramework.assert( testFramework.fileEqualTest( outputTestOutput1, inputComparisonOutput1, 0) );
			return testFramework.countFails();
		}



/* =========================================================================================================================
	Test to assure the quality of GPSEphemerisStore class member addToList()

	This test creats a list of GPSEphemeris and then adds all of the ephemeris
	members to that list.  After that of the List and Store are checked to be
	equal.
========================================================================================================================= */

		int addToListTest (void)
		{
			TestUtil testFramework( "RinexEphemerisStore", "addToList", __FILE__, __LINE__ );
			testFramework.init();

			const short PRN1 = 1;
			const short PRN15 = 15;
			const short PRN32 = 32;

			unsigned numberOfEntries = 41;
			unsigned numberOfEntries1 = 15;
			unsigned numberOfEntries15 = 13;
			unsigned numberOfEntries32 = 13;

			SatID sid1(PRN1,SatID::systemGPS);
			SatID sid15(PRN15,SatID::systemGPS);
			SatID sid32(PRN32,SatID::systemGPS);

			RinexEphemerisStore Store;
			Store.loadFile(inputRinexNavData.c_str());

			std::list<GPSEphemeris> R3NList;
			GPSEphemerisStore GStore;
			list<GPSEphemeris>::const_iterator it;
			Store.addToList(R3NList);
			for (it = R3NList.begin(); it != R3NList.end(); ++it)
			{
			  GStore.addEphemeris(GPSEphemeris(*it));
//			  GPSEphemeris(*it).dumpBody(cout);
			}
			try
			{

				//Assert that the number of added members equals the size of Store (all members added)
				testFramework.assert(Store.gpstk::OrbitEphStore::size() == GStore.gpstk::OrbitEphStore::size());
				testFramework.next();

				testFramework.assert(Store.gpstk::OrbitEphStore::size() == numberOfEntries);
				testFramework.next();


				testFramework.assert(Store.gpstk::OrbitEphStore::size(sid1) == numberOfEntries1);
				testFramework.next();


				testFramework.assert(Store.gpstk::OrbitEphStore::size(sid15) == numberOfEntries15);
				testFramework.next();


				testFramework.assert(Store.gpstk::OrbitEphStore::size(sid32) == numberOfEntries32);
				testFramework.next();
			}
			catch (Exception& e)
			{
				//cout << e;
			}
			return testFramework.countFails();
		}

/* =========================================================================================================================
	Initialize Test Data Filenames
========================================================================================================================= */

		void init( void )
		{
			dataFilePath = __FILE__;
			dataFilePath = dataFilePath.substr(0, dataFilePath.find_last_of("\\/"));

			inputRinexNavData        = dataFilePath  + "/" + "TestRinex06.031";

			outputTestOutput         =  dataFilePath + "/" + "Logs" + "/";
			outputTestOutput1;
			outputTestOutput15;
			outputTestOutput32;
			outputDataDump           =  dataFilePath + "/" + "DataDump.txt";

			inputNotaFile            =  dataFilePath + "/" + "NotaFILE";

			inputComparisonOutput    =  dataFilePath + "/" + "Checks" + "/";
			inputComparisonOutput1;
			inputComparisonOutput15;
			inputComparisonOutput32;
		}

    private:

        std::string dataFilePath;

        std::string inputRinexNavData;

        std::string outputTestOutput;
        std::string outputTestOutput1;
        std::string outputTestOutput15;
        std::string outputTestOutput32;
        std::string outputDataDump;

        std::string inputNotaFile;

	std::string inputComparisonOutput;
	std::string inputComparisonOutput1;
	std::string inputComparisonOutput15;
	std::string inputComparisonOutput32;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	RinexEphemerisStore_T testClass;
	testClass.init();

	check = testClass.RESTest();
	errorCounter += check;

	check = testClass.findEphTest();
	errorCounter += check;

	check = testClass.getXvtTest();
	errorCounter += check;

	check = testClass.getSatHealthTest();
	errorCounter += check;

	check = testClass.dumpTest();
	errorCounter += check;

	check = testClass.addToListTest();
	errorCounter += check;

	check = testClass.addEphemerisTest();
	errorCounter += check;

	check = testClass.editTest();
	errorCounter += check;

	check = testClass.clearTest();
	errorCounter += check;

	check = testClass.findUserOrbEphTest();
	errorCounter += check;

	check = testClass.findNearOrbEphTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
