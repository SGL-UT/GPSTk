#include "CivilTime.hpp"
#include "TimeSystem.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class xCivilTime
{
	public:
	xCivilTime(){eps = 1e-12;}// Default Constructor, set the precision value
	~xCivilTime() {} // Default Desructor
	double eps;
	public:

	// Test will check if CivilTime variable can be set from a map.
	int setFromInfoTest (void)
	{
		//Create several objects for testing
		CivilTime setFromInfo1;
		CivilTime setFromInfo2;
		CivilTime setFromInfo3;
		CivilTime setFromInfo4;
		CivilTime setFromInfo5;

		//Set several values to set the objects
		TimeTag::IdToValue Id;
		Id.insert(make_pair('b',"Dec"));
		Id.insert(make_pair('d',"31"));
		Id.insert(make_pair('Y',"2008"));
		Id.insert(make_pair('H',"12"));
		Id.insert(make_pair('M',"00"));
		Id.insert(make_pair('S',"00"));
		Id.insert(make_pair('P',"GPS"));

		//Can a CivilTime object be set with b,d,Y,H,M,S,P options?
		if (!(setFromInfo1.setFromInfo(Id))) return 1;

		//Is the set object what is expected?
    		CivilTime Check(2008,12,31,12,0,0,TimeSystem::GPS);
    		if (setFromInfo1 != Check) return 2; 

		Id.erase('b');
		Id.erase('Y');
		Id.insert(make_pair('m',"12"));
		Id.insert(make_pair('y',"06"));

		//Can a CivilTime object be set with d,m,y,H,M,S,P options?
		if (!(setFromInfo2.setFromInfo(Id))) return 2;
    		CivilTime Check2(2006,12,31,12,0,0,TimeSystem::GPS);
    		if (setFromInfo2 != Check2) return 3; 
		Id.erase('y');
		Id.insert(make_pair('y',"006"));

		//Can a CivilTime object be set with a 3 digit year?
		// Answer should be no. 'y' option is for 2 digit years.
		if (!(setFromInfo3.setFromInfo(Id))) return 4;
    		if (setFromInfo3 == Check2) return 5; 
		//Can a CivilTime object be set without a year?
		Id.erase('y');
		if (!(setFromInfo4.setFromInfo(Id))) return 6;
		Id.erase('m');
		Id.insert(make_pair('b',"AAA"));
		//Can a CivilTime object be set with an improper month?
		if (setFromInfo5.setFromInfo(Id)) return 7;

		return 0;
	}

	/* Test will check if the ways to initialize and set an CivilTime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		CivilTime Zero; //Use default constructor

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
		//Equality Assertion
		if (!(Aug21 == Aug21Copy)) return 1;
		//Non-equality Assertion
		if (!(Aug21 != LessThanYear)) return 2;
		//Less than assertions
		if (!(LessThanYear < Aug21)) return 3;
		if ((Aug21 < LessThanYear)) return 4;
		if (!(LessThanMonth < Aug21)) return 5;
		if ((Aug21 < LessThanMonth)) return 6;
		if (!(LessThanDay < Aug21)) return 7;
		if ((Aug21 < LessThanDay)) return 8;
		if (!(LessThanHour < Aug21)) return 9;
		if ((Aug21 < LessThanHour)) return 10;
		if (!(LessThanMinute < Aug21)) return 11;
		if ((Aug21 < LessThanMinute)) return 12;
		if (!(LessThanSecond < Aug21)) return 13;
		if ((Aug21 < LessThanSecond)) return 14;
		//Greater than assertions
		if (!(Aug21 > LessThanYear)) return 15;
		//Less than equals assertion
		if (!(LessThanYear <= Aug21)) return 16;
		if (!(Aug21Copy <= Aug21)) return 17;
		//Greater than equals assertion
		if (!(Aug21 >= LessThanYear)) return 18;
		if (!(Aug21 >= Aug21Copy)) return 19;
		// isValid assertions
		if (!(Aug21.isValid())) return 20;
		if ((Zero.isValid())) return 21;
		return 0;
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
  		CivilTime Aug21(2008,8,21,13,30,15.,TimeSystem::GPS);

  		CommonTime Test = Aug21.convertToCommonTime();

  		CivilTime Test2;
  		Test2.convertFromCommonTime(Test);

  		if (!(Test2 == Aug21)) return 1; // Converting to then from yields original

  		if (!(Aug21.getTimeSystem()==TimeSystem(2))) return 2; // Recheck TimeSystem
  		if (2008 != (int)Aug21.year) return 3; // Recheck year value
  		if (8 != (int)Aug21.month) return 4; // Recheck month value
  		if (21 != (int)Aug21.day) return 5; // Recheck day value
  		if (13 != (int)Aug21.hour) return 6; // Recheck hour value
  		if (30 != (int)Aug21.minute) return 7; // Recheck minute value
  		if (15 != (int)Aug21.second) return 8; // Recheck second value		
		return 0;
	}



	/* Test will check the reset method. */
	int resetTest (void)
	{
		//Initialize a time
  		CivilTime Aug21(2008,8,21,13,30,15.,TimeSystem::GPS);
  		if (!(Aug21.getTimeSystem()==TimeSystem(2))) return 1; // Check TimeSystem
  		if (2008 != (int)Aug21.year) return 2; // Check year value
  		if (8 != (int)Aug21.month) return 3; // Check month value
  		if (21 != (int)Aug21.day) return 4; // Check day value
  		if (13 != (int)Aug21.hour) return 5; // Check hour value
  		if (30 != (int)Aug21.minute) return 6; // Check minute value
  		if (15 != (int)Aug21.second) return 7; // Check second value	


  		Aug21.reset();
  		if (!(Aug21.getTimeSystem()==TimeSystem(0))) return 8; // Check TimeSystem
  		if (0 != (int)Aug21.year) return 9; // Check year value reset
  		if (1 != (int)Aug21.month) return 10; // Check month value reset
  		if (1 != (int)Aug21.day) return 11; // Check day value reset
  		if (0 != (int)Aug21.hour) return 12; // Check hour value reset
  		if (0 != (int)Aug21.minute) return 13; // Check minute value reset
  		if (0 != (int)Aug21.second) return 14; // Check second value reset	
		return 0;
	}

	/* Test to check the TimeSystem comparisons when using the comparison operators. */
	int timeSystemTest (void)
	{
  		CivilTime GPS1(   2008,8,21,13,30,15.,TimeSystem::GPS);
  		CivilTime GPS2(   2005,8,21,13,30,15.,TimeSystem::GPS); // Time is less than the others
  		CivilTime UTC1(   2008,8,21,13,30,15.,TimeSystem::UTC);
  		CivilTime UNKNOWN(2008,8,21,13,30,15.,TimeSystem::Unknown);
  		CivilTime ANY(    2008,8,21,13,30,15.,TimeSystem::Any);

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

	int printfTest (void)
	{
  		CivilTime GPS1(2008,8,21,13,30,15.,TimeSystem::GPS);
  		CivilTime UTC1(2008,8,21,13,30,15.,TimeSystem::UTC);

  		if (GPS1.printf("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") !=
                       (std::string)"2008 08 08 Aug 21 13 30 15 15.000000 GPS") return 1;
  		if (UTC1.printf("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") !=
                       (std::string)"2008 08 08 Aug 21 13 30 15 15.000000 UTC") return 2;
  		if (GPS1.printError("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") !=
                       (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime") return 3;
  		if (UTC1.printError("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") !=
                       (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime")return 4;
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
	xCivilTime testClass;
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
