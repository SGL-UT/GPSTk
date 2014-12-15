#include "RACRotation.hpp"
#include "Xvt.hpp"
#include "Triple.hpp"
#include "icd_200_constants.hpp" //This will need to be replaced
#include "Position.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

/* Tests should be cleaned to use a == or != operator for the Triple class, once one is added.*/
class xRACRotation
{
	public:
		xRACRotation(){eps = 1e-12;}// Default Constructor, set the precision value
		~xRACRotation() {} // Default Desructor
		// First test case.  SV at GPS orbit altitude at 0 deg N, 0 deg E
		// Heading due N at 4 km/sec
		// Resulting RAC matrix: [  1  0  0]
		//                       [  0  0  1]
		//			 [  0 -1  0]
		int tripleFirstTest (void)
		{
			double GPSAlt = 26000000;
			Triple XYZ, rotatedXYZ;
   			Triple SVPos( GPSAlt, 0, 0);
   			Triple SVVel( 0, 0, 4000 );
   			RACRotation rot( SVPos, SVVel );

      			// OK, now give it test vectors to rotate			
   			XYZ = Triple( 1.0, 1.0, 1.0);
   			rotatedXYZ = rot.convertToRAC( XYZ );
			//Compare results
   			if (1 != rotatedXYZ[0]) return 1;
   			if (1 != rotatedXYZ[1]) return 2;
   			if (-1 != rotatedXYZ[2]) return 3;			

   			XYZ = Triple( -1.0, -1.0, -1.0);
   			rotatedXYZ = rot.convertToRAC( XYZ );

   			if (-1 != rotatedXYZ[0]) return 4;
   			if (-1 != rotatedXYZ[1]) return 5;
   			if (1 != rotatedXYZ[2]) return 6;

   			XYZ = Triple( 0.0, 0.0, -1.0);
   			rotatedXYZ = rot.convertToRAC( XYZ );

   			if (0 != rotatedXYZ[0]) return 7;
   			if (-1 != rotatedXYZ[1]) return 8;
   			if (0 != rotatedXYZ[2]) return 9;

   			XYZ = Triple( 0.0, -1.0, 0.0);
   			rotatedXYZ = rot.convertToRAC( XYZ );

   			if (0 != rotatedXYZ[0]) return 10;
   			if (0 != rotatedXYZ[1]) return 11;
   			if (1 != rotatedXYZ[2]) return 12;
			return 0;
		}

		// Second test case.  SV at GPS orbit altitude at 0 deg N, 90 deg E
		// Heading due S at 4 km/sec
		// Resulting RAC matrix: [  0  1  0]
		//                       [  0  0 -1]
		//			 [ -1  0  0]
		int tripleSecondTest (void)
		{
			double GPSAlt = 26000000;
			Triple XYZ, rotatedXYZ;
   			Triple SVPos2( 0, GPSAlt, 0);
   			Triple SVVel2( 0, 0, -4000 );

   			RACRotation rot2( SVPos2, SVVel2 );

      			// OK, now give it an "error vector" that 1 m in each component
   			XYZ = Triple( 1.0, 1.0, 1.0);
   			rotatedXYZ = rot2.convertToRAC( XYZ );

   			if (1 != rotatedXYZ[0]) return 1;
   			if (-1 != rotatedXYZ[1]) return 2;
   			if (-1 != rotatedXYZ[2]) return 3;

   			XYZ = Triple( -1.0, -1.0, -1.0);
   			rotatedXYZ = rot2.convertToRAC( XYZ );

   			if (-1 != rotatedXYZ[0]) return 4;
   			if (1 != rotatedXYZ[1]) return 5;
   			if (1 != rotatedXYZ[2]) return 6;

   			XYZ = Triple( 0.0, 0.0, -1.0);
   			rotatedXYZ = rot2.convertToRAC( XYZ );

   			if (0 != rotatedXYZ[0]) return 7;
   			if (1 != rotatedXYZ[1]) return 8;
   			if (0 != rotatedXYZ[2]) return 9;

   			XYZ = Triple( 0.0, -1.0, 0.0);
   			rotatedXYZ = rot2.convertToRAC( XYZ );

   			if (-1 != rotatedXYZ[0]) return 10;
   			if (0 != rotatedXYZ[1]) return 11;
   			if (0 != rotatedXYZ[2]) return 12;
			return 0;
		}

