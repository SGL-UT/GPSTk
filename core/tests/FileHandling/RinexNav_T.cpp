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
   ~RinexNav_T() {}

   void init();

      // return values indicate number of failures, i.e., 0=PASS, !0=FAIL
   unsigned hardCodeTest();
   unsigned headerExceptionTest();
   unsigned streamReadWriteTest();
   unsigned filterOperatorsTest();
   unsigned castTest();

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

void RinexNav_T :: init()
{

   TestUtil test0;
   std::string dataFilePath = gpstk::getPathData();
   std::string tempFilePath = gpstk::getPathTestTemp();
   std::string file_sep = getFileSep();
   std::string dp = dataFilePath + file_sep;
   std::string tp = tempFilePath + file_sep;

      //----------------------------------------
      // Full file paths
      //----------------------------------------

   inputRinexNavExample     = dp+"test_input_rinex_nav_RinexNavExample.99n";
   outputTestOutput         = tp+"test_output_rinex_nav_TestOutput.99n";
   outputTestOutput2        = tp+"test_output_rinex_nav_TestOutput2.99n";
   outputTestOutput3        = tp+"test_output_rinex_nav_TestOutput3.99n";
   outputRinexDump          = tp+"test_output_rinex_nav_RinexDump";
   inputInvalidLineLength   = dp+"test_input_rinex_nav_InvalidLineLength.99n";
   inputNotaNavFile         = dp+"test_input_rinex_nav_NotaNavFile.99n";
   inputUnknownHeaderLabel  = dp+"test_input_rinex_nav_UnknownHeaderLabel.99n";
   inputIncompleteHeader    = dp+"test_input_rinex_nav_IncompleteHeader.99n";
   inputUnsupportedRinex    = dp+"test_input_rinex_nav_UnsupportedRinex.99n";
   inputBadHeader           = dp+"test_input_rinex_nav_BadHeader.99n";
   outputTestOutputHeader   = tp+"test_output_rinex_nav_TestOutputHeader.99n";
   inputFilterStream1       = dp+"test_input_rinex_nav_FilterTest1.99n";
   inputFilterStream2       = dp+"test_input_rinex_nav_FilterTest2.99n";
   inputFilterStream3       = dp+"test_input_rinex_nav_FilterTest3.99n";
   outputFilterOutput       = tp+"test_output_rinex_nav_FilterOutput.txt";
   outputRinexStore         = tp+"test_output_rinex_nav_RinexStore.txt";
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
unsigned RinexNav_T :: hardCodeTest()
{

   TUDEF("RinexNavStream", "out");

   try
   {
      gpstk::RinexNavStream RinexNavStream(inputRinexNavExample.c_str());
      gpstk::RinexNavStream out(outputTestOutput.c_str(), std::ios::out);
      gpstk::RinexNavStream dmp(outputRinexDump.c_str(), std::ios::out);
      gpstk::RinexNavHeader RinexNavHeader;
      gpstk::RinexNavData RinexNavData;

      RinexNavStream >> RinexNavHeader;
      out << RinexNavHeader;

      while(RinexNavStream >> RinexNavData)
      {
         out << RinexNavData;
      }

      TUASSERTFE(2.1, RinexNavHeader.version);
      TUASSERTE(std::string, "XXRINEXN V3", RinexNavHeader.fileProgram);
      TUASSERTE(std::string, "AIUB", RinexNavHeader.fileAgency);
      TUASSERTE(std::string, "09/02/1999 19:22:36", RinexNavHeader.date);
      std::vector<std::string>::const_iterator itr1 =
         RinexNavHeader.commentList.begin();
      TUASSERTE(std::string, "THIS IS ONE COMMENT", *itr1);
      TUCMPFILE(inputRinexNavExample, outputTestOutput, 2);

         //------------------------------------------------------------
      gpstk::RinexNavStream RinexNavStream2(outputTestOutput.c_str());
      gpstk::RinexNavStream out2(outputTestOutput2.c_str(), std::ios::out);
      gpstk::RinexNavHeader RinexNavHeader2;
      gpstk::RinexNavData RinexNavData2;

      RinexNavStream2 >> RinexNavHeader2;
      out2 << RinexNavHeader2;

      while (RinexNavStream2 >> RinexNavData2)
      {
         out2 << RinexNavData2;
      }

      gpstk::RinexNavStream RinexNavStream3(outputTestOutput2.c_str());
      gpstk::RinexNavStream out3(outputTestOutput3.c_str() , std::ios::out);
      gpstk::RinexNavHeader RinexNavHeader3;
      gpstk::RinexNavData RinexNavData3;

      RinexNavStream3 >> RinexNavHeader3;
      out3 << RinexNavHeader3;

      while (RinexNavStream3 >> RinexNavData3)
      {
         out3 << RinexNavData3;
      }
      RinexNavHeader.dump(dmp);
      RinexNavData.dump(dmp);

      TUCMPFILE(inputRinexNavExample, outputTestOutput3, 2);
   }
   catch(...)
   {
      TUFAIL("test read TestOutput2, unexpected exception");
   }

   TURETURN();
}

//------------------------------------------------------------
//   This test check that Rinex Header exceptions are thrown
//------------------------------------------------------------
unsigned RinexNav_T :: headerExceptionTest()
{
   TUDEF("RinexNavStream", "exceptions");

   std::string msg_test_desc   = " ";
   std::string msg_expected    = ", should throw a gpstk::Exception";
   std::string msg_false_pass  = ", but threw no exception.";
   std::string msg_true_fail   = ", but instead threw an unknown exception.";

   try
   {
      gpstk::RinexNavStream InvalidLineLength(inputInvalidLineLength.c_str());
      gpstk::RinexNavStream NotaNavFile(inputNotaNavFile.c_str());
      gpstk::RinexNavStream UnknownHeaderLabel(inputUnknownHeaderLabel.c_str());
      gpstk::RinexNavStream IncompleteHeader(inputIncompleteHeader.c_str());
      gpstk::RinexNavStream UnsupportedRinex(inputUnsupportedRinex.c_str());
      gpstk::RinexNavStream BadHeader(inputBadHeader.c_str());
      gpstk::RinexNavStream out(outputTestOutputHeader.c_str(), std::ios::out);
      gpstk::RinexNavHeader Header;

      InvalidLineLength.exceptions( std::fstream::failbit);
      NotaNavFile.exceptions(       std::fstream::failbit);
      UnknownHeaderLabel.exceptions(std::fstream::failbit);
      IncompleteHeader.exceptions(  std::fstream::failbit);
      UnsupportedRinex.exceptions(  std::fstream::failbit);
      BadHeader.exceptions(         std::fstream::failbit);


         //------------------------------------------------------------
      msg_test_desc = "InvalidLineLength test";
      try
      {
         InvalidLineLength >> Header;
         TUFAIL(msg_test_desc + msg_expected + msg_false_pass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_test_desc + msg_expected);
      }
      catch(...)
      {
         TUFAIL(msg_test_desc + msg_expected + msg_true_fail);
      }

         //------------------------------------------------------------
      msg_test_desc = "NotaNavFile test";
      try
      {
         NotaNavFile >> Header;
         TUFAIL(msg_test_desc + msg_expected + msg_false_pass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_test_desc + msg_expected);
      }
      catch(...)
      {
         TUFAIL(msg_test_desc + msg_expected + msg_true_fail);
      }

         //------------------------------------------------------------
      msg_test_desc = "UnknownHeaderLabel test";
      try
      {
         UnknownHeaderLabel >> Header;
         TUFAIL(msg_test_desc + msg_expected + msg_false_pass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_test_desc + msg_expected);
      }
      catch(...)
      {
         TUFAIL(msg_test_desc + msg_expected + msg_true_fail);
      }

         //------------------------------------------------------------
      msg_test_desc = "IncompleteHeader test";
      try
      {
         IncompleteHeader >> Header;
         TUFAIL(msg_test_desc + msg_expected + msg_false_pass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_test_desc + msg_expected);
      }
      catch(...)
      {
         TUFAIL(msg_test_desc + msg_expected + msg_true_fail);
      }

         //------------------------------------------------------------
      msg_test_desc = "UnsupportedRinex test";
      try
      {
         UnsupportedRinex >> Header;
         TUFAIL(msg_test_desc + msg_expected + msg_false_pass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_test_desc + msg_expected);
      }
      catch(...)
      {
         TUFAIL(msg_test_desc + msg_expected + msg_true_fail);
      }

         //------------------------------------------------------------
      msg_test_desc = "BadHeader test";
      try
      {
         BadHeader >> Header;
         TUFAIL(msg_test_desc + msg_expected + msg_false_pass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_test_desc + msg_expected);
      }
      catch(...)
      {
         TUFAIL(msg_test_desc + msg_expected + msg_true_fail);
      }

   }
   catch(...)
   {
      TUFAIL("test failure message");
   }

   TURETURN();
}

//------------------------------------------------------------
//   Test RinexNavData File read/write with streams
//   * Read Rinex Nav file directly into a RinexEphemerisStore
//   * Write contents of RinexEphemerisStore back out to a new file
//   * Diff the old file and the new file
//------------------------------------------------------------
unsigned RinexNav_T :: streamReadWriteTest()
{
   TUDEF("RinexNavData", "Redirect");

   std::string msg_test_desc   = "streamReadWriteTest test";
   std::string msg_expected    = ", compares the output file with the input file";
   std::string msg_fail_equal  = ", files are different!";
   std::string msg_fail_except = ", unexpectedly threw an exception.";

   try
   {
      RinexNavStream rinexInputStream(inputRinexNavExample.c_str() );
      RinexNavStream rinexOutputStream(outputRinexStore.c_str(), std::ios::out);
      rinexInputStream >> rinexOutputStream.header;
      rinexOutputStream << rinexOutputStream.header;

      RinexNavData data;
      while(rinexInputStream >> data)
      {
         rinexOutputStream << data;
      }
      TUCMPFILE(inputRinexNavExample, outputRinexStore, 9);
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_expected + msg_fail_except);
   }

   TURETURN();
}

