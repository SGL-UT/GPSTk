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

#include "RinexMetBase.hpp"
#include "RinexMetData.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetFilterOperators.hpp"
#include "RinexMetStream.hpp"

#include "StringUtils.hpp"
#include "Exception.hpp"

#include "build_config.h"

#include "TestUtil.hpp"
#include <fstream>
#include <string>
#include <iostream>

using namespace gpstk;
using namespace std;


//============================================================
// Test Class Declarations
//============================================================


//------------------------------------------------------------
// Class:   RinexMet_T
// Purpose: This test covers the RinexMet*
//          files for Rinex I/O Manipulation
//------------------------------------------------------------
class RinexMet_T
{

    public:

        RinexMet_T()
        {
            init();
        }

        ~RinexMet_T(){}

        void init( void );

        int bitsAsStringTest( void );
        int bitStringTest( void );
        int reallyPutRecordTest( void );
        int reallyGetRecordTest( void );
        int convertObsTypeSTRTest( void );
        int convertObsTypeHeaderTest( void );
        int hardCodeTest( void );
        int continuationTest( void );
        int dataExceptionsTest( void );
        int filterOperatorsTest( void );

    private:

        // for version controled files needed for testing
        std::string dataFilePath;
      
        // for temporary ntermediate and output files 
        // used during testing but not to be version controlled
        std::string tempFilePath;

        //---------------------------------------- 
        // Input File descriptions
        //---------------------------------------- 
        // Normal      = Normal Met File (No changes, straight from the archive)
        // BLL         = Bad Line Length Met file (Add a space characacter to the end of the first line)
        // NMF         = Not a Met File (Change Met to zet in first line)
        // BOL         = Bad Obs line (Add in something between the MET obs besides space)
        // FER         = Bad Sesor Type line, Format Error
        // ExtraH      = Extra header line with HEADERLINE2
        // UnSup       = Unsupported Rinex Version (3.30)
        // MissingSPos = Missing SensorPos
        // ObsHStrErr  = Error in the part of the header which holds the list of observations within the Met file
        // SensorType  = Error in the sensor type
        //---------------------------------------- 

        std::string inputRinexMetNormal       ;
        std::string inputRinexMetBLL          ;
        std::string inputRinexMetNMF          ;
        std::string inputRinexMetBOL          ;
        std::string inputRinexMetFER          ;
        std::string inputRinexMetExtraH       ;
        std::string inputRinexMetUnSupRinex   ;
        std::string inputRinexMetMissingSPos  ;
        std::string inputRinexMetContLines    ;
        std::string inputRinexMetNoObs        ;
        std::string inputRinexMetSensorType   ;

        std::string inputRinexMetObsHdrStrErr ;
        std::string inputRinexMetMissingMkr   ;
        std::string inputRinexMetInvTimeFmt   ;
        std::string inputRinexMetFilterTest1  ;
        std::string inputRinexMetFilterTest2  ;
        std::string inputRinexMetFilterTest3  ;
        std::string inputRinexMetFilterTest4  ;

        std::string outputRinexMetHardCode    ;
        std::string outputRinexMetExtraOutput ;
        std::string outputRinexMetContLines   ;
        std::string outputRinexMetDumps       ;
        std::string outputRinexMetExceptions  ;
        std::string outputRinexMetFilterTest  ;

        std::string failDescriptionString;
	std::stringstream failDescriptionStream;

};


//============================================================
// Test Class Definitions
//============================================================



//------------------------------------------------------------
// A test to assure that the bistAsString function works as intended
//------------------------------------------------------------

void RinexMet_T :: init( void )
{

    TestUtil test0;
    std::string dataFilePath = test0.getDataPath();
    std::string tempFilePath = test0.getTempPath();

    //---------------------------------------- 
    // Full file paths
    //---------------------------------------- 
    std::string file_sep = gpstk::getFileSep();

    inputRinexMetNormal       = dataFilePath  + file_sep + "test_input_rinex_met_408_110a.04m"    ; // Formerly without the "test_input_rinex_met_" prefix string
    inputRinexMetBLL          = dataFilePath  + file_sep + "test_input_rinex_met_BLL.04m"         ;
    inputRinexMetNMF          = dataFilePath  + file_sep + "test_input_rinex_met_NotMetFile.04m"  ;
    inputRinexMetBOL          = dataFilePath  + file_sep + "test_input_rinex_met_BOL.04m"         ;
    inputRinexMetFER          = dataFilePath  + file_sep + "test_input_rinex_met_FER.04m"         ;
    inputRinexMetExtraH       = dataFilePath  + file_sep + "test_input_rinex_met_ExtraH.04m"      ;
    inputRinexMetUnSupRinex   = dataFilePath  + file_sep + "test_input_rinex_met_UnSupRinex.04m"  ;
    inputRinexMetMissingSPos  = dataFilePath  + file_sep + "test_input_rinex_met_MissingEOH.04m"  ;
    inputRinexMetContLines    = dataFilePath  + file_sep + "test_input_rinex_met_ContLines10.04m" ;
    inputRinexMetNoObs        = dataFilePath  + file_sep + "test_input_rinex_met_NoObsData.04m"   ;
    inputRinexMetSensorType   = dataFilePath  + file_sep + "test_input_rinex_met_SensorTypeError.04m" ;
    inputRinexMetObsHdrStrErr = dataFilePath  + file_sep + "test_input_rinex_met_ObsHeaderStringError.04m" ;
    inputRinexMetMissingMkr   = dataFilePath  + file_sep + "test_input_rinex_met_MissingMarkerName";
    inputRinexMetInvTimeFmt   = dataFilePath  + file_sep + "test_input_rinex_met_InvalidTimeFormat.04m";
    inputRinexMetFilterTest1  = dataFilePath  + file_sep + "test_input_rinex_met_FilterTest1.04m"  ;
    inputRinexMetFilterTest2  = dataFilePath  + file_sep + "test_input_rinex_met_FilterTest2.04m"  ;
    inputRinexMetFilterTest3  = dataFilePath  + file_sep + "test_input_rinex_met_FilterTest3.04m"  ;
    inputRinexMetFilterTest4  = dataFilePath  + file_sep + "test_input_rinex_met_FilterTest4.04m"  ;

    outputRinexMetHardCode    = tempFilePath  + file_sep + "test_output_rinex_met_Output.txt";         // formerly Output.txt
    outputRinexMetExtraOutput = tempFilePath  + file_sep + "test_output_rinex_met_ExtraOutput.txt";    // formerly ExtraOutput.txt
    outputRinexMetContLines   = tempFilePath  + file_sep + "test_output_rinex_met_Cont.txt";           // formerly OutputCont.txt
    outputRinexMetDumps       = tempFilePath  + file_sep + "test_output_rinex_met_Dumps.txt";          // formerly MetDumps
    outputRinexMetExceptions  = tempFilePath  + file_sep + "test_output_rinex_met_DataExceptions.txt"; // formerly OutputDataExceptions.txt
    outputRinexMetFilterTest  = tempFilePath  + file_sep + "test_output_rinex_met_Filter.txt" ;        // formerly FilterOutput.txt


}

