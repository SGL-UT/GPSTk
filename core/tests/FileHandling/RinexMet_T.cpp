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

#include "TestUtil.hpp"
#include <fstream>
#include <string>


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

        RinexMet_T( const std::string& data_path = "RinexMet_Logs" ):
            dataFilePath( data_path )
        {
            init();
        }

        ~RinexMet_T(){}

        void init( void );

        void bitsAsStringTest( void );
        void bitStringTest( void );
        void reallyPutRecordTest( void );
        void reallyGetRecordTest( void );
        void convertObsTypeSTRTest( void );
        void convertObsTypeHeaderTest( void );
        void hardCodeTest( void );
        void continuationTest( void );
        void dataExceptionsTest( void );
        void filterOperatorsTest( void );

    private:

        std::string dataFilePath;

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

};


//============================================================
// Test Class Definitions
//============================================================



//------------------------------------------------------------
// A test to assure that the bistAsString function works as intended
//------------------------------------------------------------

void RinexMet_T :: init( void )
{
    // Get the full file path the to source file, strip off the filename from the path
    // and then append the data file subdirectory to the path 
    std::string sourceFileName = __FILE__;
    dataFilePath = sourceFileName.substr(0, sourceFileName.find_last_of("\\/")) + "/" + dataFilePath;

    //---------------------------------------- 
    // Full file paths
    //---------------------------------------- 

    inputRinexMetNormal       = dataFilePath  + "/" + "408_110a.04m"    ;
    inputRinexMetBLL          = dataFilePath  + "/" + "BLL.04m"         ;
    inputRinexMetNMF          = dataFilePath  + "/" + "NotMetFile.04m"  ;
    inputRinexMetBOL          = dataFilePath  + "/" + "BOL.04m"         ;
    inputRinexMetFER          = dataFilePath  + "/" + "FER.04m"         ;
    inputRinexMetExtraH       = dataFilePath  + "/" + "ExtraH.04m"      ;
    inputRinexMetUnSupRinex   = dataFilePath  + "/" + "UnSupRinex.04m"  ;
    inputRinexMetMissingSPos  = dataFilePath  + "/" + "MissingEOH.04m"  ;
    inputRinexMetContLines    = dataFilePath  + "/" + "ContLines10.04m" ;
    inputRinexMetNoObs        = dataFilePath  + "/" + "NoObsData.04m"   ;
    inputRinexMetSensorType   = dataFilePath  + "/" + "SensorTypeError.04m" ;
    inputRinexMetObsHdrStrErr = dataFilePath  + "/" + "ObsHeaderStringError.04m" ;
    inputRinexMetMissingMkr   = dataFilePath  + "/" + "MissingMarkerName";
    inputRinexMetInvTimeFmt   = dataFilePath  + "/" + "InvalidTimeFormat.04m";
    inputRinexMetFilterTest1  = dataFilePath  + "/" + "FilterTest1.04m"  ;
    inputRinexMetFilterTest2  = dataFilePath  + "/" + "FilterTest2.04m"  ;
    inputRinexMetFilterTest3  = dataFilePath  + "/" + "FilterTest3.04m"  ;
    inputRinexMetFilterTest4  = dataFilePath  + "/" + "FilterTest4.04m"  ;

    outputRinexMetHardCode    = dataFilePath  + "/" + "Output.txt";
    outputRinexMetExtraOutput = dataFilePath  + "/" + "ExtraOutput.txt";
    outputRinexMetContLines   = dataFilePath  + "/" + "OutputCont.txt";
    outputRinexMetDumps       = dataFilePath  + "/" + "MetDumps";
    outputRinexMetExceptions  = dataFilePath  + "/" + "OutputDataExceptions.txt";
    outputRinexMetFilterTest  = dataFilePath  + "/" + "FilterOutput.txt" ;


}

//------------------------------------------------------------
// A test to assure that the bistAsString function works as intended
//------------------------------------------------------------

