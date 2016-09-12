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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S.
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software.
//
//Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================



#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavFilterOperators.hpp"

#include "StringUtils.hpp"
#include "Exception.hpp"
#include "RinexEphemerisStore.hpp"

#include "build_config.h"

#include "TestUtil.hpp"
#include <string>
#include <iostream>

using namespace gpstk;

//=============================================================================
// Class declarations
//=============================================================================

class Rinex3Nav_T
{

public:

      // constructor
   Rinex3Nav_T()
   {
      init();
   }
      // destructor
   ~Rinex3Nav_T( ) {}

   void init( void );

   void toRinex3(void);

      // return values indicate number of failures, i.e., 0=PASS, 0!=FAIL
   int hardCodeTest( void );
   int headerExceptionTest( void );
   int streamReadWriteTest( void );
   int filterOperatorsTest( void );

   void toConversionTest(void);
   int version2ToVersion3Test( void );
   int version3ToVersion2Test( void );

private:

   std::string dataFilePath;
   std::string tempFilePath;
   std::string file_sep;

   std::string inputRinexNavExample;

   std::string outputTestOutput;
   std::string outputTestOutput2;
   std::string outputTestOutput3;
   std::string outputRinexDump;

   std::string inputInvalidLineLength;
   std::string inputNotaNavFile;
   std::string inputUnknownHeaderLabel;
   std::string inputIncompleteHeader;
   std::string inputUnsupportedRinex;
   std::string inputBadHeader;
   std::string outputTestOutputHeader;

   std::string inputFilterStream1;
   std::string inputFilterStream2;
   std::string inputFilterStream3;
   std::string outputFilterOutput;

   std::string outputRinexStore;

   std::stringstream failDescriptionStream;
   std::string       failDescriptionString;

   std::string inputRinex3Nav;
   std::string inputRinex2Nav;
   std::string outputRinex3Nav;
   std::string outputRinex2Nav;

   bool fileCompare;
};

//============================================================
// Initialize Test Data Filenames
//============================================================

void Rinex3Nav_T :: init( void )
{

   TestUtil test0;
   dataFilePath = gpstk::getPathData();
   tempFilePath = gpstk::getPathTestTemp();

      //----------------------------------------
      // Full file paths
      //----------------------------------------
   file_sep = getFileSep();

   inputRinexNavExample     = dataFilePath + file_sep +
                              "test_input_rinex2_nav_RinexNavExample.99n";

   outputTestOutput         = tempFilePath + file_sep +
                              "test_output_rinex2_nav_TestOutput.99n";
   outputTestOutput2        = tempFilePath + file_sep +
                              "test_output_rinex2_nav_TestOutput2.99n";
   outputTestOutput3        = tempFilePath + file_sep +
                              "test_output_rinex2_nav_TestOutput3.99n";
   outputRinexDump          = tempFilePath + file_sep +
                              "test_output_rinex2_nav_RinexDump.txt";

   inputInvalidLineLength   = dataFilePath + file_sep +
                              "test_input_rinex2_nav_InvalidLineLength.99n";
   inputNotaNavFile         = dataFilePath + file_sep +
                              "test_input_rinex2_nav_NotaNavFile.99n";
   inputUnknownHeaderLabel  = dataFilePath + file_sep +
                              "test_input_rinex2_nav_UnknownHeaderLabel.99n";
   inputIncompleteHeader    = dataFilePath + file_sep +
                              "test_input_rinex2_nav_IncompleteHeader.99n";
   inputUnsupportedRinex    = dataFilePath + file_sep +
                              "test_input_rinex2_nav_UnsupportedRinex.99n";
   inputBadHeader           = dataFilePath + file_sep +
                              "test_input_rinex2_nav_BadHeader.99n";
   outputTestOutputHeader   = tempFilePath + file_sep +
                              "test_output_rinex2_nav_TestOutputHeader.99n";

   inputFilterStream1       = dataFilePath + file_sep +
                              "test_input_rinex2_nav_FilterTest1.99n";
   inputFilterStream2       = dataFilePath + file_sep +
                              "test_input_rinex2_nav_FilterTest2.99n";
   inputFilterStream3       = dataFilePath + file_sep +
                              "test_input_rinex2_nav_FilterTest3.99n";
   outputFilterOutput       = tempFilePath + file_sep +
                              "test_output_rinex2_nav_FilterOutput.txt";

   outputRinexStore         = tempFilePath + file_sep +
                              "test_output_rinex2_nav_RinexStore.txt";

}

