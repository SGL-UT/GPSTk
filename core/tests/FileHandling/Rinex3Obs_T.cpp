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
// This software developed by Applied Research Laboratories at the University of
// Texas at Austin, under contract to an agency or agencies within the U.S.
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

#include "Rinex3ObsBase.hpp"
#include "Rinex3ObsData.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsFilterOperators.hpp"

#include "build_config.h"

#include "TestUtil.hpp"
#include <iostream>
#include <string>

using namespace gpstk;

//============================================================
// Class decalarations
//============================================================

class Rinex3Obs_T
{
    public:

        // constructor
       Rinex3Obs_T()
        {
            init();
        }

        void init( void );

        void toRinex3( void );

        // return values indicate number of failures, i.e., 0=PASS, !0=FAIL
        int headerExceptionTest( void );
        int hardCodeTest( void );
        int filterOperatorsTest( void );
        int dataExceptionsTest( void );

        void toConversionTest( void );
        int version2ToVersion3Test( void );
        int version3ToVersion2Test( void );

    private:

        std::string dataFilePath;
        std::string tempFilePath;
        std::string file_sep;    

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

        std::string dataInputRinex3ObsFile;
        std::string dataInputRinex2ObsFile;
        std::string dataOutputRinex3ObsFile;
        std::string dataOutputRinex2ObsFile;

        std::string testMesg;
};

//============================================================
// Initialize Test Data Filenames
//============================================================

void Rinex3Obs_T :: init( void )
{

    std::cout<<"Running tests for Rinex version 2.0"<<std::endl;

    TestUtil test0;
    dataFilePath = test0.getDataPath();
    tempFilePath = test0.getTempPath();

    //---------------------------------------- 
    // Full file paths
    //---------------------------------------- 
    file_sep = getFileSep();

    dataRinexObsFile            = dataFilePath + file_sep + "test_input_rinex2_obs_RinexObsFile.06o";
    dataIncompleteHeader        = dataFilePath + file_sep + "test_input_rinex2_obs_IncompleteHeader.06o";
    dataInvalidLineLength       = dataFilePath + file_sep + "test_input_rinex2_obs_InvalidLineLength.06o";
    dataInvalidNumPRNWaveFact   = dataFilePath + file_sep + "test_input_rinex2_obs_InvalidNumPRNWaveFact.06o";
    dataNotObs                  = dataFilePath + file_sep + "test_input_rinex2_obs_NotObs.06o";
    dataSystemGeosync           = dataFilePath + file_sep + "test_input_rinex2_obs_SystemGeosync.06o";
    dataSystemGlonass           = dataFilePath + file_sep + "test_input_rinex2_obs_SystemGlonass.06o";
    dataSystemMixed             = dataFilePath + file_sep + "test_input_rinex2_obs_SystemMixed.06o";
    dataSystemTransit           = dataFilePath + file_sep + "test_input_rinex2_obs_SystemTransit.06o";
    dataUnSupVersion            = dataFilePath + file_sep + "test_input_rinex2_obs_UnSupVersion.06o";
    dataRinexContData           = dataFilePath + file_sep + "test_input_rinex2_obs_RinexContData.06o";   // not in v3 test

    dataBadEpochLine            = dataFilePath + file_sep + "test_input_rinex2_obs_BadEpochLine.06o";
    dataBadEpochFlag            = dataFilePath + file_sep + "test_input_rinex2_obs_BadEpochFlag.06o";
    dataBadLineSize             = dataFilePath + file_sep + "test_input_rinex2_obs_BadLineSize.06o";
    dataInvalidTimeFormat       = dataFilePath + file_sep + "test_input_rinex2_obs_InvalidTimeFormat.06o";

    dataFilterTest1             = dataFilePath + file_sep + "test_input_rinex2_obs_FilterTest1.06o";
    dataFilterTest2             = dataFilePath + file_sep + "test_input_rinex2_obs_FilterTest2.06o";
    dataFilterTest3             = dataFilePath + file_sep + "test_input_rinex2_obs_FilterTest3.06o";
    dataFilterTest4             = dataFilePath + file_sep + "test_input_rinex2_obs_FilterTest4.06o";

    dataTestOutput              = tempFilePath + file_sep + "test_output_rinex2_obs_TestOutput.06o";
    dataTestOutput2             = tempFilePath + file_sep + "test_output_rinex2_obs_TestOutput2.06o";
    dataTestOutput3             = tempFilePath + file_sep + "test_output_rinex2_obs_TestOutput3.06o";
    dataTestOutputObsDump       = tempFilePath + file_sep + "test_output_rinex2_obs_ObsDump.06o";
    dataTestOutputDataException = tempFilePath + file_sep + "test_output_rinex2_obs_DataExceptionOutput.06o";
    dataTestFilterOutput        = tempFilePath + file_sep + "test_output_rinex2_obs_FilterOutput.txt";

}

