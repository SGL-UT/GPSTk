#pragma ident "$Id$"

#include <stdlib.h> // For lrand48()
#include "BinexData.hpp"
#include "BinexStream.hpp"
#include "BasicFramework.hpp"

/**
 * @file BinexReadWriteTest.cpp
 * 
 * Tests gpstk::BinexData
 */

using namespace std;
using namespace gpstk;

/**
 * 
 */
class BinexReadWriteTest : public BasicFramework
{
public:

      /**
       * 
       */
   BinexReadWriteTest(char* arg0);

      /**
       * 
       */
   virtual ~BinexReadWriteTest() {};

protected:

   enum TestDataType
   {
      eChar,
      eShort,
      eLong,
      eUBNXI,
      eMGFZI
   };
   
   typedef pair<TestDataType, void*>   TestData;
   typedef vector<TestData>            TestDataList;
   typedef vector<TestDataList>        TestDataListList;
   typedef vector<BinexData>           RecordList;

      /**
       * 
       */
   void process()
      throw();

      /**
       * 
       */
   void report(string description,
               bool   pass);

      /**
       * 
       */
   void report(string                  description,
               const unsigned long     expectedValue,
               const size_t            expectedSize,
               const BinexData::UBNXI& actual,
               const bool              littleEndian = false);

      /**
       * 
       */
   void report(string                  description,
               const long long         expectedValue,
               const size_t            expectedSize,
               const BinexData::MGFZI& actual,
               const bool              littleEndian = false);

      /**
       * 
       */
   void dumpBuffer(const unsigned char* buffer, size_t size);

   TestDataListList  testData;
  
   RecordList        testRecords;   
   
}; // class BinexReadWriteTest


//---------------------------------------------------------------------------
BinexReadWriteTest::BinexReadWriteTest(char* arg0) :
   BasicFramework (arg0, "Tests Binex record reading and writing")
{
   // Intentionally empty
}


