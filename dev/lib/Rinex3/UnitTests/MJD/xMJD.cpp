#include "xMJD.hpp"
#include "TimeSystem.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xMJD);

using namespace Rinex3;

void xMJD :: setUp (void)
{
}

void xMJD :: setFromInfoTest (void)
{
	MJD setFromInfo1;
	MJD setFromInfo2;
	MJD Compare(135000.0,TimeSys::GPS);
	
	TimeTag::IdToValue Id;
	Id.insert(make_pair('Q',"135000.0"));
	Id.insert(make_pair('P',"02"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	CPPUNIT_ASSERT_EQUAL(setFromInfo1,Compare);
	Id.erase('Q');
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
}

void xMJD :: operatorTest (void)
{
	
	MJD Compare(135000);
	MJD LessThanJD(134000);
	
	MJD CompareCopy(Compare);
	
	MJD CompareCopy2;
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanJD);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanJD < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanJD));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanJD);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanJD <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanJD);
	CPPUNIT_ASSERT(Compare >= CompareCopy);
	
	CPPUNIT_ASSERT(Compare.isValid());
}

void xMJD :: resetTest (void)
{
	MJD Compare(135000,TimeSys::GPS);

	CommonTime Test = Compare.convertToCommonTime();

	MJD Test2;
	Test2.convertFromCommonTime(Test);

	CPPUNIT_ASSERT_EQUAL(Test2,Compare);

	CPPUNIT_ASSERT_EQUAL(TimeSys::GPS,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(135000,(int)Compare.mjd);

	Compare.reset();
	CPPUNIT_ASSERT_EQUAL(TimeSys::Unknown,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.mjd);
}

void xMJD :: timeSystemTest (void)
{
	MJD GPS1(135000,TimeSys::GPS);
	MJD GPS2(134000,TimeSys::GPS);
	MJD UTC1(135000,TimeSys::UTC);
	MJD UNKNOWN(135000,TimeSys::Unknown);
	MJD ANY(135000,TimeSys::Any);

	CPPUNIT_ASSERT(GPS1 != GPS2);
	CPPUNIT_ASSERT_EQUAL(GPS1.getTimeSystem(),GPS2.getTimeSystem());
	CPPUNIT_ASSERT(GPS1 != UTC1);
	CPPUNIT_ASSERT(GPS1 != UNKNOWN);
	CPPUNIT_ASSERT(GPS1.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
	CPPUNIT_ASSERT(CommonTime::BEGINNING_OF_TIME < GPS1);
	CPPUNIT_ASSERT_EQUAL(GPS1,ANY);
	CPPUNIT_ASSERT_EQUAL(UTC1,ANY);
	CPPUNIT_ASSERT_EQUAL(UNKNOWN,ANY);
	CPPUNIT_ASSERT(GPS2 != ANY);
	CPPUNIT_ASSERT(GPS2 < GPS1);
	CPPUNIT_ASSERT(GPS2 < ANY);

	UNKNOWN.setTimeSystem(TimeSys::GPS);
	CPPUNIT_ASSERT_EQUAL(UNKNOWN.getTimeSystem(),TimeSys::GPS);
}

void xMJD :: printfTest (void)
{
	MJD GPS1(135000,TimeSys::GPS);
	MJD UTC1(135000,TimeSys::UTC);


	CPPUNIT_ASSERT_EQUAL(GPS1.printf("%08Q %02P"),(std::string)"135000.000000 [GPS]");
	CPPUNIT_ASSERT_EQUAL(UTC1.printf("%08Q %02P"),(std::string)"135000.000000 [UTC]");
	CPPUNIT_ASSERT_EQUAL(GPS1.printError("%08Q %02P"),(std::string)"ErrorBadTime ErrorBadTime");
	CPPUNIT_ASSERT_EQUAL(UTC1.printError("%08Q %02P"),(std::string)"ErrorBadTime ErrorBadTime");
}

