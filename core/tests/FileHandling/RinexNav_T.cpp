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
#include "GPSWeekZcount.hpp"
#include "TimeString.hpp"

#include "build_config.h"

#include "TestUtil.hpp"
#include <string>
#include <iostream>

using namespace gpstk;

#ifdef _MSC_VER
#define LDEXP(x,y) ldexp(x,y)
#else
#define LDEXP(x,y) std::ldexp(x,y)
#endif


/** Ephemeris subframe words at the end of a week.  Useful for a
 * week-rollover test of toe and toc as well as other things.
 * Sorry about the decimal, it came that way out of HDF5. 
 * @note this data has been modified so that toe != toc, to facilitate
 * verifying that the appropriate quantity is used where
 * appropriate. */
const uint32_t ephEOW[] = 
{  583228942, 824945128,  904134685,  184026330,  459310087,
    16899638, 845363969, 0x0f647980,    4193148, 1073290676,
   583228942, 824953464,  260012308,  225364840,  787693093,
  1065730353, 298759921,   46377054,   57870868,       8172,
   583228942, 824962032, 1072401983,  485782594,      84477,
   301605863, 145566781,  506082625, 1072230894,  259901040 };
/* original data as broadcast
{  583228942, 824945128,  904134685,  184026330,  459310087,
    16899638, 845363969,  255852580,    4193148, 1073290676,
   583228942, 824953464,  260012308,  225364840,  787693093,
  1065730353, 298759921,   46377054,   57870868,       8172,
   583228942, 824962032, 1072401983,  485782594,      84477,
   301605863, 145566781,  506082625, 1072230894,  259901040 };
*/
const unsigned ephEOWwk  = 1886;
const unsigned ephEOWToeWk = 1887;
const unsigned ephEOWprn = 14;
// the rest of these values were broken out by hand
const CommonTime ephEOWhowTime1 = GPSWeekZcount(ephEOWwk, 402804);
const CommonTime ephEOWhowTime2 = GPSWeekZcount(ephEOWwk, 402808);
const CommonTime ephEOWhowTime3 = GPSWeekZcount(ephEOWwk, 402812);
const long ephEOWhowSec1 = 604206;
const long ephEOWhowSec2 = 604212; 
const long ephEOWhowSec3 = 604218;
const CommonTime ephEOWxmitTime1 = ephEOWhowTime1 - 6;
const CommonTime ephEOWxmitTime2 = ephEOWhowTime2 - 6;
const CommonTime ephEOWxmitTime3 = ephEOWhowTime3 - 6;
const double ephEOWxmitTimeSec1 = GPSWeekSecond(ephEOWxmitTime1).sow;
const double ephEOWtocSec = 597600;
const long   ephEOWtocZ   = 398400;
const CommonTime ephEOWtoc = GPSWeekZcount(ephEOWwk, ephEOWtocZ);
// as-broadcast
//const CommonTime ephEOWtoc = GPSWeekZcount(ephEOWwk+1, 0);
const double ephEOWaf0      = LDEXP(double( int32_t(0xfffff91d)), -31);
const double ephEOWaf1      = LDEXP(double( int16_t(0xffed)),     -43);
const double ephEOWaf2      = 0.;
const double ephEOWiode     = 61.;
const double ephEOWCrs      = LDEXP(double( int16_t(0xfde4)),      -5);
const double ephEOWdn       = LDEXP(double( int16_t(0x35bb)),     -43) * PI;
const double ephEOWM0       = LDEXP(double( int32_t(0x2dbbccf8)), -31) * PI;
const double ephEOWCuc      = LDEXP(double( int16_t(0xfe17)),     -29);
const double ephEOWecc      = LDEXP(double(uint32_t(0x04473adb)), -33);
const double ephEOWCus      = LDEXP(double( int16_t(0x0b0e)),     -29);
const double ephEOWAhalf    = LDEXP(double(uint32_t(0xa10dcc28)), -19);
const double ephEOWToe      = 0.; //LDEXP(double(uint16_t()),4);
const double ephEOWCic      = LDEXP(double( int16_t(0xffae)),     -29);
const double ephEOWOMEGA0   = LDEXP(double( int32_t(0x3873d1d1)), -31) * PI;
const double ephEOWCis      = LDEXP(double( int16_t(0x0005)),     -29);
const double ephEOWi0       = LDEXP(double( int32_t(0x2747e88f)), -31) * PI;
const double ephEOWCrc      = LDEXP(double( int16_t(0x22b4)),      -5);
const double ephEOWw        = LDEXP(double( int32_t(0xb078a8d5)), -31) * PI;
const double ephEOWOMEGAdot = LDEXP(double( int32_t(0xffffa3c7)), -43) * PI;
const double ephEOWidot     = LDEXP(double( int16_t(0xfdc6)),     -43) * PI;
const double ephEOWTgd      = LDEXP(double(  int8_t(0xec)),       -31);
const short  ephEOWcodeflgs = 1;
const short  ephEOWl2pData  = 0;
const short  ephEOWhealth   = 0;
const double ephEOWiodc     = 0x03d;
// URA index = 0, worst case 2.4m 20.3.3.3.1.3
const double ephEOWacc      = 2.4;
// fit interval in *hours*
const double ephEOWfitint   = 4;


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
      /** Test consistency in reading data with transmit times that
       * follow the guidance in the Table A4 footnote regarding
       * transmission times and those that don't. */
   unsigned xmitReadTest();

