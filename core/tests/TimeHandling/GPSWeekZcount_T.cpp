//demo rewrite of GPSWeekZCount_T
//formatting to ANSITime_T standard
#include "GPSWeekZcount.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace gpstk;
using namespace std;

class GPSWeekZcount_T
{
	public:
		GPSWeekZcount_T(){eps = 1e-11;}// Default Constructor, set the precision value
		~GPSWeekZcount_T() {} // Default Desructor

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//Include initialization test here 
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	/* Test to ensure the values in the constructor go to their intended locations */
	int initializationTest(void)
	{
		TestUtil testFramework( "GPSWeekZcount", "Constructor(week,zcount,TimeSystem)", __FILE__, __func__ );
		testFramework.init();

		GPSWeekZcount Compare(1300,13500.,TimeSystem(2)); //Initialize an object
//--------------GPSWeekZCountTime_initializationTest_1 - Was the week value set to expectation?
		testFramework.assert(1300 == Compare.week);
		testFramework.next();

//--------------GPSWeekZCountTime_initializationTest_2 - Was the zcount value set to expectation?
		testFramework.assert(13500 - Compare.zcount < eps && Compare.zcount - 13500 < eps);
		testFramework.next();

//--------------GPSWeekZCountTime_initializationTest_3 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Constructor(GPSWeekZcount)");
		GPSWeekZcount Copy(Compare); // Initialize with copy constructor

//--------------GPSWeekZCountTime_initializationTest_4 - Was the week value set to expectation?
		testFramework.assert(1300 == Compare.week);
		testFramework.next();

//--------------GPSWeekZCountTime_initializationTest_5 - Was the zcount value set to expectation?
		testFramework.assert(13500 == Compare.zcount);
		testFramework.next();

//--------------GPSWeekZCountTime_initializationTest_6 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("= Operator");
		GPSWeekZcount Assigned;
		Assigned = Compare;

//--------------GPSWeekZCountTime_initializationTest_7 - Was the week value set to expectation?
		testFramework.assert(1300 == Compare.week);
		testFramework.next();

//--------------GPSWeekZCountTime_initializationTest_8 - Was the zcount value set to expectation?
		testFramework.assert(13500 == Compare.zcount);
		testFramework.next();

//--------------GPSWeekZCountTime_initializationTest_9 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		return testFramework.countFails();

	}


