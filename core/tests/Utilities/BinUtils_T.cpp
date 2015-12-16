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
#include "BinUtils.hpp"
#include "Exception.hpp"
#include <iostream>
#include <cmath>

using namespace std;

class BinUtils_T
{
public:


      //==========================================================
      //        Test Suite: decodeVarTest()
      //==========================================================
      //
      //        Tests if item was removed from string and output bytes
      //        are in host byte order
      //
      //==========================================================
   int decodeVarTest(void)
   {
      TestUtil testFramework("BinUtils", "decodeVar", __FILE__, __LINE__);

         //std::cout.setf(std::ios_base::hex,std::ios_base::basefield);

      std::string stringTest = "Random";
         //char out =
      gpstk::BinUtils::decodeVar<char>(stringTest);

      std::string failMesg;

      failMesg = "The method did not remove the first byte of the string";
      testFramework.assert(stringTest == "andom", failMesg, __LINE__);

      std::string stringTest0 = "Random";
      char out0 = gpstk::BinUtils::decodeVar<char>(stringTest0, 0);

      failMesg = "The method did not parse the proper value";
      testFramework.assert(out0 == 0x52, failMesg, __LINE__);

      std::string stringTest1 = "I am 5000.";
      int out1 = gpstk::BinUtils::decodeVar<int>(stringTest1, 5);

      unsigned iexpected = 0x35303030;  // '5000' as ascii bytes
      testFramework.assert(out1 == iexpected, failMesg, __LINE__); // '5000'

      std::string stringTest2 = "The word 'this' should be read";
      float out2 = gpstk::BinUtils::decodeVar<float>(stringTest2, 10);

      unsigned bytes = 0x74686973;  // 'this' as ascii bytes
      float fexpected = *(float*)&bytes;  // interpret bytes as a float
      testFramework.assert(std::abs(out2 - fexpected) < 1e-12, failMesg, __LINE__);

      return testFramework.countFails();
   }


      //==========================================================
      //        Test Suite: encodeVarTest()
      //==========================================================
      //
      //        Tests if bytes are in network byte order
      //
      //==========================================================
   int encodeVarTest(void)
   {
      TestUtil testFramework("BinUtils", "encodeVar", __FILE__, __LINE__);

      char test1 = 'H';
      std::string stringTest1 = gpstk::BinUtils::encodeVar<char>(test1);

      std::string failMesg;

      failMesg = "The method did not output the correct string";
      testFramework.assert(stringTest1 == "H", failMesg, __LINE__);

      int test2 = 0x41424344; // "ABCD"
      std::string stringTest2 = gpstk::BinUtils::encodeVar<int>(test2);

         // Should be same string coming out, unless it's a little
         // endian system.  If so, byte order reversed.
      testFramework.assert(stringTest2 == "ABCD", failMesg, __LINE__);

      float test3 = 0x4D336C316F; // "M3l10"
      std::string stringTest3 = gpstk::BinUtils::encodeVar<float>(test3);

         // Should be same string coming out, unless it's a little
         // endian system.  If so, byte order reversed.
      testFramework.assert(stringTest3 == "\x52\x9A\x66\xD8", failMesg, __LINE__);

         // odd value that has been causing issues with MDP tests
      static const double test4 = 2.4573306210644260e-12;
         // big-endian, converted using
         // http://babbage.cs.qc.edu/IEEE-754.old/64bit.html
      static const unsigned char test4Arr[] = { 0x3d, 0x85, 0x9d, 0x6a,
                                                0xa8, 0x91, 0xf3, 0x7f };
      static const std::string test4Str((char*)test4Arr, sizeof(test4Arr));
      std::string encBE(gpstk::BinUtils::encodeVar<double>(test4));
      TUASSERTE(std::string, test4Str, encBE);

      return testFramework.countFails();
   }

