#include "ValidType.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

class ValidType_T
{
public: 
   ValidType_T(){ eps = 1E-12;}// Default Constructor, set the precision value
   ~ValidType_T() {} // Default Desructor

   int methodTest(void)
   {
      gpstk::TestUtil testFramework( "ValidType", "Various Methods", __FILE__, __LINE__ );
      std::string failMesg;

      gpstk::ValidType<float> vfloat0;

      failMesg = "Is the invalid Valid object set as valid?";
      testFramework.assert(!vfloat0.is_valid(), failMesg, __LINE__);

      failMesg = "Is the invalid Valid object's value 0?";
      testFramework.assert(std::abs(vfloat0.get_value()) < eps, failMesg, __LINE__);

      gpstk::ValidType<float> vfloat (5);

      failMesg = "Does the get_value method return the correct value?";
      testFramework.assert(vfloat.get_value() == 5, failMesg, __LINE__);

      failMesg = "Is the valid Valid object set as valid?";
      testFramework.assert(vfloat.is_valid(), failMesg, __LINE__);

      vfloat.set_valid(false);

      failMesg = "Was the valid Valid object correctly set to invalid?";
      testFramework.assert(!vfloat.is_valid(), failMesg, __LINE__);

      return testFramework.countFails();
   }

   int operatorTest(void)
   {
      gpstk::TestUtil testFramework( "ValidType", "== Operator", __FILE__, __LINE__ );
      std::string failMesg;

      gpstk::ValidType<float> Compare1 (6.);
      gpstk::ValidType<float> Compare2 (6.);
      gpstk::ValidType<float> Compare3 (8.);
      gpstk::ValidType<int> Compare4 (6);
      gpstk::ValidType<float> vfloat;

      failMesg = "Are two equvalent objects equal?";
      testFramework.assert(Compare1 == Compare2, failMesg, __LINE__);

      failMesg = "Are two non-equvalent objects equal?";
      testFramework.assert(Compare1 != Compare3, failMesg, __LINE__);

      vfloat = 7.;

      testFramework.changeSourceMethod("= Operator");
      failMesg = "Did the = operator store the value correctly?";
      testFramework.assert(vfloat.get_value() == 7., failMesg, __LINE__);

      failMesg = "Did the = operator set the object as valid?";
      testFramework.assert(vfloat.is_valid(), failMesg, __LINE__);

      testFramework.changeSourceMethod("+= Operator");

      vfloat += 3.;
      failMesg = "Did the += operator store the value correctly?";
      testFramework.assert(vfloat.get_value() == 10., failMesg, __LINE__);

      failMesg = "Did the += operator change the object's valid bool?";
      testFramework.assert(vfloat.is_valid(), failMesg, __LINE__);		

      testFramework.changeSourceMethod("-= Operator");

      vfloat -= 5.;

      failMesg = "Did the -= operator store the value correctly?";
      testFramework.assert(vfloat.get_value() == 5., failMesg, __LINE__);

      failMesg = "Did the -= operator change the object's valid bool?";
      testFramework.assert(vfloat.is_valid(), failMesg, __LINE__);

      testFramework.changeSourceMethod("<< Operator");

      vfloat = 11;

      std::stringstream streamOutput;
      std::string stringOutput;
      std::string stringCompare;

      streamOutput <<  vfloat;
      stringOutput = streamOutput.str();

      stringCompare = "11";

      failMesg = "Did the << operator ouput valid object correctly?";
      testFramework.assert(stringCompare == stringOutput, failMesg, __LINE__);

      streamOutput.str("");	// Resetting stream
      vfloat.set_valid(false);

      streamOutput << vfloat;
      stringOutput = streamOutput.str();

      stringCompare = "Unknown";

      failMesg = " Did the << operator output invalid object correctly?";
      testFramework.assert(stringCompare == stringOutput, failMesg, __LINE__);

      return testFramework.countFails();
   }

private:
   double eps;
};

int main() //Main function to initialize and run all tests above
{
	int check = 0, errorCounter = 0;
	ValidType_T testClass;

	check = testClass.methodTest();
	errorCounter += check;

	check = testClass.operatorTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