//============================================================
// Change input and output file names for Rinex v.3 types
//============================================================

void Rinex3Nav_T :: toRinex3(void)
{

   std::cout<<"Running tests for Rinex version 3.0"<<std::endl;

      //----------------------------------------
      // Full file paths
      //----------------------------------------

   inputRinexNavExample     = dataFilePath + file_sep +
                              "test_input_rinex3_nav_RinexNavExample.15n";

   outputTestOutput         = tempFilePath + file_sep +
                              "test_output_rinex3_nav_TestOutput.15n";
   outputTestOutput2        = tempFilePath + file_sep +
                              "test_output_rinex3_nav_TestOutput2.15n";
   outputTestOutput3        = tempFilePath + file_sep +
                              "test_output_rinex3_nav_TestOutput3.15n";
   outputRinexDump          = tempFilePath + file_sep +
                              "test_output_rinex3_nav_RinexDump.txt";

   inputInvalidLineLength   = dataFilePath + file_sep +
                              "test_input_rinex3_nav_InvalidLineLength.15n";
   inputNotaNavFile         = dataFilePath + file_sep +
                              "test_input_rinex3_nav_NotaNavFile.15n";
   inputUnknownHeaderLabel  = dataFilePath + file_sep +
                              "test_input_rinex3_nav_UnknownHeaderLabel.15n";
   inputIncompleteHeader    = dataFilePath + file_sep +
                              "test_input_rinex3_nav_IncompleteHeader.15n";
   inputUnsupportedRinex    = dataFilePath + file_sep +
                              "test_input_rinex3_nav_UnsupportedRinex.15n";
   inputBadHeader           = dataFilePath + file_sep +
                              "test_input_rinex3_nav_BadHeader.15n";
   outputTestOutputHeader   = tempFilePath + file_sep +
                              "test_output_rinex3_nav_TestOutputHeader.15n";

   inputFilterStream1       = dataFilePath + file_sep +
                              "test_input_rinex3_nav_FilterTest1.15n";
   inputFilterStream2       = dataFilePath + file_sep +
                              "test_input_rinex3_nav_FilterTest2.15n";
   inputFilterStream3       = dataFilePath + file_sep +
                              "test_input_rinex3_nav_FilterTest3.15n";
   outputFilterOutput       = tempFilePath + file_sep +
                              "test_output_rinex3_nav_FilterOutput.txt";

   outputRinexStore         = tempFilePath + file_sep +
                              "test_output_rinex3_nav_RinexStore.txt";

}

//=============================================================
// Change input and output file names for Rinex Conversion test
//=============================================================

void Rinex3Nav_T :: toConversionTest(void)
{
   inputRinex3Nav  = dataFilePath + file_sep +
                     "test_input_rinex3_nav_RinexNavExample.15n";
   inputRinex2Nav  = dataFilePath + file_sep +
                     "test_input_rinex2_nav_Rinex3NavFile.15n";

   outputRinex3Nav = tempFilePath + file_sep +
                     "test_output_rinex3_nav_Rinex2to3Output.15n";
   outputRinex2Nav = tempFilePath + file_sep +
                     "test_output_rinex2_nav_Rinex3to2Output.15n";
}

//=============================================================================
// Test Method Definitions
//=============================================================================


