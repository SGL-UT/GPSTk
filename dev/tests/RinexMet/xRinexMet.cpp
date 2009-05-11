#pragma ident "$Id$"

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
//============================================================================

#include "xRinexMet.hpp"
#include "StringUtils.hpp"

using namespace gpstk;

using namespace std;
CPPUNIT_TEST_SUITE_REGISTRATION (xRinexMet);

void xRinexMet :: setUp (void)
{
}

/*
**** A test to assure that the bistAsString function works as intended
*/

void xRinexMet :: bitsAsStringTest (void)
{
	
	gpstk::RinexMetHeader RinexMetHeader;
  
	string a = "RINEX VERSION / TYPE";
	string b = "PGM / RUN BY / DATE";
	string c = "COMMENT";
	string d = "MARKER NAME";
	string e = "MARKER NUMBER";
	string f = "# / TYPES OF OBSERV";
	string g = "SENSOR MOD/TYPE/ACC";
	string h = "SENSOR POS XYZ/H";
	string i = "END OF HEADER";
	string z = "*UNKNOWN/INVALID BITS*";
	CPPUNIT_ASSERT_EQUAL(a,RinexMetHeader.bitsAsString(RinexMetHeader.versionValid));
	CPPUNIT_ASSERT_EQUAL(b,RinexMetHeader.bitsAsString(RinexMetHeader.runByValid));
	CPPUNIT_ASSERT_EQUAL(c,RinexMetHeader.bitsAsString(RinexMetHeader.commentValid));
	CPPUNIT_ASSERT_EQUAL(d,RinexMetHeader.bitsAsString(RinexMetHeader.markerNameValid));
	CPPUNIT_ASSERT_EQUAL(e,RinexMetHeader.bitsAsString(RinexMetHeader.markerNumberValid));
	CPPUNIT_ASSERT_EQUAL(f,RinexMetHeader.bitsAsString(RinexMetHeader.obsTypeValid));
	CPPUNIT_ASSERT_EQUAL(g,RinexMetHeader.bitsAsString(RinexMetHeader.sensorTypeValid));
	CPPUNIT_ASSERT_EQUAL(h,RinexMetHeader.bitsAsString(RinexMetHeader.sensorPosValid));
	CPPUNIT_ASSERT_EQUAL(i,RinexMetHeader.bitsAsString(RinexMetHeader.endValid));
	//Defult Case
	CPPUNIT_ASSERT_EQUAL(z,RinexMetHeader.bitsAsString(RinexMetHeader.allValid21)); 
	
}

/*
**** A test to assure that the version validity bits are what we expect them to be
*/
void xRinexMet :: bitStringTest(void)
{
	
	gpstk::RinexMetHeader RinexMetHeader;
	string sep=", ";
	string a = "\"RINEX VERSION / TYPE\"";
	string b = "\"RINEX VERSION / TYPE\", \"PGM / RUN BY / DATE\", \"MARKER NAME\", \"# / TYPES OF OBSERV\", \"SENSOR MOD/TYPE/ACC\", \"SENSOR POS XYZ/H\", \"END OF HEADER\"";
	CPPUNIT_ASSERT_EQUAL(a, RinexMetHeader.bitString(RinexMetHeader.versionValid,'\"',sep));
	CPPUNIT_ASSERT_EQUAL(b, RinexMetHeader.bitString(RinexMetHeader.allValid21,'\"',sep));
	CPPUNIT_ASSERT_EQUAL(b, RinexMetHeader.bitString(RinexMetHeader.allValid20,'\"',sep));
	//CPPUNIT_ASSERT_EQUAL(b, RinexMetHeader.bitString(RinexMetHeader.allValid20,' ',sep));
	
}

