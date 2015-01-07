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

#include "Rinex_Obs_T.hpp"
#include<iostream>
#include<string>

using namespace gpstk;

//------------------------------------------------------------
// This tests throws many GPSTK RinexObsHeader exceptions including
// Incomplete headers, invalid line lengths etc
// Also an extended obs type is used and dumped within this test.
//------------------------------------------------------------
int xRinexObs :: headerExceptionTest( void )
{
    int fail_counter = 0;

    try
    {
        gpstk::RinexObsStream RinexObsFile( "RinexObs_Logs/RinexObsFile.06o" );
        gpstk::RinexObsStream ih( "RinexObs_Logs/IncompleteHeader.06o" );
        gpstk::RinexObsStream il( "RinexObs_Logs/InvalidLineLength.06o" );
        gpstk::RinexObsStream inpwf( "RinexObs_Logs/InvalidNumPRNWaveFact.06o" );
        gpstk::RinexObsStream no( "RinexObs_Logs/NotObs.06o" );
        gpstk::RinexObsStream ss( "RinexObs_Logs/SystemGeosync.06o" );
        gpstk::RinexObsStream sr( "RinexObs_Logs/SystemGlonass.06o" );
        gpstk::RinexObsStream sm( "RinexObs_Logs/SystemMixed.06o" );
        gpstk::RinexObsStream st( "RinexObs_Logs/SystemTransit.06o" );
        gpstk::RinexObsStream unsupv( "RinexObs_Logs/UnSupVersion.06o" );
        gpstk::RinexObsStream contdata( "RinexObs_Logs/RinexContData.06o" );
 
        gpstk::RinexObsStream out( "RinexObs_Logs/TestOutput.06o",ios::out);
        gpstk::RinexObsStream out2( "RinexObs_Logs/TestOutput3.06o",ios::out);
        gpstk::RinexObsStream dump( "RinexObs_Logs/ObsDump",ios::out);

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

        gpstk::RegisterExtendedRinexObsType( "ER","Testing Type", "Candela", (unsigned) 2 );
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

        if( RinexObsFileh.NumberHeaderRecordsToBeWritten() != 39 )
        {
          fail_counter++;
          std::cout << "GPSTK_TEST_RESULT, RinexObs_T, xRinexObs, headerExceptionTest, subtest1, 1"  << std::endl;
        }
    }
    catch(...)
    {
      // the reported num_fails here should be the total of all the tests above
      // since if the try block fails, we have no way to know if any of the tests passed.
        fail_counter++;
        std::cout << "GPSTK_TEST_RESULT, RinexObs_T, xRinexObs, headerExceptionTest, subtest, 1"  << std::endl;
    }

    return( fail_counter );
}


//------------------------------------------------------------
// This test checks to make sure that the output
// from a read in RinexObsFile matches the input.
//------------------------------------------------------------
int xRinexObs :: hardCodeTest( void )
{

    int fail_counter = 0;
    int test_fail    = 1;
    bool test_result = false;
    bool test_pass   = false;

    try
    {
        gpstk::RinexObsStream RinexObsFile( "RinexObs_Logs/RinexObsFile.06o" );
        gpstk::RinexObsStream out( "RinexObs_Logs/TestOutput2.06o", ios::out );
        gpstk::RinexObsStream dump( "RinexObs_Logs/ObsDump", ios::out );
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

        test_fail = fileEqualTest( (char*)"RinexObs_Logs/RinexObsFile.06o", 
                                   (char*)"RinexObs_Logs/TestOutput2.06o"   );
        if( test_fail )
        {
            fail_counter++;
        }
    }
    catch(...)
    {
        fail_counter++;
    }

    return( fail_counter );
}

//------------------------------------------------------------
// This test throws many GPSTK exceptions within the
// RinexObsData, including BadEpochLine and BadEpochFlag
//------------------------------------------------------------
int xRinexObs :: dataExceptionsTest( void )
{

    int fail_counter = 0;

    try
      {
        gpstk::RinexObsStream BadEpochLine( "RinexObs_Logs/BadEpochLine." );
        gpstk::RinexObsStream BadEpochFlag( "RinexObs_Logs/BadEpochFlag.06o" );
        gpstk::RinexObsStream BadLineSize( "RinexObs_Logs/BadLineSize.06o" );
        gpstk::RinexObsStream InvalidTimeFormat( "RinexObs_Logs/InvalidTimeFormat.06o" );
        gpstk::RinexObsStream out( "RinexObs_Logs/DataExceptionOutput.06o", ios::out );
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
      }
    catch(...)
      {
        fail_counter++;
      }

    return( fail_counter );

}

