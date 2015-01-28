//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include "CommonTime.hpp"
#include "Exception.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <cmath>
using namespace gpstk;
using namespace std;

class CommonTime_T : public CommonTime
{
        public: 
		CommonTime_T(){eps = 1e-11;}// Default Constructor, set the precision value

		/*
		  Test to see if any of the standard assignment methods break when using
		  proper inputs.
		*/
		int initializationTest()
		{
			TestUtil testFramework( "CommonTime", "Constructor", __FILE__, __LINE__ );
			testFramework.init();

			try {CommonTime Zero;
				testFramework.passTest();}
			catch (...) 
			{
				testFramework.failTest();
			}

			try {CommonTime Test1;
			     Test1.set((long)700000,(long)0,(double)0.);
			 	 testFramework.passTest();}
			catch (...)
 			{
				testFramework.failTest();
			}
			
				//CommonTime Test1((long)700000,(long)0,(double)0.); // This is a protected method

				//Copy-Constructer
			try {CommonTime Test1;
			     Test1.set((long)700000,(long)0,(double)0.);
			     CommonTime Test2(Test1);
			  	 testFramework.passTest();}
			catch (...)
			{
				testFramework.failTest();
			}
			
			try {CommonTime Test1;
			     Test1.set((long)700000,(long)0,(double)0.);
			     CommonTime Test3 = Test1;
			   	 testFramework.passTest();}
			catch (...)
			{
				testFramework.failTest();
			}
			
			try {CommonTime Test1;
			     Test1.set((long)700000,(long)0,(double)0.);
			     CommonTime Test4;
			     Test4 = Test1;
			   	 testFramework.passTest();}
			catch (...)
			{
				testFramework.failTest();
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

			TestUtil testFramework( "CommonTime", "set", __FILE__, __LINE__ );
			testFramework.init();
			// Break the input in various ways and make sure the proper exception is called

//--------------CommonTime_improperSetTest_1 - Does a set method work with negative days?
			try {Test.set(-1,0,0.);
				testFramework.failTest();} //Negative days
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Why have InvalidRequest catcher instead of Exception?
			// Only case that does so
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			catch(gpstk::InvalidRequest e) {e.dump(cout); testFramework.passTest();}
			catch (...) {testFramework.failTest();}	

//--------------CommonTime_improperSetTest_2 - Does a set method work with too many days?
			try {Test.set(3442449,0,0.);
				testFramework.failTest();} //Too many days
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}			

//--------------CommonTime_improperSetTest_3 - Does a set method work with negative seconds?
			try {Test.set(700000,-1,0.);
				testFramework.failTest();} //Negative seconds
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	
			
//--------------CommonTime_improperSetTest_4 - Does a set method work with too many seconds?
			try {Test.set(700000,24*60*60+1,0.);
				testFramework.failTest();} //Too many seconds
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	

//--------------CommonTime_improperSetTest_5 - Does a set method work with negative fractional seconds?
			try {Test.set(700000,0,-1.);
				testFramework.failTest();} //Negative fractional seconds
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	
			
//--------------CommonTime_improperSetTest_6 - Does a set method work with too many fractional seconds?
			try {Test.set(700000,0,2.);
				testFramework.failTest();} //Too many fractional seconds

			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	
				
//--------------CommonTime_improperSetTest_7 - Does a set method work with negative days?
			try {Test.setInternal(-1,0,0.);
				testFramework.failTest();} //Negative days
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	
			
//--------------CommonTime_improperSetTest_8 - Does a set method work with too many days?
			try {Test.setInternal(3442449,0,0.);
				testFramework.failTest();} //Too many days
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	
			
//--------------CommonTime_improperSetTest_9 - Does a set method work with negative seconds?
			try {Test.setInternal(700000,-1,0.);
				testFramework.failTest();} //Negative seconds
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	
			
//--------------CommonTime_improperSetTest_10 - Does a set method work with too many seconds?
			try {Test.setInternal(700000,24*60*60+1,0.);
				testFramework.failTest();} //Too many seconds
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	
			
//--------------CommonTime_improperSetTest_11 - Does a set method work with negative fractional seconds?
			try {Test.setInternal(700000,1001,-1.);
				testFramework.failTest();} //Negative fractional seconds
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	
			
//--------------CommonTime_improperSetTest_12 - Does a set method work with too many fractional seconds?
			try {Test.setInternal(700000,1001,1001.);
				testFramework.failTest();} //Too many fractional seconds
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}	
		
