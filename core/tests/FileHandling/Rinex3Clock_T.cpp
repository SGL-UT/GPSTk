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

#include "TestUtil.hpp"
#include <iostream>

#include "Rinex3ClockHeader.hpp"
#include "Rinex3ClockData.hpp"
#include "Rinex3ClockStream.hpp"

#include "build_config.h"

class Rinex3Clock_T
{
public:
   Rinex3Clock_T()  {} // Default Constructor, set the precision value
   ~Rinex3Clock_T() {} // Default Destructor


   void init(void);
   void toRinex3(void);
   int headerExceptionTest(void);
   int hardCodeTest(void);
   int dataExceptionTest(void);
   int filterOperatorsTest(void);

private:
   std::string dataFilePath;
   std::string tempFilePath;

   std::string dataBadEpochLine;
   std::string dataIncompleteHeader;
   std::string dataInvalidLineLength;
   std::string dataNotAClockFile;
   std::string dataRinexClockFile;
   std::string dataUnknownHeaderLabel;

   std::string dataTestOutput;

   std::string testMesg;
   bool fileEqual;

};

// Initialize data file names
void Rinex3Clock_T::init(void)
{
   gpstk::TestUtil testFramework;
   dataFilePath = gpstk::getPathData();
   tempFilePath = gpstk::getPathTestTemp();

   std::cout<<"Running tests for Rinex v.2 files"<<std::endl;

   std::string file_sep = gpstk::getFileSep();

   dataBadEpochLine             = dataFilePath + file_sep +
                                  "test_input_rinex2_clock_BadEpochLine.96c";
   dataIncompleteHeader = dataFilePath + file_sep +
                          "test_input_rinex2_clock_IncompleteHeader.96c";
   dataInvalidLineLength        = dataFilePath + file_sep +
                                  "test_input_rinex2_clock_InvalidLineLength.96c";
   dataNotAClockFile            = dataFilePath + file_sep +
                                  "test_input_rinex2_clock_NotAClockFile.96c";
   dataRinexClockFile           = dataFilePath + file_sep +
                                  "test_input_rinex2_clock_RinexClockExample.96c";
   dataUnknownHeaderLabel  = dataFilePath + file_sep +
                             "test_input_rinex2_clock_UnknownHeaderLabel.96c";

   dataTestOutput                       = tempFilePath + file_sep +
                                          "test_output_rinex2_clock_TestOutput.96c";


}

void Rinex3Clock_T::toRinex3(void)
{
   std::cout<<"Running tests for Rinex v.3 files"<<std::endl;

   std::string file_sep = gpstk::getFileSep();

   dataBadEpochLine             = dataFilePath + file_sep +
                                  "test_input_rinex3_clock_BadEpochLine.96c";
   dataIncompleteHeader = dataFilePath + file_sep +
                          "test_input_rinex3_clock_IncompleteHeader.96c";
   dataInvalidLineLength        = dataFilePath + file_sep +
                                  "test_input_rinex3_clock_InvalidLineLength.96c";
   dataNotAClockFile            = dataFilePath + file_sep +
                                  "test_input_rinex3_clock_NotAClockFile.96c";
   dataRinexClockFile           = dataFilePath + file_sep +
                                  "test_input_rinex3_clock_RinexClockExample.96c";

   dataTestOutput                       = tempFilePath + file_sep +
                                          "test_output_rinex3_clock_TestOutput.96c";

}

int Rinex3Clock_T::headerExceptionTest(void)
{
   TUDEF("Rinex3ClockStream", "HeaderExceptions");

   std::string msg_desc                 = "";
   std::string msg_expect               = ", should throw gpstk::Exception";
   std::string msg_falsePass    = " but threw no exception.";
   std::string msg_trueFail     = " but instead threw an unknown exception";

   try
   {
         // gpstk::Rinex3ClockStream BadEpochLine(dataBadEpochLine.c_str());
      gpstk::Rinex3ClockStream IncompleteHeader(dataIncompleteHeader.c_str());
      gpstk::Rinex3ClockStream InvalidLineLength(dataInvalidLineLength.c_str());
      gpstk::Rinex3ClockStream NotAClockFile(dataNotAClockFile.c_str());
      gpstk::Rinex3ClockStream RinexClockFile(dataRinexClockFile.c_str());
      gpstk::Rinex3ClockStream UnknownHeaderLabel(dataUnknownHeaderLabel.c_str());

      gpstk::Rinex3ClockHeader ch;

         // BadEpochLine.exceptions(std::fstream::failbit);
      IncompleteHeader.exceptions(std::fstream::failbit);
      InvalidLineLength.exceptions(std::fstream::failbit);
      NotAClockFile.exceptions(std::fstream::failbit);
      RinexClockFile.exceptions(std::fstream::failbit);
      UnknownHeaderLabel.exceptions(std::fstream::failbit);
         /*
            //---------------------------------------------------------------------
            msg_desc = "BadEpochLine test";
            try
            {
            BadEpochLine >> ch;
            TUFAIL(msg_desc + msg_expect + msg_falsePass);
            }
            catch(gpstk::Exception e)
            {
            TUPASS(msg_desc + msg_expect);
            }
            catch(...)
            {
            TUFAIL(msg_desc + msg_expect + msg_trueFail);
            }
         */
         //--------------------------------------------------------------------

#ifdef RINEX_3_CLOCK_ACTUALLY_IMPLEMENTED
      msg_desc = "IncompleteHeader test";
      try
      {
         IncompleteHeader >> ch; //Not valid but doesn't throw errors
         TUFAIL(msg_desc + msg_expect + msg_falsePass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_desc + msg_expect);
      }
      catch(...)
      {
         TUFAIL(msg_desc + msg_expect + msg_trueFail);
      }
#endif

         //--------------------------------------------------------------------

      msg_desc = "InvalidLineLength test";
      try
      {
         InvalidLineLength >> ch;
         TUFAIL(msg_desc + msg_expect + msg_falsePass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_desc + msg_expect);
      }
      catch(...)
      {
         TUFAIL(msg_desc + msg_expect + msg_trueFail);
      }

         //--------------------------------------------------------------------

      msg_desc = "NotAClockFile test";
      try
      {
         NotAClockFile >> ch;
         TUFAIL(msg_desc + msg_expect + msg_falsePass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_desc + msg_expect);
      }
      catch(...)
      {
         TUFAIL(msg_desc + msg_expect + msg_trueFail);
      }

         //--------------------------------------------------------------------

      msg_desc = "UnknownHeaderLabel test";
      try
      {
         UnknownHeaderLabel >> ch;
         TUFAIL(msg_desc + msg_expect + msg_falsePass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_desc + msg_expect);
      }
      catch(...)
      {
         TUFAIL(msg_desc + msg_expect + msg_trueFail);
      }


   }
   catch(gpstk::Exception e)
   {
      TUFAIL("Error thrown when running headerExceptionTest: "+e.what());
   }
   catch(...)
   {
      TUFAIL("Unknown error thrown when running headerExceptionTest");
   }

   return testFramework.countFails();
}

