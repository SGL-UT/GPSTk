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

#include "BinexData.hpp"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

//=============================================================================
// Class declarations
//=============================================================================

class BinexTypes_T
{
public:

      // constructor
   BinexTypes_T() : verboseLevel(0) { init(); };

      // destructor
   virtual ~BinexTypes_T() { };

      // initialize tests
   void init();

      // test methods
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int doUbnxiInitializationTests();
   int doUbnxiEncodeDecodeTests();
   int doMgfziInitializationTests();
   int doMgfziEncodeDecodeTests();

   unsigned  verboseLevel;  // amount to display during tests, 0 = least

private:

   struct TestNum
   {
      long long  value;
      int        size;  // expected size, -1 denotes an invalid number
   };

   typedef vector<TestNum>  NumListType;

      // read a list of numbers (one per line) from the specified file
      // @return true on success, false on failure
   bool readNums(const string& filename, NumListType& numList);

   NumListType  ubnxiNumList;
   NumListType  mgfziNumList;

   string  inputUbnxiNums;
   string  inputMgfziNums;

      /** Update test results and optionally show test details
       */
   void report(TestUtil&               test,
               const unsigned long     expectedValue,
               const size_t            expectedSize,
               const BinexData::UBNXI& actual,
               const bool              isLittleEndian = false);

      /** Update test results and optionally show test details
       */
   void report(TestUtil&               test,
               const long long         expectedValue,
               const size_t            expectedSize,
               const BinexData::MGFZI& actual,
               const bool              isLittleEndian = false);

      /**
       * 
       */
   void dumpBuffer(const unsigned char* buffer, size_t size);

}; // class BinexTypes_T


//============================================================
// Initialize Test Data Filenames and Values
//============================================================

void BinexTypes_T :: init( void )
{

    TestUtil  test0;
    string  dataFilePath = test0.getDataPath();
    string  tempFilePath = test0.getTempPath();

    //---------------------------------------- 
    // Full file paths
    //---------------------------------------- 
    string  file_sep = "/";

    inputUbnxiNums = dataFilePath + file_sep + "test_input_binex_types_Ubnxi.txt";
    inputMgfziNums = dataFilePath + file_sep + "test_input_binex_types_Mgfzi.txt";

    readNums(inputUbnxiNums, ubnxiNumList);
    readNums(inputMgfziNums, mgfziNumList);
}


//---------------------------------------------------------------------------
bool BinexTypes_T :: readNums(const string& filename,
                              NumListType& numList)
{
   ifstream  ifs(filename.c_str());

   if (!ifs.good())
      return false;
   
   while (ifs.good())
   {
      string  line;
      getline(ifs, line);

         // ignore comments
      string::size_type  hashPos = line.find('#');
      if (hashPos != string::npos)
      {
         line.erase(hashPos);         
      }
         // ignore empty lines
      string::size_type  nonWhitePos = line.find_first_not_of(" \t");
      if (nonWhitePos != string::npos)
      {
         istringstream  iss(line);
         TestNum  num;
         iss >> num.value;
         iss >> num.size;
         numList.push_back(num);
      }
   }
   return (numList.size() > 0);
}


//=============================================================================
// Test Method Definitions
//=============================================================================


int BinexTypes_T :: doUbnxiInitializationTests()
{
   TestUtil  testFramework( "BinexData::UBNXI", "Initialization", __FILE__, __LINE__ );

      // BinexData::UBNXI_Initialization_1 - Uninitialized
   try
   {
      BinexData::UBNXI  u;
      report(testFramework, 0, 1, u);
   }
   catch (Exception& e)
   {
      testFramework.fail();
   }
   testFramework.next();


      // BinexData::UBNXI_Initialization_2..n - Initialization
   NumListType::const_iterator  numIter = ubnxiNumList.begin();
   for ( ; numIter != ubnxiNumList.end(); ++numIter)
   {
      try
      {
            // validate value and size of each number
         BinexData::UBNXI  u(numIter->value);
         report(testFramework,
                numIter->value, numIter->size,  // expected
                u);                             // actual
      }
      catch (Exception& e)
      {
            // was this an expected exception?
         testFramework.assert(numIter->size <= 0);
      }

      testFramework.next();
   }

   return testFramework.countFails();
}


//---------------------------------------------------------------------------
int BinexTypes_T :: doUbnxiEncodeDecodeTests()
{
   TestUtil  testFramework( "BinexData::UBNXI", "Encode/Decode", __FILE__, __LINE__ );

   NumListType::const_iterator  numIter = ubnxiNumList.begin();
   for ( ; numIter != ubnxiNumList.end(); ++numIter)
   {
      for (short isLittleEndian = 0; isLittleEndian <= 1; ++isLittleEndian)
      {
         try
         {
            string  buffer;
            size_t       offset = 0;

            BinexData::UBNXI  u1(numIter->value);
            u1.encode(buffer, offset, (bool)isLittleEndian);

            BinexData::UBNXI  u2;
            u2.decode(buffer, offset, (bool)isLittleEndian);

            report(testFramework,
                   numIter->value, numIter->size,  // expected
                   u2,                             // actual
                   (bool)isLittleEndian);
         }
         catch (Exception& e)
         {
               // was this an expected exception?
            testFramework.assert(numIter->size <= 0);            
         }

         testFramework.next();
      }
   }

   return testFramework.countFails();
}


