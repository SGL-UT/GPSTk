#include "PolyFit.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"
#include <iostream>

class PolyFit_T
{
        public: 
		PolyFit_T(){eps = 1e-11;}// Default Constructor, set the precision value
		~PolyFit_T() {} // Default Desructor

		double eps;// Shouldn't this be private?

		/* 	Test to add data. Want to add single values to empty Stats class.
			Then add another stat on top with weight. I will use the average to check
			that data was added and that the data added was correct. */
		int AddTest()
		{
			TestUtil testFramework( "PolyFit", "AddTest", __FILE__, __func__ );
			testFramework.init();

			int n = 4;
			gpstk::PolyFit<double> test(n);
			double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 8, 27, 64, 125};

//--------------PolyFit_addTest_1 - Did the add method function properly?
			try {
				for(int i=0; i<6; i++)
				{
					test.Add(dep[i],indep[i]);
				}
				testFramework.passTest();
			}
			catch(gpstk::Exception e) {testFramework.passTest();}
			catch (...)
			{
				testFramework.failTest();
			}
/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Find a way to compare the stored values to the actual values
			for(int i=0; i<6; i++)
			{
				testFramework.assert(test.?? == (dep[i],indep[i]));
				testFramework.next();
			}					

			//std::cout << test.Solution() << std::endl;
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
*/			
			return testFramework.countFails();
		};

		/*	Test to check that the correct solution is found. 
			The result should be only a 3rd order term. */
		int SolutionTest3()
		{
			TestUtil testFramework( "PolyFit", "SolutionTest3", __FILE__, __func__ );
			testFramework.init();

			int n = 4; //Highest order in polynomial fit + 1 (constant term)
			gpstk::PolyFit<double> test(n); 
			gpstk::Vector<double> soln; // To store the solution
			// Data values to create the fit
			double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 8, 27, 64, 125};
                        // Add the data to the PolyFit object
			for(int i=0; i<6; i++)
			{
				test.Add(dep[i],indep[i]);
			}
			// Store the solution
			soln = test.Solution();
			//std::cout << "Solution is: " << soln << std::endl;
			n = 0;	// Repurposing the n variable for use as a return variable
			for (int i = 0; i<4; i++) // Loop over the solution values
			{
				// Make sure the data matches what is expected
				if ((i != 3 && abs(soln(i)) > eps) || (i == 3 && abs(soln(i) - 1) > eps))
				{
					//std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1; // Increment the return value for each wrong value
				}
			}
//--------------PolyFit_solutionTest3_1 - Was the solution computed correct?
			testFramework.assert(n==0);

			return testFramework.countFails(); // Return the result of the test.
		};
		/*	Test to check that the correct solution is found. 
			The result should be only a 2nd order term. */
		int SolutionTest2()
		{
			TestUtil testFramework( "PolyFit", "SolutionTest2", __FILE__, __func__ );
			testFramework.init();

			int n = 4;
			gpstk::PolyFit<double> test(n);
			gpstk::Vector<double> soln;
			double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 4, 9, 16, 25};

			for(int i=0; i<6; i++)
			{
				test.Add(dep[i],indep[i]);
			}


			soln = test.Solution();
			//std::cout << "Solution is: " << soln << std::endl;
			n = 0;			
			for (int i = 0; i<4; i++)
			{
				if ((i != 2 && abs(soln(i)) > eps) || (i == 2 && abs(soln(i) - 1) > eps))
				{
					//std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1; // Increment the return value for each wrong value
				}
			}

//--------------PolyFit_solutionTest2_1 - Was the solution computed correct?
			testFramework.assert(n==0);

			return testFramework.countFails(); // Return the result of the test.
		};

		/*	Test to check that the correct solution is found. 
			The result should be only a 1st order term. */
		int SolutionTest1()
		{
			TestUtil testFramework( "PolyFit", "SolutionTest1", __FILE__, __func__ );
			testFramework.init();

			int n = 4;
			gpstk::PolyFit<double> test(n);
			gpstk::Vector<double> soln;
			double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 2, 3, 4, 5};

			for(int i=0; i<6; i++)
			{
				test.Add(dep[i],indep[i]);
			}


			soln = test.Solution();
			//std::cout << "Solution is: " << soln << std::endl;
			n = 0;			
			for (int i = 0; i<4; i++)
			{
				if ((i != 1 && abs(soln(i)) > eps) || (i == 1 && abs(soln(i) - 1) > eps))
				{
					//std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1; // Increment the return value for each wrong value
				}
			}

