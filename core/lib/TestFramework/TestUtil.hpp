#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include "build_config.h"

//============================================================
// class:   TestUtil
// purpose: TestUtil is a proposed utility class (not parent class)
//          for use with test classes and test methods in GPSTk.
// Example: Source usage for a test method with 4 sub-tests:
//
//     TestUtil myTest( "SomeClass", "SomeMethod", "myTestFile", __LINE__ );
//
//     myTest.assert( 1==2 );
//     myTest.assert( 1==1 );
//     myTest.changeSourceMethod("SomeOtherMethod");
//
// Output: stdout from above myTest.print() calls would be:
//
//     TestOutput, SomeClass, SomeMethod, myTestFile, 13, 1, 1
//
//============================================================
class TestUtil
{
public:

  //----------------------------------------
  // Method:  TestUtil::TestUtil()
  // Purpose: Constructor
  // Usage:   To be called at the start of each test method
  // Inputs:  sourceClassInput  = the name of the source class being tested
  //          sourceMethodInput = the name of the source method being tested
  //          testFileInput     = the name of file containing the test code, e.g., __FILE__
  //          testLineInput     = the line number in the file where testing is done, e.g. __LINE__
  //          verbosityInput    = the level of verbosity in the print output, default=1, but set to 0 will supress fail messages
  // Outputs: none
  //----------------------------------------
  TestUtil( const std::string& sourceClassInput  = "Unknown",
            const std::string& sourceMethodInput = "Unknown",
            const std::string& testFileInput     = "Unknown",
            const         int& testLineInput     = 0,
            const         int& verbosityInput    = 1
           ):
      outputKeyword( "GpstkTest" ),
      sourceClass( sourceClassInput  ),
      sourceMethod( sourceMethodInput ),
      testFileName( testFileInput ),
      testFileLine( "0" ),
      tolerance( 0 ),    
      testMessage( "Developer is a lazy slacker" ),
      failBit( 0 ),
      verbosity( verbosityInput ),
      testCount( 0 ),
      subtestID( 1 ),
      failCount( 0 )
  {
      // convert int to string
      setTestLine( testLineInput );

      // strip off the path from the full-path filename
      // so that "/home/user/test.txt" becomes "test.txt"
      std::string file_sep = gpstk::getFileSep();
      testFileName = testFileName.substr( testFileName.find_last_of( file_sep ) + 1 );
  }


  //----------------------------------------
  // Method:  getDataPath
  // Purpose: get file system path to test input and baseline output data
  // Usage:   std::string data_path = myTestUtil.getDataPath()
  // Inputs:  0
  // Outputs: std::string equal to path, with no trailing slash
  //----------------------------------------
  inline std::string getDataPath( void )
  {
      return( gpstk::getPathData() );
  }


  //----------------------------------------
  // Method:  getTempPath
  // Purpose: get file system path to location to write temp test output
  // Usage:   std::string temp_path = myTestUtil.getTempPath()
  // Inputs:  0
  // Outputs: std::string equal to path, with no trailing slash
  //----------------------------------------
  inline std::string getTempPath( void )
  {
      return( gpstk::getPathTestTemp() );
  }

  //----------------------------------------
  // Method:  TestUtil::assert()
  // Purpose: Takes a boolean expression, passes or fails the test, depending on
  //          whether the assertion is true or false, and then prints the result
  // Usage:   Use when a test can be written as boolean expression.
  // Inputs:  boolean testExpression
  // Outputs: none
  //----------------------------------------
  void assert( bool testExpression, const std::string& test_message, const int line_number )
  {
      setTestMessage( test_message );
      setTestLine( line_number );
      
      if( testExpression == false )
      {
          fail();
      }
      else
      {
          pass();
      }

      print();
      next();
  }


  void assert( bool testExpression, const std::string& test_message, const std::string& line_number )
  {
      setTestMessage( test_message );
      setTestLine( line_number );
      
      if( testExpression == false )
      {
          fail();
      }
      else
      {
          pass();
      }

      print();
      next();
  }


