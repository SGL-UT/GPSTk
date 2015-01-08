#include "BivarStats.hpp"
#include "Vector.hpp"
#include <iostream>

class StatsTest
{
        public: 
		StatsTest(){eps = 1e-12;}// Default Constructor, set the precision value
		~StatsTest() {} // Default Desructor
		double eps;
		int addTest()
		// Test to add data. Want to add single values to empty Stats class.
		// Then add another stat on top with weight. I will use the average to check
		// that data was added and that the data added was correct.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Average is: " << test.average() << std::endl;
			if (test.averageX() != 3) return 1;
			return 0;
		}

		int AverageXTest()
		// Verify the X average calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Average of X is: " << test.averageX() << std::endl;
			if (test.averageX() != 3) return 1;
			return 0;
		}

		int AverageYTest()
		// Verify the Y average calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Average of Y is: " << test.averageY() << std::endl;
			if (test.averageY() != 3) return 1;
			return 0;
		}

		int MaxXTest()
		// Verify the X maximum calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Maximum of X is: " << test.maximumX() << std::endl;
			if (test.maximumX() != 5) return 1;
			return 0;
		}

		int MaxYTest()
		// Verify the Y maximum calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Maximum of Y is: " << test.maximumY() << std::endl;
			if (test.maximumY() != 5) return 1;
			return 0;
		}

		int MinXTest()
		// Verify the X minimum calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Minimum of X is: " << test.minimumX() << std::endl;
			if (test.minimumX() != 1) return 1;
			return 0;
		}

		int MinYTest()
		// Verify the Y minimum calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Minimum of Y is: " << test.minimumY() << std::endl;
			if (test.minimumY() != 1) return 1;
			return 0;
		}

		int VarianceXTest()
		// Verify the X variance calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Variance of X is: " << test.varianceX() << std::endl;
			if (fabs(test.varianceX() - 2.5) > eps) return 1;
			return 0;	
		}

		int VarianceYTest()
		// Verify the Y variance calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Variance of Y is: " << test.varianceY() << std::endl;
			if (fabs(test.varianceY() - 2.5) > eps) return 1;
			return 0;	
		}

		int StdDevXTest()
		// Verify the X Standard Deviation calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Standard Deviation of X is: " << test.stdDevX() << std::endl;
			if (fabs(test.stdDevX() - sqrt(2.5)) > eps) return 1;
			return 0;
		}
		int StdDevYTest()
		// Verify the Y Standard Deviation calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Standard Deviation of Y is: " << test.stdDevY() << std::endl;
			if (fabs(test.stdDevY() - sqrt(2.5)) > eps) return 1;
			return 0;
		}

		int SlopeTest()
		// Verify the Slope calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Slope is: " << test.slope() << std::endl;
			if (fabs(test.slope() - 0.1) > eps) return 1;
			return 0;
		}

		int InterceptTest()
		// Verify the Intercept calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Intercept is: " << test.intercept() << std::endl;
			if (fabs(test.intercept() - 2.7) > eps) return 1;
			return 0;
		}

		int SlopeUncertaintyTest()
		// Verify the Slope Uncertainty calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Slope Uncertainty is: " << test.sigmaSlope() << std::endl;
			if (fabs(test.sigmaSlope() - 0.574456264653803) > eps) return 1;
			return 0;
		}

		int ConditionalUncertaintyTest()
		// Verify the Conditional Uncertainty calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Conditional Uncertainty is: " << test.sigmaYX() << std::endl;
			if (fabs(test.sigmaYX() - 1.81659021245849) > eps) return 1;
			return 0;
		}

		int CorrelationTest()
		// Verify the Correlation calculation.
		{
			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			//std::cout << "The Correlation is: " << test.correlation() << std::endl;
			if (fabs(test.correlation() - 0.1) > eps) return 1;
			return 0;
		}

 };

void checkResult(int check, int& errCount)
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

int main()
{
	int check, errorCounter = 0;
	StatsTest testClass;
	check = testClass.addTest();
        std::cout << "add Result is: ";
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

	return errorCounter;
}