//------------------------------------------------------------
// This is the test for several of the members within RinexObsFilterOperators
// including merge, LessThanSimple, EqualsSimple, and LessThanFull.
//------------------------------------------------------------
int xRinexObs :: filterOperatorsTest( void )
{
    int fail_counter = 0;
    bool test_result = false;
    bool test_pass   = false;

    try
    {
        gpstk::RinexObsStream FilterStream1( "RinexObs_Logs/FilterTest1.06o" );
        FilterStream1.open( "RinexObs_Logs/FilterTest1.06o", std::ios::in );

        gpstk::RinexObsStream FilterStream2( "RinexObs_Logs/FilterTest2.06o" );
        gpstk::RinexObsStream FilterStream3( "RinexObs_Logs/FilterTest3.06o" );
        gpstk::RinexObsStream FilterStream4( "RinexObs_Logs/FilterTest4.06o" );
        gpstk::RinexObsStream out( "RinexObs_Logs/FilterOutput.txt", ios::out );

        gpstk::RinexObsHeader FilterHeader1;
        gpstk::RinexObsHeader FilterHeader2;
        gpstk::RinexObsHeader FilterHeader3;
        gpstk::RinexObsHeader FilterHeader4;

        gpstk::RinexObsData FilterData1;
        gpstk::RinexObsData FilterData2;
        gpstk::RinexObsData FilterData3;
        gpstk::RinexObsData FilterData4;


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

        gpstk::RinexObsHeaderTouchHeaderMerge merged;
        merged( FilterHeader1 );
        merged( FilterHeader2 );

        gpstk::RinexObsDataOperatorLessThanFull( merged.obsSet );
        out << merged.theHeader;

        //----------------------------------------
        gpstk::RinexObsDataOperatorEqualsSimple EqualsSimple;
        test_pass   = false;
        test_result = EqualsSimple( FilterData1, FilterData1 );
        if( test_result == true )
        {
            test_pass = true;
        }
        else
        {
            fail_counter++;
        }

        //----------------------------------------
        gpstk::RinexObsDataOperatorLessThanSimple LessThanSimple;
        test_pass   = false;
        test_result = LessThanSimple( FilterData1, FilterData1 );
        if( test_result == false )
        {
            test_pass = true;
        }
        else
        {
            fail_counter++;
        }

        //----------------------------------------
        //----------------------------------------
        gpstk::RinexObsDataOperatorLessThanFull LessThanFull( merged.obsSet );
        test_pass   = false;
        test_result = LessThanFull( FilterData1, FilterData1 );
        if( test_result == false )
        {
            test_pass = true;
        }
        else
        {
            fail_counter++;
        }

        test_pass   = false;
        test_result = LessThanFull( FilterData1, FilterData2 );
        if( test_result == false )
        {
            test_pass = true;
        }
        else
        {
            fail_counter++;
        }

    }
    catch(...)
    {
        fail_counter++;
    }

}

//------------------------------------------------------------
// A helper function for xRinexObs to line by line, check if  the two 
// files given are the same.
// Takes in two file names within double quotes "FILEONE.TXT" "FILETWO.TXT".
// Returns true if the files are equal.
// Skips the first two lines becasue dates are often writen as the current
// data and thus very hard to pin down a specific time for.
//------------------------------------------------------------
int xRinexObs :: fileEqualTest( char* handle1, char* handle2 )
{
    int test_fail = 1;
    int counter = 2;
    std::ifstream File1;
    std::ifstream File2;
    std::string File1Line;
    std::string File2Line;

    File1.open( handle1 );
    File2.open( handle2 );
    std::getline( File1, File1Line );
    std::getline( File2, File2Line );
    std::getline( File1, File1Line );
    std::getline( File2, File2Line );

    while( !File1.eof() )
    {
        if( File2.eof() )
            {
                cout << counter << "ONE" << endl;
                test_fail = 1;
                return( test_fail );
            }
        getline( File1, File1Line );
        getline( File2, File2Line );
        counter++;
        if( File1Line != File2Line )
        {
            cout << counter << "TWO" << endl;
            test_fail = 1;
            return( test_fail );
        }
    }

    if( !File2.eof() )
    {
        cout << counter << "THREE" << endl;
        test_fail = 1;
        return( test_fail );
    }
    else
    {
        test_fail = 0;
        return( test_fail );
    }
}

//------------------------------------------------------------
// Helper function to check the test result
//------------------------------------------------------------

void checkResult(int check, int& errCount) // Function to handle test result output
{
	if (check == -1)
	{
		std::cout << "DIDN'T RUN!!!!" << std::endl;
	}
	else if (check == 0 )
	{
		std::cout << "GOOD!!!!" << std::endl;
	}
	else if (check > 0)
	{
		std::cout << "BAD!!!!" << std::endl;
		std::cout << "Error Message for Bad Test is Code " << check << std::endl;
		errCount++;
	}
}

//============================================================
// Run all the test methods defined above
//============================================================

int main()
{

    int check = 0;
    int errorCounter = 0;
    xRinexObs testClass;

    check = testClass.headerExceptionTest();
    std::cout << "headerExceptionTest Result is: ";
    checkResult( check, errorCounter );
    check = -1;

    check = testClass.hardCodeTest();
    std::cout << "hardCodeTest Result is: ";
    checkResult( check, errorCounter );
    check = -1;

    check = testClass.dataExceptionsTest();
    std::cout << "dataExceptionsTest Result is: ";
    checkResult( check, errorCounter );
    check = -1;

    check = testClass.filterOperatorsTest();
    std::cout << " filterOperatorsTest Result is: ";
    checkResult( check, errorCounter );
    check = -1;

    std::cout << "Total Errors: " << errorCounter << std::endl;
    return( errorCounter );
}
