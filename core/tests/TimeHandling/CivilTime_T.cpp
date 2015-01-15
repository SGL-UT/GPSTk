#include "CivilTime.hpp"
#include "TimeSystem.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class CivilTime_T
{
	public:
	CivilTime_T(){eps = 1e-12;}// Default Constructor, set the precision value
	~CivilTime_T() {} // Default Desructor

	public:

	/* Test to ensure the values in the constructor go to their intended locations */
	int  initializationTest (void)
	{
		TestUtil testFramework( "CivilTime", "Constructor(time,TimeSystem)", __FILE__, __func__ );
	  	CivilTime Compare(2008,8,21,13,30,15.,TimeSystem::GPS); //Initialize an object

//--------------CivilTime_initializationTest_1 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();		

//--------------CivilTime_initializationTest_2 - Was the year value set to expectation?
		testFramework.assert(2008 == (int)Compare.year);
		testFramework.next();

//--------------CivilTime_initializationTest_3 - Was the month value set to expectation?
		testFramework.assert(8 == (int)Compare.month);
		testFramework.next();

//--------------CivilTime_initializationTest_4 - Was the day value set to expectation?
		testFramework.assert(21 == (int)Compare.day);
		testFramework.next();

//--------------CivilTime_initializationTest_5 - Was the hour value set to expectation?
		testFramework.assert(13 == (int)Compare.hour);
		testFramework.next();

//--------------CivilTime_initializationTest_6 - Was the minute value set to expectation?
		testFramework.assert(30 == (int)Compare.minute); 
		testFramework.next();

//--------------CivilTime_initializationTest_7 - Was the second value set to expectation?
		testFramework.assert(15 == (double)Compare.second);
		testFramework.next(); 


		testFramework.changeSourceMethod("Constructor(CivilTime)");
		CivilTime Copy(Compare); // Initialize with copy constructor
//--------------CivilTime_initializationTest_8 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem());
		testFramework.next();

//--------------CivilTime_initializationTest_9 - Was the year value set to expectation?
		testFramework.assert(2008 == (int)Copy.year);
		testFramework.next();

//--------------CivilTime_initializationTest_10 - Was the month value set to expectation?
		testFramework.assert(8 == (int)Copy.month);
		testFramework.next();

//--------------CivilTime_initializationTest_11 - Was the day value set to expectation?
		testFramework.assert(21 == (int)Copy.day);
		testFramework.next();

//--------------CivilTime_initializationTest_12 - Was the hour value set to expectation?
		testFramework.assert(13 == (int)Copy.hour);
		testFramework.next();

//--------------CivilTime_initializationTest_13 - Was the minute value set to expectation?
		testFramework.assert(30 == (int)Copy.minute); 
		testFramework.next();

//--------------CivilTime_initializationTest_14 - Was the second value set to expectation?
		testFramework.assert(15 == (double)Copy.second); 
		testFramework.next();


		CivilTime Assigned;
		Assigned = Compare;
		testFramework.changeSourceMethod("= Operator");
//--------------CivilTime_initializationTest_15 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem());
		testFramework.next();

//--------------CivilTime_initializationTest_16 - Was the year value set to expectation?
		testFramework.assert(2008 == (int)Assigned.year);
		testFramework.next();

//--------------CivilTime_initializationTest_17 - Was the month value set to expectation?
		testFramework.assert(8 == (int)Assigned.month);
		testFramework.next();

//--------------CivilTime_initializationTest_18 - Was the day value set to expectation?
		testFramework.assert(21 == (int)Assigned.day);
		testFramework.next();

//--------------CivilTime_initializationTest_19 - Was the hour value set to expectation?
		testFramework.assert(13 == (int)Assigned.hour);
		testFramework.next();

//--------------CivilTime_initializationTest_20 - Was the minute value set to expectation?
		testFramework.assert(30 == (int)Assigned.minute); 
		testFramework.next();

//--------------CivilTime_initializationTest_21 - Was the second value set to expectation?
		testFramework.assert(15 == (double)Assigned.second); 
		testFramework.next();
		
		return testFramework.countFails();
	}

	// Test will check if CivilTime variable can be set from a map.
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "CivilTime", "setFromInfo", __FILE__, __func__ );

		//Create several objects for testing
		CivilTime setFromInfo1;
		CivilTime setFromInfo2;
		CivilTime setFromInfo3;
		CivilTime setFromInfo4;
		CivilTime setFromInfo5;

		//Set several values to set the objects
		TimeTag::IdToValue Id;
		Id['b'] = "Dec";
		Id['d'] = "31";
		Id['Y'] = "2008";
		Id['H'] = "12";
		Id['M'] = "00";
		Id['S'] = "00";
		Id['P'] = "GPS";

