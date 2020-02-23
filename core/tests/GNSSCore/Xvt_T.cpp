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

#include "Xvt.cpp"
#include "Triple.hpp"
#include "GPSEllipsoid.hpp"

#include "TestUtil.hpp"
#include <iostream>
#include <string>
#include <sstream>

using namespace gpstk;

class Xvt_T
{
    public: 
	Xvt_T() {eps = 1E-12;}// Default Constructor, set the precision value
	~Xvt_T() {} // Default Desructor

	/*Tests the get methods and constructor of Xvt */
	int getTest (void)
	{
		TestUtil testFramework("Xvt","Get",__FILE__,__func__ );
		testFramework.init();

		//Add in blank initialization check here

		Triple pos(1,2,3);
		Triple vel(4,5,6);
		double clockBias = 7, clockDrift = 8, relativityCorrection = 9;
		Xvt Compare;

		Compare.x = pos;
		Compare.v = vel;
		Compare.clkbias = clockBias;
		Compare.clkdrift = clockDrift;
		Compare.relcorr = relativityCorrection;

//--------------XvT_getTest_1 - Is the position value accurate?
		testFramework.assert(pos == Compare.getPos());
		testFramework.next();

//--------------XvT_getTest_2 - Is the velocity value accurate?
		testFramework.assert(vel == Compare.getVel());
		testFramework.next();

//--------------XvT_getTest_3 - Is the clock bias value accurate?
		testFramework.assert(clockBias == Compare.getClockBias());
		testFramework.next();

//--------------XvT_getTest_4 - Is the clock drift value accurate?
		testFramework.assert(clockDrift == Compare.getClockDrift());
		testFramework.next();

//--------------XvT_getTest_5 - Is the relativity correction value accurate?
		testFramework.assert(relativityCorrection == Compare.getRelativityCorr());

		return testFramework.countFails();
	}
	/* Ensures the computeRelativityCorrection method is accurate */
	int computeRelativityCorrectionTest (void)
	{
		TestUtil testFramework("Xvt","computeRelativityCorrection",__FILE__,__func__ );
		testFramework.init();

		Triple pos( 1234567000, 887654, 321 );
		Triple vel(4321, 1234, 1342); 
		Xvt Compare;
		double relcorr = -0.00011873444357376972;
		Compare.x = pos;
		Compare.v = vel;

//--------------XvT_computeRelativityCorrectionTest_1 - Was the relativity correction computation correct?
		testFramework.assert(abs(Compare.computeRelativityCorrection() - relcorr) < eps);
		testFramework.next();

		pos[0] = -1234567000;
		pos[1] = 887654;
		pos[2] = -100000;
		vel[0] = 3000;
		vel[1] = -500;
		vel[2] = -20;
		relcorr =8.242829448184317e-05;
		Compare.x = pos;
		Compare.v = vel;

//--------------XvT_computeRelativityCorrectionTest_2 - Was the relativity correction computation correct?
		testFramework.assert(abs(Compare.computeRelativityCorrection() - relcorr) < eps);
		testFramework.next();

		pos[0] = 0;
		pos[1] = 0;
		pos[2] = 0;
		vel[0] = 0;
		vel[1] = 0;
		vel[2] = 0;
		relcorr =0;
		Compare.x = pos;
		Compare.v = vel;

//--------------XvT_computeRelativityCorrectionTest_3 - Was the relativity correction computation correct?
		testFramework.assert(abs(Compare.computeRelativityCorrection()) < eps);

		return testFramework.countFails();
	}
	/* Ensures the preciseRho method is accurate */
	int preciseRhoTest (void)
	{
		TestUtil testFramework("Xvt","preciseRho Method Unverified",__FILE__,__func__ );
		testFramework.init();

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Unable to verify if operations done in Xvt.hpp are correct
//	Creating placeholder for testing, and setting it to fail as a signifier
		testFramework.assert(false);

		return testFramework.countFails();
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	}
	/* Tests to see if the redirection operator << is functioning properly*/
	int operatorTest (void)
	{
		TestUtil testFramework("Xvt","<< Operator",__FILE__,__func__ );
		testFramework.init();

		Triple pos(1,2,3);
		Triple vel(4,5,6);
		double clockBias = 7, clockDrift = 8, relativityCorrection = 9;
		Xvt output;		
		output.x = pos;
		output.v = vel;
		output.clkbias = clockBias;
		output.clkdrift = clockDrift;
		output.relcorr = relativityCorrection;

		std::stringstream streamOutput;
		std::stringstream streamCompare;
		std::string stringOuput;
		std::string stringCompare; 

		try
		{ 
			// Creates a string with the contents of output via the redirection operator
			streamOutput << output;

//--------------XvT_operatorTest_1- Was the operation using the << operator successful?			
			testFramework.passTest();

			stringOuput = streamOutput.str();

			// Creates a string of what the output stream should be
			streamCompare << "x:" << output.x
			    	<< ", v:" << output.v
     				<< ", clk bias:" << output.clkbias
    				<< ", clk drift:" << output.clkdrift
    				<< ", relcorr:" << output.relcorr;
    		stringCompare = streamCompare.str();

//--------------XvT_operatorTest_2- Were the contents of the Xvt output stream correct?
			testFramework.assert(stringCompare == stringOuput);
			testFramework.next();
		}
		catch (...) {std::cout<<"hi"<<std::endl;
		testFramework.failTest(); testFramework.failTest();}

		return testFramework.countFails();
	}

	private:
		double eps;
};


int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	Xvt_T testClass;

	check = testClass.getTest();
	errorCounter += check;

	check = testClass.preciseRhoTest();
	errorCounter += check;

	check = testClass.computeRelativityCorrectionTest();
	errorCounter += check;

	check = testClass.operatorTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}

