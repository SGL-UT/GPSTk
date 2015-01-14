#include "YDSTime.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class YDSTime_T
{
	public:

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Add Initialization test here
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	/* Test will check if YDSTime variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "YDSTime", "Constructor(year,day,second,TimeSystem)", __FILE__, __func__ );
		testFramework.init();

		YDSTime setFromInfo1;
		YDSTime setFromInfo2;
		YDSTime setFromInfo3;
		YDSTime setFromInfo4;
		YDSTime setFromInfo5;
		YDSTime Compare(2008,2,1,TimeSystem::GPS), Compare2(2006,2,1,TimeSystem::GPS);
		YDSTime Compare3(0,2,1,TimeSystem::GPS);

		TimeTag::IdToValue Id;
		Id['Y'] = "2008";
		Id['j'] = "2";
		Id['s'] = "1";
		Id['P'] = "GPS";

//--------------ANSITime_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		testFramework.assert(setFromInfo1.setFromInfo(Id);
		testFramework.next();

//--------------ANSITime_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo1);
		testFramework.next();

		Id.erase('Y');
		Id['y'] = "06";

//--------------ANSITime_setFromInfoTest_3 - Does a proper setFromInfo work with 2 digit year?
		testFramework.assert(setFromInfo2.setFromInfo(Id));
		testFramework.next();

//--------------ANSITime_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo2);
		testFramework.next();
						
		//Can we set a three digit year with 'y' option? Answer should be no.
		Id.erase('y');
		Id['y'] = "006";

//--------------ANSITime_setFromInfoTest_5 - Does a proper setFromInfo work with 3 digit year?
		testFramework.assert(setFromInfo3.setFromInfo(Id));
		testFramework.next();

//--------------ANSITime_setFromInfoTest_6 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo3);
		testFramework.next();

		Id.erase('y');
		Id['y'] = "2008";

//--------------ANSITime_setFromInfoTest_7 - Does a proper setFromInfo work with 4 digit year labeled as 2 digits?
		testFramework.assert(setFromInfo4.setFromInfo(Id));
		testFramework.next();

//--------------ANSITime_setFromInfoTest_8 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo4);
		testFramework.next();

		Id.erase('y');

//--------------ANSITime_setFromInfoTest_9 - Does a proper setFromInfo work with 4 digit year labeled as 2 digits?
		testFramework.assert(setFromInfo5.setFromInfo(Id));
		testFramework.next();

//--------------ANSITime_setFromInfoTest_10 - Did the previous setFromInfo set the proper values?
		testFramework.assert(setFromInfo5 == Compare3);

		return testFramework.countFails();
	}

	/* Test will check if the ways to initialize and set an YDSTime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil testFramework( "YDSTime", "== Operator", __FILE__, __func__ );
		testFramework.init();

		YDSTime Compare(2008,2,1);// Initialize with value
		YDSTime LessThanYear(2005,2,1);// Initialize with value with a smaller year
		YDSTime LessThanDOY(2008,1,1);// Initialize with value with a smaller month
		YDSTime LessThanSOD(2008,2,0);// Initialize with value with a smaller day
		YDSTime CompareCopy(Compare); // Initialize with copy constructor
		YDSTime CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

//--------------ANSITime_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare == CompareCopy);
		testFramework.next();

//--------------ANSITime_operatorTest_2 - Are equivalent objects equivalent?
		testFramework.assert(!(Compare == LessThanYear));
		testFramework.next();

		testFramework.changeSourceMethod("!= Operator");

//--------------ANSITime_operatorTest_3 - Are non-equivalent objects not equivalent?
		testFramework.assert(Compare != LessThanYear);
		testFramework.next();

//--------------ANSITime_operatorTest_4 - Are equivalent objects not equivalent?
		testFramework.assert(!(Compare != Compare));
		testFramework.next();

		testFramework.changeSourceMethod("< Operator");

//--------------ANSITime_operatorTest_5 - Does the < operator function when left_object < right_object by years?
		testFramework.assert(LessThanYear < Compare));
		testFramework.next();

//--------------ANSITime_operatorTest_6 - Does the < operator function when left_object > right_object by years?
		testFramework.assert(!(Compare < LessThanYear));
		testFramework.next();

//--------------ANSITime_operatorTest_7 - Does the < operator function when left_object < right_object by days?
		testFramework.assert(LessThanDOY < Compare);
		testFramework.next();

//--------------ANSITime_operatorTest_8 - Does the < operator function when left_object > right_object by days?		
		testFramework.assert(!(Compare < LessThanDOY));
		testFramework.next();

//--------------ANSITime_operatorTest_9 - Does the < operator function when left_object < right_object by seconds?
		testFramework.assert(LessThanSOD < Compare);
		testFramework.next();

//--------------ANSITime_operatorTest_10 - Does the < operator function when left_object > right_object by seconds?
		testFramework.assert(!(Compare < LessThanSOD));
		testFramework.next();

//--------------ANSITime_operatorTest_11 - Does the < operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();

		//Greater than assertions
		testFramework.changeSourceMethod("> Operator");

//--------------ANSITime_operatorTest_12 - Does the > operator function when left_object > right_object by years?
		testFramework.assert(Compare > LessThanYear));
		testFramework.next();

//--------------ANSITime_operatorTest_13 - Does the > operator function when left_object < right_object by years?
		testFramework.assert(!(LessThanYear > Compare));
		testFramework.next();

//--------------ANSITime_operatorTest_14 - Does the > operator function when left_object > right_object by days?
		testFramework.assert(Compare > LessThanDOY);
		testFramework.next();

//--------------ANSITime_operatorTest_15 - Does the > operator function when left_object < right_object by days?		
		testFramework.assert(!(LessThanDOY > Compare));
		testFramework.next();

//--------------ANSITime_operatorTest_16 - Does the > operator function when left_object > right_object by seconds?
		testFramework.assert(Compare > LessThanSOD);
		testFramework.next();

//--------------ANSITime_operatorTest_17 - Does the > operator function when left_object < right_object by seconds?
		testFramework.assert(!(LessThanSOD > Compare));
		testFramework.next();

//--------------ANSITime_operatorTest_18 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare > CompareCopy));
		testFramework.next();	

		//Less than equals assertion
		testFramework.changeSourceMethod("<= Operator");

//--------------ANSITime_operatorTest_19 - Does the < operator function when left_object < right_object by years?
		testFramework.assert(LessThanYear <= Compare));
		testFramework.next();

//--------------ANSITime_operatorTest_20 - Does the <= operator function when left_object > right_object by years?
		testFramework.assert(!(Compare <= LessThanYear));
		testFramework.next();

//--------------ANSITime_operatorTest_21 - Does the <= operator function when left_object < right_object by days?
		testFramework.assert(LessThanDOY <= Compare);
		testFramework.next();

//--------------ANSITime_operatorTest_22 - Does the <= operator function when left_object > right_object by days?		
		testFramework.assert(!(Compare <= LessThanDOY));
		testFramework.next();

//--------------ANSITime_operatorTest_23 - Does the <= operator function when left_object < right_object by seconds?
		testFramework.assert(LessThanSOD <= Compare);
		testFramework.next();

//--------------ANSITime_operatorTest_24 - Does the <= operator function when left_object > right_object by seconds?
		testFramework.assert(!(Compare <= LessThanSOD));
		testFramework.next();

//--------------ANSITime_operatorTest_25 - Does the <= operator function when left_object = right_object?
		testFramework.assert(Compare <= CompareCopy);
		testFramework.next();

		//Greater than equals assertion
		testFramework.changeSourceMethod(">= Operator");

//--------------ANSITime_operatorTest_26 - Does the >= operator function when left_object > right_object by years?
		testFramework.assert(Compare >= LessThanYear));
		testFramework.next();

//--------------ANSITime_operatorTest_27 - Does the >= operator function when left_object < right_object by years?
		testFramework.assert(!(LessThanYear >= Compare));
		testFramework.next();

//--------------ANSITime_operatorTest_28 - Does the >= operator function when left_object > right_object by days?
		testFramework.assert(Compare >= LessThanDOY);
		testFramework.next();

//--------------ANSITime_operatorTest_29 - Does the >= operator function when left_object < right_object by days?		
		testFramework.assert(!(Compare >= LessThanDOY));
		testFramework.next();

//--------------ANSITime_operatorTest_30 - Does the >= operator function when left_object > right_object by seconds?
		testFramework.assert(Compare >= LessThanSOD);
		testFramework.next();

//--------------ANSITime_operatorTest_31 - Does the >= operator function when left_object < right_object by seconds?
		testFramework.assert(!(LessThanSOD >= Compare));
		testFramework.next();

//--------------ANSITime_operatorTest_32 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();	

		//Validity check
		testFramework.changeSourceMethod("isValid Method");

//--------------ANSITime_operatorTest_33 - Does the isValid methods function properly?
		testFramework.assert(Compare.isValid());

		return testFramework.countFails();
	}

	/* Test will check the reset method. */
	int resetTest (void)
	{
		TestUtil testFramework( "YDSTime", "reset" , __FILE__, __func__ );
		testFramework.init();

	  	YDSTime Compare(2008,2,1,TimeSystem::GPS); //Initialize an object
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Test is redundant of the initialization test
/*
		//Check initial data
		if (TimeSystem(TimeSystem::GPS) != Compare.getTimeSystem()) return 1;
		if (2008 != (int)Compare.year) return 2;
		if (2 != (int)Compare.doy) return 3;
		if (1 != (int)Compare.sod) return 4;
*/
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	  	Compare.reset(); // Reset it

//--------------ANSITime_operatorTest_1 - Was the time system reset to expectation?
	  	testFramework.assert(TimeSystem(0) == Compare.getTimeSystem());
	  	testFramework.next();

//--------------ANSITime_operatorTest_2 - Was the year value reset to expectation?	  	
		testFramework.assert(0 == (int)Compare.year); 
		testFramework.next();

//--------------ANSITime_operatorTest_3 - Was the day value reset to expectation?
		testFramework.assert(0 == (int)Compare.doy); 
		testFramework.next();

//--------------ANSITime_operatorTest_4 - Was the second value reset to expectation?	  	
		testFramework.assert(0 == (int)Compare.sod); 
		testFramework.next();

		return testFramework.countFails();
	}


	/* Test will check converting to/from CommonTime. */
	int toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "ANSITime", "isValid", __FILE__, __func__ );
		testFramework.init();

	  	YDSTime Compare(2008,2,1,TimeSystem::GPS); //Initialize an object
  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

