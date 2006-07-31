#include "xRinexMetHeader.hpp"
#include "RinexMetStream.hpp"
#include "Exception.hpp"
using namespace std;
using namespace gpstk;

CPPUNIT_TEST_SUITE_REGISTRATION (rMetHeaderTest);

void rMetHeaderTest :: setUp (void)
{

}

void rMetHeaderTest :: tearDown (void) 
{

}

//Test to make sure the validity bit to string conversion is working as expected
void rMetHeaderTest :: bitsAsStringTest (void)
{
	gpstk::RinexMetHeader roh;
  
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
	CPPUNIT_ASSERT_EQUAL(a,roh.bitsAsString(roh.versionValid));
	CPPUNIT_ASSERT_EQUAL(b,roh.bitsAsString(roh.runByValid));
	CPPUNIT_ASSERT_EQUAL(c,roh.bitsAsString(roh.commentValid));
	CPPUNIT_ASSERT_EQUAL(d,roh.bitsAsString(roh.markerNameValid));
	CPPUNIT_ASSERT_EQUAL(e,roh.bitsAsString(roh.markerNumberValid));
	CPPUNIT_ASSERT_EQUAL(f,roh.bitsAsString(roh.obsTypeValid));
	CPPUNIT_ASSERT_EQUAL(g,roh.bitsAsString(roh.sensorTypeValid));
	CPPUNIT_ASSERT_EQUAL(h,roh.bitsAsString(roh.sensorPosValid));
	CPPUNIT_ASSERT_EQUAL(i,roh.bitsAsString(roh.endValid));
	//Defult Case
	CPPUNIT_ASSERT_EQUAL(z,roh.bitsAsString(roh.allValid21));  
}

//Test to make sure the validity bit to formated string is working as expected
void rMetHeaderTest :: bitStringTest(void)
{
	gpstk::RinexMetHeader roh;
	string sep=", ";
	string a = "\"RINEX VERSION / TYPE\"";
	string b = "\"RINEX VERSION / TYPE\", \"PGM / RUN BY / DATE\", \"MARKER NAME\", \"# / TYPES OF OBSERV\", \"SENSOR MOD/TYPE/ACC\", \"SENSOR POS XYZ/H\", \"END OF HEADER\"";
	CPPUNIT_ASSERT_EQUAL(a, roh.bitString(roh.versionValid,'\"',sep));
	CPPUNIT_ASSERT_EQUAL(b, roh.bitString(roh.allValid21,'\"',sep));
	CPPUNIT_ASSERT_EQUAL(b, roh.bitString(roh.allValid20,'\"',sep));
}

//Writes the RINEX Met header to the stream
void rMetHeaderTest :: reallyPutRecordTest(void)
{
	gpstk::RinexMetHeader roh; 
	gpstk::RinexMetStream MissingSPos("/home/ehagen/Desktop/zimm0070.05m");
	gpstk::RinexMetStream UnSupRinex("Logs/UnSupRinex.00m");
	gpstk::RinexMetStream output("Logs/Output.txt", ios::out);
	//MissingSPos.exceptions(fstream::failbit);
	output.exceptions(fstream::failbit);
	try
	{
		MissingSPos >> roh;
		CPPUNIT_ASSERT_THROW(output << roh, gpstk::Exception);
		UnSupRinex >> roh;
		CPPUNIT_ASSERT_THROW(output << roh, gpstk::Exception);
		
	}
	catch(gpstk::Exception& e)
  	{
 		cout << (e);
  	}	
}

