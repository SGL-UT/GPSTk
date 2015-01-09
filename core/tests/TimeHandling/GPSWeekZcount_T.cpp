#include "GPSWeekZcount.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class GPSWeekZcount_T
{
	public:
		GPSWeekZcount_T(){eps = 1e-11;}// Default Constructor, set the precision value
		~GPSWeekZcount_T() {} // Default Desructor


	/* Test will check if GPSWeekZcount variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{
		GPSWeekZcount setFromInfo1;
		GPSWeekZcount setFromInfo2;
		GPSWeekZcount setFromInfo3;
	
		TimeTag::IdToValue Id;
		Id['F'] = "1300";
		Id['z'] = "13500";
		Id['P'] = "GPS";
		if(!setFromInfo1.setFromInfo(Id)) return 1;
		GPSWeekZcount Compare(1300,13500.,TimeSystem(2));
		if (Compare != setFromInfo1) return 2;

		Id.erase('z');
		Id['w'] = "3";
		if (!setFromInfo2.setFromInfo(Id)) return 3;
		GPSWeekZcount Compare2(1300,3*57600L,TimeSystem(2));
		if (Compare2 != setFromInfo2) return 4;

		Id.erase('F');
		if (!setFromInfo3.setFromInfo(Id)) return 5;
		GPSWeekZcount Compare3(0,3*57600L,TimeSystem(2));
		if (Compare3 != setFromInfo3) return 6;
		return 0;
	}

	/* Test will check if the ways to initialize and set an GPSWeekZcount object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{

		GPSWeekZcount Compare(1300,13500);
		GPSWeekZcount LessThanWeek(1299,13500);
		GPSWeekZcount LessThanZcount(1300,13400);
		GPSWeekZcount CompareCopy(Compare); // Initialize with copy constructor
		GPSWeekZcount CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

		//Equality Assertion
		if (!(Compare == CompareCopy)) return 1;
		//Non-equality Assertion
		if (!(Compare != LessThanWeek)) return 2;
		//Less than assertions
		if (!(LessThanWeek < Compare)) return 3;
		if (Compare < LessThanWeek) return 4;
		if (!(LessThanZcount < Compare)) return 5;
		if (Compare < LessThanZcount) return 6;
		//Greater than assertions
		if(!(Compare > LessThanWeek)) return 7;
		//Less than equals assertion
		if (!(LessThanWeek <= Compare)) return 8;
		if(!(CompareCopy <= Compare)) return 9;
		//Greater than equals assertion
		if(!(Compare >= LessThanWeek)) return 10;
		if(!(Compare >= CompareCopy)) return 11;
		//Validity check
		if(!(Compare.isValid())) return 12;
		return 0;
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{
	
		GPSWeekZcount Compare(1300,13500.,TimeSystem::GPS); //Initialize an object

		if (1300 != Compare.week) return 1;
		if (13500 != Compare.zcount) return 2;
		if (TimeSystem(2) != Compare.getTimeSystem()) return 3;

	  	Compare.reset(); // Reset it

		/*GPSWeekZcount resets to GPS TimeSystem!!!!*/
	  	if (TimeSystem(2) != Compare.getTimeSystem()) return 4; 
	  	if (0 != (int)Compare.week) return 5;
	  	if (0 != (int)Compare.zcount) return 6;
		return 0;
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
	  	GPSWeekZcount Compare(1300,13500.,TimeSystem(2)); //Initialize an object
  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		GPSWeekZcount Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

  		if (!(Test2 == Compare)) return 1; // Converting to then from yields original

  		if (!(Compare.getTimeSystem()==TimeSystem(2))) return 2; // Recheck TimeSystem
  		if (13500 != (int)Test2.zcount) return 3; // Recheck value
		if (1300 != Test2.week) return 4;
		return 0;
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{

  		GPSWeekZcount GPS1(1300,13500.,TimeSystem(2));
  		GPSWeekZcount GPS2(1200,13500.,TimeSystem(2));
  		GPSWeekZcount UTC1(1300,13500.,TimeSystem(5));
  		GPSWeekZcount UNKNOWN(1300,13500.,TimeSystem(0));
  		GPSWeekZcount ANY(1300,13500.,TimeSystem(1));

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
	/* Test for the formatted printing of GPSWeekZcount objects */
	int  printfTest (void)
	{

  		GPSWeekZcount GPS1(1300,13500.,TimeSystem(2));
  		GPSWeekZcount UTC1(1300,13500.,TimeSystem(7));

  		if (GPS1.printf("%04F %05z %02P") != (std::string)"1300 13500 GPS") return 1;
  		if (UTC1.printf("%04F %05z %02P") != (std::string)"1300 13500 UTC") return 2;
  		if (GPS1.printError("%04F %05z %02P") != (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime") return 3; 
  		if (UTC1.printError("%04F %05z %02P") != (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime") return 4;
		return 0;
	}
	private:
		double eps;
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
	GPSWeekZcount_T testClass;
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