//--------------CivilTime_setFromInfoTest_1 - Can a CivilTime object be set with b,d,Y,H,M,S,P options?
		testFramework.assert(setFromInfo1.setFromInfo(Id));
		testFramework.next();

    	CivilTime Check(2008,12,31,12,0,0,TimeSystem::GPS);
//--------------CivilTime_setFromInfoTest_2 - Is the set object what is expected?
		testFramework.assert(setFromInfo1 == Check); 
		testFramework.next();

		Id.erase('b');
		Id.erase('Y');
		Id['m'] = "12";
		Id['y'] = "06";

//--------------CivilTime_setFromInfoTest_3 - Can a CivilTime object be set with d,m,y,H,M,S,P options?
		testFramework.assert(setFromInfo2.setFromInfo(Id));
		testFramework.next();
    	
    	CivilTime Check2(2006,12,31,12,0,0,TimeSystem::GPS);

//--------------CivilTime_setFromInfoTest_4 - Is the set object what is expected?
    	testFramework.assert(setFromInfo2 == Check2); 
		testFramework.next();
		
		Id.erase('y');
		Id['y'] = "006";

//--------------CivilTime_setFromInfoTest_5 - Can a CivilTime object be set with a 3 digit year? Answer should be no. 'y' option is for 2 digit years.
		testFramework.assert(setFromInfo3.setFromInfo(Id));
		testFramework.next();
//--------------CivilTime_setFromInfoTest_6 - Is the set object what is expected?
    	testFramework.assert(setFromInfo3 != Check2); 
		testFramework.next();

		Id.erase('y');
//--------------CivilTime_setFromInfoTest_7 - Can a CivilTime object be set without a year?
		testFramework.assert(setFromInfo4.setFromInfo(Id));
		testFramework.next();

		Id.erase('m');
		Id['b'] = "AAA";