		// Third test case.  SV at GPS orbit altitude at 90 deg N, X deg E
		// Heading parallel to the Y axis at 4 km/sec
		// Resulting RAC matrix: [  0  0  1]
		//                       [  0  1  0]
		//			 [ -1  0  0]
		int  tripleThirdTest (void)
		{
			double GPSAlt = 26000000;
			Triple XYZ, rotatedXYZ;
		   	Triple SVPos3( 0, 0, GPSAlt);
		   	Triple SVVel3( 0, 4000, 0 );

		   	RACRotation rot3( SVPos3, SVVel3 );

		      	// OK, now give it an "error vector" that 1 m in each component
		   	XYZ = Triple( 1.0, 1.0, 1.0);
		   	rotatedXYZ = rot3.convertToRAC( XYZ );

   			if (1 != rotatedXYZ[0]) return 1;
   			if (1 != rotatedXYZ[1]) return 2;
   			if (-1 != rotatedXYZ[2]) return 3;

		   	XYZ = Triple( -1.0, -1.0, -1.0);
		   	rotatedXYZ = rot3.convertToRAC( XYZ );

   			if (-1 != rotatedXYZ[0]) return 4;
   			if (-1 != rotatedXYZ[1]) return 5;
   			if (1 != rotatedXYZ[2]) return 6;

		   	XYZ = Triple( 0.0, 0.0, -1.0);
		   	rotatedXYZ = rot3.convertToRAC( XYZ );

   			if (-1 != rotatedXYZ[0]) return 7;
   			if (0 != rotatedXYZ[1]) return 8;
   			if (0 != rotatedXYZ[2]) return 9;

		   	XYZ = Triple( 0.0, -1.0, 0.0);
		   	rotatedXYZ = rot3.convertToRAC( XYZ );

   			if (0 != rotatedXYZ[0]) return 10;
   			if (-1 != rotatedXYZ[1]) return 11;
   			if (0 != rotatedXYZ[2]) return 12;
			return 0;
		}

		// Fourth test case.  SV at GPS orbit altitude at 45 deg N, 45 deg E
		// Heading  4 km/sec to the NW.
		// Resulting RAC matrix: [  1/2            1/2              1/sqrt(2)          ]
		//                       [ -3/(2*sqrt(3))  1/(2*sqrt(3))    sqrt(2)/(2*sqrt(3))]
		//			 [  0             -sqrt(2)/sqrt(3)  1/sqrt(3)          ]
		int tripleFourthTest (void)
		{
			double GPSAlt = 26000000;
			Triple XYZ, rotatedXYZ;
		      	// Spherical: R = 26M m, phi = 45 deg N, theta = 45 deg E
		      	// x = r sin phi cos theta
		      	// y = r sin phi sin theta
		      	// z = r cos phi
		   	double thetaphi = 45.0 * ( PI / 180.0 );
		   	double x4 = GPSAlt * sin(thetaphi) * cos(thetaphi);
		   	double y4 = GPSAlt * sin(thetaphi) * sin(thetaphi);
		   	double z4 = GPSAlt * cos(thetaphi);
		   	Triple SVPos4( x4, y4, z4 );

		      	// Spherical: R = 4000 m/sec, phi = 45 deg E, theta = 135 deg E
		      	// x = r sin phi cos theta
		      	// y = r sin phi sin theta
		      	// z = r cos phi
		   	double theta = 135.0 * (PI/180.0);
		   	double phi = 45.0 * (PI/180.0);
		   	double xv4 = 4000 * sin(phi) * cos(theta);
		   	double yv4 = 4000 * sin(phi) * sin(theta);
		   	double zv4 = 4000 * cos(phi);
		   	Triple SVVel4( xv4, yv4, zv4 );

		   	RACRotation rot4( SVPos4, SVVel4 );

		      	// OK, now give it an "error vector" that 1 m in each component
		   	XYZ = Triple( 1.0, 1.0, 1.0);
		   	rotatedXYZ = rot4.convertToRAC( XYZ );

			double x = 1.0 + 1.0/sqrt(2.0);
			double y = (-2.0+sqrt(2.0))/(2.0*sqrt(3.0));
			double z = (1.0 - sqrt(2.0))/sqrt(3.0);
		   	if (abs(x - rotatedXYZ[0]) > eps) return 1;
		   	if (abs(y - rotatedXYZ[1]) > eps) return 2;
		   	if (abs(z - rotatedXYZ[2]) > eps) return 3;

		   	XYZ = Triple( -1.0, -1.0, -1.0);
		   	rotatedXYZ = rot4.convertToRAC( XYZ );

			x = -1.0 - 1.0/sqrt(2.0);
			y = -(-2.0+sqrt(2.0))/(2.0*sqrt(3.0));
			z = -(1.0 - sqrt(2.0))/sqrt(3.0);
		   	if (abs(x - rotatedXYZ[0]) > eps) return 4;
		   	if (abs(y - rotatedXYZ[1]) > eps) return 5;
		   	if (abs(z - rotatedXYZ[2]) > eps) return 6;

		   	XYZ = Triple( 0.0, 0.0, -1.0);
		   	rotatedXYZ = rot4.convertToRAC( XYZ );

			x = -1.0/sqrt(2.0);
			y = -sqrt(2.0)/(2.0*sqrt(3.0));
			z = -1.0/sqrt(3.0);
		   	if (abs(x - rotatedXYZ[0]) > eps) return 7;
		   	if (abs(y - rotatedXYZ[1]) > eps) return 8;
		   	if (abs(z - rotatedXYZ[2]) > eps) return 9;

		   	XYZ = Triple( 0.0, -1.0, 0.0);
		   	rotatedXYZ = rot4.convertToRAC( XYZ );

			x = -0.5;
			y = -1.0/(2.0*sqrt(3.0));
			z = sqrt(2.0)/sqrt(3.0);
		   	if (abs(x - rotatedXYZ[0]) > eps) return 10;
		   	if (abs(y - rotatedXYZ[1]) > eps) return 11;
		   	if (abs(z - rotatedXYZ[2]) > eps) return 12;
			return 0;
		}

