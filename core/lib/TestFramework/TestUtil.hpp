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
  // Method:  TestUtil
  // Purpose: Constructor
  // Usage:   To be called at the start of each test method
  // Inputs:  4
  //          source_class  = the name of the source class being tested
  //          source_method = the name of the source method being tested
  //          test_file     = the name of file containing the test code 
  //          test_method   = the name of test method doing the testing
  // Outputs: none
  //----------------------------------------
  TestUtil( std::string const& source_class, 
            std::string const& source_method,
            std::string const& test_file,
            std::string const& test_method  )
  {
    sourceClass   = source_class;
    sourceMethod  = source_method;
    testFilename  = test_file;
    testMethod    = test_method;

    init();
  }

  //----------------------------------------
  // Method:  init
  // Purpose: Reset the failbit, failCount, and method subtest ID
  // Usage:   To be called at the beginning of the FIRST subtest in a method
  //          never after that.
  // Inputs:  none
  // Outputs: none
  //----------------------------------------
  void init( void )
  {
    failBit   = 0;
    failCount = 0;
    subtestID = 1;
  }

  //----------------------------------------
  // Method:  next()
  // Purpose: Increment the failCount and test method subtest ID
  // Usage:   To be called at the beginning of each subtest AFTER the first.
  // Inputs:  none
  // Outputs: none
  //----------------------------------------
  void next( void )
  {
    failBit = 0;
    subtestID++;
  }

  //----------------------------------------
  // Method:  fail()
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
  }

  //----------------------------------------
  // Method:  unfail()
  // Purpose: unFail the test! Undo a failure by unsetting failBit and
  //            decrementing failCount
  // Usage:   To be called at the end of any subtest that needs to be undone
  // Inputs:  none
  // Outputs: none
  //----------------------------------------
  void unfail( void )
  {
    failBit = 0;
    failCount--;
  }

  //----------------------------------------
  // Method:  print()
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
  void print( )
  {

      // the C++ compiler on clau is fucking retarded!
      std::ostringstream ss;
      ss << failBit;
      std:string failString = ss.str();

      // print shit
      std::cout     <<
      outputKeyword << ", " << \
      sourceClass   << ", " << \
      sourceMethod  << ", " << \
      testFilename  << ", " << \
      testMethod    << ", " << \
      subtestID     << ", " << \
      failString    << endl;
  }

  //----------------------------------------
  // Method:  assert()
  // Purpose: Takes a boolean expression, sets fail is assertion is false
  // Usage:   To be called during any test that needs a bollean expression tested.
  // Inputs:  1
  //          boolean testExpression
  // Outputs: none
  //----------------------------------------
  void assert( bool testExpression )
  {
    if( testExpression == false )
      {
        fail();
      }
  }

  //----------------------------------------
  // Method:  countFails()
  // Purpose: Return the total fail count, the value of failCount
  // Usage:   To be called at the end of the test method and AFTER all tests
  // Inputs:  none
  // Outputs: 1
  //          int failCount
  //----------------------------------------
  int countFails( )
  {
    return( failCount );
  }

  //----------------------------------------
  // Method:  countTests()
  // Purpose: Return the total test count, the value of last subtestID
  // Usage:   To be called at the end of the test method and AFTER all tests
  // Inputs:  none
  // Outputs: 1
  //          int testCount
  //----------------------------------------
  int countTests( )
  {
    return( subtestID );
  }

private:

  // The following are all used as part of the output from TestUtil::print()
  // to facilitate filtering of output that is thus printed to stdout.
  std::string outputKeyword = "TestOutput"; // Identifies a stdout line as from this class.
  std::string sourceClass   = "Unknown";    // help locate the source class causing a test failure.
  std::string sourceMethod  = "Unknown";    // help locate the source method causing a test failure.
  std::string testFilename  = "Unknown";    // help locate the test file that discovered a failure.
  std::string testMethod    = "Unknown";    // help locate the test method that discovered a failure.
  int         subtestID     = 1;            // since single test methods may contain multiple subtests.
  int         failBit       = 0;            // store the result of a test (0=pass, 1=fail)

  // Other stuff
  double tolerance = 0;
  int failCount    = 0;
  // int testCount    = 0;
  ostringstream testOutputStream; // Printed out to screen, but why bother since
                                  //  we just use std::cout everywhere?
};