//------------------------------------------------------------
// A test to assure that the bistAsString function works as intended
//------------------------------------------------------------

int RinexMet_T :: bitsAsStringTest( void )
{
    TestUtil test1( "RinexMetHeader", "bitsAsString", __FILE__, __LINE__ );

    std::string test_desc = "RinexMetHeader, bitsAsString, file read compared to expected string, did not match";
    
    gpstk::RinexMetHeader RinexMetHeader;

    std::string expected_string_a = "RINEX VERSION / TYPE";
    std::string expected_string_b = "PGM / RUN BY / DATE";
    std::string expected_string_c = "COMMENT";
    std::string expected_string_d = "MARKER NAME";
    std::string expected_string_e = "MARKER NUMBER";
    std::string expected_string_f = "# / TYPES OF OBSERV";
    std::string expected_string_g = "SENSOR MOD/TYPE/ACC";
    std::string expected_string_h = "SENSOR POS XYZ/H";
    std::string expected_string_i = "END OF HEADER";
    std::string expected_string_z = "*UNKNOWN/INVALID BITS*";

    test1.assert( expected_string_a == RinexMetHeader.bitsAsString(RinexMetHeader.validVersion)      , test_desc, __LINE__ );
    test1.assert( expected_string_b == RinexMetHeader.bitsAsString(RinexMetHeader.validRunBy)        , test_desc, __LINE__ );
    test1.assert( expected_string_c == RinexMetHeader.bitsAsString(RinexMetHeader.validComment)      , test_desc, __LINE__ );
    test1.assert( expected_string_d == RinexMetHeader.bitsAsString(RinexMetHeader.validMarkerName)   , test_desc, __LINE__ );
    test1.assert( expected_string_e == RinexMetHeader.bitsAsString(RinexMetHeader.validMarkerNumber) , test_desc, __LINE__ );
    test1.assert( expected_string_f == RinexMetHeader.bitsAsString(RinexMetHeader.validObsType)      , test_desc, __LINE__ );
    test1.assert( expected_string_g == RinexMetHeader.bitsAsString(RinexMetHeader.validSensorType)   , test_desc, __LINE__ );
    test1.assert( expected_string_h == RinexMetHeader.bitsAsString(RinexMetHeader.validSensorPos)    , test_desc, __LINE__ );
    test1.assert( expected_string_i == RinexMetHeader.bitsAsString(RinexMetHeader.validEoH)          , test_desc, __LINE__ );
    //Default Case
    test1.assert( expected_string_z == RinexMetHeader.bitsAsString(RinexMetHeader.allValid21)        , test_desc, __LINE__ );

    return( test1.countFails() );
}

//------------------------------------------------------------
// A test to assure that the version validity bits are what we expect them to be
//------------------------------------------------------------
int RinexMet_T :: bitStringTest( void )
{
    TestUtil test2( "RinexMetHeader", "bitString", __FILE__, __LINE__ );
    
    std::string test_desc = "RinexMetHeader, bitString, test to assure that the version validity bits are what we expect them to be";
    std::string test_fail = "";

    gpstk::RinexMetHeader RinexMetHeader;

    std::string sep=", ";
    std::string expected_string_a = "\"RINEX VERSION / TYPE\"";
    std::string expected_string_b = "\"RINEX VERSION / TYPE\", \"PGM / RUN BY / DATE\", \"MARKER NAME\", \"# / TYPES OF OBSERV\", \"SENSOR MOD/TYPE/ACC\", \"SENSOR POS XYZ/H\", \"END OF HEADER\"";

    test_fail = ", validVersion failed";
    test2.assert( expected_string_a == RinexMetHeader.bitString( RinexMetHeader.validVersion, '\"', sep ), test_desc + test_fail, __LINE__ );

    test_fail = ", allValid21 failed";
    test2.assert( expected_string_b == RinexMetHeader.bitString( RinexMetHeader.allValid21, '\"', sep ),   test_desc + test_fail, __LINE__ );

    test_fail = ", allValid20 failed";
    test2.assert( expected_string_b == RinexMetHeader.bitString( RinexMetHeader.allValid20, '\"', sep ),   test_desc + test_fail, __LINE__ );
    // test2.assert( expected_string_b, RinexMetHeader.bitString(RinexMetHeader.allValid20,' ',sep),   test_desc + test_fail, __LINE__ );

    return test2.countFails();
}

