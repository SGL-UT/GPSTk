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
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Average is: " << test.Average() << std::endl;
			if (test.AverageX() == 3 && test.N() == 5)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		};
		int AverageXTest()
		// Verify the X average calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Average of X is: " << test.AverageX() << std::endl;
			if (test.AverageX() == 3)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int AverageYTest()
		// Verify the Y average calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Average of Y is: " << test.AverageY() << std::endl;
			if (test.AverageY() == 3)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int MaxXTest()
		// Verify the X maximum calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Maximum of X is: " << test.MaximumX() << std::endl;
			if (test.MaximumX() == 5)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int MaxYTest()
		// Verify the Y maximum calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Maximum of Y is: " << test.MaximumY() << std::endl;
			if (test.MaximumY() == 5)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int MinXTest()
		// Verify the X minimum calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Minimum of X is: " << test.MinimumX() << std::endl;
			if (test.MinimumX() == 1)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int MinYTest()
		// Verify the Y minimum calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Minimum of Y is: " << test.MinimumY() << std::endl;
			if (test.MinimumY() == 1)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int VarianceXTest()
		// Verify the X variance calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Variance of X is: " << test.VarianceX() << std::endl;
			if (test.VarianceX() == 2)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int VarianceYTest()
		// Verify the Y variance calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Variance of Y is: " << test.VarianceY() << std::endl;
			if (test.VarianceY() == 2)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int StdDevXTest()
		// Verify the X Standard Deviation calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Standard Deviation of X is: " << test.StdDevX() << std::endl;
			if ((test.StdDevX() > (sqrt(2.0)-eps)) && (test.StdDevX() < (sqrt(2.0)+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int StdDevYTest()
		// Verify the Y Standard Deviation calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Standard Deviation of Y is: " << test.StdDevY() << std::endl;
			if ((test.StdDevY() > (sqrt(2.0)-eps)) && (test.StdDevY() < (sqrt(2.0)+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int SlopeTest()
		// Verify the Slope calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Slope is: " << test.Slope() << std::endl;
			if ((test.Slope() > (0.1-eps)) && (test.Slope() < (0.1+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int InterceptTest()
		// Verify the Intercept calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Intercept is: " << test.Intercept() << std::endl;
			if ((test.Intercept() > (2.7-eps)) && (test.Intercept() < (2.7+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int SlopeUncertaintyTest()
		// Verify the Slope Uncertainty calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Slope Uncertainty is: " << test.SigmaSlope() << std::endl;
			if ((test.SigmaSlope() > (1.2375-eps)) && (test.SigmaSlope() < (1.2375+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int ConditionalUncertaintyTest()
		// Verify the Conditional Uncertainty calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Conditional Uncertainty is: " << test.SigmaYX() << std::endl;
			if ((test.SigmaYX() > (2.475-eps)) && (test.SigmaYX() < (2.475+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int CorrelationTest()
		// Verify the Correlation calculation.
		{
			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			std::cout << "The Correlation is: " << test.Correlation() << std::endl;
			if ((test.Correlation() > (0.1-eps)) && (test.Correlation() < (0.1+eps)))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}



 };

void checkResult(int check, int& errCount)// Function to handle test result output
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

int main()//Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	StatsTest testClass;
	check = testClass.AddTest();
        std::cout << "Add Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.AverageXTest();
        std::cout << "AverageX Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.AverageYTest();
        std::cout << "AverageY Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.MaxXTest();
        std::cout << "MaximumX Result is: "; 
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.MaxYTest();
        std::cout << "MaximumY Result is: "; 
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.MinXTest();
        std::cout << "MinimumX Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.MinYTest();
        std::cout << "MinimumY Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.VarianceXTest(); 
        std::cout << "VarianceX Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.VarianceYTest(); 
        std::cout << "VarianceY Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.StdDevXTest(); 
        std::cout << "Standard Deviation in X Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.StdDevYTest(); 
        std::cout << "Standard Deviation in Y Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.SlopeTest(); 
        std::cout << "Slope Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.InterceptTest(); 
        std::cout << "Intercept Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.ConditionalUncertaintyTest(); 
        std::cout << "Conditional Uncertainty Test Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.SlopeUncertaintyTest(); 
        std::cout << "Slope Uncertainty Test Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.CorrelationTest(); 
        std::cout << "Correlation Test Result is: ";
	checkResult(check, errorCounter);
	check = -1;


	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter;//Return the total number of errors
}
