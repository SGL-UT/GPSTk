// file: xSP3EphemerisStore.cpp

#include "xSP3EphemerisStore.hpp"
#include "SatID.hpp"
#include "Exception.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xSP3EphemerisStore);


void xSP3EphemerisStore :: setUp (void)
{ 
}

/*
**** General test for the SP3EphemerisStore class
**** Test to assure the that RES throws its exceptions in the right place and
**** that it loads the SP3 file correctly

**** To further check this data, please view DumpData.txt for the dumped information
*/

void xSP3EphemerisStore :: RESTest (void)
{
	ofstream DumpData;
	DumpData.open ("DumpData.txt");
	
	CPPUNIT_ASSERT_NO_THROW(Rinex3::SP3EphemerisStore Store);
	Rinex3::SP3EphemerisStore Store;
	try
	{
	  CPPUNIT_ASSERT_THROW(Store.loadFile("NotaFILE"),gpstk::FileMissingException);
	}
	catch (gpstk::Exception& e)
	{
	  cout << "unexpected exception thrown" << endl;
	  cout << e << endl;
	}
	
	CPPUNIT_ASSERT_NO_THROW(Store.loadFile("NGA15081Test.SP3"));
	Store.loadFile("NGA15081Test.SP3");
	Store.dump(DumpData,1);
	DumpData.close();
	
}

/*
**** Test to assure the quality of SP3EphemerisStore class member getXvt()

**** This test makes sure that exceptions are thrown if there is no ephemeris data
**** for the given PRN and also that an exception is thrown if there is no data for
**** the PRN at the given time. Furthermore, this test finds an Xvt for a given
**** DayTime Time and PRN.

**** To see the Xvt information for the selected Time and PRN please see
**** getXvt.txt
*/
void xSP3EphemerisStore :: getXvtTest (void)
{
/*
	ofstream fPRN1;
	ofstream fPRN15;
	ofstream fPRN32;

	fPRN1.open ("Logs/getXvt1.txt");
	fPRN15.open ("Logs/getXvt15.txt");
	fPRN32.open ("Logs/getXvt32.txt");
	
	Rinex3::SP3EphemerisStore Store;
	Store.loadFile("NGA15081Test.SP3");
	
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
	
	gpstk::DayTime Time(2006,1,31,11,45,0);
	gpstk::DayTime bTime(2006,1,31,2,0,0); //Border Time (Time of Border test cases)
	
	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.getXvt(sid1,Time));

		fPRN1 << Store.getXvt(sid1,Time) << endl;
		fPRN15 << Store.getXvt(sid15,Time) << endl;
		fPRN32 << Store.getXvt(sid32,Time) << endl;

		CPPUNIT_ASSERT_THROW(Store.getXvt(sid0,bTime),gpstk::InvalidRequest);
		CPPUNIT_ASSERT_THROW(Store.getXvt(sid33,bTime),gpstk::InvalidRequest);	
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}

	CPPUNIT_ASSERT(fileEqualTest("Logs/getXvt1.txt","Checks/getPrnXvt1.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/getXvt15.txt","Checks/getPrnXvt15.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/getXvt32.txt","Checks/getPrnXvt32.chk"));
*/
}
/*
**** Test to assure the quality of SP3EphemerisStore class member dump()

**** This test makes sure that dump() behaves as expected.  With paramters from
**** 0-2 with each giving more and more respective information, this information is
**** then put into txt files.

**** To see the dump with paramter 0, please view DumpData0.txt
**** To see the dump with paramter 1, pleave view DumpData1.txt
**** To see the dump with paramter 2, please uncomment the test and view the command
**** line output (cout).
*/

void xSP3EphemerisStore :: dumpTest (void)
{
/*
	ofstream DumpData0;
	ofstream DumpData1;
	ofstream DumpData2;
	DumpData0.open ("Logs/DumpData0.txt");
	DumpData1.open ("Logs/DumpData1.txt");
	DumpData2.open ("Logs/DumpData2.txt");
		
	Rinex3::SP3EphemerisStore Store;
	Store.loadFile("NGA15081Test.SP3");

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
	CPPUNIT_ASSERT(fileEqualTest("Logs/DumpData0.txt","Checks/DumpData0.chk"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/DumpData1.txt","Checks/DumpData1.chk"));
	//CPPUNIT_ASSERT(fileEqualTest("Logs/DumpData2.txt","Checks/DumpData2.chk"));
*/
}

