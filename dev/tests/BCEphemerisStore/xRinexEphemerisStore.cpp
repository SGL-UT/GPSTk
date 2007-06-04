// file: xRinexEphemerisStore.cpp

#include "xRinexEphemerisStore.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION (xRinexEphemerisStore);


void xRinexEphemerisStore :: setUp (void)
{ 
}

/*
**** General test for the RinexEphemerisStore (RES) class
**** Test to assure the that RES throws its exceptions in the right place and
**** that it loads the RINEX Nav file correctly

**** To further check this data, please view DumpData.txt for the dumped information
*/

void xRinexEphemerisStore :: RESTest (void)
{
	ofstream DumpData;
	DumpData.open ("Logs/DumpData.txt");
	
	CPPUNIT_ASSERT_NO_THROW(gpstk::RinexEphemerisStore Store);
	gpstk::RinexEphemerisStore Store;
	try
	{
		CPPUNIT_ASSERT_THROW(Store.loadFile("NotaFILE"),gpstk::Exception);
	}
	catch (gpstk::Exception& e)
	{
	}
	
	CPPUNIT_ASSERT_NO_THROW(Store.loadFile("TestRinex06.031"));
	Store.loadFile("TestRinex06.031");
	Store.dump(1,DumpData);
	DumpData.close();
	
}

/*
**** Test to assure the quality of BCEphemerisStore class member findEph()

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test finds an Ephemeris for a given
**** DayTime Time and PRN.

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
	
	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
	
	gpstk::DayTime Time(2006,1,31,11,45,0);
	gpstk::DayTime bTime(2006,1,31,2,0,0); //Border Time (Time of Border test cases)
	
	
	try
	{
		gpstk::DayTime crazy(200000,1,31,2,0,0);
		CPPUNIT_ASSERT_NO_THROW(Store.findEphemeris(PRN1,Time));
		
		fPRN1 << Store.findEphemeris(PRN1,Time);
		fPRN15 << Store.findEphemeris(PRN15,Time);
		fPRN32 << Store.findEphemeris(PRN32,Time);
	
		CPPUNIT_ASSERT_THROW(Store.findEphemeris(PRN0,bTime),gpstk::EphemerisStore::NoEphemerisFound);
		CPPUNIT_ASSERT_THROW(Store.findEphemeris(PRN33,bTime),gpstk::EphemerisStore::NoEphemerisFound);
		CPPUNIT_ASSERT_THROW(Store.findEphemeris(PRN32,crazy),gpstk::EphemerisStore::NoEphemerisFound);
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest("Logs/findEph1.txt","Checks/findEph1.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/findEph15.txt","Checks/findEph15.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/findEph32.txt","Checks/findEph32.chk"));

}


/*
**** Test to assure the quality of BCEphemerisStore class member getPrnXvt()

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test finds an Xvt for a given
**** DayTime Time and PRN.

**** To see the Xvt information for the selected Time and PRN please see
**** getPrnXvt.txt
*/

void xRinexEphemerisStore :: BCESgetPrnXvtTest (void)
{
	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN32;

	fPRN1.open ("Logs/getPrnXvt1.txt");
	fPRN15.open ("Logs/getPrnXvt15.txt");
	fPRN32.open ("Logs/getPrnXvt32.txt");
	
	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
	
	gpstk::DayTime Time(2006,1,31,11,45,0);
	gpstk::DayTime bTime(2006,1,31,2,0,0); //Border Time (Time of Border test cases)
	
	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.getPrnXvt(PRN1,Time));

		fPRN1 << Store.getPrnXvt(PRN1,Time) << endl;
		fPRN15 << Store.getPrnXvt(PRN15,Time) << endl;
		fPRN32 << Store.getPrnXvt(PRN32,Time) << endl;

		CPPUNIT_ASSERT_THROW(Store.getPrnXvt(PRN0,bTime),gpstk::EphemerisStore::NoEphemerisFound);
		CPPUNIT_ASSERT_THROW(Store.getPrnXvt(PRN33,bTime),gpstk::EphemerisStore::NoEphemerisFound);	
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest("Logs/getPrnXvt1.txt","Checks/getPrnXvt1.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/getPrnXvt15.txt","Checks/getPrnXvt15.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/getPrnXvt32.txt","Checks/getPrnXvt32.chk"));
}


