Testing of the GPSTk Library
----------------------------

Contents:
---------

* Introduction
* UNIX-like Environment: Testing Procedure
* UNIX-like Environment: Automated Build and Test
* UNIX-like Environment: Manual Build and Test
* OSX and XCode: Testing Procedure
* Windows and Microsoft Visual Studio: Testing Procedure
* CTest Developer Testing Commands
* Unit Test Guidelines
* Writing New Unit Tests


Introduction:
-------------

As an open source project, the source of the GPSTk is subject to intermittent updates, contributions, 
and corrections. The GPSTk library testing process has been redesigned to build confidence in the 
functionality of the library. Testing within the GPSTk library is designed with the following distinct goals 
in mind:
	
	Testing is repeatable with a low amount of effort.
	Testing is distributed along with the library to support both internal testing and 
     to assure outside users and contributors of the quality of the library.
	Testing is designed to accommodate easy additions to the existing test suite.
  Testing is implemented to ensure changes have not broken functionality.

Currently, the test suite is only supported for UNIX-like environments.


UNIX-like Environment: Testing Procedure:
-----------------------------------------

There are two main methods for running the GPSTk test suite in a UNIX-like Environment:

	Automated build and test utilizing the build.sh script
	Manual build and test utilizing CMake and CTest


UNIX-like Environment: Automated Build and Test:
------------------------------------------------

If you prefer automation, run the build.sh script.

Typical test build without install will look like this:
	
	$ ./build.sh -et


Typical test build with install will look like this:

    $ export LD_LIBRARY_PATH=/tmp/test/lib:$LD_LIBRARY_PATH

    $ ./build.sh -et -i /tmp/test


To run a detailed test report after after build, install, and test:

    $ ./test-report.sh


UNIX-like Environment: Manual Build and Test:
---------------------------------------------

To manually build and test the GPSTk, follow the following steps:

   1. Create a directory for your build:

         $ cd $gpstk_root

         $ mkdir $gpstk_root/build

   2. Change into the $gpstk_root/build directory, this will be where all of the
      build files will reside.

   3. To build the GPSTk, execute the following commands:

         $ cd $gpstk_root/build

         $ cmake -DTEST_SWITCH=ON $gpstk_root

         $ make
   
   4. To run the GPSTk test suite:

   		 $  make test

   		 	or

   		 $ ctest

   5. To run a test report:

       $ cd $gpstk_root

       $ ./test-report.sh


OSX and XCode: Testing Procedure:
---------------------------------

For development in Terminal, the above Unix-like Environment methods (automated/manual) can be used.

Step-by-step procedure with pictures on building, installing, and testing the GPSTk under Xcode can 
be found at gpstk.org:

    http://www.gpstk.org/bin/view/Documentation/BuildingGPSTkUnderOSX 


Windows and Microsoft Visual Studio: Testing Procedure:
-------------------------------------------------------

Step-by-step procedure with pictures on building, installing, and testing the GPSTk under Visual 
Studio 2012 can be found at gpstk.org:

    http://www.gpstk.org/bin/view/Documentation/BuildingGPSTkUnderWindows


CTest Developer Testing Commands:
---------------------------------

