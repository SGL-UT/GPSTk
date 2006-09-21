#include "xGPSEpochWeekSecond.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xGPSEpochWeekSecond);

using namespace gpstk;

void xGPSEpochWeekSecond :: setUp (void)
{
}

void xGPSEpochWeekSecond :: setFromInfoTest (void)
{
	try
	{
	gpstk::GPSEpochWeekSecond setFromInfo1;
	gpstk::GPSEpochWeekSecond setFromInfo2;
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('E',"6"));
	Id.insert(make_pair('F',"1"));
	Id.insert(make_pair('g',"0"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	Id.erase('E');
	CPPUNIT_ASSERT(!(setFromInfo2.setFromInfo(Id)));
	ofstream out("Logs/printfOutput");
	
	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
	}
	catch (gpstk::Exception& e)
	{
		cout << e;
	}
}

void xGPSEpochWeekSecond :: operatorTest (void)
{
	try
	{
	gpstk::GPSEpochWeekSecond Compare(6,2,1);
	gpstk::GPSEpochWeekSecond LessThanEpoch(5,2,1);
	gpstk::GPSEpochWeekSecond LessThanWeek(6,1,1);
	gpstk::GPSEpochWeekSecond LessThanSOW(6,2,0);
	
	gpstk::GPSEpochWeekSecond CompareCopy(Compare);
	
	gpstk::GPSEpochWeekSecond CompareCopy2;
	//Assignment
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanEpoch);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanEpoch < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanEpoch));
	CPPUNIT_ASSERT(LessThanWeek < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanWeek));
	CPPUNIT_ASSERT(LessThanSOW < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanSOW));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanEpoch);
	CPPUNIT_ASSERT(Compare > LessThanWeek);
	CPPUNIT_ASSERT(Compare > LessThanSOW);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanEpoch <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanEpoch);
	CPPUNIT_ASSERT(Compare >= CompareCopy);
	
	CPPUNIT_ASSERT(Compare.isValid());
	}
	catch (gpstk::Exception& e)
	{
		cout << e;
	}
}


