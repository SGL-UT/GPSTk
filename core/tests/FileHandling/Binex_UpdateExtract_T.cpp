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
class BinexUpdateExtract_T
{
public:

      // constructor
   BinexUpdateExtract_T() : verboseLevel(0)
   {
      init();
   };

      // destructor
   virtual ~BinexUpdateExtract_T() {};

      // initialize tests
   void init();

      // update and extract primitive types from a Binex record
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   template<class T>
   int doPrimitiveTests();

      // update and extract UBNXIs from a Binex record
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int doUbnxiTests();

      // update and extract MGFZIs types from a Binex record
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int doMgfziTests();

      // update and extract a mixture of types from a Binex record
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int doMixedTestA();

      // update and extract a mixture of types from a Binex record
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int doMixedTestB();

      // update and extract a mixture of types from a Binex record
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int doMixedTestC();


   unsigned  verboseLevel;  // amount to display during tests, 0 = least

private:

      // @param seed input for test value generation;
      //        a given seed will always yield the same test value
      //
      // @return test value that predictably spans the valid range
      //         of the specified type
   template<class T>
   T getSemiRandomValue(int seed);

      // return a string describing a value mismatch
   template<class T>
   string mismatchMsg(T actual, T expected);

}; // class BinexUpdateExtract_T


void BinexUpdateExtract_T :: init( void )
{
      // empty
}


template<>
char BinexUpdateExtract_T :: getSemiRandomValue<char>(int seed)
{
   char  val = ( (seed * 23) % 256) - 127;
   return val;
}


template<>
short BinexUpdateExtract_T :: getSemiRandomValue<short>(int seed)
{
   short  val = ( (seed * 189) % 0xFFFF) - 0xFFFE;
   return val;
}


template<>
long BinexUpdateExtract_T :: getSemiRandomValue<long>(int seed)
{
   long  val = ( (seed * 12377) % 0xFFFFFFFF) - 0xFFFFFFFE;
   return val;
}


template<>
BinexData::UBNXI BinexUpdateExtract_T :: getSemiRandomValue<BinexData::UBNXI>
(int seed)
{

   BinexData::UBNXI  val = ( abs(seed) * 12377ul) % 0xFFFFFFFFul;
   return val;
}


template<>
BinexData::MGFZI BinexUpdateExtract_T :: getSemiRandomValue<BinexData::MGFZI>
(int seed)
{

   BinexData::MGFZI  val = (seed * 123797ul) % BinexData::MGFZI::MAX_VALUE;
   return val;
}


template<>
string BinexUpdateExtract_T :: mismatchMsg<char>(char actual, char expected)
{
   ostringstream  oss;
   oss << "value mismatch: actual = " << (short)actual
       << " ; expected = " << (short)expected;
   return oss.str();
}

template<class T>
string BinexUpdateExtract_T :: mismatchMsg(T actual, T expected)
{
   ostringstream  oss;
   oss << "value mismatch: actual = " << actual << " ; expected = " << expected;
   return oss.str();
}


template<class T>
int BinexUpdateExtract_T :: doPrimitiveTests()
{
   const type_info&  typeInfo = typeid(T);
   std::string  testName("Update/Extract (");
   testName += typeInfo.name();
   testName += ")";
   TUDEF( "BinexData", testName );

      // test various record sizes: 1, 2, 5, 13, 34, 89, 233
   int  recStep = 0;
   for (int recSize = 1; recSize < 240; recSize += recStep)
   {
      std::vector<T>  data;     // save values to test against
      BinexData  record(recSize);  // use the size as the ID

      size_t  offset = 0;
      size_t  expectedOffset = 0;

         // add data to the record
      for (int i = 0; i < recSize; ++i)
      {
         try
         {
            T  value = getSemiRandomValue<T>(i);
            unsigned  prevLength = record.getMessageLength();
            expectedOffset = offset + sizeof(T);
            record.updateMessageData(offset, value, sizeof(T) );
            data.push_back(value);

            TUASSERTE(size_t, expectedOffset, offset);
            TUASSERTE(size_t, record.getMessageLength(), offset);
         }
         catch (Exception& e)
         {
            ostringstream  oss;
            oss << "exception updating record: " << e;
            TUFAIL( oss.str() );
         }
      }

      offset = 0;

      for (int i = 0; i < data.size(); ++i)
      {
         try   // verify data in the record
         {
            T  value;
            record.extractMessageData(offset, value, sizeof(T) );

            TUASSERTE(T, data[i], value);
         }
         catch (Exception& e)
         {
            ostringstream  oss;
            oss << "exception extracting from record: " << e;
            TUFAIL( oss.str() );
         }
      }

      recStep += recSize;
   }

   return testFramework.countFails();
}


