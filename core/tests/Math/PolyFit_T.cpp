#include "PolyFit.hpp"
#include "Vector.hpp"
#include <iostream>

class PolyFitTest
{
        public: 
		PolyFitTest(){eps = 1e-11;}// Default Constructor, set the precision value
		~PolyFitTest() {} // Default Desructor
		double eps;
		int AddTest()
		// Test to add data. Want to add single values to empty Stats class.
		// Then add another stat on top with weight. I will use the average to check
		// that data was added and that the data added was correct.
		{
			int n = 4;
			gpstk::PolyFit<double> test(n);
			double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 8, 27, 64, 125};

			for(int i=0; i<6; i++)
			{
				test.Add(dep[i],indep[i]);
			}

			//std::cout << test.Solution() << std::endl;
			return 0;
			
		};
		int SolutionTest3()
		// Test to check that the correct solution is found. 
		// The result should be only a 3rd order term.
		{
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
					std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1; // Increment the return value for each wrong value
				}
			}
			return n; // Return the result of the test.
			
		};
		int SolutionTest2()
		// Test to check that the correct solution is found. 
		// The result should be only a 2nd order term.
		{
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
					std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1;
				}
			}
			return n;
			
		};
		int SolutionTest1()
		// Test to check that the correct solution is found. 
		// The result should be only a 1st order term.
		{
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
					std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1;
				}
			}
			return n;
			
		};
		int SolutionTest0()
		// Test to check that the correct solution is found. 
		// The result should be only a constant term.
		{
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
					std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1;
				}
			}
			return n;
			
		};
		int SolutionTest()
		// Test to check that the correct solution is found. 
		// The result should be 2 for all coefficients.
		{
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
				if (abs(soln(i) - 2) > eps)
				{
					std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1;
				}
			}
			return n;
			
		};
		int SolutionFailTest()
		// Test to check that the correct solution is found. 
		// In this case the problem is singular.
		{
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
			//std::cout << "Solution is: " << test.isSingular() << std::endl;
			n = 0;			
			if (test.isSingular())return 0; // The singular flag should be set
			else return 1;

			
		};
		int EvaluateVectorTest()
		// Test to verify the evaluate method with a vector input.
		{
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
				if (abs(soln(i) - eval(i)*eval(i)) > eps)
				{
					std::cout << "i: " << i << " value: " << soln(i) << " absVal: " << abs(soln(i)) << std::endl; 
					n += 1;
				}
			}
			return n;
			
		};

		int EvaluateTest()
		// Test to verify the evaluate method with a single input.
		{
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
			if (abs(soln - eval*eval) > eps) return 1;
			else return 0;
			
		};
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
	PolyFitTest testClass;
	check = testClass.AddTest();
        std::cout << "Add Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.SolutionTest3();
        std::cout << "Solution(3) Result is: " << check << ", ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.SolutionTest2();
        std::cout << "Solution(2) Result is: " << check << ", ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.SolutionTest1();
        std::cout << "Solution(1) Result is: " << check << ", ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.SolutionTest0();
        std::cout << "Solution(0) Result is: " << check << ", ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.SolutionTest();
        std::cout << "Solution Result is: " << check << ", ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.SolutionFailTest();
        std::cout << "SolutionFail Result is: " << check << ", ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.EvaluateTest();
        std::cout << "Evaluate Result is: " << check << ", ";
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.EvaluateVectorTest();
        std::cout << "Evaluate Vector Result is: " << check << ", ";
	checkResult(check, errorCounter);
	check = -1;

	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