// This function reads the RINEX MET header from the given FFStream.
// If an error is encountere in reading form the stream, the stream
// is reset to its original position and its fail-bit is set.
void rMetHeaderTest :: reallyGetRecordTest(void)
{
	//Header file we will be testing on
	gpstk::RinexMetHeader roh;
	
	//Normal Met File (No changes, straight from the archive)
	gpstk::RinexMetStream Normal("Logs/401.303a.00m");
	//Bad Line Length Met file (Add a space characacter to the end of the first line)
	gpstk::RinexMetStream BLL("Logs/BLL.00m");
	//Not a Met File (Change Met to zet in first line)
	gpstk::RinexMetStream NMF("Logs/NotMetFile.00m");
	//Bad Obs line (Add in something between the MET obs besides space)
	gpstk::RinexMetStream BOL("Logs/BOL.00m");
	
	//Bad Obs line on a continuation lnie (add in something between the MET obs besides space)
	//Can't get this one to work yet!
	//gpstk::RinexMetStream BOL2("BOL2.00m");
	
	//Bad Sesor Type line, Format Error
	gpstk::RinexMetStream FER("Logs/FER.00m");
	//Extra header line with HEADERLINE2
	gpstk::RinexMetStream ExtraH("Logs/ExtraH.00m");
	//Unsupported Rinex Version (3.77)
	gpstk::RinexMetStream UnSupRinex("Logs/UnSupRinex.00m");
	//Missing SensorPos
	gpstk::RinexMetStream MissingSPos("Logs/MissingEOH.00m");
	
	Normal.exceptions(fstream::failbit);
	BLL.exceptions(fstream::failbit);
	NMF.exceptions(fstream::failbit);
	BOL.exceptions(fstream::failbit);
	FER.exceptions(fstream::failbit);
	ExtraH.exceptions(fstream::failbit);
	UnSupRinex.exceptions(fstream::failbit);
	MissingSPos.exceptions(fstream::failbit);
	//BOL2.exceptions(fstream::failbit);
	
	try
	{
		//MissingSPos >> roh;
		CPPUNIT_ASSERT_THROW(BLL >> roh, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(NMF >> roh, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(BOL >> roh, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(FER >> roh, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(ExtraH >> roh, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(UnSupRinex >> roh, gpstk::Exception);
		CPPUNIT_ASSERT_THROW(MissingSPos >> roh, gpstk::Exception);
		//CPPUNIT_ASSERT_THROW(BOL2 >> roh, gpstk::Exception);
		CPPUNIT_ASSERT_NO_THROW(Normal >> roh);
	}
	catch(gpstk::Exception& e)
  	{
 		cout << e;
  	}	
}

// Not much to test here it pretty much just couts information to the screen
// for debugging purposes...
void rMetHeaderTest :: dumpTest (void)
{

}
void rMetHeaderTest :: convertObsTypeSTRTest (void)
{
	gpstk::RinexMetHeader roh;
	gpstk::RinexMetStream Normal("Logs/401.303a.00m");
	Normal >> roh;
	gpstk::RinexMetHeader::RinexMetType PR = roh.convertObsType("PR");
	gpstk::RinexMetHeader::RinexMetType TD = roh.convertObsType("TD");
	gpstk::RinexMetHeader::RinexMetType HR = roh.convertObsType("HR");
	gpstk::RinexMetHeader::RinexMetType ZW = roh.convertObsType("ZW");
	gpstk::RinexMetHeader::RinexMetType ZD = roh.convertObsType("ZD");
	gpstk::RinexMetHeader::RinexMetType ZT = roh.convertObsType("ZT");
	string PRS = "PR";
	string TDS = "TD";
	string HRS = "HR";
	string ZWS = "ZW";
	string ZDS = "ZD";
	string ZTS = "ZT";
	CPPUNIT_ASSERT_EQUAL(PR,roh.convertObsType(PRS));
	CPPUNIT_ASSERT_EQUAL(TD,roh.convertObsType(TDS));
	CPPUNIT_ASSERT_EQUAL(HR,roh.convertObsType(HRS));
	CPPUNIT_ASSERT_EQUAL(ZW,roh.convertObsType(ZWS));
	CPPUNIT_ASSERT_EQUAL(ZD,roh.convertObsType(ZDS));
	CPPUNIT_ASSERT_EQUAL(ZT,roh.convertObsType(ZTS));
}
void rMetHeaderTest :: convertObsTypeHeaderTest (void)
{
	gpstk::RinexMetHeader roh;
	gpstk::RinexMetStream Normal("Logs/401.303a.00m");
	Normal >> roh;
	gpstk::RinexMetHeader::RinexMetType PR = roh.convertObsType("PR");
	gpstk::RinexMetHeader::RinexMetType TD = roh.convertObsType("TD");
	gpstk::RinexMetHeader::RinexMetType HR = roh.convertObsType("HR");
	gpstk::RinexMetHeader::RinexMetType ZW = roh.convertObsType("ZW");
	gpstk::RinexMetHeader::RinexMetType ZD = roh.convertObsType("ZD");
	gpstk::RinexMetHeader::RinexMetType ZT = roh.convertObsType("ZT");
	string PRS = "PR";
	string TDS = "TD";
	string HRS = "HR";
	string ZWS = "ZW";
	string ZDS = "ZD";
	string ZTS = "ZT";
	CPPUNIT_ASSERT_EQUAL(PRS,roh.convertObsType(PR));
	CPPUNIT_ASSERT_EQUAL(TDS,roh.convertObsType(TD));
	CPPUNIT_ASSERT_EQUAL(HRS,roh.convertObsType(HR));
	CPPUNIT_ASSERT_EQUAL(ZWS,roh.convertObsType(ZW));
	CPPUNIT_ASSERT_EQUAL(ZDS,roh.convertObsType(ZD));
	CPPUNIT_ASSERT_EQUAL(ZTS,roh.convertObsType(ZT));
}
