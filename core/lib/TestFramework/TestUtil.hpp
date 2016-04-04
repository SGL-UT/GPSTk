//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2015, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
// This software developed by Applied Research Laboratories at the
// University of Texas at Austin, under contract to an agency or
// agencies within the U.S.  Department of Defense. The
// U.S. Government retains all rights to use, duplicate, distribute,
// disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

#include <typeinfo>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <vector>
#include <float.h>
#include "build_config.h"
#include "StringUtils.hpp"
#include "Matrix.hpp"

// Define a TestUtil object named testFramework
#define TUDEF(CLASS,METHOD) gpstk::TestUtil testFramework(CLASS, METHOD, __FILE__, __LINE__)
// Macro to make test code look nice...
#define TUCSM(METHOD) testFramework.changeSourceMethod(METHOD)
// Macro to be short form of TestUtil::assert()
#define TUASSERT(EXPR) testFramework.assert(EXPR, "Assertion failure: "#EXPR, __LINE__)
// Basic macro for doing equality tests.  Expects a TestUtil instance
// named testFramework.
#define TUASSERTE(TYPE,EXP,GOT) testFramework.assert_equals<TYPE>(EXP,GOT,__LINE__)
// Macro for doing equality tests of double/float values.  Expects a
// TestUtil instance named testFramework.
#define TUASSERTFE(EXP,GOT) testFramework.assert_equals(EXP,GOT,__LINE__)
// Macro for doing equality tests of double/float values with a
// specified epsilon.  Expects a TestUtil instance named
// testFramework.
#define TUASSERTFEPS(EXP,GOT,EPS) testFramework.assert_equals(EXP,GOT,__LINE__,"", EPS)
// Macro for doing comparisons of test files
#define TUCMPFILE(F1,F2,SKIP) testFramework.assert_files_equal(__LINE__, F1, F2, "File mismatch: "+F1+" "+F2, SKIP)
// Fail the test with a message.
#define TUFAIL(MSG) testFramework.assert(false, MSG, __LINE__)
// Pass the test with a (unprinted) message.
#define TUPASS(MSG) testFramework.assert(true, MSG, __LINE__)
// Usual return from a test function
#define TURETURN() return testFramework.countFails()

namespace gpstk
{
      /// @return a string with the name of the type
   template<typename T>
   std::string typeString()
   {
      if (typeid(T) == typeid(long double))
         return std::string("long double");
      else if (typeid(T) == typeid(double))
         return std::string("double");
      else if (typeid(T) == typeid(float))
         return std::string("float");
      else
         return 0;
   }

      //============================================================
      // class:   TestUtil
      // purpose: TestUtil is a utility class (not parent class)
      //          for use with test classes and test methods in GPSTk.
      // Example: Source usage for a test method with 4 sub-tests:
      //
      //     TUDEF("SomeClass", "SomeMethod");
      //
      //     TUASSERTE(unsigned, 1, 2);
      //        which is equivalent to
      //     testFramework.assert( 1==2 );
      //     TUASSERTE(unsigned, 1, 1);
      //     testFramework.changeSourceMethod("SomeOtherMethod");
      //
      //============================================================
   class TestUtil
   {
   public:
         /** Constructor to be called at the start of each test method.
          * @param[in] sourceClassInput the name of the source class
          *   being tested
          * @param[in] sourceMethodInput the name of the source method
          *   being tested
          * @param[in] testFileInput the name of file containing the test
          *   code, e.g., __FILE__
          * @param[in] testLineInput the line number in the file where
          *   testing is done, e.g. __LINE__
          * @param[in] verbosityInput the level of verbosity in the print
          *   output, default=1, but set to 0 will supress fail messages
          */
      TestUtil( const std::string& sourceClassInput  = "Unknown",
                const std::string& sourceMethodInput = "Unknown",
                const std::string& testFileInput     = "Unknown",
                const         int& testLineInput     = 0,
                const         int& verbosityInput    = 1
                );

         /** Takes a boolean expression, passes or fails the test,
          * depending on whether the assertion is true or false, and then
          * prints the result.
          * @param[in] testExpression Boolean value that is expected
          *   to be true.
          * @param[in] testMsg A message to be printed on failure.
          * @param[in] lineNumber The line of source in the test file
          *   where this assert is being performed, typically __LINE__.
          */
      void assert( bool testExpression,
                   const std::string& testMsg,
                   const int lineNumber );