			return testFramework.countFails();
		}

		/*
		  Test to check the various set methods are equivalent when they should be.
		*/
		int setComparisonTest()
		{
			TestUtil testFramework( "CommonTime", "set", __FILE__, __LINE__ );
			testFramework.init();

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


//--------------CommonTime_setComparisonTest_1 - Does a set method store the correct day value?
			testFramework.assert(day == day2);
			testFramework.next();

//--------------CommonTime_setComparisonTest_2 - Does a set method store the correct sod value?
			testFramework.assert(sod == sod2);
			testFramework.next();

//--------------CommonTime_setComparisonTest_3 - Does a set method store the correct fsod value?
			testFramework.assert(fabs(fsod - fsod2) < eps);
			testFramework.next();

			//Load up compare variables
			Test3.get(day2,sod2,fsod2);

//--------------CommonTime_setComparisonTest_4 - Does a set method store the correct day value?
			testFramework.assert(day == day2);
			testFramework.next();

//--------------CommonTime_setComparisonTest_5 - Does a set method store the correct sod value?
			testFramework.assert(sod == sod2);
			testFramework.next();

			//Testing results show fsod = 0.1 fsod2 = 0.100004
			//Appears to be a result of the input double is 700001.000012732
			//Rounding the last digit appears to be causing the issue and the
			//large error.

//--------------CommonTime_setComparisonTest_6 - Does a set method store the correct fsod value?			
			testFramework.assert(fabs(fsod - fsod2) < 1E-4);
			testFramework.next();


			/*Adding a test for a much lower day value to ensure the error is from
			  round off error. */
			Test1.set(1,1,.1);
			Test3.set(1+dec);
			Test1.get(day,sod,fsod);
			Test3.get(day2,sod2,fsod2);

//--------------CommonTime_setComparisonTest_7 - Does a set method store the correct fsod value?			
			testFramework.assert(fabs(fsod - fsod2) < eps);
			testFramework.next();

			//Load up compare variables
			Test1.set(700001,1,.1);
			Test1.get(day,sod,fsod);
			Test4.get(day2,sod2,fsod2);

//--------------CommonTime_setComparisonTest_8 - Does a setInternal method store the correct day value?			
			testFramework.assert(day == day2);
			testFramework.next();

//--------------CommonTime_setComparisonTest_9 - Does a setInternal method store the correct sod value?			
			testFramework.assert(sod == sod2);
			testFramework.next();

//--------------CommonTime_setComparisonTest_10 - Does a setInternal method store the correct sod value?			
			testFramework.assert(fabs(fsod - fsod2) < eps);

			return testFramework.countFails();
		}
		/* Test to check arithmetic operations function properly */
		int arithmeticTest()
		{
			TestUtil testFramework( "CommonTime", "Operators", __FILE__, __LINE__ );
			testFramework.init();

			CommonTime Arith1;
			Arith1.set(700000,1,0.1); 
			CommonTime Arith2(Arith1); //Set second time equal to the first
			CommonTime Result;
			long day, day2, sod;
			double fsod, sod2;

//--------------CommonTime_arithmeticTest_1 - Does it subtract between two CommonTime objects?
			testFramework.assert(fabs((Arith1-Arith2) - 0) < eps);
			testFramework.next();

			//Add seconds with +
			Result = Arith2 + 1;
			Result.get(day,sod,fsod);

//--------------CommonTime_arithmeticTest_2 - Does it not add to the day value?
			testFramework.assert(day == 700000);
			testFramework.next();

//--------------CommonTime_arithmeticTest_3 - Does it add to the sod value?
			testFramework.assert(sod == 2);
			testFramework.next();

//--------------CommonTime_arithmeticTest_4 - Does it not add to the fsod value?
			testFramework.assert(fabs(fsod - 0.1) < eps);
			testFramework.next();

			//Subtract seconds with -
			Result = Arith2 - 1;
			Result.get(day,sod,fsod);

//--------------CommonTime_arithmeticTest_5 - Does it not subtract from the day value?
			testFramework.assert(day == 700000);
			testFramework.next();

//--------------CommonTime_arithmeticTest_6 - Does it subtract from the sod value?
			testFramework.assert(sod == 0);
			testFramework.next();

//--------------CommonTime_arithmeticTest_7 - Does it not subtract from the fsod value?
			testFramework.assert(fabs(fsod - 0.1) < eps);
			testFramework.next();

			//Add seconds with +=
			Arith2 += 1;

//--------------CommonTime_arithmeticTest_8 - Does it add to a CommonTime object?
			testFramework.assert(fabs((Arith2-Arith1) - 1) < eps);
			testFramework.next();

//--------------CommonTime_arithmeticTest_9 - Check that values can be compared with integer seconds
			testFramework.assert(1 == Arith2 - Arith1);
			testFramework.next();

			//Subtract seconds with -=
			Arith2 -= 1;

//--------------CommonTime_arithmeticTest_10 - Does it subtract from a CommonTime object?
			testFramework.assert(fabs((Arith2-Arith1) - 0) < eps);
			testFramework.next();

			//Add days with addDays
			Arith2.addDays((long)1);
			day = Arith2.getDays();

//--------------CommonTime_arithmeticTest_11 - Does the addDays method function correctly with +?
			testFramework.assert(700001. == day);
			testFramework.next();

			//Subtract days with addDays
			Arith2.addDays((long)-1); 
			day = Arith2.getDays();

//--------------CommonTime_arithmeticTest_12 - Does the addDays method function correctly with -?			
			testFramework.assert(700000. == day);
			testFramework.next();

			//Add seconds with addSeconds(double)
			Arith2.addSeconds(86400000.+1000.);

//--------------CommonTime_arithmeticTest_13 - Does the addSeconds method function correctly with +?			
			testFramework.assert(fabs(86401000. - (Arith2-Arith1)) < eps);
			testFramework.next();

			//Subtract seconds with addSeconds(long)
			Arith2.addSeconds((long)-86401000);

//--------------CommonTime_arithmeticTest_14 - Does the addSeconds method function correctly with -?
			testFramework.assert(fabs(0. - (Arith2-Arith1)) < eps);
			testFramework.next();

			//Check that the two parameter get method returns day2 as the proper double
			Arith2.get(day2, sod2);

//--------------CommonTime_arithmeticTest_15 - Does the 2 parameter get method reuturn days as a double?
			testFramework.assert((long)700000 == day2);
			testFramework.next();

//--------------CommonTime_arithmeticTest_16 - Does the 2 parameter get method reuturn days as a double?
			testFramework.assert(((double)0. - sod2) < eps);
			testFramework.next();

			//Check seconds using getSecondOfDay()
			testFramework.assert(fabs(sod2 - Arith2.getSecondOfDay()) < eps);
			testFramework.next();

			//Add milliseconds with addMilliseconds(long)
			Arith2.addMilliseconds((long)1);
			
//--------------CommonTime_arithmeticTest_17 - Does the addMilliseconds method function correctly with +?			
			testFramework.assert(fabs(sod2+0.001 - Arith2.getSecondOfDay()) < eps);
			testFramework.next();

			Arith2.addMilliseconds((long)-1);

//--------------CommonTime_arithmeticTest_18 - Does the addMilliseconds method function correctly with -?
			testFramework.assert(fabs(sod2 - Arith2.getSecondOfDay()) < eps);

			return testFramework.countFails();
		}

		// Test the comparison operators
		int operatorTest (void)
		{
			TestUtil testFramework( "CommonTime", "Differing TimeSystem == Operator", __FILE__, __LINE__ );
			testFramework.init();
			CommonTime Compare; Compare.set(1000,200,0.2); // Initialize with value
			CommonTime LessThanDay; LessThanDay.set(100,200,0.2); // Initialize with smaller day value
			CommonTime LessThanSecond; LessThanSecond.set(1000,20,0.2); // Initialize with smaller second value
			CommonTime LessThanFSecond; LessThanFSecond.set(1000,200,0.1); // Initialize with smaller fractional second value
			CommonTime CompareCopy(Compare); // Initialize with copy constructor

//--------------GPSWeekZCount_operatorTest_1 - Are equivalent objects equivalent?
			testFramework.assert(Compare == CompareCopy);
			testFramework.next();

//--------------GPSWeekZCount_operatorTest_2 - Are non-equivalent objects equivalent?
			testFramework.assert(!(Compare == LessThanDay));
			testFramework.next();

			testFramework.changeSourceMethod("!= Operator");
//--------------GPSWeekZCount_operatorTest_3 - Are non-equivalent objects not equivalent?
			testFramework.assert(Compare != LessThanDay);
			testFramework.next();

//--------------GPSWeekZCount_operatorTest_3 - Are non-equivalent objects not equivalent?
			testFramework.assert(Compare != LessThanSecond);
			testFramework.next();

//--------------GPSWeekZCount_operatorTest_3 - Are non-equivalent objects not equivalent?
			testFramework.assert(Compare != LessThanFSecond);
			testFramework.next();

//--------------GPSWeekZCount_operatorTest_4 - Are equivalent objects not equivalent?
			testFramework.assert(!(Compare != Compare));
			testFramework.next();

			testFramework.changeSourceMethod("< Operator");
//--------------GPSWeekZCount_operatorTest_5 - Does the < operator function when left_object < right_object?
			testFramework.assert(LessThanDay < Compare);
			testFramework.next();

//--------------CommonTime_operatorTest_7 - Does the < operator function when left_object < right_object by days?
			testFramework.assert(LessThanSecond < Compare);
			testFramework.next();

//--------------CommonTime_operatorTest_8 - Does the < operator function when left_object > right_object by days?		
			testFramework.assert(!(Compare < LessThanSecond));
			testFramework.next();

//--------------CommonTime_operatorTest_9 - Does the < operator function when left_object < right_object by seconds?
			testFramework.assert(LessThanFSecond < Compare);
			testFramework.next();

//--------------CommonTime_operatorTest_10 - Does the < operator function when left_object > right_object by seconds?
			testFramework.assert(!(Compare < LessThanFSecond));
			testFramework.next();

//--------------CommonTime_operatorTest_11 - Does the < operator function when left_object = right_object?
			testFramework.assert(!(Compare < CompareCopy));
			testFramework.next();
			//Greater than assertions
			testFramework.changeSourceMethod("> Operator");

//--------------CommonTime_operatorTest_12 - Does the > operator function when left_object > right_object by years?
			testFramework.assert(Compare > LessThanDay);
			testFramework.next();

//--------------CommonTime_operatorTest_13 - Does the > operator function when left_object < right_object by years?
			testFramework.assert(!(LessThanDay > Compare));
			testFramework.next();

//--------------CommonTime_operatorTest_14 - Does the > operator function when left_object > right_object by days?
			testFramework.assert(Compare > LessThanSecond);
			testFramework.next();

//--------------CommonTime_operatorTest_15 - Does the > operator function when left_object < right_object by days?		
			testFramework.assert(!(LessThanSecond > Compare));
			testFramework.next();

//--------------CommonTime_operatorTest_16 - Does the > operator function when left_object > right_object by seconds?
			testFramework.assert(Compare > LessThanFSecond);
			testFramework.next();

//--------------CommonTime_operatorTest_17 - Does the > operator function when left_object < right_object by seconds?
			testFramework.assert(!(LessThanFSecond > Compare));
			testFramework.next();

//--------------CommonTime_operatorTest_18 - Does the > operator function when left_object = right_object?
			testFramework.assert(!(Compare > CompareCopy));
			testFramework.next();	

			//Less than equals assertion
			testFramework.changeSourceMethod("<= Operator");

//--------------CommonTime_operatorTest_19 - Does the < operator function when left_object < right_object by years?
			testFramework.assert(LessThanDay <= Compare);
			testFramework.next();

//--------------CommonTime_operatorTest_20 - Does the <= operator function when left_object > right_object by years?
			testFramework.assert(!(Compare <= LessThanDay));
			testFramework.next();

//--------------CommonTime_operatorTest_21 - Does the <= operator function when left_object < right_object by days?
			testFramework.assert(LessThanSecond <= Compare);
			testFramework.next();

//--------------CommonTime_operatorTest_22 - Does the <= operator function when left_object > right_object by days?		
			testFramework.assert(!(Compare <= LessThanSecond));
			testFramework.next();

//--------------CommonTime_operatorTest_23 - Does the <= operator function when left_object < right_object by seconds?
			testFramework.assert(LessThanFSecond <= Compare);
			testFramework.next();

//--------------CommonTime_operatorTest_24 - Does the <= operator function when left_object > right_object by seconds?
			testFramework.assert(!(Compare <= LessThanFSecond));
			testFramework.next();

//--------------CommonTime_operatorTest_25 - Does the <= operator function when left_object = right_object?
			testFramework.assert(Compare <= CompareCopy);
			testFramework.next();

			//Greater than equals assertion
			testFramework.changeSourceMethod(">= Operator");

//--------------CommonTime_operatorTest_26 - Does the >= operator function when left_object > right_object by years?
			testFramework.assert(Compare >= LessThanDay);
			testFramework.next();

//--------------CommonTime_operatorTest_27 - Does the >= operator function when left_object < right_object by years?
			testFramework.assert(!(LessThanDay >= Compare));
			testFramework.next();

//--------------CommonTime_operatorTest_28 - Does the >= operator function when left_object > right_object by days?
			testFramework.assert(Compare >= LessThanSecond);
			testFramework.next();

//--------------CommonTime_operatorTest_29 - Does the >= operator function when left_object < right_object by days?		
			testFramework.assert(!(LessThanSecond >= Compare));
			testFramework.next();

//--------------CommonTime_operatorTest_30 - Does the >= operator function when left_object > right_object by seconds?
			testFramework.assert(Compare >= LessThanFSecond);
			testFramework.next();

//--------------CommonTime_operatorTest_31 - Does the >= operator function when left_object < right_object by seconds?
			testFramework.assert(!(LessThanFSecond >= Compare));
			testFramework.next();

//--------------CommonTime_operatorTest_32 - Does the > operator function when left_object = right_object?
			testFramework.assert(!(Compare < CompareCopy));

			return testFramework.countFails();
		}

	/* Test will check the reset method. */
	int resetTest (void)
	{
		TestUtil testFramework( "CommonTime", "reset" , __FILE__, __LINE__ );
		testFramework.init();

		CommonTime Compare; Compare.set(1000,200,0.2); // Initialize with value
		long day, sod;
		double fsod;
	  	Compare.reset(); // Reset it
	  	Compare.get(day,sod,fsod);
//--------------CommonTime_operatorTest_1 - Was the time system reset to expectation?
	  	testFramework.assert(TimeSystem(0) == Compare.getTimeSystem());
	  	testFramework.next();

//--------------CommonTime_operatorTest_2 - Was the day value reset to expectation?	  	
		testFramework.assert(0 == day); 
		testFramework.next();

//--------------CommonTime_operatorTest_3 - Was the sod value reset to expectation?
		testFramework.assert(0 == sod); 
		testFramework.next();

//--------------CommonTime_operatorTest_4 - Was the fsod value reset to expectation?	  	
		testFramework.assert(0 == fsod); 
		testFramework.next();

		return testFramework.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{
		TestUtil testFramework( "CommonTime", "Differing TimeSystem == Operator", __FILE__, __LINE__ );
		testFramework.init();

  		CommonTime GPS1; GPS1.set(1000,200,0.2,TimeSystem(2));
  		CommonTime GPS2; GPS2.set(100,200,0.2,TimeSystem(2));
  		CommonTime UTC1; UTC1.set(1000,200,0.2,TimeSystem(5));
  		CommonTime UNKNOWN; UNKNOWN.set(1000,200,0.2,TimeSystem(0));
  		CommonTime ANY; ANY.set(1000,200,0.2,TimeSystem(1));

//--------------CommonTime_timeSystemTest_1 - Verify same Time System but different time inequality
		testFramework.assert(!(GPS1 == GPS2));
		testFramework.next();

//--------------CommonTime_timeSystemTest_2 - Verify same Time System equality
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Differing TimeSystem != Operator");
//--------------CommonTime_timeSystemTest_3 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UTC1);
		testFramework.next();

//--------------CommonTime_timeSystemTest_4 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);
		testFramework.next();
		
		testFramework.changeSourceMethod("ANY TimeSystem == Operator");		
//--------------CommonTime_timeSystemTest_5 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		testFramework.assert(GPS1 == ANY);
		testFramework.next();

//--------------CommonTime_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		testFramework.assert(UTC1 == ANY);
		testFramework.next();

//--------------CommonTime_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		testFramework.assert(UNKNOWN == ANY);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem < Operator");	
//--------------CommonTime_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY));
		testFramework.next();

		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
//--------------CommonTime_timeSystemTest_9 - Ensure resetting a Time System changes it
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2));

		return testFramework.countFails();
	}

	// Test Formatted Printing
	int  printfTest (void)
	{	
		TestUtil testFramework( "CommonTime", "printf", __FILE__, __LINE__ );
		testFramework.init();

  		CommonTime GPS1; GPS1.set(1234567,24000,0.2111,TimeSystem(2));
  		CommonTime UTC1; UTC1.set(1000,200,0.2,TimeSystem(7));
		
//--------------CommonTime_printfTest_1 - Verify printed output matches expectation		
  		testFramework.assert(GPS1.asString() == (std::string)"1234567 24000211 0.000100000000000 GPS");
  		testFramework.next();

//--------------CommonTime_printfTest_2 - Verify printed output matches expectation		
  		testFramework.assert(UTC1.asString() == (std::string)"0001000 00200200 0.000000000000000 UTC");
  		testFramework.next();

		// Test if BEGINNING_OF_TIME matches expectations
  		testFramework.assert(BEGINNING_OF_TIME.asString() == (std::string)"0000000 00000000 0.000000000000000 Any");		
		return testFramework.countFails();
	}
	private:
		double eps;
};


int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	CommonTime_T testClass;

	check = testClass.initializationTest();
	errorCounter += check;

	check = testClass.improperSetTest();
	errorCounter += check;

	check = testClass.setComparisonTest();
	errorCounter += check;

	check = testClass.arithmeticTest();
	errorCounter += check;

	check = testClass.operatorTest();
	errorCounter += check;

	check = testClass.resetTest();
	errorCounter += check;

	check = testClass.timeSystemTest();
	errorCounter += check;

	check = testClass.printfTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
