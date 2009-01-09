#include "xUnixTime.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xUnixTime);

using namespace gpstk;

void xUnixTime :: setUp (void)
{
}

void xUnixTime :: setFromInfoTest (void)
{
	Rinex3::UnixTime setFromInfo1;
	Rinex3::UnixTime setFromInfo2;
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('U',"1350000"));
	Id.insert(make_pair('u',"0"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	Id.erase('U');
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	ofstream out("Logs/printfOutput");
	
	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
}

void xUnixTime :: operatorTest (void)
{
	
	Rinex3::UnixTime Compare(1350000, 100);
	Rinex3::UnixTime LessThanSec(1340000, 100);
	Rinex3::UnixTime LessThanMicroSec(1350000,0);
	
	Rinex3::UnixTime CompareCopy(Compare);
	
	Rinex3::UnixTime CompareCopy2;
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanSec);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanSec < Compare);
	CPPUNIT_ASSERT(LessThanMicroSec < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanSec));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanSec);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanSec <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanSec);
	CPPUNIT_ASSERT(Compare >= CompareCopy);
	
	//gpstk::CommonTime Rollover(1,1,.9999999999999);
	//Rinex3::UnixTime Temp;
	//Temp.convertFromCommonTime(Rollover);
	
	CPPUNIT_ASSERT(Compare.isValid());
}

void xUnixTime :: resetTest (void)
{
        Rinex3::UnixTime Compare(1350000,0,gpstk::GPS);
	std::cout << Compare.getTimeSystem() << std::endl;

	CommonTime Test = Compare.convertToCommonTime();

	Rinex3::UnixTime Test2;
	Test2.convertFromCommonTime(Test);

	CPPUNIT_ASSERT_EQUAL(Test2,Compare);

	std::cout << Compare.getTimeSystem() << std::endl;
	CPPUNIT_ASSERT_EQUAL(gpstk::GPS,Compare.getTimeSystem());
/*
	CPPUNIT_ASSERT_EQUAL(1350000,(int)Compare.tv.tv_sec);
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.tv.tv_usec);

	Compare.reset();
	CPPUNIT_ASSERT_EQUAL(gpstk::Unknown,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.tv.tv_sec);
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.tv.tv_usec);
*/
}

void xUnixTime :: timeSystemTest (void)
{
/*
	Rinex3::UnixTime GPS1(1350000,gpstk::GPS);
	Rinex3::UnixTime GPS2(1340000,gpstk::GPS);
	Rinex3::UnixTime UTC(1350000,gpstk::UTC);
	Rinex3::UnixTime UNKNOWN(1350000,gpstk::Unknown);
	Rinex3::UnixTime ANY(1350000,gpstk::Any);

	CPPUNIT_ASSERT(GPS1 != GPS2);
	CPPUNIT_ASSERT_EQUAL(GPS1.getTimeSystem(),GPS2.getTimeSystem());
	CPPUNIT_ASSERT(GPS1 != UTC);
	CPPUNIT_ASSERT(GPS1 != UNKNOWN);
	CPPUNIT_ASSERT(GPS1.convertToCommonTime() > gpstk::CommonTime::BEGINNING_OF_TIME);
	CPPUNIT_ASSERT(gpstk::CommonTime::BEGINNING_OF_TIME < GPS1);
	CPPUNIT_ASSERT_EQUAL(GPS1,ANY);
	CPPUNIT_ASSERT_EQUAL(UTC,ANY);
	CPPUNIT_ASSERT_EQUAL(UNKNOWN,ANY);
	CPPUNIT_ASSERT(GPS2 != ANY);
	CPPUNIT_ASSERT(GPS2 < GPS1);
	CPPUNIT_ASSERT(GPS2 < ANY);

	UNKNOWN.setTimeSystem(gpstk::GPS);
	CPPUNIT_ASSERT_EQUAL(UNKNOWN.getTimeSystem(),gpstk::GPS);
*/
}

void xUnixTime :: printfTest (void)
{

	Rinex3::UnixTime GPS1(1350000,gpstk::GPS);
	Rinex3::UnixTime UTC(1350000,gpstk::UTC);

//	CPPUNIT_ASSERT_EQUAL(GPS1.printf("%07U %02u %02P"),(std::string)"1350000 0.0 02");
	CPPUNIT_ASSERT_EQUAL(UTC.printf("%08J %02u %02P"),(std::string)"1350000.000000 0.0 03");
	CPPUNIT_ASSERT_EQUAL(GPS1.printError("%08J %02u %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");
	CPPUNIT_ASSERT_EQUAL(UTC.printError("%08J %02u %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");
}