	/* Test will check if GPSWeekZcount variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//Possible error due to float?
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

		TestUtil testFramework( "GPSWeekZcount", "setFromInfo", __FILE__, __func__ );
		testFramework.init();

		GPSWeekZcount setFromInfo1;
		GPSWeekZcount setFromInfo2;
		GPSWeekZcount setFromInfo3;
		TimeTag::IdToValue Id;
		Id['F'] = "1300";
		Id['z'] = "13500";
		Id['P'] = "GPS";

//--------------GPSWeekZcount_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		testFramework.assert(setFromInfo1.setFromInfo(Id));
		testFramework.next();

		GPSWeekZcount Compare(1300,13500.,TimeSystem(2)); //Initialize an object

//--------------GPSWeekZcount_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo1);
		testFramework.next();

		Id.erase('z');
		Id['w'] = "3";

//--------------GPSWeekZcount_setFromInfoTest_3 - Does a proper setFromInfo work with different data type?
		testFramework.assert(setFromInfo2.setFromInfo(Id));
		testFramework.next();

		GPSWeekZcount Compare2(1300,3*57600L,TimeSystem(2));

//--------------GPSWeekZcount_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo2);
		testFramework.next();

		Id.erase('F');

//--------------GPSWeekZcount_setFromInfoTest_3 - Does a proper setFromInfo work with missing information?
		testFramework.assert(setFromInfo3.setFromInfo(Id));
		testFramework.next();

		GPSWeekZcount Compare3(0,3*57600L,TimeSystem(2));

//--------------GPSWeekZCount_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare3 == setFromInfo3);

		return testFramework.countFails();
	}

	/* Test will check if the ways to initialize and set an GPSWeekZcount object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil testFramework( "GPSWeekZCount", "== Operator", __FILE__, __func__ );
		testFramework.init();

		GPSWeekZcount Compare(1300,13500);
		GPSWeekZcount LessThanWeek(1299,13500);
		GPSWeekZcount LessThanZcount(1300,13400);
		GPSWeekZcount CompareCopy(Compare); // Initialize with copy constructor
		GPSWeekZcount CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Why have CompareCopy2? Never called. Also, LessThanZCount was only called once
//	in the original test. Shouldn't it be called more for more rigorous testing?
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//--------------GPSWeekZCount_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare == CompareCopy);
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_2 - Are equivalent objects equivalent?
		testFramework.assert(!(Compare == LessThanWeek));
		testFramework.next();

		testFramework.changeSourceMethod("!= Operator");
//--------------GPSWeekZCount_operatorTest_3 - Are non-equivalent objects not equivalent?
		testFramework.assert(Compare != LessThanWeek);
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_4 - Are equivalent objects not equivalent?
		testFramework.assert(!(Compare != Compare));
		testFramework.next();

		testFramework.changeSourceMethod("< Operator");
//--------------GPSWeekZCount_operatorTest_5 - Does the < operator function when left_object < right_object?
		testFramework.assert(LessThanWeek < Compare);
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_6 - Does the < operator function when left_object > right_object?
		testFramework.assert(!(Compare < LessThanWeek));
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_7 - Does the Zcount cause left_object > right_object?
		testFramework.assert(Compare > LessThanZcount);

//--------------GPSWeekZCount_operatorTest_8 - Does the < operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("> Operator");
//--------------GPSWeekZCount_operatorTest_9 - Does the > operator function when left_object < right_object?
		testFramework.assert(!(LessThanWeek > Compare));
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_10 - Does the > operator function when left_object > right_object?
		testFramework.assert(Compare > LessThanWeek);
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_11 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare > CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("<= Operator");
//--------------GPSWeekZCount_operatorTest_12 - Does the <= operator function when left_object < right_object?
		testFramework.assert(LessThanWeek <= Compare);
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_13 - Does the <= operator function when left_object > right_object?
		testFramework.assert(!(Compare <= LessThanWeek));
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_14 - Does the <= operator function when left_object = right_object?
		testFramework.assert(Compare <= CompareCopy);
		testFramework.next();

		testFramework.changeSourceMethod(">= Operator");
//--------------GPSWeekZCount_operatorTest_15 - Does the >= operator function when left_object < right_object?
		testFramework.assert(!(LessThanWeek >= Compare));
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_16 - Does the >= operator function when left_object > right_object?
		testFramework.assert(Compare >= LessThanWeek);
		testFramework.next();

//--------------GPSWeekZCount_operatorTest_17 - Does the >= operator function when left_object = right_object?
		testFramework.assert(Compare >= CompareCopy);

		return testFramework.countFails();
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Tests 1-3 seem redundant to setFromInfoTests
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

		TestUtil testFramework( "GPSWeekZcount", "reset", __FILE__, __func__ );
		testFramework.init();

		GPSWeekZcount Compare(1300,13500.,TimeSystem::GPS); //Initialize an object

//--------------GPSWeekZCount_resetTest_1 - Was the week value set to expectation?
		testFramework.assert(1300 == Compare.week);
		testFramework.next();

//--------------GPSWeekZCount_resetTest_2 - Was the zcount value set to expectation?
		testFramework.assert(13500 == Compare.zcount);
		testFramework.next();

//--------------GPSWeekZCount_resetTest_3 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

	  	Compare.reset(); // Reset it

//--------------GPSWeekZCount_resetTest_4 - Was the week value reset to expectation?
	  	testFramework.assert(0 == (int)Compare.week);
	  	testFramework.next();

//--------------GPSWeekZCount_resetTest_5 - Was the zcount value reset to expectation?
	  	testFramework.assert(0 == (int)Compare.zcount);
	  	testFramework.next();

//--------------GPSWeekZCount_resetTest_6 - Was the time system reset to expectation?
		/*GPSWeekZcount resets to GPS TimeSystem!!!!*/
	  	testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());

		return testFramework.countFails();
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "GPSWeekZcount", "isValid", __FILE__, __func__ );
		testFramework.init();

	  	GPSWeekZcount Compare(1300,13500.,TimeSystem(2)); //Initialize an object

