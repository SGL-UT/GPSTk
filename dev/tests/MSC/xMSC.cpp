#pragma ident "$Id$"
#include "xMSC.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xMSC);

using namespace gpstk;

void xMSC :: setUp (void)
{
}

void xMSC :: firstTest (void)
{
	MSCStream Input("Logs/CoordFile");
	MSCStream Out("Logs/Output",ios::out);
	MSCData Data;
	
	Input >> Data;
	gpstk::DayTime Time(2001,360,0.0);
	CPPUNIT_ASSERT_EQUAL(Time,Data.time);
	CPPUNIT_ASSERT_EQUAL((long unsigned) 11111,Data.station);
	CPPUNIT_ASSERT_EQUAL((string)"AAA    " ,Data.mnemonic);
	
	Out << Data;
	while (Input >> Data)
	{
		Out << Data;
	}
	
	MSCStream Input2("Logs/Output");
	MSCStream Out2("Logs/Output2",ios::out);
	MSCData Data2;
	while (Input2 >> Data2)
	{
		Out2 << Data2;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/CoordFile","Logs/Output"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/CoordFile","Logs/Output2"));
}

bool xMSC :: fileEqualTest (char* handle1, char* handle2)
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
