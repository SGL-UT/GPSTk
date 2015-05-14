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

#include "Triple.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <cmath>
using namespace std;
using namespace gpstk;
class TripleTest
{
        public: 
		TripleTest(){eps = 1e-12;}// Default Constructor, set the precision value
		~TripleTest() {} // Default Desructor
		double eps;// Shouldn't this be private?

		/*	Test to initialize and set Triple objects. */
		int setTest() 
		{
			TestUtil testFramework( "Triple", "Set", __FILE__, __LINE__ );
			std::string failMesg;

			Triple test, test2(1,2,3), test3(test2);
			test = valarray<double>(3);		

			//std::cout << "The Average is: " << test.Average() << std::endl;

			failMesg = "Was tje Triple created correctly?";			
			testFramework.assert((test.size() == 3) && (test2.size() == 3) && (test3.size() == 3), failMesg, __LINE__);
			
			return testFramework.countFails();
		}
		/* Verify the dot product calculation. */		
		int dotTest()
		{
			TestUtil testFramework( "Triple", "Dot", __FILE__, __LINE__ );
			std::string failMesg;

			Triple test(1,2,3),test2(2,2,2);
			double result;

			result = test.dot(test2);

			//std::cout << "The dot product is: " << result << std::endl;

			failMesg = "Did the dot method function properly?";			
			testFramework.assert(result == 12, failMesg, __LINE__);

			return testFramework.countFails();	
		}
		/*	Verify the cross product calculation. */
		int crossTest()
		{
			TestUtil testFramework( "Triple", "Cross", __FILE__, __LINE__ );
			std::string failMesg;

			Triple test(1,2,3), test2(2,2,2), test3;
			
			test3 = test.cross(test2);

			//std::cout << "The cross product is: " << test3 << std::endl;

			failMesg = "Did the method function properly?";			
			testFramework.assert((test3[0] == -2) && (test3[1] == 4) && (test3[2] == -2), failMesg, __LINE__);
			return testFramework.countFails();
		}
		/*	Verify the magnitude calculation.
			Note this presumes the L2 (Euclidian) norm of the vector. */		
		int magTest()
		{
			TestUtil testFramework( "Triple", "Mag", __FILE__, __LINE__ );
			std::string failMesg;

			Triple test(3,4,0);

			failMesg = "Did the calculation return the correct values?"; 
			testFramework.assert(test.mag() == 5, failMesg, __LINE__);

			test[0] = 0; test[1] = 0; test[2] = -2;

			failMesg = "Did the calculation return the correct values?"; 
			testFramework.assert(test.mag() == 2, failMesg, __LINE__);

			return testFramework.countFails();
		}
		/*	Verify the unit vector calculation. */
		int unitVectorTest()
		{
			TestUtil testFramework( "Triple", "unitVector", __FILE__, __LINE__ );
			std::string failMesg;

			Triple test(3,4,0),test2;
			test2 = test.unitVector();
			
			failMesg = "Did the calculation return the correct values?"; 	
			testFramework.assert( (fabs(test2[0] - 3.0/5.0)*5.0/3.0 < eps) && (fabs(test2[1] - 4.0/5.0)*5.0/4.0 < eps) && (fabs(test2[2]) < eps) , failMesg, __LINE__);

			test[0] = 0; test[1] = 0; test[2] = -2;
			test2 = test.unitVector();

			failMesg = "Did the calculation return the correct values?"; 
			testFramework.assert((test2[0] == 0) && (test2[1] == 0) && (test2[2] == -1.0), failMesg, __LINE__);

			return testFramework.countFails();	
		}
		/*	Verify the cosine of the angle between two triples calculation. */
		int cosVectorTest()
		{
			TestUtil testFramework( "Triple", "cosVector", __FILE__, __LINE__ );
			std::string failMesg;

			Triple test(1,0,1),test2(-1,0,-1);
            
			failMesg = "Is the computed cosine value correct?";		
			testFramework.assert(fabs(test.cosVector(test2) + 1) < eps, failMesg, __LINE__);

			test2[0] = 0; test2[1] = 1; test2[2] = 0;

			failMesg = "Is the computed cosine value correct?";
			testFramework.assert(fabs(test.cosVector(test2)) < eps, failMesg, __LINE__);

			test[0] = 1; test[1] = 0; test[2] = 0;
			test2[0] = 1; test2[1] = 1; test2[2] = 0;

			failMesg = "Is the computed cosine value correct?";
			testFramework.assert(fabs(test.cosVector(test2) - sqrt(2.0)/2.0) < eps, failMesg, __LINE__);

			return testFramework.countFails();
		}
		/*	Verify the slant range calculation */
		int slantRangeTest()
		{
			TestUtil testFramework( "Triple", "slantRange", __FILE__, __LINE__ );
			std::string failMesg;

			Triple test(4,6,3),test2(1,2,3);

			failMesg = "Was the slant range calculation computed correctly?";
			testFramework.assert(fabs(test.slantRange(test2)- 5) < eps, failMesg, __LINE__);

			test[0] = 11; test[1] = -12; test[2] = 10;
			test2[0] = 1; test2[1] = 2; test2[2] = 3;
			
			failMesg = "Was the slant range calculation computed correctly?";
			testFramework.assert(fabs(test.slantRange(test2) - sqrt(345.0)) < eps, failMesg, __LINE__);

			return testFramework.countFails();	
		}
		/*	Verify the elevation angle calculation */
		int elvAngleTest()
		{
			TestUtil testFramework( "Triple", "elvAngle", __FILE__, __LINE__ );
			std::string failMesg;

			Triple test(1,0,0),test2(0,-1,0);
			double result;

			failMesg = "Was the elevation angle calculation computed correctly?";
			testFramework.assert(fabs(test.elvAngle(test2) + 45) < eps, failMesg, __LINE__);

			test[0] = 11; test[1] = -12; test[2] = 10;
			test2[0] = 1; test2[1] = 2; test2[2] = 3;
			result = acos(-348.0/(sqrt(365.0)*sqrt(345.0)))*180.0/(4.0*atan(1.0));
			result = 90 - result;

			failMesg = "Was the elevation angle calculation computed correctly?";
			testFramework.assert(fabs(test.elvAngle(test2) - result) < eps, failMesg, __LINE__);

			test[0] = 1; test[1] = 1; test[2] = 1;
			test2[0] = 1; test2[1] = 0; test2[2] = 0;
			
			failMesg = "Was the elevation angle calculation computed correctly?";
			testFramework.assert(fabs(test.elvAngle(test2) - (90 - acos(-2.0/sqrt(6.0))*180.0/(4.0*atan(1.0)))) < eps, failMesg, __LINE__);
			
			return testFramework.countFails();	
		}
		/* 	Verify the azimuthal angle calculation
			Uses relative error to check the number of correct digits */
		int azAngleTest()
		{
			TestUtil testFramework( "Triple", "azAngle", __FILE__, __LINE__ );
			std::string failMesg;

			Triple test(1,1,1),test2(-1,1,1);

			failMesg = "Was the azimutal angle calculation computed correctly?";	
			testFramework.assert(fabs(test.azAngle(test2) - 60) < eps, failMesg, __LINE__);

			test[0] = 11; test[1] = -12; test[2] = 10;
			test2[0] = 1; test2[1] = 2; test2[2] = 3;

			failMesg = "Was the azimutal angle calculation computed correctly?";	
			testFramework.assert(fabs(test.azAngle(test2) - 35.0779447169289) < eps, failMesg, __LINE__);

			test[0] = 1; test[1] = 0; test[2] = 0;
			test2[0] = 0; test2[1] = 1; test2[2] = 0;

			failMesg = "Was the azimutal angle calculation computed correctly?";	
			testFramework.assert(fabs(test.azAngle(test2) - 90)/90 < eps, failMesg, __LINE__);

			test[0] = 1; test[1] = -1; test[2] = 1;
			test2[0] = 1; test2[1] = 1; test2[2] = 1;

			failMesg = "Was the azimutal angle calculation computed correctly?";	
			testFramework.assert(fabs(test.azAngle(test2) - 60)/60 < eps, failMesg, __LINE__);


/* Special case: Using the origin as the point in which to find the azimuthal angle should ALWAYS cause test for p1+p2 != 0 to fail (see Triple.cpp).
   This next test is to ensure that the error is indeed thrown when it should. */ 

			test2[0] = 0; test2[1] = 0; test2[2] = 0;
			failMesg = "[testing] Triple.azAngle() at origin, [expected] exception gpstk::Exception, [actual] threw no exception";	
			try {test.azAngle(test2); testFramework.assert(false, failMesg, __LINE__);}
			catch (Exception& e) {testFramework.assert(true, failMesg, __LINE__);}
			catch (...)
			{
				failMesg = "[testing] Triple.azAngle() at origin, [expected] exception gpstk::Exception, [actual] threw different exception";
				testFramework.assert(false, failMesg, __LINE__);
			}

/* Special case: Using the south, (0,0,-1), direction as the position from which to find the azimuthal angle should also cause the initial check to
   get the angles to fail. */ 

			test[0] = 0; test[1] = 0; test[2] = -1;
			test2[0] = 1; test2[1] = 1; test2[2] = 1;
			failMesg = "[testing] Triple.azAngle() at origin and due south, [expected] exception gpstk::Exception, [actual] threw no exception";
			try {test.azAngle(test2); testFramework.assert(false, failMesg, __LINE__);}
			catch (Exception& e) {testFramework.assert(true, failMesg, __LINE__);}
			catch (...) 
			{
				failMesg = "[testing] Triple.azAngle() at origin and due south, [expected] exception gpstk::Exception, [actual] threw no exception";
				testFramework.assert(false, failMesg, __LINE__);
			}

			return testFramework.countFails();
		}

 };


int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	TripleTest testClass;

	check = testClass.setTest();
	errorCounter += check;

	check = testClass.dotTest();
	errorCounter += check;

	check = testClass.crossTest();
	errorCounter += check;

	check = testClass.magTest();
	errorCounter += check;

	check = testClass.unitVectorTest();
	errorCounter += check;

	check = testClass.cosVectorTest();
	errorCounter += check;

	check = testClass.slantRangeTest();
	errorCounter += check;

	check = testClass.elvAngleTest();
	errorCounter += check;

	check = testClass.azAngleTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