//---------------------------------------------------------------------------
int BinexTypes_T :: doMgfziInitializationTests()
{
   TestUtil  testFramework( "BinexData::MGFZI", "Initialization", __FILE__, __LINE__ );

      // BinexData::MGFZI_Initialization_1 - Uninitialized
   try
   {
      BinexData::MGFZI  m;
      report(testFramework, 0, 1, m);
   }
   catch (Exception& e)
   {
      testFramework.fail();
   }
   testFramework.next();


      // BinexData::MGFZI_Initialization_2..n - Initialization
   NumListType::const_iterator  numIter = mgfziNumList.begin();
   for ( ; numIter != mgfziNumList.end(); ++numIter)
   {
      try
      {
            // validate value and size of each number
         BinexData::MGFZI  m(numIter->value);
         report(testFramework,
                numIter->value, numIter->size,  // expected
                m);                             // actual
      }
      catch (Exception& e)
      {
            // was this an expected exception?
         testFramework.assert(numIter->size <= 0);
      }

      testFramework.next();
   }

   return testFramework.countFails();
}


//---------------------------------------------------------------------------
int BinexTypes_T :: doMgfziEncodeDecodeTests()
{
   TestUtil  testFramework( "BinexData::MGFZI", "Encode/Decode", __FILE__, __LINE__ );

   NumListType::const_iterator  numIter = mgfziNumList.begin();
   for ( ; numIter != mgfziNumList.end(); ++numIter)
   {
      for (short isLittleEndian = 0; isLittleEndian <= 1; ++isLittleEndian)
      {
         try
         {
            string  buffer;
            size_t  offset = 0;

            BinexData::MGFZI  m1(numIter->value);
            m1.encode(buffer, offset, (bool)isLittleEndian);

            BinexData::MGFZI  m2;
            m2.decode(buffer, offset, (bool)isLittleEndian);

            report(testFramework,
                   numIter->value, numIter->size,  // expected
                   m2,                             // actual
                   (bool)isLittleEndian);
         }
         catch (Exception& e)
         {
               // was this an expected exception?
            testFramework.assert(numIter->size <= 0);            
         }
         testFramework.next();
      }
   }

   return testFramework.countFails();
}


//---------------------------------------------------------------------------
void BinexTypes_T::report(TestUtil&               test,
                          const unsigned long     expectedValue,
                          const size_t            expectedSize,
                          const BinexData::UBNXI& actual,
                          const bool              isLittleEndian)
{
   unsigned long  actualValue = (unsigned long)actual;
   size_t         actualSize  = actual.getSize();
   
   if (  (expectedValue != (unsigned long)actualValue)
      || (expectedSize  != actualSize) )
   {
      test.print();  // FAIL

      if (verboseLevel > 1)
      {
         cout << "        Expected Value = " << expectedValue << endl;
         cout << "        Actual Value   = " << actualValue   << endl;
         cout << "        Expected Size  = " << expectedSize  << endl;
         cout << "        Actual Size    = " << actualSize    << endl;
         cout << "        Endian         = "
              << (isLittleEndian ? "little" : "BIG") << endl;
      }
      /*
      string   bytes;
      size_t        offset = 0;
      actual.encode(bytes, offset); //, littleEndian);
      cout << "        Raw Hex Bytes  =";
      for (size_t i = 0; i < bytes.size(); i++)
      {
         cout << " " << hex << (unsigned short)bytes[i];
      }
      cout << dec << endl;
      */
   }
   else
   {
      if (verboseLevel > 1)
      {
         test.print();  // PASS
         /*
         string   bytes;
         size_t        offset = 0;
         actual.encode(bytes, offset); //, littleEndian);
         cout << "        Value = " << (unsigned long)actual << "  Raw Hex Bytes  =";
         for (size_t i = 0; i < bytes.size(); i++)
         {
            cout << " " << hex << (unsigned short)bytes[i];
         }
         cout << dec << endl;
         */
      }
   }
}


//---------------------------------------------------------------------------
void BinexTypes_T::report(TestUtil&               test,
                          const long long         expectedValue,
                          const size_t            expectedSize,
                          const BinexData::MGFZI& actual,
                          const bool              isLittleEndian)
{
   long long  actualValue = (long long)actual;
   size_t     actualSize  = actual.getSize();
   
   if (  (expectedValue != (long long)actualValue)
      || (expectedSize  != actualSize) )
   {
      test.print();  // FAIL

      if (verboseLevel > 0)
      {
         cout << "        Expected Value = " << expectedValue << endl;
         cout << "        Actual Value   = " << actualValue   << endl;
         cout << "        Expected Size  = " << expectedSize  << endl;
         cout << "        Actual Size    = " << actualSize    << endl;
         cout << "        Endian         = "
              << (isLittleEndian ? "little" : "BIG") << endl;
      }
   }
   else
   {
      if (verboseLevel > 0)
      {
         test.print();  // PASS
      }
   }
}


//---------------------------------------------------------------------------
void BinexTypes_T :: dumpBuffer(const unsigned char* buffer, size_t size)
{
   cout << "       Raw Hex Bytes  =";
   for (size_t i = 0; i < size; i++)
   {
      cout << " " << hex << (unsigned short)buffer[i];
   }
   cout << dec << endl;
}


/** Run the program.
 *
 * @return 0 if successful
 */
int main(int argc, char *argv[])
{
   int  errorCount = 0;
   int  errorTotal = 0;

   BinexTypes_T  testClass;  // test data is loaded here

   errorCount = testClass.doUbnxiInitializationTests();
   errorTotal = errorTotal + errorCount;

   errorCount = testClass.doUbnxiEncodeDecodeTests();
   errorTotal = errorTotal + errorCount;

   errorCount = testClass.doMgfziInitializationTests();
   errorTotal = errorTotal + errorCount;

   errorCount = testClass.doMgfziEncodeDecodeTests();
   errorTotal = errorTotal + errorCount;

   return( errorTotal );
   
} // main()