//--------------GPSWeekZCount_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testFramework.next();

//--------------GPSWeekZCount_toFromCommonTimeTest_2 - Is the set object valid?
		testFramework.assert(Compare.isValid());
		testFramework.next();

  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		GPSWeekZcount Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

		testFramework.changeSourceMethod("CommonTime Conversion");
//--------------GPSWeekZCount_toFromCommonTimeTest_3 - Is the result of conversion the same?
		testFramework.assert(Test2 == Compare);
		testFramework.next();

//--------------GPSWeekZCount_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		testFramework.assert(Compare.getTimeSystem()==TimeSystem(2));
		testFramework.next();

//--------------GPSWeekZCount_toFromCommonTimeTest_5 - Is the week after conversion what is expected?
		testFramework.assert(Compare.week==1300);
		testFramework.next();

//--------------GPSWeekZCount_toFromCommonTimeTest_6 - Is the zcount after conversion what is expected?
		testFramework.assert(Compare.zcount==13500);
		testFramework.next();
		return testFramework.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{

		TestUtil testFramework( "GPSWeekZcount", "Differing TimeSystem == Operator", __FILE__, __func__ );
		testFramework.init();

  		GPSWeekZcount GPS1(1300,13500.,TimeSystem(2));
  		GPSWeekZcount GPS2(1200,13500.,TimeSystem(2));
  		GPSWeekZcount UTC1(1300,13500.,TimeSystem(5));
  		GPSWeekZcount UNKNOWN(1300,13500.,TimeSystem(0));
  		GPSWeekZcount ANY(1300,13500.,TimeSystem(1));
//--------------GPSWeekZCount_timeSystemTest_1 - Verify same Time System but different time inequality
		testFramework.assert(!(GPS1 == GPS2));
		testFramework.next();

//--------------GPSWeekZCount_timeSystemTest_2 - Verify same Time System equality
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Differing TimeSystem != Operator");
//--------------GPSWeekZCount_timeSystemTest_3 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UTC1);
		testFramework.next();

//--------------GPSWeekZCount_timeSystemTest_4 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem == Operator");		
//--------------GPSWeekZCount_timeSystemTest_5 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		testFramework.assert(GPS1 == ANY);
		testFramework.next();

//--------------GPSWeekZCount_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		testFramework.assert(UTC1 == ANY);
		testFramework.next();

//--------------GPSWeekZCount_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		testFramework.assert(UNKNOWN == ANY);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem < Operator");	
//--------------GPSWeekZCount_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY));
		testFramework.next();

		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
//--------------GPSWeekZCount_timeSystemTest_9 - Ensure resetting a Time System changes it
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2));

		return testFramework.countFails();
	}
	/* Test for the formatted printing of GPSWeekZcount objects */
	int  printfTest (void)
	{
		TestUtil testFramework( "GPSWeekZCount", "printf", __FILE__, __func__ );
		testFramework.init();

  		GPSWeekZcount GPS1(1300,13500.,TimeSystem(2));
  		GPSWeekZcount UTC1(1300,13500.,TimeSystem(7));

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//Error with "1350000" compared to 1350000?
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//--------------GPSWeekZCount_printfTest_1 - Verify printed output matches expectation
		testFramework.assert(GPS1.printf("%04F %05z %02P") == (std::string)"1300 13500 GPS");
		testFramework.next();

//--------------GPSWeekZCount_printfTest_2 - Verify printed output matches expectation
		testFramework.assert(UTC1.printf("%04F %05z %02P") == (std::string)"1300 13500 UTC");
		testFramework.next();

		testFramework.changeSourceMethod("printError");	
//--------------GPSWeekZCount_printfTest_3 - Verify printed error message matches expectation
		testFramework.assert(GPS1.printError("%04F %05z %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");
		testFramework.next();

//--------------GPSWeekZCount_printfTest_4 - Verify printed error message matches expectation
		testFramework.assert(UTC1.printError("%04F %05z %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");

		return testFramework.countFails();
	}
	private:
		double eps;
};


int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	GPSWeekZcount_T testClass;

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

	return errorCounter; //Return the total number of errors
}