         /** Takes two values of the same type, compares them and passes
          * the test if the values are equal.
          * @param[in] expected The expected value to be compared against.
          * @param[in] got The value produced by the method under test.
          * @param[in] lineNumber The line of source in the test file
          *   where this assert is being performed, typically __LINE__.
          * @param[in] testMsg A message to be printed on failure.
          *   A default message will simply say what was expected and
          *   what the value actually was when expected != got.
          */
      template <class T>
      void assert_equals( const T& expected,
                          const T& got,
                          int lineNumber,
                          const std::string& testMsg = std::string());

         /** Takes two floating point values, compares them and passes the test
          * if the values are equal within an epsilon.
          * @param[in] expected The expected value to be compared against.
          * @param[in] got The value produced by the method under test.
          * @param[in] lineNumber The line of source in the test file
          *   where this assert is being performed, typically __LINE__.
          * @param[in] testMsg A message to be printed on failure.
          *   A default message will simply say what was expected and
          *   what the value actually was when expected != got.
          * @param[in] epsilon The maximum difference between expected
          *   and got that will be considered "equal". If this number is 
          *   less than zero, the type's epsilon is used.
          */
      void assert_equals( double expected,
                          double got,
                          int lineNumber,
                          const std::string& testMsg = std::string(),
                          double epsilon=-1)
      {assert_equals_fp<double>(expected, got, lineNumber, testMsg, epsilon);}
   
      void assert_equals( long double expected,
                          long double got,
                          int lineNumber,
                          const std::string& testMsg = std::string(),
                          long double epsilon=-1)
      {assert_equals_fp<double>(expected, got, lineNumber, testMsg, epsilon);}
   
      void assert_equals( float expected,
                          float got,
                          int lineNumber,
                          const std::string& testMsg = std::string(),
                          float epsilon=-1)
      {assert_equals_fp<double>(expected, got, lineNumber, testMsg, epsilon);}
   
      template <typename T>
      void assert_equals_fp( const T& expected,
                             const T& got,
                             int lineNumber,
                             const std::string& testMsg = std::string(),
                             T epsilon = -1);
   

         /** Takes two matricies, compares them and passes the test
          * if the values are equal within an epsilon.
          * @param[in] expected The expected value to be compared against.
          * @param[in] got The value produced by the method under test.
          * @param[in] lineNumber The line of source in the test file
          *   where this assert is being performed, typically __LINE__.
          * @param[in] testMsg A message to be printed on failure.
          *   A default message will simply say what was expected and
          *   what the value actually was when expected != got.
          * @param[in] epsilon The maximum difference between expected
          *   and got that will be considered "equal". If this number is 
          *   less than zero, the type's epsilon is used.
          */
      template<typename T>
      void assert_equals( const gpstk::Matrix<T>& expected,
                          const gpstk::Matrix<T>& got,
                          int lineNumber,
                          const std::string& testMsg = std::string(),
                          T epsilon = -1);
      template<typename T>
      void assert_equals( const gpstk::Vector<T>& expected,
                          const gpstk::Vector<T>& got,
                          int lineNumber,
                          const std::string& testMsg = std::string(),
                          T epsilon = -1);
   
         /** Compare two text files, line-by-line.  Test passes if there
          * are no differences according to the rules set by parameters.
          * @param[in] lineNumber The line of source in the test file
          *   where this assert is being performed, typically __LINE__.
          * @param[in] file1Name The full path to the reference file
          *   being compared against.
          * @param[in] file2Name The full path to the test output file to
          *   compare to the reference.
          * @param[in] testMsg A message to be printed on failure.
          * @param[in] numLinesSkip The number of lines to ignore in the
          *   two files, starting from the beginning.
          * @param[in] ignoreLeadingSpaces If true, changes are ignored
          *   between the two files in white space at the beginning of a
          *   line.
          * @param[in] ignoreTrailingSpaces If true, changes are ignored
          *   between the two files in white space at the end of a
          *   line.
          * @param[in] ignoreRegex An optional vector of regular
          *   expression strings that, if matched in the SOURCE FILE
          *   (i.e. file1name), differences on that line will be ignored.
          */
      void assert_files_equal( int lineNumber,
                               const std::string& file1Name,
                               const std::string& file2Name,
                               const std::string& testMsg,
                               int numLinesSkip=0,
                               bool ignoreLeadingSpaces = false,
                               bool ignoreTrailingSpaces = false,
                               std::vector<std::string> ignoreRegex = std::vector<std::string>(0) );

