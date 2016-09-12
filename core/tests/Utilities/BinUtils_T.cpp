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

// tests for
// 1) integer quantities with no offset
// 2) integer quantities with an offset into the buffer
// 3) decimal quantities with no offset
// 4) decimal quantities with an offset into the buffer
#define TOHOSTTEST(TYPE,STR,EXP,FN)             \
   {                                            \
      testFramework.changeSourceMethod(#FN);    \
      TYPE val;                                 \
      gpstk::BinUtils::FN(STR,val);             \
      TUASSERTE(TYPE,EXP,val);                  \
   }
#define TOHOSTTESTPOS(TYPE,STR,EXP,FN,POS)      \
   {                                            \
      testFramework.changeSourceMethod(#FN);    \
      TYPE val;                                 \
      gpstk::BinUtils::FN(STR,val,POS);         \
      TUASSERTE(TYPE,EXP,val);                  \
   }
#define TOHOSTTESTF(TYPE,STR,EXP,FN)            \
   {                                            \
      testFramework.changeSourceMethod(#FN);    \
      TYPE val;                                 \
      gpstk::BinUtils::FN(STR,val);             \
      TUASSERTFE(EXP,val);                      \
   }
#define TOHOSTTESTFPOS(TYPE,STR,EXP,FN,POS)     \
   {                                            \
      testFramework.changeSourceMethod(#FN);    \
      TYPE val;                                 \
      gpstk::BinUtils::FN(STR,val,POS);         \
      TUASSERTFE(EXP,val);                      \
   }

#define HOSTTOTEST(TYPE,STR,VAL,FN)                     \
   testFramework.changeSourceMethod(#FN);               \
   gpstk::BinUtils::FN(buffer,VAL);                     \
   TUASSERTE(int,0,memcmp(buffer,STR,sizeof(TYPE)));
#define HOSTTOTESTPOS(TYPE,STR,VAL,FN,POS)                      \
   testFramework.changeSourceMethod(#FN);                       \
   gpstk::BinUtils::FN(buffer,VAL,POS);                         \
   TUASSERTE(int,0,memcmp(&buffer[POS],STR,sizeof(TYPE)));

class BinUtils_T
{
public:

   int intelToHostTest()
   {
      TUDEF("BinUtils", "buitoh*");

      TOHOSTTEST(uint16_t,"\x34\x96",0x9634,buitohs);
      TOHOSTTEST(uint32_t,"\xde\xad\xbe\xef",0xefbeadde,buitohl);
      TOHOSTTEST(uint64_t,"\x01\x02\x03\x04\x05\x06\x07\x08",0x0807060504030201,buitohll);
         // With signed quantities, we try to exacerbate possible sign
         // extension problems.
      TOHOSTTEST(int16_t,"\xf7\x03",0x03f7,buitohss);
      TOHOSTTEST(int32_t,"\xff\x0c\x0b\x0a",0x0a0b0cff,buitohsl);
      TOHOSTTEST(int64_t,"\xff\xff\x56\x34\x12\x0f\x0e\x0d",0x0d0e0f123456ffff,buitohsll);
         // using http://babbage.cs.qc.edu/courses/cs341/IEEE-754.html
         // fiddled around until I found a number that seemed to be
         // able to reasonably survive a round-trip, i.e. looked for a
         // number that was representable consistently.
      TOHOSTTESTF(float,"\xe2\x3b\x5d\x40",(float)3.45678,buitohf);
      TOHOSTTESTF(double,"\x2c\xbc\xcb\x45\x7c\xa7\x0b\x40",(double)3.45678,buitohd);
         // Make sure we aren't doing things wrong where byte-swapped
         // data could result in an inappropriate NaN interpretation.
      uint32_t fbits = 0x0100807f;
      float *fptr = (float*)&fbits;
      TOHOSTTESTF(float,"\x7f\x80\x00\x01",*fptr,buitohf);
         // if we're doing things wrong, the f37f will get changed to fb7f
      uint64_t dbits = 0x3d859d6aa891f37f;
      double *dptr = (double*)&dbits;
      TOHOSTTESTF(double,"\x7f\xf3\x91\xa8\x6a\x9d\x85\x3d",*dptr,buitohd);

         // same as above with buffer offsets

      TOHOSTTESTPOS(uint16_t,"#%!\x34\x96",0x9634,buitohs,3);
      TOHOSTTESTPOS(uint32_t,"#%!\xde\xad\xbe\xef",0xefbeadde,buitohl,3);
      TOHOSTTESTPOS(uint64_t,"#%!\x01\x02\x03\x04\x05\x06\x07\x08",0x0807060504030201,buitohll,3);
      TOHOSTTESTPOS(int16_t,"#%!\xf7\x03",0x03f7,buitohss,3);
      TOHOSTTESTPOS(int32_t,"#%!\xff\x0c\x0b\x0a",0x0a0b0cff,buitohsl,3);
      TOHOSTTESTPOS(int64_t,"#%!\xff\xff\x56\x34\x12\x0f\x0e\x0d",0x0d0e0f123456ffff,buitohsll,3);
      TOHOSTTESTFPOS(float,"#%!\xe2\x3b\x5d\x40",(float)3.45678,buitohf,3);
      TOHOSTTESTFPOS(double,"#%!\x2c\xbc\xcb\x45\x7c\xa7\x0b\x40",(double)3.45678,buitohd,3);
      TOHOSTTESTFPOS(float,"#%!\x7f\x80\x00\x01",*fptr,buitohf,3);
      TOHOSTTESTFPOS(double,"#%!\x7f\xf3\x91\xa8\x6a\x9d\x85\x3d",*dptr,buitohd,3);

      return testFramework.countFails();
   }


   int netToHostTest()
   {
      TUDEF("BinUtils", "buntoh*");

      TOHOSTTEST(uint16_t,"\x34\x96",0x3496,buntohs);
      TOHOSTTEST(uint32_t,"\xde\xad\xbe\xef",0xdeadbeef,buntohl);
      TOHOSTTEST(uint64_t,"\x01\x02\x03\x04\x05\x06\x07\x08",0x0102030405060708,buntohll);
         // With signed quantities, we try to exacerbate possible sign
         // extension problems.
      TOHOSTTEST(int16_t,"\xf7\x03",0xf703,buntohss);
      TOHOSTTEST(int32_t,"\xff\x0c\x0b\x0a",0xff0c0b0a,buntohsl);
      TOHOSTTEST(int64_t,"\xff\xff\x56\x34\x12\x0f\x0e\x0d",0xffff5634120f0e0d,buntohsll);
         // using http://babbage.cs.qc.edu/courses/cs341/IEEE-754.html
         // fiddled around until I found a number that seemed to be
         // able to reasonably survive a round-trip, i.e. looked for a
         // number that was representable consistently.
      TOHOSTTESTF(float,"\x40\x5d\x3b\xe2",(float)3.45678,buntohf);
      TOHOSTTESTF(double,"\x40\x0b\xa7\x7c\x45\xcb\xbc\x2c",(double)3.45678,buntohd);
         // Make sure we aren't doing things wrong where byte-swapped
         // data could result in an inappropriate NaN interpretation.
      uint32_t fbits = 0x0100807f;
      float *fptr = (float*)&fbits;
      TOHOSTTESTF(float,"\x01\x00\x80\x7f",*fptr,buntohf);
         // if we're doing things wrong, the f37f will get changed to fb7f
      uint64_t dbits = 0x3d859d6aa891f37f;
      double *dptr = (double*)&dbits;
      TOHOSTTESTF(double,"\x3d\x85\x9d\x6a\xa8\x91\xf3\x7f",*dptr,buntohd);

         // same as above with offsets

      TOHOSTTESTPOS(uint16_t,"abc\x34\x96",0x3496,buntohs,3);
      TOHOSTTESTPOS(uint32_t,"abc\xde\xad\xbe\xef",0xdeadbeef,buntohl,3);
      TOHOSTTESTPOS(uint64_t,"abc\x01\x02\x03\x04\x05\x06\x07\x08",0x0102030405060708,buntohll,3);
      TOHOSTTESTPOS(int16_t,"abc\xf7\x03",0xf703,buntohss,3);
      TOHOSTTESTPOS(int32_t,"abc\xff\x0c\x0b\x0a",0xff0c0b0a,buntohsl,3);
      TOHOSTTESTPOS(int64_t,"abc\xff\xff\x56\x34\x12\x0f\x0e\x0d",0xffff5634120f0e0d,buntohsll,3);
      TOHOSTTESTFPOS(float,"abc\x40\x5d\x3b\xe2",(float)3.45678,buntohf,3);
      TOHOSTTESTFPOS(double,"abc\x40\x0b\xa7\x7c\x45\xcb\xbc\x2c",(double)3.45678,buntohd,3);
      TOHOSTTESTFPOS(float,"abc\x01\x00\x80\x7f",*fptr,buntohf,3);
      TOHOSTTESTFPOS(double,"abc\x3d\x85\x9d\x6a\xa8\x91\xf3\x7f",*dptr,buntohd,3);

      return testFramework.countFails();
   }


   int hostToIntelTest()
   {
      TUDEF("BinUtils", "buhtoi*");

      char buffer[20];
      HOSTTOTEST(uint16_t,"\x34\x96",0x9634,buhtois);
      HOSTTOTEST(uint32_t,"\xde\xad\xbe\xef",0xefbeadde,buhtoil);
      HOSTTOTEST(uint64_t,"\x01\x02\x03\x04\x05\x06\x07\x08",0x0807060504030201,buhtoill);
      HOSTTOTEST(int16_t,"\xf7\x03",0x03f7,buhtoiss);
      HOSTTOTEST(int32_t,"\xff\x0c\x0b\x0a",0x0a0b0cff,buhtoisl);
      HOSTTOTEST(int64_t,"\xff\xff\x56\x34\x12\x0f\x0e\x0d",0x0d0e0f123456ffff,buhtoisll);
      HOSTTOTEST(float,"\xe2\x3b\x5d\x40",3.45678,buhtoif);
      HOSTTOTEST(double,"\x2c\xbc\xcb\x45\x7c\xa7\x0b\x40",3.45678,buhtoid);
      uint32_t fbits = 0x0100807f;
      float *fptr = (float*)&fbits;
      HOSTTOTEST(float,"\x7f\x80\x00\x01",*fptr,buhtoif);
      uint64_t dbits = 0x3d859d6aa891f37f;
      double *dptr = (double*)&dbits;
      HOSTTOTEST(double,"\x7f\xf3\x91\xa8\x6a\x9d\x85\x3d",*dptr,buhtoid);

         // same as above with offsets

      HOSTTOTESTPOS(uint16_t,"\x34\x96",0x9634,buhtois,3);
      HOSTTOTESTPOS(uint32_t,"\xde\xad\xbe\xef",0xefbeadde,buhtoil,3);
      HOSTTOTESTPOS(uint64_t,"\x01\x02\x03\x04\x05\x06\x07\x08",0x0807060504030201,buhtoill,3);
      HOSTTOTESTPOS(int16_t,"\xf7\x03",0x03f7,buhtoiss,3);
      HOSTTOTESTPOS(int32_t,"\xff\x0c\x0b\x0a",0x0a0b0cff,buhtoisl,3);
      HOSTTOTESTPOS(int64_t,"\xff\xff\x56\x34\x12\x0f\x0e\x0d",0x0d0e0f123456ffff,buhtoisll,3);
      HOSTTOTESTPOS(float,"\xe2\x3b\x5d\x40",3.45678,buhtoif,3);
      HOSTTOTESTPOS(double,"\x2c\xbc\xcb\x45\x7c\xa7\x0b\x40",3.45678,buhtoid,3);
      HOSTTOTESTPOS(float,"\x7f\x80\x00\x01",*fptr,buhtoif,3);
      HOSTTOTESTPOS(double,"\x7f\xf3\x91\xa8\x6a\x9d\x85\x3d",*dptr,buhtoid,3);

      return testFramework.countFails();
   }


   int hostToNetTest()
   {
      TUDEF("BinUtils", "buhton*");

      char buffer[20];
      HOSTTOTEST(uint16_t,"\x96\x34",0x9634,buhtons);
      HOSTTOTEST(uint32_t,"\xef\xbe\xad\xde",0xefbeadde,buhtonl);
      HOSTTOTEST(uint64_t,"\x08\x07\x06\x05\x04\x03\x02\x01",0x0807060504030201,buhtonll);
      HOSTTOTEST(int16_t,"\x03\xf7",0x03f7,buhtonss);
      HOSTTOTEST(int32_t,"\x0a\x0b\x0c\xff",0x0a0b0cff,buhtonsl);
      HOSTTOTEST(int64_t,"\x0d\x0e\x0f\x12\x34\x56\xff\xff",0x0d0e0f123456ffff,buhtonsll);
      HOSTTOTEST(float,"\x40\x5d\x3b\xe2",3.45678,buhtonf);
      HOSTTOTEST(double,"\x40\x0b\xa7\x7c\x45\xcb\xbc\x2c",3.45678,buhtond);
      uint32_t fbits = 0x0100807f;
      float *fptr = (float*)&fbits;
      HOSTTOTEST(float,"\x01\x00\x80\x7f",*fptr,buhtonf);
      uint64_t dbits = 0x3d859d6aa891f37f;
      double *dptr = (double*)&dbits;
      HOSTTOTEST(double,"\x3d\x85\x9d\x6a\xa8\x91\xf3\x7f",*dptr,buhtond);

         // same as above with offsets

      HOSTTOTESTPOS(uint16_t,"\x96\x34",0x9634,buhtons,3);
      HOSTTOTESTPOS(uint32_t,"\xef\xbe\xad\xde",0xefbeadde,buhtonl,3);
      HOSTTOTESTPOS(uint64_t,"\x08\x07\x06\x05\x04\x03\x02\x01",0x0807060504030201,buhtonll,3);
      HOSTTOTESTPOS(int16_t,"\x03\xf7",0x03f7,buhtonss,3);
      HOSTTOTESTPOS(int32_t,"\x0a\x0b\x0c\xff",0x0a0b0cff,buhtonsl,3);
      HOSTTOTESTPOS(int64_t,"\x0d\x0e\x0f\x12\x34\x56\xff\xff",0x0d0e0f123456ffff,buhtonsll,3);
      HOSTTOTESTPOS(float,"\x40\x5d\x3b\xe2",3.45678,buhtonf,3);
      HOSTTOTESTPOS(double,"\x40\x0b\xa7\x7c\x45\xcb\xbc\x2c",3.45678,buhtond,3);
      HOSTTOTESTPOS(float,"\x01\x00\x80\x7f",*fptr,buhtonf,3);
      HOSTTOTESTPOS(double,"\x3d\x85\x9d\x6a\xa8\x91\xf3\x7f",*dptr,buhtond,3);

      return testFramework.countFails();
   }

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
      TUDEF("BinUtils", "decodeVar");

         //std::cout.setf(std::ios_base::hex,std::ios_base::basefield);

      std::string stringTest = "Random";
         // should remove first character of stringTest
      gpstk::BinUtils::decodeVar<char>(stringTest);
      TUASSERTE(std::string,"andom",stringTest);

      std::string stringTest0 = "Random";
      char out0 = gpstk::BinUtils::decodeVar<char>(stringTest0, 0);
      TUASSERTE(char,'R',out0);

         // test possibility of corruption with numbers that would be
         // NaN if byte swapped.
      uint64_t dbits = 0x3d859d6aa891f37f;
      double *dptr = (double*)&dbits;
      static const unsigned char test4Arr[] = { 0x3d, 0x85, 0x9d, 0x6a,
                                                0xa8, 0x91, 0xf3, 0x7f };
      std::string encBE((char*)test4Arr, sizeof(test4Arr));
      TUASSERTFE(*dptr,gpstk::BinUtils::decodeVar<double>(encBE));
      TUASSERTE(size_t,0,encBE.size());

         // These tests are strange to look at.  Do not make the
         // mistake of thinking that the expected value being decoded
         // is 5000; it is not.  The expected value being decoded is
         // 0x35303030 or 892350512.
         //
         // Ordinarily the decodeVar/encodeVar methods would not be
         // used in this fashion.  They would instead be used to
         // encode/decode binary values, so the string wouldn't be
         // "5000", it would be 0x1388 which is <CR>X in text.

      std::string stringTest1 = "I am 5000.";
      uint32_t out1 = gpstk::BinUtils::decodeVar<uint32_t>(stringTest1, 5);
      TUASSERTE(uint32_t,0x35303030,out1);

      std::string stringTest2 = "The word 'this' should be read";
      float out2 = gpstk::BinUtils::decodeVar<float>(stringTest2, 10);
      uint32_t bytes = 0x74686973;  // 'this' as ascii bytes
      float fexpected = *(float*)&bytes;  // interpret bytes as a float
      TUASSERTFE(fexpected,out2);

      return testFramework.countFails();
   }


   int decodeVarLETest(void)
   {
      TUDEF("BinUtils", "decodeVarLE");

         //std::cout.setf(std::ios_base::hex,std::ios_base::basefield);

      std::string stringTest = "Random";
         // should remove first character of stringTest
      gpstk::BinUtils::decodeVarLE<char>(stringTest);
      TUASSERTE(std::string,"andom",stringTest);

      std::string stringTest0 = "Random";
      char out0 = gpstk::BinUtils::decodeVarLE<char>(stringTest0, 0);
      TUASSERTE(char,'R',out0);

         // test possibility of corruption with numbers that would be
         // NaN if byte swapped.
      uint64_t dbits = 0x3d859d6aa891f37f;
      double *dptr = (double*)&dbits;
      static const unsigned char test4Arr[] = { 0x7f, 0xf3, 0x91, 0xa8,
                                                0x6a, 0x9d, 0x85, 0x3d  };
      std::string encLE((char*)test4Arr, sizeof(test4Arr));
      TUASSERTFE(*dptr,gpstk::BinUtils::decodeVarLE<double>(encLE));
      TUASSERTE(size_t,0,encLE.size());

         // These tests are strange to look at.  Do not make the
         // mistake of thinking that the expected value being decoded
         // is 5000; it is not.  The expected value being decoded is
         // 0x35303030 or 892350512.
         //
         // Ordinarily the decodeVarLE/encodeVarLE methods would not be
         // used in this fashion.  They would instead be used to
         // encode/decode binary values, so the string wouldn't be
         // "5000", it would be 0x1388 which is <CR>X in text.

      std::string stringTest1 = "I am 5012.";
      uint32_t out1 = gpstk::BinUtils::decodeVarLE<uint32_t>(stringTest1, 5);
      TUASSERTE(uint32_t,0x32313035,out1);

      std::string stringTest2 = "The word 'this' should be read";
      float out2 = gpstk::BinUtils::decodeVarLE<float>(stringTest2, 10);
      uint32_t bytes = 0x73696874;  // 'this' as ascii bytes
      float fexpected = *(float*)&bytes;  // interpret bytes as a float
      TUASSERTFE(fexpected,out2);

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
      TUDEF("BinUtils", "encodeVar");

      char test1 = 'H';
      std::string stringTest1 = gpstk::BinUtils::encodeVar<char>(test1);
      TUASSERTE(std::string,"H",stringTest1);

      uint32_t test2 = 0x41424344; // "ABCD"
      std::string stringTest2 = gpstk::BinUtils::encodeVar<uint32_t>(test2);
      TUASSERTE(std::string,"ABCD",stringTest2);

         // This is weird.  Why do it this way?  Converting from hex
         // to 331575210351 decimal and storing it in a float...
      float test3 = 0x4D336C316F; // "M3l10"
      std::string stringTest3 = gpstk::BinUtils::encodeVar<float>(test3);
      TUASSERTE(std::string,"\x52\x9A\x66\xD8",stringTest3);

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


   int encodeVarLETest(void)
   {
      TUDEF("BinUtils", "encodeVarLE");

      char test1 = 'H';
      std::string stringTest1 = gpstk::BinUtils::encodeVarLE<char>(test1);
      TUASSERTE(std::string,"H",stringTest1);

      uint32_t test2 = 0x41424344; // "ABCD"
      std::string stringTest2 = gpstk::BinUtils::encodeVarLE<uint32_t>(test2);
      TUASSERTE(std::string,"DCBA",stringTest2);

         // This is weird.  Why do it this way?  Converting from hex
         // to 331575210351 decimal and storing it in a float...
      float test3 = 0x4D336C316F; // "M3l10"
      std::string stringTest3 = gpstk::BinUtils::encodeVarLE<float>(test3);
      TUASSERTE(std::string,"\xD8\x66\x9A\x52",stringTest3);

         // odd value that has been causing issues with MDP tests
      static const double test4 = 2.4573306210644260e-12;
         // big-endian, converted using
         // http://babbage.cs.qc.edu/IEEE-754.old/64bit.html
      static const unsigned char test4Arr[] = { 0x7f, 0xf3, 0x91, 0xa8,
                                                0x6a, 0x9d, 0x85, 0x3d  };
      static const std::string test4Str((char*)test4Arr, sizeof(test4Arr));
      std::string encLE(gpstk::BinUtils::encodeVarLE<double>(test4));
      TUASSERTE(std::string, test4Str, encLE);

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
      TUDEF("BinUtils", "computeCRC");
      uint32_t crc;
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
      TUDEF("BinUtils", "xorChecksum");

      try
      {
         gpstk::BinUtils::xorChecksum("Hello", 2);
         TUFAIL("xorChecksum should have failed on uneven input string");
      }
      catch(gpstk::InvalidParameter e)
      {
         TUPASS("xorChecksum");
      }
      catch(...)
      {
         TUFAIL("xorChecksum threw the wrong exception type");
      }

      std::string cksum;

      cksum = gpstk::BinUtils::xorChecksum("7", 1);
      TUASSERTE(std::string,std::string("7"),cksum);

      cksum = gpstk::BinUtils::xorChecksum("Bc", 1);
      TUASSERTE(std::string,std::string("!"),cksum);

      cksum = gpstk::BinUtils::xorChecksum("P/Q", 1);
      TUASSERTE(std::string,std::string("."),cksum);

      cksum = gpstk::BinUtils::xorChecksum("mn", 2);
      TUASSERTE(std::string,std::string("mn"),cksum);

      cksum = gpstk::BinUtils::xorChecksum("59WZ", 2);
      TUASSERTE(std::string,std::string("bc"),cksum);

      cksum = gpstk::BinUtils::xorChecksum("am+*09", 2);
      TUASSERTE(std::string,std::string("z~"),cksum);

      cksum = gpstk::BinUtils::xorChecksum("97Bg", 4);
      TUASSERTE(std::string,std::string("97Bg"),cksum);

      cksum = gpstk::BinUtils::xorChecksum("ABCD!#$%", 4);
      TUASSERTE(std::string,std::string("`aga"),cksum);

      cksum = gpstk::BinUtils::xorChecksum("+a0.ehZ64xYN", 4);
      TUASSERTE(std::string,std::string("zq3V"),cksum);

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
      TUDEF("BinUtils", "countBits");
      std::string failMesg;

      TUASSERTE(unsigned short,2,gpstk::BinUtils::countBits(5));

         // testing if bit count is constant in a left-shift operation
      TUASSERTE(unsigned short,2,gpstk::BinUtils::countBits(10));
      TUASSERTE(unsigned short,2,gpstk::BinUtils::countBits(20));

         // same but for right bit
      TUASSERTE(unsigned short,1,gpstk::BinUtils::countBits(16));
      TUASSERTE(unsigned short,1,gpstk::BinUtils::countBits(8));

         // random case
      TUASSERTE(unsigned short,4,gpstk::BinUtils::countBits(15));

      return testFramework.countFails();
   }

};


int main (void)
{
   int errorTotal = 0;
   BinUtils_T testClass;

   errorTotal += testClass.intelToHostTest();
   errorTotal += testClass.netToHostTest();
   errorTotal += testClass.hostToIntelTest();
   errorTotal += testClass.hostToNetTest();
   errorTotal += testClass.decodeVarTest();
   errorTotal += testClass.decodeVarLETest();
   errorTotal += testClass.encodeVarTest();
   errorTotal += testClass.encodeVarLETest();
   errorTotal += testClass.computeCRCTest();
   errorTotal += testClass.xorChecksumTest();
   errorTotal += testClass.countBitsTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; //Return the total number of errors
}