//--------------CivilTime_setFromInfoTest_8 - Can a CivilTime object be set with an improper month?
		testFramework.assert(!(setFromInfo5.setFromInfo(Id)));
		testFramework.next();

		return testFramework.countFails();
	}

	/* Test will check if the ways to initialize and set an CivilTime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil testFramework( "CivilTime", "== Operator", __FILE__, __func__ );

		CivilTime Aug21(2008,8,21,13,30,15.); //Reference time
		// Series of conditions less than Aug21 above
		CivilTime LessThanYear(2005,8,21,13,30,15.);
		CivilTime LessThanMonth(2008,7,21,13,30,15.);
		CivilTime LessThanDay(2008,8,20,13,30,15.);
		CivilTime LessThanHour(2008,8,21,12,30,15.);
		CivilTime LessThanMinute(2008,8,21,13,20,15.);
		CivilTime LessThanSecond(2008,8,21,13,30,0.);

		CivilTime Aug21Copy(Aug21); //Use copy constructor

		CivilTime Aug21Copy2 = Aug21Copy; //Use the = set operator.
		Aug21Copy2 = Aug21Copy;
//--------------CivilTime_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Aug21 == Aug21Copy);
		testFramework.next();

		testFramework.changeSourceMethod("!= Operator");
//--------------CivilTime_operatorTest_2 - Are non-equivalent objects equivalent?
		testFramework.assert(Aug21 != LessThanYear);
		testFramework.next();

		testFramework.changeSourceMethod("< Operator");
//--------------CivilTime_operatorTest_3 - Does the < operator function when left_year < right_year?
		testFramework.assert(LessThanYear < Aug21);
		testFramework.next();

//--------------CivilTime_operatorTest_4 - Does the < operator function when left_year > right_year?
		testFramework.assert(!(Aug21 < LessThanYear));
		testFramework.next();

//--------------CivilTime_operatorTest_5 - Does the < operator function when left_month < right_month?
		testFramework.assert(LessThanMonth < Aug21);
		testFramework.next();

//--------------CivilTime_operatorTest_6 - Does the < operator function when left_month > right_month?
		testFramework.assert(!(Aug21 < LessThanMonth));
		testFramework.next();

//--------------CivilTime_operatorTest_7 - Does the < operator function when left_day < right_day?
		testFramework.assert(LessThanDay < Aug21);
		testFramework.next();
		
//--------------CivilTime_operatorTest_8 - Does the < operator function when left_day > right_day?
		testFramework.assert(!(Aug21 < LessThanDay));
		testFramework.next();
		
//--------------CivilTime_operatorTest_9 - Does the < operator function when left_hour < right_hour?
		testFramework.assert(LessThanHour < Aug21);
		testFramework.next();
		
//--------------CivilTime_operatorTest_10 - Does the < operator function when left_hour > right_hour?
		testFramework.assert(!(Aug21 < LessThanHour));
		testFramework.next();
		
//--------------CivilTime_operatorTest_11 - Does the < operator function when left_minute < right_minute?
		testFramework.assert(LessThanMinute < Aug21);
		testFramework.next();
		
//--------------CivilTime_operatorTest_12 - Does the < operator function when left_minute > right_minute?
		testFramework.assert(!(Aug21 < LessThanMinute));
		testFramework.next();
		
//--------------CivilTime_operatorTest_13 - Does the < operator function when left_second < right_second?
		testFramework.assert(LessThanSecond < Aug21);
		testFramework.next();
		
//--------------CivilTime_operatorTest_14 - Does the < operator function when left_second > right_second?
		testFramework.assert(!(Aug21 < LessThanSecond));
		testFramework.next();

		testFramework.changeSourceMethod("> Operator");
//--------------CivilTime_operatorTest_15 - Does the > operator function when left_year > right_year?
		testFramework.assert(Aug21 > LessThanYear);
		testFramework.next();

		testFramework.changeSourceMethod("<= Operator");
//--------------CivilTime_operatorTest_16 - Does the <= operator function when left_year < right_year?
		testFramework.assert(LessThanYear <= Aug21);
		testFramework.next();

//--------------CivilTime_operatorTest_17 - Does the <= operator function when left_year = right_year?
		testFramework.assert(Aug21Copy <= Aug21);
		testFramework.next();

		testFramework.changeSourceMethod(">= Operator");
//--------------CivilTime_operatorTest_18 - Does the >= operator function when left_year > right_year?
		testFramework.assert(Aug21 >= LessThanYear);
		testFramework.next();

//--------------CivilTime_operatorTest_19 - Does the >= operator function when left_year = right_year?
		testFramework.assert(Aug21 >= Aug21Copy);
		testFramework.next();

		return testFramework.countFails();
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
  		TestUtil testFramework( "CivilTime", "isValid", __FILE__, __func__ );

  		CivilTime Aug21(2008,8,21,13,30,15.,TimeSystem::GPS);

//--------------CivilTime_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		testFramework.assert(Aug21.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testFramework.next();

//--------------CivilTime_toFromCommonTimeTest_2 - Is the set object valid?
		testFramework.assert(Aug21.isValid());
		testFramework.next();
  		
  		CommonTime Test = Aug21.convertToCommonTime();

  		CivilTime Test2;
  		Test2.convertFromCommonTime(Test);

		testFramework.changeSourceMethod("CommonTime Conversion");
//--------------CivilTime_toFromCommonTimeTest_3 - Is the result of conversion the same?
		testFramework.assert(Test2 == Aug21);
		testFramework.next();

//--------------CivilTime_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		testFramework.assert(Aug21.getTimeSystem()==TimeSystem(2));
		testFramework.next();

//--------------CivilTime_toFromCommonTimeTest_5 - Is the year after conversion what is expected?
  		testFramework.assert(2008 == (int)Aug21.year);
		testFramework.next();
//--------------CivilTime_toFromCommonTimeTest_6 - Is the month after conversion what is expected?
  		testFramework.assert(8 == (int)Aug21.month);
		testFramework.next();

//--------------CivilTime_toFromCommonTimeTest_7 - Is the day after conversion what is expected?
  		testFramework.assert(21 == (int)Aug21.day);
		testFramework.next();

//--------------CivilTime_toFromCommonTimeTest_8 - Is the hour after conversion what is expected?
  		testFramework.assert(13 == (int)Aug21.hour);
		testFramework.next();

//--------------CivilTime_toFromCommonTimeTest_9 - Is the minute after conversion what is expected?
  		testFramework.assert(30 == (int)Aug21.minute);
		testFramework.next();

//--------------CivilTime_toFromCommonTimeTest_10 - Is the second after conversion what is expected?
  		testFramework.assert(15 == (int)Aug21.second);		
		testFramework.next();

		return testFramework.countFails();
	}



	/* Test will check the reset method. */
	int resetTest (void)
	{
		TestUtil testFramework( "CivilTime", "reset", __FILE__, __func__ );
		//Initialize a time
  		CivilTime Aug21(2008,8,21,13,30,15.,TimeSystem::GPS);

  		Aug21.reset();
//--------------CivilTime_operatorTest_1 - Was the time system reset to expectation?
  		testFramework.assert(Aug21.getTimeSystem()==TimeSystem(0));
		testFramework.next();

//--------------CivilTime_operatorTest_2 - Was the year reset to expectation?
  		testFramework.assert(0 == (int)Aug21.year);
		testFramework.next();

//--------------CivilTime_operatorTest_3 - Was the month reset to expectation?
  		testFramework.assert(1 == (int)Aug21.month);
		testFramework.next();

//--------------CivilTime_operatorTest_4 - Was the day reset to expectation?
  		testFramework.assert(1 == (int)Aug21.day);
		testFramework.next();

//--------------CivilTime_operatorTest_5 - Was the hour reset to expectation?
  		testFramework.assert(0 == (int)Aug21.hour);
		testFramework.next();

//--------------CivilTime_operatorTest_6 - Was the minute reset to expectation?
  		testFramework.assert(0 == (int)Aug21.minute);
		testFramework.next();

//--------------CivilTime_operatorTest_7 - Was the second reset to expectation?
  		testFramework.assert(0 == (int)Aug21.second);
		testFramework.next();


		return testFramework.countFails();
	}

	/* Test to check the TimeSystem comparisons when using the comparison operators. */
	int timeSystemTest (void)
	{
  		TestUtil testFramework( "CivilTime", "Differing TimeSystem == Operator", __FILE__, __func__ );

  		CivilTime GPS1(   2008,8,21,13,30,15.,TimeSystem::GPS);
  		CivilTime GPS2(   2005,8,21,13,30,15.,TimeSystem::GPS); // Time is less than the others
  		CivilTime UTC1(   2008,8,21,13,30,15.,TimeSystem::UTC);
  		CivilTime UNKNOWN(2008,8,21,13,30,15.,TimeSystem::Unknown);
  		CivilTime ANY(    2008,8,21,13,30,15.,TimeSystem::Any);

//--------------CivilTime_timeSystemTest_1 - Verify same Time System but different time inequality
		testFramework.assert(!(GPS1 == GPS2));
		testFramework.next();

//--------------CivilTime_timeSystemTest_2 - Verify same Time System equality
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem());
		testFramework.next();


		testFramework.changeSourceMethod("Differing TimeSystem != Operator");