//------------------------------------------------------------
// This test checks to make sure that the internal members of
// the Rinex3NavHeader are as we think they should be.
// Also at the end of this test, we check and make sure our
// output file is equal to our input
// This assures that if any changes happen, the test will fail
// and the user will know.
// Also, output was put into input three times over to make sure
// there were no small errors which blow up into big errors
//------------------------------------------------------------
int Rinex3Nav_T :: hardCodeTest( void )
{
   double CompareVersion;
   std::string CompareFileProgram;
   std::string CompareFileAgency;
   std::string CompareDate;
   std::string CompareComment;

   TestUtil test1( "Rinex3NavStream", "out", __FILE__, __LINE__ );

   try
   {
      gpstk::Rinex3NavStream Rinex3NavStream( inputRinexNavExample.c_str() );
      gpstk::Rinex3NavStream out( outputTestOutput.c_str(), std::ios::out );
      gpstk::Rinex3NavStream dmp( outputRinexDump.c_str(), std::ios::out );
      gpstk::Rinex3NavHeader Rinex3NavHeader;
      gpstk::Rinex3NavData Rinex3NavData;

      Rinex3NavStream >> Rinex3NavHeader;
      out << Rinex3NavHeader;

      while( Rinex3NavStream >> Rinex3NavData )
      {
         out << Rinex3NavData;
      }

      if (Rinex3NavHeader.version == 2.1)
      {
         CompareVersion = 2.1;
         CompareFileProgram = (std::string)"XXRINEXN V3";
         CompareFileAgency = (std::string)"AIUB";
         CompareDate = (std::string)"1999-09-02 19:22:36";
         CompareComment = (std::string)"THIS IS ONE COMMENT";
      }

      else if (Rinex3NavHeader.version == 3.02)
      {
         CompareVersion = 3.02;
         CompareFileProgram = (std::string)"cnvtToRINEX 2.25.0";
         CompareFileAgency = (std::string)"convertToRINEX OPR";
         CompareDate = (std::string)"23-Jan-15 22:34 UTC";
         CompareComment = (std::string)
                          "-----------------------------------------------------------";
      }

      test1.assert( Rinex3NavHeader.version == CompareVersion,
                    "RinexNav Header version comparison",      __LINE__ );
      test1.assert( Rinex3NavHeader.fileProgram == CompareFileProgram,
                    "RinexNav Header file program comparison", __LINE__ );
      test1.assert( Rinex3NavHeader.fileAgency == CompareFileAgency,
                    "RinexNav Header file agency comparison",  __LINE__ );
      test1.assert( Rinex3NavHeader.date == CompareDate,
                    "RinexNav Header date comparison",         __LINE__ );

      std::vector<std::string>::const_iterator itr1 =
         Rinex3NavHeader.commentList.begin();
      test1.assert( (*itr1) == CompareComment, "RinexNav Header Comment comparison",
                    __LINE__ );


      test1.assert( test1.fileEqualTest( inputRinexNavExample, outputTestOutput, 2 ),
                    "RinexNav file read and then write, the two should be equal", __LINE__  );

         //------------------------------------------------------------
      gpstk::Rinex3NavStream Rinex3NavStream2( outputTestOutput.c_str() );
      gpstk::Rinex3NavStream out2( outputTestOutput2.c_str(), std::ios::out );
      gpstk::Rinex3NavHeader Rinex3NavHeader2;
      gpstk::Rinex3NavData Rinex3NavData2;

      Rinex3NavStream2 >> Rinex3NavHeader2;
      out2 << Rinex3NavHeader2;

      while (Rinex3NavStream2 >> Rinex3NavData2)
      {
         out2 << Rinex3NavData2;
      }

      gpstk::Rinex3NavStream Rinex3NavStream3( outputTestOutput2.c_str() );
      gpstk::Rinex3NavStream out3( outputTestOutput3.c_str() , std::ios::out );
      gpstk::Rinex3NavHeader Rinex3NavHeader3;
      gpstk::Rinex3NavData Rinex3NavData3;

      Rinex3NavStream3 >> Rinex3NavHeader3;
      out3 << Rinex3NavHeader3;

      while (Rinex3NavStream3 >> Rinex3NavData3)
      {
         out3 << Rinex3NavData3;
      }
      Rinex3NavHeader.dump( dmp );
      Rinex3NavData.dump( dmp );

      test1.assert( test1.fileEqualTest( inputRinexNavExample, outputTestOutput3,
                                         2 ), "test read TestOutput2", __LINE__ );
   }
   catch(...)
   {
      test1.assert( false, "test read TestOutput2, unexpected exception", __LINE__ );
   }

   return( test1.countFails() );
}