//---------------------------------------------------------------------------
void BinexReadWriteTest::process()
   throw()
{
   if (verboseLevel > 0)
   {
      cout << "Creating BINEX records . . ." << endl;
   }
   
   for (short recNum = 0; recNum < 10; recNum++)
   {
      BinexData record(recNum);
      TestDataList      recordData;
      size_t            offset = 0;
      
      for (short dataNum = 0; dataNum < 80; dataNum++)
      {
         TestDataType whichType = (TestDataType)(rand() % (eMGFZI + 1) );
         void         *value = NULL;
         
         switch (whichType)
         {
            case eChar:
            {
               char c = (char)(rand() % 0x100);
               record.updateMessageData(offset, c, sizeof(c) );
               value  = new char(c);
               break;
            }   
            case eShort:
            {
               short s = (short)(rand() % 10000);
               record.updateMessageData(offset, s, sizeof(s) );
               value  = new short(s);
               break;
            }  
            case eLong:
            {
               long l = (long)(lrand48() );
               record.updateMessageData(offset, l, sizeof(l) );
               value  = new long(l);
               break;
            }   
            case eUBNXI:
            {
               BinexData::UBNXI u( (unsigned long)(abs(lrand48() ) ) % BinexData::UBNXI::MAX_VALUE);
               record.updateMessageData(offset, u);
               value = new BinexData::UBNXI(u);
               break;
            }   
            case eMGFZI:
            {
               BinexData::MGFZI m( (long long)(lrand48() ) );
               record.updateMessageData(offset, m);
               value = new BinexData::MGFZI(m);
               break;
            }   
            default:
                  // Internal error
               exit(1);
         }
         if (value != NULL)
         {
            recordData.push_back(TestData(whichType, value) );
         }
      }
      testData.push_back(recordData);      
      testRecords.push_back(record);
   }

   if (verboseLevel > 0)
   {
      cout << "Verifying BINEX records . . ." << endl;
   }
   TestDataListList::iterator dataListIter = testData.begin();
   RecordList::iterator       recordIter   = testRecords.begin();
   bool more = true;
   while (  (dataListIter != testData.end() )
         && (recordIter   != testRecords.end() ) )
   {
      TestDataList      dataList = *dataListIter;
      BinexData record   = *recordIter;
      try
      {
         size_t offset = 0;
         TestDataList::iterator dataIter = (*dataListIter).begin();
         while (dataIter != (*dataListIter).end() )
         {
            switch ( (*dataIter).first)
            {
               case eChar:
               {
                  string desc = "Comparing character record message data";
                  char   c;
                  record.extractMessageData(offset, c, sizeof(c) );
                  if (memcmp( (void*)&c, (*dataIter).second, sizeof(c) ) )
                  {
                     report(desc, false);
                     cout << "  Actual:   " << c << endl;
                     cout << "  Expected: " << *( (char*)(*dataIter).second) << endl;
                  }
                  else
                  {
                     report(desc, true);
                  }
                  break;
               }   
               case eShort:
               {
                  string desc = "Comparing short record message data";
                  short  s;
                  record.extractMessageData(offset, s, sizeof(s) );
                  if (memcmp( (void*)&s, (*dataIter).second, sizeof(s) ) )
                  {
                     report(desc, false);
                     cout << "  Actual:   " << s << endl;
                     cout << "  Expected: " << *( (char*)(*dataIter).second) << endl;
                  }
                  else
                  {
                     report(desc, true);
                  }
                  break;
               }  
               case eLong:
               {
                  string desc = "Comparing long record message data";
                  long   l;
                  record.extractMessageData(offset, l, sizeof(l) );
                  if (memcmp( (void*)&l, (*dataIter).second, sizeof(l) ) )
                  {
                     report(desc, false);
                     cout << "  Actual:   " << l << endl;
                     cout << "  Expected: " << *( (char*)(*dataIter).second) << endl;
                  }
                  else
                  {
                     report(desc, true);
                  }
                  break;
               }   
               case eUBNXI:
               {
                  string           desc = "Comparing UBNXI record message data";
                  BinexData::UBNXI u;
                  record.extractMessageData(offset, u);
                  if (u == *( (BinexData::UBNXI*)(*dataIter).second) )
                  {
                     report(desc, true);
                  }
                  else
                  {
                     report(desc, false);
                     cout << "  Actual:   " << (unsigned long)u << endl;
                     cout << "  Expected: " << (unsigned long)*( (BinexData::UBNXI*)(*dataIter).second) << endl;
                  }
                  break;
               }   
               case eMGFZI:
               {
                  string           desc = "Comparing MGFZI record message data";
                  BinexData::MGFZI m;
                  record.extractMessageData(offset, m);
                  if (m == *( (BinexData::MGFZI*)(*dataIter).second) )
                  {
                     report(desc, true);
                  }
                  else
                  {
                     report(desc, false);
                     cout << "  Actual:   " << (long long)m << endl;
                     cout << "  Expected: " << (long long)*( (BinexData::MGFZI*)(*dataIter).second) << endl;
                  }
                  break;
               }   
               default:
                     // Internal error
                  exit(1);
            }
            dataIter++;
         }
      }
      catch (FFStreamError e)
      {
         cout << "  FFStreamError reading record." << endl;
      }
      catch (...)
      {
         cout << "  Unknown error reading record." << endl;
      }
      dataListIter++;
      recordIter++;
      
   }

   if (verboseLevel > 0)
   {
      cout << "Writing BINEX file . . ." << endl;
   }
   BinexStream outStream("test.out", std::ios::out | std::ios::binary);
   outStream.exceptions(ios_base::failbit | ios_base::badbit);
   recordIter = testRecords.begin();
   while (recordIter != testRecords.end() )
   {
      try
      {
         (*recordIter).putRecord(outStream);
      }
      catch(...)
      {
         cout << "  Error writing record." << endl;
      }
      recordIter++;
   }
   outStream.close();
   
   if (verboseLevel > 0)
   {
      cout << "Reading BINEX file . . ." << endl;
   }
   BinexStream inStream("test.out", std::ios::in | std::ios::binary);   
   inStream.exceptions(ios_base::failbit);
   recordIter = testRecords.begin();
   while (inStream.good() && (EOF != inStream.peek() ) )
   {
      if (recordIter == testRecords.end() )
      {
         cout << "Stored records exhausted before file records - exiting." << endl;
         break;
      }
      BinexData record;
      try
      {
         record.getRecord(inStream);
         if (record == *recordIter)
         {
            report("Reading and comparing BINEX record", true);
         }
         else
         {
            report("Reading and comparing BINEX record", false);
            cout << "Actual record:" << endl;
            (*recordIter).dump(cout);
            cout << "Expected record:" << endl;
            record.dump(cout);
         }
      }
      catch (FFStreamError e)
      {
         cout << e << endl;
      }
      catch (...)
      {
         cout << "  Unknown error reading record." << endl;
      }
      recordIter++;      
   }
   inStream.close();

}


//---------------------------------------------------------------------------
void BinexReadWriteTest::report(string description,
                                bool   pass)
{
   if (pass)
   {
      if (verboseLevel > 1)
      {
         cout << " PASS - " << description << endl;
      }
   }
   else
   {
      cout << " FAIL - " << description << endl;
   }
}