//------------------------------------------------------------
// Test for several of the members within RinexNavFilterOperators
//  including merge, EqualsFull, LessThanSimple, LessThanFull, and FilterPRN
//------------------------------------------------------------
unsigned RinexNav_T :: filterOperatorsTest()
{
   TUDEF("RinexNavStream", "open");

   try
   {
      gpstk::RinexNavStream FilterStream1(inputFilterStream1.c_str());
      FilterStream1.open(inputFilterStream1.c_str(), std::ios::in);
      gpstk::RinexNavStream FilterStream2(inputFilterStream2.c_str());
      gpstk::RinexNavStream FilterStream3(inputFilterStream3.c_str());
      gpstk::RinexNavStream out(outputFilterOutput.c_str(), std::ios::out);

      gpstk::RinexNavHeader FilterHeader1;
      gpstk::RinexNavHeader FilterHeader2;
      gpstk::RinexNavHeader FilterHeader3;

      gpstk::RinexNavData FilterData1;
      gpstk::RinexNavData FilterData2;
      gpstk::RinexNavData FilterData3;
      gpstk::RinexNavData rndata;

      FilterStream1 >> FilterHeader1;
      FilterStream2 >> FilterHeader2;
      FilterStream3 >> FilterHeader3;

      while (FilterStream1 >> rndata)
      {
         FilterData1 = rndata;
      }
      while (FilterStream2 >> rndata)
      {
         FilterData2 = rndata;
      }
      while (FilterStream3 >> rndata)
      {
         FilterData3 = rndata;
      }

      gpstk::RinexNavHeaderTouchHeaderMerge merged;
      merged(FilterHeader1);
      merged(FilterHeader2);
      out << merged.theHeader;

      gpstk::RinexNavDataOperatorEqualsFull EqualsFull;

      TUASSERT(EqualsFull(FilterData1, FilterData2));
      TUASSERT(!EqualsFull(FilterData1, FilterData3));

      gpstk::RinexNavDataOperatorLessThanSimple LessThanSimple;

      TUASSERT(!LessThanSimple(FilterData1, FilterData2));
         //CPPUNIT_ASSERT_EQUAL(true,LessThanSimple(FilterData1, FilterData3));

      gpstk::RinexNavDataOperatorLessThanFull LessThanFull;

         //CPPUNIT_ASSERT_EQUAL(true,LessThanFull(FilterData1, FilterData3));
         //CPPUNIT_ASSERT_EQUAL(false,LessThanFull(FilterData3, FilterData1));
      TUASSERT(!LessThanFull(FilterData1, FilterData1));

      std::list<long> list;
      list.push_front(6);
      gpstk::RinexNavDataFilterPRN FilterPRN(list);
      TUASSERT(FilterPRN(FilterData3));
         //cout << FilterPRN(FilterData3) << std:endl;
   }
   catch(...)
   {
      TUFAIL("unexpected exception");
   }

   TURETURN();
}


unsigned RinexNav_T ::
castTest()
{
   TUDEF("RinexNavData", "cast");
   TURETURN();
}

//============================================================
// Run all the test methods defined above
//============================================================

int main()
{
   using namespace std;
   unsigned errorTotal = 0;
   RinexNav_T testClass;

   errorTotal += testClass.headerExceptionTest();
   errorTotal += testClass.hardCodeTest();
   errorTotal += testClass.streamReadWriteTest();
   errorTotal += testClass.filterOperatorsTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return(errorTotal);
}
