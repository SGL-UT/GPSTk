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

   ~RinexMet_T() {}

   void init();

   int openMissingFileTest();
   int bitsAsStringTest();
   int bitStringTest();
   int reallyPutRecordTest();
   int reallyGetRecordTest();
   int convertObsTypeSTRTest();
   int convertObsTypeHeaderTest();
   int hardCodeTest();
   int continuationTest();
   int dataExceptionsTest();
   int filterOperatorsTest();

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

   std::string inputNormal;
   std::string inputMissing;
   std::string inputBLL;
   std::string inputNMF;
   std::string inputBOL;
   std::string inputFER;
   std::string inputExtraH;
   std::string inputUnSupRinex;
   std::string inputMissingSPos;
   std::string inputContLines;
   std::string inputNoObs;
   std::string inputSensorType;

   std::string inputObsHdrStrErr;
   std::string inputMissingMkr;
   std::string inputInvTimeFmt;
   std::string inputFilterTest1;
   std::string inputFilterTest2;
   std::string inputFilterTest3;
   std::string inputFilterTest4;

   std::string outputHardCode;
   std::string outputExtraOutput;
   std::string outputContLines;
   std::string outputDumps;
   std::string outputExceptions;
   std::string outputFilterTest;

   std::string failDescriptionString;
   std::stringstream failDescriptionStream;

};


//============================================================
// Test Class Definitions
//============================================================



//------------------------------------------------------------
// A test to assure that the bistAsString function works as intended
//------------------------------------------------------------

void RinexMet_T :: init()
{
   TestUtil test0;
   std::string dataFilePath = gpstk::getPathData();
   std::string tempFilePath = gpstk::getPathTestTemp();

      //----------------------------------------
      // Full file paths
      //----------------------------------------
   std::string fileSep = gpstk::getFileSep();
   std::string dp = dataFilePath  + fileSep;
   std::string tp = tempFilePath  + fileSep;

   inputNormal       = dp + "test_input_rinex_met_408_110a.04m";
   inputMissing      = dp + "not-a-real-file-sasquatch.moo";
   inputBLL          = dp + "test_input_rinex_met_BLL.04m";
   inputNMF          = dp + "test_input_rinex_met_NotMetFile.04m";
   inputBOL          = dp + "test_input_rinex_met_BOL.04m";
   inputFER          = dp + "test_input_rinex_met_FER.04m";
   inputExtraH       = dp + "test_input_rinex_met_ExtraH.04m";
   inputUnSupRinex   = dp + "test_input_rinex_met_UnSupRinex.04m";
   inputMissingSPos  = dp + "test_input_rinex_met_MissingEOH.04m";
   inputContLines    = dp + "test_input_rinex_met_ContLines10.04m";
   inputNoObs        = dp + "test_input_rinex_met_NoObsData.04m";
   inputSensorType   = dp + "test_input_rinex_met_SensorTypeError.04m";
   inputObsHdrStrErr = dp + "test_input_rinex_met_ObsHeaderStringError.04m";
   inputMissingMkr   = dp + "test_input_rinex_met_MissingMarkerName";
   inputInvTimeFmt   = dp + "test_input_rinex_met_InvalidTimeFormat.04m";
   inputFilterTest1  = dp + "test_input_rinex_met_Filter1.04m";
   inputFilterTest2  = dp + "test_input_rinex_met_Filter2.04m";
   inputFilterTest3  = dp + "test_input_rinex_met_Filter3.04m";

   outputHardCode    = tp + "test_output_rinex_met_Output.txt";
   outputExtraOutput = tp + "test_output_rinex_met_ExtraOutput.txt";
   outputContLines   = tp + "test_output_rinex_met_Cont.txt";
   outputDumps       = tp + "test_output_rinex_met_Dumps.txt";
   outputExceptions  = tp + "test_output_rinex_met_DataExceptions.txt";
   outputFilterTest  = tp + "test_output_rinex_met_Filter.txt";
}


int RinexMet_T :: openMissingFileTest()
{
   TUDEF("RinexMetStream", "open");

   ifstream fstr(inputMissing.c_str(), ios::in);
   testFramework.assert(!fstr,
                        "std::ifstream marked good for non-existent file",
                        __LINE__);
   fstr.close();

   RinexMetStream mstr(inputMissing.c_str(), ios::in);
   testFramework.assert(!mstr,
                        "RinexMetStream marked good for non-existent file",
                        __LINE__);
   mstr.close();

   return testFramework.countFails();
}


//------------------------------------------------------------
// A test to assure that the bistAsString function works as intended
//------------------------------------------------------------