/*
**** A small test of the reallyPutRecord member of the RinexMetHeader with a few execptions such as
**** an Unsupported Rinex version (e.g. 3.33) and a Missing Marker Name
*/
void xRinexMet :: reallyPutRecordTest(void)
{
	
	gpstk::RinexMetHeader RinexMetHeader; 
	gpstk::RinexMetStream UnSupRinex("Logs/UnSupRinex.04m");
	gpstk::RinexMetStream MissingMarkerName("Logs/MissingMarkerName");
	gpstk::RinexMetStream output("Logs/ExtraOutput.txt", ios::out);
	output.exceptions(fstream::failbit);
	try
	{
		UnSupRinex >> RinexMetHeader;
		CPPUNIT_ASSERT_THROW(output << RinexMetHeader, gpstk::Exception);
		MissingMarkerName >> RinexMetHeader;
		CPPUNIT_ASSERT_THROW(output << RinexMetHeader, gpstk::Exception);
		
	}
	catch(gpstk::Exception& e)
  	{
 		cout << (e);
  	}	
	
}

// This function reads the RINEX MET header from the given FFStream.
// If an error is encountered in reading form the stream, the stream
// is reset to its original position and its fail-bit is set.
void xRinexMet :: reallyGetRecordTest(void)
{
	
	//Header file we will be testing on
	gpstk::RinexMetHeader RinexMetHeader;
	
	//Normal Met File (No changes, straight from the archive)
	gpstk::RinexMetStream Normal("Logs/408_110a.04m");
	
	//Bad Line Length Met file (Add a space characacter to the end of the first line)
	gpstk::RinexMetStream BLL("Logs/BLL.04m");
	
	//Not a Met File (Change Met to zet in first line)
	gpstk::RinexMetStream NMF("Logs/NotMetFile.04m");
	
	//Bad Obs line (Add in something between the MET obs besides space)
	gpstk::RinexMetStream BOL("Logs/BOL.04m");
	
	//Bad Sesor Type line, Format Error
	gpstk::RinexMetStream FER("Logs/FER.04m");
	
	//Extra header line with HEADERLINE2
	gpstk::RinexMetStream ExtraH("Logs/ExtraH.04m");
	
	//Unsupported Rinex Version (3.30)
	gpstk::RinexMetStream UnSupRinex("Logs/UnSupRinex.04m");
	
	//Missing SensorPos
	gpstk::RinexMetStream MissingSPos("Logs/MissingEOH.04m");
	
	//Error in the part of the header which holds the list of observations within the Met file
	gpstk::RinexMetStream ObsHeaderStringError("Logs/ObsHeaderStringError.04m");
	
	//Error in the sensor type
	gpstk::RinexMetStream SensorType("Logs/SensorTypeError.04m");
	
	Normal.exceptions(fstream::failbit);
	BLL.exceptions(fstream::failbit);
	NMF.exceptions(fstream::failbit);
	BOL.exceptions(fstream::failbit);
	FER.exceptions(fstream::failbit);
	ExtraH.exceptions(fstream::failbit);
	UnSupRinex.exceptions(fstream::failbit);
	MissingSPos.exceptions(fstream::failbit);
	ObsHeaderStringError.exceptions(fstream::failbit);
	SensorType.exceptions(fstream::failbit);
	
	try
	{
		CPPUNIT_ASSERT_THROW(ExtraH >> RinexMetHeader, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(UnSupRinex >> RinexMetHeader, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(MissingSPos >> RinexMetHeader, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(ObsHeaderStringError >> RinexMetHeader, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(SensorType >> RinexMetHeader, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(BLL >> RinexMetHeader, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(NMF >> RinexMetHeader, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(BOL >> RinexMetHeader, gpstk::Exception);
	
		CPPUNIT_ASSERT_NO_THROW(Normal >> RinexMetHeader);
	}
	catch(gpstk::Exception& e)
  	{
 		//cout << e;
  	}	
	
}

/*
**** This test tests the convertObsType member of RinexMetHeader
**** This particular test asserts if two Met Types are equal
*/
void xRinexMet :: convertObsTypeSTRTest (void)
{
	
	gpstk::RinexMetHeader RinexMetHeader;
	gpstk::RinexMetStream Normal("Logs/408_110a.04m");
	Normal >> RinexMetHeader;
	gpstk::RinexMetHeader::RinexMetType PR = RinexMetHeader.convertObsType("PR");
	gpstk::RinexMetHeader::RinexMetType TD = RinexMetHeader.convertObsType("TD");
	gpstk::RinexMetHeader::RinexMetType HR = RinexMetHeader.convertObsType("HR");
	gpstk::RinexMetHeader::RinexMetType ZW = RinexMetHeader.convertObsType("ZW");
	gpstk::RinexMetHeader::RinexMetType ZD = RinexMetHeader.convertObsType("ZD");
	gpstk::RinexMetHeader::RinexMetType ZT = RinexMetHeader.convertObsType("ZT");
	gpstk::RinexMetHeader::RinexMetType HI = RinexMetHeader.convertObsType("HI");
	gpstk::RinexMetHeader::RinexMetType KE = RinexMetHeader.convertObsType("HI");
	
	string PRS = "PR";
	string TDS = "TD";
	string HRS = "HR";
	string ZWS = "ZW";
	string ZDS = "ZD";
	string ZTS = "ZT";
	string HIS = "HI";
	CPPUNIT_ASSERT_EQUAL(PR,RinexMetHeader.convertObsType(PRS));
	CPPUNIT_ASSERT_EQUAL(TD,RinexMetHeader.convertObsType(TDS));
	CPPUNIT_ASSERT_EQUAL(HR,RinexMetHeader.convertObsType(HRS));
	CPPUNIT_ASSERT_EQUAL(ZW,RinexMetHeader.convertObsType(ZWS));
	CPPUNIT_ASSERT_EQUAL(ZD,RinexMetHeader.convertObsType(ZDS));
	CPPUNIT_ASSERT_EQUAL(ZT,RinexMetHeader.convertObsType(ZTS));
	CPPUNIT_ASSERT_EQUAL(HI,RinexMetHeader.convertObsType(HIS));
	
}

/*
**** This test tests the convertObsType member of RinexMetHeader
**** This particular test asserts if two strings are equal
*/
void xRinexMet :: convertObsTypeHeaderTest (void)
{
	
	gpstk::RinexMetHeader RinexMetHeader;
	gpstk::RinexMetStream Normal("Logs/408_110a.04m");
	Normal >> RinexMetHeader;
	gpstk::RinexMetHeader::RinexMetType PR = RinexMetHeader.convertObsType("PR");
	gpstk::RinexMetHeader::RinexMetType TD = RinexMetHeader.convertObsType("TD");
	gpstk::RinexMetHeader::RinexMetType HR = RinexMetHeader.convertObsType("HR");
	gpstk::RinexMetHeader::RinexMetType ZW = RinexMetHeader.convertObsType("ZW");
	gpstk::RinexMetHeader::RinexMetType ZD = RinexMetHeader.convertObsType("ZD");
	gpstk::RinexMetHeader::RinexMetType ZT = RinexMetHeader.convertObsType("ZT");
	gpstk::RinexMetHeader::RinexMetType KE = RinexMetHeader.convertObsType("ZT");
	string PRS = "PR";
	string TDS = "TD";
	string HRS = "HR";
	string ZWS = "ZW";
	string ZDS = "ZD";
	string ZTS = "ZT";
	CPPUNIT_ASSERT_EQUAL(PRS,RinexMetHeader.convertObsType(PR));
	CPPUNIT_ASSERT_EQUAL(TDS,RinexMetHeader.convertObsType(TD));
	CPPUNIT_ASSERT_EQUAL(HRS,RinexMetHeader.convertObsType(HR));
	CPPUNIT_ASSERT_EQUAL(ZWS,RinexMetHeader.convertObsType(ZW));
	CPPUNIT_ASSERT_EQUAL(ZDS,RinexMetHeader.convertObsType(ZD));
	CPPUNIT_ASSERT_EQUAL(ZTS,RinexMetHeader.convertObsType(ZT));
	try
	{
		CPPUNIT_ASSERT_THROW(RinexMetHeader.convertObsType("KE"),gpstk::FFStreamError);
	}
	catch (gpstk::Exception& e)
	{
	}
	
}

/*
**** This test checks to make sure that the internal members of the RinexMetHeader are as we think they
**** should be.  Also at the end of this test, we check and make sure our output file is equal to our input
*/
void xRinexMet :: hardCodeTest (void)
{
	
	gpstk::RinexMetStream RinexMetStream("Logs/408_110a.04m");
	gpstk::RinexMetStream out("Logs/Output.txt",ios::out);
	gpstk::RinexMetHeader RinexMetHeader;
	RinexMetStream >> RinexMetHeader;
	
	//Start of RinexMetHeader member check to assure that what we want is whats in there
	CPPUNIT_ASSERT_EQUAL(2.1,RinexMetHeader.version);
	CPPUNIT_ASSERT_EQUAL((string)"Meteorological",RinexMetHeader.fileType);
	CPPUNIT_ASSERT_EQUAL((string)"GFW - RMW",RinexMetHeader.fileProgram);
	CPPUNIT_ASSERT_EQUAL((string)"NIMA", RinexMetHeader.fileAgency);
	CPPUNIT_ASSERT_EQUAL((string)"04/18/2004 23:58:50",RinexMetHeader.date);
	
	vector<string>::const_iterator itr1 = RinexMetHeader.commentList.begin();
	while (itr1 != RinexMetHeader.commentList.end())
	{
		CPPUNIT_ASSERT_EQUAL((string)"Some weather data may have corrected values",(*itr1));
		itr1++;
	}
	
	CPPUNIT_ASSERT_EQUAL((string)"85408", RinexMetHeader.markerName);
	CPPUNIT_ASSERT_EQUAL((string)"85408", RinexMetHeader.markerNumber);
	
	vector<RinexMetHeader::RinexMetType>::const_iterator itr2 = RinexMetHeader.obsTypeList.begin();
	CPPUNIT_ASSERT_EQUAL((string)"PR",RinexMetHeader.convertObsType(*itr2));
	itr2++;
	CPPUNIT_ASSERT_EQUAL((string)"TD",RinexMetHeader.convertObsType(*itr2));
	itr2++;
	CPPUNIT_ASSERT_EQUAL((string)"HI",RinexMetHeader.convertObsType(*itr2));	
	
	vector<RinexMetHeader::sensorType>::const_iterator itr3 = RinexMetHeader.sensorTypeList.begin();
	CPPUNIT_ASSERT_EQUAL((string)"Vaisala",(*itr3).model);
	CPPUNIT_ASSERT_EQUAL((string)"PTB220",(*itr3).type);
	CPPUNIT_ASSERT_EQUAL(0.1, (*itr3).accuracy);
	CPPUNIT_ASSERT_EQUAL((string)"PR",RinexMetHeader.convertObsType((*itr3).obsType));
	itr3++;
		
	CPPUNIT_ASSERT_EQUAL((string)"Vaisala",(*itr3).model);
	CPPUNIT_ASSERT_EQUAL((string)"HMP230",(*itr3).type);
	CPPUNIT_ASSERT_EQUAL(0.1, (*itr3).accuracy);
	CPPUNIT_ASSERT_EQUAL((string)"TD",RinexMetHeader.convertObsType((*itr3).obsType));
	itr3++;
	CPPUNIT_ASSERT_EQUAL((string)"Vaisala",(*itr3).model);
	CPPUNIT_ASSERT_EQUAL((string)"HMP230",(*itr3).type);
	CPPUNIT_ASSERT_EQUAL(0.1, (*itr3).accuracy);
	CPPUNIT_ASSERT_EQUAL((string)"HI",RinexMetHeader.convertObsType((*itr3).obsType));
	itr3++;
			
	vector<RinexMetHeader::sensorPosType>::const_iterator itr4 = RinexMetHeader.sensorPosList.begin();
	CPPUNIT_ASSERT_EQUAL(-740289.8363,(*itr4).position[0]);
	CPPUNIT_ASSERT_EQUAL(-5457071.7414,(*itr4).position[1]);
	CPPUNIT_ASSERT_EQUAL(3207245.6207,(*itr4).position[2]);
	CPPUNIT_ASSERT_EQUAL(0.0000,(*itr4).height);
	CPPUNIT_ASSERT_EQUAL((string) "PR",RinexMetHeader.convertObsType((*itr4).obsType));
	itr4++;
	CPPUNIT_ASSERT_EQUAL(-740289.8363,(*itr4).position[0]);
	CPPUNIT_ASSERT_EQUAL(-5457071.7414,(*itr4).position[1]);
	CPPUNIT_ASSERT_EQUAL(3207245.6207,(*itr4).position[2]);
	CPPUNIT_ASSERT_EQUAL(0.0000,(*itr4).height);
	CPPUNIT_ASSERT_EQUAL((string) "TD",RinexMetHeader.convertObsType((*itr4).obsType));
	//End of Header
	
	out << RinexMetHeader;
	
	gpstk::RinexMetData RinexMetData;

	RinexMetStream >> RinexMetData;
	gpstk::DayTime TimeGuess(2004,4,19,0,0,0);
	CPPUNIT_ASSERT_EQUAL(TimeGuess,RinexMetData.time);
	CPPUNIT_ASSERT_EQUAL(992.6,RinexMetData.data[RinexMetHeader.convertObsType("PR")]);
	CPPUNIT_ASSERT_EQUAL(23.9,RinexMetData.data[RinexMetHeader.convertObsType("TD")]);
	CPPUNIT_ASSERT_EQUAL(59.7,RinexMetData.data[RinexMetHeader.convertObsType("HI")]);
	out << RinexMetData;
	RinexMetStream >> RinexMetData;
	gpstk::DayTime TimeGuess2(2004,4,19,0,15,0);
	CPPUNIT_ASSERT_EQUAL(TimeGuess2,RinexMetData.time);
	CPPUNIT_ASSERT_EQUAL(992.8,RinexMetData.data[RinexMetHeader.convertObsType("PR")]);
	CPPUNIT_ASSERT_EQUAL(23.6,RinexMetData.data[RinexMetHeader.convertObsType("TD")]);
	CPPUNIT_ASSERT_EQUAL(61.6,RinexMetData.data[RinexMetHeader.convertObsType("HI")]);
	out << RinexMetData;
	while (RinexMetStream >> RinexMetData)
	{
		out << RinexMetData;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/408_110a.04m","Logs/Output.txt"));
	
	gpstk::RinexMetStream MetDumps("Logs/MetDumps",ios::out);
	RinexMetHeader.dump(MetDumps);
	RinexMetData.dump(MetDumps);
}

/*
**** This test covers are of the RinexMetHeader and Data which deal with continuation lines
**** for the observations
*/
void xRinexMet :: continuationTest (void)
{

		gpstk::RinexMetStream RinexMetStream("Logs/ContLines10.04m");
		gpstk::RinexMetStream out("Logs/OutputCont.txt",ios::out);
		gpstk::RinexMetStream MetDumps("Logs/MetDumps",ios::out);
		gpstk::RinexMetHeader RinexMetHeader;
		gpstk::RinexMetData RinexMetData;
	try
	{
		RinexMetStream >> RinexMetHeader;
		out << RinexMetHeader;
	
		RinexMetHeader.dump(MetDumps);
		RinexMetData.dump(MetDumps);
		RinexMetStream >> RinexMetData;
		RinexMetData.dump(MetDumps);
		out << RinexMetData;
		while (RinexMetStream >> RinexMetData)
		{
			out << RinexMetData;
		}
		
		CPPUNIT_ASSERT(fileEqualTest("Logs/ContLines10.04m","Logs/OutputCont.txt"));
	}
	catch (gpstk::Exception& e)
	{
		cout << e;
	}
}

/*
**** This test coveres several RinexMetData exceptions including a test with no observations data for the
**** listed header Obs and an invalid time format
*/
void xRinexMet :: dataExceptionsTest (void)
{
		gpstk::RinexMetStream NoObs("Logs/NoObsData.04m");
		gpstk::RinexMetStream InvalidTimeFormat("Logs/InvalidTimeFormat.04m");
		gpstk::RinexMetStream out("Logs/OutputDataExceptions.txt",ios::out);
		gpstk::RinexMetHeader rmh;
		gpstk::RinexMetData rme;
	try{
		NoObs >> rmh;
		out << rmh;
		
		while (NoObs >> rme)
		{
			out << rme;
		}
		InvalidTimeFormat >> rme;
		out << rme;
		InvalidTimeFormat >> rme;
		out << rme;
	}
	catch (gpstk::Exception& e)
	{
		cout << e;
	}
}

/*
**** Test for several of the members within RinexMetFilterOperators including merge, LessThanSimple, EqualsSimple,
**** and LessThanFull.
*/
void xRinexMet :: filterOperatorsTest (void)
{
	try
	{
		
		
		gpstk::RinexMetStream FilterStream1("Logs/FilterTest1.04m");
		FilterStream1.open("Logs/FilterTest1.04m",std::ios::in);
		gpstk::RinexMetStream FilterStream2("Logs/FilterTest2.04m");
		gpstk::RinexMetStream FilterStream3("Logs/FilterTest3.04m");
		gpstk::RinexMetStream FilterStream4("Logs/FilterTest4.04m");
		gpstk::RinexMetStream out("Logs/FilterOutput.txt",ios::out);
		
		gpstk::RinexMetHeader FilterHeader1;
		gpstk::RinexMetHeader FilterHeader2;
		gpstk::RinexMetHeader FilterHeader3;
		gpstk::RinexMetHeader FilterHeader4;
		
		gpstk::RinexMetData FilterData1;
		gpstk::RinexMetData FilterData2;
		gpstk::RinexMetData FilterData3;
		gpstk::RinexMetData FilterData4;
		
		
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
		
		gpstk::RinexMetHeaderTouchHeaderMerge merged;
		merged(FilterHeader1);
		merged(FilterHeader2);
		gpstk::RinexMetDataOperatorLessThanFull(merged.obsSet);
		out << merged.theHeader;
		
		gpstk::RinexMetDataOperatorEqualsSimple EqualsSimple;
		CPPUNIT_ASSERT_EQUAL(true,EqualsSimple(FilterData1, FilterData2));
		CPPUNIT_ASSERT_EQUAL(false,EqualsSimple(FilterData1, FilterData3));
		
		gpstk::RinexMetDataOperatorLessThanSimple LessThanSimple;
		CPPUNIT_ASSERT_EQUAL(false,LessThanSimple(FilterData1, FilterData2));
		CPPUNIT_ASSERT_EQUAL(true,LessThanSimple(FilterData1, FilterData3));
		
		gpstk::RinexMetDataOperatorLessThanFull LessThanFull(merged.obsSet);
		CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData1, FilterData2));
		CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData2, FilterData1));
		CPPUNIT_ASSERT_EQUAL(true,LessThanFull(FilterData1, FilterData3));
		CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData3, FilterData1));
		
		gpstk::DayTime Start = gpstk::DayTime::END_OF_TIME;
		gpstk::DayTime End = gpstk::DayTime::BEGINNING_OF_TIME;
		gpstk::DayTime Start2 = gpstk::DayTime::BEGINNING_OF_TIME;
		gpstk::DayTime End2 = gpstk::DayTime::END_OF_TIME;
		gpstk::RinexMetDataFilterTime FilterTime(Start,End);
		gpstk::RinexMetDataFilterTime FilterTime2(Start2,End2);
		CPPUNIT_ASSERT_EQUAL(true,FilterTime(FilterData1));
		CPPUNIT_ASSERT_EQUAL(false,FilterTime2(FilterData1));
	}
	catch(gpstk::Exception& e)
	{
	
	}
	

}

/*
**** A helper function for xRinexMet to line by line, check if the two files given are the same.
**** Takes in two file names within double quotes "FILEONE.TXT" "FILETWO.TXT".  Returns true if
**** the files are equal.  Skips the first two lines becasue dates are often writen as the current
**** data and thus very hard to pin down a specific time for.
*/
bool xRinexMet :: fileEqualTest (char* handle1, char* handle2)
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