		// Repeat first test case using the Xvt signatures of
		// the RACRotation class.
		// Resulting RAC matrix: [  1  0  0]
		//                       [  0  0  1]
		//			 [  0 -1  0]
		int xvtTest (void)
		{
			double GPSAlt = 26000000;
			Triple XYZ, rotatedXYZ;
		   	Xvt refPoint;
		   	refPoint.x = Position( GPSAlt, 0, 0 );
		   	refPoint.v = Triple( 0, 0, 4000 );
		   	refPoint.dtime = 0.0;
		   	refPoint.ddtime = 0.0;
		   	//refPoint.relcorr = 0.0;

		   	RACRotation rot5( refPoint );

		      	// OK, now set up unit vectors in radius and velocity to 
                        // see if the results align properly.
		   	Xvt testPoint;
		   	testPoint.x = Position( 1.0, 0.0, 0.0 );
		   	testPoint.v = Triple( 0.0, 0.0, 1.0 );
		   	testPoint.dtime = 0.0;
		   	testPoint.ddtime = 0.0;
		   	//testPoint.relcorr = 0.0;
		   	Xvt rotatedTestPoint = rot5.convertToRAC( testPoint );

		   	if (Position(1,0,0) != (gpstk::Position)rotatedTestPoint.x) return 1;
		   	//if (Triple(0,1,0) != rotatedTestPoint.v) return 2;
   			if (0 != rotatedTestPoint.v[0]) return 2;
   			if (1 != rotatedTestPoint.v[1]) return 2;
   			if (0 != rotatedTestPoint.v[2]) return 2;
		   	if (0.0 != rotatedTestPoint.dtime) return 3;
		   	if (0.0 != rotatedTestPoint.ddtime) return 4;
		   	//if (0.0 != rotatedTestPoint.relcorr) return 5;

		      	// Using more comlpex test vectors.
		   	testPoint.x = Position( 0.0, 0.5, -0.5);
		   	testPoint.v = Triple(  1.0, -1.0, 1.0);
		   	testPoint.dtime = 0.0;
		   	testPoint.ddtime = 0.0;
		   	//testPoint.relcorr = 0.0;
		   	rotatedTestPoint = rot5.convertToRAC( testPoint );

		   	if (Position(0, -0.5, -0.5) != (gpstk::Position)rotatedTestPoint.x) return 6;
		   	//if (Triple(1.0, 1.0, 1.0) != rotatedTestPoint.v) return 7;
   			if (1 != rotatedTestPoint.v[0]) return 7;
   			if (1 != rotatedTestPoint.v[1]) return 7;
   			if (1 != rotatedTestPoint.v[2]) return 7;
		   	if (0.0 != rotatedTestPoint.dtime) return 8;
		   	if (0.0 != rotatedTestPoint.ddtime) return 9;
		   	//if (0.0 != rotatedTestPoint.relcorr) return 10;

			// The conversions should still accept any Triples
		   	XYZ = Triple( 1.0, 1.0, -1.0);
		   	rotatedXYZ = rot5.convertToRAC( XYZ );

		   	//if (Triple(1,-1,-1) == rotatedXYZ)) return 11;
   			if (1 != rotatedXYZ[0]) return 11;
   			if (-1 != rotatedXYZ[1]) return 11;
   			if (-1 != rotatedXYZ[2]) return 11;
			return 0;
		}
	private:
		double eps;
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
	xRACRotation testClass;

	check = testClass.tripleFirstTest();
        std::cout << "tripleFirstTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.tripleSecondTest(); 
        std::cout << "tripleSecondTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.tripleThirdTest();
        std::cout << "tripleThirdTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.tripleFourthTest();
        std::cout << "tripleFourthTest Result is: "; 
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.xvtTest();
        std::cout << "xvtTest Result is: "; 
	checkResult(check, errorCounter);
	check = -1;

	
	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
