#include "Triple.hpp"
#include <iostream>
#include <cmath>
using namespace std;
using namespace gpstk;
class TripleTest
{
        public: 
		TripleTest(){eps = 1e-12;}// Default Constructor, set the precision value
		~TripleTest() {} // Default Desructor
		double eps;
		int setTest()
		// Test to initialize and set Triple objects. 
		{
			Triple test, test2(1,2,3), test3(test2);
			test = valarray<double>(3);		

			//std::cout << "The Average is: " << test.Average() << std::endl;
			if ((test.size() == 3) && (test2.size() == 3) && (test3.size() == 3))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		};
		int dotTest()
		// Verify the dot product calculation.
		{
			Triple test(1,2,3),test2(2,2,2);
			double result;

			result = test.dot(test2);

			//std::cout << "The dot product is: " << result << std::endl;
			if (result == 12)
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int crossTest()
		// Verify the cross product calculation.
		{
			Triple test(1,2,3), test2(2,2,2), test3;
			
			test3 = test.cross(test2);

			//std::cout << "The cross product is: " << test3 << std::endl;
			if ((test3[0] == -2) && (test3[1] == 4) && (test3[2] == -2))
			{
				return 0;
			}
			else
			{
				return 1;
			}
			
		}
		int magTest()
		// Verify the magnitude calculation.
		// Note this presumes the L2 (Euclidian) norm of the vector.
		{
			Triple test(3,4,0);
			if (test.mag() != 5) return 1;
			test[0] = 0; test[1] = 0; test[2] = -2;
			if (test.mag() != 2) return 2;
			return 0;	
		}
		int unitVectorTest()
		// Verify the unit vector calculation.
		{
			Triple test(3,4,0),test2;
			test2 = test.unitVector();
			if ((test2[0] != 3.0/5.0) && (test2[1] != 4.0/5.0) && (test2[2] != 0)) return 1;
			test[0] = 0; test[1] = 0; test[2] = -2;
			test2 = test.unitVector();
			if ((test2[0] != 0) && (test2[1] != 0) && (test2[2] != 1.0)) return 2;
			return 0;	
		}
		int cosVectorTest()
		// Verify the cosine of the angle between two triples calculation.
		{
			Triple test(1,0,1),test2(-1,0,-1);
			double angle;
			if (fabs(test.cosVector(test2) + 1) > eps) return 1;
			test2[0] = 0; test2[1] = 1; test2[2] = 0;
			if (fabs(test.cosVector(test2)) > eps) return 2;
			test[0] = 1; test[1] = 0; test[2] = 0;
			test2[0] = 1; test2[1] = 1; test2[2] = 0;
			if (fabs(test.cosVector(test2) - sqrt(2.0)/2.0) > eps) return 3;
			return 0;	
		}
		int slantRangeTest()
		// Verify the slant range calculation
		{
			Triple test(4,6,3),test2(1,2,3);
			if (fabs(test.slantRange(test2)- 5) > eps) return 1;
			test[0] = 11; test[1] = -12; test[2] = 10;
			test2[0] = 1; test2[1] = 2; test2[2] = 3;
			if (fabs(test.slantRange(test2) - sqrt(345.0)) > eps) return 2;
			return 0;	
		}
		int elvAngleTest()
		// Verify the elevation angle calculation
		{
			Triple test(1,0,0),test2(0,-1,0);
			double result;
			if (fabs(test.elvAngle(test2) + 45) > eps) return 1;
			test[0] = 11; test[1] = -12; test[2] = 10;
			test2[0] = 1; test2[1] = 2; test2[2] = 3;
			result = acos(-348.0/(sqrt(365.0)*sqrt(345.0)))*180.0/(4.0*atan(1.0));
			result = 90 - result;
			if (fabs(test.elvAngle(test2) - result) > eps) return 2;
			test[0] = 1; test[1] = 1; test[2] = 1;
			test2[0] = 1; test2[1] = 0; test2[2] = 0;
			if (fabs(test.elvAngle(test2) - (90 - acos(-2.0/sqrt(6.0))*180.0/(4.0*atan(1.0)))) > eps) return 3;
			return 0;	
		}
		int azAngleTest()
		// Verify the azimuthal angle calculation
		// Uses relative error to check the number of correct digits
		{
			Triple test(1,1,1),test2(-1,1,1);
			if (fabs(test.azAngle(test2) - 60) > eps) return 1;
			
			test[0] = 11; test[1] = -12; test[2] = 10;
			test2[0] = 1; test2[1] = 2; test2[2] = 3;
			if (fabs(test.azAngle(test2) - 35.0779447169289) > eps) return 2;

			test2[0] = 0; test2[1] = 0; test2[2] = 0;
			if (fabs(test.azAngle(test2) - 270)/270 > eps) return 3;

			test[0] = 1; test[1] = 0; test[2] = 0;
			test2[0] = 0; test2[1] = 1; test2[2] = 0;
			if (fabs(test.azAngle(test2) - 90)/90 > eps) return 4;

			test[0] = 1; test[1] = -1; test[2] = 1;
			test2[0] = 1; test2[1] = 1; test2[2] = 1;
			if (fabs(test.azAngle(test2) - 60)/60 > eps) return 5;
			return 0;
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
	TripleTest testClass;
	check = testClass.setTest();
        std::cout << "Set Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.dotTest();
        std::cout << "Dot Product Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.crossTest();
        std::cout << "Cross Product Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.magTest();
        std::cout << "magTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.unitVectorTest();
        std::cout << "unitVectorTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.cosVectorTest();
        std::cout << "cosVectorTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.slantRangeTest();
        std::cout << "slantRangeTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.elvAngleTest();
        std::cout << "elvAngleTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.azAngleTest();
        std::cout << "azAngleTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