//------------------------------------------------------------
//   This test check that Rinex Header exceptions are thrown
//------------------------------------------------------------
int Rinex3Nav_T :: headerExceptionTest( void )
{
   TestUtil test2( "Rinex3NavStream", "exceptions", __FILE__, __LINE__ );

   std::string msg_test_desc       = " ";
   std::string msg_expected    = ", should throw a gpstk::Exception";
   std::string msg_false_pass  = ", but threw no exception.";
   std::string msg_true_fail   = ", but instead threw an unknown exception.";

   try
   {
      gpstk::Rinex3NavStream InvalidLineLength( inputInvalidLineLength.c_str() );
      gpstk::Rinex3NavStream NotaNavFile( inputNotaNavFile.c_str() );
      gpstk::Rinex3NavStream UnknownHeaderLabel( inputUnknownHeaderLabel.c_str() );
      gpstk::Rinex3NavStream IncompleteHeader( inputIncompleteHeader.c_str() );
      gpstk::Rinex3NavStream UnsupportedRinex( inputUnsupportedRinex.c_str() );
      gpstk::Rinex3NavStream BadHeader( inputBadHeader.c_str() );
      gpstk::Rinex3NavStream out( outputTestOutputHeader.c_str(), std::ios::out );
      gpstk::Rinex3NavHeader Header;

      InvalidLineLength.exceptions(  std::fstream::failbit );
      NotaNavFile.exceptions(        std::fstream::failbit );
      UnknownHeaderLabel.exceptions( std::fstream::failbit );
      IncompleteHeader.exceptions(   std::fstream::failbit );
      UnsupportedRinex.exceptions(   std::fstream::failbit );
      BadHeader.exceptions(          std::fstream::failbit );


         //------------------------------------------------------------
      msg_test_desc = "InvalidLineLength test";
      try
      {
         InvalidLineLength >> Header;
         test2.assert( false, msg_test_desc + msg_expected + msg_false_pass ,
                       __LINE__ );
      }
      catch( gpstk::Exception e )
      {
         test2.assert( true, msg_test_desc + msg_expected, __LINE__ );
      }
      catch(...)
      {
         test2.assert( false, msg_test_desc + msg_expected + msg_true_fail, __LINE__ );
      }

         //------------------------------------------------------------
      msg_test_desc = "NotaNavFile test";
      try
      {
         NotaNavFile >> Header;
         test2.assert( false, msg_test_desc + msg_expected + msg_false_pass ,
                       __LINE__ );
      }
      catch( gpstk::Exception e )
      {
         test2.assert( true, msg_test_desc + msg_expected, __LINE__ );
      }
      catch(...)
      {
         test2.assert( false, msg_test_desc + msg_expected + msg_true_fail, __LINE__ );
      }

         //------------------------------------------------------------
      msg_test_desc = "UnknownHeaderLabel test";
      try
      {
         UnknownHeaderLabel >> Header;
         test2.assert( false, msg_test_desc + msg_expected + msg_false_pass ,
                       __LINE__ );
      }
      catch( gpstk::Exception e )
      {
         test2.assert( true, msg_test_desc + msg_expected, __LINE__ );
      }
      catch(...)
      {
         test2.assert( false, msg_test_desc + msg_expected + msg_true_fail, __LINE__ );
      }

         //------------------------------------------------------------
      msg_test_desc = "IncompleteHeader test";
      try
      {
         IncompleteHeader >> Header;
         test2.assert( false, msg_test_desc + msg_expected + msg_false_pass ,
                       __LINE__ );
      }
      catch( gpstk::Exception e )
      {
         test2.assert( true, msg_test_desc + msg_expected, __LINE__ );
      }
      catch(...)
      {
         test2.assert( false, msg_test_desc + msg_expected + msg_true_fail, __LINE__ );
      }

         //------------------------------------------------------------
         // msg_test_desc = "UnsupportedRinex test"; Rinex version 3.33 is marked supported
         /*          try
                   {
                       UnsupportedRinex >> Header;
                       test2.assert( false, msg_test_desc + msg_expected + msg_false_pass , __LINE__ );
                   }
                   catch( gpstk::Exception e )
                   {
                       test2.assert( true, msg_test_desc + msg_expected, __LINE__ );
                   }
                   catch(...)
                   {
                       test2.assert( false, msg_test_desc + msg_expected + msg_true_fail, __LINE__ );
                   }
         */
         //------------------------------------------------------------
         // msg_test_desc = "BadHeader test"; Rinex version 3.33 is marked supported, is header only bad b/c v. 3.33?
         /*          try
                   {
                       BadHeader >> Header;
                       test2.assert( false, msg_test_desc + msg_expected + msg_false_pass , __LINE__ );
                   }
                   catch( gpstk::Exception e )
                   {
                       test2.assert( true, msg_test_desc + msg_expected, __LINE__ );
                   }
                   catch(...)
                   {
                       test2.assert( false, msg_test_desc + msg_expected + msg_true_fail, __LINE__ );
                   }
         */
         //------------------------------------------------------------

   }
   catch(...)
   {
      test2.assert( false, "test failure message", __LINE__ );
   }

   return( test2.countFails() );

}

