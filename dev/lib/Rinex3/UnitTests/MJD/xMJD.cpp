#include "xMJD.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xMJD);

using namespace gpstk;

void xMJD :: setUp (void)
{
}

void xMJD :: setFromInfoTest (void)
{
	Rinex3::MJD setFromInfo1;
	Rinex3::MJD setFromInfo2;
	Rinex3::MJD Compare(135000.0,GPS);
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('Q',"135000.0"));
	Id.insert(make_pair('P',"02"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	CPPUNIT_ASSERT_EQUAL(setFromInfo1,Compare);
	Id.erase('Q');
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
}

void xMJD :: operatorTest (void)
{
	
	Rinex3::MJD Compare(135000);
	Rinex3::MJD LessThanJD(134000);
	
	Rinex3::MJD CompareCopy(Compare);
	
	Rinex3::MJD CompareCopy2;
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
	Rinex3::MJD Compare(135000,gpstk::GPS);

	CommonTime Test = Compare.convertToCommonTime();

	Rinex3::MJD Test2;
	Test2.convertFromCommonTime(Test);

	CPPUNIT_ASSERT_EQUAL(Test2,Compare);

	CPPUNIT_ASSERT_EQUAL(gpstk::GPS,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(135000,(int)Compare.mjd);

	Compare.reset();
	CPPUNIT_ASSERT_EQUAL(gpstk::Unknown,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.mjd);
}

void xMJD :: timeSystemTest (void)
{
	Rinex3::MJD GPS1(135000,gpstk::GPS);
	Rinex3::MJD GPS2(134000,gpstk::GPS);
	Rinex3::MJD UTC(135000,gpstk::UTC);
	Rinex3::MJD UNKNOWN(135000,gpstk::Unknown);
	Rinex3::MJD ANY(135000,gpstk::Any);

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
}

void xMJD :: printfTest (void)
{
	Rinex3::MJD GPS1(135000,gpstk::GPS);
	Rinex3::MJD UTC(135000,gpstk::UTC);


	CPPUNIT_ASSERT_EQUAL(GPS1.printf("%08Q %02P"),(std::string)"135000.000000 02");
	CPPUNIT_ASSERT_EQUAL(UTC.printf("%08Q %02P"),(std::string)"135000.000000 03");
	CPPUNIT_ASSERT_EQUAL(GPS1.printError("%08Q %02P"),(std::string)"ErrorBadTime ErrorBadTime");
	CPPUNIT_ASSERT_EQUAL(UTC.printError("%08Q %02P"),(std::string)"ErrorBadTime ErrorBadTime");
}

