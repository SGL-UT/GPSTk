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

#include "PolyFit.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"
#include <iostream>

class PolyFit_T
{
public: 
   PolyFit_T(){eps = 1e-11;}// Default Constructor, set the precision value
   ~PolyFit_T() {} // Default Desructor

//==========================================================================================================================
//	constructorTest ensures the constructors set the object properly
//==========================================================================================================================
   int constructorTest(void)
   {
      gpstk::TestUtil testFramework( "PolyFit", "Constructor", __FILE__, __LINE__ );
      try
      {
         //---------------------------------------------------------------------
         //Test the default constructor
         //---------------------------------------------------------------------
         try
         {
            gpstk::PolyFit<double> PolyCheck;
            testFramework.assert(true, "Default constructor successfully built a PolyFit object", __LINE__);
         }
         catch(...){testFramework.assert(false, "Unexpected exception thrown during default construction of a PolyFit object", __LINE__); }
         gpstk::PolyFit<double> Poly;
         testFramework.assert((unsigned) 0 == Poly.N()     , "Default constructor created an object with data in it"    , __LINE__);
         testFramework.assert((unsigned) 0 == Poly.Degree(), "Default constructor allows for non-constant fits"         , __LINE__);
         testFramework.assert(Poly.isSingular()            , "The fit found after default construction was not singular", __LINE__);

         //---------------------------------------------------------------------
         //Test the explicit constructor
         //---------------------------------------------------------------------
         try
         {
            gpstk::PolyFit<double> PolyCheck(4);
            testFramework.assert(true, "Explicit constructor successfully built a PolyFit object", __LINE__);
         }
         catch(...){testFramework.assert(false, "Unexpected exception thrown during explicit construction of a PolyFit object", __LINE__); }
         gpstk::PolyFit<double> Poly4((unsigned) 4);
         testFramework.assert((unsigned) 0 == Poly4.N()     , "Explicit constructor created an object with data in it"            , __LINE__);
         testFramework.assert((unsigned) 4 == Poly4.Degree(), "Explicit constructor does not fit polynomials of the correct order", __LINE__);
         testFramework.assert(Poly4.isSingular()            , "The fit found after explicit construction was not singular"        , __LINE__);
      }
      catch (gpstk::Exception& e) {}

      return testFramework.countFails();
   }


//==========================================================================================================================
//	Test to add data. Want to add single values to empty Stats class.
//	Then add another stat on top with weight. I will use the average to check
//	that data was added and that the data added was correct.
//==========================================================================================================================
   int AddTest()
   {
      gpstk::TestUtil testFramework( "PolyFit", "Add", __FILE__, __LINE__ );
      std::string failMesg;

      int n = 4;
      gpstk::PolyFit<double> testSingle(n), testGpstkVector(n), testStdVector(n);
      double indepSingle[6] = {0, 1, 2, 3, 4, 5}, depSingle[6] = {0, 1, 8, 27, 64, 125};
      gpstk::Vector<double> indepGpstkVector(6,0.), depGpstkVector(6,0.);
      std::vector<double>   indepStdVector(6,0.)  , depStdVector(6,0.);

      for(int i=0; i<6; i++)
      {
         indepGpstkVector[i] = indepSingle[i];
         indepStdVector[i]   = indepSingle[i];	
         depGpstkVector[i]   = depSingle[i];
         depStdVector[i]     = depSingle[i];			
      }			

      //---------------------------------------------------------------------
      //Test the single entry Add
      //---------------------------------------------------------------------
      try 
      {
         //Can single values be added
         for(int i=0; i<6; i++)
         {
            testSingle.Add(depSingle[i],indepSingle[i]);
         }
         //Were all single values added successfully?
         testFramework.assert(testSingle.N() == 6, "Not all single adds were successful", __LINE__);

      }
      catch(...){testFramework.assert(false, "Exception thrown during Single add", __LINE__);}		

      //---------------------------------------------------------------------
      //Test the gpstk::Vector Add
      //---------------------------------------------------------------------
      try 
      {
         //Check that all values can be added with a gpstk::Vector
         testGpstkVector.Add(indepGpstkVector, depGpstkVector);
         testFramework.assert(testGpstkVector.N() == 6, "Not all gpstk::Vector adds were successful", __LINE__);		
      }
      catch(...){testFramework.assert(false, "Exception thrown during gpstk::Vector add", __LINE__);}	


      //---------------------------------------------------------------------
      //Test the std::vector Add
      //---------------------------------------------------------------------
      try 
      {
         //Check that all values can be added with a std::Vector
         testStdVector.Add(indepStdVector, depStdVector);
         testFramework.assert(testStdVector.N() == 6, "Not all std::vector adds were successful", __LINE__);	
      }
      catch(...){testFramework.assert(false, "Exception thrown during std::vector add", __LINE__);}	


      return testFramework.countFails();
   }



//==========================================================================================================================
//	This test is designed to test the validity of the reset member of the PolyFit class
//	Reset is tested by first adding data to a blank PolyFit object and then clearing
//	that data (Please note this dad was already tested in the previous test)

//	Please note isSingular, Solution, Degreem N and Solve were tested inderectly here
//==========================================================================================================================
   int resetTest(void)
   {
      gpstk::TestUtil testFramework( "PolyFit", "Reset", __FILE__, __LINE__ );
      bool covMatDiffBool = true;
      bool solnDiffBool = true;	
		
      //Polynomial will be reset without user inputed parameter
      gpstk::PolyFit<double> resetPolyD(2);
	
      //Poly will be reset with a parameter
      gpstk::PolyFit<double> resetPolyP(2);
	

      double data[4] = {0.,2.,4.,-1.};
      double time[4] = {3.,3.,4.,2.,};
	
      for (int i =0;i<4;i++)
      {
         resetPolyD.Add(time[i],data[i]);
         resetPolyP.Add(time[i],data[i]);
      }

      //---------------------------------------------------------------------
      //Test Reset()
      //---------------------------------------------------------------------	
      resetPolyD.Reset();
	
      gpstk::Matrix<double> Blank(2,2,0.);
      gpstk::Vector<double> Zero(2,0.);
	
      gpstk::Vector<double> resetPolyDSolution = resetPolyD.Solution();
      gpstk::Matrix<double> resetPolyDCov = resetPolyD.Covariance();
	
      testFramework.assert((unsigned) 0 == resetPolyD.N()     , "Reset did not set the datapoint counter to zero"   , __LINE__);
      testFramework.assert((unsigned) 2 == resetPolyD.Degree(), "Reset did not maintain the maximum fit degree as 2", __LINE__);
      testFramework.assert(resetPolyD.isSingular()            , "The fit found after Reset was not singular"        , __LINE__);
      for (int i = 0; i<2; i++)
      {
         for (int j = 0; j<2; j++)
         {
            covMatDiffBool = covMatDiffBool && (Blank[i][j] == resetPolyDCov[i][j]);
         }
         solnDiffBool = solnDiffBool && (fabs(resetPolyDSolution[i]) < eps);
      }
      testFramework.assert(covMatDiffBool, "Covariance matrix found to be nonzero after Reset", __LINE__);
      testFramework.assert(solnDiffBool  , "Solution vector found to be nonzero after Reset"  , __LINE__);


      //---------------------------------------------------------------------
      //Test Reset(int)
      //---------------------------------------------------------------------		
      resetPolyP.Reset((unsigned) 3);

      covMatDiffBool = true;
      solnDiffBool = true;	

      gpstk::Matrix<double> BlankP(3,3,0.);
      gpstk::Vector<double> ZeroP(3,0.);
	
      gpstk::Vector<double> resetPolyPSolution = resetPolyP.Solution();
      gpstk::Matrix<double> resetPolyPCov = resetPolyP.Covariance();
	
      testFramework.assert((unsigned) 0 == resetPolyP.N()     , "Reset(int) did not set the datapoint counter to zero" , __LINE__);
      testFramework.assert((unsigned) 3 == resetPolyP.Degree(), "Reset(int) did not change the maximum fit degree to 3", __LINE__);
      testFramework.assert(resetPolyP.isSingular()            , "The fit found after Reset(int) was not singular"      , __LINE__);
      for (int i = 0; i<3; i++)
      {
         for (int j = 0; j<3; j++)
         {
            covMatDiffBool = covMatDiffBool && (fabs(resetPolyPCov[i][j]) < eps);
         }
         solnDiffBool = solnDiffBool && (fabs(resetPolyPSolution[i]) < eps);
      }
      testFramework.assert(covMatDiffBool, "Covariance matrix found to be nonzero after Reset(int)", __LINE__);
      testFramework.assert(solnDiffBool  , "Solution vector found to be nonzero after Reset(int)"  , __LINE__);
			
      return testFramework.countFails();
   }



//==========================================================================================================================
//	Test to check that the correct solution is found. 
//	The result should be only a 3rd order term.
//==========================================================================================================================
   int SolutionTest3rdOrderResult()
   {
      gpstk::TestUtil testFramework( "PolyFit", "Solution", __FILE__, __LINE__ );
      std::string failMesg;

      int n = 4; //Highest order in polynomial fit + 1 (constant term)
      int errorCounter = 0;
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
      for (int i = 0; i<4; i++) // Loop over the solution values
      {

         // Make sure the data matches what is expected
         // Expect soln = {0, 0, 0, 1}
         if ((i != 3 && std::abs(soln[i]) > eps) || (i == 3 && std::abs(soln[i] - 1) > eps))
         {
            //std::cout << "i: " << i << " value: " << soln[i] << " absVal: " << fabs(soln[i]) << " fabsVal(soln-1): " << fabs(soln[i]-1) << std::endl; 
            errorCounter += 1; // Increment the return value for each wrong value
         }
      }
      failMesg = "Was the solution computed correct?";
      testFramework.assert(errorCounter == 0, failMesg, __LINE__);

      return testFramework.countFails(); // Return the result of the test.
   }



//==========================================================================================================================
//	Test to check that the correct solution is found. 
//	The result should be only a 2nd order term.
//==========================================================================================================================
   int SolutionTest2ndOrderResult()
   {
      gpstk::TestUtil testFramework( "PolyFit", "Solution", __FILE__, __LINE__ );
      std::string failMesg;

      int n = 4;
      int errorCounter = 0;
      gpstk::PolyFit<double> test(n);
      gpstk::Vector<double> soln;
      double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 4, 9, 16, 25};