private:

   std::string dataFilePath;

   std::string inputRinexNavExample;

   std::string outputTestOutput;
   std::string outputTestOutput2;
   std::string outputTestOutput3;
   std::string outputTestOutput4;
   std::string outputRinexDump;

   std::string inputInvalidLineLength;
   std::string inputNotaNavFile;
   std::string inputUnknownHeaderLabel;
   std::string inputIncompleteHeader;
   std::string inputUnsupportedRinex;
   std::string inputBadHeader;
   std::string outputTestOutputHeader;
   std::string inputXmitTime;
   std::string outputXmitTime;

   std::string inputFilterStream1;
   std::string inputFilterStream2;
   std::string inputFilterStream3;
   std::string inputTestOutput4;
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
   outputTestOutput4        = tp+"test_output_rinex_nav_TestOutput4.16n";
   inputTestOutput4         = dp+"test_input_rinex_nav_TestOutput4.16n";
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
   inputXmitTime            = dp+"test_input_rinex_nav_TestInput1.16n";
   outputXmitTime           = tp+"test_output_rinex_nav_TestInput1.16n";
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
      gpstk::RinexNavStream inp(inputRinexNavExample.c_str());
      gpstk::RinexNavStream out(outputTestOutput.c_str(), std::ios::out);
      gpstk::RinexNavStream dmp(outputRinexDump.c_str(), std::ios::out);
      gpstk::RinexNavHeader header;
      gpstk::RinexNavData data;

      inp >> header;
      out << header;

      while(inp >> data)
      {
         out << data;
      }

      TUASSERTFE(2.1, header.version);
      TUASSERTE(std::string, "XXRINEXN V3", header.fileProgram);
      TUASSERTE(std::string, "AIUB", header.fileAgency);
      TUASSERTE(std::string, "09/02/1999 19:22:36", header.date);
      std::vector<std::string>::const_iterator itr1 =
         header.commentList.begin();
      TUASSERTE(std::string, "THIS IS ONE COMMENT", *itr1);
      TUCMPFILE(inputRinexNavExample, outputTestOutput, 2);

         //------------------------------------------------------------
      gpstk::RinexNavStream inp2(outputTestOutput.c_str());
      gpstk::RinexNavStream out2(outputTestOutput2.c_str(), std::ios::out);
      gpstk::RinexNavHeader header2;
      gpstk::RinexNavData data2;

      inp2 >> header2;
      out2 << header2;

      while (inp2 >> data2)
      {
         out2 << data2;
      }

      gpstk::RinexNavStream inp3(outputTestOutput2.c_str());
      gpstk::RinexNavStream out3(outputTestOutput3.c_str() , std::ios::out);
      gpstk::RinexNavHeader header3;
      gpstk::RinexNavData data3;

      inp3 >> header3;
      out3 << header3;

      while (inp3 >> data3)
      {
         out3 << data3;
      }
      header.dump(dmp);
      data.dump(dmp);

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
   TUDEF("RinexNavData", "RinexNavData(EngEphemeris)");

   EngEphemeris eeph;
   TUASSERT(eeph.addSubframe(&ephEOW[ 0], ephEOWwk, ephEOWprn, 1));
   TUASSERT(eeph.addSubframe(&ephEOW[10], ephEOWwk, ephEOWprn, 1));
   TUASSERT(eeph.addSubframe(&ephEOW[20], ephEOWwk, ephEOWprn, 1));

   RinexNavData rnd(eeph);
      // epoch
   TUASSERTE(short, ephEOWprn, rnd.PRNID);
   TUASSERTE(CommonTime, ephEOWtoc, rnd.time);
   TUASSERTFE(ephEOWaf0, rnd.af0);
   TUASSERTFE(ephEOWaf1, rnd.af1);
   TUASSERTFE(ephEOWaf2, rnd.af2);
      // 1
   TUASSERTFE(ephEOWiode, rnd.IODE);
   TUASSERTFE(ephEOWCrs, rnd.Crs);
   TUASSERTFE(ephEOWdn, rnd.dn);
   TUASSERTFE(ephEOWM0, rnd.M0);
      // 2
   TUASSERTFE(ephEOWCuc, rnd.Cuc);
   TUASSERTFE(ephEOWecc, rnd.ecc);
   TUASSERTFE(ephEOWCus, rnd.Cus);
   TUASSERTFE(ephEOWAhalf, rnd.Ahalf);
      // 3
   TUASSERTFE(ephEOWToe, rnd.Toe);
   TUASSERTFE(ephEOWCic, rnd.Cic);
   TUASSERTFE(ephEOWOMEGA0, rnd.OMEGA0);
   TUASSERTFE(ephEOWCis, rnd.Cis);
      // 4
   TUASSERTFE(ephEOWi0, rnd.i0);
   TUASSERTFE(ephEOWCrc, rnd.Crc);
   TUASSERTFE(ephEOWw, rnd.w);
   TUASSERTFE(ephEOWOMEGAdot, rnd.OMEGAdot);
      // 5
   TUASSERTFE(ephEOWidot, rnd.idot);
   TUASSERTE(short, ephEOWcodeflgs, rnd.codeflgs);
   TUASSERTE(short, ephEOWToeWk, rnd.toeWeek);
   TUASSERTE(short, ephEOWl2pData, rnd.L2Pdata);
      // 6
   TUASSERTFE(ephEOWacc, rnd.accuracy);
   TUASSERTE(short, ephEOWhealth, rnd.health);
   TUASSERTFE(ephEOWTgd, rnd.Tgd);
   TUASSERTFE(ephEOWiodc, rnd.IODC);
      // 7
      // not available... yay.
      //TUASSERTFE(ephEOWxmitTimeSec1, 
   TUASSERTFE(ephEOWfitint, rnd.fitint);

   gpstk::RinexNavStream out(outputTestOutput4.c_str(), std::ios::out);
   out << rnd;
   out.close();
   TUCMPFILE(inputTestOutput4, outputTestOutput4, 0);

