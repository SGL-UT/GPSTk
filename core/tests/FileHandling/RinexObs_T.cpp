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

#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsFilterOperators.hpp"

#include "build_config.h"

#include "TestUtil.hpp"
#include <iostream>
#include <string>

using namespace gpstk;

//============================================================
// Class decalarations
//============================================================

class RinexObs_T
{
public:

      // constructor
   RinexObs_T()
   {
      init();
   }

      // return values indicate number of failures, i.e., 0=PASS, !0=FAIL
   void init( void );
   int headerExceptionTest( void );
   int hardCodeTest( void );
   int filterOperatorsTest( void );
   int dataExceptionsTest( void );

private:

   std::string dataFilePath;

   std::string dataRinexObsFile;
   std::string dataIncompleteHeader;
   std::string dataInvalidLineLength;
   std::string dataInvalidNumPRNWaveFact;
   std::string dataNotObs;
   std::string dataSystemGeosync;
   std::string dataSystemGlonass;
   std::string dataSystemMixed;
   std::string dataSystemTransit;
   std::string dataUnSupVersion ;
   std::string dataRinexContData;

   std::string dataBadEpochLine;
   std::string dataBadEpochFlag;
   std::string dataBadLineSize;
   std::string dataInvalidTimeFormat;

   std::string dataFilterTest1;
   std::string dataFilterTest2;
   std::string dataFilterTest3;
   std::string dataFilterTest4;

   std::string dataTestOutput;
   std::string dataTestOutput2;
   std::string dataTestOutput3;
   std::string dataTestOutputObsDump;
   std::string dataTestOutputDataException;
   std::string dataTestFilterOutput;
};

//============================================================
// Initialize Test Data Filenames
//============================================================

void RinexObs_T :: init( void )
{

   TestUtil test0;
   std::string dataFilePath = gpstk::getPathData();
   std::string tempFilePath = gpstk::getPathTestTemp
      ();

      //----------------------------------------
      // Full file paths
      //----------------------------------------
   std::string file_sep = getFileSep();

   dataRinexObsFile            = dataFilePath + file_sep +
                                 "test_input_rinex_obs_RinexObsFile.06o";
   dataIncompleteHeader        = dataFilePath + file_sep +
                                 "test_input_rinex_obs_IncompleteHeader.06o";
   dataInvalidLineLength       = dataFilePath + file_sep +
                                 "test_input_rinex_obs_InvalidLineLength.06o";
   dataInvalidNumPRNWaveFact   = dataFilePath + file_sep +
                                 "test_input_rinex_obs_InvalidNumPRNWaveFact.06o";
   dataNotObs                  = dataFilePath + file_sep +
                                 "test_input_rinex_obs_NotObs.06o";
   dataSystemGeosync           = dataFilePath + file_sep +
                                 "test_input_rinex_obs_SystemGeosync.06o";
   dataSystemGlonass           = dataFilePath + file_sep +
                                 "test_input_rinex_obs_SystemGlonass.06o";
   dataSystemMixed             = dataFilePath + file_sep +
                                 "test_input_rinex_obs_SystemMixed.06o";
   dataSystemTransit           = dataFilePath + file_sep +
                                 "test_input_rinex_obs_SystemTransit.06o";
   dataUnSupVersion            = dataFilePath + file_sep +
                                 "test_input_rinex_obs_UnSupVersion.06o";
   dataRinexContData           = dataFilePath + file_sep +
                                 "test_input_rinex_obs_RinexContData.06o";

   dataBadEpochLine            = dataFilePath + file_sep +
                                 "test_input_rinex_obs_BadEpochLine.06o";
   dataBadEpochFlag            = dataFilePath + file_sep +
                                 "test_input_rinex_obs_BadEpochFlag.06o";
   dataBadLineSize             = dataFilePath + file_sep +
                                 "test_input_rinex_obs_BadLineSize.06o";
   dataInvalidTimeFormat       = dataFilePath + file_sep +
                                 "test_input_rinex_obs_InvalidTimeFormat.06o";

   dataFilterTest1             = dataFilePath + file_sep +
                                 "test_input_rinex_obs_FilterTest1.06o";
   dataFilterTest2             = dataFilePath + file_sep +
                                 "test_input_rinex_obs_FilterTest2.06o";
   dataFilterTest3             = dataFilePath + file_sep +
                                 "test_input_rinex_obs_FilterTest3.06o";
   dataFilterTest4             = dataFilePath + file_sep +
                                 "test_input_rinex_obs_FilterTest4.06o";

   dataTestOutput              = tempFilePath + file_sep +
                                 "test_output_rinex_obs_TestOutput.06o";
   dataTestOutput2             = tempFilePath + file_sep +
                                 "test_output_rinex_obs_TestOutput2.06o";
   dataTestOutput3             = tempFilePath + file_sep +
                                 "test_output_rinex_obs_TestOutput3.06o";
   dataTestOutputObsDump       = tempFilePath + file_sep +
                                 "test_output_rinex_obs_ObsDump.06o";
   dataTestOutputDataException = tempFilePath + file_sep +
                                 "test_output_rinex_obs_DataExceptionOutput.06o";
   dataTestFilterOutput        = tempFilePath + file_sep +
                                 "test_output_rinex_obs_FilterOutput.txt";

}