int Rinex3Clock_T::dataExceptionTest(void)
{
   TUDEF("Rinex3ClockStream", "DataExceptions");

   std::string msg_desc                 = "";
   std::string msg_expect               = ", should throw gpstk::Exception";
   std::string msg_falsePass    = " but threw no exception.";
   std::string msg_trueFail     = " but instead threw an unknown exception";

   try
   {
      gpstk::Rinex3ClockStream BadEpochLine(dataBadEpochLine.c_str());

      gpstk::Rinex3ClockData cd;

      msg_desc = "BadEpochLine test";
      try
      {
         while (BadEpochLine >> cd);
         TUFAIL(msg_desc + msg_expect + msg_falsePass);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(msg_desc + msg_expect);
      }
      catch(...)
      {
         TUFAIL(msg_desc + msg_expect + msg_trueFail);
      }

   }
   catch(gpstk::Exception e)
   {
      TUFAIL("Error thrown when running dataExceptionTest: "+e.what());
   }
   catch(...)
   {
      TUFAIL("Unknown error thrown when running dataExceptionTest");
   }

   return testFramework.countFails();
}

//Test that reading/writing out the file doesn't change it
int Rinex3Clock_T::hardCodeTest(void)
{
   TUDEF("Rinex3ClockStream", "Read & write to file");

   gpstk::Rinex3ClockHeader ch;
   gpstk::Rinex3ClockData cd;

   try
   {
      gpstk::Rinex3ClockStream inputStream(dataRinexClockFile.c_str());
      TUPASS(testMesg);
   }
   catch (gpstk::Exception e)
   {
      testMesg = "Input stream failed to open: " + e.what();
      TUFAIL(testMesg);
   }

   try
   {
      gpstk::Rinex3ClockStream outputStream(dataTestOutput.c_str());
      TUPASS(testMesg);
   }
   catch (gpstk::Exception e)
   {
      testMesg = "Output stream failed to open: " + e.what();
      TUFAIL(testMesg);
   }

   gpstk::Rinex3ClockStream inputStream;
   gpstk::Rinex3ClockStream outputStream;

   try
   {
      inputStream.open(dataRinexClockFile.c_str(), std::ios::in);
      outputStream.open(dataTestOutput.c_str(), std::ios::out);
      inputStream >> ch;
         // ch.dump(std::cout);
      outputStream << ch;
      while(inputStream >> cd)
      {
            // cd.dump(std::cout);
         outputStream << cd;
      }

      TUPASS(testMesg);
   }
   catch (gpstk::Exception e)
   {
      testMesg = "Unable to read/write to file stream: " + e.what();
      TUFAIL(testMesg);
   }

   fileEqual = false;

   int skipLines = 2; //First two lines of the header are not supposed to match

   fileEqual = testFramework.fileEqualTest(dataRinexClockFile,
                                           dataTestOutput, skipLines);

   testMesg = "Files are not consistent after input & out";
   testFramework.assert(fileEqual, testMesg, __LINE__);

   return testFramework.countFails();
}


int main(void) //Main function to initialize and run all tests above
{
   Rinex3Clock_T testClass;
   int check, errorCounter = 0;

   testClass.init();

   check = testClass.headerExceptionTest();
   errorCounter += check;

#ifdef RINEX_3_CLOCK_ACTUALLY_IMPLEMENTED
   check = testClass.dataExceptionTest();
   errorCounter += check;

   check = testClass.hardCodeTest();
   errorCounter += check;
#endif

      //Run all tests for Rinex v.3 files
   testClass.toRinex3();

   check = testClass.headerExceptionTest();
   errorCounter += check;

#ifdef RINEX_3_CLOCK_ACTUALLY_IMPLEMENTED
   check = testClass.dataExceptionTest();
   errorCounter += check;

   check = testClass.hardCodeTest();
   errorCounter += check;
#endif

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter
             << std::endl;

   return errorCounter; //Return the total number of errors
}