int BinexUpdateExtract_T :: doUbnxiTests()
{
   TUDEF( "BinexData", "Update/Extract (UBNXI)" );

      // test various record sizes: 1, 2, 5, 13, 34, 89, 233
   int  recStep = 0;
   for (int recSize = 1; recSize < 240; recSize += recStep)
   {
      std::vector<BinexData::UBNXI>  data;  // save values to test against
      BinexData  record(recSize);  // use the size as the ID

      size_t  offset = 0;
      size_t  expectedOffset = 0;

         // add data to the record
      for (int i = 0; i < recSize; ++i)
      {
         try
         {
            BinexData::UBNXI  value = getSemiRandomValue<BinexData::UBNXI>(i);
            unsigned  prevLength = record.getMessageLength();
            expectedOffset = offset + value.getSize();
            record.updateMessageData(offset, value);
            data.push_back(value);

            testFramework.assert( (  (offset == expectedOffset)
                              && (offset == record.getMessageLength()) ),
                           "incorrect offset", __LINE__ );
         }
         catch (Exception& e)
         {
            ostringstream  oss;
            oss << "exception updating record: " << e;
            testFramework.assert( false, oss.str(), __LINE__ );
         }
      }

      offset = 0;

      for (int i = 0; i < data.size(); ++i)
      {
         try   // verify data in the record
         {
            BinexData::UBNXI  value;
            record.extractMessageData(offset, value);

            testFramework.assert( (value == data[i]), mismatchMsg(value, data[i]), __LINE__ );
         }
         catch (Exception& e)
         {
            ostringstream  oss;
            oss << "exception extracting from record: " << e;
            testFramework.assert( false, oss.str(), __LINE__ );
         }
      }

      recStep += recSize;
   }

   return testFramework.countFails();
}


int BinexUpdateExtract_T :: doMgfziTests()
{
   TUDEF( "BinexData", "Update/Extract (MGFZI)" );

      // test various record sizes: 1, 2, 5, 13, 34, 89, 233
   int  recStep = 0;
   for (int recSize = 1; recSize < 240; recSize += recStep)
   {
      std::vector<BinexData::MGFZI>  data;  // save values to test against
      BinexData  record(recSize);  // use the size as the ID

      size_t  offset = 0;
      size_t  expectedOffset = 0;

         // add data to the record
      for (int i = 0; i < recSize; ++i)
      {
         try
         {
            BinexData::MGFZI  value = getSemiRandomValue<BinexData::MGFZI>(i);
            unsigned  prevLength = record.getMessageLength();
            expectedOffset = offset + value.getSize();
            record.updateMessageData(offset, value);
            data.push_back(value);

            testFramework.assert ( (  (offset == expectedOffset)
                               && (offset == record.getMessageLength()) ),
                            "incorrect offset", __LINE__ );
         }
         catch (Exception& e)
         {
            ostringstream  oss;
            oss << "exception updating record: " << e;
            testFramework.assert( false, oss.str(), __LINE__ );
         }
      }

      offset = 0;

      for (int i = 0; i < data.size(); ++i)
      {
         try   // verify data in the record
         {
            BinexData::MGFZI  value;
            record.extractMessageData(offset, value);

            testFramework.assert( (value == data[i]), mismatchMsg(value, data[i]), __LINE__ );
         }
         catch (Exception& e)
         {
            ostringstream  oss;
            oss << "exception extracting from record: " << e;
            testFramework.assert( false, oss.str(), __LINE__ );
         }
      }

      recStep += recSize;
   }

   return testFramework.countFails();
}