//============================================================
// Change input and output file names for Rinex v.3 types
//============================================================

void Rinex3Obs_T :: toRinex3(void)
{

    std::cout<<"Running tests for Rinex version 3.0"<<std::endl;

    //---------------------------------------- 
    // Full file paths
    //---------------------------------------- 

    dataRinexObsFile            = dataFilePath + file_sep + "test_input_rinex3_obs_RinexObsFile.15o";
    dataIncompleteHeader        = dataFilePath + file_sep + "test_input_rinex3_obs_IncompleteHeader.15o";
    dataInvalidLineLength       = dataFilePath + file_sep + "test_input_rinex3_obs_InvalidLineLength.15o";
    dataInvalidNumPRNWaveFact   = dataFilePath + file_sep + "test_input_rinex3_obs_InvalidNumPRNWaveFact.15o";
    dataNotObs                  = dataFilePath + file_sep + "test_input_rinex3_obs_NotObs.15o";
    dataSystemGeosync           = dataFilePath + file_sep + "test_input_rinex3_obs_SystemGeosync.15o";
    dataSystemGlonass           = dataFilePath + file_sep + "test_input_rinex3_obs_SystemGlonass.15o";
    dataSystemMixed             = dataFilePath + file_sep + "test_input_rinex3_obs_SystemMixed.15o";
    dataSystemTransit           = dataFilePath + file_sep + "test_input_rinex3_obs_SystemTransit.15o";
    dataUnSupVersion            = dataFilePath + file_sep + "test_input_rinex3_obs_UnSupVersion.15o";
    dataRinexContData           = dataFilePath + file_sep + "test_input_rinex3_obs_RinexContData.15o";

    dataBadEpochLine            = dataFilePath + file_sep + "test_input_rinex3_obs_BadEpochLine.15o";
    dataBadEpochFlag            = dataFilePath + file_sep + "test_input_rinex3_obs_BadEpochFlag.15o";
    dataBadLineSize             = dataFilePath + file_sep + "test_input_rinex3_obs_BadLineSize.15o";
    dataInvalidTimeFormat       = dataFilePath + file_sep + "test_input_rinex3_obs_InvalidTimeFormat.15o";

    dataFilterTest1             = dataFilePath + file_sep + "test_input_rinex3_obs_FilterTest1.15o";
    dataFilterTest2             = dataFilePath + file_sep + "test_input_rinex3_obs_FilterTest2.15o";
    dataFilterTest3             = dataFilePath + file_sep + "test_input_rinex3_obs_FilterTest3.15o";
    dataFilterTest4             = dataFilePath + file_sep + "test_input_rinex3_obs_FilterTest4.15o";

    dataTestOutput              = tempFilePath + file_sep + "test_output_rinex3_obs_TestOutput.15o";
    dataTestOutput2             = tempFilePath + file_sep + "test_output_rinex3_obs_TestOutput2.15o";
    dataTestOutput3             = tempFilePath + file_sep + "test_output_rinex3_obs_TestOutput3.15o";
    dataTestOutputObsDump       = tempFilePath + file_sep + "test_output_rinex3_obs_ObsDump.15o";
    dataTestOutputDataException = tempFilePath + file_sep + "test_output_rinex3_obs_DataExceptionOutput.15o";
    dataTestFilterOutput        = tempFilePath + file_sep + "test_output_rinex3_obs_FilterOutput.txt";

}

//=============================================================
// Change input and output file names for Rinex Conversion test
//=============================================================