      for(int i=0; i<6; i++)
      {
         test.Add(dep[i],indep[i]);
      }


      soln = test.Solution();
      //std::cout << "Solution is: " << soln << std::endl;		
      for (int i = 0; i<4; i++)
      {
         // Make sure the data matches what is expected
         // Expect soln = {0, 0, 1, 0}
         if ((i != 2 && std::abs(soln[i]) > eps) || (i == 2 && std::abs(soln[i] - 1) > eps))
         {
            //std::cout << "i: " << i << " value: " << soln[i] << " absVal: " << fabs(soln[i]) << " fabsVal(soln-1): " << fabs(soln[i]-1) << std::endl; 
            errorCounter += 1; // Increment the return value for each wrong value
         }
      }

      failMesg = "Was the solution computed correct?";
      testFramework.assert(errorCounter==0, failMesg, __LINE__);

      return testFramework.countFails(); // Return the result of the test.
   }


//==========================================================================================================================
//	Test to check that the correct solution is found. 
//	The result should be only a 1st order term.
//==========================================================================================================================
   int SolutionTest1stOrderResult()
   {
      gpstk::TestUtil testFramework( "PolyFit", "Solution", __FILE__, __LINE__ );
      std::string failMesg;

      int n = 4;
      int errorCounter = 0;
      gpstk::PolyFit<double> test(n);
      gpstk::Vector<double> soln;
      double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 2, 3, 4, 5};

      for(int i=0; i<6; i++)
      {
         test.Add(dep[i],indep[i]);
      }


      soln = test.Solution();
      //std::cout << "Solution is: " << soln << std::endl;			
      for (int i = 0; i<4; i++)
      {
         // Make sure the data matches what is expected
         // Expect soln = {0, 1, 0, 0}
         if ((i != 1 && std::abs(soln[i]) > eps) || (i == 1 && std::abs(soln[i] - 1) > eps))
         {
            //std::cout << "i: " << i << " value: " << soln[i] << " absVal: " << fabs(soln[i]) << " fabsVal(soln-1): " << fabs(soln[i]-1) << std::endl; 
            errorCounter += 1; // Increment the return value for each wrong value
         }
      }

      failMesg = "Was the solution computed correct?";
      testFramework.assert(errorCounter==0, failMesg, __LINE__);

      return testFramework.countFails(); // Return the result of the test.
   }


