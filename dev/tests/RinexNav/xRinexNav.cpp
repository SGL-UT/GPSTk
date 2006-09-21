#include "xRinexNav.hpp"
#include "Exception.hpp"
#include "RinexEphemerisStore.hpp"
#include <string>
#include "StringUtils.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xRinexNav);

using namespace gpstk;

void xRinexNav :: setUp (void)
{
}
/*
**** This test checks to make sure that the internal members of the RinexNavHeader are as we think they
**** should be.  Also at the end of this test, we check and make sure our output file is equal to our input
**** This assures that if any changes happen, the test will fail and the user will know.  Also, output was put
**** into input three times over to make sure there were no small errors which blow up into big errors
*/
void xRinexNav :: hardCodeTest (void)
{
	try{
		gpstk::RinexNavStream RinexNavStream("Logs/RinexNavExample.99n");
		gpstk::RinexNavStream out("Logs/TestOutput.99n",ios::out);
		gpstk::RinexNavStream dmp("Logs/RinexDump",ios::out);
		gpstk::RinexNavHeader RinexNavHeader;
		gpstk::RinexNavData RinexNavData;
		RinexNavStream >> RinexNavHeader;
		out << RinexNavHeader;
		
		while (RinexNavStream >> RinexNavData)
		{
			out << RinexNavData;
		}
		
		
		CPPUNIT_ASSERT_EQUAL(2.1,RinexNavHeader.version);
		
		CPPUNIT_ASSERT_EQUAL((string)"XXRINEXN V3",RinexNavHeader.fileProgram);
		CPPUNIT_ASSERT_EQUAL((string)"AIUB", RinexNavHeader.fileAgency);
		CPPUNIT_ASSERT_EQUAL((string)"09/02/1999 19:22:36",RinexNavHeader.date);
	
		vector<string>::const_iterator itr1 = RinexNavHeader.commentList.begin();
		CPPUNIT_ASSERT_EQUAL((string)"THIS IS ONE COMMENT",(*itr1));
	
		CPPUNIT_ASSERT(fileEqualTest("Logs/RinexNavExample.99n","Logs/TestOutput.99n"));
		
		gpstk::RinexNavStream RinexNavStream2("Logs/TestOutput.99n");
		gpstk::RinexNavStream out2("Logs/TestOutput2.99n",ios::out);
		gpstk::RinexNavHeader RinexNavHeader2;
		gpstk::RinexNavData RinexNavData2;
		RinexNavStream2 >> RinexNavHeader2;
		out2 << RinexNavHeader2;

		while (RinexNavStream2 >> RinexNavData2)
		{
			out2 << RinexNavData2;
		}
		
		gpstk::RinexNavStream RinexNavStream3("Logs/TestOutput2.99n");
		gpstk::RinexNavStream out3("Logs/TestOutput3.99n",ios::out);
		gpstk::RinexNavHeader RinexNavHeader3;
		gpstk::RinexNavData RinexNavData3;
		RinexNavStream3 >> RinexNavHeader3;
		out3 << RinexNavHeader3;

		while (RinexNavStream3 >> RinexNavData3)
		{
			out3 << RinexNavData3;
		}
		RinexNavHeader.dump(dmp);
		RinexNavData.dump(dmp);
		CPPUNIT_ASSERT(fileEqualTest("Logs/RinexNavExample.99n","Logs/TestOutput3.99n"));
	}
	catch (gpstk::Exception& e)
	{
		cout << e;
	}
}

/*
**** This test check that Rinex Header exceptions are thrown 
*/
void xRinexNav :: headerExceptionTest (void)
{
	try{
		gpstk::RinexNavStream InvalidLineLength("Logs/InvalidLineLength.99n");
		gpstk::RinexNavStream NotaNavFile("Logs/NotaNavFile.99n");
		gpstk::RinexNavStream UnknownHeaderLabel("Logs/UnknownHeaderLabel.99n");
		gpstk::RinexNavStream IncompleteHeader("Logs/IncompleteHeader.99n");
		gpstk::RinexNavStream UnsupportedRinex("Logs/UnsupportedRinex.99n");
		gpstk::RinexNavStream BadHeader("Logs/BadHeader.99n");
		gpstk::RinexNavStream out("Logs/TestOutputHeader.99n",ios::out);
		gpstk::RinexNavHeader Header;

		InvalidLineLength.exceptions(fstream::failbit);
		NotaNavFile.exceptions(fstream::failbit);
		UnknownHeaderLabel.exceptions(fstream::failbit);
		IncompleteHeader.exceptions(fstream::failbit);
		UnsupportedRinex.exceptions(fstream::failbit);
		BadHeader.exceptions(fstream::failbit);

		CPPUNIT_ASSERT_THROW(InvalidLineLength >> Header,gpstk::Exception);
		CPPUNIT_ASSERT_THROW(NotaNavFile >> Header,gpstk::Exception);
		CPPUNIT_ASSERT_THROW(UnknownHeaderLabel >> Header,gpstk::Exception);
		CPPUNIT_ASSERT_THROW(IncompleteHeader >> Header,gpstk::Exception);
		CPPUNIT_ASSERT_THROW(UnsupportedRinex >> Header,gpstk::Exception);
		CPPUNIT_ASSERT_THROW(BadHeader >> Header,gpstk::Exception);
		
		IncompleteHeader >> Header;
		out << Header;
		
		UnsupportedRinex >> Header;
		out << Header;
	
		Header.dump(out);
		
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}


}