/*
//---------------------------------------------------------------------------
void BinexReadWriteTest::report(string                  description,
                            const unsigned long     expectedValue,
                            const unsigned short    expectedSize,
                            const BinexData::UBNXI& actual,
                            const bool              littleEndian)
{
   unsigned long  actualValue = (unsigned long)actual;
   unsigned short actualSize  = actual.getSize();
   
   if (  (expectedValue != (unsigned long)actualValue)
      || (expectedSize  != actualSize) )
   {
      cout << " FAIL - " << description;
      if (littleEndian)
      {
         cout << " (Little Endian)" << endl;
      }
      else
      {
         cout << " (Big Endian)" << endl;
      }
      cout << "        Expected Value = " << expectedValue << endl;
      cout << "        Actual Value   = " << actualValue   << endl;
      cout << "        Expected Size  = " << expectedSize  << endl;
      cout << "        Actual Size    = " << actualSize    << endl;
      
      unsigned char bytes[8];
      unsigned char byteCount;
      actual.encode(bytes, byteCount); //, littleEndian);
      cout << "        Raw Hex Bytes  =";
      for (unsigned char i = 0; i < byteCount; i++)
      {
         cout << " " << hex << (unsigned short)bytes[i];
      }
      cout << dec << endl;
   }
   else
   {
      if (verboseLevel > 1)
      {
         cout << " PASS - " << description;
         if (littleEndian)
         {
            cout << " (Little Endian)" << endl;
         }
         else
         {
            cout << " (Big Endian)" << endl;
         }
         unsigned char bytes[8];
         unsigned char byteCount;
         actual.encode(bytes, byteCount); //, littleEndian);
         cout << "        Value = " << (unsigned long)actual << "  Raw Hex Bytes  =";
         for (unsigned char i = 0; i < byteCount; i++)
         {
            cout << " " << hex << (unsigned short)bytes[i];
         }
         cout << dec << endl;
      }
   }
}


//---------------------------------------------------------------------------
void BinexReadWriteTest::report(string                  description,
                            const long long         expectedValue,
                            const unsigned short    expectedSize,
                            const BinexData::MGFZI& actual,
                            const bool              littleEndian)
{
   long long       actualValue = (long long)actual;
   unsigned short  actualSize  = actual.getSize();
   
   if (  (expectedValue != (long long)actualValue)
      || (expectedSize  != actualSize) )
   {
      cout << " FAIL - " << description;
      if (littleEndian)
      {
         cout << " (Little Endian)" << endl;
      }
      else
      {
         cout << " (Big Endian)" << endl;
      }
      cout << "        Expected Value = " << expectedValue << endl;
      cout << "        Actual Value   = " << actualValue   << endl;
      cout << "        Expected Size  = " << expectedSize  << endl;
      cout << "        Actual Size    = " << actualSize    << endl;
      
      unsigned char bytes[8];
      unsigned char byteCount;
      actual.encode(bytes, byteCount); //, littleEndian);
      cout << "        Raw Hex Bytes  =";
      for (unsigned char i = 0; i < byteCount; i++)
      {
         cout << " " << hex << (unsigned short)bytes[i];
      }
      cout << dec << endl;
   }
   else
   {
      if (verboseLevel > 1)
      {
         cout << " PASS - " << description;
         if (littleEndian)
         {
            cout << " (Little Endian)" << endl;
         }
         else
         {
            cout << " (Big Endian)" << endl;
         }
         unsigned char bytes[8];
         unsigned char byteCount;
         actual.encode(bytes, byteCount); //, littleEndian);
         cout << "        Value = " << actualValue << "  Raw Hex Bytes  =";
         for (unsigned char i = 0; i < byteCount; i++)
         {
            cout << " " << hex << (unsigned short)bytes[i];
         }
         cout << dec << endl;
      }
   }
}
*/

//---------------------------------------------------------------------------
void BinexReadWriteTest::dumpBuffer(const unsigned char* buffer, size_t size)
{
   cout << "       Raw Hex Bytes  =";
   for (size_t i = 0; i < size; i++)
   {
      cout << " " << hex << (unsigned short)buffer[i];
   }
   cout << dec << endl;
}


/**
 * Returns 0 if successful.
 */
main(int argc, char *argv[])
{
   BinexReadWriteTest app(argv[0]);

   if (!app.initialize(argc, argv) )
   {
      return 0;
   }

   if (!app.run() )
   {
      return 1;
   }

   exit(0);
   
} // main()
