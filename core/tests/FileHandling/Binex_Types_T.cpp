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
#include <iostream>

using namespace std;
using namespace gpstk;

//=============================================================================
// Class declarations
//=============================================================================

class BinexTypes_T
{
public:

      // constructor
   BinexTypes_T() : verboseLevel(0)
   {
      init();
   };

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


      // output raw data in a readable format
   void dumpBuffer(const unsigned char* buffer, size_t size);

}; // class BinexTypes_T


//============================================================
// Initialize Test Data Filenames and Values
//============================================================

void BinexTypes_T :: init( void )
{

   TestUtil  test0;
   string  dataFilePath = gpstk::getPathData();
   string  tempFilePath = gpstk::getPathTestTemp();

      //----------------------------------------
      // Full file paths
      //----------------------------------------
   inputUbnxiNums = dataFilePath + gpstk::getFileSep()
                    + "test_input_binex_types_Ubnxi.txt";

   inputMgfziNums = dataFilePath + gpstk::getFileSep()
                    + "test_input_binex_types_Mgfzi.txt";

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
   TestUtil  tester( "BinexData::UBNXI", "Initialization", __FILE__, __LINE__ );

      // BinexData::UBNXI_Initialization_1 - Uninitialized
   try
   {
      BinexData::UBNXI  u;

      unsigned long  actualValue = (unsigned long)u;
      size_t         actualSize  = u.getSize();

      ostringstream  oss;
      oss << "Expected Value = " << 0
          << " ; Actual Value = " << actualValue
          << " ; Expected Size = " << 1
          << " ; Actual Size = " << actualSize;

      tester.assert( (  (0 == actualValue)
                        && (1 == actualSize) ),
                     oss.str(), __LINE__ );
   }
   catch (Exception& e)
   {
      ostringstream  oss;
      oss << "exception creating uninitialized UBNXI" << e;
      tester.assert( false, oss.str(), __LINE__ );
   }

      // BinexData::UBNXI_Initialization_2..n - Initialization
   NumListType::const_iterator  numIter = ubnxiNumList.begin();
   for ( ; numIter != ubnxiNumList.end(); ++numIter)
   {
      try
      {
            // validate value and size of each number
         BinexData::UBNXI  u(numIter->value);

         unsigned long  actualValue = (unsigned long)u;
         size_t         actualSize  = u.getSize();

         ostringstream  oss;
         oss << "Expected Value = " << numIter->value
             << " ; Actual Value = " << actualValue
             << " ; Expected Size = " << numIter->size
             << " ; Actual Size = " << actualSize;

         tester.assert( (  (numIter->value == actualValue)
                           && (numIter->size  == actualSize) ),
                        oss.str(), __LINE__ );
      }
      catch (Exception& e)
      {
            // was this an expected exception?
         ostringstream  oss;
         oss << "unexpected exception creating initialized UBNXI" << e;
         tester.assert( (numIter->size <= 0) , oss.str(), __LINE__ );
      }
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int BinexTypes_T :: doUbnxiEncodeDecodeTests()
{
   TestUtil  tester( "BinexData::UBNXI", "Encode/Decode", __FILE__, __LINE__ );

   NumListType::const_iterator  numIter = ubnxiNumList.begin();
   for ( ; numIter != ubnxiNumList.end(); ++numIter)
   {
      for (short isLittleEndian = 0; isLittleEndian <= 1; ++isLittleEndian)
      {
         try
         {
            string  buffer;
            size_t  offset = 0;

            BinexData::UBNXI  u1(numIter->value);
            u1.encode(buffer, offset, (bool)isLittleEndian);

            BinexData::UBNXI  u2;
            u2.decode(buffer, offset, (bool)isLittleEndian);

            unsigned long  actualValue = (unsigned long)u2;
            size_t         actualSize  = u2.getSize();

            ostringstream  oss;
            oss << "Expected Value = " << numIter->value
                << " ; Actual Value = " << actualValue
                << " ; Expected Size = " << numIter->size
                << " ; Actual Size = " << actualSize
                << " ; Endian = " << (isLittleEndian ? "little" : "BIG");

            tester.assert( (  (numIter->value == actualValue)
                              && (numIter->size  == actualSize) ),
                           oss.str(), __LINE__ );
         }
         catch (Exception& e)
         {
               // was this an expected exception?
            ostringstream  oss;
            oss << "unexpected exception encoding/decoding UBNXI" << e;
            tester.assert( (numIter->size <= 0), oss.str(), __LINE__ );
         }
      }
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int BinexTypes_T :: doMgfziInitializationTests()
{
   TestUtil  tester( "BinexData::MGFZI", "Initialization", __FILE__, __LINE__ );

      // BinexData::MGFZI_Initialization_1 - Uninitialized
   try
   {
      BinexData::MGFZI  m;

      long long  actualValue = (long long)m;
      size_t     actualSize  = m.getSize();

      ostringstream  oss;
      oss << "Expected Value = " << 0
          << " ; Actual Value = " << actualValue
          << " ; Expected Size = " << 1
          << " ; Actual Size = " << actualSize;

      tester.assert( (  (0 == actualValue)
                        && (1 == actualSize) ),
                     oss.str(), __LINE__ );
   }
   catch (Exception& e)
   {
      ostringstream  oss;
      oss << "exception creating uninitialized MGFZI" << e;
      tester.assert( false, oss.str(), __LINE__ );
   }

      // BinexData::MGFZI_Initialization_2..n - Initialization
   NumListType::const_iterator  numIter = mgfziNumList.begin();
   for ( ; numIter != mgfziNumList.end(); ++numIter)
   {
      try
      {
            // validate value and size of each number
         BinexData::MGFZI  m(numIter->value);

         long long  actualValue = (long long)m;
         size_t     actualSize  = m.getSize();

         ostringstream  oss;
         oss << "Expected Value = " << numIter->value
             << " ; Actual Value = " << actualValue
             << " ; Expected Size = " << numIter->size
             << " ; Actual Size = " << actualSize;

         tester.assert( (  (numIter->value == actualValue)
                           && (numIter->size  == actualSize) ),
                        oss.str(), __LINE__ );
      }
      catch (Exception& e)
      {
            // was this an expected exception?
         ostringstream  oss;
         oss << "unexpected exception creating initialized MGFZI" << e;
         tester.assert( (numIter->size <= 0) , oss.str(), __LINE__ );
      }
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int BinexTypes_T :: doMgfziEncodeDecodeTests()
{
   TestUtil  tester( "BinexData::MGFZI", "Encode/Decode", __FILE__, __LINE__ );

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

            long long  actualValue = (long long)m2;
            size_t     actualSize  = m2.getSize();

            ostringstream  oss;
            oss << "Expected Value = " << numIter->value
                << " ; Actual Value = " << actualValue
                << " ; Expected Size = " << numIter->size
                << " ; Actual Size = " << actualSize
                << " ; Endian = " << (isLittleEndian ? "little" : "BIG");

            tester.assert( (  (numIter->value == actualValue)
                              && (numIter->size  == actualSize) ),
                           oss.str(), __LINE__ );
         }
         catch (Exception& e)
         {
               // was this an expected exception?
            ostringstream  oss;
            oss << "unexpected exception encoding/decoding initialized MGFZI" << e;
            tester.assert( (numIter->size <= 0) , oss.str(), __LINE__ );
         }
      }
   }

   return tester.countFails();
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
    * @return Total error count for all tests
    */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   BinexTypes_T  testClass;  // test data is loaded here

   errorTotal += testClass.doUbnxiInitializationTests();

   errorTotal += testClass.doUbnxiEncodeDecodeTests();

   errorTotal += testClass.doMgfziInitializationTests();

   errorTotal += testClass.doMgfziEncodeDecodeTests();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal <<
             std::endl;

   return( errorTotal );

} // main()