//==========================================================================================================================
//	Test to check that the correct solution is found. 
//	The result should be only a 0th order term.
//==========================================================================================================================
   int SolutionTest0thOrderResult()
   {
      gpstk::TestUtil testFramework( "PolyFit", "Solution", __FILE__, __LINE__ );
      std::string failMesg;

      int n = 4;
      int errorCounter = 0;
      gpstk::PolyFit<double> test(n);
      gpstk::Vector<double> soln;
      double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {7, 7, 7, 7, 7, 7};

      for(int i=0; i<6; i++)
      {
         test.Add(dep[i],indep[i]);
      }


      soln = test.Solution();
      //std::cout << "Solution is: " << soln << std::endl;		
      for (int i = 0; i<4; i++)
      {
         // Make sure the data matches what is expected
         // Expect soln = {7, 0, 0, 0}
         if ((i != 0 && std::abs(soln[i]) > eps) || (i == 0 && std::abs(soln[i] - 7) > eps))
         {
            //std::cout << "i: " << i << " value: " << soln[i] << " absVal: " << fabs(soln[i]) << " fabsVal(soln-1): " << fabs(soln[i]-1) << std::endl; 
            errorCounter += 1; // Increment the return value for each wrong value
         }
      }

      failMesg = "Was the solution computed correct?";
      testFramework.assert(errorCounter==0, failMesg, __LINE__);

      return testFramework.countFails(); // Return the result of the test.
   }


