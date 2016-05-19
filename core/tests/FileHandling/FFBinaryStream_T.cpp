//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as
//  published by the Free Software Foundation; either version 3.0 of
//  the License, or any later version.
//
//  The GPSTk is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//  02110, USA
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

#include "FFBinaryStream.hpp"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

// generic little-endian binary stream
class FFBinaryStreamLE : public FFBinaryStream
{
public:
   FFBinaryStreamLE(const char* fn,
                    std::ios::openmode mode=std::ios::in|std::ios::binary)
         : gpstk::FFBinaryStream(fn, mode)
   {}

   virtual bool isStreamLittleEndian() const throw()
   { return true; }
};

// generic big-endian binary stream
class FFBinaryStreamBE : public FFBinaryStream
{
public:
   FFBinaryStreamBE(const char* fn,
                    std::ios::openmode mode=std::ios::in|std::ios::binary)
         : gpstk::FFBinaryStream(fn, mode)
   {}

   virtual bool isStreamLittleEndian() const throw()
   { return false; }
};

class FFBinaryStream_T
{
public:

      // constructor
   FFBinaryStream_T()
   {
      init();
   }

      // initialize tests
   void init();

   int testTypes();

   template <class StreamType>
   void testStream(const std::string& outfn, const std::string& reffn,
                   TestUtil& testFramework);

   string beFile; ///< Big-endian output file
   string leFile; ///< Little-endian output file
   string beRef;  ///< Big-endian output reference file
   string leRef;  ///< Little-endian output reference file
}; // class FFBinaryStream_T


//============================================================
// Initialize Test Data Filenames and Values
//============================================================

void FFBinaryStream_T ::
init()
{

   TestUtil testUtil;
   string dp = gpstk::getPathData() + gpstk::getFileSep();
   string op = gpstk::getPathTestTemp() + gpstk::getFileSep();

      //----------------------------------------
      // Full file paths
      //----------------------------------------
   beFile = op + "test_output_FFBinaryStream_BE.dat";
   leFile = op + "test_output_FFBinaryStream_LE.dat";
   beRef  = dp + "test_output_FFBinaryStream_BE.exp";
   leRef  = dp + "test_output_FFBinaryStream_LE.exp";
}


int FFBinaryStream_T ::
testTypes()
{
   TUDEF("FFBinaryStream", "writeData");
   testFramework.changeSourceMethod("getData/writeData (little-endian)");
   testStream<FFBinaryStreamLE>(leFile, leRef, testFramework);
   testFramework.changeSourceMethod("getData/writeData (big-endian)");
   testStream<FFBinaryStreamBE>(beFile, beRef, testFramework);
   return testFramework.countFails();
}


