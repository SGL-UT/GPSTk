#include "xGPSWeekSecond.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xGPSWeekSecond);

using namespace gpstk;

void xGPSWeekSecond :: setUp (void)
{
}

void xGPSWeekSecond :: setFromInfoTest (void)
{
	gpstk::GPSWeekSecond setFromInfo1;
	gpstk::GPSWeekSecond setFromInfo2;
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('F',"1300"));
	Id.insert(make_pair('g',"13500"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	Id.erase('F');
	CPPUNIT_ASSERT(!(setFromInfo2.setFromInfo(Id)));
	ofstream out("Logs/printfOutput");
	
	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
}

void xGPSWeekSecond :: operatorTest (void)
{
	gpstk::GPSWeekSecond Zero;
	
	gpstk::GPSWeekSecond Compare(1300,13500.);
	gpstk::GPSWeekSecond LessThanWeek(1299,13500.);
	gpstk::GPSWeekSecond LessThanSecond(1300,13400.);
	
	gpstk::GPSWeekSecond CompareCopy(Compare);
	
	gpstk::GPSWeekSecond CompareCopy2;
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanWeek);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanWeek < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanWeek));
	CPPUNIT_ASSERT(LessThanSecond < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanSecond));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanWeek);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanWeek <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanWeek);
	CPPUNIT_ASSERT(Compare >= CompareCopy);
	
	CPPUNIT_ASSERT(Compare.isValid());
	//CPPUNIT_ASSERT(!Zero.isValid());
}