void RinexMet_T :: bitsAsStringTest( void )
{
    TestUtil test1( "RinexMetHeader", "bitsAsString", __FILE__, __LINE__ );

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

    test1.next();
    test1.assert( expected_string_a == RinexMetHeader.bitsAsString(RinexMetHeader.validVersion)      );
    test1.next();
    test1.assert( expected_string_b == RinexMetHeader.bitsAsString(RinexMetHeader.validRunBy)        );
    test1.next();
    test1.assert( expected_string_c == RinexMetHeader.bitsAsString(RinexMetHeader.validComment)      );
    test1.next();
    test1.assert( expected_string_d == RinexMetHeader.bitsAsString(RinexMetHeader.validMarkerName)   );
    test1.next();
    test1.assert( expected_string_e == RinexMetHeader.bitsAsString(RinexMetHeader.validMarkerNumber) );
    test1.next();
    test1.assert( expected_string_f == RinexMetHeader.bitsAsString(RinexMetHeader.validObsType)      );
    test1.next();
    test1.assert( expected_string_g == RinexMetHeader.bitsAsString(RinexMetHeader.validSensorType)   );
    test1.next();
    test1.assert( expected_string_h == RinexMetHeader.bitsAsString(RinexMetHeader.validSensorPos)    );
    test1.next();
    test1.assert( expected_string_i == RinexMetHeader.bitsAsString(RinexMetHeader.validEoH)          );

    //Defult Case
    test1.next();
    test1.assert( expected_string_z == RinexMetHeader.bitsAsString(RinexMetHeader.allValid21) );

}

//------------------------------------------------------------
// A test to assure that the version validity bits are what we expect them to be
//------------------------------------------------------------
void RinexMet_T :: bitStringTest( void )
{
    TestUtil test2( "RinexMetHeader", "bitString", __FILE__, __LINE__ );

    gpstk::RinexMetHeader RinexMetHeader;

    std::string sep=", ";
    std::string expected_string_a = "\"RINEX VERSION / TYPE\"";
    std::string expected_string_b = "\"RINEX VERSION / TYPE\", \"PGM / RUN BY / DATE\", \"MARKER NAME\", \"# / TYPES OF OBSERV\", \"SENSOR MOD/TYPE/ACC\", \"SENSOR POS XYZ/H\", \"END OF HEADER\"";

    test2.next();
    test2.assert( expected_string_a == RinexMetHeader.bitString( RinexMetHeader.validVersion, '\"', sep ) );

    test2.next();
    test2.assert( expected_string_b == RinexMetHeader.bitString( RinexMetHeader.allValid21, '\"', sep ) );

    test2.next();
    test2.assert( expected_string_b == RinexMetHeader.bitString( RinexMetHeader.allValid20, '\"', sep ) );

    // test2.next();
    // test2.assert( expected_string_b, RinexMetHeader.bitString(RinexMetHeader.allValid20,' ',sep) );

}

//------------------------------------------------------------
// A small test of the reallyPutRecord member of the RinexMetHeader 
// with a few execptions such as an Unsupported Rinex version (e.g. 3.33)
//  and a Missing Marker Name
//------------------------------------------------------------
void RinexMet_T :: reallyPutRecordTest( void )
{
    TestUtil test3( "RinexMetHeader", "exceptions", __FILE__, __LINE__ );

    gpstk::RinexMetHeader RinexMetHeader;

    gpstk::RinexMetStream UnSupRinex( inputRinexMetUnSupRinex.c_str() );
    gpstk::RinexMetStream MissingMarkerName( inputRinexMetMissingMkr.c_str() );
    gpstk::RinexMetStream output( outputRinexMetExtraOutput.c_str(), std::ios::out );

    output.exceptions( std::fstream::failbit );

    try { UnSupRinex >> RinexMetHeader; test3.failTest(); }
    catch(gpstk::Exception e) { test3.passTest(); }
    catch(...) { test3.failTest(); }

    try { MissingMarkerName >> RinexMetHeader; test3.failTest(); }
    catch(gpstk::Exception e) { test3.passTest(); }
    catch(...) { test3.failTest(); }

}