//============================================================
// Test Method Definitions
//============================================================

//------------------------------------------------------------
// This tests throws many GPSTK RinexObsHeader exceptions including
// Incomplete headers, invalid line lengths etc
// Also an extended obs type is used and dumped within this test.
//------------------------------------------------------------
int RinexObs_T :: headerExceptionTest( void )
{

   TUDEF( "RinexObsStream", "dump" );

   std::string msg_test_desc  = "RinexObsStream, headerExceptionTest";
   std::string msg_false_pass = ", threw the wrong number of exceptions.";
   std::string msg_fail       = ", threw an unexpected exception.";

   try
   {

      gpstk::RinexObsStream RinexObsFile( dataRinexObsFile );
      gpstk::RinexObsStream ih( dataIncompleteHeader );
      gpstk::RinexObsStream il( dataInvalidLineLength );
      gpstk::RinexObsStream inpwf( dataInvalidNumPRNWaveFact );
      gpstk::RinexObsStream no( dataNotObs );
      gpstk::RinexObsStream ss( dataSystemGeosync );
      gpstk::RinexObsStream sr( dataSystemGlonass );
      gpstk::RinexObsStream sm( dataSystemMixed );
      gpstk::RinexObsStream st( dataSystemTransit );
      gpstk::RinexObsStream unsupv( dataUnSupVersion );
      gpstk::RinexObsStream contdata( dataRinexContData );

      gpstk::RinexObsStream out( dataTestOutput, std::ios::out );
      gpstk::RinexObsStream out2( dataTestOutput3, std::ios::out );
      gpstk::RinexObsStream dump( dataTestOutputObsDump, std::ios::out );

      gpstk::RinexObsHeader RinexObsFileh;
      gpstk::RinexObsHeader ihh;
      gpstk::RinexObsHeader ilh;
      gpstk::RinexObsHeader inpwfh;
      gpstk::RinexObsHeader noh;
      gpstk::RinexObsHeader ssh;
      gpstk::RinexObsHeader srh;
      gpstk::RinexObsHeader smh;
      gpstk::RinexObsHeader sth;
      gpstk::RinexObsHeader unsupvh;
      gpstk::RinexObsHeader contdatah;

      gpstk::RegisterExtendedRinexObsType( "ER","Testing Type", "Candela",
                                           (unsigned) 2 );
      gpstk::RinexObsData RinexObsFiled;
      gpstk::RinexObsData contdatad;

      RinexObsFile >> RinexObsFileh;
      ih >> ihh;
      il >> ilh;
      inpwf >> inpwfh;
      no >> noh;
      ss >> ssh;
      sr >> srh;
      sm >> smh;
      unsupv >> unsupvh;
      contdata >> contdatah;

      out << RinexObsFileh;
      out << ihh;
      out << ilh;
      out << inpwfh;
      out << noh;
      out << ssh;
      out << srh;
      out << smh;
      out << unsupvh;
      out2 << contdatah;
      RinexObsFile >> RinexObsFiled;
      RinexObsFiled.dump( dump );

      while( RinexObsFile >> RinexObsFiled )
      {
         out << RinexObsFiled;
      }

      while( contdata >> contdatad )
      {
         out2 << contdatad;
      }

      RinexObsFileh.dump( dump );
      contdatah.dump( dump );
      ilh.dump( dump );
      gpstk::DisplayExtendedRinexObsTypes( dump );

      testFramework.assert( 39 == RinexObsFileh.NumberHeaderRecordsToBeWritten(),
                            msg_test_desc + msg_false_pass, __LINE__ );
   }
   catch(...)
   {
      testFramework.assert( false, msg_test_desc + msg_fail, __LINE__ );
   }

   return( testFramework.countFails() );
}


