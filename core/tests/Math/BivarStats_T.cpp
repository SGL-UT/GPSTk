#include "BivarStats.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"
#include <iostream>

class BivarStats_T
{
        public: 
		BivarStats_T(){eps = 1e-12;}// Default Constructor, set the precision value
		~BivarStats_T() {} // Default Desructor
		double eps;
		int addTest()
		// Test to add data. Want to add single values to empty Stats class.
		// Then add another stat on top with weight. I will use the average to check
		// that data was added and that the data added was correct.
		{
                   gpstk::TestUtil testFramework("BivarStats","Add()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The Add method was unsuccessful";
			//std::cout << "The Average is: " << test.average() << std::endl;
			testFramework.assert(test.averageX() == 3, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int AverageXTest()
		// Verify the X average calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","AverageX()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The AverageX method was unsuccessful";
			//std::cout << "The Average of X is: " << test.averageX() << std::endl;
			testFramework.assert(test.averageX() == 3, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int AverageYTest()
		// Verify the Y average calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","AverageY()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The AverageY method was unsuccessful";
			//std::cout << "The Average of Y is: " << test.averageY() << std::endl;
			testFramework.assert(test.averageY() == 3, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int MaxXTest()
		// Verify the X maximum calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","MaximumX()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The MaximumX method was unsuccessful";
			//std::cout << "The Maximum of X is: " << test.maximumX() << std::endl;
			testFramework.assert(test.maximumX() == 5, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int MaxYTest()
		// Verify the Y maximum calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","MaximumY()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The MaximumY method was unsuccessful";
			//std::cout << "The Maximum of Y is: " << test.maximumY() << std::endl;
			testFramework.assert(test.maximumY() == 5, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int MinXTest()
		// Verify the X minimum calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","MinimumX()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The MinimumX method was unsuccessful";
			//std::cout << "The Minimum of X is: " << test.minimumX() << std::endl;
			testFramework.assert(test.minimumX() == 1, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int MinYTest()
		// Verify the Y minimum calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","MinimumY()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The MinimumY method was unsuccessful";
			//std::cout << "The Minimum of Y is: " << test.minimumY() << std::endl;
			testFramework.assert(test.minimumY() == 1, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int VarianceXTest()
		// Verify the X variance calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","VarianceX()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The VarianceX method was unsuccessful";
			//std::cout << "The Variance of X is: " << test.varianceX() << std::endl;
			testFramework.assert(fabs(test.varianceX() - 2.5) < eps, failMesg, __LINE__);
			return testFramework.countFails();	
		}

		int VarianceYTest()
		// Verify the Y variance calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","VarianceY()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The VarianceY method was unsuccessful";
			//std::cout << "The Variance of Y is: " << test.varianceY() << std::endl;
			testFramework.assert(fabs(test.varianceY() - 2.5) < eps, failMesg, __LINE__);
			return testFramework.countFails();	
		}

		int StdDevXTest()
		// Verify the X Standard Deviation calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","StdDevX()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The stdDevX method was unsuccessful";
			//std::cout << "The Standard Deviation of X is: " << test.stdDevX() << std::endl;
			testFramework.assert(fabs(test.stdDevX() - sqrt(2.5)) < eps, failMesg, __LINE__);
			return testFramework.countFails();
		}
		int StdDevYTest()
		// Verify the Y Standard Deviation calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","StdDevY()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The stdDevY method was unsuccessful";
			//std::cout << "The Standard Deviation of Y is: " << test.stdDevY() << std::endl;
			testFramework.assert(fabs(test.stdDevY() - sqrt(2.5)) < eps, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int SlopeTest()
		// Verify the Slope calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","Slope()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The Slope method was unsuccessful";
			//std::cout << "The Slope is: " << test.slope() << std::endl;
			testFramework.assert(fabs(test.slope() - 0.1) < eps, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int InterceptTest()
		// Verify the Intercept calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","Intercept()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The Intercept method was unsuccessful";
			//std::cout << "The Intercept is: " << test.intercept() << std::endl;
			testFramework.assert(fabs(test.intercept() - 2.7) < eps, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int SlopeUncertaintyTest()
		// Verify the Slope Uncertainty calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","SlopeUncertainty()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The sigmaSlope method was unsuccessful";
			//std::cout << "The Slope Uncertainty is: " << test.sigmaSlope() << std::endl;
			testFramework.assert(fabs(test.sigmaSlope() - 0.574456264653803) < eps, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int ConditionalUncertaintyTest()
		// Verify the Conditional Uncertainty calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","ConditionalUncertaintyTest()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The sigmaYX method was unsuccessful";
			//std::cout << "The Conditional Uncertainty is: " << test.sigmaYX() << std::endl;
			testFramework.assert(fabs(test.sigmaYX() - 1.81659021245849) < eps, failMesg, __LINE__);
			return testFramework.countFails();
		}

		int CorrelationTest()
		// Verify the Correlation calculation.
		{
			gpstk::TestUtil testFramework("BivarStats","Correlation()",__FILE__,__LINE__);
			std::string failMesg;

			gpstk::BivarStats<double> test;
			test.add(1, 2);		
			test.add(2, 5);
			test.add(3, 1);
			test.add(4, 4);
			test.add(5, 3);

			failMesg  = "The correlation method was unsuccessful";
			//std::cout << "The Correlation is: " << test.correlation() << std::endl;
			testFramework.assert(fabs(test.correlation() - 0.1) < eps, failMesg, __LINE__);
			return testFramework.countFails();
		}

 };

int main()
{
	int check, errorCounter = 0;
	BivarStats_T testClass;

	check = testClass.addTest();
	errorCounter += check;

	check = testClass.AverageXTest();
	errorCounter += check;

	check = testClass.AverageYTest();
	errorCounter += check;

	check = testClass.MaxXTest();
	errorCounter += check;

	check = testClass.MaxYTest();
	errorCounter += check;

	check = testClass.MinXTest();
	errorCounter += check;

	check = testClass.MinYTest();
	errorCounter += check;

	check = testClass.VarianceXTest(); 
	errorCounter += check;

	check = testClass.VarianceYTest(); 
	errorCounter += check;

	check = testClass.StdDevXTest(); 
	errorCounter += check;

	check = testClass.StdDevYTest(); 
	errorCounter += check;

	check = testClass.SlopeTest(); 
	errorCounter += check;

	check = testClass.InterceptTest(); 
	errorCounter += check;

	check = testClass.ConditionalUncertaintyTest(); 
	errorCounter += check;

	check = testClass.SlopeUncertaintyTest(); 
	errorCounter += check;

	check = testClass.CorrelationTest(); 
	errorCounter += check;

	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter;
}