/*
   using namespace std;
   using gpstk::StringUtils::doub2for;
   ofstream out(outputTestOutput4.c_str(), std::ios::out);
   out << setw(2) << ephEOWprn
       << " " << gpstk::printTime(ephEOWtoc, "%2y %2m %2d %2H %2M%5.1f ")
       << doub2for(ephEOWaf0, 18, 2) << " "
       << doub2for(ephEOWaf1, 18, 2) << " "
       << doub2for(ephEOWaf2, 18, 2) << endl
       << "    " << doub2for(ephEOWiode, 18, 2)
       << " " << doub2for(ephEOWCrs, 18, 2)
       << " " << doub2for(ephEOWdn, 18, 2)
       << " " << doub2for(ephEOWM0, 18, 2) << endl
       << "    " << doub2for(ephEOWCuc, 18, 2)
       << " " << doub2for(ephEOWecc, 18, 2)
       << " " << doub2for(ephEOWCus, 18, 2)
       << " " << doub2for(ephEOWAhalf, 18, 2) << endl
       << "    " << doub2for(ephEOWToe, 18, 2)
       << " " << doub2for(ephEOWCic, 18, 2)
       << " " << doub2for(ephEOWOMEGA0, 18, 2)
       << " " << doub2for(ephEOWCis, 18, 2) << endl
       << "    " << doub2for(ephEOWi0, 18, 2)
       << " " << doub2for(ephEOWCrc, 18, 2)
       << " " << doub2for(ephEOWw, 18, 2)
       << " " << doub2for(ephEOWOMEGAdot, 18, 2) << endl
       << "    " << doub2for(ephEOWidot, 18, 2)
       << " " << doub2for(ephEOWcodeflgs, 18, 2)
       << " " << doub2for(ephEOWToeWk, 18, 2)
       << " " << doub2for(ephEOWl2pData, 18, 2) << endl
       << "    " << doub2for(ephEOWacc, 18, 2)
       << " " << doub2for(ephEOWhealth, 18, 2)
       << " " << doub2for(ephEOWTgd, 18, 2)
       << " " << doub2for(ephEOWiodc, 18, 2) << endl
       << "    " << doub2for(ephEOWxmitTimeSec1, 18, 2)
       << " " << doub2for(ephEOWfitint, 18, 2) << endl;
   out.close();
*/

   TURETURN();
}