         /** Compare two binary files, byte-by-byte.  Test passes if there
          * are no differences in the data.
          * @param[in] lineNumber The line of source in the test file
          *   where this assert is being performed, typically __LINE__.
          * @param[in] file1Name The full path to the reference file
          *   being compared against.
          * @param[in] file2Name The full path to the test output file to
          *   compare to the reference.
          * @param[in] testMsg A message to be printed on failure.
          * @param[in] from First byte to compare.
          * @param[in] to Last byte to compare.
          */
      void assert_binary_files_equal( int lineNumber,
                                      const std::string& file1Name,
                                      const std::string& file2Name,
                                      const std::string& testMsg,
                                      unsigned long long from = 0,
                                      unsigned long long to = -1);

         /// @return the number of tests that have failed so far.
      int countFails( void );

         /// @return the number of tests that have been executed so far.
      int countTests( void );

         /** Change the method, function, or feature of the source class
          * under test in the test output stream.
          * @param[in] newMethod the name of the method under test */
      void changeSourceMethod( const std::string& newMethod );

         /** Set the message text that is reported when print() is
          * called, usually a fail message.
          * @param[in] testMsg the text to be sent to a log, usually to
          *   describe what failed and why. */
      void setTestMessage( const std::string& testMsg );

         /** Set the message text that is reported when print() is
          * called, usually a fail message.
          * @param[in] testMsg The text to be sent to a log, usually to
          *   describe what failed and why.
          * @param[in] lineNumber The line number in the test app where
          *   pass(), fail(), assert() was called */
      void setTestMessage( const std::string& testMsg, const int lineNumber );

      void setTestLine( const int lineNumber );

         /** Compare two text files for differences, line by line.
          * @param[in] refFile The reference file to compare against.
          * @param[in] checkFile The generated file being compared.
          * @param[in] numLinesSkip Number of lines to ignore at the
          *   start of each file.
          * @param[in] ignoreLeadingSpaces If true, ignore any changes in
          *   whitespace at the beginning of each line.
          * @param[in] ignoreTrailingSpaces If true, ignore any changes in
          *   whitespace at the end of each line.
          * @param[in] ignoreRegex A vector of POSIX regular expressions
          *   that, if matched in refFile, differences in those lines
          *   will be ignored.  Regular expressions are not matched
          *   against checkFile.
          * @return true if the files are equal, false if not.
          */
      bool fileEqualTest( const std::string& refFile,
                          const std::string& checkFile,
                          int numLinesSkip=0,
                          bool ignoreLeadingSpaces = false,
                          bool ignoreTrailingSpaces = false,
                          std::vector<std::string> ignoreRegex = std::vector<std::string>(0) );

         /** Compare two binary files for differences, byte by byte.
          * @param[in] refFile The reference file to compare against.
          * @param[in] checkFile The generated file being compared.
          * @param[in] from First byte to compare.
          * @param[in] to Last byte to compare. */
      bool fileCompTest( const std::string& refFile,
                         const std::string& checkFile,
                         unsigned long long from = 0,
                         unsigned long long to = -1);

   private:

         // The following are all used as part of the output from
         // TestUtil::print() to facilitate filtering of output that is
         // thus printed to stdout

         /// Identifies a stdout line as a test record from this class
      std::string outputKeyword;
         /// help locate source class causing a test failure
      std::string sourceClass;
         /// help locate source method causing a test failure
      std::string sourceMethod;
         /// help locate test file that discovered a failure
      std::string testFileName;
         /// help locate test line where the failure occured
      std::string testFileLine;

         /** if failBit==1 && verbosity>=1, print this string description
          * of why the test failed to be set by the test app developer */
      std::string testMessage;
    