      //====================================================================
      //        Test Suite: computeCRCTest()
      //====================================================================
      //
      //        Tests if computeCRC is generating valid Cyclic
      //        Redundancy Checks
      //
      // Using http://www.zorc.breitbandkatze.de/crc.html for
      // generating comparison for CRCs, in addition to writing out
      // the calculations by hand.
      //
      //=====================================================================
   int computeCRCTest(void)
   {
      using gpstk::BinUtils::computeCRC;
      using gpstk::BinUtils::CRCParam;
      TestUtil testFramework("BinUtils", "computeCRC", __FILE__, __LINE__);
      unsigned long crc;
         // Use printable characters for the convenience of checking
         // against web calculators that only allow you to enter
         // printable characters.
      unsigned char data1[] = "This is a Test!@#$^...";
         // -1 ignores the NULL at the end
      unsigned long len1 = sizeof(data1)-1;

         // these tests exercise computeCRC with:
         // 1) simple and complex polynomials
         // 2) bit-wide (e.g. 24-bit) and byte-wide (e.g. 16-, 32-bit) orders
         // 3) zero and non-zero initial and final XOR values
         // 4) direct and non-direct computation
         // 5) data bytes reversed and non-reversed
         // 6) reverse and do not reverse CRC before final XOR

         // Test a standard CRC-32
      crc = computeCRC(data1, len1, gpstk::BinUtils::CRC32);
      TUASSERTE(unsigned long, 0xeaa96e4d, crc);

         // Test a standard CRC-16
      crc = computeCRC(data1, len1, gpstk::BinUtils::CRC16);
      TUASSERTE(unsigned long, 0x2c74, crc);

         // Test a standard CRC-CCITT
      crc = computeCRC(data1, len1, gpstk::BinUtils::CRCCCITT);
      TUASSERTE(unsigned long, 0x3bcc, crc);

         // Test a standard CRC-24Q
      crc = computeCRC(data1, len1, gpstk::BinUtils::CRC24Q);
      TUASSERTE(unsigned long, 0x6fa2f6, crc);

         // non-direct test
      CRCParam nonDirect(24, 0x823ba9, 0xffffff, 0xffffff, false, false,false);
      crc = computeCRC(data1, len1, nonDirect);
      TUASSERTE(unsigned long, 0x982748, crc);

         // Parity bit.
      CRCParam parity(1, 1, 0, 0, true, false, false);
      crc = computeCRC(data1, len1, parity);
      TUASSERTE(unsigned long, 1, crc);

         //test crc computation on 1 ASCII char
      unsigned char data2[] = { 0x72 };
      unsigned long len2 = 1;
      crc = computeCRC(data2, len2, gpstk::BinUtils::CRC32);
      TUASSERTE(unsigned long, 0x6c09ff9d, crc);
      crc = computeCRC(data2, len2, gpstk::BinUtils::CRC16);
      TUASSERTE(unsigned long, 0x2580, crc);
      crc = computeCRC(data2, len2, gpstk::BinUtils::CRCCCITT);
      TUASSERTE(unsigned long, 0xbf25, crc);

      return testFramework.countFails();
   }

