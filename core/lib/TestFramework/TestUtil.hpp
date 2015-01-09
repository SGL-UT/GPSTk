#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

//============================================================
// class:   TestUtil
// purpose: TestUtil is a proposed utility class (not parent class)
//          for use with test classes and test methods in GPSTk.
// Example: Source usage for a test method with 4 sub-tests:
//
//     TestUtil myTest( "SomeClass", "SomeMethod", "myTestFile", "myTestMethod" );
//
//     myTest.init();
//     myTest.assert( 1==2 );
//     myTest.print();
//
//     myTest.next();
//     myTest.assert( 1==1 );
//     myTest.print();
//
//     myTest.next();
//     if( 2 > 1 ), myTest.fail();
//     myTest.print();
//
//     myTest.next();
//     if( 2 < 1 ), myTest.fail();
//     myTest.print();
//
// Output: stdout from above myTest.print() calls would be:
//
//     TestOutput, SomeClass, SomeMethod, myTestFile, myTestMethod, 1, 1
//     TestOutput, SomeClass, SomeMethod, myTestFile, myTestMethod, 2, 0
//     TestOutput, SomeClass, SomeMethod, myTestFile, myTestMethod, 3, 0
//     TestOutput, SomeClass, SomeMethod, myTestFile, myTestMethod, 4, 1
//
//============================================================
class TestUtil
{
public:

  //----------------------------------------
  // Method:  TestUtil::TestUtil()
  // Purpose: Constructor
  // Usage:   To be called at the start of each test method
  // Inputs:  source_class  = the name of the source class being tested
  //          source_method = the name of the source method being tested
  //          test_file     = the name of file containing the test code, e.g., __FILE__
  //          test_method   = the name of test method doing the testing, e.g. __func__
  // Outputs: none
  //----------------------------------------
  TestUtil( std::string const& sourceClassInput  = "Unknown", 
            std::string const& sourceMethodInput = "Unknown",
            std::string const& testFileInput     = "Unknown",
            std::string const& testMethodInput   = "Unknown"
           ):
      outputKeyword( "TestOutput" ), 
      sourceClass( sourceClassInput  ),
      sourceMethod( sourceMethodInput ),
      testFilename( testFileInput ),
      testMethod( testMethodInput ),
      failBit( 0 ),
      failCount( 0 ),
      testCount( 0 ),
      subtestID( 1 ),
      tolerance( 0 )
  {

  }

  //----------------------------------------
  // Method:  TestUtil::init()
  // Purpose: Reset the failbit, failCount, testCount, and subtestID
  // Usage:   To be called at the beginning of the FIRST subtest in a method
  //          never after that.
  // Inputs:  none
  // Outputs: none
  //----------------------------------------
  void init( void )
  {
    failBit   = 0;
    failCount = 0;
    testCount = 0;
    subtestID = 1;
    tolerance = 0;
  }

  //----------------------------------------
  // Method:  TestUtil::next()
  // Purpose: Increment the failCount and reset subtestID based on current testCount
  // Usage:   To be called at the beginning of each subtest AFTER the first
  // Inputs:  none
  // Outputs: none
  //----------------------------------------
  void next( void )
  {
    failBit = 0;
    subtestID = countTests() + 1;
  }

  //----------------------------------------
  // Method:  TestUtil::fail()
  // Purpose: Fail the test! Record a failure by setting the failBit and
  //           incrementing failCount
  // Usage:   To be called (once!) at the end of any subtest that has failed
  // Inputs:  none
  // Outputs: none
  //----------------------------------------
  void fail( void )
  {
    failBit = 1;
    failCount++;
    testCount++;
  }

  //----------------------------------------
  // Method:  TestUtil::pass()
  // Purpose: Pass the test! Record a pass by setting the failBit=0 and
  //           incrementing the testCount
  // Usage:   To be called (once!) at the end of any subtest that has passed
  // Inputs:  none
  // Outputs: none
  //----------------------------------------
  void pass( void )
  {
    failBit = 0;
    testCount++;
  }

  //----------------------------------------
  // Method:  TestUtil::undo()
  // Purpose: undo the test! Undo a pass/fail by unsetting failBit and
  //           decrementing failCount (only if failed) and 
  //           decrementing the testCount
  // Usage:   To be called at the end of any subtest that needs to be undone
  // Inputs:  none
  // Outputs: none
  //----------------------------------------
  void undo( void )
  {
    if( failBit==1 )
      { 
         failBit = 0;
         failCount--;
         testCount--;
      }
    else
      {
         failBit = 0;
         testCount--;
      }
    next();
  }

  //----------------------------------------
  // Method:  TestUtil::print()
  // Purpose: print test results and information on classes being tested
  //          to stdout in a common format that is both human-readable
  //          and easy to filter using tools like grep so as to help
  //          isolate where problems are happening.
  // Usage:   to be called after each test method subtest is performed
  // Inputs:  none
  // Outputs: none
  // STDOUT:  "outputKeyword, sourceClass, sourceMethod, testFilename,
  //             testMethod, subtestID, failBit"
  //----------------------------------------
  void print( void )
  {
      // print test description and result to stdout
      std::cout     <<
      outputKeyword << ", " << 
      sourceClass   << ", " << 
      sourceMethod  << ", " << 
      testFilename  << ", " << 
      testMethod    << ", " <<
      subtestID     << ", " << 
      failBit       << std::endl;     // implicit conversion from int to string
  }

  //----------------------------------------
  // Method:  TestUtil::assert()
  // Purpose: Takes a boolean expression, passes or fails the test, depending on
  //          whether the assertion is true or false, and then prints the result
  // Usage:   Use when a test can be written as boolean expression.
  // Inputs:  boolean testExpression
  // Outputs: none
  //----------------------------------------
  void assert( bool testExpression )
  {
    if( testExpression == false )
      {
        fail();
      }
    else
      {
        pass();
      }
    print();
  }

  //----------------------------------------
  // Method:  TestUtil::countFails()
  // Purpose: Return the number of tests that have failed so far
  // Usage:   To be called at the end of the test method and AFTER all tests
  // Inputs:  none
  // Outputs: int failCount
  //----------------------------------------
  int countFails( void )
  {
    return( failCount );
  }

  //----------------------------------------
  // Method:  TestUtil::countTests()
  // Purpose: Return the number of tests that have been run so far
  // Usage:   To be called at the end of the test method and AFTER all tests
  // Inputs:  none
  // Outputs: int testCount
  //----------------------------------------
  int countTests( void )
  {
    return( testCount );
  }

private:

  // The following are all used as part of the output from TestUtil::print()
  // to facilitate filtering of output that is thus printed to stdout

  std::string outputKeyword; // Identifies a stdout line as from this class
  std::string sourceClass;   // help locate source class causing a test failure
  std::string sourceMethod;  // help locate source method causing a test failure
  std::string testFilename;  // help locate test file that discovered a failure
  std::string testMethod;    // help locate test method that discovered a failure
  std::string testLine;      // help locate line number where the test code lives
  int         failBit;       // store the result of a test (0=pass, 1=fail)

  double      tolerance;     // acceptable difference between test output and 
                             //  expected or baseline output

  //  since single test methods may contain multiple subtests.

  int subtestID; // ID of the current sub-test, used in TestUtil::print()
  int failCount; // Count of tests that have fails
  int testCount; // Count of tests that have been run

};
