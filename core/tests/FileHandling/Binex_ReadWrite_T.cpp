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
#include "BinexStream.hpp"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

//=============================================================================
// Class declarations
//=============================================================================
class BinexReadWrite_T
{
public:

      // constructor
   BinexReadWrite_T(int v = 0) : verboseLevel(v)
   {
      init();
   };

      // destructor
   virtual ~BinexReadWrite_T() {};

      // initialize tests
   void init();

      // test methods
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int doForwardTests();
   int doReverseTests();

   unsigned  verboseLevel;  // amount to display during tests, 0 = least

private:

   enum TestDataType
   {
      CharType  = 'c',
      ShortType = 's',
      LongType  = 'l',
      UbnxiType = 'U',
      MgfziType = 'M'
   };

   struct TestData
   {
      TestDataType  dtype;
      long long     value;
   };

   typedef vector<TestData>      TestDataList;
   typedef vector<TestDataList>  TestDataListList;
   typedef vector<BinexData>     RecordList;

      // @param[in] c character to convert
      // @param[in/out] dtype data type derived from c
      // @return true if the character is a valid data type,
      //         false otherwise
   bool charToType(char c, TestDataType& t);

      // read a list of numbers (one per line) from the specified file
      // @return true on success, false on failure
   bool readNums(const string& filename);

      // generate BINEX records from the contents of numList
      // @return true if successful, false otherwise
   bool createRecs();

      /**
       *
       */
   void dumpBuffer(const unsigned char* buffer, size_t size);

   TestDataList  numList;

   TestDataListList  testData;

   RecordList  testRecords;

}; // class BinexReadWrite_T


//============================================================
// Initialize Test Data Filenames and Values
//============================================================

void BinexReadWrite_T :: init( void )
{

   TestUtil  testUtil;
   string  dataFilePath = gpstk::getPathData();

      //----------------------------------------
      // Full file paths
      //----------------------------------------
   string  inputFile = dataFilePath + gpstk::getFileSep()
                       + "test_input_binex_readwrite.txt";

   if (verboseLevel > 0)
   {
      cout << "  Reading test input . . ." << endl;
   }
   readNums(inputFile);

   if (verboseLevel > 0)
   {
      cout << "    " << numList.size() << " numbers" << endl;
      cout << "  Creating BINEX records . . ." << endl;
   }
   createRecs();
   if (verboseLevel > 0)
   {
      cout << "    " << testRecords.size() << " records" << endl;
   }
}


bool BinexReadWrite_T :: charToType(char c, TestDataType& t)
{
   switch (c)
   {
      case CharType:
         t = CharType;
         break;
      case ShortType:
         t = ShortType;
         break;
      case LongType:
         t = LongType;
         break;
      case UbnxiType:
         t = UbnxiType;
         break;
      case MgfziType:
         t = MgfziType;
         break;

      default:
         return false;
   }
   return true;
}


//---------------------------------------------------------------------------
bool BinexReadWrite_T :: readNums(const string& filename)
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
         TestData  num;
         char  c;
         iss >> c;

            // make sure the type is valid
         if (charToType(c, num.dtype))
         {
            iss >> num.value;
            numList.push_back(num);
         }
         else
         {
            if (verboseLevel > 0)
            {
               cout << "  Warning: Unrecognized data type: " << c << endl;
            }
         }
      }
   }
   return (numList.size() > 0);
}


bool BinexReadWrite_T :: createRecs()
{
   TestDataList::const_iterator  tdIter = numList.begin();
   while (tdIter != numList.end() )
   {
      BinexData     record(1);
      TestDataList  recordData;
      size_t        offset = 0;

         // create multiple records with 9 fields each
      short  dataNum = 0;
      for ( ; (dataNum < 9) && (tdIter != numList.end()); ++dataNum, ++tdIter)
      {
         switch (tdIter->dtype)
         {
            case CharType:
            {
               char  c = tdIter->value;
               record.updateMessageData(offset, c, sizeof(c) );
               break;
            }
            case ShortType:
            {
               short  s = tdIter->value;
               record.updateMessageData(offset, s, sizeof(s) );
               break;
            }
            case LongType:
            {
               long  l = tdIter->value;
               record.updateMessageData(offset, l, sizeof(l) );
               break;
            }
            case UbnxiType:
            {
               BinexData::UBNXI  u(tdIter->value);
               record.updateMessageData(offset, u);
               break;
            }
            case MgfziType:
            {
               BinexData::MGFZI  m(tdIter->value);
               record.updateMessageData(offset, m);
               break;
            }
            default:
                  // Internal error
               cout << "  Internal error during record creation" << std::endl;
               return false;
         }
         recordData.push_back(*tdIter);
      }
      testData.push_back(recordData);
      testRecords.push_back(record);
   }
   return true;
}


int BinexReadWrite_T :: doForwardTests()
{
   TestUtil  tester( "BinexData", "Read/Write (Fwd)", __FILE__, __LINE__ );

   string  tempFilePath = gpstk::getPathTestTemp();
   string  tempFileName = tempFilePath + gpstk::getFileSep() +
                          "test_output_binex_readwrite.binex";
   BinexStream  outStream(tempFileName.c_str(),
                          std::ios::out | std::ios::binary);

   tester.assert( outStream.good(), "error creating ouput stream", __LINE__ );

   outStream.exceptions(ios_base::failbit | ios_base::badbit);
   RecordList::iterator  recordIter = testRecords.begin();
   for ( ; recordIter != testRecords.end(); ++recordIter)
   {
      try
      {
         (*recordIter).putRecord(outStream);
         tester.assert( true, "put record", __LINE__ );
      }
      catch (Exception& e)
      {
         ostringstream  oss;
         oss << "exception writing record: " << e;
         tester.assert( false, oss.str(), __LINE__ );
      }
      catch (...)
      {
         tester.assert( false, "unknown exception writing record", __LINE__ );
      }
   }
   outStream.close();

   BinexStream  inStream(tempFileName.c_str(),
                         std::ios::in | std::ios::binary);
   inStream.exceptions(ios_base::failbit);

   tester.assert( inStream.good(), "error creating input stream", __LINE__ );

   recordIter = testRecords.begin();
   while (inStream.good() && (EOF != inStream.peek() ) )
   {
      if (recordIter == testRecords.end() )
      {
         tester.assert( false, "stored records exhausted before file records",
                        __LINE__ );
         break;
      }
      BinexData record;
      try
      {
         record.getRecord(inStream);
         if (record == *recordIter)
         {
            tester.assert( true, "get record", __LINE__ );
         }
         else
         {
            ostringstream  oss;
            oss << "Actual record:" << endl;
            (*recordIter).dump(oss);
            oss << "Expected record:" << endl;
            record.dump(oss);

            tester.assert( false, oss.str(), __LINE__ );
         }
      }
      catch (Exception& e)
      {
         ostringstream  oss;
         oss << "stream exception reading record: " << e;
         tester.assert( false, oss.str(), __LINE__ );
      }
      catch (...)
      {
         tester.assert( false, "unknown exception reading record", __LINE__ );
      }

      recordIter++;
   }
   inStream.close();

   return tester.countFails();
}


int BinexReadWrite_T :: doReverseTests()
{
   TestUtil  tester( "BinexData", "Read/Write (Rev)", __FILE__, __LINE__ );

      // @todo

   return tester.countFails();
}


   /** Run the program.
    *
    * @return Total error count for all tests
    */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   BinexReadWrite_T  testClass;  // test data is loaded here

   errorTotal += testClass.doForwardTests();

      //errorTotal += testClass.doReverseTests();

   return( errorTotal );

} // main()