/*
**** Test to assure the quality of BCEphemerisStore class member getPrnXvt()
**** This test differs from the previous in that this getPrnXvt() has another parameter
**** for the IODC

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test finds an Xvt for a given
**** DayTime Time and PRN and IODC.

**** To see the Xvt information for the selected Time and PRN please see
**** getPrnXvt2.txt
*/
void xRinexEphemerisStore :: BCESgetPrnXvt2Test (void)
{

	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN32;

	fPRN1.open ("Logs/getPrnXvt2_1.txt");
	fPRN15.open ("Logs/getPrnXvt2_15.txt");
	fPRN32.open ("Logs/getPrnXvt2_32.txt");
	
	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
	
	short IODC0 = 89;
	short IODC1 = 372;
	short IODC15 = 455;
	short IODC32 = 441;
	short IODC33 = 392;
	
	gpstk::DayTime Time(2006,1,31,11,45,0);
	gpstk::DayTime bTime(2006,1,31,2,0,0); //Border Time (Time of Border test cases)
	
	try
	{
		fPRN1 << Store.getPrnXvt(PRN1,Time,IODC1) << endl;
		fPRN15 << Store.getPrnXvt(PRN15,Time,IODC15) << endl;
		fPRN32 << Store.getPrnXvt(PRN32,Time,IODC32) << endl;
		
		CPPUNIT_ASSERT_THROW(Store.getPrnXvt(PRN0,bTime,IODC0),gpstk::EphemerisStore::NoEphemerisFound);
		CPPUNIT_ASSERT_THROW(Store.getPrnXvt(PRN33,bTime,IODC33),gpstk::EphemerisStore::NoEphemerisFound);
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest("Logs/getPrnXvt2_1.txt","Checks/getPrnXvt1.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/getPrnXvt2_15.txt","Checks/getPrnXvt15.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/getPrnXvt2_32.txt","Checks/getPrnXvt32.chk"));
}

/*
**** Test to assure the quality of BCEphemerisStore class member getPrnHealth()

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test assures that for a specific PRN
**** and Time, that SV is as we expect it, health (0).
*/