Developers may not want to run the full suite of GPSTk tests, as outlined in the previous sections. 
CTest supports multiple methods to specify subset of tests to list, run, & omit.

	To determine what tests are available in list format without running them:
         
         $ ctest -N

    To run an individual test, specify the exact test name. For example:

         $ ctest TestRinexHeader

    To run one or more tests that match a regular expression, use the -R option. For example:

         $ ctest -R Rinex

    To omit one or more tests that match a regular expression, use the -E option. For example:

         $ ctest -E Python     

    To run tests using explicit test numbers, use option -I.  For example:

         Format: -I [Start,End,Stride,test#,test#|Test file]

         To run tests 3 - 5:
         	
         	$ ctest -I 3,5

         To specify stride:

         	$ ctest -I ,,3

         To run individual tests:

         	$ ctest -I 4,4,,4,7,13


Unit Testing Guidelines:
------------------------

Guidelines for writing unit tests can be found at:

  http://sglwiki/bin/view/GPSTk/GPSTk_UnitTesting

For Unit Testing styles/tools, refer to /core/lib/TestFramework/TestUtil.hpp. 


Writing New Unit Tests:
-----------------------

----------------------
Current Test Structure
----------------------

   * Tests are run using CTest tools directly or as a part of the build process. For more information on 
      running the tests refer to the above sections in this document.
   * Unit tests for a particular GPSTk library class are often organized in a single cpp file titled by 
      the GPSTk library class with a _T.cpp appended.
   * The individual cpp files are broken into two parts, a test class to test the GPSTk library class and 
      a main() segment to run those tests.
   * The test class is organized into multiple public methods in which each method contains multiple 
      assertions which test a particular feature of the GPSTk library class under test.
   * The test class might inherit from the GPSTk library class in order to access protected members for 
      direct checking of values.
   * To facilitate reporting to the testing logs, GPSTk uses its own TestUtil class.
   * TestUtil provides standardized output containing information on the GPSTk library class being tested, 
      feature of class being tested, test file name, line number of test in that file, pass/fail bit, and 
      a failure message should the test have failed. It also provides the number of failures to the main() 
      portion of the test cpp file.
   * The main() portion of the code creates the test class object and executes its methods. It then tallies 
      the number of failures and reports it to the screen/log.


-------------------------
Example Test File
-------------------------

  #include "Foo.hpp"
  #include "Exception.hpp"
  #include "TestUtil.hpp"
  #include <iostream>


  // Foo_T is the test class for the Foo class.
  // The Test class is designed to be a container for test methods. The test methods
  // will then run the single unit test assertions. 
  class Foo_T : public gpstk::Foo
  {
      public:
    //------------------------------------------------------------
    // Default Constructor for the Test class
    //------------------------------------------------------------
    Foo_T()
    {
        eps = 1e-14;
    }


    //============================================================
    // Test Suite: constructorTest()
    //============================================================
    //  Test to see if any of the constructors work as planned
    //============================================================
    int constructorTest( void )
    {
        //Create a TestUtil object to store relevant info about:
        //      1) The class being tested
        //      2) What method/feature of the class is being tested
        //      3) Which test file is this? (For reporting in the logs)
        //      4) Which TestUtil object to which the later assertions belong 

        TestUtil testFramework( "Foo", "Constructor", __FILE__, __LINE__ );

        //----------------------------------------
        // Default Constructor test
        //----------------------------------------
        try
        {
            // Create a Foo object
            Foo foo;
            
            // The below assert tests if the creation of the Foo object did not return an error and 
            // it should not return an error, pass the assert with a true bit.
            // Note that the assert still requires all 3 inputs: 1) The bool, 
            // 2) The failure description, and 3) the line of the assert.
            testFramework.assert( true, "Description that the constructor worked properly", __LINE__ );
        }
        catch(...)
        {
            // If the creation of the Foo object did returns an error and it should not return an error, 
            fail the assert with a false bit.

            testFramework.assert( false, "Description that the constructor could not create the object properly", __LINE__ );
        }

        //Return the number of failed tests
        return testFramework.countFails();
    }


    //============================================================
    // Test Suite: methodTest()
    //============================================================
    // These tests check to see if methods/features of the class
    // work as intended
    //
    //
    //
    // For the purposes of this demonstration:
    // Imagine that the Foo class has a method bar() which should 
    // return true by default and a method getX() which is returns
    // a double that is always less than 100.0.
    //============================================================
    int constructorTest( void )
    {
        TestUtil testFramework( "Foo", "bar", __FILE__, __LINE__ );

        double diff;
        Foo foo;

        // The assert line to check if bar() does indeed return true would look like:
        testFramework.assert(foo.bar(), "Error Message describing how/that bar() returned false", __LINE__)

        // If multiple class methods are tested in the same block you can change the method name.
        testFramework.changeSourceMethod("getX")

        // The assert line to check if x fits within its necessary bound would look like:
        diff = fabs(100.0 - foo.getX())/100.0;
        testFramework.assert(diff < eps, "Error Message describing how x is greater than 100.0", __LINE__)


        //Return the number of failed tests
        return testFramework.countFails();
    }

     private:
       double eps; //Tolerance for floating point differences
  };


  //============================================================
  // Main function to initialize and run all tests above
  //============================================================
  int main( void )
  {
      int check, errorCounter = 0;
      Foo_T testClass;

      //----------------------------------------
      // Run all test methods
      //----------------------------------------

      check = testClass.constructorTest();
      errorCounter += check;

      check = testClass.methodTest();
      errorCounter += check;

      //----------------------------------------
      // Echo total fails to stdout
      //----------------------------------------
      std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

      //----------------------------------------
      // Return total fails
      //----------------------------------------
      return errorCounter;
  }


----------------------
Example Test Output
----------------------

Presuming that the default constructor works properly, the bar() method returns false, and the getX method functions, the output will look like:


  GpstkTest, Class=Foo, Method=Constructor, testFile=Foo_T.cpp, testLine=49, subtest=1, failBit=0
  GpstkTest, Class=Foo, Method=bar, testFile=Foo_T.cpp, testLine=83, subtest=1, failBit=1, testMsg=Error Message describing how/that bar() returned false
  GpstkTest, Class=Foo, Method=getX, testFile=Foo_T.cpp, testLine=90, subtest=2, failBit=0
  Total Failures for /home/nfitz/git/gpstk/core/tests/TimeHandling/ANSITime_T.cpp: 1


