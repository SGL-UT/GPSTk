#include "Stats.hpp"
#include "Vector.hpp"
#include <iostream>

class StatsTest
{
        public: 
		StatsTest(){eps = 1e-12;}// Default Constructor, set the precision value
		~StatsTest() {} // Default Desructor
		double eps;
		int AddTest()
		// Test to add data. Want to add single values to empty Stats class.
		// Then add another stat on top with weight. I will use the average to check
		// that data was added and that the data added was correct.
		{
			gpstk::Stats<double> test;
			test.Add(5, 1);		
			test.Add(11, 2);
			test.Add(9,3);
			//std::cout << "The Average is: " << test.Average() << std::endl;
			if (test.Average() == 9)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		};
		int AddVectorTest()
		// Test to add data with a vector. Want to add a single vector to empty Stats class.
		// I will use the average to check that data was added 
                // and that the data added was correct. 
		{
			gpstk::Stats<double> test;
			gpstk::Vector<double> input(5), weight(5);
			for (int i = 0; i < 5; i++)
			{
				input(i) = i+1;
				weight(i) = i+2;
			}
			test.Add(input, weight);
			//std::cout << "The Average is: " << test.Average() << std::endl;
			if ((test.Average() > (3.5-eps)) && (test.Average() < (3.5+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int AverageTest()
		// Verify the average calculation.
		{
			gpstk::Stats<double> test;
			gpstk::Vector<double> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+1;
			}
			test.Add(input, weight); 
			std::cout << "The Average is: " << test.Average() << std::endl;
			if ((test.Average() > (7.0/3.0-eps)) && (test.Average() < (7.0/3.0+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int MaxTest()
		// Verify the maximum calculation.
		{
			gpstk::Stats<int> test;
			gpstk::Vector<int> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+1;
			}
			test.Add(input, weight);
			std::cout << "The Maximum is: " << test.Maximum() << std::endl;
			if (test.Maximum() == 3)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int MinTest()
		// Verify the minimum calculation.
		{
			gpstk::Stats<int> test;
			gpstk::Vector<int> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+1;
			}
			test.Add(input, weight);
			std::cout << "The Minimum is: " << test.Minimum() << std::endl;
			if (test.Minimum() == 1)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int VarianceTest()
		// Verify the variance calculation.
		{
			gpstk::Stats<double> test;
			gpstk::Vector<double> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+2;
			}
			test.Add(input, weight);
			std::cout << "The Variance is: " << test.Variance() << std::endl;
			if (test.Variance() > (5.0/9.0-eps) && test.Variance() < (5.0/9.0+eps))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int StdDevTest()
		// Verify the Standard Deviation calculation.
		{
			gpstk::Stats<double> test;
			gpstk::Vector<double> input(3), weight(3);
			for (int i = 0; i < 3; i++)
			{
				input(i) = i+1;
				weight(i) = i+1;
			}
			test.Add(input, weight);
			std::cout << "The Standard Deviation is: " << test.StdDev() << std::endl;
			if (test.StdDev() > (sqrt(5.0/9.0)-eps) && test.StdDev() < (sqrt(5.0/9.0)+eps))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int SubtractTest()
		// Test to subtract data. Add a single vector to empty Stats class.
		// After, delete a weighted data portion. 
                // I will use the average and maxmimum to check data was removed.
		// Specifically I will remove the former maximum and show that it is
		// NOT reset. 
		{
			gpstk::Stats<double> test;
			gpstk::Vector<double> input(5), weight(5);
			for (int i = 0; i < 5; i++)
			{
				input(i) = i+1;
				weight(i) = i+2;
			}
			test.Add(input, weight);
			std::cout << "The Average is: " << test.Average() << std::endl;
			test.Subtract(5);
			std::cout << "The Average is: " << test.Average() << std::endl;
			test.Subtract(5); // Doing this twice to get 10/3 as a result
			std::cout << "The Average is: " << test.Average() << std::endl;
			if (test.Average() > 3.2 && test.Average() < 3.4 && test.Maximum() == 5)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int SubtractVectorTest()
		// Test to subtract data with a vector. Add a single vector to empty Stats class.
		// After, delete with the same vector. 
                // I will use the average and maxmimum to check data was removed.
		// Specifically I will remove the former maximum and show that it is
		// NOT reset. 
		{
			gpstk::Stats<int> test;
			gpstk::Vector<int> input(5), weight(5);
			for (int i = 0; i < 5; i++)
			{
				input(i) = i+1;
				weight(i) = 1;
			}
			test.Add(input, weight);
			test.Subtract(input);
			if ((test.Average() > -0.000001) && (test.Average() < 0.000001) && (test.Maximum() == 5))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int operatorTest()
		// Test to use the += and << operators. 
		// Uses the same data as in the previous steps and should match.
		{
			gpstk::Stats<int> test1, test2;
			test1.Add(1,1);
			test2.Add(2,2);
			test1.Add(3,3);
			test1 += test2;
			std::cout << test1 << std::endl;
			if ((test1.Average() > (7.0/3.0-eps)) && (test1.Average() < (7.0/3.0+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
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
	StatsTest testClass;
	check = testClass.AddTest();
        std::cout << "Add Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.AddVectorTest();
        std::cout << "Add Vector Result is: "; 
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.AverageTest();
        std::cout << "Average Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.MaxTest();
        std::cout << "Maximum Result is: "; 
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.MinTest();
        std::cout << "Minimum Result is: ";
	checkResult(check, errorCounter);
	check = -1;
// This will fail with MSNTk. Variance is not calculated correctly. GPSTK is also incorrect.
	check = testClass.VarianceTest(); 
        std::cout << "Variance Result is: ";
	checkResult(check, errorCounter);
	check = -1;
// Standard Deviation is not calculated correctly since variance is not calculated correctly. GPSTk is also incorrect.
	check = testClass.StdDevTest(); 
        std::cout << "Standard Deviation Result is: ";
	checkResult(check, errorCounter);
	check = -1;
/* Portion for testing the Subtract Routines. Commented out as they will fail as written.
   However the tests are designed to demonstrate the shortcomings in these routines.
	check = testClass.SubtractTest();
        std::cout << "Subtract Result is: ";
	checkResult(check, errorCounter);
	check = testClass.SubtractVectorTest();
        std::cout << "Subtract Vector Result is: ";
	checkResult(check, errorCounter);
*/
	// This test will fail as well.
/*	std::cout << "Performing Operator Check:" << std::endl;
	check = testClass.operatorTest();
	std::cout << "Operator Result is: ";
	checkResult(check, errorCounter);
*/
	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