      //==========================================================
      //        Test Suite: xorChecksumTest()
      //==========================================================
      //
      //        Computes xorChecksums with 1, 2 and 3 words for chars,
      //        shorts, and ints
      //
      //==========================================================
   int xorChecksumTest(void)
   {
      TestUtil testFramework("BinUtils", "xorChecksum", __FILE__, __LINE__);

//=============================================================================
//
// Below try/catch block doesn't catch gpstk::Exception.
// gpstk::InvalidParameter seems to terminate program regardless if I
// catch it or not
/*
  try {gpstk::BinUtils::xorChecksum<int>("Hello");testFramework.failTest("The word 'Hello' should of an improper length for an int xor checksum");}
  catch(gpstk::Exception e) {testFramework.passTest();}
  catch(...) {testFramework.failTest();}
*/
//
//
// ALSO, all tests below depend on constant lengths of the data types
// to parse the string correctly. Is there another way around this?
//
//=============================================================================

      std::string failMesg;

      failMesg = "The checksum for a single word char was not computed correctly";
      char xc = gpstk::BinUtils::xorChecksum<char>("7");
      testFramework.assert(xc == '7', failMesg, __LINE__);

      char xc1 = gpstk::BinUtils::xorChecksum<char>("Bc");
      failMesg = "The checksum for a double word char was not computed correctly";
      testFramework.assert(xc1 == '!', failMesg, __LINE__);

      char xc2 = gpstk::BinUtils::xorChecksum<char>("P/Q");
      failMesg = "The checksum for a triple word char was not computed correctly";
      testFramework.assert(xc2 == '.', failMesg, __LINE__);

//=============================================================================
//
//              Reverses the order of bits in xorChecksum. Is this desired?
//
//=============================================================================

      short xc3 = gpstk::BinUtils::xorChecksum<short>("mn");
      failMesg = "The checksum for a single word short was not computed correctly";
      testFramework.assert(xc3 == 0x6E6D, failMesg, __LINE__); //'nm'

      short xc4 = gpstk::BinUtils::xorChecksum<short>("59WZ");
      failMesg = "The checksum for a double word short was not computed correctly";
      testFramework.assert(xc4 == 0x6362, failMesg, __LINE__); // 'cb'

      short xc5 = gpstk::BinUtils::xorChecksum<short>("am+*09");
      failMesg = "The checksum for a triple word short was not computed correctly";
      testFramework.assert(xc5 == 0x7E7A, failMesg, __LINE__); // '~z'

      int xc6 = gpstk::BinUtils::xorChecksum<int>("97Bg");
      failMesg = "The checksum for a single word int was not computed correctly";
      testFramework.assert(xc6 == 0x67423739, failMesg, __LINE__); // 'gB79'

      int xc7 = gpstk::BinUtils::xorChecksum<int>("ABCD!#$%");
      failMesg = "The checksum for a double word int was not computed correctly";
      testFramework.assert(xc7 == 0x61676160, failMesg, __LINE__); // 'aga`'

      int xc8 = gpstk::BinUtils::xorChecksum<int>("+a0.ehZ64xYN");
      failMesg = "The checksum for a triple word int was not computed correctly";
      testFramework.assert(xc8 == 0x5633717A, failMesg, __LINE__); // 'V3qz'

      return testFramework.countFails();

   }

      //==========================================================
      //        Test Suite: countBitsTest()
      //==========================================================
      //
      //        Counts the number of set bits in 32 bit unsigned int
      //
      //==========================================================
   int countBitsTest(void)
   {
      TestUtil testFramework("BinUtils", "countBits", __FILE__, __LINE__);
      std::string failMesg;

      failMesg  = "The number of bits counted is incorrect";
      testFramework.assert(gpstk::BinUtils::countBits(5) == 2, failMesg, __LINE__);

//testing if bit count is constant in a left-shift operation
      testFramework.assert(gpstk::BinUtils::countBits(10) == 2, failMesg, __LINE__);

      failMesg = "The number of bits counted is incorrect after left-shift operation";
      testFramework.assert(gpstk::BinUtils::countBits(20) == 2, failMesg, __LINE__);

//same but for right bit
      failMesg = "The number of bits counted is incorrect";

      testFramework.assert(gpstk::BinUtils::countBits(16) == 1, failMesg, __LINE__);

      failMesg = "The number of bits counted is incorrect after a right-shift operation";
      testFramework.assert(gpstk::BinUtils::countBits(8) == 1, failMesg, __LINE__);

//random case
      failMesg = "The number of bits counted is incorrect";
      testFramework.assert(gpstk::BinUtils::countBits(15) == 4, failMesg, __LINE__);

      return testFramework.countFails();
   }

};


int main (void)
{

      // ntoh, itoh, htoi, hton are not tested here - they are tested
      // through FFBinaryStream_T

   int errorTotal = 0;
   BinUtils_T testClass;

   errorTotal += testClass.decodeVarTest();
   errorTotal += testClass.encodeVarTest();
   errorTotal += testClass.computeCRCTest();
      /** @todo Seriously, if you recognize the fact that simple
       * atomic type definitions are architecture dependent, write the
       * test using atomic types that aren't e.g. uint16_t */
   errorTotal += testClass.xorChecksumTest(); //dependent on size of variables, may fail on other platforms
   errorTotal += testClass.countBitsTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; //Return the total number of errors
}