int BinexUpdateExtract_T :: doMixedTestA()
{
   TUDEF( "BinexData", "Update/Extract (Mixed A)" );

      // A Data = s U U M c l l s c c M M U l s

   BinexData  record(1);  // id = 1

   size_t  offset = 0;


      // add a mixture of data to the record . . .

   short  s1(2803);
   record.updateMessageData(offset, s1, sizeof(short));

   BinexData::UBNXI  u1(61036);
   record.updateMessageData(offset, u1);

   BinexData::UBNXI  u2(7);
   record.updateMessageData(offset, u2);

   BinexData::MGFZI  m1(3579);
   record.updateMessageData(offset, m1);

   char  c1(75);
   record.updateMessageData(offset, c1, sizeof(char));

   long  l1(-38926);
   record.updateMessageData(offset, l1, sizeof(long));

   long  l2(8830017);
   record.updateMessageData(offset, l2, sizeof(long));

   short  s2(9);
   record.updateMessageData(offset, s2, sizeof(short));

   char  c2(125);
   record.updateMessageData(offset, c2, sizeof(char));

   char  c3(4);
   record.updateMessageData(offset, c3, sizeof(char));

   BinexData::MGFZI  m2(-101918);
   record.updateMessageData(offset, m2);

   BinexData::MGFZI  m3(92765438);
   record.updateMessageData(offset, m3);

   BinexData::UBNXI  u3(4500);
   record.updateMessageData(offset, u3);

   long  l3(-1);
   record.updateMessageData(offset, l3, sizeof(long));

   short  s3(-15490);
   record.updateMessageData(offset, s3, sizeof(short));

      // verify data in the record . . .

   offset = 0;

   short  vs1;
   record.extractMessageData(offset, vs1, sizeof(short));
   testFramework.assert( (vs1 == s1), mismatchMsg(vs1, s1), __LINE__ );

   BinexData::UBNXI  vu1;
   record.extractMessageData(offset, vu1);
   testFramework.assert( (vu1 == u1), mismatchMsg(vu1, u1), __LINE__ );

   BinexData::UBNXI  vu2;
   record.extractMessageData(offset, vu2);
   testFramework.assert( (vu2 == u2), mismatchMsg(vu2, u2), __LINE__ );

   BinexData::MGFZI  vm1;
   record.extractMessageData(offset, vm1);
   testFramework.assert( (vm1 == m1), mismatchMsg(vm1, m1), __LINE__ );

   char  vc1;
   record.extractMessageData(offset, vc1, sizeof(char));
   testFramework.assert( (vc1 == c1), mismatchMsg(vc1, c1), __LINE__ );

   long  vl1;
   record.extractMessageData(offset, vl1, sizeof(long));
   testFramework.assert( (vl1 == l1), mismatchMsg(vl1, l1), __LINE__ );

   long  vl2;
   record.extractMessageData(offset, vl2, sizeof(long));
   testFramework.assert( (vl2 == l2), mismatchMsg(vl2, l2), __LINE__ );

   short  vs2;
   record.extractMessageData(offset, vs2, sizeof(short));
   testFramework.assert( (vs2 == s2), mismatchMsg(vs2, s2), __LINE__ );

   char  vc2;
   record.extractMessageData(offset, vc2, sizeof(char));
   testFramework.assert( (vc2 == c2), mismatchMsg(vc2, c2), __LINE__ );

   char  vc3;
   record.extractMessageData(offset, vc3, sizeof(char));
   testFramework.assert( (vc3 == c3), mismatchMsg(vc3, c3), __LINE__ );

   BinexData::MGFZI  vm2;
   record.extractMessageData(offset, vm2);
   testFramework.assert( (vm2 == m2), mismatchMsg(vm2, m2), __LINE__ );

   BinexData::MGFZI  vm3;
   record.extractMessageData(offset, vm3);
   testFramework.assert( (vm3 == m3), mismatchMsg(vm3, m3), __LINE__ );

   BinexData::UBNXI  vu3;
   record.extractMessageData(offset, vu3);
   testFramework.assert( (vu3 == u3), mismatchMsg(vu3, u3), __LINE__ );

   long  vl3;
   record.extractMessageData(offset, vl3, sizeof(long));
   testFramework.assert( (vl3 == l3), mismatchMsg(vl3, l3), __LINE__ );

   short  vs3;
   record.extractMessageData(offset, vs3, sizeof(short));
   testFramework.assert( (vs3 == s3), mismatchMsg(vs3, s3), __LINE__ );

   return testFramework.countFails();
}


