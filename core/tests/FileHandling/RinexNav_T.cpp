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



#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavFilterOperators.hpp"

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

class RinexNav_T
{

    public:

        // constructor
       RinexNav_T()
        {
            init();
        }
        // destructor
      ~RinexNav_T( ){}

        void init( void );

        // return values indicate number of failures, i.e., 0=PASS, !0=FAIL
        int hardCodeTest( void );
        int headerExceptionTest( void );
        int streamReadWriteTest( void );
        int filterOperatorsTest( void );

    private:

        std::string dataFilePath;

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

};

//============================================================
// Initialize Test Data Filenames
//============================================================

void RinexNav_T :: init( void )
{

    TestUtil test0;
    std::string dataFilePath = test0.getDataPath();
    std::string tempFilePath = test0.getTempPath();

    //---------------------------------------- 
    // Full file paths
    //---------------------------------------- 
    std::string file_sep = getFileSep();

    inputRinexNavExample     = dataFilePath + file_sep + "test_input_rinex_nav_RinexNavExample.99n";

    outputTestOutput         = tempFilePath + file_sep + "test_output_rinex_nav_TestOutput.99n";
    outputTestOutput2        = tempFilePath + file_sep + "test_output_rinex_nav_TestOutput2.99n";
    outputTestOutput3        = tempFilePath + file_sep + "test_output_rinex_nav_TestOutput3.99n";
    outputRinexDump          = tempFilePath + file_sep + "test_output_rinex_nav_RinexDump";

    inputInvalidLineLength   = dataFilePath + file_sep + "test_input_rinex_nav_InvalidLineLength.99n";
    inputNotaNavFile         = dataFilePath + file_sep + "test_input_rinex_nav_NotaNavFile.99n";
    inputUnknownHeaderLabel  = dataFilePath + file_sep + "test_input_rinex_nav_UnknownHeaderLabel.99n";
    inputIncompleteHeader    = dataFilePath + file_sep + "test_input_rinex_nav_IncompleteHeader.99n";
    inputUnsupportedRinex    = dataFilePath + file_sep + "test_input_rinex_nav_UnsupportedRinex.99n";
    inputBadHeader           = dataFilePath + file_sep + "test_input_rinex_nav_BadHeader.99n";
    outputTestOutputHeader   = tempFilePath + file_sep + "test_output_rinex_nav_TestOutputHeader.99n";

    inputFilterStream1       = dataFilePath + file_sep + "test_input_rinex_nav_FilterTest1.99n";
    inputFilterStream2       = dataFilePath + file_sep + "test_input_rinex_nav_FilterTest2.99n";
    inputFilterStream3       = dataFilePath + file_sep + "test_input_rinex_nav_FilterTest3.99n";
    outputFilterOutput       = tempFilePath + file_sep + "test_output_rinex_nav_FilterOutput.txt";

    outputRinexStore         = tempFilePath + file_sep + "test_output_rinex_nav_RinexStore.txt";

}

//=============================================================================
// Test Method Definitions
//=============================================================================