         /// store the result of a test (0=pass, 1=fail)
      int failBit;
         /** if verbosity>=0, print summary line; if verbosity>=1, print
          * testMessage when fail() is called. */
      int verbosity;

         //  since single test methods may contain multiple subtests.

      int testCount; ///< Count of tests that have been run
      int subtestID; ///< ID of the current sub-test, used in TestUtil::print()
      int failCount; ///< Count of tests that have failed



         /** Print test results and information on classes being tested
          * to stdout in a common format that is both human-readable and
          * easy to filter using tools like grep so as to help isolate
          * where problems are happening. */
      void print( void );

         /** Pass the test! Record a pass by setting the failBit=0 and
          * incrementing the testCount */
      void pass( void );

         /** Fail the test! Record a failure by setting the failBit and
          * incrementing failCount. */
      void fail( void );

         /** Fail the test! Record a failure by setting the failBit and
          * incrementing failCount. */
      void fail( const std::string& failMsg );

         /** Fail the test! Record a failure by setting the failBit and
          * incrementing failCount. */
      void fail( const std::string& failMsg, const int lineNumber );

         /** Increment the failCount and reset subtestID based on current
          * testCount. */
      void next( void );

         /** Undo the test! Undo a pass/fail by unsetting failBit and
          * decrementing failCount (only if failed) and decrementing the
          * testCount. */
      void undo( void );
   }; // class TestUtil



