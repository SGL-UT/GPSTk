// file: xSP3EphemerisStore.cpp

#include "xSP3EphemerisStore.hpp"
#include "SatID.hpp"
#include "Exception.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"
#include "SP3Stream.hpp"

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
      using namespace std;
      cout << "Reading " << "NGA15081Test.SP3" << endl;
      Rinex3::SP3Stream roffs("NGA15081Test.SP3");
      cout << "Writing " << "Output.txt" << endl;
      Rinex3::SP3Stream out("Output1.txt",ios::out);
      //out.exceptions(fstream::failbit);
      Rinex3::SP3Header roh;
      Rinex3::SP3Data roe;

      roffs >> roh;
      out << roh;
      roh.dump(cout);

      while (roffs >> roe)
      {
         out << roe;
	 roe.dump(cout);
      }
      CPPUNIT_ASSERT(fileEqualTest("NGA15081Test.SP3","Output1.txt"));

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