/*
**** Test to assure the quality of SP3EphemerisStore class member addEphemeris()

**** This test assures that no exceptions are thrown when a an ephemeris, taken from Store
**** is added to a blank  Object.  Then the test makes sure that only that Ephemeris
**** is in the object by checking the start and end times of the Object

**** Question:  Why does this eph data begin two hours earlier than it does on the output?
*/

void xSP3EphemerisStore :: addEphemerisTest (void)
{
/*
	ofstream DumpData;
	DumpData.open ("Logs/addEphemerisTest.txt");


	Rinex3::SP3EphemerisStore Blank;
	Rinex3::SP3EphemerisStore Store;
	Store.loadFile("NGA15081Test.SP3");
	short PRN = 1;
   	gpstk::SatID sid(PRN,gpstk::SatID::systemGPS);
	
	gpstk::DayTime Time(2006,1,31,11,45,0);
	gpstk::DayTime TimeB(2006,1,31,9,59,44);
	const gpstk::EngEphemeris& eph = Store.findEphemeris(sid,Time);

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Blank.addEphemeris(eph));
		Blank.addEphemeris(eph);
		
		CPPUNIT_ASSERT_EQUAL(TimeB,Blank.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(TimeB,Blank.getFinalTime());
		
		Blank.dump(DumpData,1);
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/addEphemerisTest.txt","Checks/addEphemerisTest.chk"));
*/
}

/*
**** Test to assure the quality of SP3EphemerisStore class member edit()

**** This test assures that no exceptions are thrown when we edit a RES object
**** then after we edit the RES Object, we test to make sure that our edit time
**** parameters are now the time endpoints of the object.

**** For further inspection of the edit, please view editTest.txt
*/

void xSP3EphemerisStore :: editTest (void)
{
/*
	ofstream DumpData;
	DumpData.open ("Logs/editTest.txt");

	Rinex3::SP3EphemerisStore Store;
	Store.loadFile("NGA15081Test.SP3");
	
	gpstk::DayTime TimeMax(2006,1,31,15,45,0);
	gpstk::DayTime TimeMin(2006,1,31,3,0,0);

	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.edit(TimeMin, TimeMax));
		Store.edit(TimeMin, TimeMax);
		CPPUNIT_ASSERT_EQUAL(TimeMin,Store.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(TimeMax,Store.getFinalTime());
		Store.dump(DumpData,1);
		
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/editTest.txt","Checks/editTest.chk"));
*/
}

/*
**** Test to assure the quality of SP3EphemerisStore class member clear()

**** This test assures that no exceptions are thrown when we clear a SP3 object
**** then after we clear the SP3 Object, we test to make sure that END_OF_TIME is our
**** initial time and BEGINNING_OF_TIME is our final time

**** For further inspection of the edit, please view clearTest.txt
*/

void xSP3EphemerisStore :: clearTest (void)
{
/*
	ofstream DumpData;
	DumpData.open ("Logs/clearTest.txt");

	Rinex3::SP3EphemerisStore Store;
	Store.loadFile("NGA15081Test.SP3");
	
	try
	{
		CPPUNIT_ASSERT_NO_THROW(Store.clear());
		
		CPPUNIT_ASSERT_EQUAL(gpstk::DayTime::END_OF_TIME,Store.getInitialTime());
		CPPUNIT_ASSERT_EQUAL(gpstk::DayTime::END_OF_TIME,Store.getFinalTime());
		Store.dump(DumpData,1);
		
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/clearTest.txt","Checks/clearTest.chk"));
*/
}

void xSP3EphemerisStore :: dumpBadPositionsTest (void)
{
}

void xSP3EphemerisStore :: dumpBadClocksTest (void)
{
}
void xSP3EphemerisStore :: maxIntervalTest (void)
{
}
void xSP3EphemerisStore :: getTimeTest (void)
{
}

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
