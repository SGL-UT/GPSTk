#include "xJulianDate.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xJulianDate);

using namespace gpstk;

void xJulianDate :: setUp (void)
{
}

void xJulianDate :: setFromInfoTest (void)
{
	Rinex3::JulianDate setFromInfo1;
	Rinex3::JulianDate setFromInfo2;
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('J',"1350000"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	Id.erase('J');
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	ofstream out("Logs/printfOutput");
	
	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
}

void xJulianDate :: operatorTest (void)
{
	
	Rinex3::JulianDate Compare(1350000);
	Rinex3::JulianDate LessThanJD(1340000);
	
	Rinex3::JulianDate CompareCopy(Compare);
	
	Rinex3::JulianDate CompareCopy2;
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

void xJulianDate :: resetTest (void)
{

	Rinex3::JulianDate Compare(13500000,gpstk::GPS);

//	CommonTime Test = Compare.convertToCommonTime();

//	Rinex3::JulianDate Test2;
//	Test2.convertFromCommonTime(Test);

//	CPPUNIT_ASSERT_EQUAL(Test2,Compare);

	CPPUNIT_ASSERT_EQUAL(gpstk::GPS,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(13500000,(int)Compare.jd);

	Compare.reset();
	CPPUNIT_ASSERT_EQUAL(gpstk::Unknown,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.jd);

}

void xJulianDate :: timeSystemTest (void)
{

	Rinex3::JulianDate GPS1(13500000,gpstk::GPS);
	Rinex3::JulianDate GPS2(13400000,gpstk::GPS);
	Rinex3::JulianDate UTC(13500000,gpstk::UTC);
	Rinex3::JulianDate UNKNOWN(13500000,gpstk::Unknown);
	Rinex3::JulianDate ANY(13500000,gpstk::Any);

	CPPUNIT_ASSERT(GPS1 != GPS2);
	CPPUNIT_ASSERT_EQUAL(GPS1.getTimeSystem(),GPS2.getTimeSystem());
	CPPUNIT_ASSERT(GPS1 != UTC);
	CPPUNIT_ASSERT(GPS1 != UNKNOWN);
//	CPPUNIT_ASSERT(GPS1.convertToCommonTime() > gpstk::CommonTime::BEGINNING_OF_TIME);
//	CPPUNIT_ASSERT(gpstk::CommonTime::BEGINNING_OF_TIME < GPS1);
	CPPUNIT_ASSERT_EQUAL(GPS1,ANY);
	CPPUNIT_ASSERT_EQUAL(UTC,ANY);
	CPPUNIT_ASSERT_EQUAL(UNKNOWN,ANY);
	CPPUNIT_ASSERT(GPS2 != ANY);
	CPPUNIT_ASSERT(GPS2 < GPS1);
	CPPUNIT_ASSERT(GPS2 < ANY);

	UNKNOWN.setTimeSystem(gpstk::GPS);
	CPPUNIT_ASSERT_EQUAL(UNKNOWN.getTimeSystem(),gpstk::GPS);

}

void xJulianDate :: printfTest (void)
{
	Rinex3::JulianDate GPS1(13500000,gpstk::GPS);
	Rinex3::JulianDate UTC(13500000,gpstk::UTC);


	CPPUNIT_ASSERT_EQUAL(GPS1.printf("%08J %02P"),(std::string)"13500000.000000 02");
	CPPUNIT_ASSERT_EQUAL(UTC.printf("%08J %02P"),(std::string)"13500000.000000 03");
	CPPUNIT_ASSERT_EQUAL(GPS1.printError("%08J %02P"),(std::string)"ErrorBadTime ErrorBadTime");
	CPPUNIT_ASSERT_EQUAL(UTC.printError("%08J %02P"),(std::string)"ErrorBadTime ErrorBadTime");
}