void xRinexEphemerisStore :: BCESgetPrnHealthTest (void)
{
	const short PRN0 = 0; // Zero PRN (Border test case)
	const short PRN1 = 1;
	const short PRN15 = 15;
	const short PRN32 = 32;
	const short PRN33 = 33;  //Top PRN (33) (Border test case);
	
	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	gpstk::DayTime Time(2006,1,31,11,45,0);
	gpstk::DayTime bTime(2006,1,31,2,0,0); //Border Time (Time of Border test cases)
	
	try
	{
		
		
		CPPUNIT_ASSERT_NO_THROW(Store.getPrnHealth(PRN1,Time));
		
		CPPUNIT_ASSERT_EQUAL((short) 0,Store.getPrnHealth(PRN1,Time));
		CPPUNIT_ASSERT_EQUAL((short) 0,Store.getPrnHealth(PRN15,Time));
		CPPUNIT_ASSERT_EQUAL((short) 0,Store.getPrnHealth(PRN32,Time));
		
		CPPUNIT_ASSERT_THROW(Store.getPrnHealth(PRN0,bTime),gpstk::EphemerisStore::NoEphemerisFound);
		CPPUNIT_ASSERT_THROW(Store.getPrnHealth(PRN33,bTime),gpstk::EphemerisStore::NoEphemerisFound);
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
}

/*
**** Test to assure the quality of BCEphemerisStore class member dump()

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
		
	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.dump(0,DumpData0));
		CPPUNIT_ASSERT_NO_THROW(Store.dump(1,DumpData1));
		//Code outputs to cout but does pass, just dont want to run that every time
		//CPPUNIT_ASSERT_NO_THROW(Store.dump(2,DumpData2));
		//Store.dump(2,DumpData2);
		
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/DumpData0.txt","Checks/DumpData0.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/DumpData1.txt","Checks/DumpData1.chk"));
	//CPPUNIT_ASSERT(fileEqualTest("Logs/DumpData2.txt","Checks/DumpData2.chk"));
}

/*
**** Test to assure the quality of BCEphemerisStore class member addEphemeris()

**** This test assures that no exceptions are thrown when a an ephemeris, taken from Store
**** is added to a blank BCES Object.  Then the test makes sure that only that Ephemeris
**** is in the object by checking the start and end times of the Object

**** Question:  Why does this eph data begin two hours earlier than it does on the output?
*/

void xRinexEphemerisStore :: BCESaddEphemerisTest (void)
{
	ofstream DumpData;
	DumpData.open ("Logs/addEphemerisTest.txt");


	gpstk::BCEphemerisStore Blank;
	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	short PRN = 1;
	
	gpstk::DayTime Time(2006,1,31,11,45,0);
	gpstk::DayTime TimeB(2006,1,31,9,59,44);
	const gpstk::EngEphemeris& eph = Store.findEphemeris(PRN,Time);

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Blank.addEphemeris(eph));
		Blank.addEphemeris(eph);
		
		CPPUNIT_ASSERT_EQUAL(TimeB,Blank.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(TimeB,Blank.getFinalTime());
		
		Blank.dump(1,DumpData);
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/addEphemerisTest.txt","Checks/addEphemerisTest.chk"));
}

/*
**** Test to assure the quality of BCEphemerisStore class member edit()

**** This test assures that no exceptions are thrown when we edit a RES object
**** then after we edit the RES Object, we test to make sure that our edit time
**** parameters are now the time endpoints of the object.

**** For further inspection of the edit, please view editTest.txt
*/

void xRinexEphemerisStore :: BCESeditTest (void)
{
	ofstream DumpData;
	DumpData.open ("Logs/editTest.txt");

	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	gpstk::DayTime TimeMax(2006,1,31,15,45,0);
	gpstk::DayTime TimeMin(2006,1,31,3,0,0);

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.edit(TimeMin, TimeMax));
		Store.edit(TimeMin, TimeMax);
		CPPUNIT_ASSERT_EQUAL(TimeMin,Store.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(TimeMax,Store.getFinalTime());
		Store.dump(1,DumpData);
		
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/editTest.txt","Checks/editTest.chk"));
}

/*
**** Test to assure the quality of BCEphemerisStore class member wiper()

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

	
	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	gpstk::DayTime Time(2006,1,31,11,45,0);
	
	try
	{
		//Make sure it doesn't fail but dont wipe anything
		CPPUNIT_ASSERT_NO_THROW(Store.wiper(gpstk::DayTime::BEGINNING_OF_TIME));
		//Wipe everything and make sure that we did wipe all the data
		Store.wiper(Time);
		Store.dump(1,DumpData1);
		
		CPPUNIT_ASSERT_EQUAL(Time,Store.getInitialTime());
		
		unsigned int Num = Store.ubeSize();
		
		CPPUNIT_ASSERT_EQUAL(Num,Store.wiper(gpstk::DayTime::END_OF_TIME));
		
		Store.dump(1,DumpData2);
		
		CPPUNIT_ASSERT_EQUAL(gpstk::DayTime::END_OF_TIME,Store.getInitialTime());
		
		
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/wiperTest.txt","Checks/wiperTest.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/wiperTest.txt","Checks/wiperTest.chk"));
}

/*
**** Test to assure the quality of BCEphemerisStore class member clear()

**** This test assures that no exceptions are thrown when we clear a RES object
**** then after we clear the RES Object, we test to make sure that END_OF_TIME is our
**** initial time and BEGINNING_OF_TIME is our final time

**** For further inspection of the edit, please view clearTest.txt
*/
void xRinexEphemerisStore :: BCESclearTest (void)
{
	ofstream DumpData;
	DumpData.open ("Checks/clearTest.chk");

	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.clear());
		
		CPPUNIT_ASSERT_EQUAL(gpstk::DayTime::END_OF_TIME,Store.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(gpstk::DayTime::BEGINNING_OF_TIME,Store.getFinalTime());
		Store.dump(1,DumpData);
		
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/clearTest.txt","Checks/clearTest.chk"));
}

/*
**** Test to assure the quality of BCEphemerisStore class member findUserEphemeris()

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

	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	gpstk::DayTime Time(2006,1,31,13,0,1);
	
	short PRN0 = 0;
	short PRN1 = 1;
	short PRN15 = 15;
	short PRN32 = 32;
	short PRN33 = 33;
	
	try
	{
		CPPUNIT_ASSERT_THROW(Store.findUserEphemeris(PRN0,Time),gpstk::EphemerisStore::NoEphemerisFound);
		CPPUNIT_ASSERT_THROW(Store.findUserEphemeris(PRN33,Time),gpstk::EphemerisStore::NoEphemerisFound);
		CPPUNIT_ASSERT_THROW(Store.findUserEphemeris(PRN1,gpstk::DayTime::END_OF_TIME),
					gpstk::EphemerisStore::NoEphemerisFound);
		
		CPPUNIT_ASSERT_NO_THROW(Store.findUserEphemeris(PRN1, Time));
		
		const gpstk::EngEphemeris& Eph1 = Store.findUserEphemeris(PRN1, Time);
		const gpstk::EngEphemeris& Eph15 = Store.findUserEphemeris(PRN15, Time);
		const gpstk::EngEphemeris& Eph32 = Store.findUserEphemeris(PRN32, Time);
		
		Store.clear();
		
		Store.addEphemeris(Eph1);
		Store.addEphemeris(Eph15);
		Store.addEphemeris(Eph32);
		
		Store.dump(1,DumpData);
		
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/findUserTest.txt","Checks/findUserTest.chk"));
}

/*
**** Test to assure the quality of BCEphemerisStore class member findNearEphemeris()
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

	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	gpstk::DayTime Time(2006,1,31,13,0,1);
	
	short PRN0 = 0;
	short PRN1 = 1;
	short PRN15 = 15;
	short PRN32 = 32;
	short PRN33 = 33;
	
	try
	{
		CPPUNIT_ASSERT_THROW(Store.findNearEphemeris(PRN0,Time),gpstk::EphemerisStore::NoEphemerisFound);
		CPPUNIT_ASSERT_THROW(Store.findNearEphemeris(PRN33,Time),gpstk::EphemerisStore::NoEphemerisFound);
		CPPUNIT_ASSERT_THROW(Store.findNearEphemeris(PRN1,gpstk::DayTime::END_OF_TIME),
					gpstk::EphemerisStore::NoEphemerisFound);
		
		CPPUNIT_ASSERT_NO_THROW(Store.findNearEphemeris(PRN1, Time));
		
		const gpstk::EngEphemeris& Eph1 = Store.findNearEphemeris(PRN1, Time);
		const gpstk::EngEphemeris& Eph15 = Store.findNearEphemeris(PRN15, Time);
		const gpstk::EngEphemeris& Eph32 = Store.findNearEphemeris(PRN32, Time);
		
		Store.clear();
		
		Store.addEphemeris(Eph1);
		Store.addEphemeris(Eph15);
		Store.addEphemeris(Eph32);
		
		Store.dump(1,DumpData);
		
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/findNearTest.txt","Checks/findNearTest.chk"));
}



/*
**** Test to assure the quality of BCEphemerisStore class member addToList()

**** This test creats a list of EngEphemeris and then adds all of the ephemeris
**** members to that list.  After that of the List and Store are checked to be
**** equal.

**** For further inspection of the add, please view addToListTest.txt
*/

void xRinexEphemerisStore :: BCESaddToListTest (void)
{
	ofstream DumpData;
	DumpData.open ("Logs/addToListTest.txt");

	
	gpstk::RinexEphemerisStore Store;
	Store.loadFile("TestRinex06.031");
	
	try
	{
		std::list<gpstk::EngEphemeris> EphList; // Empty Ephemeris List
		
		//Assert that the number of added members equals the size of Store (all members added)
		CPPUNIT_ASSERT_EQUAL(Store.ubeSize(),(unsigned) Store.addToList(EphList));
		CPPUNIT_ASSERT_EQUAL((unsigned) EphList.size(),Store.ubeSize());
		
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
	CPPUNIT_ASSERT(fileEqualTest("Logs/addToListTest.txt","Checks/addToListTest.chk"));
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