// ------------------------------------------------------------
// This function reads the RINEX MET header from the given FFStream.
// If an error is encountered in reading form the stream, the stream
// is reset to its original position and its fail-bit is set.
// ------------------------------------------------------------
void RinexMet_T :: reallyGetRecordTest( void )
{
    TestUtil test4( "RinexMetHeader", "exceptions", __FILE__, __LINE__ );

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


    // Fail any of the following tests which does NOT throw a gpstk::Exception

    try { ExtraH >> RinexMetHeader; test4.failTest(); }
    catch(gpstk::Exception e) { test4.passTest(); }
    catch(...) { test4.failTest(); }

    try { UnSupRinex >> RinexMetHeader ; test4.failTest(); }
    catch(gpstk::Exception e) { test4.passTest(); }
    catch(...) { test4.failTest(); }

    try {  MissingSPos >> RinexMetHeader   ; test4.failTest(); }
    catch(gpstk::Exception e) { test4.passTest(); }
    catch(...) { test4.failTest(); }

    try {  ObsHeaderStringError >> RinexMetHeader   ; test4.failTest(); }
    catch(gpstk::Exception e) { test4.passTest(); }
    catch(...) { test4.failTest(); }

    try { SensorType >> RinexMetHeader    ; test4.failTest(); }
    catch(gpstk::Exception e) { test4.passTest(); }
    catch(...) { test4.failTest(); }

    try { BLL >> RinexMetHeader    ; test4.failTest(); }
    catch(gpstk::Exception e) { test4.passTest(); }
    catch(...) { test4.failTest(); }

    try { NMF >> RinexMetHeader     ; test4.failTest(); }
    catch(gpstk::Exception e) { test4.passTest(); }
    catch(...) { test4.failTest(); }

    try { BOL >> RinexMetHeader    ; test4.failTest(); }
    catch(gpstk::Exception e) { test4.passTest(); }
    catch(...) { test4.failTest(); }


    // Fail the follow test if it throws ANYTHING!

    try { Normal >> RinexMetHeader ; test4.passTest(); }
    catch(gpstk::Exception e) { test4.failTest(); }
    catch(...) { test4.failTest(); }

}

//------------------------------------------------------------
// This test tests the convertObsType member of RinexMetHeader
// This particular test asserts if two Met Types are equal
//------------------------------------------------------------

void RinexMet_T :: convertObsTypeSTRTest( void )
{
    TestUtil test5( "RinexMetHeader", "convertObsType", __FILE__, __LINE__ );

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

    test5.assert( PR == RinexMetHeader.convertObsType(PRS) ); 
    test5.next();
    test5.assert( TD == RinexMetHeader.convertObsType(TDS) ); 
    test5.next();
    test5.assert( HR == RinexMetHeader.convertObsType(HRS) ); 
    test5.next();
    test5.assert( ZW == RinexMetHeader.convertObsType(ZWS) ); 
    test5.next();
    test5.assert( ZD == RinexMetHeader.convertObsType(ZDS) ); 
    test5.next();
    test5.assert( ZT == RinexMetHeader.convertObsType(ZTS) ); 
    test5.next();
    test5.assert( HI == RinexMetHeader.convertObsType(HIS) );

}

//------------------------------------------------------------
//
// This test tests the convertObsType member of RinexMetHeader
// This particular test asserts if two strings are equal
//
//------------------------------------------------------------
void RinexMet_T :: convertObsTypeHeaderTest( void )
{
    TestUtil test6( "RinexMetHeader", "convertObsType", __FILE__, __LINE__ );

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

    test6.assert( PRS == RinexMetHeader.convertObsType(PR) );
    test6.next();
    test6.assert( TDS == RinexMetHeader.convertObsType(TD) );
    test6.next();
    test6.assert( HRS == RinexMetHeader.convertObsType(HR) );
    test6.next();
    test6.assert( ZWS == RinexMetHeader.convertObsType(ZW) );
    test6.next();
    test6.assert( ZDS == RinexMetHeader.convertObsType(ZD) );
    test6.next();
    test6.assert( ZTS == RinexMetHeader.convertObsType(ZT) );
    test6.next();

    try { RinexMetHeader.convertObsType( "KE" )   ; test6.failTest(); }
    catch(gpstk::FFStreamError) { test6.passTest(); }
    catch(...) { test6.failTest(); }

}

