#include "CommonTime.hpp"
#include "Exception.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <cmath>
using namespace gpstk;
using namespace std;

class xCommonTime : public CommonTime
{
        public: 
		xCommonTime(){eps = 1e-11;}// Default Constructor, set the precision value

		/*
		  Test to see if any of the standard assignment methods break when using
		  proper inputs.
		*/
		int initializationTest()
		{
			TestUtil testFramework( "CommonTime", "Constructor(time,TimeSystem)", __FILE__, __func__ );
			testFramework.init();

			try {CommonTime Zero;}
			catch (...) 
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {CommonTime Test1;
			     Test1.set((long)700000,(long)0,(double)0.);}
			catch (...)
 			{
				testFramework.failTest();
				testFramework.print();
			}
				//CommonTime Test1((long)700000,(long)0,(double)0.); // This is a protected method

				//Copy-Constructer
			try {CommonTime Test1;
			     Test1.set((long)700000,(long)0,(double)0.);
			     CommonTime Test2(Test1);}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {CommonTime Test1;
			     Test1.set((long)700000,(long)0,(double)0.);
			     CommonTime Test3 = Test1;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {CommonTime Test1;
			     Test1.set((long)700000,(long)0,(double)0.);
			     CommonTime Test4;
			     Test4 = Test1;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			return testFramework.countFails();
		}
		/*
		  Test to see if setting improper values induces the correct exception handling.
		*/
		int improperSetTest()
		{
			CommonTime Test;
			Test.set(700000,0,0.);
			int brokeWhenAndHowItShouldCounter = 0;

			TestUtil testFramework( "CommonTime", "Constructor(time,TimeSystem)", __FILE__, __func__ );
			testFramework.init();
			// Break the input in various ways and make sure the proper exception is called
			try {Test.set(-1,0,0.);} //Negative days
			catch(gpstk::InvalidRequest e) {e.dump(cout); brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.set(3442449,0,0.);} //Too many days
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.set(700000,-1,0.);} //Negative seconds
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.set(700000,24*60*60+1,0.);} //Too many seconds
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.set(700000,0,-1.);} //Negative fractional seconds
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.set(700000,0,2.);} //Too many fractional seconds
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.setInternal(-1,0,0.);} //Negative days
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.setInternal(3442449,0,0.);} //Too many days
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.setInternal(700000,-1,0.);} //Negative seconds
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.setInternal(700000,24*60*60+1,0.);} //Too many seconds
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.setInternal(700000,1001,-1.);} //Negative fractional seconds
			catch(gpstk::Exception e) {brokeWhenAndHowItShouldCounter++;}
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			try {Test.setInternal(700000,1001,1001.);} //Too many fractional seconds
			catch (...)
			{
				testFramework.failTest();
				testFramework.print();
			}

			return testFramework.countFails();
		}

		/*
		  Test to check the various set methods are equivalent when they should be.
		*/
		int setComparisonTest()
		{
			CommonTime Test1, Test2, Test3, Test4;
			long day, day2;
			long sod, sod2;
			double fsod, fsod2;
			double dec = 1.1/SEC_PER_DAY;
				
			// Set in different ways
			Test1.set(700001,1,.1);
			Test2.set(700001,1.1);
			Test3.set(700001 + dec);
			Test4.setInternal(700001,1100,0.);
			//Load up compare variables
			Test1.get(day,sod,fsod);
			Test2.get(day2,sod2,fsod2);

			//Compare different sets
			if (day != day2) return 1;
			if (sod != sod2) return 2;
			if (fabs(fsod - fsod2) > eps) return 3;

			//Load up compare variables
			Test3.get(day2,sod2,fsod2);

			//Compare different sets
			if (day != day2) return 4;
			if (sod != sod2) return 5;

			//Testing results show fsod = 0.1 fsod2 = 0.100004
			//Appears to be a result of the input double is 700001.000012732
			//Rounding the last digit appears to be causing the issue and the
			//large error.
			if (fabs(fsod - fsod2) > 1E-4) return 6;


			/*Adding a test for a much lower day value to ensure the error is from
			  round off error. */
			Test1.set(1,1,.1);
			Test3.set(1+dec);
			Test1.get(day,sod,fsod);
			Test3.get(day2,sod2,fsod2);
			if (fabs(fsod-fsod2) > eps) return 7;

			//Load up compare variables
			Test1.set(700001,1,.1);
			Test1.get(day,sod,fsod);
			Test4.get(day2,sod2,fsod2);

			//Compare setInternal
			if (day != day2) return 8;
			if (sod != sod2) return 9;
			if (fabs(fsod - fsod2) > eps) return 10;
			return 0;
		}

		int arithmeticTest()
		{
			CommonTime Arith1;
			Arith1.set(700000,1,0.1); 
			CommonTime Arith2(Arith1); //Set second time equal to the first
			CommonTime Result;
			long day, day2, sod;
			double fsod, sod2;
			//- between two CommonTimes
			if (fabs((Arith1-Arith2) - 0) > eps) return 1;

			//Add Seconds with +
			Result = Arith2 + 1;
			Result.get(day,sod,fsod);
			if (day != 700000) return 2;
			if (sod != 2) return 3;
			if (fabs(fsod - 0.1) > eps) return 4;

			//Subtract seconds with -
			Result = Arith2 - 1;
			Result.get(day,sod,fsod);
			if (day != 700000) return 5;
			if (sod != 0) return 6;
			if (fabs(fsod - 0.1) > eps) return 7;

			//Add seconds with +=
			Arith2 += 1;
			if (fabs((Arith2-Arith1) - 1) > eps) return 8;

			//Check that values can be compared with integer seconds
			if (1 != Arith2 - Arith1) return 11;

			//Subtract seconds with -=
			Arith2 -= 1;
			if (fabs((Arith2-Arith1) - 0) > eps) return 9;

			//Add days with addDays
			Arith2.addDays((long)1);
			day = Arith2.getDays();
			if (700001. != day) return 15;

			//Subtract days with addDays
			Arith2.addDays((long)-1); 
			day = Arith2.getDays();
			if (700000. != day) return 16;

			//Add seconds with addSeconds(double)
			Arith2.addSeconds(86400000.+1000.);
			if (fabs(86401000. - (Arith2-Arith1)) > eps) return 17;

			//Subtract seconds with addSeconds(long)
			Arith2.addSeconds((long)-86401000);
			if (fabs(0. - (Arith2-Arith1)) > eps) return 18;

			//Check that the two parameter get method returns day2 as the proper double
			Arith2.get(day2, sod2);
			if ((long)700000 != day2) return 19;
			if (((double)0. - sod2) > eps) return 20;

			//Check seconds using getSecondOfDay()
			if (fabs(sod2 - Arith2.getSecondOfDay()) > eps) return 21;

			//Add milliseconds with addMilliseconds(long)
			Arith2.addMilliseconds((long)1);
			if (fabs(sod2+0.001 - Arith2.getSecondOfDay()) > eps) return 22;
			Arith2.addMilliseconds((long)-1);
			if (fabs(sod2 - Arith2.getSecondOfDay()) > eps) return 23;
			return 0;
		}

		// Test the comparison operators
		int operatorTest (void)
		{

			CommonTime Compare; Compare.set(1000,200,0.2); // Initialize with value
			CommonTime LessThanDay; LessThanDay.set(100,200,0.2); // Initialize with smaller day value
			CommonTime LessThanSecond; LessThanSecond.set(1000,20,0.2); // Initialize with smaller second value
			CommonTime LessThanFSecond; LessThanFSecond.set(1000,200,0.1); // Initialize with smaller fractional second value
			CommonTime CompareCopy(Compare); // Initialize with copy constructor

			//Equality Assertion
			if (!(Compare == CompareCopy)) return 1;			
			//Non-equality Assertion
			if (!(Compare != LessThanDay)) return 2;
			if (!(Compare != LessThanSecond)) return 3;
			if (!(Compare != LessThanFSecond)) return 4;
			//Less than assertions
			if (!(LessThanDay < Compare)) return 3;
			if (Compare < LessThanDay) return 4;
			if (!(LessThanSecond < Compare)) return 5;
			if (Compare < LessThanSecond) return 6;
			if (!(LessThanFSecond < Compare)) return 7;
			if (Compare < LessThanFSecond) return 8;
			//Greater than assertions
			if(!(Compare > LessThanDay)) return 5;
			//Less than equals assertion
			if (!(LessThanDay <= Compare)) return 6;
			if(!(CompareCopy <= Compare)) return 7;
			//Greater than equals assertion
			if(!(Compare >= LessThanDay)) return 8;
			if(!(Compare >= CompareCopy)) return 9;

			return 0;
		}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{

  		CommonTime GPS1; GPS1.set(1000,200,0.2,TimeSystem(2));
  		CommonTime GPS2; GPS2.set(100,200,0.2,TimeSystem(2));
  		CommonTime UTC1; UTC1.set(1000,200,0.2,TimeSystem(5));
  		CommonTime UNKNOWN; UNKNOWN.set(1000,200,0.2,TimeSystem(0));
  		CommonTime ANY; ANY.set(1000,200,0.2,TimeSystem(1));

  		if (GPS1 == GPS2) return 1; // GPS1 and GPS2 should have different times
  		if (GPS1.getTimeSystem() != GPS2.getTimeSystem()) return 2; // Should have the same time system
  		if (GPS1 == UTC1) return 3; //Should have different time systems
  		if (GPS1 == UNKNOWN) return 4;
		
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

	// Test Formatted Printing
	int  printfTest (void)
	{

  		CommonTime GPS1; GPS1.set(1234567,24000,0.2111,TimeSystem(2));
  		CommonTime UTC1; UTC1.set(1000,200,0.2,TimeSystem(7));
		
  		if (GPS1.asString() != (std::string)"1234567 24000211 0.000100000000000 GPS") return 1;
  		if (UTC1.asString() != (std::string)"0001000 00200200 0.000000000000000 UTC") return 2;
		// Test if BEGINNING_OF_TIME matches expectations
  		if (BEGINNING_OF_TIME.asString() != (std::string)"0000000 00000000 0.000000000000000 Any") return 3;		
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
	int check = -1, errorCounter = 0;
	xCommonTime testClass;

	check = testClass.initializationTest();
        std::cout << "initializationTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.improperSetTest();
        std::cout << "improperSetTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.setComparisonTest();
        std::cout << "setComparisonTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.arithmeticTest();
        std::cout << "arithmeticTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.operatorTest();
        std::cout << "opertatorTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.timeSystemTest();
        std::cout << "timeSystemTest Result is: "; 
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.printfTest();
        std::cout << "printfTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