//------------------------------------------------------------
// This test checks to make sure that the internal members of
// the RinexNavHeader are as we think they should be.
// Also at the end of this test, we check and make sure our
// output file is equal to our input
// This assures that if any changes happen, the test will fail
// and the user will know.
// Also, output was put into input three times over to make sure
// there were no small errors which blow up into big errors
//------------------------------------------------------------
int RinexNav_T :: hardCodeTest( void )
{

  TestUtil test1( "RinexNavStream", "out", __FILE__, __LINE__ );

  try
  {
    gpstk::RinexNavStream RinexNavStream( inputRinexNavExample.c_str() );
    gpstk::RinexNavStream out( outputTestOutput.c_str(), std::ios::out );
    gpstk::RinexNavStream dmp( outputRinexDump.c_str(), std::ios::out );
    gpstk::RinexNavHeader RinexNavHeader;
    gpstk::RinexNavData RinexNavData;

    RinexNavStream >> RinexNavHeader;
    out << RinexNavHeader;

    while( RinexNavStream >> RinexNavData )
      {
        out << RinexNavData;
      }

    test1.assert( RinexNavHeader.version == 2.1,                             "RinexNav Header version comparison",      __LINE__ );
    test1.assert( RinexNavHeader.fileProgram == (std::string)"XXRINEXN V3",  "RinexNav Header file program comparison", __LINE__ );
    test1.assert( RinexNavHeader.fileAgency == (std::string)"AIUB",          "RinexNav Header file agency comparison",  __LINE__ );
    test1.assert( RinexNavHeader.date == (std::string)"09/02/1999 19:22:36", "RinexNav Header date comparison",         __LINE__ );

    std::vector<std::string>::const_iterator itr1 = RinexNavHeader.commentList.begin();
    test1.assert( (*itr1) == (std::string)"THIS IS ONE COMMENT", "RinexNav Header Comment comparison", __LINE__ );


    test1.assert( test1.fileEqualTest( inputRinexNavExample, outputTestOutput, 2 ), "RinexNav file read and then write, the two should be equal", __LINE__  );

    //------------------------------------------------------------
    gpstk::RinexNavStream RinexNavStream2( outputTestOutput.c_str() );
    gpstk::RinexNavStream out2( outputTestOutput2.c_str(), std::ios::out );
    gpstk::RinexNavHeader RinexNavHeader2;
    gpstk::RinexNavData RinexNavData2;

    RinexNavStream2 >> RinexNavHeader2;
    out2 << RinexNavHeader2;

    while (RinexNavStream2 >> RinexNavData2)
      {
        out2 << RinexNavData2;
      }

    gpstk::RinexNavStream RinexNavStream3( outputTestOutput2.c_str() );
    gpstk::RinexNavStream out3( outputTestOutput3.c_str() , std::ios::out );
    gpstk::RinexNavHeader RinexNavHeader3;
    gpstk::RinexNavData RinexNavData3;

    RinexNavStream3 >> RinexNavHeader3;
    out3 << RinexNavHeader3;

    while (RinexNavStream3 >> RinexNavData3)
      {
        out3 << RinexNavData3;
      }
    RinexNavHeader.dump( dmp );
    RinexNavData.dump( dmp );

    test1.assert( test1.fileEqualTest( inputRinexNavExample, outputTestOutput3, 2 ), "test read TestOutput2", __LINE__ );
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
int RinexNav_T :: headerExceptionTest( void )
{
    TestUtil test2( "RinexNavStream", "exceptions", __FILE__, __LINE__ );

    std::string msg_test_desc       = " ";
    std::string msg_expected    = ", should throw a gpstk::Exception";
    std::string msg_false_pass  = ", but threw no exception.";
    std::string msg_true_fail   = ", but instead threw an unknown exception.";
    
    try
    {
          gpstk::RinexNavStream InvalidLineLength( inputInvalidLineLength.c_str() );
          gpstk::RinexNavStream NotaNavFile( inputNotaNavFile.c_str() );
          gpstk::RinexNavStream UnknownHeaderLabel( inputUnknownHeaderLabel.c_str() );
          gpstk::RinexNavStream IncompleteHeader( inputIncompleteHeader.c_str() );
          gpstk::RinexNavStream UnsupportedRinex( inputUnsupportedRinex.c_str() );
          gpstk::RinexNavStream BadHeader( inputBadHeader.c_str() );
          gpstk::RinexNavStream out( outputTestOutputHeader.c_str(), std::ios::out );
          gpstk::RinexNavHeader Header;

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

          //------------------------------------------------------------
          msg_test_desc = "NotaNavFile test";
          try
          {
              NotaNavFile >> Header;
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

          //------------------------------------------------------------
          msg_test_desc = "UnknownHeaderLabel test";
          try
          {
              UnknownHeaderLabel >> Header;
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

          //------------------------------------------------------------
          msg_test_desc = "IncompleteHeader test";
          try
          {
              IncompleteHeader >> Header;
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

          //------------------------------------------------------------
          msg_test_desc = "UnsupportedRinex test";
          try
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

          //------------------------------------------------------------
          msg_test_desc = "BadHeader test";
          try
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

    }
    catch(...)
    {
        test2.assert( false, "test failure message", __LINE__ );
    }

    return( test2.countFails() );

}

//------------------------------------------------------------
//   Test RinexNavData File read/write with streams
//   * Read Rinex Nav file directly into a RinexEphemerisStore
//   * Write contents of RinexEphemerisStore back out to a new file
//   * Diff the old file and the new file
//------------------------------------------------------------
int RinexNav_T :: streamReadWriteTest( void )
{
    TestUtil test3( "RinexNavData", "Redirect", __FILE__, __LINE__ );

    std::string msg_test_desc   = "streamReadWriteTest test";
    std::string msg_expected    = ", compares the output file with the input file";
    std::string msg_fail_equal  = ", files are different!";
    std::string msg_fail_except = ", unexpectedly threw an exception.";

    try
    {
        RinexNavStream rinexInputStream( inputRinexNavExample.c_str()  );
        RinexNavStream rinexOutputStream( outputRinexStore.c_str(), std::ios::out );
        rinexOutputStream.header = rinexInputStream.header;
        rinexOutputStream << rinexOutputStream.header;

	RinexNavData data;
	while( rinexInputStream >> data )
	{
            rinexOutputStream << data;
	}
        test3.assert( test3.fileEqualTest( inputRinexNavExample, outputRinexStore, 9), msg_test_desc + msg_expected + msg_fail_equal, __LINE__ );
    }
    catch(...)
    {
        test3.assert( false, msg_test_desc + msg_expected + msg_fail_except, __LINE__ );
    }

    return( test3.countFails() );

}

//------------------------------------------------------------
// Test for several of the members within RinexNavFilterOperators
//  including merge, EqualsFull, LessThanSimple, LessThanFull, and FilterPRN
//------------------------------------------------------------
int RinexNav_T :: filterOperatorsTest( void )
{
    TestUtil test4( "RinexNavStream", "open", __FILE__, __LINE__ );

    std::string msg_test_desc = "";
    std::string msg_expected  = ", ";
    std::string msg_fail      = ", ";

    try
    {

      gpstk::RinexNavStream FilterStream1( inputFilterStream1.c_str() );
      FilterStream1.open( inputFilterStream1.c_str(), std::ios::in );
      gpstk::RinexNavStream FilterStream2( inputFilterStream2.c_str() );
      gpstk::RinexNavStream FilterStream3( inputFilterStream3.c_str() );
      gpstk::RinexNavStream out( outputFilterOutput.c_str(), std::ios::out );

      gpstk::RinexNavHeader FilterHeader1;
      gpstk::RinexNavHeader FilterHeader2;
      gpstk::RinexNavHeader FilterHeader3;

      gpstk::RinexNavData FilterData1;
      gpstk::RinexNavData FilterData2;
      gpstk::RinexNavData FilterData3;

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

      gpstk::RinexNavHeaderTouchHeaderMerge merged;
      merged( FilterHeader1 );
      merged( FilterHeader2 );
      out << merged.theHeader;

      gpstk::RinexNavDataOperatorEqualsFull EqualsFull;

      msg_test_desc = "RinexNavDataOperatorEqualsFull, EqualsFUll FilterData1 FilterData2, fail";
      test4.assert( EqualsFull( FilterData1, FilterData2 ), msg_test_desc, __LINE__ );
      msg_test_desc = "RinexNavDataOperatorEqualsFull, FilterData1 not equal FilterData3, fail";
      test4.assert( !EqualsFull( FilterData1, FilterData3 ), msg_test_desc, __LINE__ );

      gpstk::RinexNavDataOperatorLessThanSimple LessThanSimple;
      
      msg_test_desc = "RinexNavDataOperatorLessThanSimple, not LessThanSimple FilterData1 FilterData3, fail";
      test4.assert( !LessThanSimple(FilterData1, FilterData2), msg_test_desc, __LINE__ );
      //CPPUNIT_ASSERT_EQUAL(true,LessThanSimple(FilterData1, FilterData3));

      gpstk::RinexNavDataOperatorLessThanFull LessThanFull;

      msg_test_desc = "RinexNavDataOperatorLessThanFull, not LessThanFull FilterData1 FilterData1, fail";
      //CPPUNIT_ASSERT_EQUAL(true,LessThanFull(FilterData1, FilterData3));
      //CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData3, FilterData1));
      test4.assert( !LessThanFull(FilterData1, FilterData1), msg_test_desc, __LINE__ );

      std::list<long> list;
      list.push_front(6);
      gpstk::RinexNavDataFilterPRN FilterPRN(list);
      msg_test_desc = "RinexNavDataFilterPRN, FilterPRN FilterData3, fail";
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

//============================================================
// Run all the test methods defined above
//============================================================

int main()
{

    int errorCount = 0;
    int errorTotal = 0;
    RinexNav_T testClass;

    errorCount = testClass.headerExceptionTest();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.hardCodeTest();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.streamReadWriteTest();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.filterOperatorsTest();
    errorTotal = errorTotal + errorCount;

    std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal << std::endl;

    return( errorTotal );
}