//--------------PolyFit_solutionTest1_1 - Was the solution computed correct?
			testFramework.assert(n==0);

			return testFramework.countFails(); // Return the result of the test.
		};

		/*	Test to check that the correct solution is found. 
			The result should be only a constant term. */
		int SolutionTest0()
		{
			TestUtil testFramework( "PolyFit", "SolutionTest0", __FILE__, __func__ );
			testFramework.init();

			int n = 4;
			gpstk::PolyFit<double> test(n);
			gpstk::Vector<double> soln;
			double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {7, 7, 7, 7, 7, 7};

			for(int i=0; i<6; i++)
			{
				test.Add(dep[i],indep[i]);
			}


			soln = test.Solution();
			//std::cout << "Solution is: " << soln << std::endl;
			n = 0;			
			for (int i = 0; i<4; i++)
			{
				if ((i != 0 && abs(soln(i)) > eps) || (i == 0 && abs(soln(i) - 7) > eps))
				{
					//std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1; // Increment the return value for each wrong value
				}
			}

//--------------PolyFit_solutionTest0_1 - Was the solution computed correct?
			testFramework.assert(n==0);

			return testFramework.countFails(); // Return the result of the test.
		};

		/*	Test to check that the correct solution is found. */
		int SolutionTest()
		{
			TestUtil testFramework( "PolyFit", "SolutionTest", __FILE__, __func__ );
			testFramework.init();

			int n = 4;
			gpstk::PolyFit<double> test(n);
			gpstk::Vector<double> soln;
			double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {2, 8, 30, 80, 170, 312};

			for(int i=0; i<6; i++)
			{
				test.Add(dep[i],indep[i]);
			}


			soln = test.Solution();
			//std::cout << "Solution is: " << soln << std::endl;
			n = 0;			
			for (int i = 0; i<4; i++)
			{
				if (fabs(soln(i) - 2) > eps)
				{
					//std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1; // Increment the return value for each wrong value
				}
			}

			testFramework.assert(n==0);

			return testFramework.countFails(); // Return the result of the test.
		};

		/*	Test to check that the correct solution is found. 
			In this case the problem is singular. */
		int SolutionFailTest()
		{
			TestUtil testFramework( "PolyFit", "SolutionTest", __FILE__, __func__ );
			testFramework.init();

			int n = 4;
			gpstk::PolyFit<double> test(n);
			gpstk::Vector<double> soln;
			// Set the independent variable to the same value
			double indep[6] = {1, 1, 1, 1, 1, 1}, dep[6] = {1, 2, 3, 4, 5, 6};

			for(int i=0; i<6; i++)
			{
				test.Add(dep[i],indep[i]);
			}

			soln = test.Solution(); // This should still work
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	Add a error handler here? "Should work" suggests it's possible to fail...
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			//std::cout << "Solution is: " << test.isSingular() << std::endl;
			n = 0;			

//--------------PolyFit_solutionFailTest_1 - Was the solution computed singular?
			testFramework.assert(test.isSingular()); // The singular flag should be set

			return testFramework.countFails();
		};

		/*	Test to verify the evaluate method with a vector input. */
		int EvaluateVectorTest()
		{
			TestUtil testFramework( "PolyFit", "EvaluateVectorTest", __FILE__, __func__ );
			testFramework.init();

			int n = 4;
			gpstk::PolyFit<double> test(n);
			gpstk::Vector<double> soln(3), eval(3);
			double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 4, 9, 16, 25};

			eval(0) = 6;
			eval(1) = 8;
			eval(2) = 10;
			
			for(int i=0; i<6; i++)
			{
				test.Add(dep[i],indep[i]);
			}


			soln = test.Evaluate(eval);
			//std::cout << "Solution is: " << soln << std::endl;
			n = 0;			
			for (int i = 0; i<3; i++)
			{
				// Using relative error since the soln >> 1
				if (fabs(soln(i) - eval(i)*eval(i))/(eval(i)*eval(i)) > eps) 
				{
					//std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << " Expected Value: " << eval(i)*eval(i) << " Error: " << fabs(soln(i) - eval(i)*eval(i)) << std::endl; 
					n += 1; // Increment the return value for each wrong value
				}
			}
//--------------PolyFit_EvaluateVectorTest_1 - Was the solution computed correct?		
			testFramework.assert(n==0);

			return testFramework.countFails(); // Return the result of the test.
		};

		/* Test to verify the evaluate method with a single input. */
		int EvaluateTest()
		{
			TestUtil testFramework( "PolyFit", "EvaluateTest", __FILE__, __func__ );
			testFramework.init();

			int n = 4;
			gpstk::PolyFit<double> test(n);
			double soln, eval;
			double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 4, 9, 16, 25};

			eval = 6;
			
			for(int i=0; i<6; i++)
			{
				test.Add(dep[i],indep[i]);
			}


			soln = test.Evaluate(eval);
			//std::cout << "Solution is: " << soln << std::endl;

//--------------PolyFit_EvaluateVectorTest_1 - Was the solution computed correct?
			testFramework.assert(fabs(soln - eval*eval) < eps);

			return testFramework.countFails();
		};
 };

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	PolyFit_T testClass;

	check = testClass.AddTest();
	errorCounter += check;

	check = testClass.SolutionTest3();
	errorCounter += check;

	check = testClass.SolutionTest2();
	errorCounter += check;

	check = testClass.SolutionTest1();
	errorCounter += check;

	check = testClass.SolutionTest0();
	errorCounter += check;

	check = testClass.SolutionTest();
	errorCounter += check;

	check = testClass.SolutionFailTest();
	errorCounter += check;

	check = testClass.EvaluateTest();
	errorCounter += check;

	check = testClass.EvaluateVectorTest();
	errorCounter += check;

	std::cout << "Total Errors: " << errorCounter << std::endl;

  	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

   	return errorCounter; //Return the total number of errors
}
