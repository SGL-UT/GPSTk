#include "xRinexObs.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xRinexObs);

using namespace gpstk;

void xRinexObs :: setUp (void)
{
}
/*
**** This tests throws many GPSTK RinexObsHeader exceptions including Incomplete headers, invalid line lengths etc
**** Also an extended obs type is used and dumped within this test.
*/
void xRinexObs :: headerExceptionTest (void)
{
	try{
		gpstk::RinexObsStream RinexObsFile("Logs/RinexObsFile.06o");
		gpstk::RinexObsStream ih("Logs/IncompleteHeader.06o");
		gpstk::RinexObsStream il("Logs/InvalidLineLength.06o");
		gpstk::RinexObsStream inpwf("Logs/InvalidNumPRNWaveFact.06o");
		gpstk::RinexObsStream no("Logs/NotObs.06o");
		gpstk::RinexObsStream ss("Logs/SystemGeosync.06o");
		gpstk::RinexObsStream sr("Logs/SystemGlonass.06o");
		gpstk::RinexObsStream sm("Logs/SystemMixed.06o");
		gpstk::RinexObsStream st("Logs/SystemTransit.06o");
		gpstk::RinexObsStream unsupv("Logs/UnSupVersion.06o");
		gpstk::RinexObsStream contdata("Logs/RinexContData.06o");
		
		gpstk::RinexObsStream out("Logs/TestOutput.06n",ios::out);
		gpstk::RinexObsStream out2("Logs/TestOutput3.06n",ios::out);
		gpstk::RinexObsStream dump("Logs/ObsDump",ios::out);
		
		gpstk::RinexObsHeader RinexObsFileh;
		gpstk::RinexObsHeader ihh;
		gpstk::RinexObsHeader ilh;
		gpstk::RinexObsHeader inpwfh;
		gpstk::RinexObsHeader noh;
		gpstk::RinexObsHeader ssh;
		gpstk::RinexObsHeader srh;
		gpstk::RinexObsHeader smh;
		gpstk::RinexObsHeader sth;
		gpstk::RinexObsHeader unsupvh;
		gpstk::RinexObsHeader contdatah;
		
		gpstk::RegisterExtendedRinexObsType("ER","Testing Type", "Candela", (unsigned) 2);
		gpstk::RinexObsData RinexObsFiled;
		gpstk::RinexObsData contdatad;
				
		RinexObsFile >> RinexObsFileh;
		ih >> ihh;
		il >> ilh;
		inpwf >> inpwfh;
		no >> noh;
		ss >> ssh;
		sr >> srh;
		sm >> smh;
		unsupv >> unsupvh;
		contdata >> contdatah;
		
		out << RinexObsFileh;
		out << ihh;
		out << ilh;
		out << inpwfh;
		out << noh;
		out << ssh;
		out << srh;
		out << smh;
		out << unsupvh;
		out2 << contdatah;
		RinexObsFile >> RinexObsFiled;
		RinexObsFiled.dump(dump);
		while (RinexObsFile >> RinexObsFiled)
		{
			out << RinexObsFiled;
		}
		while (contdata >> contdatad)
		{
			out2 << contdatad;
		}
		RinexObsFileh.dump(dump);
		contdatah.dump(dump);
		ilh.dump(dump);
		gpstk::DisplayExtendedRinexObsTypes(dump);
		//RinexObsFiled.dump(cout);
		CPPUNIT_ASSERT_EQUAL(39,RinexObsFileh.NumberHeaderRecordsToBeWritten());
	}
	catch (gpstk::Exception& e)
	{
		cout << e;
	}
}

/*
**** This test checks to make sure that the output from a read in RinexObsFile matches the input.
*/
void xRinexObs :: hardCodeTest (void)
{
	gpstk::RinexObsStream RinexObsFile("Logs/RinexObsFile.06o");
	gpstk::RinexObsStream out("Logs/TestOutput2.06n",ios::out);
	gpstk::RinexObsStream dump("Logs/ObsDump",ios::out);
	gpstk::RinexObsHeader RinexObsFileh;	
	gpstk::RinexObsData RinexObsFiled;			
	RinexObsFile >> RinexObsFileh;	
	out << RinexObsFileh;
	while (RinexObsFile >> RinexObsFiled)
	{
		out << RinexObsFiled;
	}
	RinexObsFiled.dump(dump);
	RinexObsFileh.dump(dump);
	CPPUNIT_ASSERT(fileEqualTest("Logs/RinexObsFile.06o","Logs/TestOutput2.06n"));
}

