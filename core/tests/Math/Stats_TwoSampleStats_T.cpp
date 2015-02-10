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

class Stats_TwoSampleStats_T
{
        public: 
		Stats_TwoSampleStats_T(){eps = 1e-12;}// Default Constructor, set the precision value
		~Stats_TwoSampleStats_T() {} // Default Desructor
		double eps; //shouldn't this be private?

		/*	Test to add data. Want to add single values to empty Stats class.
			Then add another stat on top with weight. I will use the average to check
			that data was added and that the data added was correct. */
		int AddTest()
		{
			TestUtil testFramework( "TwoSampleStats", "Add", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<int> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Average is: " << test.Average() << std::endl;
			testFramework.assert(test.N() == 5);

			return testFramework.countFails();
		}
		/*	Verify the X average calculation. */
		int AverageXTest()
		{
			TestUtil testFramework( "TwoSampleStats", "AverageX", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Average of X is: " << test.AverageX() << std::endl;
			testFramework.assert(test.AverageX() == 3);

			return testFramework.countFails();
		}
		/* Verify the Y average calculation. */
		int AverageYTest()
		{
			TestUtil testFramework( "TwoSampleStats", "AverageY", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Average of Y is: " << test.AverageY() << std::endl;
			testFramework.assert(test.AverageY() == 3);

			return testFramework.countFails();
		}
		/* Verify the X maximum calculation. */
		int MaxXTest()
		{
			TestUtil testFramework( "TwoSampleStats", "MaxX", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Maximum of X is: " << test.MaximumX() << std::endl;
			testFramework.assert(test.MaximumX() == 5);

			return testFramework.countFails();
		}
		/*	Verify the Y maximum calculation. */
		int MaxYTest()
		{
			TestUtil testFramework( "TwoSampleStats", "MaxY", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Maximum of Y is: " << test.MaximumY() << std::endl;
			testFramework.assert(test.MaximumY() == 5);

			return testFramework.countFails();
		}
		/*	Verify the X minimum calculation. */
		int MinXTest()
		{
			TestUtil testFramework( "TwoSampleStats", "MinX", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Minimum of X is: " << test.MinimumX() << std::endl;
			testFramework.assert(test.MinimumX() == 1);

			return testFramework.countFails();
		}
		/*	Verify the Y minimum calculation. */
		int MinYTest()
		{
			TestUtil testFramework( "TwoSampleStats", "MinY", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Minimum of Y is: " << test.MinimumY() << std::endl;
			testFramework.assert(test.MinimumY() == 1);

			return testFramework.countFails();
		}
		/* Verify the X variance calculation. */
		int VarianceXTest()
		{
			TestUtil testFramework( "TwoSampleStats", "VarianceX", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Variance of X is: " << test.VarianceX() << std::endl;
			testFramework.assert(fabs(test.VarianceX() - 2.5) < eps);

			return testFramework.countFails();	
		}
		/* Verify the Y variance calculation. */
		int VarianceYTest()
		{
			TestUtil testFramework( "TwoSampleStats", "VarianceY", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Variance of Y is: " << test.VarianceY() << std::endl;
			testFramework.assert(fabs(test.VarianceY() - 2.5) < eps);

			return testFramework.countFails();	
		}
		/* Verify the X Standard Deviation calculation. */
		int StdDevXTest()
		{
			TestUtil testFramework( "TwoSampleStats", "StdDevX", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Standard Deviation of X is: " << test.StdDevX() << std::endl;
			testFramework.assert(fabs(test.StdDevX() - sqrt(2.5)) < eps);

			return testFramework.countFails();
		}
		/*	Verify the Y Standard Deviation calculation. */		
		int StdDevYTest()
		{
			TestUtil testFramework( "TwoSampleStats", "StdDevY", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Standard Deviation of Y is: " << test.StdDevY() << std::endl;
			testFramework.assert(fabs(test.StdDevY() - sqrt(2.5)) < eps);

			return testFramework.countFails();
		}
		/*	Verify the Slope calculation. */
		int SlopeTest()
		{
			TestUtil testFramework( "TwoSampleStats", "Slope", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Slope is: " << test.Slope() << std::endl;
			testFramework.assert(fabs(test.Slope() - 0.1) < eps);

			return testFramework.countFails();
		}
		/* Verify the Intercept calculation. */
		int InterceptTest()
		{
			TestUtil testFramework( "TwoSampleStats", "Intercept", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Intercept is: " << test.Intercept() << std::endl;
			testFramework.assert(fabs(test.Intercept() - 2.7) < eps);

			return testFramework.countFails();
		}
		/*	Verify the Slope Uncertainty calculation. */
		int SlopeUncertaintyTest()
		{
			TestUtil testFramework( "TwoSampleStats", "SlopeUncertainty", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Slope Uncertainty is: " << test.SigmaSlope() << std::endl;
			testFramework.assert(fabs(test.SigmaSlope() - 0.574456264653803) < eps);

			return testFramework.countFails();
		}
		/* Verify the Conditional Uncertainty calculation. */
		int ConditionalUncertaintyTest()
		{
			TestUtil testFramework( "TwoSampleStats", "ConditionalUncertainty", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Conditional Uncertainty is: " << test.SigmaYX() << std::endl;
			testFramework.assert(fabs(test.SigmaYX() - 1.81659021245849) < eps);

			return testFramework.countFails();
		}
		/* Verify the Correlation calculation. */
		int CorrelationTest()
		{
			TestUtil testFramework( "TwoSampleStats", "Correlation", __FILE__, __LINE__ );
			testFramework.init();

			gpstk::TwoSampleStats<double> test;
			test.Add(1, 2);		
			test.Add(2, 5);
			test.Add(3, 1);
			test.Add(4, 4);
			test.Add(5, 3);

			//std::cout << "The Correlation is: " << test.Correlation() << std::endl;
			testFramework.assert(fabs(test.Correlation() - 0.1) < eps);

			return testFramework.countFails();
		}

 };

int main()//Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	Stats_TwoSampleStats_T testClass;

	check = testClass.AddTest();
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
	
	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter;//Return the total number of errors
}