   template <class T>
   void assert_equals( const T& expected, const T& got,
                       int line_number,
                       const std::string& test_message = std::string() )
   {
      std::string mess;
      if (test_message.empty())
      {
         ostringstream ostr;
         ostr << "Expected:'" << expected << "'" << endl << " But got:'"
              << got << "'" << endl;
         mess = ostr.str();
      }
      assert(expected == got, mess, line_number);
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


  //----------------------------------------
  // Method:  getTolerance
  // Purpose: return numerical value of test tolerance
  //----------------------------------------
  inline double getTolerance( void )
  {
      return( tolerance );
  }


  //----------------------------------------
  // Method:  changeSourceMethod()
  // Purpose: changeSourceMethod allows for the change of the method,
  //          function, or feature of the source class under test
  //          in the test output stream.
  // Usage:   to be called as needed to change the sourceMethod string
  // Inputs:  1
  //          string newMethod
  // Outputs: none
  //----------------------------------------
  void changeSourceMethod( const std::string& newMethod )
  {
      sourceMethod = newMethod;
  }


  //----------------------------------------
  // Method:  setTestMessage()
  // Purpose: Set the message text that is reported when print() is called, usually a fail message
  // Inputs:  2
  //          std::string test_message, the text to be sent to a log, usually to describe what failed and why
  //          int         line_number, the line number in the test app where pass(), fail(), assert() was called
  // Outputs: none
  //----------------------------------------
  void setTestMessage( const std::string& test_message )
  {
      testMessage  = test_message;
  }

  void setTestMessage( const std::string& test_message, const int line_number )
  {
      setTestMessage( test_message );
      setTestLine( line_number );
  }

  void setTestMessage( const std::string& test_message, const std::string& line_number )
  {
      setTestMessage( test_message );
      setTestLine( line_number );
  }


  //----------------------------------------
  // Method:  setTestLine()
  // Purpose: Set the testFileLine
  // Inputs:  1
  //          int         line_number, the line number in the test app where fail() was called
  // Outputs: none
  //----------------------------------------
  void setTestLine( const int line_number_int )
  {
      std::ostringstream conversionStringStream;
      conversionStringStream << line_number_int;
      testFileLine = conversionStringStream.str();
  }

  void setTestLine( const std::string& line_number_string )
  {
      testFileLine = line_number_string;
  }


  //------------------------------------------------------------
  // Method:  fileEqualTest()
  // Purpose: compare two files for differences
  // Inputs:  Takes in two file names "FILEONE.TXT" "FILETWO.TXT".
  //          Take one integrer number of lines at the top of the two files to skip
  // Outputs: Returns boolean: true if the files are equal, false if not.
  //------------------------------------------------------------
  bool fileEqualTest( const std::string& file1_name, const std::string& file2_name, int num_lines_skip=0 )
  {
      int           line_number = 0;
      bool          files_equal = false;
      std::ifstream file1_stream;
      std::ifstream file2_stream;
      std::string   file1_line;
      std::string   file2_line;

      file1_stream.open( file1_name.c_str() );
      file2_stream.open( file2_name.c_str() );

      // Compare each line until you reach the end of File1
      while( !file1_stream.eof() )
      {
          line_number++;

          // If we reach the end of File2, but there is
          // more left in File1, then they are not equal
          if( file2_stream.eof() )
          {
              files_equal = false;
              return( files_equal );
          }

          // get the next line and compare
          getline( file1_stream, file1_line );
          getline( file2_stream, file2_line );

          // only fail if you find differences AFTER the skipped lines
          if( (line_number > num_lines_skip) && (file1_line != file2_line) )
          {
              files_equal = false;
              return( files_equal );
          }
      }

      // If we reach the end of File1, but there is
      // more left in File2, then they are not equal
      if( !file2_stream.eof() )
      {
          files_equal = false;
          return( files_equal );
      }
      else
      {
          files_equal = true;
          return( files_equal );
      }
  }

private:

  //============================================================
  // Private Data Members
  //============================================================

  // The following are all used as part of the output from TestUtil::print()
  // to facilitate filtering of output that is thus printed to stdout

  std::string outputKeyword; // Identifies a stdout line as a test record from this class
  std::string sourceClass;   // help locate source class causing a test failure
  std::string sourceMethod;  // help locate source method causing a test failure
  std::string testFileName;  // help locate test file that discovered a failure
  std::string testFileLine;  // help locate test line where the failure occured

  double      tolerance;     // acceptable difference between test output and
                             //  expected or baseline output

  std::string testMessage;   // if failBit==1 && verbosity>=1, print this string
                             // description of why the test failed to be set by the test app developer
    
  int         failBit;       // store the result of a test (0=pass, 1=fail)
  int         verbosity;     // if verbosity>=0, print summary line; if verbosity>=1, print testMessage when fail() is called.

  //  since single test methods may contain multiple subtests.

  int testCount; // Count of tests that have been run
  int subtestID; // ID of the current sub-test, used in TestUtil::print()
  int failCount; // Count of tests that have fails


  //============================================================
  // Private Methods 
  //============================================================

  //----------------------------------------
  // Method:  TestUtil::print()
  // Purpose: print test results and information on classes being tested
  //          to stdout in a common format that is both human-readable
  //          and easy to filter using tools like grep so as to help
  //          isolate where problems are happening.
  // Usage:   to be called after each test method subtest is performed
  // Inputs:  none
  // Outputs: none
  // STDOUT:  "outputKeyword, sourceClass, sourceMethod, testFileName,
  //             testMethod, subtestID, failBit"
  //----------------------------------------
  void print( void )
  {
      // print test summary description and result to stdout
      if( failBit==1 && verbosity >=1 )
      {
         std::cout     <<
         outputKeyword << ", " <<
         "Class="      << sourceClass   << ", " <<
         "Method="     << sourceMethod  << ", " <<
         "testFile="   << testFileName  << ", " <<
         "testLine="   << testFileLine  << ", " <<
         "subtest="    << subtestID     << ", " <<
         "failBit="    << failBit       << ", " <<
         "testMsg="    << testMessage
         << std::endl;     // implicit conversion from int to string

      }
      else
      {
         std::cout     <<
         outputKeyword << ", " <<
         "Class="      << sourceClass   << ", " <<
         "Method="     << sourceMethod  << ", " <<
         "testFile="   << testFileName  << ", " <<
         "testLine="   << testFileLine  << ", " <<
         "subtest="    << subtestID     << ", " <<
         "failBit="    << failBit
         << std::endl;     // implicit conversion from int to string

      }
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
  // Method:  TestUtil::fail()
  // Purpose: Fail the test! Record a failure by setting the failBit and
  //           incrementing failCount
  // Usage:   To be called (once!) at the end of any subtest that has failed
  // Inputs:  2 [optional]
  //          string fail_message
  //          int    line_number
  // Outputs: none
  //----------------------------------------
  void fail( void )
  {
      failBit = 1;
      failCount++;
      testCount++;
  }

  void fail( const std::string& fail_message )
  {
      setTestMessage( fail_message );
      fail();
  }

  void fail( const std::string& fail_message, const int line_number )
  {
      setTestMessage( fail_message );
      setTestLine( line_number );
      fail();
  }

  void fail( const std::string& fail_message, const std::string& line_number )
  {
      setTestMessage( fail_message );
      setTestLine( line_number );
      fail();
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
      // increment subtest counter/ID
      subtestID = countTests() + 1;

      // reset fail parameters for next/new subtest
      failBit = 0;
      testMessage = "Developer is a lazy slacker";
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


};