//------------------------------------------------------------
// A small test of the reallyPutRecord member of the RinexMetHeader 
// with a few execptions such as an Unsupported Rinex version (e.g. 3.33)
//  and a Missing Marker Name
//------------------------------------------------------------
int RinexMet_T :: reallyPutRecordTest( void )
{
  TestUtil test3( "RinexMetHeader", "exceptions", __FILE__, __LINE__ );

    std::string msg_test_desc   = "RinexMetHeader, reallyPutRecordTest, exception tests";
    std::string msg_false_pass  = ", should have thrown a gpstk::Exception but did not.";
    std::string msg_fail        = ", should have thrown a gpstk::Exception but threw an unexpected exception.";
    std::string msg_test_detail = "";
    
    gpstk::RinexMetHeader RinexMetHeader;

    gpstk::RinexMetStream UnSupRinex( inputRinexMetUnSupRinex.c_str() );
    gpstk::RinexMetStream MissingMarkerName( inputRinexMetMissingMkr.c_str() );
    gpstk::RinexMetStream output( outputRinexMetExtraOutput.c_str(), std::ios::out );

    output.exceptions( std::fstream::failbit );

    //------------------------------------------------------------
    // Unsupported Rinex
    //------------------------------------------------------------
    msg_test_detail = ", Unsupported Rinex version";
    
    try
    {
        UnSupRinex >> RinexMetHeader;
        test3.assert( false, msg_test_desc + msg_test_detail + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test3.assert( true, msg_test_desc + msg_test_detail, __LINE__ );
    }
    catch(...)
    {
        test3.assert( false, msg_test_desc + msg_test_detail + msg_fail, __LINE__ );
    }

    //------------------------------------------------------------
    // Missing Marker
    //------------------------------------------------------------
    msg_test_detail = ", Missing Marker Name";
    try
    {
        MissingMarkerName >> RinexMetHeader;
        test3.assert( false, msg_test_desc + msg_test_detail + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test3.assert( true, msg_test_desc + msg_test_detail, __LINE__ );
    }
    catch(...)
    {
        test3.assert( false, msg_test_desc + msg_test_detail + msg_fail, __LINE__ );
    }

    return test3.countFails();
}

// ------------------------------------------------------------
// This function reads the RINEX MET header from the given FFStream.
// If an error is encountered in reading form the stream, the stream
// is reset to its original position and its fail-bit is set.
// ------------------------------------------------------------
int RinexMet_T :: reallyGetRecordTest( void )
{
    TestUtil test4( "RinexMetHeader", "exceptions", __FILE__, __LINE__ );

    std::string msg_test_desc = "";
    std::string msg_false_pass  = ", should have thrown a gpstk::Exception but did not.";
    std::string msg_fail        = ", should have thrown a gpstk::Exception but threw an unexpected exception.";
    
    

    //Header file we will be testing on
    gpstk::RinexMetHeader RinexMetHeader;

    //Normal Met File (No changes, straight from the archive)
    gpstk::RinexMetStream Normal( inputRinexMetNormal.c_str() );

    //Bad Line Length Met file (Add a space characacter to the end of the first line)
    gpstk::RinexMetStream BLL( inputRinexMetBLL.c_str() );

    //Not a Met File (Change Met to zet in first line)
    gpstk::RinexMetStream NMF( inputRinexMetNMF.c_str() );

    //Bad Obs line (Add in something between the MET obs besides space)
    gpstk::RinexMetStream BOL( inputRinexMetBOL.c_str() );

    //Bad Sesor Type line, Format Error
    gpstk::RinexMetStream FER( inputRinexMetFER.c_str() );

    //Extra header line with HEADERLINE2
    gpstk::RinexMetStream ExtraH( inputRinexMetExtraH.c_str() );

    //Unsupported Rinex Version (3.30)
    gpstk::RinexMetStream UnSupRinex( inputRinexMetUnSupRinex.c_str() );

    //Missing SensorPos
    gpstk::RinexMetStream MissingSPos( inputRinexMetMissingSPos.c_str() );

    //Error in the part of the header which holds the list of observations within the Met file
    gpstk::RinexMetStream ObsHeaderStringError( inputRinexMetObsHdrStrErr.c_str() );

    //Error in the sensor type
    gpstk::RinexMetStream SensorType( inputRinexMetSensorType.c_str() );

    Normal.exceptions( std::fstream::failbit );
    BLL.exceptions( std::fstream::failbit );
    NMF.exceptions( std::fstream::failbit );
    BOL.exceptions( std::fstream::failbit );
    FER.exceptions( std::fstream::failbit );
    ExtraH.exceptions( std::fstream::failbit );
    UnSupRinex.exceptions( std::fstream::failbit );
    MissingSPos.exceptions( std::fstream::failbit );
    ObsHeaderStringError.exceptions( std::fstream::failbit );
    SensorType.exceptions( std::fstream::failbit );


    //============================================================
    // Fail any of the following tests which does NOT throw a gpstk::Exception
    //============================================================

    //----------------------------------------
    //----------------------------------------
    msg_test_desc = "ExtraH >> RinexMetHeader";
    try
    {
        ExtraH >> RinexMetHeader;
        test4.assert( false, msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test4.assert( true, msg_test_desc, __LINE__ );
    }
    catch(...)
    {
        test4.assert( false, msg_test_desc + msg_fail, __LINE__ );
    }

    //----------------------------------------
    //----------------------------------------
    msg_test_desc = "UnSupRinex >> RinexMetHeader";
    try
    {
        UnSupRinex >> RinexMetHeader ;
        test4.assert( false, msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test4.assert( true, msg_test_desc, __LINE__ );
    }
    catch(...)
    {
        test4.assert( false, msg_test_desc + msg_fail, __LINE__ );
    }

    //----------------------------------------
    //----------------------------------------
    msg_test_desc = "MissingSPos >> RinexMetHeader";
    try
    {
        MissingSPos >> RinexMetHeader ;
        test4.assert( false, msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test4.assert( true, msg_test_desc, __LINE__ );
    }
    catch(...)
    {
        test4.assert( false, msg_test_desc + msg_fail, __LINE__ );
    }

    //----------------------------------------
    //----------------------------------------
    msg_test_desc = "ObsHeaderStringError >> RinexMetHeader";
    try
    {
        ObsHeaderStringError >> RinexMetHeader ;
        test4.assert( false, msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test4.assert( true, msg_test_desc, __LINE__ );
    }
    catch(...)
    {
        test4.assert( false, msg_test_desc + msg_fail, __LINE__ );
    }

    //----------------------------------------
    //----------------------------------------
    msg_test_desc = "SensorType >> RinexMetHeader";
    try
    {
        SensorType >> RinexMetHeader ;
        test4.assert( false, msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test4.assert( true, msg_test_desc, __LINE__ );
    }
    catch(...)
    {
        test4.assert( false, msg_test_desc + msg_fail, __LINE__ );
    }

    //----------------------------------------
    //----------------------------------------
    msg_test_desc = "BLL >> RinexMetHeader";
    try
    {
        BLL >> RinexMetHeader ;
        test4.assert( false, msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test4.assert( true, msg_test_desc, __LINE__ );
    }
    catch(...)
    {
        test4.assert( false, msg_test_desc + msg_fail, __LINE__ );
    }

    //----------------------------------------
    //----------------------------------------
    msg_test_desc = "NMF >> RinexMetHeader";
    try
    {
        NMF >> RinexMetHeader ;
        test4.assert( false, msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test4.assert( true, msg_test_desc, __LINE__ );
    }
    catch(...)
    {
        test4.assert( false, msg_test_desc + msg_fail, __LINE__ );
    }

    //----------------------------------------
    //----------------------------------------
    msg_test_desc = "BOL >> RinexMetHeader";
    try
    {
        BOL >> RinexMetHeader ;
        test4.assert( false, msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch( gpstk::Exception e )
    {
        test4.assert( true, msg_test_desc, __LINE__ );
    }
    catch(...)
    {
        test4.assert( false, msg_test_desc + msg_fail, __LINE__ );
    }


    //----------------------------------------
    // Fail the follow test if it throws ANYTHING!
    //----------------------------------------
    msg_test_desc = "Normal >> RinexMetHeader";
    std::string msg_fail_gpstk = ", should have thrown nothing, but threw a gpstk::Exception.";
    std::string msg_fail_other = ", should have thrown nothing, but threw an exception.";
    try
    {
        Normal >> RinexMetHeader ;
        test4.assert( true, msg_test_desc, __LINE__ );
    }
    catch( gpstk::Exception e)
    {
        test4.assert( false, msg_test_desc + msg_fail_gpstk, __LINE__ );
    }
    catch(...)
    {
        test4.assert( false, msg_test_desc + msg_fail_other, __LINE__ );
    }



    return test4.countFails();
}

//------------------------------------------------------------
// This test tests the convertObsType member of RinexMetHeader
// This particular test asserts if two Met Types are equal
//------------------------------------------------------------

int RinexMet_T :: convertObsTypeSTRTest( void )
{
    TestUtil test5( "RinexMetHeader", "convertObsType", __FILE__, __LINE__ );
    std::string msg_test_desc = "convertObsTypeSTRTest, ";
    std::string msg_test_fail = "";

    gpstk::RinexMetHeader RinexMetHeader;
    gpstk::RinexMetStream Normal( inputRinexMetNormal.c_str() );

    Normal >> RinexMetHeader;

    gpstk::RinexMetHeader::RinexMetType PR = RinexMetHeader.convertObsType( "PR" );
    gpstk::RinexMetHeader::RinexMetType TD = RinexMetHeader.convertObsType( "TD" );
    gpstk::RinexMetHeader::RinexMetType HR = RinexMetHeader.convertObsType( "HR" );
    gpstk::RinexMetHeader::RinexMetType ZW = RinexMetHeader.convertObsType( "ZW" );
    gpstk::RinexMetHeader::RinexMetType ZD = RinexMetHeader.convertObsType( "ZD" );
    gpstk::RinexMetHeader::RinexMetType ZT = RinexMetHeader.convertObsType( "ZT" );
    gpstk::RinexMetHeader::RinexMetType HI = RinexMetHeader.convertObsType( "HI" );
    // gpstk::RinexMetHeader::RinexMetType KE = RinexMetHeader.convertObsType( "HI" );

    std::string PRS = "PR";
    std::string TDS = "TD";
    std::string HRS = "HR";
    std::string ZWS = "ZW";
    std::string ZDS = "ZD";
    std::string ZTS = "ZT";
    std::string HIS = "HI";

    msg_test_fail = "convertObsType(PRS) did not return expected value";
    test5.assert( PR == RinexMetHeader.convertObsType(PRS), msg_test_desc + msg_test_fail, __LINE__ ); 

    msg_test_fail = "convertObsType(TDS) did not return expected value";
    test5.assert( TD == RinexMetHeader.convertObsType(TDS), msg_test_desc + msg_test_fail, __LINE__ ); 

    msg_test_fail = "convertObsType(HRS) did not return expected value";
    test5.assert( HR == RinexMetHeader.convertObsType(HRS), msg_test_desc + msg_test_fail, __LINE__ ); 

    msg_test_fail = "convertObsType(ZWS) did not return expected value";
    test5.assert( ZW == RinexMetHeader.convertObsType(ZWS), msg_test_desc + msg_test_fail, __LINE__ ); 

    msg_test_fail = "convertObsType(ZDS) did not return expected value";
    test5.assert( ZD == RinexMetHeader.convertObsType(ZDS), msg_test_desc + msg_test_fail, __LINE__ ); 

    msg_test_fail = "convertObsType(ZTS) did not return expected value";
    test5.assert( ZT == RinexMetHeader.convertObsType(ZTS), msg_test_desc + msg_test_fail, __LINE__ ); 

    msg_test_fail = "convertObsType(HIS) did not return expected value";
    test5.assert( HI == RinexMetHeader.convertObsType(HIS), msg_test_desc + msg_test_fail, __LINE__ );



    return test5.countFails();
}

//------------------------------------------------------------
//
// This test tests the convertObsType member of RinexMetHeader
// This particular test asserts if two strings are equal
//
//------------------------------------------------------------
int RinexMet_T :: convertObsTypeHeaderTest( void )
{
    TestUtil test6( "RinexMetHeader", "convertObsType", __FILE__, __LINE__ );
    std::string msg_test_desc = "convertObsTypeHeaderTest, ";
    std::string msg_test_fail = "";

    gpstk::RinexMetHeader RinexMetHeader;
    gpstk::RinexMetStream Normal( inputRinexMetNormal.c_str() );

    Normal >> RinexMetHeader;

    gpstk::RinexMetHeader::RinexMetType PR = RinexMetHeader.convertObsType( "PR" );
    gpstk::RinexMetHeader::RinexMetType TD = RinexMetHeader.convertObsType( "TD" );
    gpstk::RinexMetHeader::RinexMetType HR = RinexMetHeader.convertObsType( "HR" );
    gpstk::RinexMetHeader::RinexMetType ZW = RinexMetHeader.convertObsType( "ZW" );
    gpstk::RinexMetHeader::RinexMetType ZD = RinexMetHeader.convertObsType( "ZD" );
    gpstk::RinexMetHeader::RinexMetType ZT = RinexMetHeader.convertObsType( "ZT" );
    // gpstk::RinexMetHeader::RinexMetType KE = RinexMetHeader.convertObsType( "ZT" );

    std::string PRS = "PR";
    std::string TDS = "TD";
    std::string HRS = "HR";
    std::string ZWS = "ZW";
    std::string ZDS = "ZD";
    std::string ZTS = "ZT";

    msg_test_fail = "convertObsType(PR) did not return expected value";
    test6.assert( PRS == RinexMetHeader.convertObsType(PR), msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "convertObsType(TD) did not return expected value";
    test6.assert( TDS == RinexMetHeader.convertObsType(TD), msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "convertObsType(HR) did not return expected value";
    test6.assert( HRS == RinexMetHeader.convertObsType(HR), msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "convertObsType(ZW) did not return expected value";
    test6.assert( ZWS == RinexMetHeader.convertObsType(ZW), msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "convertObsType(ZD) did not return expected value";
    test6.assert( ZDS == RinexMetHeader.convertObsType(ZD), msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "convertObsType(ZT) did not return expected value";
    test6.assert( ZTS == RinexMetHeader.convertObsType(ZT), msg_test_desc + msg_test_fail, __LINE__ );


    //----------------------------------------
    // Fail if the following throws anything but a FFStreamError
    //----------------------------------------
    msg_test_desc = "convertObsType( \"KE\" ), should throw  gpstk::FFStreamError";
    std::string msg_false_pass = ", but threw no exceptions.";
    std::string msg_fail_other = ", but threw a different exception.";
    try
    {
        RinexMetHeader.convertObsType( "KE" );
        test6.assert( false, msg_test_desc + msg_false_pass, __LINE__ );
    }
    catch(gpstk::FFStreamError)
    {
        test6.assert( true, msg_test_desc, __LINE__  );
    }
    catch(...)
    {
        test6.assert( false, msg_test_desc + msg_fail_other, __LINE__  );
    }



    
    return test6.countFails();
}

//------------------------------------------------------------
//
// This test checks to make sure that the internal members of 
// the RinexMetHeader are as we think they should be.
// Also at the end of this test, we check and make sure our 
// output file is equal to our input
//
//------------------------------------------------------------
int RinexMet_T :: hardCodeTest( void )
{
    TestUtil test7( "RinexMetHeader", "version", __FILE__, __LINE__ );
    std::string msg_test_desc = "RinexMetHeader data member value tests, ";
    std::string msg_test_fail = "";

    gpstk::RinexMetStream testRinexMetStream( inputRinexMetNormal.c_str() );
    gpstk::RinexMetStream out( outputRinexMetHardCode.c_str(), std::ios::out );
    gpstk::RinexMetHeader testRinexMetHeader;

    testRinexMetStream >> testRinexMetHeader;
 
    //============================================================
    // Test RinexMet Header content
    //============================================================
    
    msg_test_fail = "RinexMetHeader.version, does not match expected value ";
    test7.assert( testRinexMetHeader.version     == 2.1, msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetHeader.fileType, does not match expected value ";
    test7.assert( testRinexMetHeader.fileType    == (std::string)"Meteorological", msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetHeader.fileProgram, does not match expected value ";
    test7.assert( testRinexMetHeader.fileProgram == (std::string)"GFW - RMW", msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetHeader.fileAgency, does not match expected value ";
    test7.assert( testRinexMetHeader.fileAgency  == (std::string)"NIMA", msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetHeader.date, does not match expected value ";
    test7.assert( testRinexMetHeader.date        == (std::string)"04/18/2004 23:58:50", msg_test_desc + msg_test_fail, __LINE__ );


    std::vector<std::string>::const_iterator itr1 = testRinexMetHeader.commentList.begin();
    msg_test_fail = "weather data correct values message does not match";
    while( itr1 != testRinexMetHeader.commentList.end() )
    {
        test7.assert( (*itr1) == (std::string)"Some weather data may have corrected values", msg_test_desc + msg_test_fail, __LINE__ );

        itr1++;
    }

    msg_test_fail = "RinexMetHeader.markerName, does not match expected value ";
    test7.assert( testRinexMetHeader.markerName   == (std::string)"85408", msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetHeader.markerNumber, does not match expected value ";
    test7.assert( testRinexMetHeader.markerNumber == (std::string)"85408", msg_test_desc + msg_test_fail, __LINE__ );

    
    msg_test_fail = "testRinexMetHeader.convertObsType(iterator) did not return expected value PR";
    vector<RinexMetHeader::RinexMetType>::const_iterator itr2 = testRinexMetHeader.obsTypeList.begin();
    if( itr2 != testRinexMetHeader.obsTypeList.end() )
    {
        test7.assert( testRinexMetHeader.convertObsType(*itr2) == (std::string)"PR", msg_test_desc + msg_test_fail, __LINE__ );
        itr2++;
    } else {
        test7.assert( false, msg_test_desc, __LINE__ );
    }

    
    msg_test_fail = "testRinexMetHeader.convertObsType(iterator) did not return expected value TD";
    if( itr2 != testRinexMetHeader.obsTypeList.end() )
    {
        test7.assert( testRinexMetHeader.convertObsType(*itr2) == (std::string)"TD", msg_test_desc + msg_test_fail, __LINE__ );
        itr2++;
    } else {
      test7.assert( false, msg_test_desc, __LINE__ );
    }

    
    msg_test_fail = "testRinexMetHeader.convertObsType(iterator) did not return expected value HI";
    if( itr2 != testRinexMetHeader.obsTypeList.end() )
    {
        test7.assert( testRinexMetHeader.convertObsType(*itr2) == (std::string)"HI", msg_test_desc + msg_test_fail, __LINE__ );
    } else {
      test7.assert( false, msg_test_desc, __LINE__ );
    }

    
    //------------------------------------------------------------
    //------------------------------------------------------------
    vector<RinexMetHeader::sensorType>::const_iterator itr3 = testRinexMetHeader.sensorTypeList.begin();
    if( itr3 != testRinexMetHeader.sensorTypeList.end() )
    {
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.model did not return expected value Vaisala";
        test7.assert( (*itr3).model    == (std::string)"Vaisala", msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.type did not return expected value PTB220";
        test7.assert( (*itr3).type     == (std::string)"PTB220", msg_test_desc + msg_test_fail, __LINE__  );

        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.accuracy did not return expected value 0.1";
        test7.assert( (*itr3).accuracy == 0.1, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType did not return expected value PR";
        test7.assert( testRinexMetHeader.convertObsType((*itr3).obsType) == (std::string)"PR", msg_test_desc + msg_test_fail, __LINE__ );

        itr3++;
    } else {
        // Must fail all four tests above
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.model test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.type test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.accuracy test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
    }

    
    //------------------------------------------------------------
    //------------------------------------------------------------
    if( itr3 != testRinexMetHeader.sensorTypeList.end() )
    {
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.model did not return expected value Vaisala";
        test7.assert( (*itr3).model    == (std::string)"Vaisala", msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.type did not return expected value HMP230";
        test7.assert( (*itr3).type     == (std::string)"HMP230", msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.accuracy did not return expected value 0.1";
        test7.assert( (*itr3).accuracy == 0.1, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType did not return expected value TD";
        test7.assert( testRinexMetHeader.convertObsType((*itr3).obsType) == (std::string)"TD", msg_test_desc + msg_test_fail, __LINE__ );

        itr3++;
    } else {
        // Must fail all four tests above
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.model test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.type test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.accuracy test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
    }

    
    //------------------------------------------------------------
    //------------------------------------------------------------
    if( itr3 != testRinexMetHeader.sensorTypeList.end() )
    {
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.model did not return expected value Vaisala";
        test7.assert( (*itr3).model    == (std::string)"Vaisala", msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.type did not return expected value HMP230";
        test7.assert( (*itr3).type     == (std::string)"HMP230", msg_test_desc + msg_test_fail, __LINE__  );

        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.accuracy did not return expected value 0.1";
        test7.assert( (*itr3).accuracy == 0.1, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType did not return expected value HI";
        test7.assert( testRinexMetHeader.convertObsType((*itr3).obsType) == (std::string)"HI", msg_test_desc + msg_test_fail, __LINE__ );

        itr3++;

    } else {
        // Must fail all four tests above
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.model test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.type test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorTypeList iterator.accuracy test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
    }

    //------------------------------------------------------------
    //------------------------------------------------------------
    vector<RinexMetHeader::sensorPosType>::const_iterator itr4 = testRinexMetHeader.sensorPosList.begin();

    if( itr4 != testRinexMetHeader.sensorPosList.end() )
    {
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[0] did not return expected value";
        test7.assert( (*itr4).position[0] ==  -740289.8363, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[1] did not return expected value";
        test7.assert( (*itr4).position[1] == -5457071.7414, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[2] did not return expected value";
        test7.assert( (*itr4).position[2] ==  3207245.6207, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorPosType iterator.height did not return expected value";
        test7.assert( (*itr4).height      ==        0.0000, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType did not return expected value PR";
        test7.assert( testRinexMetHeader.convertObsType((*itr4).obsType) == (std::string) "PR", msg_test_desc + msg_test_fail, __LINE__ );

        itr4++;

    } else {
        // Must fail all five tests above
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[0] test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[1] test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[2] test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.height test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
    }

    //------------------------------------------------------------
    //------------------------------------------------------------
    if( itr4 != testRinexMetHeader.sensorPosList.end() )
    {
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[0] did not return expected value";
        test7.assert( (*itr4).position[0] ==  -740289.8363, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[1] did not return expected value";
        test7.assert( (*itr4).position[1] == -5457071.7414, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[2] did not return expected value";
        test7.assert( (*itr4).position[2] ==  3207245.6207, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.sensorPosType iterator.height did not return expected value";
        test7.assert( (*itr4).height      ==        0.0000, msg_test_desc + msg_test_fail, __LINE__ );

        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType did not return expected value TD";
        test7.assert( testRinexMetHeader.convertObsType((*itr4).obsType) == (std::string) "TD", msg_test_desc + msg_test_fail, __LINE__ );

    } else {
        // Must fail all five tests above
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[0] test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[1] test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.position[2] test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.sensorPosType iterator.height test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
        msg_test_fail = "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
        test7.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
    }
    //End of Header

    //============================================================
    // Test the RinexMetData content
    //============================================================

    out << testRinexMetHeader;
    gpstk::RinexMetData testRinexMetData;
    testRinexMetStream >> testRinexMetData;
    gpstk::CivilTime TimeGuess(2004,4,19,0,0,0);
    
    msg_test_desc = "RinexMetData data member value tests, ";
    msg_test_fail = "";



    msg_test_fail = "RinexMetData.time did not equal TimeGuess";
    test7.assert( testRinexMetData.time == (gpstk::CommonTime)TimeGuess, msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetData.data for PR does not match expected value ";
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "PR" )] == 992.6, msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetData.data for TD does not match expected value ";
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "TD" )] ==  23.9, msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetData.data for HI does not match expected value ";
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "HI" )] ==  59.7, msg_test_desc + msg_test_fail, __LINE__ );

    out << testRinexMetData;
    testRinexMetStream >> testRinexMetData;
    gpstk::CivilTime TimeGuess2(2004,4,19,0,15,0);

    msg_test_fail = "RinexMetData.time did not equal TimeGuess2";
    test7.assert( testRinexMetData.time == (gpstk::CommonTime)TimeGuess2, msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetData.data for PR does not match expected value";
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "PR" )] == 992.8, msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetData.data for TD does not match expected value";
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "TD" )] ==  23.6, msg_test_desc + msg_test_fail, __LINE__ );

    msg_test_fail = "RinexMetData.data for HI does not match expected value";
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "HI" )] ==  61.6, msg_test_desc + msg_test_fail, __LINE__ );

    out << testRinexMetData;
    while( testRinexMetStream >> testRinexMetData )
    {
        out << testRinexMetData;
    }

    msg_test_fail = "files not equal, inputRinexMetNormal and outputRinexMetHardCode";
    test7.assert( test7.fileEqualTest( inputRinexMetNormal, outputRinexMetHardCode, 2), msg_test_desc + msg_test_fail, __LINE__  );

    gpstk::RinexMetStream MetDumps( outputRinexMetDumps.c_str(), std::ios::out );
    testRinexMetHeader.dump( MetDumps );
    testRinexMetData.dump( MetDumps );

    
    return test7.countFails();
}

//------------------------------------------------------------
//
// This test covers are of the RinexMetHeader and Data which 
// deal with continuation lines for the observations
//
//------------------------------------------------------------

int RinexMet_T :: continuationTest( void )
{
    TestUtil test8( "RinexMetHeader", "continuation", __FILE__, __LINE__ );
    std::string msg_test_desc = "continuation file comparison";
    std::string msg_test_fail = ", files are not equal, inputRinexMetContLines outputRinexMetContLines";
    std::string msg_fail_exception = ", threw unexpected exception";
    
    gpstk::RinexMetStream RinexMetStream( inputRinexMetContLines.c_str() );
    gpstk::RinexMetStream out( outputRinexMetContLines.c_str(), std::ios::out );
    gpstk::RinexMetStream MetDumps( outputRinexMetDumps.c_str(), std::ios::out );
    gpstk::RinexMetHeader RinexMetHeader;
    gpstk::RinexMetData RinexMetData;
   
     try
    {
        RinexMetStream >> RinexMetHeader;
        out << RinexMetHeader;

        RinexMetHeader.dump( MetDumps );
        RinexMetData.dump( MetDumps );
        RinexMetStream >> RinexMetData;
        RinexMetData.dump( MetDumps );
        out << RinexMetData;
        while( RinexMetStream >> RinexMetData )
        {
            out << RinexMetData;
        }

        test8.assert( test8.fileEqualTest( inputRinexMetContLines, outputRinexMetContLines, 2), msg_test_desc + msg_test_fail, __LINE__ );
    }
    catch(...)
    {
        test8.assert( false, msg_test_desc + msg_fail_exception, __LINE__ );
    }
    return test8.countFails();
}

//------------------------------------------------------------
//
// This test covers several RinexMetData exceptions including
// a test with no observations data for the
// listed header Obs and an invalid time format
//
// Note: assuming this means that these test SHOULD throw a gpstk::Exception
// Throwing any exception other than gpstk::Exception is considered a failure.
// Throwing nothing is assumed also to be a failure of the test.
//------------------------------------------------------------
int RinexMet_T :: dataExceptionsTest( void )
{
    TestUtil test9( "RinexMetHeader", "nodata", __FILE__, __LINE__ );
    std::string msg_test_desc = "dataExceptionsTest, ";
    std::string msg_test_fail = "";
 
    gpstk::RinexMetStream NoObs( inputRinexMetNoObs.c_str() );
    gpstk::RinexMetStream InvalidTimeFormat( inputRinexMetInvTimeFmt.c_str() );
    gpstk::RinexMetStream out( outputRinexMetExceptions.c_str(), std::ios::out );
    gpstk::RinexMetHeader rmh;
    gpstk::RinexMetData rme;

    try{
        NoObs >> rmh;
        out << rmh;

        while( NoObs >> rme )
        {
            out << rme;
        }
        InvalidTimeFormat >> rme;
        out << rme;
        InvalidTimeFormat >> rme;
        out << rme;

        test9.assert( false, "Test looking for a gpstk::Exception to be thrown when there is no header obs. No exception was thrown.", __LINE__);
    }
    catch( gpstk::Exception& e )
    {
        test9.assert( true, msg_test_desc, __LINE__ );
    }
    catch(...)
    {
        test9.assert( false, "Test looking for a gpstk::Exception to be thrown when there is no header obs. A different exception was thrown.", __LINE__);
    }
    return test9.countFails();
}

//------------------------------------------------------------
//
// Test for several of the members within RinexMetFilterOperators 
// including merge, LessThanSimple, EqualsSimple,
// and LessThanFull.
//
//------------------------------------------------------------
int RinexMet_T :: filterOperatorsTest( void )
{
    TestUtil test10( "RinexMetStream", "filter", __FILE__, __LINE__ );
    std::string msg_test_desc = "filterOperatorsTest, ";
    std::string msg_test_fail = "";

    try
    {

        gpstk::RinexMetStream FilterStream1( inputRinexMetFilterTest1.c_str() );
        FilterStream1.open( inputRinexMetFilterTest1.c_str(), std::ios::in );
        gpstk::RinexMetStream FilterStream2( inputRinexMetFilterTest2.c_str() );
        gpstk::RinexMetStream FilterStream3( inputRinexMetFilterTest3.c_str() );
        gpstk::RinexMetStream FilterStream4( inputRinexMetFilterTest4.c_str() );
        gpstk::RinexMetStream out( outputRinexMetFilterTest.c_str(), std::ios::out );

        gpstk::RinexMetHeader FilterHeader1;
        gpstk::RinexMetHeader FilterHeader2;
        gpstk::RinexMetHeader FilterHeader3;
        gpstk::RinexMetHeader FilterHeader4;

        gpstk::RinexMetData FilterData1;
        gpstk::RinexMetData FilterData2;
        gpstk::RinexMetData FilterData3;
        gpstk::RinexMetData FilterData4;

        FilterStream1 >> FilterHeader1;
        FilterStream2 >> FilterHeader2;
        FilterStream3 >> FilterHeader3;
        FilterStream4 >> FilterHeader4;

        while( FilterStream1 >> FilterData1 )
        {
        }
        while( FilterStream2 >> FilterData2 )
        {
        }
        while( FilterStream3 >> FilterData3 )
        {
        }
        while( FilterStream4 >> FilterData4 )
        {
        }

        gpstk::RinexMetHeaderTouchHeaderMerge merged;
        merged( FilterHeader1 );
        merged( FilterHeader2 );
        gpstk::RinexMetDataOperatorLessThanFull( merged.obsSet );
        out << merged.theHeader;

        gpstk::RinexMetDataOperatorEqualsSimple EqualsSimple;

	msg_test_fail = "Check to see if two equivalent files have the same times. They DO NOT.";
        test10.assert( EqualsSimple(FilterData1, FilterData2) == true, msg_test_desc + msg_test_fail, __LINE__ );

	msg_test_fail = "Check to see if two files with different times have the same time values. They DO.";
        test10.assert( EqualsSimple(FilterData1, FilterData3) == false, msg_test_desc + msg_test_fail, __LINE__ );

        gpstk::RinexMetDataOperatorLessThanSimple LessThanSimple;
	msg_test_fail = "Check to see if one file occurred earlier than another using equivalent files. One is found to be earlier than the other.";
        test10.assert( LessThanSimple(FilterData1, FilterData2) == false, msg_test_desc + msg_test_fail, __LINE__ );

	msg_test_fail = "Check to see if one file occurred earlier than another using two files with different times. The earlier file is not found to be earlier.";
        test10.assert( LessThanSimple(FilterData1, FilterData3) == true, msg_test_desc + msg_test_fail, __LINE__ );

        gpstk::RinexMetDataOperatorLessThanFull LessThanFull(merged.obsSet);

	msg_test_fail = "Perform the full less than comparison on two identical files. FilterData1 has been found to be different than FilterData2.";
        test10.assert( LessThanFull(FilterData1, FilterData2) == false, msg_test_desc + msg_test_fail, __LINE__ );

	msg_test_fail = "Perform the full less than comparison on two identical files. FilterData1 has been found to be different than FilterData2.";
        test10.assert( LessThanFull(FilterData2, FilterData1) == false, msg_test_desc + msg_test_fail, __LINE__ );

	msg_test_fail = "Perform the full less than comparison on two different files. FilterData1, an earlier date, has been found to NOT be less than FilterData3.";
        test10.assert( LessThanFull(FilterData1, FilterData3) == true, msg_test_desc + msg_test_fail, __LINE__ );

	msg_test_fail = "Perform the full less than comparison on two different files. FilterData3, a later date, has been found to be less than FilterData1.";
        test10.assert( LessThanFull(FilterData3, FilterData1) == false, msg_test_desc + msg_test_fail, __LINE__ );

        gpstk::CivilTime Start = gpstk::CommonTime::END_OF_TIME;
        gpstk::CivilTime End = gpstk::CommonTime::BEGINNING_OF_TIME;
        gpstk::CivilTime Start2 = gpstk::CommonTime::BEGINNING_OF_TIME;
        gpstk::CivilTime End2 = gpstk::CommonTime::END_OF_TIME;
        gpstk::RinexMetDataFilterTime FilterTime(Start,End);
        gpstk::RinexMetDataFilterTime FilterTime2(Start2,End2);

	msg_test_fail = "FilterTime(FilterData1) == true, should evaluate as true but evaluated as false";
        test10.assert( FilterTime(FilterData1) == true, msg_test_desc + msg_test_fail, __LINE__ );

	msg_test_fail = "FilterTime2(FilterData1) == false, should evaluate as true but evaluated as false";
        test10.assert( FilterTime2(FilterData1) == false, msg_test_desc + msg_test_fail, __LINE__ );
    }
    catch(...)
    {
        msg_test_fail = "Unexpected exception was thrown";
        test10.assert( false, msg_test_desc + msg_test_fail, __LINE__ );
    }


    return test10.countFails();
}

//============================================================
// Run all the test methods defined above
//============================================================

int main()
{
    int check, errorCounter = 0;
    RinexMet_T testClass;

    check = testClass.bitsAsStringTest();
    errorCounter += check;

    check = testClass.bitStringTest();
    errorCounter += check;

    check = testClass.reallyPutRecordTest();
    errorCounter += check;

    check = testClass.reallyGetRecordTest();
    errorCounter += check;

    check = testClass.convertObsTypeSTRTest();
    errorCounter += check;

    check = testClass.convertObsTypeHeaderTest();
    errorCounter += check;

    check = testClass.hardCodeTest();
    errorCounter += check;

    check = testClass.continuationTest();
    errorCounter += check;

    check = testClass.dataExceptionsTest();
    errorCounter += check;

    check = testClass.filterOperatorsTest();
    errorCounter += check;

    std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

    return( errorCounter );
}