int RinexMet_T :: bitsAsStringTest()
{
   TUDEF( "RinexMetHeader", "bitsAsString" );

   std::string test_desc =
      "RinexMetHeader, bitsAsString, file read compared to expected string, did not match";

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

   testFramework.assert( expected_string_a == RinexMetHeader.bitsAsString(
                            RinexMetHeader.validVersion)      , test_desc, __LINE__ );
   testFramework.assert( expected_string_b == RinexMetHeader.bitsAsString(
                            RinexMetHeader.validRunBy)        , test_desc, __LINE__ );
   testFramework.assert( expected_string_c == RinexMetHeader.bitsAsString(
                            RinexMetHeader.validComment)      , test_desc, __LINE__ );
   testFramework.assert( expected_string_d == RinexMetHeader.bitsAsString(
                            RinexMetHeader.validMarkerName)   , test_desc, __LINE__ );
   testFramework.assert( expected_string_e == RinexMetHeader.bitsAsString(
                            RinexMetHeader.validMarkerNumber) , test_desc, __LINE__ );
   testFramework.assert( expected_string_f == RinexMetHeader.bitsAsString(
                            RinexMetHeader.validObsType)      , test_desc, __LINE__ );
   testFramework.assert( expected_string_g == RinexMetHeader.bitsAsString(
                            RinexMetHeader.validSensorType)   , test_desc, __LINE__ );
   testFramework.assert( expected_string_h == RinexMetHeader.bitsAsString(
                            RinexMetHeader.validSensorPos)    , test_desc, __LINE__ );
   testFramework.assert( expected_string_i == RinexMetHeader.bitsAsString(
                            RinexMetHeader.validEoH)          , test_desc, __LINE__ );
      //Default Case
   testFramework.assert( expected_string_z == RinexMetHeader.bitsAsString(
                            RinexMetHeader.allValid21)        , test_desc, __LINE__ );

   return( testFramework.countFails() );
}

//------------------------------------------------------------
// A test to assure that the version validity bits are what we expect them to be
//------------------------------------------------------------
int RinexMet_T :: bitStringTest()
{
   TUDEF( "RinexMetHeader", "bitString" );

   std::string test_desc =
      "RinexMetHeader, bitString, test to assure that the version validity bits are what we expect them to be";
   std::string test_fail = "";

   gpstk::RinexMetHeader RinexMetHeader;

   std::string sep=", ";
   std::string expected_string_a = "\"RINEX VERSION / TYPE\"";
   std::string expected_string_b =
      "\"RINEX VERSION / TYPE\", \"PGM / RUN BY / DATE\", \"MARKER NAME\", \"# / TYPES OF OBSERV\", \"SENSOR MOD/TYPE/ACC\", \"SENSOR POS XYZ/H\", \"END OF HEADER\"";

   test_fail = ", validVersion failed";
   testFramework.assert( expected_string_a == RinexMetHeader.bitString(
                            RinexMetHeader.validVersion, '\"', sep ), test_desc + test_fail, __LINE__ );

   test_fail = ", allValid21 failed";
   testFramework.assert( expected_string_b == RinexMetHeader.bitString(
                            RinexMetHeader.allValid21, '\"', sep ),   test_desc + test_fail, __LINE__ );

   test_fail = ", allValid20 failed";
   testFramework.assert( expected_string_b == RinexMetHeader.bitString(
                            RinexMetHeader.allValid20, '\"', sep ),   test_desc + test_fail, __LINE__ );
      // testFramework.assert( expected_string_b, RinexMetHeader.bitString(RinexMetHeader.allValid20,' ',sep),   test_desc + test_fail, __LINE__ );

   return testFramework.countFails();
}

