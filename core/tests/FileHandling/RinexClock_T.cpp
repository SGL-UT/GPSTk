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

#include "RinexClockBase.hpp"
#include "RinexClockData.hpp"
#include "RinexClockStream.hpp"
#include "RinexClockHeader.hpp"

#include "build_config.h"

#include "TestUtil.hpp"
#include <iostream>

using namespace gpstk;

class RinexClock_T
{
public:
   RinexClock_T();

   void init();

      /// Check that exceptions are thrown/not thrown for invalid/valid headers
   int headerExceptionTest();
      /** Check that reading a file and writing it back out results in
       * identical files.
       * @note This is not a *true* round trip, as the source file may
       * have numbers with leading zeroes that are absent in the
       * output and vice versa.  A different reference file is used
       * that has consistent leading zero behavior, which is not
       * required for the input file, but is required for the test. */
   int roundTripTest();
      /** Check that data with an invalid epoch will cause an
       * exception when exceptions are enabled. */
   int dataExceptionTest();

   std::string dataRinexClockFile;
   std::string dataRinexClockRef;
   std::string dataBadEpochLine;
   std::string dataIncompleteHeader;
   std::string dataInvalidLineLength;
   std::string dataNotAClockFile;
   std::string dataUnknownHeaderLabel;

   std::string dataTestOutput;
   std::string dataRoundTripOutput;
};


RinexClock_T ::
RinexClock_T()
{
   init();
}


void RinexClock_T ::
init()
{
   TestUtil test0;
   std::string iPath = gpstk::getPathData() + getFileSep();
   std::string oPath = gpstk::getPathTestTemp() + getFileSep();

   dataRinexClockFile     = iPath +
      "test_input_rinex2_clock_RinexClockExample.96c";
      // This file is the same *content* as dataRinexClockFile, except
      // that the formatting of numbers has been made more consistent.
      // The example file may sometimes have leading zeroes on numbers
      // but not always.
   dataRinexClockRef      = iPath +
      "test_input_rinex2_clock_RinexClockReference.96c";
   dataBadEpochLine       = iPath + "test_input_rinex2_clock_BadEpochLine.96c";
   dataIncompleteHeader   = iPath +
      "test_input_rinex2_clock_IncompleteHeader.96c";
   dataInvalidLineLength  = iPath +
      "test_input_rinex2_clock_InvalidLineLength.96c";
   dataNotAClockFile      = iPath + "test_input_rinex2_clock_NotAClockFile.96c";
   dataUnknownHeaderLabel = iPath +
      "test_input_rinex2_clock_UnknownHeaderLabel.96c";

   dataTestOutput         = oPath + "test_output_rinex_clock_TestOutput.96o";
   dataRoundTripOutput    = oPath + "test_output_rinex_clock_RoundTripOutput.96o";
}


   /* What the hell are we doing here?
    *
    * 1) Doing permissive reads of error-ridden RINEX OBS headers,
    *    i.e. allowing the headers to be read into memory despite errors.
    *    This is done by leaving the default behavior of streams that no
    *    exceptions are thrown on error conditions.
    *
    * 2) Doing strict writes of same error-ridden headers and verifying
    *    that exceptions are being thrown as expected.
    */
int RinexClock_T ::
headerExceptionTest()
{
   TUDEF( "RinexClockStream", "operator<<" );

   try
   {
      gpstk::RinexClockStream
         rinexClockFile( dataRinexClockFile.c_str() ),
         ih( dataIncompleteHeader.c_str() ),
         il( dataInvalidLineLength.c_str() ),
         no( dataNotAClockFile.c_str() ),
         uh( dataUnknownHeaderLabel.c_str() ),
         out( dataTestOutput.c_str(), std::ios::out );
      gpstk::RinexClockHeader
         rinexClockHeader,
         ihh,
         ilh,
         noh,
         uhh;
      gpstk::RinexClockData rinexClockData;

         // read in some good and some bad headers
      rinexClockFile >> rinexClockHeader;
      ih >> ihh;
      il >> ilh;
      no >> noh;
      uh >> uhh;

      out.exceptions( std::fstream::failbit );
         // write good and bad headers, checking for exceptions
      try
      {
         out << rinexClockHeader;
         TUPASS("exception");
      }
      catch (...)
      {
         TUFAIL("Exception while writing valid RINEX clock header");
      }
      out.clear();
      try
      {
         out << ihh;
         TUFAIL("No Exception while writing invalid RINEX clock header");
      }
      catch (...)
      {
         TUPASS("exception");
      }
      out.clear();
      try
      {
         out << ilh;
         TUFAIL("No Exception while writing invalid RINEX clock header");
      }
      catch (...)
      {
         TUPASS("exception");
      }
      out.clear();
      try
      {
         out << noh;
         TUFAIL("No Exception while writing invalid RINEX clock header");
      }
      catch (...)
      {
         TUPASS("exception");
      }
      out.clear();
      try
      {
            // The error in this header will not have made it into the
            // data structure, being an invalid header line, thus
            // output is expected to succeed in this case.
         out << uhh;
         TUPASS("exception");
      }
      catch (...)
      {
         TUFAIL("Exception while writing valid(ish) RINEX clock header");
      }
      out.clear();
      while( rinexClockFile >> rinexClockData )
      {
         out << rinexClockData;
      }
   }
   catch (...)
   {
      TUFAIL("Unanticipated exception caught");
   }

   return testFramework.countFails();
}


int RinexClock_T ::
roundTripTest()
{
   TUDEF( "RinexClockData", "operator>>" );
   int numLinesSkip = 0;
   try
   {
      gpstk::RinexClockStream
         rinexClockFile( dataRinexClockFile.c_str() ),
         out( dataRoundTripOutput.c_str(), std::ios::out );
      gpstk::RinexClockHeader rinexClockHeader;
      gpstk::RinexClockData rinexClockData;

      rinexClockFile.exceptions(std::fstream::failbit);
      out.exceptions(std::fstream::failbit);

      rinexClockFile >> rinexClockHeader;
      out << rinexClockHeader;

      while (rinexClockFile >> rinexClockData)
      {
         out << rinexClockData;
      }
      rinexClockFile.close();
      out.close();

      testFramework.assert_files_equal(
         __LINE__, dataRinexClockRef, dataRoundTripOutput,
         "files do not match: " + dataRinexClockRef + " " + dataRoundTripOutput,
         numLinesSkip, false, true );
   }
   catch (...)
   {
      TUFAIL("Caught unanticipated exception");
   }

   return testFramework.countFails();
}


int RinexClock_T::dataExceptionTest()
{
   TUDEF("RinexClockStream", "DataExceptions");

   std::string msg_desc                 = "";
   std::string msg_expect               = ", should throw gpstk::Exception";
   std::string msg_falsePass    = " but threw no exception.";
   std::string msg_trueFail     = " but instead threw an unknown exception";

   try
   {
      gpstk::RinexClockStream badEpochLine(dataBadEpochLine.c_str());
      badEpochLine.exceptions(std::fstream::failbit);
      gpstk::RinexClockData cd;

      msg_desc = "BadEpochLine test";
      try
      {
         while (badEpochLine >> cd);
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


int main() //Main function to initialize and run all tests above
{
   RinexClock_T testClass;
   int errorTotal = 0;

   errorTotal += testClass.headerExceptionTest();
   errorTotal += testClass.roundTripTest();
   errorTotal += testClass.dataExceptionTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal; //Return the total number of errors
}