//--------------CivilTime_timeSystemTest_3 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UTC1);
		testFramework.next();


//--------------CivilTime_timeSystemTest_4 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);
		testFramework.next();

		
		testFramework.changeSourceMethod("ANY TimeSystem == Operator");		
//--------------CivilTime_timeSystemTest_5 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		testFramework.assert(GPS1 == ANY);
		testFramework.next();

//--------------CivilTime_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		testFramework.assert(UTC1 == ANY);
		testFramework.next();

//--------------CivilTime_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		testFramework.assert(UNKNOWN == ANY);
		testFramework.next();


		testFramework.changeSourceMethod("ANY TimeSystem < Operator");	
//--------------CivilTime_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY));
		testFramework.next();


		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
//--------------CivilTime_timeSystemTest_9 - Ensure resetting a Time System changes it
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2));
		testFramework.next();


		return testFramework.countFails();

	}

	int printfTest (void)
	{
		TestUtil testFramework( "CivilTime", "printf", __FILE__, __func__ );

  		CivilTime GPS1(2008,8,21,13,30,15.,TimeSystem::GPS);
  		CivilTime UTC1(2008,8,21,13,30,15.,TimeSystem::UTC);

//--------------CivilTime_printfTest_1 - Verify printed output matches expectation
		testFramework.assert(GPS1.printf("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") ==
                       (std::string)"2008 08 08 Aug 21 13 30 15 15.000000 GPS");
		testFramework.next();

//--------------CivilTime_printfTest_2 - Verify printed output matches expectation
		testFramework.assert(UTC1.printf("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") ==
                       (std::string)"2008 08 08 Aug 21 13 30 15 15.000000 UTC");
		testFramework.next();

		testFramework.changeSourceMethod("printError");	
//--------------CivilTime_printfTest_3 - Verify printed error message matches expectation
		testFramework.assert(GPS1.printError("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") ==
                       (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime");
		testFramework.next();

//--------------CivilTime_printfTest_4 - Verify printed error message matches expectation
		testFramework.assert(UTC1.printError("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") ==
                       (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime");
		testFramework.next();
		
		return testFramework.countFails();
	}

	private:
		double eps;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	CivilTime_T testClass;

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
	
	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