int BinexUpdateExtract_T :: doMixedTestB()
{
   TUDEF( "BinexData", "Update/Extract (Mixed B)" );

      // B Data = M l s s U c U l M s c l M U c

   BinexData  record(1);  // id = 1

   size_t  offset = 0;


      // add a mixture of data to the record . . .

   BinexData::MGFZI  m1(3579);
   record.updateMessageData(offset, m1);

   long  l1(-38926);
   record.updateMessageData(offset, l1, sizeof(long));

   short  s1(2803);
   record.updateMessageData(offset, s1, sizeof(short));

   short  s2(9);
   record.updateMessageData(offset, s2, sizeof(short));

   BinexData::UBNXI  u1(61036);
   record.updateMessageData(offset, u1);

   char  c1(75);
   record.updateMessageData(offset, c1, sizeof(char));

   BinexData::UBNXI  u2(7);
   record.updateMessageData(offset, u2);

   long  l2(8830017);
   record.updateMessageData(offset, l2, sizeof(long));

   BinexData::MGFZI  m2(-101918);
   record.updateMessageData(offset, m2);

   short  s3(-15490);
   record.updateMessageData(offset, s3, sizeof(short));

   char  c2(125);
   record.updateMessageData(offset, c2, sizeof(char));

   long  l3(-1);
   record.updateMessageData(offset, l3, sizeof(long));

   BinexData::MGFZI  m3(92765438);
   record.updateMessageData(offset, m3);

   BinexData::UBNXI  u3(4500);
   record.updateMessageData(offset, u3);

   char  c3(4);
   record.updateMessageData(offset, c3, sizeof(char));

      // verify data in the record . . .

   offset = 0;

   BinexData::MGFZI  vm1;
   record.extractMessageData(offset, vm1);
   testFramework.assert( (vm1 == m1), mismatchMsg(vm1, m1), __LINE__ );

   long  vl1;
   record.extractMessageData(offset, vl1, sizeof(long));
   testFramework.assert( (vl1 == l1), mismatchMsg(vl1, l1), __LINE__ );

   short  vs1;
   record.extractMessageData(offset, vs1, sizeof(short));
   testFramework.assert( (vs1 == s1), mismatchMsg(vs1, s1), __LINE__ );

   short  vs2;
   record.extractMessageData(offset, vs2, sizeof(short));
   testFramework.assert( (vs2 == s2), mismatchMsg(vs2, s2), __LINE__ );

   BinexData::UBNXI  vu1;
   record.extractMessageData(offset, vu1);
   testFramework.assert( (vu1 == u1), mismatchMsg(vu1, u1), __LINE__ );

   char  vc1;
   record.extractMessageData(offset, vc1, sizeof(char));
   testFramework.assert( (vc1 == c1), mismatchMsg(vc1, c1), __LINE__ );

   BinexData::UBNXI  vu2;
   record.extractMessageData(offset, vu2);
   testFramework.assert( (vu2 == u2), mismatchMsg(vu2, u2), __LINE__ );

   long  vl2;
   record.extractMessageData(offset, vl2, sizeof(long));
   testFramework.assert( (vl2 == l2), mismatchMsg(vl2, l2), __LINE__ );

   BinexData::MGFZI  vm2;
   record.extractMessageData(offset, vm2);
   testFramework.assert( (vm2 == m2), mismatchMsg(vm2, m2), __LINE__ );

   short  vs3;
   record.extractMessageData(offset, vs3, sizeof(short));
   testFramework.assert( (vs3 == s3), mismatchMsg(vs3, s3), __LINE__ );

   char  vc2;
   record.extractMessageData(offset, vc2, sizeof(char));
   testFramework.assert( (vc2 == c2), mismatchMsg(vc2, c2), __LINE__ );

   long  vl3;
   record.extractMessageData(offset, vl3, sizeof(long));
   testFramework.assert( (vl3 == l3), mismatchMsg(vl3, l3), __LINE__ );

   BinexData::MGFZI  vm3;
   record.extractMessageData(offset, vm3);
   testFramework.assert( (vm3 == m3), mismatchMsg(vm3, m3), __LINE__ );

   BinexData::UBNXI  vu3;
   record.extractMessageData(offset, vu3);
   testFramework.assert( (vu3 == u3), mismatchMsg(vu3, u3), __LINE__ );

   char  vc3;
   record.extractMessageData(offset, vc3, sizeof(char));
   testFramework.assert( (vc3 == c3), mismatchMsg(vc3, c3), __LINE__ );

   return testFramework.countFails();
}


