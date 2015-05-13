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

#include "Position.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

class Position_T
{
	public:
	Position_T(){eps = 1e-3;}// Default Constructor, set the precision value
	~Position_T() {} // Default Desructor
	double eps;

	/* Test will check the transforms of Position Objects.
	   There are 4  position types. This test will take a
	   position starting in each type and transform it to
	   each of the remaining types. This is a one-way transform,
	   and comparisons will be performed using the range() function.

	   Suppressing print lines from the test. */
	int transformTest (void)
	{
		TestUtil testFramework( "Position", "Cartesian transformTo", __FILE__, __LINE__ );
		std::string failMesg;
		try
   		{
	      		Position c,s,d,g; //An object for each of the Position types.
			//cout << "Position() " << c << endl; // Show that it is empty.
	
		      // test transformTo
			//cout << "Test set...() and transformTo\n";

				// Start in ECEF (Cartesian)
		        	c.setECEF(-1575232.0141,-4707872.2332, 3993198.4383);
		        	//cout << "Position.set        to Cartesian  " << c << endl;
		        	Position t(c); // Comparison Object
		        	t.transformTo(Position::Geodetic);
		        	//cout << "Transform Cartesian to Geodetic   " << t;
		        	//cout << "  Error : " << range(t,c) << " m" << endl;
				failMesg = "Were the ECEF coordinates converted to Geodetic?";
				testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__);

		        	t = c; // Reset comparison object
		        	t.transformTo(Position::Geocentric);
		       		//cout << "Transform Cartesian to Geocentric " << t;
	         		//cout << "  Error : " << range(t,c) << " m" << endl;
 				failMesg = "Were the ECEF coordinates converted to Geocentric?";
				testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__);

