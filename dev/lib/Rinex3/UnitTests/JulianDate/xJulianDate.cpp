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