//------------------------------------------------------------
// A small test of the reallyPutRecord member of the RinexMetHeader
// with a few execptions such as an Unsupported Rinex version (e.g. 3.33)
//  and a Missing Marker Name
//------------------------------------------------------------
int RinexMet_T :: reallyPutRecordTest()
{
   TUDEF( "RinexMetHeader", "exceptions" );

   std::string msg_test_desc   =
      "RinexMetHeader, reallyPutRecordTest, exception tests";
   std::string msg_false_pass  =
      ", should have thrown a gpstk::Exception but did not.";
   std::string msg_fail        =
      ", should have thrown a gpstk::Exception but threw an unexpected exception.";
   std::string msg_test_detail = "";

   gpstk::RinexMetHeader RinexMetHeader;

   gpstk::RinexMetStream UnSupRinex( inputUnSupRinex.c_str() );
   gpstk::RinexMetStream MissingMarkerName( inputMissingMkr.c_str() );
   gpstk::RinexMetStream output( outputExtraOutput.c_str(),
                                 std::ios::out );

   output.exceptions( std::fstream::failbit );
   UnSupRinex.exceptions( std::fstream::failbit );
   MissingMarkerName.exceptions( std::fstream::failbit );

      //------------------------------------------------------------
      // Unsupported Rinex
      //------------------------------------------------------------
   msg_test_detail = ", Unsupported Rinex version";

   try
   {
      UnSupRinex >> RinexMetHeader;
      TUFAIL( msg_test_desc + msg_test_detail + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS( msg_test_desc + msg_test_detail );
   }
   catch(...)
   {
      TUFAIL( msg_test_desc + msg_test_detail + msg_fail );
   }

      //------------------------------------------------------------
      // Missing Marker
      //------------------------------------------------------------
   msg_test_detail = ", Missing Marker Name";
   try
   {
      MissingMarkerName >> RinexMetHeader;
      TUFAIL( msg_test_desc + msg_test_detail + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS( msg_test_desc + msg_test_detail );
   }
   catch(...)
   {
      TUFAIL( msg_test_desc + msg_test_detail + msg_fail );
   }

   return testFramework.countFails();
}

// ------------------------------------------------------------
// This function reads the RINEX MET header from the given FFStream.
// If an error is encountered in reading form the stream, the stream
// is reset to its original position and its fail-bit is set.
// ------------------------------------------------------------
int RinexMet_T :: reallyGetRecordTest()
{
   TUDEF( "RinexMetHeader", "exceptions" );

   std::string msg_test_desc = "";
   std::string msg_false_pass  =
      ", should have thrown a gpstk::Exception but did not.";
   std::string msg_fail        =
      ", should have thrown a gpstk::Exception but threw an unexpected exception.";



      //Header file we will be testing on
   gpstk::RinexMetHeader RinexMetHeader;

      //Normal Met File (No changes, straight from the archive)
   gpstk::RinexMetStream Normal( inputNormal.c_str() );

      //Bad Line Length Met file (Add a space characacter to the end of the first line)
   gpstk::RinexMetStream BLL( inputBLL.c_str() );

      //Not a Met File (Change Met to zet in first line)
   gpstk::RinexMetStream NMF( inputNMF.c_str() );

      //Bad Obs line (Add in something between the MET obs besides space)
   gpstk::RinexMetStream BOL( inputBOL.c_str() );

      //Bad Sesor Type line, Format Error
   gpstk::RinexMetStream FER( inputFER.c_str() );

      //Extra header line with HEADERLINE2
   gpstk::RinexMetStream ExtraH( inputExtraH.c_str() );

      //Unsupported Rinex Version (3.30)
   gpstk::RinexMetStream UnSupRinex( inputUnSupRinex.c_str() );

      //Missing SensorPos
   gpstk::RinexMetStream MissingSPos( inputMissingSPos.c_str() );

      //Error in the part of the header which holds the list of observations within the Met file
   gpstk::RinexMetStream ObsHeaderStringError(
      inputObsHdrStrErr.c_str() );

      //Error in the sensor type
   gpstk::RinexMetStream SensorType( inputSensorType.c_str() );

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
      TUFAIL(msg_test_desc + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS(msg_test_desc );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail );
   }

      //----------------------------------------
      //----------------------------------------
   msg_test_desc = "UnSupRinex >> RinexMetHeader";
   try
   {
      UnSupRinex >> RinexMetHeader;
      TUFAIL(msg_test_desc + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS(msg_test_desc );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail );
   }

      //----------------------------------------
      //----------------------------------------
   msg_test_desc = "MissingSPos >> RinexMetHeader";
   try
   {
      MissingSPos >> RinexMetHeader;
      TUFAIL(msg_test_desc + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS(msg_test_desc );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail );
   }

      //----------------------------------------
      //----------------------------------------
   msg_test_desc = "ObsHeaderStringError >> RinexMetHeader";
   try
   {
      ObsHeaderStringError >> RinexMetHeader;
      TUFAIL(msg_test_desc + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS(msg_test_desc );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail );
   }

      //----------------------------------------
      //----------------------------------------
   msg_test_desc = "SensorType >> RinexMetHeader";
   try
   {
      SensorType >> RinexMetHeader;
      TUFAIL(msg_test_desc + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS(msg_test_desc );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail );
   }

      //----------------------------------------
      //----------------------------------------
   msg_test_desc = "BLL >> RinexMetHeader";
   try
   {
      BLL >> RinexMetHeader;
      TUFAIL(msg_test_desc + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS(msg_test_desc );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail );
   }

      //----------------------------------------
      //----------------------------------------
   msg_test_desc = "NMF >> RinexMetHeader";
   try
   {
      NMF >> RinexMetHeader;
      TUFAIL(msg_test_desc + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS(msg_test_desc );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail );
   }

      //----------------------------------------
      //----------------------------------------
   msg_test_desc = "BOL >> RinexMetHeader";
   try
   {
      BOL >> RinexMetHeader;
      TUFAIL(msg_test_desc + msg_false_pass );
   }
   catch( gpstk::Exception e )
   {
      TUPASS(msg_test_desc );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail );
   }


      //----------------------------------------
      // Fail the follow test if it throws ANYTHING!
      //----------------------------------------
   msg_test_desc = "Normal >> RinexMetHeader";
   std::string msg_fail_gpstk =
      ", should have thrown nothing, but threw a gpstk::Exception.";
   std::string msg_fail_other =
      ", should have thrown nothing, but threw an exception.";
   try
   {
      Normal >> RinexMetHeader;
      TUPASS(msg_test_desc );
   }
   catch( gpstk::Exception e)
   {
      cout << e << endl;
      TUFAIL(msg_test_desc + msg_fail_gpstk );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail_other );
   }



   return testFramework.countFails();
}