	         		t = c; // Reset comparison object
	         		t.transformTo(Position::Spherical);
	         		//cout << "Transform Cartesian to Spherical  " << t;
	         		//cout << "  Error : " << range(t,c) << " m" << endl;
				failMesg = "Were the ECEF coordinates converted to Spherical?";
				testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__); 

				//Start in Geodetic
				testFramework.changeSourceMethod("Geodetic transformTo");
	         		d.setGeodetic(39.000004186778,251.499999999370,1400.009066903964);
	         		//cout << "Position.set         to Geodetic  " << d << endl;
	         		t = d;
				t.transformTo(Position::Cartesian);
				//cout << "Transform Geodetic to Cartesian   " << t;
				//cout << "  Error : " << range(t,d) << " m" << endl;
				failMesg = "Were the Geodetic coordinates converted to ECEF?";
				testFramework.assert(fabs(range(t,d)) < eps, failMesg, __LINE__);

					t = d;  // Reset comparison object
					t.transformTo(Position::Geocentric);
					//cout << "Transform Geodetic to Geocentric  " << t;
					//cout << "  Error : " << range(t,d) << " m" << endl;
				failMesg = "Were the Geodetic coordinates converted to Geocentric?";
				testFramework.assert(fabs(range(t,d)) < eps, failMesg, __LINE__); 

					t = d;  // Reset comparison object
					t.transformTo(Position::Spherical);
	         		//cout << "Transform Geodetic to Spherical   " << t;
	         		//cout << "  Error : " << range(t,d) << " m" << endl;
				failMesg = "Were the Geodetic coordinates converted to Spherical?";
				testFramework.assert(fabs(range(t,d)) < eps, failMesg, __LINE__); 

				//Start in Geocentric
				testFramework.changeSourceMethod("Geocentric transformTo");
	         		g.setGeocentric(38.811958506159,251.499999999370,6371110.627671023800);
	         		//cout << "Position.set        to Geocentric " << g << endl;
	         		t = g;  // Reset comparison object
	         		t.transformTo(Position::Cartesian);
	         		//cout << "Transform Geocentric to Cartesian " << t;
	         		//cout << "  Error : " << range(t,g) << " m" << endl;
				failMesg = "Were the Geocentric coordinates converted to ECEF?";
				testFramework.assert(fabs(range(t,g)) < eps, failMesg, __LINE__); 

	         		t = g;  // Reset comparison object
	         		t.transformTo(Position::Geodetic);
	         		//cout << "Transform Geocentric to Geodetic  " << t;
	         		//cout << "  Error : " << range(t,g) << " m" << endl;
				failMesg = "Were the Geocentric coordinates converted to Geodetic?";
				testFramework.assert(fabs(range(t,g)) < eps, failMesg, __LINE__);

	         		t = g;  // Reset comparison object
	         		t.transformTo(Position::Spherical);
	         		//cout << "Transform Geocentric to Spherical " << t;
	         		//cout << "  Error : " << range(t,g) << " m" << endl;
				failMesg = "Were the Geocentric coordinates converted to Spherical?";
				testFramework.assert(fabs(range(t,g)) < eps, failMesg, __LINE__); 

				//Start in Spherical
				testFramework.changeSourceMethod("Spherical transformTo");
	         		s.setSpherical(51.188041493841,251.499999999370,6371110.627671023800);
	         		//cout << "Position.set      to Spherical    " << s << endl;
	         		t = s;
	         		t.transformTo(Position::Cartesian);
	         		//cout << "Transform Spherical to Cartesian  " << t;
	         		//cout << "  Error : " << range(t,s) << " m" << endl;
				failMesg =  "Were the  Spherical coordinates converted to ECEF?";
				testFramework.assert(fabs(range(t,s)) < eps, failMesg, __LINE__);

	         		t = s;
	         		t.transformTo(Position::Geocentric);
	         		//cout << "Transform Spherical to Geocentric " << t;
	         		//cout << "  Error : " << range(t,s) << " m" << endl;
				failMesg = "Were the Spherical coordinates converted to Geocentric?";
				testFramework.assert(fabs(range(t,s)) < eps, failMesg, __LINE__);

	         		t = s;
	         		t.transformTo(Position::Geodetic);
	         		//cout << "Transform Spherical to Geodetic   " << t;
	         		//cout << "  Error : " << range(t,s) << " m" << endl;
				failMesg = "Were the Spherical coordinates converted to Geodetic?";
				testFramework.assert(fabs(range(t,s)) < eps, failMesg, __LINE__); 

			//cout << endl << endl << endl;
			return testFramework.countFails();
		}
		catch(...)
		{
			std::cout << "Exception encountered at: " << testFramework.countTests() << std::endl;
			std::cout << "Test method failed" << std::endl;
		}
		return 12 - testFramework.countTests() + testFramework.countFails(); // Sets all unrun tests as failed and adds previous errors
	}

	/* Test will check the formatted printing of Position objects. */
	int printfTest()
	{
		TestUtil testFramework( "Position", "printf", __FILE__, __LINE__ );
		std::string failMesg;

		try
		{
      		Position c;
	        c.setECEF(-1575232.0141,-4707872.2332, 3993198.4383);

			failMesg = "Did the x value print out as expected?";
			testFramework.assert(c.printf("%13.4x     X() (meters)") == (std::string)"-1575232.0141     X() (meters)", failMesg, __LINE__);
			failMesg = "Did the y value print out as expected?";			
			testFramework.assert(c.printf("%13.4y     Y() (meters)") == (std::string)"-4707872.2332     Y() (meters)", failMesg, __LINE__);
			failMesg = "Did the z value print out as expected?";			
			testFramework.assert(c.printf("%13.4z     Z() (meters)") == (std::string)" 3993198.4383     Z() (meters)", failMesg, __LINE__);
			failMesg = "Did the X value print out as expected?";
			testFramework.assert(c.printf("%13.4X     X()/1000 (kilometers)") == (std::string)"   -1575.2320     X()/1000 (kilometers)", failMesg, __LINE__);
			failMesg = "Did the Y value print out as expected?";			
			testFramework.assert(c.printf("%13.4Y     Y()/1000 (kilometers)") == (std::string)"   -4707.8722     Y()/1000 (kilometers)", failMesg, __LINE__);
			failMesg = "Did the Z value print out as expected?";
			testFramework.assert(c.printf("%13.4Z     Z()/1000 (kilometers)") == (std::string)"    3993.1984     Z()/1000 (kilometers)", failMesg, __LINE__);
			failMesg = "Did the A value print out as expected?";
			testFramework.assert(c.printf("%15.6A   geodeticLatitude() (degrees North)") == (std::string)"      39.000004   geodeticLatitude() (degrees North)", failMesg, __LINE__);
			failMesg = "Did the a value print out as expected?";
			testFramework.assert(c.printf("%15.6a   geocentricLatitude() (degrees North)") == (std::string)"      38.811959   geocentricLatitude() (degrees North)", failMesg, __LINE__);
			failMesg = "Did the L value print out as expected?";
			testFramework.assert(c.printf("%15.6L   longitude() (degrees East)") == (std::string)"     251.500000   longitude() (degrees East)", failMesg, __LINE__);
			failMesg = "Did the l value print out as expected?";
			testFramework.assert(c.printf("%15.6l   longitude() (degrees East)") == (std::string)"     251.500000   longitude() (degrees East)", failMesg, __LINE__);
			failMesg = "Did the w value print out as expected?";				
			testFramework.assert(c.printf("%15.6w   longitude() (degrees West)") == (std::string)"     108.500000   longitude() (degrees West)", failMesg, __LINE__);
			failMesg = "Did the W value print out as expected?";
			testFramework.assert(c.printf("%15.6W   longitude() (degrees West)") == (std::string)"     108.500000   longitude() (degrees West)", failMesg, __LINE__);
			failMesg = "Did the t value print out as expected?";
			testFramework.assert(c.printf("%15.6t   theta() (degrees)") == (std::string)"      51.188041   theta() (degrees)", failMesg, __LINE__);
			failMesg = "Did the T value print out as expected?";
			testFramework.assert(c.printf("%15.6T   theta() (radians)") == (std::string)"       0.893400   theta() (radians)", failMesg, __LINE__);
			failMesg = "Did the p value print out as expected?";
			testFramework.assert(c.printf("%15.6p   phi() (degrees)") == (std::string)"     251.500000   phi() (degrees)", failMesg, __LINE__);
			failMesg = "Did the P value print out as expected?";	
			testFramework.assert(c.printf("%15.6P   phi() (radians)") == (std::string)"       4.389503   phi() (radians)", failMesg, __LINE__);
			failMesg = "Did the r value print out as expected?";	
			testFramework.assert(c.printf("%13.4r     radius() meters") == (std::string)" 6371110.6277     radius() meters", failMesg, __LINE__);
			failMesg = "Did the R value print out as expected?";	
			testFramework.assert(c.printf("%13.4R     radius()/1000 kilometers") == (std::string)"    6371.1106     radius()/1000 kilometers", failMesg, __LINE__);
			failMesg = "Did the h value print out as expected?";	
			testFramework.assert(c.printf("%13.4h     height() meters") == (std::string)"    1400.0091     height() meters", failMesg, __LINE__);
			failMesg = "Did the H value print out as expected?";	
			testFramework.assert(c.printf("%13.4H     height()/1000 kilometers") == (std::string)"       1.4000     height()/1000 kilometers", failMesg, __LINE__);
			return testFramework.countFails();
		}
		catch(...)
		{
			std::cout << "Exception encountered at: " << testFramework.countTests() << std::endl;
			std::cout << "Test method failed" << std::endl;
		}
		return 20 - testFramework.countTests() + testFramework.countFails(); // Sets all unrun tests as failed and adds previous errors
	}

	/*	Test for scanning strings
		Additional print lines are commented out. */
	int scanTest()
	{
		TestUtil testFramework( "Position", "scan", __FILE__, __LINE__ );
		std::string failMesg;

		try
		{
	      		Position c; // Initial position
		        c.setECEF(-1575232.0141,-4707872.2332, 3993198.4383);
			string fmt[5]={ //Various string formats
				"", //This one is left empty but is skipped in the loop
				"%A degN %L degE %h m",
				"%a degN %L degE %r m",
				"%x m %y m %z m",
				"%t deg %p deg %r M"};
			for(int i=1; i<5; i++) 
			{
				string str;
				Position t(c),tt; // A dummy Position initialized at c and another Position for comparison
				t.transformTo(static_cast<Position::CoordinateSystem>(i));
				{
					ostringstream o;
					o << t; //Output the Position object to stream
					str = o.str(); //Store that output as a string
				}
				tt.setToString(str,fmt[i]); //Set the comparison object using the output string

				failMesg = "Was the string set to expectation?";
				testFramework.assert(range(tt,t) < eps, failMesg, __LINE__); //Perform the comparison

				// Suppressed Output
				//cout << "System: " << t.getSystemName() << endl;
				//cout << "Original    " << t << endl;
				//cout << "string      " << str << endl;
				//cout << "setToString " << tt << " Error " << range(tt,t) << " m" << endl;
         			//cout << " equality: " << (t==tt ? "==" : "not ==");
         			//cout << " but " << (t!=tt ? "!=" : "not !=") << endl;
         			//cout << endl;
      			}
			return testFramework.countFails();
		}
		catch(...)
		{
			std::cout << "Exception encountered at: " << testFramework.countTests() << std::endl;
			std::cout << "Test method failed" << std::endl;
		}
		return 1 - testFramework.countTests() + testFramework.countFails(); // Sets all unrun tests as failed and adds previous errors
	}

	/*	Elevation and Azimuth tests
		Comparing these calculations from the ones in Triple
		(which are tested in the Triple tests) */
	int elevationAzimuthTest()
	{
		TestUtil testFramework( "Position", "elevationAzimuth", __FILE__, __LINE__ );
		std::string failMesg;		
		try
		{
	      		Position c,s;
      			c.setECEF(-1575232.0141,-4707872.2332, 3993198.4383);
      			s.setECEF(3*6371110.62767,0,0);
      			//cout << setw(6) << setprecision(2) << c.elevation(s)
         		//	<< " " << setw(6) << setprecision(2) << c.azimuth(s) << endl;

      			//cout << setw(6) << setprecision(2) << c.elvAngle(s)
         		//	<< " " << setw(6) << setprecision(2) << c.azAngle(s) << endl;
			failMesg = "Was the elevation computed correctly?";
			testFramework.assert(fabs(c.elevation(s) - c.elvAngle(s)) < eps, failMesg, __LINE__);
			failMesg = "Was the azmuith computed correctly?";
			testFramework.assert(fabs(c.azimuth(s) - c.azAngle(s)) < eps, failMesg, __LINE__);
			return testFramework.countFails();
		}
		catch(...)
		{
			std::cout << "Exception encountered at: " << testFramework.countTests() << std::endl;
			std::cout << "Test method failed" << std::endl;
		}
		return 2 - testFramework.countTests() + testFramework.countFails(); // Sets all unrun tests as failed and adds previous errors
	}

	/*	Transform tests at a pole. The pole is a unique location
		which may cause the transforms to break. */
	int poleTransformTest()
	{
		TestUtil testFramework( "Position", "poleTransform", __FILE__, __LINE__ );
		std::string failMesg;	
		try
		{
      		Position c,t;
			//cout << "Try to break it at the pole\n";
			c.setECEF(0,0,6371110.6277);
			//c.setECEF(0,0,0.0001);         // this will break it
			t = c;

//======================================================================================
//		Code below tests every possible conversion from one coordinate system to the
//		next at the pole.
//		i.e.	ECEF -> Geodetic
//				ECEF -> Geocentric
//				ECEF -> Spherical
//				Spherical -> Geodetic
//				Spherical -> Geocentric
//				Spherical -> ECEF
//				etc...
//======================================================================================			

			//cout << "The pole " << c << endl;
			t.transformTo(Position::Geodetic);
			//cout << "The pole in geodetic   " << t << endl;
			failMesg = "Were the ECEF coordinates converted to Geodetic at the pole?";
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__);
			t.transformTo(Position::Geocentric);
			//cout << "The pole in geocentric " << t << endl;
			failMesg = "Were the Geoedetic coordinates converted to Geocentric at the pole?";		
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__); 
			t.transformTo(Position::Spherical);
 			//cout << "The pole in spherical  " << t << endl;
			failMesg = "Were the Geocentric coordinates converted to Spherical at the pole?";
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__);
			t.transformTo(Position::Cartesian);
			//cout << "The pole in cartesian  " << t << endl;
			failMesg = "Were the Spherical coordinates converted to ECEF at the pole?";
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__); 
			t.transformTo(Position::Geodetic);
			//cout << "The pole in geodetic   " << t << endl;
			failMesg = "Were the Geocentric coordinates converted to Geodetic at the pole?";			
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__); 
			t.transformTo(Position::Cartesian);
			//cout << "The pole in cartesian  " << t << endl;
			failMesg = "Were the Geodetic coordinates converted to ECEF at the pole?";
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__); 
			t.transformTo(Position::Spherical);
			//cout << "The pole in spherical  " << t << endl;
			failMesg = "Were the Geocentric coordinates converted to Spherical at the pole?";			
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__); 
			t.transformTo(Position::Geodetic);
			//cout << "The pole in geodetic   " << t << endl;
			failMesg = "Were the Spherical coordinates converted to Geodetic at the pole?";			
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__); 
			t.transformTo(Position::Spherical);
			//cout << "The pole in spherical  " << t << endl;
			failMesg = "Were the Geodetic coordinates converted to Spherical at the pole?";
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__);
			t.transformTo(Position::Geocentric);
			//cout << "The pole in geocentric " << t << endl;
			failMesg = "Were the Spherical coordinates converted to Geocentric at the pole?";		
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__); 
			t.transformTo(Position::Cartesian);
			//cout << "The pole in cartesian  " << t << endl;
			failMesg = "Were the Geocentric coordinates converted to ECEF at the pole?";			
			testFramework.assert(fabs(range(t,c)) < eps, failMesg, __LINE__);
			//cout << "Tests complete." << endl;

			return testFramework.countFails();	
		}
		catch(...)
		{
			std::cout << "Exception encountered at: " << testFramework.countTests() << std::endl;
			std::cout << "Test method failed" << std::endl;
		}
		return 11 - testFramework.countTests() + testFramework.countFails(); // Sets all unrun tests as failed and adds previous errors
	}

	/*	Many of the tests above use the range() function to
		measure the distances between two positions. It in turn 
		needs to be tested to ensure that it works. */
	int rangeTest()
	{
		TestUtil testFramework( "Position", "range()", __FILE__, __LINE__ );
		std::string failMesg;
		try
		{
      		Position c,t;
			c.setECEF(0,0,6371110.6277);
			t.setECEF(20,0,6371110.6277);
			failMesg = "Was the range computation correct?";
			testFramework.assert(fabs(range(c,t)-20) < eps, failMesg, __LINE__);
			t.setECEF(0,-20,6371110.6277);
			failMesg = "Was the range computation correct?";
			testFramework.assert(fabs(range(c,t)-20) < eps, failMesg, __LINE__);
			t.setECEF(0,0,6371210.6277);
			failMesg = "Was the range computation correct?";
			testFramework.assert(fabs(range(c,t)-100) < eps, failMesg, __LINE__);
			t.setECEF(300,400,6371610.6277);
			failMesg = "Was the range computation correct?";
			testFramework.assert(fabs(range(c,t)-sqrt(500000.0)) < eps, failMesg, __LINE__);
			return testFramework.countFails();

		}
		catch(...)
		{
			std::cout << "Exception encountered at: " << testFramework.countTests() << std::endl;
			std::cout << "Test method failed" << std::endl;
		}
		return 4 - testFramework.countTests() + testFramework.countFails(); // Sets all unrun tests as failed and adds previous errors
	}
};

int main() //Main function to initialize and run all tests above
{
	int check = 0, errorCounter = 0;
	Position_T testClass;

	check = testClass.rangeTest();
	errorCounter += check;

	check = testClass.transformTest();
	errorCounter += check;

	check = testClass.printfTest();
	errorCounter += check;

	check = testClass.scanTest();
	errorCounter += check;

	check = testClass.elevationAzimuthTest();
 	errorCounter += check;
	
	check = testClass.poleTransformTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