//------------------------------------------------------------
//
// This test checks to make sure that the internal members of 
// the RinexMetHeader are as we think they should be.
// Also at the end of this test, we check and make sure our 
// output file is equal to our input
//
//------------------------------------------------------------
void RinexMet_T :: hardCodeTest( void )
{
    TestUtil test7( "RinexMetHeader", "version", __FILE__, __LINE__ );

    gpstk::RinexMetStream testRinexMetStream( inputRinexMetNormal.c_str() );
    gpstk::RinexMetStream out( outputRinexMetHardCode.c_str(), std::ios::out );
    gpstk::RinexMetHeader testRinexMetHeader;

    testRinexMetStream >> testRinexMetHeader;
 
    //Start of RinexMetHeader member check to assure that what we want is whats in there
    test7.assert( testRinexMetHeader.version     == 2.1 );
    test7.next();
    test7.assert( testRinexMetHeader.fileType    == (std::string)"Meteorological" );
    test7.next();
    test7.assert( testRinexMetHeader.fileProgram == (std::string)"GFW - RMW" );
    test7.next();
    test7.assert( testRinexMetHeader.fileAgency  == (std::string)"NIMA" );
    test7.next();
    test7.assert( testRinexMetHeader.date        == (std::string)"04/18/2004 23:58:50" );
    test7.next();

    std::vector<std::string>::const_iterator itr1 = testRinexMetHeader.commentList.begin();
    while( itr1 != testRinexMetHeader.commentList.end() )
    {
        test7.assert( (*itr1) == (std::string)"Some weather data may have corrected values" );
        test7.next();
        itr1++;
    }

    test7.assert( testRinexMetHeader.markerName   == (std::string)"85408" );
    test7.next();

    test7.assert( testRinexMetHeader.markerNumber == (std::string)"85408" );
    test7.next();

    vector<RinexMetHeader::RinexMetType>::const_iterator itr2 = testRinexMetHeader.obsTypeList.begin();

    if( itr2 != testRinexMetHeader.obsTypeList.end() )
    {
       test7.assert( testRinexMetHeader.convertObsType(*itr2) == (std::string)"PR" );
       test7.next();
       itr2++;
    } else {
      test7.failTest();
    }

    if( itr2 != testRinexMetHeader.obsTypeList.end() )
    {
        test7.assert( testRinexMetHeader.convertObsType(*itr2) == (std::string)"TD" );
        test7.next();
        itr2++;
    } else {
      test7.failTest();
    }

    if( itr2 != testRinexMetHeader.obsTypeList.end() )
    {
        test7.assert( testRinexMetHeader.convertObsType(*itr2) == (std::string)"HI" );
        test7.next();
    } else {
      test7.failTest();
    }

    vector<RinexMetHeader::sensorType>::const_iterator itr3 = testRinexMetHeader.sensorTypeList.begin();

    if( itr3 != testRinexMetHeader.sensorTypeList.end() )
    {
        test7.assert( (*itr3).model    == (std::string)"Vaisala" );
        test7.next();
        test7.assert( (*itr3).type     == (std::string)"PTB220"  );
        test7.next();
        test7.assert( (*itr3).accuracy == 0.1 );
        test7.next();
        test7.assert( testRinexMetHeader.convertObsType((*itr3).obsType) == (std::string)"PR" );
        test7.next();

        itr3++;

    } else {
        // Must fail all four tests above
        test7.failTest();
        test7.failTest();
        test7.failTest();
        test7.failTest();
    }

    if( itr3 != testRinexMetHeader.sensorTypeList.end() )
    {
        test7.assert( (*itr3).model    == (std::string)"Vaisala" );
        test7.next();
        test7.assert( (*itr3).type     == (std::string)"HMP230"  );
        test7.next();
        test7.assert( (*itr3).accuracy == 0.1 );
        test7.next();
        test7.assert( testRinexMetHeader.convertObsType((*itr3).obsType) == (std::string)"TD" );
        test7.next();

        itr3++;

    } else {
        // Must fail all four tests above
        test7.failTest();
        test7.failTest();
        test7.failTest();
        test7.failTest();
    }

    if( itr3 != testRinexMetHeader.sensorTypeList.end() )
    {
        test7.assert( (*itr3).model    == (std::string)"Vaisala" );
        test7.next();
        test7.assert( (*itr3).type     == (std::string)"HMP230"  );
        test7.next();
        test7.assert( (*itr3).accuracy == 0.1 );
        test7.next();
        test7.assert( testRinexMetHeader.convertObsType((*itr3).obsType) == (std::string)"HI" );
        test7.next();

        itr3++;

    } else {
        // Must fail all four tests above
        test7.failTest();
        test7.failTest();
        test7.failTest();
        test7.failTest();
    }

    vector<RinexMetHeader::sensorPosType>::const_iterator itr4 = testRinexMetHeader.sensorPosList.begin();

    if( itr4 != testRinexMetHeader.sensorPosList.end() )
    {
        test7.assert( (*itr4).position[0] ==  -740289.8363 );
        test7.next();
        test7.assert( (*itr4).position[1] == -5457071.7414 );
        test7.next();
        test7.assert( (*itr4).position[2] ==  3207245.6207 );
        test7.next();
        test7.assert( (*itr4).height      ==        0.0000 );
        test7.next();
        test7.assert( testRinexMetHeader.convertObsType((*itr4).obsType) == (std::string) "PR" );
        test7.next();

        itr4++;

    } else {
        // Must fail all five tests above
        test7.failTest();
        test7.failTest();
        test7.failTest();
        test7.failTest();
        test7.failTest();
    }

    if( itr4 != testRinexMetHeader.sensorPosList.end() )
    {
        test7.assert( (*itr4).position[0] ==  -740289.8363 );
        test7.next();
        test7.assert( (*itr4).position[1] == -5457071.7414 );
        test7.next();
        test7.assert( (*itr4).position[2] ==  3207245.6207 );
        test7.next();
        test7.assert( (*itr4).height      ==        0.0000 );
        test7.next();
        test7.assert( testRinexMetHeader.convertObsType((*itr4).obsType) == (std::string) "TD" );
        test7.next();
    } else {
        // Must fail all five tests above
        test7.failTest();
        test7.failTest();
        test7.failTest();
        test7.failTest();
        test7.failTest();
    }
    //End of Header

    // Now test the RinexMetData
    out << testRinexMetHeader;
    gpstk::RinexMetData testRinexMetData;
    testRinexMetStream >> testRinexMetData;
    gpstk::CivilTime TimeGuess(2004,4,19,0,0,0);


    test7.assert( testRinexMetData.time == (gpstk::CommonTime)TimeGuess );
    test7.next();
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "PR" )] == 992.6 );
    test7.next();
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "TD" )] ==  23.9 );
    test7.next();
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "HI" )] ==  59.7 );
    test7.next();

    out << testRinexMetData;
    testRinexMetStream >> testRinexMetData;
    gpstk::CivilTime TimeGuess2(2004,4,19,0,15,0);

    test7.assert( testRinexMetData.time == (gpstk::CommonTime)TimeGuess2 );
    test7.next();
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "PR" )] == 992.8 );
    test7.next();
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "TD" )] ==  23.6 );
    test7.next();
    test7.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "HI" )] ==  61.6 );
    test7.next();

    out << testRinexMetData;
    while( testRinexMetStream >> testRinexMetData )
    {
        out << testRinexMetData;
    }

    test7.assert( test7.fileEqualTest( inputRinexMetNormal, outputRinexMetHardCode, 2) );
    test7.next();

    gpstk::RinexMetStream MetDumps( outputRinexMetDumps.c_str(), std::ios::out );
    testRinexMetHeader.dump( MetDumps );
    testRinexMetData.dump( MetDumps );
}