//------------------------------------------------------------
// This test checks to make sure that the output
// from a read in RinexObsFile matches the input.
//------------------------------------------------------------
int RinexObs_T :: hardCodeTest( void )
{

   bool files_equal = false;

      // Previous comments indicated that these Rinex methods
      // are not expected to match in the top two lines of the file
   int num_lines_skip = 2;

   TUDEF( "RinexObsStream", "dump" );

   std::string msg_test_desc   =
      "RinexObsStream, read write test, comparing input file and output file";
   std::string msg_fail_equal  = ", input and output do not match.";
   std::string msg_fail_except = ", threw an unexpected exception.";

   try
   {
      gpstk::RinexObsStream RinexObsFile( dataRinexObsFile );
      gpstk::RinexObsStream out( dataTestOutput2, std::ios::out );
      gpstk::RinexObsStream dump( dataTestOutputObsDump, std::ios::out );
      gpstk::RinexObsHeader RinexObsFileh;
      gpstk::RinexObsData RinexObsFiled;

      RinexObsFile >> RinexObsFileh;
      out << RinexObsFileh;

      while( RinexObsFile >> RinexObsFiled )
      {
         out << RinexObsFiled;
      }

      RinexObsFiled.dump( dump );
      RinexObsFileh.dump( dump );

      files_equal = testFramework.fileEqualTest( dataRinexObsFile, dataTestOutput2,
                    num_lines_skip );
      testFramework.assert( files_equal, msg_test_desc + msg_fail_equal, __LINE__ );
   }
   catch(...)
   {
      testFramework.assert( false, msg_test_desc + msg_fail_except, __LINE__ );
   }

   return( testFramework.countFails() );
}

//------------------------------------------------------------
// This test throws many GPSTK exceptions within the
// RinexObsData, including BadEpochLine and BadEpochFlag
//------------------------------------------------------------
int RinexObs_T :: dataExceptionsTest( void )
{

   TUDEF( "RinexObsStream", "dump" );

   std::string msg_test_desc   =
      "RinexObsStream, test various gpstk exception throws, including BadEpochLine and BadEpochFlag";
   std::string msg_fail_throw  =
      ", not all gpstk exceptions were thrown as expected.";
   std::string msg_fail_except = ", threw an unexpected exception.";

   try
   {
      gpstk::RinexObsStream BadEpochLine( dataBadEpochLine );
      gpstk::RinexObsStream BadEpochFlag( dataBadEpochFlag );
      gpstk::RinexObsStream BadLineSize( dataBadLineSize );
      gpstk::RinexObsStream InvalidTimeFormat( dataInvalidTimeFormat );
      gpstk::RinexObsStream out( dataTestOutputDataException, std::ios::out );
      gpstk::RinexObsData BadEpochLined;
      gpstk::RinexObsData BadEpochFlagd;
      gpstk::RinexObsData BadLineSized;
      gpstk::RinexObsData InvalidTimeFormatd;

      while( BadEpochLine >> BadEpochLined )
      {
         out << BadEpochLined;
      }
      while( BadEpochFlag >> BadEpochFlagd )
      {
         out << BadEpochFlagd;
      }
      while( BadLineSize >> BadLineSized )
      {
         out << BadLineSized;
      }
      while( InvalidTimeFormat >> InvalidTimeFormatd )
      {
         out << InvalidTimeFormatd;
      }
      testFramework.assert( true, msg_test_desc + msg_fail_throw, __LINE__ );
   }
   catch(...)
   {
      testFramework.assert( false, msg_test_desc + msg_fail_except, __LINE__ );
   }

   return( testFramework.countFails() );

}

