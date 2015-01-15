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
	/* Test to ensure the values in the constructor go to their intended locations */
	int initializationTest (void)
	{
		TestUtil testFramework( "YDSTime", "Constructor(year,day,second,TimeSystem)", __FILE__, __func__ );
		testFramework.init();

		YDSTime Compare(2008,2,1,TimeSystem::GPS); //Initialize an object

//--------------YDSTime_initializationTest_1 - Was the year value set to expectation?
		testFramework.assert(2008 == Compare.year);
		testFramework.next();

//--------------YDSTime_initializationTest_2 - Was the day value set to expectation?
		testFramework.assert(2 == Compare.doy);
		testFramework.next();

//--------------YDSTime_initializationTest_3 - Was the second value set to expectation?
		testFramework.assert(1 == Compare.sod);
		testFramework.next();

//--------------YDSTime_initializationTest_4 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Constructor(YDSTime)");
		YDSTime Copy(Compare); //Initialize the copy constructor

//--------------YDSTime_initializationTest_5 - Was the year value set to expectation?
		testFramework.assert(2008 == Copy.year);
		testFramework.next();

//--------------YDSTime_initializationTest_6 - Was the day value set to expectation?
		testFramework.assert(2 == Copy.doy);
		testFramework.next();

//--------------YDSTime_initializationTest_7 - Was the second value set to expectation?
		testFramework.assert(1 == Copy.sod);
		testFramework.next();

//--------------YDSTime_initializationTest_8 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("= Operator");
		YDSTime Assigned;
		Assigned = Compare;

//--------------YDSTime_initializationTest_5 - Was the year value set to expectation?
		testFramework.assert(2008 == Assigned.year);
		testFramework.next();

//--------------YDSTime_initializationTest_6 - Was the day value set to expectation?
		testFramework.assert(2 == Assigned.doy);
		testFramework.next();

//--------------YDSTime_initializationTest_7 - Was the second value set to expectation?
		testFramework.assert(1 == Assigned.sod);
		testFramework.next();

//--------------YDSTime_initializationTest_8 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem());

		return testFramework.countFails();
	}

	/* Test will check if YDSTime variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "YDSTime", "setFromInfo", __FILE__, __func__ );
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

//--------------YDSTime_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		testFramework.assert(setFromInfo1.setFromInfo(Id));
		testFramework.next();

//--------------YDSTime_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo1);
		testFramework.next();

		Id.erase('Y');
		Id['y'] = "06";

//--------------YDSTime_setFromInfoTest_3 - Does a proper setFromInfo work with 2 digit year?
		testFramework.assert(setFromInfo2.setFromInfo(Id));
		testFramework.next();

//--------------YDSTime_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo2);
		testFramework.next();
						
		//Can we set a three digit year with 'y' option? Answer should be no.
		Id.erase('y');
		Id['y'] = "006";

//--------------YDSTime_setFromInfoTest_5 - Does a proper setFromInfo work with 3 digit year?
		testFramework.assert(!setFromInfo3.setFromInfo(Id));
		testFramework.next();

		Id.erase('y');
		Id['y'] = "2008";

//--------------YDSTime_setFromInfoTest_6 - Does a proper setFromInfo work with 4 digit year labeled as 2 digits?
		testFramework.assert(!setFromInfo4.setFromInfo(Id));
		testFramework.next();

		Id.erase('y');

//--------------YDSTime_setFromInfoTest_7 - Does a proper setFromInfo work with no year provided?
		testFramework.assert(setFromInfo5.setFromInfo(Id));
		testFramework.next();

//--------------YDSTime_setFromInfoTest_8 - Did the previous setFromInfo set the proper values?
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

//--------------YDSTime_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare == CompareCopy);
		testFramework.next();

//--------------YDSTime_operatorTest_2 - Are equivalent objects equivalent?
		testFramework.assert(!(Compare == LessThanYear));
		testFramework.next();

		testFramework.changeSourceMethod("!= Operator");

//--------------YDSTime_operatorTest_3 - Are non-equivalent objects not equivalent?
		testFramework.assert(Compare != LessThanYear);
		testFramework.next();

//--------------YDSTime_operatorTest_4 - Are equivalent objects not equivalent?
		testFramework.assert(!(Compare != Compare));
		testFramework.next();

		testFramework.changeSourceMethod("< Operator");

//--------------YDSTime_operatorTest_5 - Does the < operator function when left_object < right_object by years?
		testFramework.assert(LessThanYear < Compare);
		testFramework.next();

//--------------YDSTime_operatorTest_6 - Does the < operator function when left_object > right_object by years?
		testFramework.assert(!(Compare < LessThanYear));
		testFramework.next();

//--------------YDSTime_operatorTest_7 - Does the < operator function when left_object < right_object by days?
		testFramework.assert(LessThanDOY < Compare);
		testFramework.next();

//--------------YDSTime_operatorTest_8 - Does the < operator function when left_object > right_object by days?		
		testFramework.assert(!(Compare < LessThanDOY));
		testFramework.next();

//--------------YDSTime_operatorTest_9 - Does the < operator function when left_object < right_object by seconds?
		testFramework.assert(LessThanSOD < Compare);
		testFramework.next();

//--------------YDSTime_operatorTest_10 - Does the < operator function when left_object > right_object by seconds?
		testFramework.assert(!(Compare < LessThanSOD));
		testFramework.next();

//--------------YDSTime_operatorTest_11 - Does the < operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();

		//Greater than assertions
		testFramework.changeSourceMethod("> Operator");

//--------------YDSTime_operatorTest_12 - Does the > operator function when left_object > right_object by years?
		testFramework.assert(Compare > LessThanYear);
		testFramework.next();

//--------------YDSTime_operatorTest_13 - Does the > operator function when left_object < right_object by years?
		testFramework.assert(!(LessThanYear > Compare));
		testFramework.next();

//--------------YDSTime_operatorTest_14 - Does the > operator function when left_object > right_object by days?
		testFramework.assert(Compare > LessThanDOY);
		testFramework.next();

//--------------YDSTime_operatorTest_15 - Does the > operator function when left_object < right_object by days?		
		testFramework.assert(!(LessThanDOY > Compare));
		testFramework.next();

//--------------YDSTime_operatorTest_16 - Does the > operator function when left_object > right_object by seconds?
		testFramework.assert(Compare > LessThanSOD);
		testFramework.next();

//--------------YDSTime_operatorTest_17 - Does the > operator function when left_object < right_object by seconds?
		testFramework.assert(!(LessThanSOD > Compare));
		testFramework.next();

//--------------YDSTime_operatorTest_18 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare > CompareCopy));
		testFramework.next();	

		//Less than equals assertion
		testFramework.changeSourceMethod("<= Operator");

//--------------YDSTime_operatorTest_19 - Does the < operator function when left_object < right_object by years?
		testFramework.assert(LessThanYear <= Compare);
		testFramework.next();

//--------------YDSTime_operatorTest_20 - Does the <= operator function when left_object > right_object by years?
		testFramework.assert(!(Compare <= LessThanYear));
		testFramework.next();

//--------------YDSTime_operatorTest_21 - Does the <= operator function when left_object < right_object by days?
		testFramework.assert(LessThanDOY <= Compare);
		testFramework.next();

//--------------YDSTime_operatorTest_22 - Does the <= operator function when left_object > right_object by days?		
		testFramework.assert(!(Compare <= LessThanDOY));
		testFramework.next();

//--------------YDSTime_operatorTest_23 - Does the <= operator function when left_object < right_object by seconds?
		testFramework.assert(LessThanSOD <= Compare);
		testFramework.next();

//--------------YDSTime_operatorTest_24 - Does the <= operator function when left_object > right_object by seconds?
		testFramework.assert(!(Compare <= LessThanSOD));
		testFramework.next();

//--------------YDSTime_operatorTest_25 - Does the <= operator function when left_object = right_object?
		testFramework.assert(Compare <= CompareCopy);
		testFramework.next();

		//Greater than equals assertion
		testFramework.changeSourceMethod(">= Operator");

//--------------YDSTime_operatorTest_26 - Does the >= operator function when left_object > right_object by years?
		testFramework.assert(Compare >= LessThanYear);
		testFramework.next();

//--------------YDSTime_operatorTest_27 - Does the >= operator function when left_object < right_object by years?
		testFramework.assert(!(LessThanYear >= Compare));
		testFramework.next();

//--------------YDSTime_operatorTest_28 - Does the >= operator function when left_object > right_object by days?
		testFramework.assert(Compare >= LessThanDOY);
		testFramework.next();

//--------------YDSTime_operatorTest_29 - Does the >= operator function when left_object < right_object by days?		
		testFramework.assert(!(LessThanDOY >= Compare));
		testFramework.next();

//--------------YDSTime_operatorTest_30 - Does the >= operator function when left_object > right_object by seconds?
		testFramework.assert(Compare >= LessThanSOD);
		testFramework.next();

//--------------YDSTime_operatorTest_31 - Does the >= operator function when left_object < right_object by seconds?
		testFramework.assert(!(LessThanSOD >= Compare));
		testFramework.next();

//--------------YDSTime_operatorTest_32 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();	

		//Validity check
		testFramework.changeSourceMethod("isValid Method");

//--------------YDSTime_operatorTest_33 - Does the isValid methods function properly?
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

//--------------YDSTime_operatorTest_1 - Was the time system reset to expectation?
	  	testFramework.assert(TimeSystem(0) == Compare.getTimeSystem());
	  	testFramework.next();

//--------------YDSTime_operatorTest_2 - Was the year value reset to expectation?	  	
		testFramework.assert(0 == (int)Compare.year); 
		testFramework.next();

//--------------YDSTime_operatorTest_3 - Was the day value reset to expectation?
		testFramework.assert(0 == (int)Compare.doy); 
		testFramework.next();

//--------------YDSTime_operatorTest_4 - Was the second value reset to expectation?	  	
		testFramework.assert(0 == (int)Compare.sod); 
		testFramework.next();

		return testFramework.countFails();
	}


	/* Test will check converting to/from CommonTime. */
	int toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "YDSTime", "isValid", __FILE__, __func__ );
		testFramework.init();

	  	YDSTime Compare(2008,2,1,TimeSystem::GPS); //Initialize an object
  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