/*
**** Test for RinexNavData methods
*/
void xRinexNav :: dataTest (void)
{
	try
	{
		gpstk::RinexEphemerisStore Store;
		gpstk::DayTime Time(1999,9,2,17,51,44);
		Store.loadFile("Logs/RinexNavExample.99n");
		const gpstk::EngEphemeris& Eph6 = Store.findUserEphemeris((short) 6, Time);
		gpstk::RinexNavData Data(Eph6);
		list<double> NavDataList = Data.toList();
	
	}
	catch (gpstk::Exception& e)
	{
		cout << e;
	}

}
/*
**** test for several of the members within RinexNavFilterOperators including merge, EqualsFull, LessThanSimple
**** LessThanFull, and FilterPRN
*/
void xRinexNav :: filterOperatorsTest (void)
{
	try
	{
		
		
		gpstk::RinexNavStream FilterStream1("Logs/FilterTest1.99n");
		FilterStream1.open("Logs/FilterTest1.99n",std::ios::in);
		gpstk::RinexNavStream FilterStream2("Logs/FilterTest2.99n");
		gpstk::RinexNavStream FilterStream3("Logs/FilterTest3.99n");
		gpstk::RinexNavStream out("Logs/FilterOutput.txt",ios::out);
		
		gpstk::RinexNavHeader FilterHeader1;
		gpstk::RinexNavHeader FilterHeader2;
		gpstk::RinexNavHeader FilterHeader3;

		
		gpstk::RinexNavData FilterData1;
		gpstk::RinexNavData FilterData2;
		gpstk::RinexNavData FilterData3;

		
		
		FilterStream1 >> FilterHeader1;
		FilterStream2 >> FilterHeader2;
		FilterStream3 >> FilterHeader3;

		
		while (FilterStream1 >> FilterData1)
		{
		}
		while (FilterStream2 >> FilterData2)
		{
		}
		while (FilterStream3 >> FilterData3)
		{
		}

		
		gpstk::RinexNavHeaderTouchHeaderMerge merged;
		merged(FilterHeader1);
		merged(FilterHeader2);
		out << merged.theHeader;
		
		gpstk::RinexNavDataOperatorEqualsFull EqualsFull;
		CPPUNIT_ASSERT_EQUAL(true,EqualsFull(FilterData1, FilterData2));
		CPPUNIT_ASSERT_EQUAL(false,EqualsFull(FilterData1, FilterData3));
		
		gpstk::RinexNavDataOperatorLessThanSimple LessThanSimple;
		CPPUNIT_ASSERT_EQUAL(false,LessThanSimple(FilterData1, FilterData2));
		//CPPUNIT_ASSERT_EQUAL(true,LessThanSimple(FilterData1, FilterData3));
		
		gpstk::RinexNavDataOperatorLessThanFull LessThanFull;
		//CPPUNIT_ASSERT_EQUAL(true,LessThanFull(FilterData1, FilterData3));
		//CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData3, FilterData1));
		CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData1, FilterData1));
		
		std::list<long> list;
		list.push_front(6);
		gpstk::RinexNavDataFilterPRN FilterPRN(list);
		CPPUNIT_ASSERT_EQUAL(true,FilterPRN(FilterData3));
		//cout << FilterPRN(FilterData3) << endl;
	}
	catch(gpstk::Exception& e)
	{
	
	}
	

}

/*
**** A helper function for xRinexNav to line by line, check if the two files given are the same.
**** Takes in two file names within double quotes "FILEONE.TXT" "FILETWO.TXT".  Returns true if
**** the files are equal.  Skips the first two lines becasue dates are often writen as the current
**** data and thus very hard to pin down a specific time for.
*/
bool xRinexNav :: fileEqualTest (char* handle1, char* handle2)
{
	bool isEqual = false;
	int counter = 0;
	ifstream File1;
	ifstream File2;
	
	std::string File1Line;
	std::string File2Line;
	
	File1.open(handle1);
	File2.open(handle2);
	getline (File1, File1Line);
	getline (File2, File2Line);
	getline (File1, File1Line);
	getline (File2, File2Line);
	
	while (!File1.eof())
	{
		if (File2.eof()) 
		{
			cout << "ONE" << counter << endl;
			
			return isEqual;
			}
		getline (File1, File1Line);
		getline (File2, File2Line);

		if (File1Line != File2Line)
		{
			cout << "TWO"  << counter << endl;
			cout << File1Line << endl;
			cout << File2Line << endl;
			return isEqual;
		}
	}
	if (!File2.eof()){
	cout << "THREE" << counter  << endl;
		return isEqual;
		}
	else
		return isEqual = true;
}