   TestUtil ::
   TestUtil( const std::string& sourceClassInput,
             const std::string& sourceMethodInput,
             const std::string& testFileInput,
             const         int& testLineInput,
             const         int& verbosityInput )
      : outputKeyword( "GPSTkTest" ),
        sourceClass( sourceClassInput  ),
        sourceMethod( sourceMethodInput ),
        testFileName( testFileInput ),
        testFileLine( "0" ),
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
      testFileName = testFileName.substr(
         testFileName.find_last_of( file_sep ) + 1 );
   }


   void TestUtil ::
   assert( bool testExpression,
           const std::string& testMsg,
           const int lineNumber )
   {
      setTestMessage( testMsg );
      setTestLine( lineNumber );
      
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
   void TestUtil ::
   assert_equals( const T& expected,
                  const T& got,
                  int lineNumber,
                  const std::string& testMsg)
   {
      std::string mess(testMsg);
      if (testMsg.empty())
      {
         std::ostringstream ostr;
         ostr << "Expected:'" << expected << "' ,But got:'" << got << "'";
         mess = ostr.str();
      }
      assert(expected == got, mess, lineNumber);
   }


   template<typename T>
   void TestUtil ::
   assert_equals_fp( const T& expected,
                     const T& got,
                     int lineNumber,
                     const std::string& testMsg,
                     T epsilon )
   {
      T err = std::abs(expected - got);
      if (epsilon < 0)
         epsilon = std::numeric_limits<T>::epsilon();

      bool good = err < epsilon;
      std::string mess(testMsg);
      if (testMsg.empty())
      {
         std::ostringstream ostr;
         ostr << "abs(" << expected << " - " << got << ") = " << err;
         if (good)
            ostr << " <= ";
         else
            ostr << " > ";
         ostr << epsilon;
         mess = ostr.str();
      }      
      assert(good, mess, lineNumber);
   }


   template<typename T>
   void TestUtil ::
   assert_equals( const gpstk::Matrix<T>& expected,
                  const gpstk::Matrix<T>& got,
                  int lineNumber,
                  const std::string& testMsg,
                  T epsilon )
   {
      if (epsilon < 0)
         epsilon = std::numeric_limits<T>::epsilon();
   
      std::string mess(testMsg);
      T mag = maxabs(expected - got);
      if (testMsg.empty())
      {
         std::ostringstream ostr;
         ostr << "maxabs(expected-computed) = " << mag;
         mess = ostr.str();
      }
      assert_equals(T(mag) , T(0.0), lineNumber, mess, epsilon);
   }


   template<typename T>
   void TestUtil ::
   assert_equals( const gpstk::Vector<T>& expected,
                  const gpstk::Vector<T>& got,
                  int lineNumber,
                  const std::string& testMsg,
                  T epsilon )
   {
      if (epsilon < 0)
         epsilon = std::numeric_limits<T>::epsilon();
      std::string mess(testMsg);
      T mag = maxabs(expected - got);
      if (testMsg.empty())
      {
         std::ostringstream ostr;
         ostr << "absmag(expected-computed) = " << mag;
         mess = ostr.str();
      }
      assert_equals(T(mag) , T(0.0), lineNumber, mess, epsilon);
   }

   void TestUtil ::
   assert_files_equal( int lineNumber,
                       const std::string& file1Name,
                       const std::string& file2Name,
                       const std::string& testMsg,
                       int numLinesSkip,
                       bool ignoreLeadingSpaces,
                       bool ignoreTrailingSpaces,
                       std::vector<std::string> ignoreRegex )
   {
      bool eq = fileEqualTest(
         file1Name, file2Name, numLinesSkip, ignoreLeadingSpaces,
         ignoreTrailingSpaces, ignoreRegex );
      assert(eq, testMsg, lineNumber);
   }


   void TestUtil ::
   assert_binary_files_equal( int lineNumber,
                              const std::string& file1Name,
                              const std::string& file2Name,
                              const std::string& testMsg,
                              unsigned long long from,
                              unsigned long long to)
   {
      bool eq = fileCompTest(file1Name, file2Name, from, to);
      assert(eq, testMsg, lineNumber);
   }


   int TestUtil ::
   countFails( void )
   {
      return( failCount );
   }


   int TestUtil ::
   countTests( void )
   {
      return( testCount );
   }


   void TestUtil ::
   changeSourceMethod( const std::string& newMethod )
   {
      sourceMethod = newMethod;
   }


   void TestUtil ::
   setTestMessage( const std::string& testMsg )
   {
      testMessage  = testMsg;
   }

   void TestUtil ::
   setTestMessage( const std::string& testMsg, const int lineNumber )
   {
      setTestMessage( testMsg );
      setTestLine( lineNumber );
   }


   void TestUtil ::
   setTestLine( const int lineNumber )
   {
      std::ostringstream conversionStringStream;
      conversionStringStream << lineNumber;
      testFileLine = conversionStringStream.str();
   }


   bool TestUtil ::
   fileEqualTest( const std::string& refFile,
                  const std::string& checkFile,
                  int numLinesSkip,
                  bool ignoreLeadingSpaces,
                  bool ignoreTrailingSpaces,
                  std::vector<std::string> ignoreRegex )
   {
      int           lineNumber = 0;
      bool          filesEqual = false;
      std::ifstream refStream;
      std::ifstream checkStream;
      std::string   refLine;
      std::string   checkLine;
   
      refStream.open( refFile.c_str() );
      checkStream.open( checkFile.c_str() );
   
         // Compare each line until you reach the end of Ref
      while( !refStream.eof() )
      {
         lineNumber++;
      
            // If we reach the end of Check, but there is
            // more left in Ref, then they are not equal
         if( checkStream.eof() )
         {
            filesEqual = false;
            return( filesEqual );
         }
      
            // get the next line and compare
         getline( refStream, refLine );
         getline( checkStream, checkLine );
      
         if (lineNumber <= numLinesSkip)
            continue;
      
         if (ignoreLeadingSpaces)
         {
            std::size_t idx = refLine.find_first_not_of(" \t\r\n\f\v");
            if (idx != std::string::npos)
               refLine.erase(0,idx-1);
            idx = checkLine.find_first_not_of(" \t\r\n\f\v");
            if (idx != std::string::npos)
               checkLine.erase(0,idx-1);
         }
         if (ignoreTrailingSpaces)
         {
            std::size_t idx = refLine.find_last_not_of(" \t\r\n\f\v");
            if (idx != std::string::npos)
               refLine.erase(idx+1);
            else
               refLine.clear(); // all whitespace
            idx = checkLine.find_last_not_of(" \t\r\n\f\v");
            if (idx != std::string::npos)
               checkLine.erase(idx+1);
            else
               checkLine.clear(); // all whitespace
         }
         if (!ignoreRegex.empty())
         {
               // check for regular expressions
               // Use a flag because break/continue in C++ doesn't
               // allow you to skip multiple levels.
            bool ignore = false;
            for (int i = 0; i < ignoreRegex.size(); i++)
            {
               if (gpstk::StringUtils::isLike(refLine, ignoreRegex[i]))
               {
                  ignore = true;
                  break;
               }
            }
            if (ignore)
               continue;
         }
      
            // only fail if you find differences AFTER the skipped lines
         if (refLine != checkLine)
         {
            filesEqual = false;
            return( filesEqual );
         }
      }
   
         // If we reach the end of Ref, but there is
         // more left in Check, then they are not equal
      if( !checkStream.eof() )
      {
         filesEqual = false;
         if (verbosity>1)
            std::cout << "refLine:" << refLine << std::endl
                      << "checkLine:" << checkLine << std::endl;
         return( filesEqual );
      }
      else
      {
         filesEqual = true;
         return( filesEqual );
      }
   }


   bool TestUtil ::
   fileCompTest( const std::string& refFile,
                 const std::string& checkFile,
                 unsigned long long from,
                 unsigned long long to)
   {
      static const unsigned bufsize = 4096;
      unsigned readsize = bufsize;
      bool done = false;
      std::vector<char> refBuf(bufsize, 0), checkBuf(bufsize, 0);
      std::ifstream ref(refFile.c_str()), check(checkFile.c_str());
      if (!ref || !check)
         return false; // missing or inaccessible file
         // get the file sizes
      unsigned long long refSize, checkSize, curPos;
      ref.seekg(0, std::ios_base::end);
      refSize = ref.tellg();
      check.seekg(0, std::ios_base::end);
      checkSize = check.tellg();
      if (refSize != checkSize)
         return false; // files not the same size
         // set our limit to the smaller of the file size and "to"
      to = std::min(to, refSize);
      if (!ref.seekg(from, std::ios_base::beg))
         return false; // seek failure, usually file too short
      if (!check.seekg(from, std::ios_base::beg))
         return false; // seek failure, usually file too short

      while (!done)
      {
         curPos = ref.tellg();
            // stop where requested
         if ((curPos + readsize) > to)
         {
            readsize = 1+(to - curPos);
            done = true;
         }
         ref.read(&refBuf[0], readsize);
         check.read(&checkBuf[0], readsize);
         if (refBuf != checkBuf)
            return false;
      }
      return true;
   }


   void TestUtil ::
   print( void )
   {
         // print test summary description and result to stdout
      if( failBit==1 && verbosity >=1 )
      {
         std::cout
            << outputKeyword << ", "
            << "Class="      << sourceClass   << ", "
            << "Method="     << sourceMethod  << ", "
            << "testFile="   << testFileName  << ", "
            << "testLine="   << testFileLine  << ", "
            << "subtest="    << subtestID     << ", "
            << "failBit="    << failBit       << ", "
            << "testMsg="    << testMessage
            << std::endl;     // implicit conversion from int to string
      }
      else
      {
         std::cout
            << outputKeyword << ", "
            << "Class="      << sourceClass   << ", "
            << "Method="     << sourceMethod  << ", "
            << "testFile="   << testFileName  << ", "
            << "testLine="   << testFileLine  << ", "
            << "subtest="    << subtestID     << ", "
            << "failBit="    << failBit
            << std::endl;     // implicit conversion from int to string
      }
   }


   void TestUtil ::
   pass( void )
   {
      failBit = 0;
      testCount++;
   }


   void TestUtil ::
   fail( void )
   {
      failBit = 1;
      failCount++;
      testCount++;
   }


   void TestUtil ::
   fail( const std::string& failMsg )
   {
      setTestMessage( failMsg );
      fail();
   }


   void TestUtil ::
   fail( const std::string& failMsg, const int lineNumber )
   {
      setTestMessage( failMsg );
      setTestLine( lineNumber );
      fail();
   }


   void TestUtil ::
   next( void )
   {
         // increment subtest counter/ID
      subtestID = countTests() + 1;
   
         // reset fail parameters for next/new subtest
      failBit = 0;
      testMessage = "Developer is a lazy slacker";
   }


   void TestUtil ::
   undo( void )
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
}
