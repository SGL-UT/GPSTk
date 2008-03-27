#pragma ident "$Id$"

#include <stdlib.h> // For lrand48()
#include "BinexData.hpp"
#include "BasicFramework.hpp"

/**
 * @file binex_types_test.cpp
 * 
 * Tests gpstk::BinexData::UBNXI, gpstk::BinexData::MGFZI
 */

using namespace std;
using namespace gpstk;

/**
 * 
 */
class BinexTypesTest : public BasicFramework
{
public:

      /**
       * 
       */
   BinexTypesTest(char* arg0);

      /**
       * 
       */
   virtual ~BinexTypesTest() {};

protected:

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

}; // class BinexTypesTest


//---------------------------------------------------------------------------
BinexTypesTest::BinexTypesTest(char* arg0) :
   BasicFramework (arg0, "Tests Binex types UBNXI and MGFZI")
{
   // Intentionally empty
}


//---------------------------------------------------------------------------
void BinexTypesTest::process()
   throw()
{
   // Test UBNXI class
   if (verboseLevel > 0)
   {
      cout << "Testing UBNXI initialization . . ." << endl;
   }
   {
      BinexData::UBNXI u;
      report("UBNXI: Uninitialized", 0, 1, u);
   }
   {
      BinexData::UBNXI u(0);
      report("UBNXI: Initialize boundary condition", 0, 1, u);
   }
   {
      BinexData::UBNXI u(127);
      report("UBNXI: Initialize boundary condition", 127, 1, u);
   }
   {
      BinexData::UBNXI u(128);
      report("UBNXI: Initialize boundary condition", 128, 2, u);
   }
   {
      BinexData::UBNXI u(16383);
      report("UBNXI: Initialize boundary condition", 16383, 2, u);
   }
   {
      BinexData::UBNXI u(16384);
      report("UBNXI: Initialize boundary condition", 16384, 3, u);
   }
   {
      BinexData::UBNXI u(2097151);
      report("UBNXI: Initialize boundary condition", 2097151, 3, u);
   }
   {
      BinexData::UBNXI u(2097152);
      report("UBNXI: Initialize boundary condition", 2097152, 4, u);
   }
   {
      BinexData::UBNXI u(536870911);
      report("UBNXI: Initialize boundary condition", 536870911, 4, u);
   }
   try
   {      
      BinexData::UBNXI u(536870912);
      report("UBNXI: Overflow exception not generated for 536870912", false);
   }
   catch (Exception e)
   {
      report("UBNXI: Overflow exception correctly generated for 536870912", true);
   }

   if (verboseLevel > 0)
   {
      cout << "Testing UBNXI encoding/decoding . . ." << endl;
   }
   for (unsigned short littleEndian = 0; littleEndian <= 1; littleEndian++)
   {
      BinexData::UBNXI u2;
      //unsigned char    buffer[4];
      std::string      buffer;
      size_t           offset = 0;
      size_t           len;
      string           descB = "UBNXI: Encode/decode, boundary condition";
      string           descR = "UBNXI: Encode/decode, random";
            
      {
         BinexData::UBNXI u1(0);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 0, 1, u2, (bool)littleEndian);
      }
      {
         BinexData::UBNXI u1(127);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 127, 1, u2, (bool)littleEndian);
      }
      {
         BinexData::UBNXI u1(128);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 128, 2, u2, (bool)littleEndian);
      }
      {
         BinexData::UBNXI u1(506);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 506, 2, u2, (bool)littleEndian);
      }
      {
         BinexData::UBNXI u1(15619);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 15619, 2, u2, (bool)littleEndian);
      }
      {
         BinexData::UBNXI u1(16383);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 16383, 2, u2, (bool)littleEndian);
      }
      {
         BinexData::UBNXI u1(16384);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 16384, 3, u2, (bool)littleEndian);
      }
      {
         BinexData::UBNXI u1(2097151);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 2097151, 3, u2, (bool)littleEndian);
      }
      {
         BinexData::UBNXI u1(2097152);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 2097152, 4, u2, (bool)littleEndian);
      }
      {
         BinexData::UBNXI u1(536870911);
         u1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         u2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 536870911, 4, u2, (bool)littleEndian);
      }
      
      for (unsigned long i = 1000; i <  BinexData::UBNXI::MAX_VALUE; i <<= 3)
      {
         for (unsigned long j = 0; j < 50; j++)
         {
            unsigned long v = (unsigned long)lrand48() % i;
            BinexData::UBNXI u1(v);
            u1.encode(buffer, offset, (bool)littleEndian);
            u2.decode(buffer, offset, (bool)littleEndian);
            report(descR, v, u1.getSize(), u2, (bool)littleEndian);
         }
      }
      
   }

   // Test MGFZI class
   if (verboseLevel > 0)
   {
      cout << "Testing MGFZI initialization . . ." << endl;
   }
   {
      BinexData::MGFZI m;
      report("MGFZI: Uninitialized", 0, 1, m);
   }
   {
      BinexData::MGFZI m(0);
      report("MGFZI: Initialize boundary condition", 0, 1, m);
   }
   {
      BinexData::MGFZI m(15);
      report("MGFZI: Initialize boundary condition", 15, 1, m);
   }
   {
      BinexData::MGFZI m(-15);
      report("MGFZI: Initialize boundary condition", -15, 1, m);
   }
   {
      BinexData::MGFZI m(16);
      report("MGFZI: Initialize boundary condition", 16, 2, m);
   }
   {
      BinexData::MGFZI m(-16);
      report("MGFZI: Initialize boundary condition", -16, 2, m);
   }
   {
      BinexData::MGFZI m(4109);
      report("MGFZI: Initialize boundary condition", 4109, 2, m);
   }
   {
      BinexData::MGFZI m(-4109);
      report("MGFZI: Initialize boundary condition", -4109, 2, m);
   }
   {
      BinexData::MGFZI m(4110);
      report("MGFZI: Initialize boundary condition", 4110, 3, m);
   }
   {
      BinexData::MGFZI m(-4110);
      report("MGFZI: Initialize boundary condition", -4110, 3, m);
   }
   {
      BinexData::MGFZI m(1052684);
      report("MGFZI: Initialize boundary condition", 1052684, 3, m);
   }
   {
      BinexData::MGFZI m(-1052684);
      report("MGFZI: Initialize boundary condition", -1052684, 3, m);
   }
   {
      BinexData::MGFZI m(1052685);
      report("MGFZI: Initialize boundary condition", 1052685, 4, m);
   }
   {
      BinexData::MGFZI m(-1052685);
      report("MGFZI: Initialize boundary condition", -1052685, 4, m);
   }
   {
      BinexData::MGFZI m(269488139);
      report("MGFZI: Initialize boundary condition", 269488139, 4, m);
   }
   {
      BinexData::MGFZI m(-269488139);
      report("MGFZI: Initialize boundary condition", -269488139, 4, m);
   }
   {
      BinexData::MGFZI m(269488140);
      report("MGFZI: Initialize boundary condition", 269488140, 5, m);
   }
   {
      BinexData::MGFZI m(-269488140);
      report("MGFZI: Initialize boundary condition", -269488140, 5, m);
   }
   {
      BinexData::MGFZI m(68988964874LL);
      report("MGFZI: Initialize boundary condition", 68988964874LL, 5, m);
   }
   {
      BinexData::MGFZI m(-68988964874LL);
      report("MGFZI: Initialize boundary condition", -68988964874LL, 5, m);
   }
   {
      BinexData::MGFZI m(68988964875LL);
      report("MGFZI: Initialize boundary condition", 68988964875LL, 6, m);
   }
   {
      BinexData::MGFZI m(-68988964875LL);
      report("MGFZI: Initialize boundary condition", -68988964875LL, 6, m);
   }
   {
      BinexData::MGFZI m(17661175009289LL);
      report("MGFZI: Initialize boundary condition", 17661175009289LL, 6, m);
   }
   {
      BinexData::MGFZI m(-17661175009289LL);
      report("MGFZI: Initialize boundary condition", -17661175009289LL, 6, m);
   }
   {
      BinexData::MGFZI m(17661175009290LL);
      report("MGFZI: Initialize boundary condition", 17661175009290LL, 7, m);
   }
   {
      BinexData::MGFZI m(-17661175009290LL);
      report("MGFZI: Initialize boundary condition", -17661175009290LL, 7, m);
   }
   {
      BinexData::MGFZI m(4521260802379784LL);
      report("MGFZI: Initialize boundary condition", 4521260802379784LL, 7, m);
   }
   {
      BinexData::MGFZI m(-4521260802379784LL);
      report("MGFZI: Initialize boundary condition", -4521260802379784LL, 7, m);
   }
   {
      BinexData::MGFZI m(4521260802379785LL);
      report("MGFZI: Initialize boundary condition", 4521260802379785LL, 8, m);
   }
   {
      BinexData::MGFZI m(-4521260802379785LL);
      report("MGFZI: Initialize boundary condition", -4521260802379785LL, 8, m);
   }
   {
      BinexData::MGFZI m(BinexData::MGFZI::MAX_VALUE);
      report("MGFZI: Initialize boundary condition", BinexData::MGFZI::MAX_VALUE, 8, m);
   }
   {
      BinexData::MGFZI m(BinexData::MGFZI::MIN_VALUE);
      report("MGFZI: Initialize boundary condition", BinexData::MGFZI::MIN_VALUE, 8, m);
   }
   try
   {      
      BinexData::MGFZI m(BinexData::MGFZI::MAX_VALUE + 1);
      report("MGFZI: Overflow exception not generated for BinexData::MGFZI::MAX_VALUE + 1", false);
   }
   catch (Exception e)
   {
      report("MGFZI: Overflow exception correctly generated for BinexData::MGFZI::MAX_VALUE + 1", true);
   }
   try
   {      
      BinexData::MGFZI m(BinexData::MGFZI::MIN_VALUE - 1);
      report("MGFZI: Underflow exception not generated for BinexData::MGFZI::MIN_VALUE - 1", false);
   }
   catch (Exception e)
   {
      report("MGFZI: Underflow exception correctly generated for BinexData::MGFZI::MIN_VALUE - 1", true);
   }

   if (verboseLevel > 0)
   {
      cout << "Testing MGFZI encoding/decoding . . ." << endl;
   }
   for (unsigned short littleEndian = 0; littleEndian <= 1; littleEndian++)
   {
      BinexData::MGFZI m2;
      //unsigned char    buffer[8];
      std::string      buffer;
      size_t           offset = 0;
      size_t           len;
      string           descB = "MGFZI: Encode/decode, boundary condition";
      string           descR = "MGFZI: Encode/decode, random";

      {
         BinexData::MGFZI m1(0);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 0, 1, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(15);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 15, 1, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-15);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -15, 1, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(16);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 16, 2, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-16);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -16, 2, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(4109);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 4109, 2, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-4109);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -4109, 2, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(4110);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 4110, 3, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-4110);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -4110, 3, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(1052684);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 1052684, 3, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-1052684);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -1052684, 3, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(1052685);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 1052685, 4, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-1052685);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -1052685, 4, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(269488139);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 269488139, 4, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-269488139);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -269488139, 4, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(269488140);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 269488140, 5, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-269488140);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -269488140, 5, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(68988964874LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 68988964874LL, 5, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-68988964874LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -68988964874LL, 5, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(68988964875LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 68988964875LL, 6, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-68988964875LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -68988964875LL, 6, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(17661175009289LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 17661175009289LL, 6, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-17661175009289LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -17661175009289LL, 6, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(17661175009290LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 17661175009290LL, 7, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-17661175009290LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -17661175009290LL, 7, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(4521260802379784LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 4521260802379784LL, 7, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-4521260802379784LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -4521260802379784LL, 7, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(4521260802379785LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, 4521260802379785LL, 8, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(-4521260802379785LL);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, -4521260802379785LL, 8, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(BinexData::MGFZI::MAX_VALUE);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, BinexData::MGFZI::MAX_VALUE, 8, m2, (bool)littleEndian);
      }
      {
         BinexData::MGFZI m1(BinexData::MGFZI::MIN_VALUE);
         m1.encode(buffer, offset, (bool)littleEndian);
         //dumpBuffer(buffer, len);
         m2.decode(buffer, offset, (bool)littleEndian);
         report(descB, BinexData::MGFZI::MIN_VALUE, 8, m2, (bool)littleEndian);
      }
      
      for (long long i = 1000; i <  BinexData::MGFZI::MAX_VALUE; i <<= 3)
      {
         for (unsigned long j = 0; j < 50; j++)
         {
            long long v = ( (long long)lrand48() | ( (long long)lrand48() ) << 32) % i;
            BinexData::MGFZI m1(v);
            m1.encode(buffer, offset, (bool)littleEndian);
            m2.decode(buffer, offset, (bool)littleEndian);
            report(descR, v, m1.getSize(), m2, (bool)littleEndian);
         }
      }
      
   }

}


//---------------------------------------------------------------------------
void BinexTypesTest::report(string description,
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


//---------------------------------------------------------------------------
void BinexTypesTest::report(string                  description,
                            const unsigned long     expectedValue,
                            const size_t            expectedSize,
                            const BinexData::UBNXI& actual,
                            const bool              littleEndian)
{
   unsigned long  actualValue = (unsigned long)actual;
   size_t         actualSize  = actual.getSize();
   
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

      std::string   bytes;
      size_t        offset = 0;
      actual.encode(bytes, offset); //, littleEndian);
      cout << "        Raw Hex Bytes  =";
      for (size_t i = 0; i < bytes.size(); i++)
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
         std::string   bytes;
         size_t        offset = 0;
         actual.encode(bytes, offset); //, littleEndian);
         cout << "        Value = " << (unsigned long)actual << "  Raw Hex Bytes  =";
         for (size_t i = 0; i < bytes.size(); i++)
         {
            cout << " " << hex << (unsigned short)bytes[i];
         }
         cout << dec << endl;
      }
   }
}


//---------------------------------------------------------------------------
void BinexTypesTest::report(string                  description,
                            const long long         expectedValue,
                            const size_t            expectedSize,
                            const BinexData::MGFZI& actual,
                            const bool              littleEndian)
{
   long long  actualValue = (long long)actual;
   size_t     actualSize  = actual.getSize();
   
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
      
      std::string   bytes;
      size_t        offset = 0;
      actual.encode(bytes, offset); //, littleEndian);
      cout << "        Raw Hex Bytes  =";
      for (size_t i = 0; i < bytes.size(); i++)
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
         std::string   bytes;
         size_t        offset = 0;
         actual.encode(bytes, offset); //, littleEndian);
         cout << "        Value = " << actualValue << "  Raw Hex Bytes  =";
         for (size_t i = 0; i < bytes.size(); i++)
         {
            cout << " " << hex << (unsigned short)bytes[i];
         }
         cout << dec << endl;
      }
   }
}


//---------------------------------------------------------------------------
void BinexTypesTest::dumpBuffer(const unsigned char* buffer, size_t size)
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
   BinexTypesTest app(argv[0]);

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
