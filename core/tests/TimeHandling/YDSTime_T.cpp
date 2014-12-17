#include "YDSTime.hpp"
#include "TimeTag.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class xYDSTime
{
	public:

	// Test is not currently compiling with Solaris compilers. Leaving it commented for now.

	/* Test will check if YDSTime variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	/*int setFromInfoTest (void)
	{
		YDSTime setFromInfo1;
		YDSTime setFromInfo2;
		YDSTime setFromInfo3;
		YDSTime setFromInfo4;
		YDSTime setFromInfo5;
		YDSTime Compare(2008,2,1,TimeSystem::GPS);

		IdToValue Id;
		Id.insert(make_pair('Y',"2008"));
		Id.insert(make_pair('j',"2"));
		Id.insert(make_pair('s',"1"));
		Id.insert(make_pair('P',"02"));
		if (!setFromInfo1.setFromInfo(Id)) return 1;
		if (setFromInfo1 != Compare) return 2;
		Id.erase('Y');
		Id.insert(make_pair('y',"06"));
		if (!setFromInfo2.setFromInfo(Id)) return 3;
		Id.erase('y');
		Id.insert(make_pair('y',"006"));
		if (!setFromInfo3.setFromInfo(Id)) return 4;
		Id.erase('y');
		Id.insert(make_pair('y',"2008"));
		if (!setFromInfo4.setFromInfo(Id)) return 5;
		Id.erase('y');
		if (!setFromInfo5.setFromInfo(Id)) return 6;
		ofstream out("Logs/printfOutput");

		out << setFromInfo1 << endl;
		out << setFromInfo2 << endl;
		out << setFromInfo3 << endl;
		out << setFromInfo4 << endl;
		return 0;
	}*/

	/* Test will check if the ways to initialize and set an YDSTime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		YDSTime Compare(2008,2,1);// Initialize with value
		YDSTime LessThanYear(2005,2,1);// Initialize with value with a smaller year
		YDSTime LessThanDOY(2008,1,1);// Initialize with value with a smaller month
		YDSTime LessThanSOD(2008,2,0);// Initialize with value with a smaller day
		YDSTime CompareCopy(Compare); // Initialize with copy constructor
		YDSTime CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

		//Equality Assertion
		if (!(Compare == CompareCopy)) return 1;
		//Non-equality Assertion
		if (!(Compare != LessThanYear)) return 2;
		//Less than assertions
		if (!(LessThanYear < Compare)) return 3;
		if (Compare < LessThanYear) return 4;
		if (!(LessThanDOY < Compare)) return 5;
		if (Compare < LessThanDOY) return 6;
		if (!(LessThanSOD < Compare)) return 7;
		if (Compare < LessThanSOD) return 8;
		//Greater than assertions
		if (!(Compare > LessThanYear)) return 9;
		if (!(Compare > LessThanDOY)) return 10;
		if (!(Compare > LessThanSOD)) return 11;
		//Less than equals assertion
		if (!(LessThanYear <= Compare)) return 12;
		if(!(CompareCopy <= Compare)) return 13;
		//Greater than equals assertion
		if(!(Compare >= LessThanYear)) return 14;
		if(!(Compare >= CompareCopy)) return 15;
		//Validity check
		if(!(Compare.isValid())) return 16;
		return 0;
	}

	/* Test will check the reset method. */
	int resetTest (void)
	{
	
	  	YDSTime Compare(2008,2,1,TimeSystem::GPS); //Initialize an object
		//Check initial data
		if (TimeSystem(TimeSystem::GPS) != Compare.getTimeSystem()) return 1;
		if (2008 != (int)Compare.year) return 2;
		if (2 != (int)Compare.doy) return 3;
		if (1 != (int)Compare.sod) return 4;

	  	Compare.reset(); // Reset it
		//Check reset results
	  	if (TimeSystem(0) != Compare.getTimeSystem()) return 5; 
		if (0 != (int)Compare.year) return 6; 
		if (0 != (int)Compare.doy) return 7; 
		if (0 != (int)Compare.sod) return 8; 
		return 0;
	}

	/* Test will check converting to/from CommonTime. */
	int toFromCommonTimeTest (void)
	{
	  	YDSTime Compare(2008,2,1,TimeSystem::GPS); //Initialize an object
  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		YDSTime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

  		if (!(Test2 == Compare)) return 1; // Converting to then from yields original

  		if (!(Compare.getTimeSystem()==TimeSystem(2))) return 2; // Recheck TimeSystem
		if (2008 != (int)Compare.year) return 3;
		if (2 != (int)Compare.doy) return 4;
		if (1 != (int)Compare.sod) return 5;
		return 0;
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
	xYDSTime testClass;
	check = testClass.operatorTest();
        std::cout << "opertatorTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	/*check = testClass.setFromInfoTest(); // Not run due to issue with Solaris compiler.
        std::cout << "setFromInfoTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;*/
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
