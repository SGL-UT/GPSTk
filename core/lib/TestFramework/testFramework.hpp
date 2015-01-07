#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;
/*
 Class testFramework is the parent class for the automated tests.
 The class will contain methods and variables to be shared among all tests.
*/
class testFramework
{
	public:
		// testSetup is a method to reset the counters at the start of each test method.
		void testSetup()
		{
			failCount = 0;
			testCount = 1;
		}

		/* testAssertion takes a testable assertion (or its result) and provides an output to screen.
		   The output is designed to have the following structure (with no spaces):

		   TestOutput: sourceClass, sourceMethod, testFilename, testMethod, testID, failBit

		   Where the various components are:
		   TestOutput:  - A delimiter to easily pull out test output lines
		   sourceClass  - The class in the source directory which is being tested
		   sourceMethod - The particular method/operator/function of the sourceClass being tested
		   testFilename - The filename of the test source file
		   testMethod   - The method in the test source file which executed the testable assertion
		   testID       - Identifier for the particular testable assertion
		   failBit      - Bit value where 1 = Failed, 0 = Passed

		   Additionally this information may be used to find the failed assertion. One can grep for
		   the line containing sourceClass_testMethod_testID in testFilename. The line after will contain
		   the assertion. The line returned by the grep will also contain some indicator of what the
		   the assertion is to verify.

		   Finally the testAssertion method will update the two counters. */
		void testAssertion(bool result, string sourceMethod, string testFilename, string testMethod)
		{
			cout << outputString << "," << \
				sourceMethod << "," << \
				testFilename << "," << \
				testMethod   << "," << \
				testCount    << "," << \
				(bool)(!result) << endl;
			if (!result) failCount++;
			testCount++;
		}

	protected:
		std::string outputString; //String for grep-able output. Is to contain the "TestOutput:sourceClass" information.
		bool passFail; //Storage for the result of a test assertion
		int failCount; //Number of failed tests in a given test method
		int testCount; //Counter for number of tests in a given test method
		double eps; //For floating point variables the level of acceptable difference
};
