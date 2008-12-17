#include "xYDSTime.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xYDSTime);

using namespace gpstk;

void xYDSTime :: setUp (void)
{
}

void xYDSTime :: setFromInfoTest (void)
{
	Rinex3::YDSTime setFromInfo1;
	Rinex3::YDSTime setFromInfo2;
	Rinex3::YDSTime setFromInfo3;
	Rinex3::YDSTime setFromInfo4;
	Rinex3::YDSTime setFromInfo5;
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('Y',"2006"));
	Id.insert(make_pair('j',"1"));
	Id.insert(make_pair('s',"0"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	Id.erase('Y');
	Id.insert(make_pair('y',"06"));
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	Id.erase('y');
	Id.insert(make_pair('y',"006"));
	CPPUNIT_ASSERT(setFromInfo3.setFromInfo(Id));
	Id.erase('y');
	Id.insert(make_pair('y',"2006"));
	CPPUNIT_ASSERT(setFromInfo4.setFromInfo(Id));
	Id.erase('y');
	CPPUNIT_ASSERT(setFromInfo5.setFromInfo(Id));
	ofstream out("Logs/printfOutput");
	
	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
	out << setFromInfo3 << endl;
	out << setFromInfo4 << endl;
}

void xYDSTime :: operatorTest (void)
{
	
	Rinex3::YDSTime Compare(2006,2,1);
	Rinex3::YDSTime LessThanYear(2005,2,1);
	Rinex3::YDSTime LessThanDOY(2006,1,1);
	Rinex3::YDSTime LessThanSOD(2006,2,0);
	
	Rinex3::YDSTime CompareCopy(Compare);
	
	Rinex3::YDSTime CompareCopy2;
	//Assignment
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanYear);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanYear < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanYear));
	CPPUNIT_ASSERT(LessThanDOY < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanDOY));
	CPPUNIT_ASSERT(LessThanSOD < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanSOD));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanYear);
	CPPUNIT_ASSERT(Compare > LessThanDOY);
	CPPUNIT_ASSERT(Compare > LessThanSOD);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanYear <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanYear);
	CPPUNIT_ASSERT(Compare >= CompareCopy);
	
	CPPUNIT_ASSERT(Compare.isValid());
}