int BinexUpdateExtract_T :: doMixedTestC()
{
   TUDEF( "BinexData", "Update/Extract (Mixed C)" );

      // C Data = c M U l s s s M c l U U l c M

   BinexData  record(1);  // id = 1

   size_t  offset = 0;

      // add a mixture of data to the record . . .

   char  c1(75);
   record.updateMessageData(offset, c1, sizeof(char));

   BinexData::MGFZI  m1(3579);
   record.updateMessageData(offset, m1);

   BinexData::UBNXI  u1(61036);
   record.updateMessageData(offset, u1);

   long  l1(-38926);
   record.updateMessageData(offset, l1, sizeof(long));

   short  s1(2803);
   record.updateMessageData(offset, s1, sizeof(short));

   short  s2(9);
   record.updateMessageData(offset, s2, sizeof(short));

   short  s3(-15490);
   record.updateMessageData(offset, s3, sizeof(short));

   BinexData::MGFZI  m2(-101918);
   record.updateMessageData(offset, m2);

   char  c2(125);
   record.updateMessageData(offset, c2, sizeof(char));

   long  l2(8830017);
   record.updateMessageData(offset, l2, sizeof(long));

   BinexData::UBNXI  u2(7);
   record.updateMessageData(offset, u2);

   BinexData::UBNXI  u3(4500);
   record.updateMessageData(offset, u3);

   long  l3(-1);
   record.updateMessageData(offset, l3, sizeof(long));

   char  c3(4);
   record.updateMessageData(offset, c3, sizeof(char));

   BinexData::MGFZI  m3(92765438);
   record.updateMessageData(offset, m3);

      // verify data in the record . . .

   offset = 0;

   char  vc1;
   record.extractMessageData(offset, vc1, sizeof(char));
   testFramework.assert( (vc1 == c1), mismatchMsg(vc1, c1), __LINE__ );

   BinexData::MGFZI  vm1;
   record.extractMessageData(offset, vm1);
   testFramework.assert( (vm1 == m1), mismatchMsg(vm1, m1), __LINE__ );

   BinexData::UBNXI  vu1;
   record.extractMessageData(offset, vu1);
   testFramework.assert( (vu1 == u1), mismatchMsg(vu1, u1), __LINE__ );

   long  vl1;
   record.extractMessageData(offset, vl1, sizeof(long));
   testFramework.assert( (vl1 == l1), mismatchMsg(vl1, l1), __LINE__ );

   short  vs1;
   record.extractMessageData(offset, vs1, sizeof(short));
   testFramework.assert( (vs1 == s1), mismatchMsg(vs1, s1), __LINE__ );

   short  vs2;
   record.extractMessageData(offset, vs2, sizeof(short));
   testFramework.assert( (vs2 == s2), mismatchMsg(vs2, s2), __LINE__ );

   short  vs3;
   record.extractMessageData(offset, vs3, sizeof(short));
   testFramework.assert( (vs3 == s3), mismatchMsg(vs3, s3), __LINE__ );

   BinexData::MGFZI  vm2;
   record.extractMessageData(offset, vm2);
   testFramework.assert( (vm2 == m2), mismatchMsg(vm2, m2), __LINE__ );

   char  vc2;
   record.extractMessageData(offset, vc2, sizeof(char));
   testFramework.assert( (vc2 == c2), mismatchMsg(vc2, c2), __LINE__ );

   long  vl2;
   record.extractMessageData(offset, vl2, sizeof(long));
   testFramework.assert( (vl2 == l2), mismatchMsg(vl2, l2), __LINE__ );

   BinexData::UBNXI  vu2;
   record.extractMessageData(offset, vu2);
   testFramework.assert( (vu2 == u2), mismatchMsg(vu2, u2), __LINE__ );

   BinexData::UBNXI  vu3;
   record.extractMessageData(offset, vu3);
   testFramework.assert( (vu3 == u3), mismatchMsg(vu3, u3), __LINE__ );

   long  vl3;
   record.extractMessageData(offset, vl3, sizeof(long));
   testFramework.assert( (vl3 == l3), mismatchMsg(vl3, l3), __LINE__ );

   char  vc3;
   record.extractMessageData(offset, vc3, sizeof(char));
   testFramework.assert( (vc3 == c3), mismatchMsg(vc3, c3), __LINE__ );

   BinexData::MGFZI  vm3;
   record.extractMessageData(offset, vm3);
   testFramework.assert( (vm3 == m3), mismatchMsg(vm3, m3), __LINE__ );

   return testFramework.countFails();
}


   /** Run the program.
    *
    * @return Total error count for all tests
    */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   BinexUpdateExtract_T  testClass;  // test data is loaded here

   errorTotal += testClass.doPrimitiveTests<char>();

   errorTotal += testClass.doPrimitiveTests<short>();

   errorTotal += testClass.doPrimitiveTests<long>();

   errorTotal += testClass.doUbnxiTests();

   errorTotal += testClass.doMgfziTests();

   errorTotal += testClass.doMixedTestA();

   errorTotal += testClass.doMixedTestB();

   errorTotal += testClass.doMixedTestC();

   return( errorTotal );

} // main()