//------------------------------------------------------------
//   Test Rinex3NavData File read/write with streams
//   * Read Rinex Nav file directly into a RinexEphemerisStore
//   * Write contents of RinexEphemerisStore back out to a new file
//   * Diff the old file and the new file
//------------------------------------------------------------
int Rinex3Nav_T :: streamReadWriteTest( void )
{
   TestUtil test3( "Rinex3NavData", "Redirect", __FILE__, __LINE__ );

   std::string msg_test_desc   = "streamReadWriteTest test";
   std::string msg_expected    = ", compares the output file with the input file";
   std::string msg_fail_equal  = ", files are different!";
   std::string msg_fail_except = ", unexpectedly threw an exception.";

   try
   {
      Rinex3NavStream rinexInputStream( inputRinexNavExample.c_str()  );
      Rinex3NavStream rinexOutputStream( outputRinexStore.c_str(), std::ios::out );
      Rinex3NavHeader streamTool;
      rinexInputStream >>
                       streamTool; // used to set rinexInputStream.header to a valid object
      rinexOutputStream.header = rinexInputStream.header;
      rinexOutputStream << rinexOutputStream.header;

      Rinex3NavData data;
      while( rinexInputStream >> data )
      {
         rinexOutputStream << data;
      }
      test3.assert( test3.fileEqualTest( inputRinexNavExample, outputRinexStore, 9),
                    msg_test_desc + msg_expected + msg_fail_equal, __LINE__ );
   }
   catch(...)
   {
      test3.assert( false, msg_test_desc + msg_expected + msg_fail_except,
                    __LINE__ );
   }

   return( test3.countFails() );

}

//------------------------------------------------------------
// Test for several of the members within RinexNavFilterOperators
//  including merge, EqualsFull, LessThanSimple, LessThanFull, and FilterPRN
//------------------------------------------------------------
int Rinex3Nav_T :: filterOperatorsTest( void )
{
   TestUtil test4( "Rinex3NavStream", "open", __FILE__, __LINE__ );

   std::string msg_test_desc = "";
   std::string msg_expected  = ", ";
   std::string msg_fail      = ", ";

   try
   {

      gpstk::Rinex3NavStream FilterStream1( inputFilterStream1.c_str() );
      FilterStream1.open( inputFilterStream1.c_str(), std::ios::in );
      gpstk::Rinex3NavStream FilterStream2( inputFilterStream2.c_str() );
      gpstk::Rinex3NavStream FilterStream3( inputFilterStream3.c_str() );
      gpstk::Rinex3NavStream out( outputFilterOutput.c_str(), std::ios::out );

      gpstk::Rinex3NavHeader FilterHeader1;
      gpstk::Rinex3NavHeader FilterHeader2;
      gpstk::Rinex3NavHeader FilterHeader3;

      gpstk::Rinex3NavData FilterData1;
      gpstk::Rinex3NavData FilterData2;
      gpstk::Rinex3NavData FilterData3;

      FilterStream1 >> FilterHeader1;
      FilterStream2 >> FilterHeader2;
      FilterStream3 >> FilterHeader3;

      while (FilterStream1 >> FilterData1)
      {
      }
      while (FilterStream2 >> FilterData2)
      {
      }
      while (FilterStream3 >> FilterData3)
      {
      }

      gpstk::Rinex3NavHeaderTouchHeaderMerge merged;
      merged( FilterHeader1 );
      merged( FilterHeader2 );
      out << merged.theHeader;

      gpstk::Rinex3NavDataOperatorEqualsFull EqualsFull;

      msg_test_desc =
         "Rinex3NavDataOperatorEqualsFull, EqualsFUll FilterData1 FilterData2, fail";
      test4.assert( EqualsFull( FilterData1, FilterData2 ), msg_test_desc,
                    __LINE__ );
      msg_test_desc =
         "Rinex3NavDataOperatorEqualsFull, FilterData1 not equal FilterData3, fail";
      test4.assert( !EqualsFull( FilterData1, FilterData3 ), msg_test_desc,
                    __LINE__ );

      gpstk::Rinex3NavDataOperatorLessThanSimple LessThanSimple;

      msg_test_desc =
         "Rinex3NavDataOperatorLessThanSimple, not LessThanSimple FilterData1 FilterData3, fail";
      test4.assert( !LessThanSimple(FilterData1, FilterData2), msg_test_desc,
                    __LINE__ );
         //CPPUNIT_ASSERT_EQUAL(true,LessThanSimple(FilterData1, FilterData3));

      gpstk::Rinex3NavDataOperatorLessThanFull LessThanFull;

      msg_test_desc =
         "Rinex3NavDataOperatorLessThanFull, not LessThanFull FilterData1 FilterData1, fail";
         //CPPUNIT_ASSERT_EQUAL(true,LessThanFull(FilterData1, FilterData3));
         //CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData3, FilterData1));
      test4.assert( !LessThanFull(FilterData1, FilterData1), msg_test_desc,
                    __LINE__ );

      std::list<long> list;
      list.push_front(6);
      gpstk::Rinex3NavDataFilterPRN FilterPRN(list);
      msg_test_desc = "Rinex3NavDataFilterPRN, FilterPRN FilterData3, fail";
      test4.assert( FilterPRN( FilterData3 ), msg_test_desc, __LINE__ );
         //cout << FilterPRN(FilterData3) << std:endl;
   }
   catch(...)
   {
      msg_test_desc = "filterOperatorsTest, threw unexpected exception, fail";
      test4.assert( false, msg_test_desc, __LINE__ );
   }

   return( test4.countFails() );

}

