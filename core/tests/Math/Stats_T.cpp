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

#include "Stats.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"
#include <iostream>

class Stats_T
{
        public: 
		Stats_T(){eps = 1e-12;}// Default Constructor, set the precision value
		~Stats_T() {} // Default Desructor

		double eps; // Shouldn't this be private?

		/*	Test to add data. Want to add single values to empty Stats class.
			Then add another stat on top with weight. I will use the average to check
			that data was added and that the data added was correct. */
		int AddTest()
		{
			TestUtil testFramework( "Stats", "Add", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<double> test;
			test.Add(5, 1);		
			test.Add(11, 2);
			test.Add(9,3);
			//std::cout << "The Average is: " << test.Average() << std::endl;

			testFramework.assert(test.Average()==9);

			return testFramework.countFails();
			
		};
		/*	Test to add data with a vector. Want to add a single vector to empty Stats class.
			I will use the average to check that data was added 
			and that the data added was correct. */
		int AddVectorTest()
		{
			TestUtil testFramework( "Stats", "AddVector", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<double> test;
			gpstk::Vector<double> input(5), weight(5);
			for (int i = 0; i < 5; i++)
			{
				input(i) = i+1;
				weight(i) = i+2;
			}
			test.Add(input, weight);
			//std::cout << "The Average is: " << test.Average() << std::endl;
			testFramework.assert((test.Average() > (3.5-eps)) && (test.Average() < (3.5+eps)));

			return testFramework.countFails();	
		}
		/*	Verify the average calculation. */
		int AverageTest()
		{
			TestUtil testFramework( "Stats", "Average", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<double> test;
			gpstk::Vector<double> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+1;
			}
			test.Add(input, weight); 

			//std::cout << "The Average is: " << test.Average() << std::endl;

			testFramework.assert((test.Average() > (7.0/3.0-eps)) && (test.Average() < (7.0/3.0+eps)));

			return testFramework.countFails();
		}
		/*	Verify the maximum calculation. */
		int MaxTest()
		{
			TestUtil testFramework( "Stats", "Max", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<int> test;
			gpstk::Vector<int> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+1;
			}
			test.Add(input, weight);

			//std::cout << "The Maximum is: " << test.Maximum() << std::endl;

			testFramework.assert(test.Maximum() == 3);

			return testFramework.countFails();
		}
		/*	Verify the minimum calculation */
		int MinTest()
		{
			TestUtil testFramework( "Stats", "Min", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<int> test;
			gpstk::Vector<int> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+1;
			}
			test.Add(input, weight);

			//std::cout << "The Minimum is: " << test.Minimum() << std::endl;

			testFramework.assert(test.Minimum() == 1);

			return testFramework.countFails();
		}
		/*	Verify the variance calculation */
		int VarianceTest()
		{
			TestUtil testFramework( "Stats", "Variance", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<double> test;
			gpstk::Vector<double> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+2;
			}
			test.Add(input, weight);
			//std::cout << "The Variance is: " << test.Variance() << std::endl;

			testFramework.assert(test.Variance() > (5.0/9.0-eps) && test.Variance() < (5.0/9.0+eps));

			return testFramework.countFails();
		}
		/*	Verify the Standard Deviation calculation.*/
		int StdDevTest()
		{
			TestUtil testFramework( "Stats", "StdDev", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<double> test;
			gpstk::Vector<double> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+1;
			}
			test.Add(input, weight);
			//std::cout << "The Standard Deviation is: " << test.StdDev() << std::endl;
			testFramework.assert(test.StdDev() > (sqrt(5.0/9.0)-eps) && test.StdDev() < (sqrt(5.0/9.0)+eps));

			return testFramework.countFails();
		}
		/*	Test to subtract data. Add a single vector to empty Stats class.
			After, delete a weighted data portion. 
            I will use the average and maxmimum to check data was removed.
			Specifically I will remove the former maximum and show that it is
			NOT reset. */
		int SubtractTest()
		{
			TestUtil testFramework( "Stats", "Subtract", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<double> test;
			gpstk::Vector<double> input(5), weight(5);
			for (int i = 0; i < 5; i++)
			{
				input(i) = i+1;
				weight(i) = i+2;
			}
			test.Add(input, weight);
			//std::cout << "The Average is: " << test.Average() << std::endl;
			test.Subtract(5);
			//std::cout << "The Average is: " << test.Average() << std::endl;
			test.Subtract(5); // Doing this twice to get 10/3 as a result
			//std::cout << "The Average is: " << test.Average() << std::endl;
			testFramework.assert(test.Average() > 3.2 && test.Average() < 3.4 && test.Maximum() == 5);

			return testFramework.countFails();			
		}
		/*	Test to subtract data with a vector. Add a single vector to empty Stats class.
			After, delete with the same vector. 
			I will use the average and maxmimum to check data was removed.
			Specifically I will remove the former maximum and show that it is
			NOT reset. */
		int SubtractVectorTest()
		{
			TestUtil testFramework( "Stats", "SubtractVector", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<int> test;
			gpstk::Vector<int> input(5), weight(5);
			for (int i = 0; i < 5; i++)
			{
				input(i) = i+1;
				weight(i) = 1;
			}
			test.Add(input, weight);
			test.Subtract(input);
			testFramework.assert((test.Average() > -0.000001) && (test.Average() < 0.000001) && (test.Maximum() == 5));
	
			return testFramework.countFails();
		}
		/*	Test to use the += and << operators. 
			Uses the same data as in the previous steps and should match. */
		int operatorTest()
		{
			TestUtil testFramework( "Stats", "+= Operator", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::Stats<int> test1, test2;
			test1.Add(1,1);
			test2.Add(2,2);
			test1.Add(3,3);
			test1 += test2;
			std::cout << test1 << std::endl;
			testFramework.assert((test1.Average() > (7.0/3.0-eps)) && (test1.Average() < (7.0/3.0+eps)));

			return testFramework.countFails();
		}

 };

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	Stats_T testClass;

	check = testClass.AddTest();
	errorCounter += check;

	check = testClass.AddVectorTest();
	errorCounter += check;

	check = testClass.AverageTest();
	errorCounter += check;

	check = testClass.MaxTest();
	errorCounter += check;
	check = testClass.MinTest();
	errorCounter += check;

// This will fail with MSNTk. Variance is not calculated correctly. GPSTK is also incorrect.
	check = testClass.VarianceTest(); 
	errorCounter += check;

// Standard Deviation is not calculated correctly since variance is not calculated correctly. GPSTk is also incorrect.
	check = testClass.StdDevTest(); 
	errorCounter += check;

/* Portion for testing the Subtract Routines. Commented out as they will fail as written.
   However the tests are designed to demonstrate the shortcomings in these routines. */
	check = testClass.SubtractTest();
	errorCounter += check;

	check = testClass.SubtractVectorTest();
	errorCounter += check;

	// This test will fail as well.

	check = testClass.operatorTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