//==========================================================================================================================
//	Test to check that the correct solution is found.
//==========================================================================================================================
   int SolutionTest()
   {
      gpstk::TestUtil testFramework( "PolyFit", "Solution", __FILE__, __LINE__ );
      std::string failMesg;

      int n = 4;
      int errorCounter = 0;
      gpstk::PolyFit<double> test(n);
      gpstk::Vector<double> soln;
      double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {2, 8, 30, 80, 170, 312};

      for(int i=0; i<6; i++)
      {
         test.Add(dep[i],indep[i]);
      }


      soln = test.Solution();
      //std::cout << "Solution is: " << soln << std::endl;		
      for (int i = 0; i<4; i++)
      {
         // Make sure the data matches what is expected
         // Expect soln = {2, 2, 2, 2}
         if (fabs(soln[i] - 2) > eps)
         {
            //std::cout << "i: " << i << " value: " << soln[i] << " absVal: " << fabs(soln[i]) << " fabs(val-2): " << fabs(soln[i] - 2) << std::endl; 
            errorCounter += 1; // Increment the return value for each wrong value
         }
      }

      failMesg = "Was the solution computed correct?";
      testFramework.assert(errorCounter==0, failMesg, __LINE__);

      return testFramework.countFails(); // Return the result of the test.
   }


//==========================================================================================================================
//	Test to check that the correct solution is found. 
//	In this case the problem is singular.
//==========================================================================================================================
   int SolutionFailTest()
   {
      gpstk::TestUtil testFramework( "PolyFit", "Solution", __FILE__, __LINE__ );
      std::string failMesg;

      int n = 4;
      gpstk::PolyFit<double> test(n);
      gpstk::Vector<double> soln;
      // Set the independent variable to the same value
      double indep[6] = {1, 1, 1, 1, 1, 1}, dep[6] = {1, 2, 3, 4, 5, 6};

      for(int i=0; i<6; i++)
      {
         test.Add(dep[i],indep[i]);
      }

      soln = test.Solution(); // This might NaN

      //std::cout << "Solution is: " << test.isSingular() << std::endl;		

      failMesg = "Was the solution computed singular?";
      testFramework.assert(test.isSingular(), failMesg, __LINE__); // The singular flag should be set

      return testFramework.countFails();
   }


//==========================================================================================================================
//	Test to verify the evaluate method with a vector input.
//==========================================================================================================================
   int EvaluateVectorTest()
   {
      gpstk::TestUtil testFramework( "PolyFit", "Evaluate", __FILE__, __LINE__ );
      std::string failMesg;

      int n = 4;
      gpstk::PolyFit<double> test(n);
      gpstk::Vector<double> soln(3,0.), eval(3,0.);
      double indep[6] = {0, 1, 2, 3, 4, 5}, dep[6] = {0, 1, 4, 9, 16, 25};

      eval[0] = 6;
      eval[1] = 8;
      eval[2] = 10;
			
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
         if (fabs(soln[i] - eval[i]*eval[i])/(eval[i]*eval[i]) > eps) 
         {
            n += 1; // Increment the return value for each wrong value
         }
      }
      failMesg = "Was the solution computed correct?";
      testFramework.assert(n==0, failMesg, __LINE__);

      return testFramework.countFails(); // Return the result of the test.
   }


//==========================================================================================================================
//	Test to verify the evaluate method with a single input.
//==========================================================================================================================
   int EvaluateTest()
   {
      gpstk::TestUtil testFramework( "PolyFit", "Evaluate", __FILE__, __LINE__ );
      std::string failMesg;

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

      failMesg = "Was the solution computed correct?";
      testFramework.assert(fabs(soln - eval*eval) < eps, failMesg, __LINE__);

      return testFramework.countFails();
   }
private:
   double eps;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	PolyFit_T testClass;

	check = testClass.constructorTest();
	errorCounter += check;

	check = testClass.AddTest();
	errorCounter += check;

	check = testClass.resetTest();
	errorCounter += check;

	check = testClass.SolutionTest3rdOrderResult();
	errorCounter += check;

	check = testClass.SolutionTest2ndOrderResult();
	errorCounter += check;

	check = testClass.SolutionTest1stOrderResult();
	errorCounter += check;

	check = testClass.SolutionTest0thOrderResult();
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