template <class StreamType>
void FFBinaryStream_T ::
testStream(const std::string& outfn, const std::string& reffn,
           TestUtil& testFramework)
{
   StreamType testStrm(outfn.c_str(), ios::out);
   static const uint8_t uint8Arr[] = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W',
                                       'o', 'r', 'l', 'd', '!' };
      // test edges as well as "random" non-edge numbers
   static const uint16_t uint16Arr[] = { 0xffff, 0x0000, 0xdead, 0xbeef,
                                         0x7ff3 };
   static const uint32_t uint32Arr[] = { 0, 2147483647, 4294967295, 6456568 };
   static const uint64_t uint64Arr[] = { 0, 9223372036854775807,
                                         18446744073709551615lu, 2904954569522 };

   static const int8_t int8Arr[] = { 0, 127, -127, -1 };
   static const int16_t int16Arr[] = { 0, -1, -32767, 32767, 98, -1553 };
   static const int32_t int32Arr[] = { 0, -1, -2147483647, 2147483647, -123456,
                                       63454567 };
   static const int64_t int64Arr[] = { 0, -1, -9223372036854775807,
                                       9223372036854775807, 104993356393,
                                       -544342456764 };

      // http://www.h-schmidt.net/FloatConverter/IEEE754.html
      // 3a00f07f
   static const float fltArr[] = { 4.918649210594594E-4 };
      // http://babbage.cs.qc.edu/courses/cs341/IEEE-754.html
      // 01 23 45 67 89 ab cd ef
      // 3d 85 9d 6a a8 91 f3 7f
   static const double dblArr[] = { 3.5127005640885040e-303,
                                    2.4573306210644260e-12 };

   static const streampos fileSize =
      sizeof(uint8Arr) +
      sizeof(uint16Arr) +
      sizeof(uint32Arr) +
      sizeof(uint64Arr) +
      sizeof(int8Arr) +
      sizeof(int16Arr) +
      sizeof(int32Arr) +
      sizeof(int64Arr) +
      sizeof(fltArr) +
      sizeof(dblArr);

      // Note that this puts 17 bytes into the file which means that
      // the longer quantities (2-, 4-, and 8-bytes) will not be
      // aligned.  This is in itself a useful test.
   unsigned i;
   uint8_t tu8;
   uint16_t tu16;
   uint32_t tu32;
   uint64_t tu64;
   uint8_t ts8;
   int16_t ts16;
   int32_t ts32;
   int64_t ts64;
   float tf;
   double td;

      // writeData makes sure that the output file grows by the
      // expected number of bytes for the data type.

      // unsigned types
   streampos expectedPos = 0;
   for (i = 0; i < sizeof(uint8Arr); i++)
   {
      testStrm.writeData(uint8Arr[i]);
      expectedPos += 1;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }
      // sizeof is in bytes, divide by 2 to get array size, repeat as necessary
   for (i = 0; i < (sizeof(uint16Arr) >> 1); i++)
   {
      testStrm.writeData(uint16Arr[i]);
      expectedPos += 2;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }
   for (i = 0; i < (sizeof(uint32Arr) >> 2); i++)
   {
      testStrm.writeData(uint32Arr[i]);
      expectedPos += 4;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }
   for (i = 0; i < (sizeof(uint64Arr) >> 3); i++)
   {
      testStrm.writeData(uint64Arr[i]);
      expectedPos += 8;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }

      // signed types
   for (i = 0; i < sizeof(int8Arr); i++)
   {
      testStrm.writeData(int8Arr[i]);
      expectedPos += 1;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }
   for (i = 0; i < (sizeof(int16Arr) >> 1); i++)
   {
      testStrm.writeData(int16Arr[i]);
      expectedPos += 2;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }
   for (i = 0; i < (sizeof(int32Arr) >> 2); i++)
   {
      testStrm.writeData(int32Arr[i]);
      expectedPos += 4;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }
   for (i = 0; i < (sizeof(int64Arr) >> 3); i++)
   {
      testStrm.writeData(int64Arr[i]);
      expectedPos += 8;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }
   for (i = 0; i < (sizeof(fltArr) >> 2); i++)
   {
      testStrm.writeData(fltArr[i]);
      expectedPos += 4;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }
   for (i = 0; i < (sizeof(dblArr) >> 3); i++)
   {
      testStrm.writeData(dblArr[i]);
      expectedPos += 8;
      TUASSERTE(streampos, expectedPos, testStrm.tellg());
   }

   testStrm.close();

      // getData tests make sure that the data read back in from the
      // file matches what was written out in the first place.
   
   
   StreamType testStrmIn(outfn.c_str(), ios::in);

   #ifdef WIN32
   testFramework.assert(bool(testStrmIn), "Couldn't open " + outfn + " for input", __LINE__);
   #else
   testFramework.assert(testStrmIn, "Couldn't open " + outfn + " for input", __LINE__);
   #endif

      // check file size
   testStrmIn.seekg(0, testStrmIn.end);
   TUASSERTE(streampos, fileSize, testStrmIn.tellg());
   testStrmIn.seekg(0, testStrmIn.beg);

      // unsigned types
   for (i = 0; i < sizeof(uint8Arr); i++)
   {
      testStrmIn.getData(tu8);
      TUASSERTE(uint8_t, uint8Arr[i], tu8);
   }
      // sizeof is in bytes, divide by 2 to get array size, repeat as necessary
   for (i = 0; i < (sizeof(uint16Arr) >> 1); i++)
   {
      testStrmIn.getData(tu16);
      TUASSERTE(uint16_t, uint16Arr[i], tu16);
   }
   for (i = 0; i < (sizeof(uint32Arr) >> 2); i++)
   {
      testStrmIn.getData(tu32);
      TUASSERTE(uint32_t, uint32Arr[i], tu32);
   }
   for (i = 0; i < (sizeof(uint64Arr) >> 3); i++)
   {
      testStrmIn.getData(tu64);
      TUASSERTE(uint64_t, uint64Arr[i], tu64);
   }

      // signed types
   for (i = 0; i < sizeof(int8Arr); i++)
   {
      testStrmIn.getData(ts8);
      TUASSERTE(int8_t, int8Arr[i], ts8);
   }
   for (i = 0; i < (sizeof(int16Arr) >> 1); i++)
   {
      testStrmIn.getData(ts16);
      TUASSERTE(int16_t, int16Arr[i], ts16);
   }
   for (i = 0; i < (sizeof(int32Arr) >> 2); i++)
   {
      testStrmIn.getData(ts32);
      TUASSERTE(int32_t, int32Arr[i], ts32);
   }
   for (i = 0; i < (sizeof(int64Arr) >> 3); i++)
   {
      testStrmIn.getData(ts64);
      TUASSERTE(int64_t, int64Arr[i], ts64);
   }
   for (i = 0; i < (sizeof(fltArr) >> 2); i++)
   {
      testStrmIn.getData(tf);
      TUASSERTFE(fltArr[i], tf);
   }
   for (i = 0; i < (sizeof(dblArr) >> 3); i++)
   {
      testStrmIn.getData(td);
      TUASSERTFE(dblArr[i], td);
   }

   testStrmIn.close();

   testFramework.assert_binary_files_equal
      (__LINE__, reffn, outfn, "Encoded output file does not match reference");
}


   /** Run the program.
    *
    * @return Total error count for all tests
    */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   FFBinaryStream_T  testClass;  // test data is loaded here

   errorTotal += testClass.testTypes();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return( errorTotal );

} // main()