//------------------------------------------------------------
//
// This test covers are of the RinexMetHeader and Data which 
// deal with continuation lines for the observations
//
//------------------------------------------------------------

void RinexMet_T :: continuationTest( void )
{
    TestUtil test8( "RinexMetHeader", "continuation", __FILE__, __LINE__ );

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

        test8.assert( test8.fileEqualTest( inputRinexMetContLines, outputRinexMetContLines, 2) );
        test8.next();
    }
    catch(...)
    {
        test8.failTest();
    }
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
void RinexMet_T :: dataExceptionsTest( void )
{
    TestUtil test9( "RinexMetHeader", "nodata", __FILE__, __LINE__ );
 
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

        test9.failTest();
    }
    catch( gpstk::Exception& e )
    {
        test9.passTest();
    }
    catch(...)
    {
        test9.failTest();
    }
}

//------------------------------------------------------------
//
// Test for several of the members within RinexMetFilterOperators 
// including merge, LessThanSimple, EqualsSimple,
// and LessThanFull.
//
//------------------------------------------------------------
void RinexMet_T :: filterOperatorsTest( void )
{
    TestUtil test10( "RinexMetStream", "filter", __FILE__, __LINE__ );
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

        test10.assert( EqualsSimple(FilterData1, FilterData2) == true );
        test10.next();
        test10.assert( EqualsSimple(FilterData1, FilterData3) == false );
        test10.next();

        gpstk::RinexMetDataOperatorLessThanSimple LessThanSimple;
        test10.assert( LessThanSimple(FilterData1, FilterData2) == false );
        test10.next();
        test10.assert( LessThanSimple(FilterData1, FilterData3) == true );
        test10.next();

        gpstk::RinexMetDataOperatorLessThanFull LessThanFull(merged.obsSet);

        test10.assert( LessThanFull(FilterData1, FilterData2) == false );
        test10.next();
        test10.assert( LessThanFull(FilterData2, FilterData1) == false );
        test10.next();
        test10.assert( LessThanFull(FilterData1, FilterData3) == true );
        test10.next();
        test10.assert( LessThanFull(FilterData3, FilterData1) == false );
        test10.next();

        gpstk::CivilTime Start = gpstk::CommonTime::END_OF_TIME;
        gpstk::CivilTime End = gpstk::CommonTime::BEGINNING_OF_TIME;
        gpstk::CivilTime Start2 = gpstk::CommonTime::BEGINNING_OF_TIME;
        gpstk::CivilTime End2 = gpstk::CommonTime::END_OF_TIME;
        gpstk::RinexMetDataFilterTime FilterTime(Start,End);
        gpstk::RinexMetDataFilterTime FilterTime2(Start2,End2);

        test10.assert( FilterTime(FilterData1) == true );
        test10.next();
        test10.assert( FilterTime2(FilterData1) == false );
        test10.next();
    }
    catch(...)
    {
        test10.failTest();

    }

}

//============================================================
// Run all the test methods defined above
//============================================================

int main()
{

    RinexMet_T testClass;

    testClass.bitsAsStringTest();
    testClass.bitStringTest();
    testClass.reallyPutRecordTest();
    testClass.reallyGetRecordTest();
    testClass.convertObsTypeSTRTest();
    testClass.convertObsTypeHeaderTest();
    testClass.hardCodeTest();
    testClass.continuationTest();
    testClass.dataExceptionsTest();
    testClass.filterOperatorsTest();

    return( 0 );
}
