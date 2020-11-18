//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================


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
   ~Rinex3Nav_T() {}

   void init();

   void toRinex3();

      // return values indicate number of failures, i.e., 0=PASS, 0!=FAIL
   unsigned hardCodeTest();
   unsigned headerExceptionTest();
   unsigned streamReadWriteTest();
   unsigned filterOperatorsTest();

   void toConversionTest();
   unsigned version2ToVersion3Test();
   unsigned version3ToVersion2Test();

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

void Rinex3Nav_T :: init()
{

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

void Rinex3Nav_T :: toRinex3()
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

void Rinex3Nav_T :: toConversionTest()
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
unsigned Rinex3Nav_T :: hardCodeTest()
{
   double compareVersion;
   std::string compareFileProgram;
   std::string compareFileAgency;
   std::string compareDate;
   std::string compareComment;

   TUDEF("Rinex3NavStream", "ostream<<");

   try
   {
      gpstk::Rinex3NavStream rinex3NavStream(inputRinexNavExample.c_str());
      gpstk::Rinex3NavStream out(outputTestOutput.c_str(), std::ios::out);
      gpstk::Rinex3NavStream dmp(outputRinexDump.c_str(), std::ios::out);
      gpstk::Rinex3NavHeader rinex3NavHeader;
      gpstk::Rinex3NavData rinex3NavData;

      rinex3NavStream >> rinex3NavHeader;
      out << rinex3NavHeader;

      while (rinex3NavStream >> rinex3NavData)
      {
         out << rinex3NavData;
      }

      if (rinex3NavHeader.version == 2.1)
      {
         compareVersion = 2.1;
         compareFileProgram = (std::string)"XXRINEXN V3";
         compareFileAgency = (std::string)"AIUB";
         compareDate = (std::string)"1999-09-02 19:22:36";
         compareComment = (std::string)"THIS IS ONE COMMENT";
      }
      else if (rinex3NavHeader.version == 3.02)
      {
         compareVersion = 3.02;
         compareFileProgram = (std::string)"cnvtToRINEX 2.25.0";
         compareFileAgency = (std::string)"convertToRINEX OPR";
         compareDate = (std::string)"23-Jan-15 22:34 UTC";
         compareComment = (std::string)
                          "-----------------------------------------------------------";
      }

      TUCSM("operator>>");
      TUASSERTFE(compareVersion, rinex3NavHeader.version);
      TUASSERTE(std::string, compareFileProgram, rinex3NavHeader.fileProgram);
      TUASSERTE(std::string, compareFileAgency, rinex3NavHeader.fileAgency);
      TUASSERTE(std::string, compareDate, rinex3NavHeader.date);

      std::vector<std::string>::const_iterator itr1 =
         rinex3NavHeader.commentList.begin();
      TUASSERTE(std::string, compareComment, *itr1);

      TUCSM("operator<<");
      TUCMPFILE(inputRinexNavExample, outputTestOutput, 2);

         //------------------------------------------------------------
      gpstk::Rinex3NavStream rinex3NavStream2(outputTestOutput.c_str());
      gpstk::Rinex3NavStream out2(outputTestOutput2.c_str(), std::ios::out);
      gpstk::Rinex3NavHeader rinex3NavHeader2;
      gpstk::Rinex3NavData rinex3NavData2;

      rinex3NavStream2 >> rinex3NavHeader2;
      out2 << rinex3NavHeader2;

      while (rinex3NavStream2 >> rinex3NavData2)
      {
         out2 << rinex3NavData2;
      }

      gpstk::Rinex3NavStream rinex3NavStream3(outputTestOutput2.c_str());
      gpstk::Rinex3NavStream out3(outputTestOutput3.c_str() , std::ios::out);
      gpstk::Rinex3NavHeader rinex3NavHeader3;
      gpstk::Rinex3NavData rinex3NavData3;

      rinex3NavStream3 >> rinex3NavHeader3;
      out3 << rinex3NavHeader3;

      while (rinex3NavStream3 >> rinex3NavData3)
      {
         out3 << rinex3NavData3;
      }
      rinex3NavHeader.dump(dmp);
      rinex3NavData.dump(dmp);

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
unsigned Rinex3Nav_T :: headerExceptionTest()
{
   TUDEF("rinex3NavStream", "");

   std::string msg_test_desc       = " ";
   std::string msg_expected    = ", should throw a gpstk::Exception";
   std::string msg_false_pass  = ", but threw no exception.";
   std::string msg_true_fail   = ", but instead threw an unknown exception.";

   try
   {
      gpstk::Rinex3NavStream invalidLineLength(inputInvalidLineLength.c_str());
      gpstk::Rinex3NavStream notaNavFile(inputNotaNavFile.c_str());
      gpstk::Rinex3NavStream unknownHeaderLabel(inputUnknownHeaderLabel.c_str());
      gpstk::Rinex3NavStream incompleteHeader(inputIncompleteHeader.c_str());
      gpstk::Rinex3NavStream unsupportedRinex(inputUnsupportedRinex.c_str());
      gpstk::Rinex3NavStream badHeader(inputBadHeader.c_str());
      gpstk::Rinex3NavStream out(outputTestOutputHeader.c_str(), std::ios::out);
      gpstk::Rinex3NavHeader header;

      invalidLineLength.exceptions( std::fstream::failbit);
      notaNavFile.exceptions(       std::fstream::failbit);
      unknownHeaderLabel.exceptions(std::fstream::failbit);
      incompleteHeader.exceptions(  std::fstream::failbit);
      unsupportedRinex.exceptions(  std::fstream::failbit);
      badHeader.exceptions(         std::fstream::failbit);


         //------------------------------------------------------------
      msg_test_desc = "InvalidLineLength test";
      try
      {
         invalidLineLength >> header;
         TUFAIL(msg_test_desc + msg_expected + msg_false_pass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_test_desc + msg_expected);
      }
      catch(...)
      {
         TUFAIL( msg_test_desc + msg_expected + msg_true_fail);
      }

         //------------------------------------------------------------
      msg_test_desc = "NotaNavFile test";
      try
      {
         notaNavFile >> header;
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
         unknownHeaderLabel >> header;
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
         incompleteHeader >> header;
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
         // msg_test_desc = "UnsupportedRinex test"; Rinex version 3.33 is marked supported
         /*          try
                   {
                       UnsupportedRinex >> header;
                       test2.assert(false, msg_test_desc + msg_expected + msg_false_pass , __LINE__);
                   }
                   catch(gpstk::Exception e)
                   {
                       test2.assert(true, msg_test_desc + msg_expected, __LINE__);
                   }
                   catch(...)
                   {
                       test2.assert(false, msg_test_desc + msg_expected + msg_true_fail, __LINE__);
                   }
         */
         //------------------------------------------------------------
         // msg_test_desc = "BadHeader test"; Rinex version 3.33 is marked supported, is header only bad b/c v. 3.33?
         /*          try
                   {
                       BadHeader >> header;
                       test2.assert(false, msg_test_desc + msg_expected + msg_false_pass , __LINE__);
                   }
                   catch(gpstk::Exception e)
                   {
                       test2.assert(true, msg_test_desc + msg_expected, __LINE__);
                   }
                   catch(...)
                   {
                       test2.assert(false, msg_test_desc + msg_expected + msg_true_fail, __LINE__);
                   }
         */
         //------------------------------------------------------------

   }
   catch(...)
   {
      TUFAIL("Unknown exception");
   }
   TURETURN();
}

//------------------------------------------------------------
//   Test rinex3NavData File read/write with streams
//   * Read Rinex Nav file directly into a RinexEphemerisStore
//   * Write contents of RinexEphemerisStore back out to a new file
//   * Diff the old file and the new file
//------------------------------------------------------------
unsigned Rinex3Nav_T :: streamReadWriteTest()
{
   TUDEF("rinex3NavData", "Redirect");

   std::string msg_test_desc   = "streamReadWriteTest test";
   std::string msg_expected    = ", compares the output file with the input file";
   std::string msg_fail_equal  = ", files are different!";
   std::string msg_fail_except = ", unexpectedly threw an exception.";

   try
   {
      Rinex3NavStream rinexInputStream(inputRinexNavExample.c_str() );
      Rinex3NavStream rinexOutputStream(outputRinexStore.c_str(),std::ios::out);
      Rinex3NavHeader streamTool;
         // used to set rinexInputStream.header to a valid object
      rinexInputStream >> streamTool;
      rinexOutputStream.header = rinexInputStream.header;
      rinexOutputStream << rinexOutputStream.header;

      Rinex3NavData data;
      while (rinexInputStream >> data)
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
unsigned Rinex3Nav_T :: filterOperatorsTest()
{
      // todo: This is a brokent test as of 4/25/16. In some environments
      // this test is returning a false pass and on others its failing.
   return 0;
#if 0
   TestUtil test4("rinex3NavStream", "open", __FILE__, __LINE__);

   std::string msg_test_desc = "";
   std::string msg_expected  = ", ";
   std::string msg_fail      = ", ";

   try
   {

      gpstk::Rinex3NavStream FilterStream1(inputFilterStream1.c_str());
      FilterStream1.open(inputFilterStream1.c_str(), std::ios::in);
      gpstk::Rinex3NavStream FilterStream2(inputFilterStream2.c_str());
      gpstk::Rinex3NavStream FilterStream3(inputFilterStream3.c_str());
      gpstk::Rinex3NavStream out(outputFilterOutput.c_str(), std::ios::out);

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
      merged(FilterHeader1);
      merged(FilterHeader2);
      out << merged.theHeader;

      gpstk::Rinex3NavDataOperatorEqualsFull EqualsFull;

      msg_test_desc =
         "rinex3NavDataOperatorEqualsFull, EqualsFUll FilterData1 FilterData2, fail";
      test4.assert(EqualsFull(FilterData1, FilterData2), msg_test_desc,
                    __LINE__);
      msg_test_desc =
         "rinex3NavDataOperatorEqualsFull, FilterData1 not equal FilterData3, fail";
      test4.assert(!EqualsFull(FilterData1, FilterData3), msg_test_desc,
                    __LINE__);

      gpstk::Rinex3NavDataOperatorLessThanSimple LessThanSimple;

      msg_test_desc =
         "rinex3NavDataOperatorLessThanSimple, not LessThanSimple FilterData1 FilterData3, fail";
      test4.assert(!LessThanSimple(FilterData1, FilterData2), msg_test_desc,
                    __LINE__);

      gpstk::Rinex3NavDataOperatorLessThanFull LessThanFull;

      msg_test_desc =
         "rinex3NavDataOperatorLessThanFull, not LessThanFull FilterData1 FilterData1, fail";
      test4.assert(!LessThanFull(FilterData1, FilterData1), msg_test_desc,
                    __LINE__);

      std::list<long> list;
      list.push_front(6);
      gpstk::Rinex3NavDataFilterPRN FilterPRN(list);
      msg_test_desc = "rinex3NavDataFilterPRN, FilterPRN FilterData3, fail";
      test4.assert(FilterPRN(FilterData3), msg_test_desc, __LINE__);
   }
   catch(...)
   {
      msg_test_desc = "filterOperatorsTest, threw unexpected exception, fail";
      test4.assert(false, msg_test_desc, __LINE__);
   }

   return(test4.countFails());
#endif
}

//------------------------------------------------------------
// Tests if a input Rinex 3 file can be output as a version 2 file
//------------------------------------------------------------

unsigned Rinex3Nav_T :: version3ToVersion2Test()
{
   TUDEF("Rinex3Nav", "Convert v.3 to v.2");

   gpstk::Rinex3NavStream inputStream(inputRinex3Nav.c_str());
   gpstk::Rinex3NavStream outputStream(outputRinex2Nav.c_str(), std::ios::out);
   gpstk::Rinex3NavHeader navHeader;
   gpstk::Rinex3NavData navData;

   inputStream >> navHeader;

   navHeader.version = 2.11;

   outputStream << navHeader;
   while (inputStream >> navData)
   {
      outputStream << navData;
   }

      //skip first 2 lines, not expected to match
   TUCMPFILE(inputRinex2Nav, outputRinex2Nav, 2);

   TURETURN();
}

//------------------------------------------------------------
// Tests if a input Rinex 2 file can be output as a version 3 file
//------------------------------------------------------------


unsigned Rinex3Nav_T :: version2ToVersion3Test()
{
   TUDEF("Rinex3Nav", "Convert v.2 to v.3");

   gpstk::Rinex3NavStream inputStream(inputRinex2Nav.c_str());
   gpstk::Rinex3NavStream outputStream(outputRinex3Nav.c_str(), std::ios::out);
   gpstk::Rinex3NavHeader navHeader;
   gpstk::Rinex3NavData navData;

   inputStream >> navHeader;

   navHeader.version = 3.02;

   outputStream << navHeader;
   while (inputStream >> navData)
   {
      outputStream << navData;
   }

      //skip first 2 lines, not expected to match
   TUCMPFILE(inputRinex3Nav, outputRinex3Nav, 2);

   TURETURN();
}


//============================================================
// Run all the test methods defined above
//============================================================

int main()
{
   unsigned errorTotal = 0;
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

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return(errorTotal);
}