//--------------ANSITime_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testFramework.next();

//--------------ANSITime_toFromCommonTimeTest_2 - Is the set object valid?
		testFramework.assert(Compare.isValid());
		testFramework.next();

  		YDSTime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

 		testFramework.changeSourceMethod("CommonTime Conversion");
//--------------ANSITime_toFromCommonTimeTest_3 - Is the result of conversion the same?
		testFramework.assert(Test2 == Compare);
		testFramework.next();

//--------------ANSITime_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		testFramework.assert(Compare.getTimeSystem()==TimeSystem(2));
		testFramework.next();

//--------------ANSITime_toFromCommonTimeTest_5 - Is the year after conversion what is expected?
		testFramework.assert(2008 == (int)Compare.year);
		testFramework.next();

//--------------ANSITime_toFromCommonTimeTest_6 - Is the day after conversion what is expected?
		testFramework.assert(2 == (int)Compare.doy);
		testFramework.next();

//--------------ANSITime_toFromCommonTimeTest_5 - Is the second after conversion what is expected?
		testFramework.assert(1 == (int)Compare.sod);
		return testFramework.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int timeSystemTest (void)
	{

  		YDSTime GPS1(   2008,2,1,TimeSystem::GPS    );
		YDSTime GPS2(   2005,2,1,TimeSystem::GPS    );
		YDSTime UTC1(   2008,2,1,TimeSystem::UTC    );
		YDSTime UNKNOWN(2008,2,1,TimeSystem::Unknown);
		YDSTime ANY(    2008,2,1,TimeSystem::Any    );

  		if (GPS1 == GPS2) return 1; // GPS1 and GPS2 should have different times
  		if (GPS1.getTimeSystem() != GPS2.getTimeSystem()) return 2; // Should have the same time system
  		if (GPS1 == UTC1) return 3; //Should have different time systems
  		if (GPS1 == UNKNOWN) return 4;

		// Perform comparisons to start of CommonTime
  		if (GPS1.convertToCommonTime() < CommonTime::BEGINNING_OF_TIME) return 11;
  		if (CommonTime::BEGINNING_OF_TIME > GPS1) return 12;
		
		// Make TimeSystem part not matter and perform comparisons
		// which solely depend on the time value.
  		if (GPS1 != ANY) return 5; 
  		if (UTC1 != ANY) return 6;
  		if (UNKNOWN != ANY) return 7;
  		if (GPS2 == ANY) return 8;
  		if (GPS2 > GPS1) return 9;
  		if (GPS2 > ANY) return 10;

  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
  		if (UNKNOWN.getTimeSystem()!=TimeSystem(2)) return 13;
		return 0;
	}
	/* Test for the formatted printing of YDSTime objects */
	int printfTest (void)
	{

		YDSTime GPS1(2008,2,1,TimeSystem::GPS);
		YDSTime UTC1(2008,2,1,TimeSystem::UTC);
		
  		if (GPS1.printf("%04Y %02y %03j %02s %02P") != (std::string)"2008 08 002 1.000000 GPS") return 1;
  		if (UTC1.printf("%04Y %02y %03j %02s %02P") != (std::string)"2008 08 002 1.000000 UTC") return 2;
  		if (GPS1.printError("%04Y %02y %03j %02s %02P") != (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime") return 3; 
  		if (UTC1.printError("%04Y %02y %03j %02s %02P") != (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime") return 4;
		return 0;
	}
};

void checkResult(int check, int& errCount) // Function to handle test result output
{
	if (check == -1)
	{
		std::cout << "DIDN'T RUN!!!!" << std::endl;
	}
	else if (check == 0 )
	{
		std::cout << "GOOD!!!!" << std::endl;
	}
	else if (check > 0)
	{
		std::cout << "BAD!!!!" << std::endl;
		std::cout << "Error Message for Bad Test is Code " << check << std::endl;
		errCount++;
	}
}

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	YDSTime_T testClass;
	check = testClass.operatorTest();
        std::cout << "opertatorTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.setFromInfoTest();
        std::cout << "setFromInfoTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.resetTest();
        std::cout << "resetTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.timeSystemTest();
        std::cout << "timeSystemTest Result is: "; 
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.toFromCommonTimeTest();
        std::cout << "toFromCommonTimeTest Result is: "; 
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.printfTest();
        std::cout << "printfTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	
	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