/*
**** This test throws many GPSTK exceptions within the RinexObsData including BadEpochLine and BadEpochFlag
*/
void xRinexObs :: dataExceptionsTest (void)
{	
	gpstk::RinexObsStream BadEpochLine("Logs/BadEpochLine.06o");
	gpstk::RinexObsStream BadEpochFlag("Logs/BadEpochFlag.06o");
	gpstk::RinexObsStream BadLineSize("Logs/BadLineSize.06o");
	gpstk::RinexObsStream InvalidTimeFormat("Logs/InvalidTimeFormat.06o");
	gpstk::RinexObsStream out("Logs/DataExceptionOutput.06o",ios::out);
	gpstk::RinexObsData BadEpochLined;
	gpstk::RinexObsData BadEpochFlagd;
	gpstk::RinexObsData BadLineSized;
	gpstk::RinexObsData InvalidTimeFormatd;	
	while (BadEpochLine >> BadEpochLined)
	{
		out << BadEpochLined;
	}
	while (BadEpochFlag >> BadEpochFlagd)
	{
		out << BadEpochFlagd;
	}
	while (BadLineSize >> BadLineSized)
	{
		out << BadLineSized;
	}
	while (InvalidTimeFormat >> InvalidTimeFormatd)
	{
		out << InvalidTimeFormatd;
	}

}

/*
**** This is the test for several of the members within RinexObsFilterOperators including merge, LessThanSimple,
**** EqualsSimple, and LessThanFull

**** This is still under development
*/
void xRinexObs :: filterOperatorsTest (void)
{
	try
	{
		
		
		gpstk::RinexObsStream FilterStream1("Logs/FilterTest1.06o");
		FilterStream1.open("Logs/FilterTest1.06o",std::ios::in);
		gpstk::RinexObsStream FilterStream2("Logs/FilterTest2.06o");
		gpstk::RinexObsStream FilterStream3("Logs/FilterTest3.06o");
		gpstk::RinexObsStream FilterStream4("Logs/FilterTest4.06o");
		gpstk::RinexObsStream out("Logs/FilterOutput.txt",ios::out);
		
		gpstk::RinexObsHeader FilterHeader1;
		gpstk::RinexObsHeader FilterHeader2;
		gpstk::RinexObsHeader FilterHeader3;
		gpstk::RinexObsHeader FilterHeader4;
		
		gpstk::RinexObsData FilterData1;
		gpstk::RinexObsData FilterData2;
		gpstk::RinexObsData FilterData3;
		gpstk::RinexObsData FilterData4;
		
		
		FilterStream1 >> FilterHeader1;
		FilterStream2 >> FilterHeader2;
		FilterStream3 >> FilterHeader3;
		FilterStream4 >> FilterHeader4;
		
		while (FilterStream1 >> FilterData1)
		{
		}
		while (FilterStream2 >> FilterData2)
		{
		}
		while (FilterStream3 >> FilterData3)
		{
		}
		while (FilterStream4 >> FilterData4)
		{
		}
		
		gpstk::RinexObsHeaderTouchHeaderMerge merged;
		merged(FilterHeader1);
		merged(FilterHeader2);
		gpstk::RinexObsDataOperatorLessThanFull(merged.obsSet);
		out << merged.theHeader;
		
		gpstk::RinexObsDataOperatorEqualsSimple EqualsSimple;
		CPPUNIT_ASSERT_EQUAL(true,EqualsSimple(FilterData1, FilterData1));
		//CPPUNIT_ASSERT_EQUAL(false,EqualsSimple(FilterData1, FilterData3));
		
		gpstk::RinexObsDataOperatorLessThanSimple LessThanSimple;
		CPPUNIT_ASSERT_EQUAL(false,LessThanSimple(FilterData1, FilterData1));
		//CPPUNIT_ASSERT_EQUAL(true,LessThanSimple(FilterData3, FilterData1));
		
		gpstk::RinexObsDataOperatorLessThanFull LessThanFull(merged.obsSet);
		CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData1, FilterData1));
		CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData1, FilterData2));
		//CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData1, FilterData1));
		//CPPUNIT_ASSERT_EQUAL(true,LessThanFull(FilterData4, FilterData2));

	}
	catch(gpstk::Exception& e)
	{
	
	}
	

}

/*
**** A helper function for xRinexObs to line by line, check if the two files given are the same.
**** Takes in two file names within double quotes "FILEONE.TXT" "FILETWO.TXT".  Returns true if
**** the files are equal.  Skips the first two lines becasue dates are often writen as the current
**** data and thus very hard to pin down a specific time for.
*/
bool xRinexObs :: fileEqualTest (char* handle1, char* handle2)
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