//------------------------------------------------------------
// Tests if a input Rinex 3 file can be output as a version 2 file
//------------------------------------------------------------

int Rinex3Nav_T :: version3ToVersion2Test(void)
{
   TestUtil testFramework("Rinex3Nav", "Convert v.3 to v.2", __FILE__, __LINE__ );

   gpstk::Rinex3NavStream inputStream(inputRinex3Nav.c_str());
   gpstk::Rinex3NavStream outputStream(outputRinex2Nav.c_str(), std::ios::out);
   gpstk::Rinex3NavHeader NavHeader;
   gpstk::Rinex3NavData NavData;

   inputStream >> NavHeader;

   NavHeader.version = 2.11;

   outputStream << NavHeader;
   while(inputStream >> NavData)
   {
      outputStream << NavData;
   }

      //skip first 2 lines, not expected to match
   fileCompare = testFramework.fileEqualTest(inputRinex2Nav, outputRinex2Nav, 2);

   failDescriptionString = "Version 2.11 output does not match expected file";
   testFramework.assert(fileCompare, failDescriptionString, __LINE__);

   return testFramework.countFails();
}

//------------------------------------------------------------
// Tests if a input Rinex 2 file can be output as a version 3 file
//------------------------------------------------------------


int Rinex3Nav_T :: version2ToVersion3Test(void)
{
   TestUtil testFramework("Rinex3Nav", "Convert v.2 to v.3", __FILE__, __LINE__ );

   gpstk::Rinex3NavStream inputStream(inputRinex2Nav.c_str());
   gpstk::Rinex3NavStream outputStream(outputRinex3Nav.c_str(), std::ios::out);
   gpstk::Rinex3NavHeader NavHeader;
   gpstk::Rinex3NavData NavData;

   inputStream >> NavHeader;

   NavHeader.version = 3.02;

   outputStream << NavHeader;
   while(inputStream >> NavData)
   {
      outputStream << NavData;
   }

      //skip first 2 lines, not expected to match
   fileCompare = testFramework.fileEqualTest(inputRinex3Nav, outputRinex3Nav, 2);

   failDescriptionString = "Version 3.02 output does not match expected file";
   testFramework.assert(fileCompare, failDescriptionString, __LINE__);

   return testFramework.countFails();
}


//============================================================
// Run all the test methods defined above
//============================================================

int main()
{

   int errorCount = 0;
   int errorTotal = 0;
   Rinex3Nav_T testClass;

   std::cout << "Running tests for Rinex version 2.1" << std::endl;

   errorTotal += testClass.headerExceptionTest();
   errorTotal += testClass.hardCodeTest();
   errorTotal += testClass.streamReadWriteTest();
   errorTotal += testClass.filterOperatorsTest();

      //Change to test v.3 Rinex files
   testClass.toRinex3();

   errorTotal += testClass.headerExceptionTest();
   errorTotal += testClass.hardCodeTest();
   errorTotal += testClass.streamReadWriteTest();
   errorTotal += testClass.filterOperatorsTest();

      // Unimplemented features
      //testClass.toConversionTest();
      //errorTotal += testClass.version2ToVersion3Test();
      //errorTotal += testClass.version3ToVersion2Test();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal <<
             std::endl;

   return( errorTotal );
}
