#include "RACRotation.hpp"
#include "Xvt.hpp"
#include "Triple.hpp"
//#include "icd_200_constants.hpp" //This will need to be replaced
#include "GNSSconstants.hpp"
#include "GPS_URA.hpp"
#include "Position.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

/* Tests should be cleaned to use a == or != operator for the Triple class, once one is added.*/
class RACRotation_T
{
	public:
		RACRotation_T(){eps = 1e-12;}// Default Constructor, set the precision value
		~RACRotation_T() {} // Default Desructor

	/*	First test case.  SV at GPS orbit altitude at 0 deg N, 0 deg E
		Heading due N at 4 km/sec
		Resulting RAC matrix: [  1  0  0]
		                      [  0  0  1]
				 			  [  0 -1  0] */
		int tripleFirstTest (void)
		{
			TestUtil testFramework( "RACRotation", "tripleFirst", __FILE__, __func__ );
			testFramework.init();

			double GPSAlt = 26000000;
			Triple XYZ, rotatedXYZ;
   			Triple SVPos( GPSAlt, 0, 0);
   			Triple SVVel( 0, 0, 4000 );
   			RACRotation rot( SVPos, SVVel );

      			// OK, now give it test vectors to rotate			
   			XYZ = Triple( 1.0, 1.0, 1.0);
   			rotatedXYZ = rot.convertToRAC( XYZ );
			//Compare results

//--------------RACRotation_tripleFirstTest_1 - Was the X value rotated properly?
   			testFramework.assert(1 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleFirstTest_2 - Was the Y value rotated properly?
   			testFramework.assert(1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleFirstTest_3 - Was the Z value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[2]);
   			testFramework.next();			

   			XYZ = Triple( -1.0, -1.0, -1.0);
   			rotatedXYZ = rot.convertToRAC( XYZ );

//--------------RACRotation_tripleFirstTest_4 - Was the X value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleFirstTest_5 - Was the Y value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleFirstTest_6 - Was the Z value rotated properly?
   			testFramework.assert(1 == rotatedXYZ[2]);
   			testFramework.next();		

   			XYZ = Triple( 0.0, 0.0, -1.0);
   			rotatedXYZ = rot.convertToRAC( XYZ );

//--------------RACRotation_tripleFirstTest_7 - Was the X value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleFirstTest_8 - Was the Y value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleFirstTest_9 - Was the Z value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[2]);
   			testFramework.next();	

   			XYZ = Triple( 0.0, -1.0, 0.0);
   			rotatedXYZ = rot.convertToRAC( XYZ );

//--------------RACRotation_tripleFirstTest_10 - Was the X value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleFirstTest_11 - Was the Y value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleFirstTest_12 - Was the Z value rotated properly?
   			testFramework.assert(1 == rotatedXYZ[2]);	

			return testFramework.countFails();   
		}

		/* Second test case.  SV at GPS orbit altitude at 0 deg N, 90 deg E
			Heading due S at 4 km/sec
			Resulting RAC matrix: [  0  1  0]
		    	                  [  0  0 -1]
					 			  [ -1  0  0] */
		int tripleSecondTest (void)
		{
			TestUtil testFramework( "RACRotation", "tripleSecond", __FILE__, __func__ );
			testFramework.init();

			double GPSAlt = 26000000;
			Triple XYZ, rotatedXYZ;
   			Triple SVPos2( 0, GPSAlt, 0);
   			Triple SVVel2( 0, 0, -4000 );

   			RACRotation rot2( SVPos2, SVVel2 );

      			// OK, now give it an "error vector" that 1 m in each component
   			XYZ = Triple( 1.0, 1.0, 1.0);
   			rotatedXYZ = rot2.convertToRAC( XYZ );

//--------------RACRotation_tripleSecondTest_1 - Was the X value rotated properly?
   			testFramework.assert(1 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleSecondTest_2 - Was the Y value rotated properly?   			
   			testFramework.assert(-1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleSecondTest_3 - Was the Z value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[2]);
   			testFramework.next();

   			XYZ = Triple( -1.0, -1.0, -1.0);
   			rotatedXYZ = rot2.convertToRAC( XYZ );

//--------------RACRotation_tripleSecondTest_4 - Was the X value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleSecondTest_5 - Was the Y value rotated properly?   			
   			testFramework.assert(1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleSecondTest_6 - Was the Z value rotated properly?
   			testFramework.assert(1 == rotatedXYZ[2]);
   			testFramework.next();

   			XYZ = Triple( 0.0, 0.0, -1.0);
   			rotatedXYZ = rot2.convertToRAC( XYZ );

//--------------RACRotation_tripleSecondTest_7 - Was the X value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleSecondTest_8 - Was the Y value rotated properly?   			
   			testFramework.assert(1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleSecondTest_9 - Was the Z value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[2]);
   			testFramework.next();

   			XYZ = Triple( 0.0, -1.0, 0.0);
   			rotatedXYZ = rot2.convertToRAC( XYZ );

//--------------RACRotation_tripleSecondTest_10 - Was the X value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleSecondTest_11 - Was the Y value rotated properly?   			
   			testFramework.assert(0 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleSecondTest_12 - Was the Z value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[2]);
   			testFramework.next();

			return testFramework.countFails();
		}

		/*	Third test case.  SV at GPS orbit altitude at 90 deg N, X deg E
			Heading parallel to the Y axis at 4 km/sec
			Resulting RAC matrix: [  0  0  1]
			                      [  0  1  0]
								  [ -1  0  0] */
		int  tripleThirdTest (void)
		{
			TestUtil testFramework( "RACRotation", "tripleThird", __FILE__, __func__ );
			testFramework.init();

			double GPSAlt = 26000000;
			Triple XYZ, rotatedXYZ;
		   	Triple SVPos3( 0, 0, GPSAlt);
		   	Triple SVVel3( 0, 4000, 0 );

		   	RACRotation rot3( SVPos3, SVVel3 );

		      	// OK, now give it an "error vector" that 1 m in each component
		   	XYZ = Triple( 1.0, 1.0, 1.0);
		   	rotatedXYZ = rot3.convertToRAC( XYZ );

//--------------RACRotation_tripleThirdTest_1 - Was the X value rotated properly?
   			testFramework.assert(1 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleThirdTest_2 - Was the Y value rotated properly?   			
   			testFramework.assert(1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleThirdTest_3 - Was the Z value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[2]);
   			testFramework.next();

		   	XYZ = Triple( -1.0, -1.0, -1.0);
		   	rotatedXYZ = rot3.convertToRAC( XYZ );

//--------------RACRotation_tripleThirdTest_4 - Was the X value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleThirdTest_5 - Was the Y value rotated properly?   			
   			testFramework.assert(-1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleThirdTest_6 - Was the Z value rotated properly?
   			testFramework.assert(1 == rotatedXYZ[2]);
   			testFramework.next();

		   	XYZ = Triple( 0.0, 0.0, -1.0);
		   	rotatedXYZ = rot3.convertToRAC( XYZ );

//--------------RACRotation_tripleThirdTest_7 - Was the X value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleThirdTest_8 - Was the Y value rotated properly?   			
   			testFramework.assert(0 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleThirdTest_9 - Was the Z value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[2]);
   			testFramework.next();

		   	XYZ = Triple( 0.0, -1.0, 0.0);
		   	rotatedXYZ = rot3.convertToRAC( XYZ );

//--------------RACRotation_tripleThirdTest_10 - Was the X value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_tripleThirdTest_11 - Was the Y value rotated properly?   			
   			testFramework.assert(-1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_tripleThirdTest_12 - Was the Z value rotated properly?
   			testFramework.assert(0 == rotatedXYZ[2]);
   			
			return testFramework.countFails();
		}

		/*	Fourth test case.  SV at GPS orbit altitude at 45 deg N, 45 deg E
			Heading  4 km/sec to the NW.
			Resulting RAC matrix: [  1/2            1/2              1/sqrt(2)          ]
			                      [ -3/(2*sqrt(3))  1/(2*sqrt(3))    sqrt(2)/(2*sqrt(3))]
								  [  0             -sqrt(2)/sqrt(3)  1/sqrt(3)          ] */
		int tripleFourthTest (void)
		{
			TestUtil testFramework( "RACRotation", "tripleFourthTest", __FILE__, __func__ );
			testFramework.init();

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

//--------------RACRotation_tripleFourthTest_1 - Was the X value rotated properly?			
		   	testFramework.assert(fabs(x - rotatedXYZ[0]) < eps);
		   	testFramework.next();

//--------------RACRotation_tripleFourthTest_2 - Was the Y value rotated properly?
		   	testFramework.assert(fabs(y - rotatedXYZ[1]) < eps);
		   	testFramework.next();

//--------------RACRotation_tripleFourthTest_3 - Was the Z value rotated properly?
		   	testFramework.assert(fabs(z - rotatedXYZ[2]) < eps);
		   	testFramework.next();

		   	XYZ = Triple( -1.0, -1.0, -1.0);
		   	rotatedXYZ = rot4.convertToRAC( XYZ );

			x = -1.0 - 1.0/sqrt(2.0);
			y = -(-2.0+sqrt(2.0))/(2.0*sqrt(3.0));
			z = -(1.0 - sqrt(2.0))/sqrt(3.0);

//--------------RACRotation_tripleFourthTest_4 - Was the X value rotated properly?			
		   	testFramework.assert(fabs(x - rotatedXYZ[0]) < eps);
		   	testFramework.next();

//--------------RACRotation_tripleFourthTest_5 - Was the Y value rotated properly?
		   	testFramework.assert(fabs(y - rotatedXYZ[1]) < eps);
		   	testFramework.next();

//--------------RACRotation_tripleFourthTest_6 - Was the Z value rotated properly?
		   	testFramework.assert(fabs(z - rotatedXYZ[2]) < eps);
		   	testFramework.next();

		   	XYZ = Triple( 0.0, 0.0, -1.0);
		   	rotatedXYZ = rot4.convertToRAC( XYZ );

			x = -1.0/sqrt(2.0);
			y = -sqrt(2.0)/(2.0*sqrt(3.0));
			z = -1.0/sqrt(3.0);

//--------------RACRotation_tripleFourthTest_7 - Was the X value rotated properly?			
		   	testFramework.assert(fabs(x - rotatedXYZ[0]) < eps);
		   	testFramework.next();

//--------------RACRotation_tripleFourthTest_8 - Was the Y value rotated properly?
		   	testFramework.assert(fabs(y - rotatedXYZ[1]) < eps);
		   	testFramework.next();

//--------------RACRotation_tripleFourthTest_9 - Was the Z value rotated properly?
		   	testFramework.assert(fabs(z - rotatedXYZ[2]) < eps);
		   	testFramework.next();

		   	XYZ = Triple( 0.0, -1.0, 0.0);
		   	rotatedXYZ = rot4.convertToRAC( XYZ );

			x = -0.5;
			y = -1.0/(2.0*sqrt(3.0));
			z = sqrt(2.0)/sqrt(3.0);

//--------------RACRotation_tripleFourthTest_10 - Was the X value rotated properly?			
		   	testFramework.assert(fabs(x - rotatedXYZ[0]) < eps);
		   	testFramework.next();

//--------------RACRotation_tripleFourthTest_11 - Was the Y value rotated properly?
		   	testFramework.assert(fabs(y - rotatedXYZ[1]) < eps);
		   	testFramework.next();

//--------------RACRotation_tripleFourthTest_12 - Was the Z value rotated properly?
		   	testFramework.assert(fabs(z - rotatedXYZ[2]) < eps);

			return testFramework.countFails();
		}

		/*	Repeat first test case using the Xvt signatures of
			the RACRotation class.
			Resulting RAC matrix: [  1  0  0]
			                      [  0  0  1]
					 			  [  0 -1  0] */
		int xvtTest (void)
		{
			TestUtil testFramework( "RACRotation", "xvtTest", __FILE__, __func__ );
			testFramework.init();

			double GPSAlt = 26000000;
			Triple XYZ, rotatedXYZ;
		   	Xvt refPoint;
		   	refPoint.x = Position( GPSAlt, 0, 0 );
		   	refPoint.v = Triple( 0, 0, 4000 );
		   	refPoint.clkbias = 0.0;
		   	refPoint.clkdrift = 0.0;
		   	refPoint.relcorr = 0.0;

		   	RACRotation rot5( refPoint );

		      	// OK, now set up unit vectors in radius and velocity to 
                        // see if the results align properly.
		   	Xvt testPoint;
		   	testPoint.x = Position( 1.0, 0.0, 0.0 );
		   	testPoint.v = Triple( 0.0, 0.0, 1.0 );
		   	testPoint.clkbias = 0.0;
		   	testPoint.clkdrift = 0.0;
		   	testPoint.relcorr = 0.0;

		   	Xvt rotatedTestPoint = rot5.convertToRAC( testPoint );

//--------------RACRotation_xvtTest_1 - Were the position values stored properly?
		   	testFramework.assert(Position(1,0,0) == (gpstk::Position)rotatedTestPoint.x);
		   	testFramework.next();

//--------------RACRotation_xvtTest_2 - Were the velocity values stored properly?
		   	testFramework.assert(Triple(0,1,0) == rotatedTestPoint.v);
		   	testFramework.next();

//--------------RACRotation_xvtTest_3 - Was the first velocity value stored properly?
   			testFramework.assert(0 == rotatedTestPoint.v[0]);
   			testFramework.next();

//--------------RACRotation_xvtTest_4 - Was the second velocity value stored properly?
   			testFramework.assert(1 == rotatedTestPoint.v[1]);
   			testFramework.next();

//--------------RACRotation_xvtTest_5 - Was the third velocity value stored properly?
   			testFramework.assert(0 == rotatedTestPoint.v[2]);
   			testFramework.next();

//--------------RACRotation_xvtTest_6 - Was the clock bias value stored properly?
		   	testFramework.assert(0.0 == rotatedTestPoint.clkbias);
		   	testFramework.next();

//--------------RACRotation_xvtTest_7 - Was the clock drift value stored properly?
		   	testFramework.assert(0.0 == rotatedTestPoint.clkdrift);
		   	testFramework.next();

//--------------RACRotation_xvtTest_8 - Was the relativity correction value stored properly?
		   	testFramework.assert(0.0 == rotatedTestPoint.relcorr);
		   	testFramework.next();

		      	// Using more comlpex test vectors.
		   	testPoint.x = Position( 0.0, 0.5, -0.5);
		   	testPoint.v = Triple(  1.0, -1.0, 1.0);
		   	testPoint.clkbias = 0.0;
		   	testPoint.clkdrift = 0.0;
		   	testPoint.relcorr = 0.0;

		   	rotatedTestPoint = rot5.convertToRAC( testPoint );

//--------------RACRotation_xvtTest_9 - Were the position values stored properly?
		   	testFramework.assert(Position(0, -0.5, -0.5) == (gpstk::Position)rotatedTestPoint.x);
		   	testFramework.next();

//--------------RACRotation_xvtTest_10 - Were the velocity values stored properly?
		   	testFramework.assert(Triple(1.0, 1.0, 1.0) == rotatedTestPoint.v);
		   	testFramework.next();

//--------------RACRotation_xvtTest_11 - Was the first velocity value stored properly?
   			testFramework.assert(1 == rotatedTestPoint.v[0]);
   			testFramework.next();

//--------------RACRotation_xvtTest_12 - Was the second velocity value stored properly?
   			testFramework.assert(1 == rotatedTestPoint.v[1]);
   			testFramework.next();

//--------------RACRotation_xvtTest_13 - Was the third velocity value stored properly?
   			testFramework.assert(1 == rotatedTestPoint.v[2]);
   			testFramework.next();

//--------------RACRotation_xvtTest_14 - Was the clock bias value stored properly?
		   	testFramework.assert(0.0 == rotatedTestPoint.clkbias);
		   	testFramework.next();

//--------------RACRotation_xvtTest_15 - Was the clock drift value stored properly?
		   	testFramework.assert(0.0 == rotatedTestPoint.clkdrift);
		   	testFramework.next();

//--------------RACRotation_xvtTest_16 - Was the relativity correction value stored properly?
		   	testFramework.assert(0.0 == rotatedTestPoint.relcorr);
		   	testFramework.next();

			// The conversions should still accept any Triples
		   	XYZ = Triple( 1.0, 1.0, -1.0);
		   	rotatedXYZ = rot5.convertToRAC( XYZ );

//--------------RACRotation_xvtTest_16 - Was the Triple converted properly?
		   	testFramework.assert(Triple(1,-1,-1) == rotatedXYZ);
			testFramework.next();

//--------------RACRotation_xvtTest_17 - Was the x value rotated properly?
   			testFramework.assert(1 == rotatedXYZ[0]);
   			testFramework.next();

//--------------RACRotation_xvtTest_18 - Was the y value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[1]);
   			testFramework.next();

//--------------RACRotation_xvtTest_19 - Was the z correction value rotated properly?
   			testFramework.assert(-1 == rotatedXYZ[2]);

			return testFramework.countFails();
		}
	private:
		double eps;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	RACRotation_T testClass;

	check = testClass.tripleFirstTest();
	errorCounter += check;

	check = testClass.tripleSecondTest(); 
	errorCounter += check;

	check = testClass.tripleThirdTest();
	errorCounter += check;

	check = testClass.tripleFourthTest();
	errorCounter += check;

	check = testClass.xvtTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