unsigned RinexNav_T ::
xmitReadTest()
{
   TUDEF("RinexNavData", "reallyGetRecord");
   RinexNavStream ins(inputXmitTime.c_str(), std::ios::in);
   RinexNavData positive, negative;
   RinexNavHeader header;
   CommonTime expXmit = GPSWeekSecond(1886, 604200, gpstk::TimeSystem::GPS);
   #ifdef WIN32
   TUASSERT(bool(ins));
   #else
   TUASSERT(ins);
   #endif
   ins >> header;
   #ifdef WIN32
   TUASSERT(bool(ins));
   #else
   TUASSERT(ins);
   #endif
      // negative transmit time requires adjustment of the seconds of
      // week to get the transmit time right
   ins >> negative;
      // positive transmit time requires adjustment of the week to get
      // the transmit time right
   #ifdef WIN32
   TUASSERT(bool(ins));
   #else
   TUASSERT(ins);
   #endif
   ins >> positive;
   #ifdef WIN32
   TUASSERT(bool(ins));
   #else
   TUASSERT(ins);
   #endif
   ins.close();
   TUASSERTE(CommonTime, expXmit, negative.getXmitTime());
   TUASSERTE(CommonTime, expXmit, positive.getXmitTime());
      // write the data back out and make sure nothing has changed
   RinexNavStream outs(outputXmitTime.c_str(), std::ios::out);
   #ifdef WIN32
   TUASSERT(bool(outs));
   #else
   TUASSERT(outs);
   #endif
   outs << header << negative << positive;
   #ifdef WIN32
   TUASSERT(bool(outs));
   #else
   TUASSERT(outs);
   #endif
   outs.close();
   TUCMPFILE(inputXmitTime, outputXmitTime, 2);
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
   errorTotal += testClass.castTest();
   errorTotal += testClass.xmitReadTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return(errorTotal);
}
