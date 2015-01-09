#include <iostream>
#include <iomanip>
#include "Position.hpp"

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
		try
   		{
      			int i;
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
				if (fabs(range(t,c)) > eps) return 1; 
		        	t = c; // Reset comparison object
		        	t.transformTo(Position::Geocentric);
		       		//cout << "Transform Cartesian to Geocentric " << t;
	         		//cout << "  Error : " << range(t,c) << " m" << endl;
				if (fabs(range(t,c)) > eps) return 2; 
	         		t = c;
	         		t.transformTo(Position::Spherical);
	         		//cout << "Transform Cartesian to Spherical  " << t;
	         		//cout << "  Error : " << range(t,c) << " m" << endl;
				if (fabs(range(t,c)) > eps) return 3; 

				//Start in Geodetic
	         		d.setGeodetic(39.000004186778,251.499999999370,1400.009066903964);
	         		//cout << "Position.set         to Geodetic  " << d << endl;
	         		t = d;
				t.transformTo(Position::Cartesian);
				//cout << "Transform Geodetic to Cartesian   " << t;
				//cout << "  Error : " << range(t,d) << " m" << endl;
				if (fabs(range(t,d)) > eps) return 4; 
				t = d;
				t.transformTo(Position::Geocentric);
				//cout << "Transform Geodetic to Geocentric  " << t;
				//cout << "  Error : " << range(t,d) << " m" << endl;
				if (fabs(range(t,d)) > eps) return 5; 
				t = d;
				t.transformTo(Position::Spherical);
	         		//cout << "Transform Geodetic to Spherical   " << t;
	         		//cout << "  Error : " << range(t,d) << " m" << endl;
				if (fabs(range(t,d)) > eps) return 6; 

				//Start in Geocentric
	         		g.setGeocentric(38.811958506159,251.499999999370,6371110.627671023800);
	         		//cout << "Position.set        to Geocentric " << g << endl;
	         		t = g;
	         		t.transformTo(Position::Cartesian);
	         		//cout << "Transform Geocentric to Cartesian " << t;
	         		//cout << "  Error : " << range(t,g) << " m" << endl;
				if (fabs(range(t,g)) > eps) return 7; 
	         		t = g;
	         		t.transformTo(Position::Geodetic);
	         		//cout << "Transform Geocentric to Geodetic  " << t;
	         		//cout << "  Error : " << range(t,g) << " m" << endl;
				if (fabs(range(t,g)) > eps) return 8; 
	         		t = g;
	         		t.transformTo(Position::Spherical);
	         		//cout << "Transform Geocentric to Spherical " << t;
	         		//cout << "  Error : " << range(t,g) << " m" << endl;
				if (fabs(range(t,g)) > eps) return 9; 

				//Start in Spherical
	         		s.setSpherical(51.188041493841,251.499999999370,6371110.627671023800);
	         		//cout << "Position.set      to Spherical    " << s << endl;
	         		t = s;
	         		t.transformTo(Position::Cartesian);
	         		//cout << "Transform Spherical to Cartesian  " << t;
	         		//cout << "  Error : " << range(t,s) << " m" << endl;
				if (fabs(range(t,s)) > eps) return 10; 
	         		t = s;
	         		t.transformTo(Position::Geocentric);
	         		//cout << "Transform Spherical to Geocentric " << t;
	         		//cout << "  Error : " << range(t,s) << " m" << endl;
				if (fabs(range(t,s)) > eps) return 11; 
	         		t = s;
	         		t.transformTo(Position::Geodetic);
	         		//cout << "Transform Spherical to Geodetic   " << t;
	         		//cout << "  Error : " << range(t,s) << " m" << endl;
				if (fabs(range(t,s)) > eps) return 12; 

			//cout << endl << endl << endl;
			return 0;
		}
		catch(...)
		{
			cout << "An exception was thrown!!!!!!!!" << endl;
		}
		return -1;
	}

	/* Test will check the formatted printing of Position objects. */
	int printfTest()
	{
		try
		{
	      		Position c;
		        c.setECEF(-1575232.0141,-4707872.2332, 3993198.4383);

			if (c.printf("%13.4x     X() (meters)") != (std::string)"-1575232.0141     X() (meters)") return 1;
			if (c.printf("%13.4y     Y() (meters)") != (std::string)"-4707872.2332     Y() (meters)") return 2;
			if (c.printf("%13.4z     Z() (meters)") != (std::string)" 3993198.4383     Z() (meters)") return 3;
			if (c.printf("%13.4X     X()/1000 (kilometers)") != (std::string)"   -1575.2320     X()/1000 (kilometers)") return 4;
			if (c.printf("%13.4Y     Y()/1000 (kilometers)") != (std::string)"   -4707.8722     Y()/1000 (kilometers)") return 5;
			if (c.printf("%13.4Z     Z()/1000 (kilometers)") != (std::string)"    3993.1984     Z()/1000 (kilometers)") return 6;
			if (c.printf("%15.6A   geodeticLatitude() (degrees North)") != (std::string)"      39.000004   geodeticLatitude() (degrees North)") return 7;
			if (c.printf("%15.6a   geocentricLatitude() (degrees North)") != (std::string)"      38.811959   geocentricLatitude() (degrees North)") return 8;
			if (c.printf("%15.6L   longitude() (degrees East)") != (std::string)"     251.500000   longitude() (degrees East)") return 9;
			if (c.printf("%15.6l   longitude() (degrees East)") != (std::string)"     251.500000   longitude() (degrees East)") return 10;
			if (c.printf("%15.6w   longitude() (degrees West)") != (std::string)"     108.500000   longitude() (degrees West)") return 11;
			if (c.printf("%15.6W   longitude() (degrees West)") != (std::string)"     108.500000   longitude() (degrees West)") return 12;
			if (c.printf("%15.6t   theta() (degrees)") != (std::string)"      51.188041   theta() (degrees)") return 13;
			if (c.printf("%15.6T   theta() (radians)") != (std::string)"       0.893400   theta() (radians)") return 14;
			if (c.printf("%15.6p   phi() (degrees)") != (std::string)"     251.500000   phi() (degrees)") return 15;
			if (c.printf("%15.6P   phi() (radians)") != (std::string)"       4.389503   phi() (radians)") return 16;
			if (c.printf("%13.4r     radius() meters") != (std::string)" 6371110.6277     radius() meters") return 17;
			if (c.printf("%13.4R     radius()/1000 kilometers") != (std::string)"    6371.1106     radius()/1000 kilometers") return 18;
			if (c.printf("%13.4h     height() meters") != (std::string)"    1400.0091     height() meters") return 19;
			if (c.printf("%13.4H     height()/1000 kilometers") != (std::string)"       1.4000     height()/1000 kilometers") return 20;
			return 0;
		}
		catch(...)
		{
			cout << "An exception was thrown!!!!!!!!" << endl;
		}
		return -1;
	}

	//Test for scanning strings
	//Additional print lines are commented out.
	int scanTest()
	{
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
				if (range(tt,t) > eps) return i; //Perform the comparison

				// Suppressed Output
				//cout << "System: " << t.getSystemName() << endl;
				//cout << "Original    " << t << endl;
				//cout << "string      " << str << endl;
				//cout << "setToString " << tt << " Error " << range(tt,t) << " m" << endl;
         			//cout << " equality: " << (t==tt ? "==" : "not ==");
         			//cout << " but " << (t!=tt ? "!=" : "not !=") << endl;
         			//cout << endl;
      			}
			return 0;
		}
		catch(...)
		{
			cout << "An exception was thrown!!!!!!!!" << endl;
		}
		return -1;
	}

	// Elevation and Azimuth tests
	// Comparing these calculations from the ones in Triple (which are tested in the Triple tests).
	int elevationAzimuthTest()
	{
		try
		{
	      		Position c,s;
      			c.setECEF(-1575232.0141,-4707872.2332, 3993198.4383);
      			s.setECEF(3*6371110.62767,0,0);
      			//cout << setw(6) << setprecision(2) << c.elevation(s)
         		//	<< " " << setw(6) << setprecision(2) << c.azimuth(s) << endl;

      			//cout << setw(6) << setprecision(2) << c.elvAngle(s)
         		//	<< " " << setw(6) << setprecision(2) << c.azAngle(s) << endl;
			if (fabs(c.elevation(s) - c.elvAngle(s)) > eps) return 1;
			if (fabs(c.azimuth(s) - c.azAngle(s)) > eps) return 2;
			return 0;
		}
		catch(...)
		{
			cout << "An exception was thrown!!!!!!!!" << endl;
		}
		return -1;
	}

	//Transform tests at a pole. The pole is a unique location which may cause the transforms to break.
	int poleTransformTest()
	{
		try
		{
	      		Position c,t;
			//cout << "Try to break it at the pole\n";
			c.setECEF(0,0,6371110.6277);
			//c.setECEF(0,0,0.0001);         // this will break it
			t = c;

			//cout << "The pole " << c << endl;
			t.transformTo(Position::Geodetic);
			//cout << "The pole in geodetic   " << t << endl;
			if (fabs(range(t,c)) > eps) return 1; 
			t.transformTo(Position::Geocentric);
			//cout << "The pole in geocentric " << t << endl;
			if (fabs(range(t,c)) > eps) return 2; 
			t.transformTo(Position::Spherical);
 			//cout << "The pole in spherical  " << t << endl;
			if (fabs(range(t,c)) > eps) return 3; 
			t.transformTo(Position::Cartesian);
			//cout << "The pole in cartesian  " << t << endl;
			if (fabs(range(t,c)) > eps) return 4; 
			t.transformTo(Position::Geocentric);
			//cout << "The pole in geocentric " << t << endl;
			if (fabs(range(t,c)) > eps) return 5; 
			t.transformTo(Position::Geodetic);
			//cout << "The pole in geodetic   " << t << endl;
			if (fabs(range(t,c)) > eps) return 6; 
			t.transformTo(Position::Cartesian);
			//cout << "The pole in cartesian  " << t << endl;
			if (fabs(range(t,c)) > eps) return 7; 
			t.transformTo(Position::Spherical);
			//cout << "The pole in spherical  " << t << endl;
			if (fabs(range(t,c)) > eps) return 8; 
			t.transformTo(Position::Geodetic);
			//cout << "The pole in geodetic   " << t << endl;
			if (fabs(range(t,c)) > eps) return 9; 
			t.transformTo(Position::Spherical);
			//cout << "The pole in spherical  " << t << endl;
			if (fabs(range(t,c)) > eps) return 10; 
			t.transformTo(Position::Geocentric);
			//cout << "The pole in geocentric " << t << endl;
			if (fabs(range(t,c)) > eps) return 11; 
			t.transformTo(Position::Cartesian);
			//cout << "The pole in cartesian  " << t << endl;
			if (fabs(range(t,c)) > eps) return 12; 
			//cout << "Tests complete." << endl;	
			return 0;	
		}
		catch(...)
		{
			cout << "An exception was thrown!!!!!!!!" << endl;
		}
		return -1;
	}

	// Many of the tests above use the range() function to measure the distances
	// between two positions. It in turn needs to be tested to ensure that it works.
	int rangeTest()
	{
		try
		{
	      		Position c,t;
			c.setECEF(0,0,6371110.6277);
			t.setECEF(20,0,6371110.6277);
			if(fabs(range(c,t)-20) > 1E-12) return 1;
			t.setECEF(0,-20,6371110.6277);
			if(fabs(range(c,t)-20) > 1E-12) return 2;
			t.setECEF(0,0,6371210.6277);
			if(fabs(range(c,t)-100) > 1E-12) return 3;
			t.setECEF(300,400,6371610.6277);
			if(fabs(range(c,t)-sqrt(500000.0)) > 1E-12) return 4;
			return 0;

		}
		catch(...)
		{
			cout << "An exception was thrown!!!!!!!!" << endl;
		}
		return -1;
	}
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
	Position_T testClass;

	check = testClass.rangeTest();
        std::cout << "rangeTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.transformTest();
        std::cout << "transformTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.printfTest();
        std::cout << "printfTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.scanTest();
        std::cout << "scanTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.elevationAzimuthTest();
        std::cout << "elevationAzimuthTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	
	check = testClass.poleTransformTest();
        std::cout << "poleTransformTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