//------------------------------------------------------------
// This test tests the convertObsType member of RinexMetHeader
// This particular test asserts if two Met Types are equal
//------------------------------------------------------------

int RinexMet_T :: convertObsTypeSTRTest()
{
   TUDEF( "RinexMetHeader", "convertObsType" );
   std::string msg_test_desc = "convertObsTypeSTRTest, ";
   std::string msg_test_fail = "";

   gpstk::RinexMetHeader RinexMetHeader;
   gpstk::RinexMetStream Normal( inputNormal.c_str() );

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
   testFramework.assert( PR == RinexMetHeader.convertObsType(PRS),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(TDS) did not return expected value";
   testFramework.assert( TD == RinexMetHeader.convertObsType(TDS),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(HRS) did not return expected value";
   testFramework.assert( HR == RinexMetHeader.convertObsType(HRS),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(ZWS) did not return expected value";
   testFramework.assert( ZW == RinexMetHeader.convertObsType(ZWS),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(ZDS) did not return expected value";
   testFramework.assert( ZD == RinexMetHeader.convertObsType(ZDS),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(ZTS) did not return expected value";
   testFramework.assert( ZT == RinexMetHeader.convertObsType(ZTS),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(HIS) did not return expected value";
   testFramework.assert( HI == RinexMetHeader.convertObsType(HIS),
                         msg_test_desc + msg_test_fail, __LINE__ );



   return testFramework.countFails();
}

//------------------------------------------------------------
//
// This test tests the convertObsType member of RinexMetHeader
// This particular test asserts if two strings are equal
//
//------------------------------------------------------------
int RinexMet_T :: convertObsTypeHeaderTest()
{
   TUDEF( "RinexMetHeader", "convertObsType" );
   std::string msg_test_desc = "convertObsTypeHeaderTest, ";
   std::string msg_test_fail = "";

   gpstk::RinexMetHeader RinexMetHeader;
   gpstk::RinexMetStream Normal( inputNormal.c_str() );

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
   testFramework.assert( PRS == RinexMetHeader.convertObsType(PR),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(TD) did not return expected value";
   testFramework.assert( TDS == RinexMetHeader.convertObsType(TD),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(HR) did not return expected value";
   testFramework.assert( HRS == RinexMetHeader.convertObsType(HR),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(ZW) did not return expected value";
   testFramework.assert( ZWS == RinexMetHeader.convertObsType(ZW),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(ZD) did not return expected value";
   testFramework.assert( ZDS == RinexMetHeader.convertObsType(ZD),
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "convertObsType(ZT) did not return expected value";
   testFramework.assert( ZTS == RinexMetHeader.convertObsType(ZT),
                         msg_test_desc + msg_test_fail, __LINE__ );


      //----------------------------------------
      // Fail if the following throws anything but a FFStreamError
      //----------------------------------------
   msg_test_desc = "convertObsType( \"KE\" ), should throw  gpstk::FFStreamError";
   std::string msg_false_pass = ", but threw no exceptions.";
   std::string msg_fail_other = ", but threw a different exception.";
   try
   {
      RinexMetHeader.convertObsType( "KE" );
      TUFAIL(msg_test_desc + msg_false_pass );
   }
   catch(gpstk::FFStreamError)
   {
      TUPASS(msg_test_desc  );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail_other  );
   }




   return testFramework.countFails();
}

//------------------------------------------------------------
//
// This test checks to make sure that the internal members of
// the RinexMetHeader are as we think they should be.
// Also at the end of this test, we check and make sure our
// output file is equal to our input
//
//------------------------------------------------------------
int RinexMet_T :: hardCodeTest()
{
   TUDEF( "RinexMetHeader", "version" );
   std::string msg_test_desc = "RinexMetHeader data member value tests, ";
   std::string msg_test_fail = "";

   gpstk::RinexMetStream testRinexMetStream( inputNormal.c_str() );
   gpstk::RinexMetStream out( outputHardCode.c_str(), std::ios::out );
   gpstk::RinexMetHeader testRinexMetHeader;

   testRinexMetStream >> testRinexMetHeader;

      //============================================================
      // Test RinexMet Header content
      //============================================================

   msg_test_fail = "RinexMetHeader.version, does not match expected value ";
   testFramework.assert( testRinexMetHeader.version     == 2.1,
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetHeader.fileType, does not match expected value ";
   testFramework.assert( testRinexMetHeader.fileType    == (std::string)"Meteorological",
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetHeader.fileProgram, does not match expected value ";
   testFramework.assert( testRinexMetHeader.fileProgram == (std::string)"GFW - RMW",
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetHeader.fileAgency, does not match expected value ";
   testFramework.assert( testRinexMetHeader.fileAgency  == (std::string)"NIMA",
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetHeader.date, does not match expected value ";
   testFramework.assert( testRinexMetHeader.date        == (std::string)
                         "04/18/2004 23:58:50", msg_test_desc + msg_test_fail, __LINE__ );


   std::vector<std::string>::const_iterator itr1 =
      testRinexMetHeader.commentList.begin();
   msg_test_fail = "weather data correct values message does not match";
   while( itr1 != testRinexMetHeader.commentList.end() )
   {
      testFramework.assert( (*itr1) == (std::string)
                            "Some weather data may have corrected values", msg_test_desc + msg_test_fail,
                            __LINE__ );

      itr1++;
   }

   msg_test_fail = "RinexMetHeader.markerName, does not match expected value ";
   testFramework.assert( testRinexMetHeader.markerName   == (std::string)"85408",
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetHeader.markerNumber, does not match expected value ";
   testFramework.assert( testRinexMetHeader.markerNumber == (std::string)"85408",
                         msg_test_desc + msg_test_fail, __LINE__ );


   msg_test_fail =
      "testRinexMetHeader.convertObsType(iterator) did not return expected value PR";
   vector<RinexMetHeader::RinexMetType>::const_iterator itr2 =
      testRinexMetHeader.obsTypeList.begin();
   if( itr2 != testRinexMetHeader.obsTypeList.end() )
   {
      testFramework.assert( testRinexMetHeader.convertObsType(*itr2) == (std::string)"PR",
                            msg_test_desc + msg_test_fail, __LINE__ );
      itr2++;
   }
   else
   {
      TUFAIL(msg_test_desc );
   }


   msg_test_fail =
      "testRinexMetHeader.convertObsType(iterator) did not return expected value TD";
   if( itr2 != testRinexMetHeader.obsTypeList.end() )
   {
      testFramework.assert( testRinexMetHeader.convertObsType(*itr2) == (std::string)"TD",
                            msg_test_desc + msg_test_fail, __LINE__ );
      itr2++;
   }
   else
   {
      TUFAIL(msg_test_desc );
   }


   msg_test_fail =
      "testRinexMetHeader.convertObsType(iterator) did not return expected value HI";
   if( itr2 != testRinexMetHeader.obsTypeList.end() )
   {
      testFramework.assert( testRinexMetHeader.convertObsType(*itr2) == (std::string)"HI",
                            msg_test_desc + msg_test_fail, __LINE__ );
   }
   else
   {
      TUFAIL(msg_test_desc );
   }


      //------------------------------------------------------------
      //------------------------------------------------------------
   vector<RinexMetHeader::sensorType>::const_iterator itr3 =
      testRinexMetHeader.sensorTypeList.begin();
   if( itr3 != testRinexMetHeader.sensorTypeList.end() )
   {
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.model did not return expected value Vaisala";
      testFramework.assert( (*itr3).model    == (std::string)"Vaisala",
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.type did not return expected value PTB220";
      testFramework.assert( (*itr3).type     == (std::string)"PTB220",
                            msg_test_desc + msg_test_fail, __LINE__  );

      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.accuracy did not return expected value 0.1";
      testFramework.assert( (*itr3).accuracy == 0.1, msg_test_desc + msg_test_fail,
                            __LINE__ );

      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType did not return expected value PR";
      testFramework.assert( testRinexMetHeader.convertObsType((*itr3).obsType) ==
                            (std::string)"PR", msg_test_desc + msg_test_fail, __LINE__ );

      itr3++;
   }
   else
   {
         // Must fail all four tests above
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.model test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.type test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.accuracy test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
   }


      //------------------------------------------------------------
      //------------------------------------------------------------
   if( itr3 != testRinexMetHeader.sensorTypeList.end() )
   {
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.model did not return expected value Vaisala";
      testFramework.assert( (*itr3).model    == (std::string)"Vaisala",
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.type did not return expected value HMP230";
      testFramework.assert( (*itr3).type     == (std::string)"HMP230",
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.accuracy did not return expected value 0.1";
      testFramework.assert( (*itr3).accuracy == 0.1, msg_test_desc + msg_test_fail,
                            __LINE__ );

      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType did not return expected value TD";
      testFramework.assert( testRinexMetHeader.convertObsType((*itr3).obsType) ==
                            (std::string)"TD", msg_test_desc + msg_test_fail, __LINE__ );

      itr3++;
   }
   else
   {
         // Must fail all four tests above
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.model test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.type test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.accuracy test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
   }


      //------------------------------------------------------------
      //------------------------------------------------------------
   if( itr3 != testRinexMetHeader.sensorTypeList.end() )
   {
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.model did not return expected value Vaisala";
      testFramework.assert( (*itr3).model    == (std::string)"Vaisala",
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.type did not return expected value HMP230";
      testFramework.assert( (*itr3).type     == (std::string)"HMP230",
                            msg_test_desc + msg_test_fail, __LINE__  );

      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.accuracy did not return expected value 0.1";
      testFramework.assert( (*itr3).accuracy == 0.1, msg_test_desc + msg_test_fail,
                            __LINE__ );

      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType did not return expected value HI";
      testFramework.assert( testRinexMetHeader.convertObsType((*itr3).obsType) ==
                            (std::string)"HI", msg_test_desc + msg_test_fail, __LINE__ );

      itr3++;

   }
   else
   {
         // Must fail all four tests above
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.model test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.type test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorTypeList iterator.accuracy test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
   }

      //------------------------------------------------------------
      //------------------------------------------------------------
   vector<RinexMetHeader::sensorPosType>::const_iterator itr4 =
      testRinexMetHeader.sensorPosList.begin();

   if( itr4 != testRinexMetHeader.sensorPosList.end() )
   {
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[0] did not return expected value";
      testFramework.assert( (*itr4).position[0] ==  -740289.8363,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[1] did not return expected value";
      testFramework.assert( (*itr4).position[1] == -5457071.7414,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[2] did not return expected value";
      testFramework.assert( (*itr4).position[2] ==  3207245.6207,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.height did not return expected value";
      testFramework.assert( (*itr4).height      ==        0.0000,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType did not return expected value PR";
      testFramework.assert( testRinexMetHeader.convertObsType((*itr4).obsType) ==
                            (std::string) "PR", msg_test_desc + msg_test_fail, __LINE__ );

      itr4++;

   }
   else
   {
         // Must fail all five tests above
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[0] test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[1] test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[2] test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.height test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
   }

      //------------------------------------------------------------
      //------------------------------------------------------------
   if( itr4 != testRinexMetHeader.sensorPosList.end() )
   {
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[0] did not return expected value";
      testFramework.assert( (*itr4).position[0] ==  -740289.8363,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[1] did not return expected value";
      testFramework.assert( (*itr4).position[1] == -5457071.7414,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[2] did not return expected value";
      testFramework.assert( (*itr4).position[2] ==  3207245.6207,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.height did not return expected value";
      testFramework.assert( (*itr4).height      ==        0.0000,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType did not return expected value TD";
      testFramework.assert( testRinexMetHeader.convertObsType((*itr4).obsType) ==
                            (std::string) "TD", msg_test_desc + msg_test_fail, __LINE__ );

   }
   else
   {
         // Must fail all five tests above
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[0] test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[1] test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.position[2] test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.sensorPosType iterator.height test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
      msg_test_fail =
         "RinexMetHeader.convertObsType iterator.obsType test is in a block that failed";
      TUFAIL(msg_test_desc + msg_test_fail );
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
   testFramework.assert( testRinexMetData.time == (gpstk::CommonTime)TimeGuess,
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetData.data for PR does not match expected value ";
   testFramework.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "PR" )]
                         == 992.6, msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetData.data for TD does not match expected value ";
   testFramework.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "TD" )]
                         ==  23.9, msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetData.data for HI does not match expected value ";
   testFramework.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "HI" )]
                         ==  59.7, msg_test_desc + msg_test_fail, __LINE__ );

   out << testRinexMetData;
   testRinexMetStream >> testRinexMetData;
   gpstk::CivilTime TimeGuess2(2004,4,19,0,15,0);

   msg_test_fail = "RinexMetData.time did not equal TimeGuess2";
   testFramework.assert( testRinexMetData.time == (gpstk::CommonTime)TimeGuess2,
                         msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetData.data for PR does not match expected value";
   testFramework.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "PR" )]
                         == 992.8, msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetData.data for TD does not match expected value";
   testFramework.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "TD" )]
                         ==  23.6, msg_test_desc + msg_test_fail, __LINE__ );

   msg_test_fail = "RinexMetData.data for HI does not match expected value";
   testFramework.assert( testRinexMetData.data[testRinexMetHeader.convertObsType( "HI" )]
                         ==  61.6, msg_test_desc + msg_test_fail, __LINE__ );

   out << testRinexMetData;
   while( testRinexMetStream >> testRinexMetData )
   {
      out << testRinexMetData;
   }

   msg_test_fail =
      "files not equal, inputNormal and outputHardCode";
   testFramework.assert( testFramework.fileEqualTest( inputNormal, outputHardCode,
                                                      2), msg_test_desc + msg_test_fail, __LINE__  );

   gpstk::RinexMetStream MetDumps( outputDumps.c_str(), std::ios::out );
   testRinexMetHeader.dump( MetDumps );
   testRinexMetData.dump( MetDumps );


   return testFramework.countFails();
}

//------------------------------------------------------------
//
// This test covers are of the RinexMetHeader and Data which
// deal with continuation lines for the observations
//
//------------------------------------------------------------

int RinexMet_T :: continuationTest()
{
   TUDEF( "RinexMetHeader", "continuation" );
   std::string msg_test_desc = "continuation file comparison";
   std::string msg_test_fail =
      ", files are not equal, inputContLines outputContLines";
   std::string msg_fail_exception = ", threw unexpected exception";

   gpstk::RinexMetStream RinexMetStream( inputContLines.c_str() );
   gpstk::RinexMetStream out( outputContLines.c_str(), std::ios::out );
   gpstk::RinexMetStream MetDumps( outputDumps.c_str(), std::ios::out );
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

      testFramework.assert( testFramework.fileEqualTest( inputContLines,
                                                         outputContLines, 2), msg_test_desc + msg_test_fail, __LINE__ );
   }
   catch(...)
   {
      TUFAIL(msg_test_desc + msg_fail_exception );
   }
   return testFramework.countFails();
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
int RinexMet_T :: dataExceptionsTest()
{
   TUDEF( "RinexMetHeader", "nodata" );
   std::string msg_test_desc = "dataExceptionsTest, ";
   std::string msg_test_fail = "";

   gpstk::RinexMetStream NoObs( inputNoObs.c_str() );
   gpstk::RinexMetStream InvalidTimeFormat( inputInvTimeFmt.c_str() );
   gpstk::RinexMetStream out( outputExceptions.c_str(), std::ios::out );
   gpstk::RinexMetHeader rmh;
   gpstk::RinexMetData rme;

   NoObs.exceptions( std::fstream::failbit );
   InvalidTimeFormat.exceptions( std::fstream::failbit );
   out.exceptions( std::fstream::failbit );

   try
   {
      NoObs >> rmh;
      try
      {
         out << rmh;
         TUPASS("header");
      }
      catch (...)
      {
         TUFAIL("Failed to write valid header");
      }

      try
      {
         while( NoObs >> rme )
         {
            out << rme;
         }
         TUPASS("data");
      }
      catch (...)
      {
         TUFAIL("Failed to read/write valid data");
      }
      try
      {
         InvalidTimeFormat >> rme;
         TUFAIL("Read improperly formatted data");
      }
      catch (...)
      {
         TUPASS("exception");
      }
      InvalidTimeFormat.clear();
         // try to read a second improperly formatted record.. but is
         // it really doing it, or has the stream backed-up to before
         // the previous bad record and it's just trying to read the
         // same broken one?
      try
      {
         InvalidTimeFormat >> rme;
         TUFAIL("Read improperly formatted data");
      }
      catch (...)
      {
         TUPASS("exception");
      }
      InvalidTimeFormat.clear();
   }
   catch (...)
   {
      TUFAIL(msg_test_desc);
   }
   return testFramework.countFails();
}

//------------------------------------------------------------
//
// Test for several of the members within RinexMetFilterOperators
// including merge, LessThanSimple, EqualsSimple,
// and LessThanFull.
//
//------------------------------------------------------------
int RinexMet_T :: filterOperatorsTest()
{
   TUDEF( "RinexMetStream", "filter" );
   std::string msg_test_desc = "filterOperatorsTest, ";
   std::string msg_test_fail = "";

   try
   {

      gpstk::RinexMetStream FilterStream1( inputFilterTest1.c_str() );
      FilterStream1.open( inputFilterTest1.c_str(), std::ios::in );
      gpstk::RinexMetStream FilterStream2( inputFilterTest2.c_str() );
      gpstk::RinexMetStream FilterStream3( inputFilterTest3.c_str() );
      gpstk::RinexMetStream out( outputFilterTest.c_str(), std::ios::out );

      gpstk::RinexMetHeader FilterHeader1;
      gpstk::RinexMetHeader FilterHeader2;
      gpstk::RinexMetHeader FilterHeader3;

      gpstk::RinexMetData FilterData1;
      gpstk::RinexMetData FilterData2;
      gpstk::RinexMetData FilterData3;
      gpstk::RinexMetData rmdata;

      FilterStream1 >> FilterHeader1;
      FilterStream2 >> FilterHeader2;
      FilterStream3 >> FilterHeader3;

      while( FilterStream1 >> rmdata )
      {
         FilterData1 = rmdata;
      }
      while( FilterStream2 >> rmdata )
      {
         FilterData2 = rmdata;
      }
      while( FilterStream3 >> rmdata )
      {
         FilterData3 = rmdata;
      }

      gpstk::RinexMetHeaderTouchHeaderMerge merged;
      merged( FilterHeader1 );
      merged( FilterHeader2 );
      gpstk::RinexMetDataOperatorLessThanFull( merged.obsSet );
      out << merged.theHeader;

      gpstk::RinexMetDataOperatorEqualsSimple EqualsSimple;

      msg_test_fail =
         "Check to see if two equivalent files have the same times. They DO NOT.";
      testFramework.assert( EqualsSimple(FilterData1, FilterData2) == true,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "Check to see if two files with different times have the same time values. They DO.";
      testFramework.assert( EqualsSimple(FilterData1, FilterData3) == false,
                            msg_test_desc + msg_test_fail, __LINE__ );

      gpstk::RinexMetDataOperatorLessThanSimple LessThanSimple;
      msg_test_fail =
         "Check to see if one file occurred earlier than another using equivalent files. One is found to be earlier than the other.";
      testFramework.assert( LessThanSimple(FilterData1, FilterData2) == false,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "Check to see if one file occurred earlier than another using two files with different times. The earlier file is not found to be earlier.";
      testFramework.assert( LessThanSimple(FilterData1, FilterData3) == true,
                            msg_test_desc + msg_test_fail, __LINE__ );

      gpstk::RinexMetDataOperatorLessThanFull LessThanFull(merged.obsSet);

      msg_test_fail =
         "Perform the full less than comparison on two identical files. FilterData1 has been found to be different than FilterData2.";
      testFramework.assert( LessThanFull(FilterData1, FilterData2) == false,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "Perform the full less than comparison on two identical files. FilterData1 has been found to be different than FilterData2.";
      testFramework.assert( LessThanFull(FilterData2, FilterData1) == false,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "Perform the full less than comparison on two different files. FilterData1, an earlier date, has been found to NOT be less than FilterData3.";
      testFramework.assert( LessThanFull(FilterData1, FilterData3) == true,
                            msg_test_desc + msg_test_fail, __LINE__ );

      msg_test_fail =
         "Perform the full less than comparison on two different files. FilterData3, a later date, has been found to be less than FilterData1.";
      testFramework.assert( LessThanFull(FilterData3, FilterData1) == false,
                            msg_test_desc + msg_test_fail, __LINE__ );

      gpstk::CivilTime Start = gpstk::CommonTime::END_OF_TIME;
      gpstk::CivilTime End = gpstk::CommonTime::BEGINNING_OF_TIME;
      gpstk::CivilTime Start2 = gpstk::CommonTime::BEGINNING_OF_TIME;
      gpstk::CivilTime End2 = gpstk::CommonTime::END_OF_TIME;
      gpstk::RinexMetDataFilterTime FilterTime(Start,End);
      gpstk::RinexMetDataFilterTime FilterTime2(Start2,End2);

      msg_test_fail =
         "FilterTime(FilterData1) == true, should evaluate as true but evaluated as false";
      testFramework.assert( FilterTime(FilterData1) == true, msg_test_desc + msg_test_fail,
                            __LINE__ );

      msg_test_fail =
         "FilterTime2(FilterData1) == false, should evaluate as true but evaluated as false";
      testFramework.assert( FilterTime2(FilterData1) == false,
                            msg_test_desc + msg_test_fail, __LINE__ );
   }
   catch(gpstk::Exception& exc)
   {
      cout << exc << endl;
      msg_test_fail = "Unexpected exception was thrown";
      TUFAIL(msg_test_desc + msg_test_fail );
   }
   catch(...)
   {
      msg_test_fail = "Unexpected exception was thrown";
      TUFAIL(msg_test_desc + msg_test_fail );
   }

   return testFramework.countFails();
}

//============================================================
// Run all the test methods defined above
//============================================================

int main()
{
   int errorTotal = 0;
   RinexMet_T testClass;

   errorTotal += testClass.bitsAsStringTest();
   errorTotal += testClass.bitStringTest();
   errorTotal += testClass.reallyPutRecordTest();
   errorTotal += testClass.reallyGetRecordTest();
   errorTotal += testClass.convertObsTypeSTRTest();
   errorTotal += testClass.convertObsTypeHeaderTest();
   errorTotal += testClass.hardCodeTest();
   errorTotal += testClass.continuationTest();
   errorTotal += testClass.dataExceptionsTest();
   errorTotal += testClass.filterOperatorsTest();
   errorTotal += testClass.openMissingFileTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return( errorTotal );
}