void Rinex3Obs_T :: toConversionTest( void )
{
    dataInputRinex3ObsFile  = dataFilePath + file_sep + "test_input_rinex3_obs_RinexObsFile.15o";
    dataInputRinex2ObsFile  = dataFilePath + file_sep + "test_input_rinex2_obs_RinexObsFile.06o";

    dataOutputRinex3ObsFile = tempFilePath + file_sep + "test_output_rinex3_obs_Rinex2to3Output.06o";
    dataOutputRinex2ObsFile = tempFilePath + file_sep + "test_output_rinex2_obs_Rinex3to2Output.15o";
}

//============================================================
// Test Method Definitions
//============================================================

//------------------------------------------------------------
// This tests throws many GPSTK Rinex3ObsHeader exceptions including
// Incomplete headers, invalid line lengths etc
// Also an extended obs type is used and dumped within this test.
//------------------------------------------------------------
int Rinex3Obs_T :: headerExceptionTest( void )
{

    TestUtil test1( "Rinex3ObsStream", "dump", __FILE__, __LINE__ );

    std::string msg_test_desc  = "Rinex3ObsStream, headerExceptionTest";
    std::string msg_false_pass = ", threw the wrong number of exceptions.";
    std::string msg_fail       = ", threw an unexpected exception.";
    
    try
    {

        gpstk::Rinex3ObsStream Rinex3ObsFile( dataRinexObsFile );
        gpstk::Rinex3ObsStream ih( dataIncompleteHeader );
        gpstk::Rinex3ObsStream il( dataInvalidLineLength );
        gpstk::Rinex3ObsStream inpwf( dataInvalidNumPRNWaveFact );
        gpstk::Rinex3ObsStream no( dataNotObs );
        gpstk::Rinex3ObsStream ss( dataSystemGeosync );
        gpstk::Rinex3ObsStream sr( dataSystemGlonass );
        gpstk::Rinex3ObsStream sm( dataSystemMixed );
        gpstk::Rinex3ObsStream st( dataSystemTransit );
        gpstk::Rinex3ObsStream unsupv( dataUnSupVersion );
        gpstk::Rinex3ObsStream contdata( dataRinexContData );

        gpstk::Rinex3ObsStream out( dataTestOutput, std::ios::out );
        gpstk::Rinex3ObsStream out2( dataTestOutput3, std::ios::out );
        gpstk::Rinex3ObsStream dump( dataTestOutputObsDump, std::ios::out );

        gpstk::Rinex3ObsHeader Rinex3ObsFileh;
        gpstk::Rinex3ObsHeader ihh;
        gpstk::Rinex3ObsHeader ilh;
        gpstk::Rinex3ObsHeader inpwfh;
        gpstk::Rinex3ObsHeader noh;
        gpstk::Rinex3ObsHeader ssh;
        gpstk::Rinex3ObsHeader srh;
        gpstk::Rinex3ObsHeader smh;
        gpstk::Rinex3ObsHeader sth;
        gpstk::Rinex3ObsHeader unsupvh;
        gpstk::Rinex3ObsHeader contdatah;

        //Unneccesary, ObsID has it's own test
        // gpstk::RegisterExtendedRinexObsType( "ER","Testing Type", "Candela", (unsigned) 2 );
        // gpstk::ObsID testID = gpstk::ObsID::newID("ER", "Testing Type" /* No way to set Unit type or depend(whatever depend is)*/);
        gpstk::Rinex3ObsData Rinex3ObsFiled;
        gpstk::Rinex3ObsData contdatad;

        int HeaderRecordNumber;

        Rinex3ObsFile >> Rinex3ObsFileh;
        ih >> ihh;
        il >> ilh;
        inpwf >> inpwfh;
        no >> noh;
        ss >> ssh;
        sr >> srh;
        sm >> smh;
        unsupv >> unsupvh;
        contdata >> contdatah; // not in v3 test

        out << Rinex3ObsFileh;
        out << ihh;
        out << ilh;
        out << inpwfh;
        out << noh;
        out << ssh;
        out << srh;
        out << smh;
        out << unsupvh;
        out2 << contdatah;  // not in v3 test
        Rinex3ObsFile >> Rinex3ObsFiled;
        Rinex3ObsFiled.dump( dump );

        while( Rinex3ObsFile >> Rinex3ObsFiled )
        {
            out << Rinex3ObsFiled; // Outputting v.3 data instead of v.2
        }

        while( contdata >> contdatad )  // not in v3 test
        {
            out2 << contdatad;
        }


        Rinex3ObsFileh.dump( dump );
        contdatah.dump( dump );  // not in v3 test
        ilh.dump( dump );
        // gpstk::DisplayExtendedRinexObsTypes( dump );
        // testID.dump( dump );

        // std::cout<<Rinex3ObsFileh.NumberHeaderRecordsToBeWritten()<<std::endl;

        if ( Rinex3ObsFileh.version == 2.1 ) HeaderRecordNumber = 40;
        if ( Rinex3ObsFileh.version > 3 ) HeaderRecordNumber = 30; 

        test1.assert( HeaderRecordNumber == Rinex3ObsFileh.NumberHeaderRecordsToBeWritten(), msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch(gpstk::Exception e)
    {
        test1.assert( false, msg_test_desc + msg_fail + e.what(), __LINE__ );
    }
    catch(...)
    {
        test1.assert( false, msg_test_desc + msg_fail, __LINE__ );
    }

    return( test1.countFails() );
}


//------------------------------------------------------------
// This test checks to make sure that the output
// from a read in Rinex3ObsFile matches the input.
//------------------------------------------------------------
int Rinex3Obs_T :: hardCodeTest( void )
{

    bool files_equal = false;
    double CompareVersion;
    std::string CompareFileProgram;
    std::string CompareFileAgency;
    std::string CompareDate;

    // Previous comments indicated that these Rinex methods
    // are not expected to match in the top two lines of the file
    int num_lines_skip = 2;

    TestUtil test2( "Rinex3ObsStream", "dump", __FILE__, __LINE__ );

    std::string msg_test_desc   = "Rinex3ObsStream, read write test, comparing input file and output file";
    std::string msg_fail_equal  = ", input and output do not match.";
    std::string msg_fail_except = ", threw an unexpected exception.";

    try
    {
        gpstk::Rinex3ObsStream Rinex3ObsFile( dataRinexObsFile );
        gpstk::Rinex3ObsStream out( dataTestOutput2, std::ios::out );
        gpstk::Rinex3ObsStream dump( dataTestOutputObsDump, std::ios::out );
        gpstk::Rinex3ObsHeader Rinex3ObsFileh;
        gpstk::Rinex3ObsData Rinex3ObsFiled;

        Rinex3ObsFile >> Rinex3ObsFileh;
        out << Rinex3ObsFileh;

        while( Rinex3ObsFile >> Rinex3ObsFiled )
        {
            out << Rinex3ObsFiled;
            // std::cout<<out.header.version<<std::endl; stream has header info passed to it
        }

        if (Rinex3ObsFileh.version == 2.1)
        {
          CompareVersion = 2.10;
          CompareFileProgram = (std::string)"row";
          CompareFileAgency = (std::string)"Dataflow Processing";
          CompareDate = (std::string)"04/11/2006 23:59:18";
        }

        else if (Rinex3ObsFileh.version == 3.02)
        {
          CompareVersion = 3.02;
          CompareFileProgram = (std::string)"cnvtToRINEX 2.25.0";
          CompareFileAgency = (std::string)"convertToRINEX OPR";
          CompareDate = (std::string)"23-Jan-15 22:34 UTC";
        }

        test2.assert( Rinex3ObsFileh.version == CompareVersion,                  "RinexObs Header version comparison",      __LINE__ );
        test2.assert( Rinex3ObsFileh.fileProgram == CompareFileProgram,          "RinexObs Header file program comparison", __LINE__ );
        test2.assert( Rinex3ObsFileh.fileAgency == CompareFileAgency,            "RinexObs Header file agency comparison",  __LINE__ );
        test2.assert( Rinex3ObsFileh.date == CompareDate,                        "RinexObs Header date comparison",         __LINE__ );

        Rinex3ObsFiled.dump( dump );
        Rinex3ObsFileh.dump( dump );

        files_equal = test2.fileEqualTest( dataRinexObsFile, dataTestOutput2, num_lines_skip );
        test2.assert( files_equal, msg_test_desc + msg_fail_equal, __LINE__ );
    }
    catch(...)
    {
      test2.assert( false, msg_test_desc + msg_fail_except, __LINE__ );
    }

     return( test2.countFails() );
}

//------------------------------------------------------------
// This test throws many GPSTK exceptions within the
// Rinex3ObsData, including BadEpochLine and BadEpochFlag
//------------------------------------------------------------
int Rinex3Obs_T :: dataExceptionsTest( void )
{

    TestUtil test3( "Rinex3ObsStream", "dump", __FILE__, __LINE__ );

    std::string msg_test_desc   = "Rinex3ObsStream, test various gpstk exception throws, including BadEpochLine and BadEpochFlag";
    std::string msg_fail_throw  = ", not all gpstk exceptions were thrown as expected.";
    std::string msg_fail_except = ", threw an unexpected exception.";

    try
    {
        gpstk::Rinex3ObsStream BadEpochLine( dataBadEpochLine );
        gpstk::Rinex3ObsStream BadEpochFlag( dataBadEpochFlag );
        gpstk::Rinex3ObsStream BadLineSize( dataBadLineSize );
        gpstk::Rinex3ObsStream InvalidTimeFormat( dataInvalidTimeFormat );
        gpstk::Rinex3ObsStream out( dataTestOutputDataException, std::ios::out );
        gpstk::Rinex3ObsData BadEpochLined;
        gpstk::Rinex3ObsData BadEpochFlagd;
        gpstk::Rinex3ObsData BadLineSized;
        gpstk::Rinex3ObsData InvalidTimeFormatd;

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
        test3.assert( true, msg_test_desc + msg_fail_throw, __LINE__ );
    }
    catch(...)
    {
        test3.assert( false, msg_test_desc + msg_fail_except, __LINE__ );
    }

    return( test3.countFails() );

}

//------------------------------------------------------------
// This is the test for several of the members within Rinex3ObsFilterOperators
// including merge, LessThanSimple, EqualsSimple, and LessThanFull.
//------------------------------------------------------------
int Rinex3Obs_T :: filterOperatorsTest( void )
{

    TestUtil test4( "Rinex3ObsStream", "open", __FILE__, __LINE__ );

    std::string msg_test_desc = "";
    
    try
    {
        gpstk::Rinex3ObsStream FilterStream1( dataFilterTest1 );
        FilterStream1.open( dataFilterTest1, std::ios::in );

        gpstk::Rinex3ObsStream FilterStream2( dataFilterTest2  );
        gpstk::Rinex3ObsStream FilterStream3( dataFilterTest3  );
        gpstk::Rinex3ObsStream FilterStream4( dataFilterTest4  );
        gpstk::Rinex3ObsStream out( dataTestFilterOutput, std::ios::out );

        gpstk::Rinex3ObsHeader FilterHeader1;
        gpstk::Rinex3ObsHeader FilterHeader2;
        gpstk::Rinex3ObsHeader FilterHeader3;
        gpstk::Rinex3ObsHeader FilterHeader4;

        gpstk::Rinex3ObsData FilterData1;
        gpstk::Rinex3ObsData FilterData2;
        gpstk::Rinex3ObsData FilterData3;
        gpstk::Rinex3ObsData FilterData4;


        FilterStream1 >> FilterHeader1;
        FilterStream2 >> FilterHeader2;
        FilterStream3 >> FilterHeader3;
        FilterStream4 >> FilterHeader4;

        while( FilterStream1 >> FilterData1)
        {
        }
        while( FilterStream2 >> FilterData2)
        {
        }
        while( FilterStream3 >> FilterData3)
        {
        }
        while( FilterStream4 >> FilterData4)
        {
        }

        gpstk::Rinex3ObsHeaderTouchHeaderMerge merged;
        merged( FilterHeader1 );
        merged( FilterHeader2 );

        gpstk::Rinex3ObsDataOperatorLessThanFull( merged.obsSet );
        out << merged.theHeader;

        gpstk::Rinex3ObsDataOperatorEqualsSimple EqualsSimple;
        msg_test_desc = "Rinex3ObsDataOperatorEqualsSimple( FilterData1, FilterData1 ), should evaluate as true";
        test4.assert( EqualsSimple( FilterData1, FilterData1 ), msg_test_desc, __LINE__ );

        gpstk::Rinex3ObsDataOperatorLessThanSimple LessThanSimple;
        msg_test_desc = "Rinex3ObsDataOperatorLessThanSimple( FilterData1, FilterData1 ) should evaluated as false";
        test4.assert( !LessThanSimple( FilterData1, FilterData1 ), msg_test_desc, __LINE__ );

        gpstk::Rinex3ObsDataOperatorLessThanFull LessThanFull( merged.obsSet );
        msg_test_desc = "Rinex3ObsDataOperator LessThanFull( FilterData1, FilterData1 ) should evaluate as false ";
        test4.assert( !LessThanFull( FilterData1, FilterData1 ) , msg_test_desc, __LINE__ );
        
        msg_test_desc = " Rinex3ObsDataOperator LessThanFull( FilterData1, FilterData2 ) should evaluate as false ";
        test4.assert( !LessThanFull( FilterData1, FilterData2 ) , msg_test_desc, __LINE__ );

    }
    catch(...)
    {
        msg_test_desc = "One or more of the tests for Rinex3ObsDataOperator LessThanFull threw an exception when it should not have";
        test4.assert( false, msg_test_desc, __LINE__ );
    }

    return( test4.countFails() );

}

//------------------------------------------------------------
// Tests if a input Rinex 3 file can be output as a version 2 file
//------------------------------------------------------------

int Rinex3Obs_T :: version3ToVersion2Test( void )
{
    TestUtil testFramework("Rinex3Obs", "Convert v.3 to v.2", __FILE__, __LINE__ );

    gpstk::Rinex3ObsStream inputStream(dataInputRinex3ObsFile.c_str());
    gpstk::Rinex3ObsStream outputStream(dataOutputRinex2ObsFile.c_str(), std::ios::out);
    gpstk::Rinex3ObsHeader ObsHeader;
    gpstk::Rinex3ObsData ObsData;

    inputStream >> ObsHeader;

    ObsHeader.PrepareVer2Write();

    outputStream << ObsHeader;
    while(inputStream >> ObsData)
    {
        outputStream << ObsData;
    }

    testMesg = "No automatic comparison implemented. If " + 
        dataOutputRinex2ObsFile + " is not the proper conversion of " +
        dataInputRinex3ObsFile + "test has failed. Currently PrepareVer2Write() " +
        "function is BROKEN!";

    testFramework.assert(false, testMesg, __LINE__);

    return testFramework.countFails();
}

//------------------------------------------------------------
// Tests if a input Rinex 2 file can be output as a version 3 file
//------------------------------------------------------------

int Rinex3Obs_T :: version2ToVersion3Test( void )
{
    TestUtil testFramework("Rinex3Obs", "Convert v.2 to v.3", __FILE__, __LINE__ );

    gpstk::Rinex3ObsStream inputStream(dataInputRinex2ObsFile.c_str());
    gpstk::Rinex3ObsStream outputStream(dataOutputRinex3ObsFile.c_str(), std::ios::out);
    gpstk::Rinex3ObsHeader ObsHeader;
    gpstk::Rinex3ObsData ObsData;

    inputStream >> ObsHeader;

    ObsHeader.version = 3.02; //No PrepareVersion3Write function, only way to change version number

    outputStream << ObsHeader;
    while(inputStream >> ObsData)
    {
        outputStream << ObsData;
    }

    testMesg = "No automatic comparison implemented. If " + 
        dataOutputRinex2ObsFile + " is not the proper conversion of " +
        dataInputRinex3ObsFile + "test has failed. Currently only conversion " +
        "from v.2.11 to v.3.02 is to change version number by hand. This "
        + "doesn't produce a valid header, so this functionality is MISSING!";

    testFramework.assert(false, testMesg, __LINE__);

    return testFramework.countFails();
}

//============================================================
// Run all the test methods defined above
//============================================================

int main()
{
    int errorCount = 0;
    int errorTotal = 0;
    Rinex3Obs_T testClass;

    errorCount = testClass.headerExceptionTest();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.hardCodeTest();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.dataExceptionsTest();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.filterOperatorsTest();
    errorTotal = errorTotal + errorCount;

    //Change to test v.3
    testClass.toRinex3();

    errorCount = testClass.headerExceptionTest();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.hardCodeTest();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.dataExceptionsTest();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.filterOperatorsTest();
    errorTotal = errorTotal + errorCount;

    testClass.toConversionTest();

    errorCount = testClass.version3ToVersion2Test();
    errorTotal = errorTotal + errorCount;

    errorCount = testClass.version2ToVersion3Test();
    errorTotal = errorTotal + errorCount;

    std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal << std::endl;

    return( errorTotal );
}