//--------------YDSTime_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testFramework.next();

//--------------YDSTime_toFromCommonTimeTest_2 - Is the set object valid?
		testFramework.assert(Compare.isValid());
		testFramework.next();

  		YDSTime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

 		testFramework.changeSourceMethod("CommonTime Conversion");
//--------------YDSTime_toFromCommonTimeTest_3 - Is the result of conversion the same?
		testFramework.assert(Test2 == Compare);
		testFramework.next();

//--------------YDSTime_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		testFramework.assert(Compare.getTimeSystem()==TimeSystem(2));
		testFramework.next();

//--------------YDSTime_toFromCommonTimeTest_5 - Is the year after conversion what is expected?
		testFramework.assert(2008 == (int)Compare.year);
		testFramework.next();

//--------------YDSTime_toFromCommonTimeTest_6 - Is the day after conversion what is expected?
		testFramework.assert(2 == (int)Compare.doy);
		testFramework.next();

//--------------YDSTime_toFromCommonTimeTest_5 - Is the second after conversion what is expected?
		testFramework.assert(1 == (int)Compare.sod);

		return testFramework.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int timeSystemTest (void)
	{
		TestUtil testFramework( "YDSTime", "Differing TimeSystem == Operator", __FILE__, __func__ );
		testFramework.init();

  		YDSTime GPS1(   2008,2,1,TimeSystem::GPS    );
		YDSTime GPS2(   2005,2,1,TimeSystem::GPS    );
		YDSTime UTC1(   2008,2,1,TimeSystem::UTC    );
		YDSTime UNKNOWN(2008,2,1,TimeSystem::Unknown);
		YDSTime ANY(    2008,2,1,TimeSystem::Any    );

//--------------YDSTime_timeSystemTest_1 - Verify same Time System but different time inequality
		testFramework.assert(!(GPS1 == GPS2));
		testFramework.next();

//--------------YDSTime_timeSystemTest_2 - Verify same Time System equality
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Differing TimeSystem != Operator");
//--------------YDSTime_timeSystemTest_3 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UTC1);
		testFramework.next();

//--------------YDSTime_timeSystemTest_4 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);
		testFramework.next();
		
		testFramework.changeSourceMethod("ANY TimeSystem == Operator");		
//--------------YDSTime_timeSystemTest_5 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		testFramework.assert(GPS1 == ANY);
		testFramework.next();

//--------------YDSTime_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		testFramework.assert(UTC1 == ANY);
		testFramework.next();

//--------------YDSTime_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		testFramework.assert(UNKNOWN == ANY);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem < Operator");	
//--------------YDSTime_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY));
		testFramework.next();

		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
//--------------YDSTime_timeSystemTest_9 - Ensure resetting a Time System changes it
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2));

		return testFramework.countFails();
	}
	/* Test for the formatted printing of YDSTime objects */
	int printfTest (void)
	{
		TestUtil testFramework( "YDSTime", "printf", __FILE__, __func__ );
		testFramework.init();

		YDSTime GPS1(2008,2,1,TimeSystem::GPS);
		YDSTime UTC1(2008,2,1,TimeSystem::UTC);
		
//--------------YDSTime_printfTest_1 - Verify printed output matches expectation		
  		testFramework.assert(GPS1.printf("%04Y %02y %03j %02s %02P") == (std::string)"2008 08 002 1.000000 GPS");
  		testFramework.next();

//--------------YDSTime_printfTest_2 - Verify printed output matches expectation
  		testFramework.assert(UTC1.printf("%04Y %02y %03j %02s %02P") == (std::string)"2008 08 002 1.000000 UTC");
  		testFramework.next();

//--------------YDSTime_printfTest_3 - Verify printed error message matches expectation
  		testFramework.assert(GPS1.printError("%04Y %02y %03j %02s %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime"); 
  		testFramework.next();

//--------------YDSTime_printfTest_4 - Verify printed error message matches expectation
  		testFramework.assert(UTC1.printError("%04Y %02y %03j %02s %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime");
		testFramework.next();

		return testFramework.countFails();
	}
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	YDSTime_T testClass;

	check = testClass.initializationTest();
	errorCounter += check;

	check = testClass.operatorTest();
    errorCounter += check;

	check = testClass.setFromInfoTest();
    errorCounter += check;

	check = testClass.resetTest();
    errorCounter += check;

	check = testClass.timeSystemTest();
    errorCounter += check;

	check = testClass.toFromCommonTimeTest();
    errorCounter += check;

	check = testClass.printfTest();
    errorCounter += check;
	
	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