//------------------------------------------------------------
// This is the test for several of the members within RinexObsFilterOperators
// including merge, LessThanSimple, EqualsSimple, and LessThanFull.
//------------------------------------------------------------
int RinexObs_T :: filterOperatorsTest( void )
{

   TUDEF( "RinexObsStream", "open" );

   std::string msg_test_desc = "";

   try
   {
      gpstk::RinexObsStream FilterStream1( dataFilterTest1 );
      FilterStream1.open( dataFilterTest1, std::ios::in );

      gpstk::RinexObsStream FilterStream2( dataFilterTest2  );
      gpstk::RinexObsStream FilterStream3( dataFilterTest3  );
      gpstk::RinexObsStream FilterStream4( dataFilterTest4  );
      gpstk::RinexObsStream out( dataTestFilterOutput, std::ios::out );

      gpstk::RinexObsHeader FilterHeader1;
      gpstk::RinexObsHeader FilterHeader2;
      gpstk::RinexObsHeader FilterHeader3;
      gpstk::RinexObsHeader FilterHeader4;

      gpstk::RinexObsData FilterData1;
      gpstk::RinexObsData FilterData2;
      gpstk::RinexObsData FilterData3;
      gpstk::RinexObsData FilterData4;
      gpstk::RinexObsData rodata;


      FilterStream1 >> FilterHeader1;
      FilterStream2 >> FilterHeader2;
      FilterStream3 >> FilterHeader3;
      FilterStream4 >> FilterHeader4;

      while( FilterStream1 >> rodata)
      {
         FilterData1 = rodata;
      }
      while( FilterStream2 >> rodata)
      {
         FilterData2 = rodata;
      }
      while( FilterStream3 >> rodata)
      {
         FilterData3 = rodata;
      }
      while( FilterStream4 >> rodata)
      {
         FilterData4 = rodata;
      }

      gpstk::RinexObsHeaderTouchHeaderMerge merged;
      merged( FilterHeader1 );
      merged( FilterHeader2 );

      gpstk::RinexObsDataOperatorLessThanFull( merged.obsSet );
      out << merged.theHeader;

      gpstk::RinexObsDataOperatorEqualsSimple EqualsSimple;
      msg_test_desc =
         "RinexObsDataOperatorEqualsSimple( FilterData1, FilterData1 ), should evaluate as true";
      testFramework.assert( EqualsSimple( FilterData1, FilterData1 ), msg_test_desc,
                            __LINE__ );

      gpstk::RinexObsDataOperatorLessThanSimple LessThanSimple;
      msg_test_desc =
         "RinexObsDataOperatorLessThanSimple( FilterData1, FilterData1 ) should evaluated as false";
      testFramework.assert( !LessThanSimple( FilterData1, FilterData1 ),
                            msg_test_desc, __LINE__ );

      gpstk::RinexObsDataOperatorLessThanFull LessThanFull( merged.obsSet );
      msg_test_desc =
         "RinexObsDataOperator LessThanFull( FilterData1, FilterData1 ) should evaluate as false ";
      testFramework.assert( !LessThanFull( FilterData1, FilterData1 ) ,
                            msg_test_desc, __LINE__ );

      msg_test_desc =
         " RinexObsDataOperator LessThanFull( FilterData1, FilterData2 ) should evaluate as false ";
      testFramework.assert( !LessThanFull( FilterData1, FilterData2 ) ,
                            msg_test_desc, __LINE__ );

   }
   catch(...)
   {
      msg_test_desc =
         "One or more of the tests for RinexObsDataOperator LessThanFull threw an exception when it should not have";
      testFramework.assert( false, msg_test_desc, __LINE__ );
   }

   return( testFramework.countFails() );

}

//============================================================
// Run all the test methods defined above
//============================================================

int main()
{
   int errorTotal = 0;
   RinexObs_T testClass;

   errorTotal += testClass.headerExceptionTest();
   errorTotal += testClass.hardCodeTest();
   errorTotal += testClass.dataExceptionsTest();
   errorTotal += testClass.filterOperatorsTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return( errorTotal );
}
