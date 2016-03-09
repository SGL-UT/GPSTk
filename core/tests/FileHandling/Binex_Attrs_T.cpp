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
class BinexAttrs_T
{
public:

      // constructor
   BinexAttrs_T() : verboseLevel(0)
   {
      init();
   };

      // destructor
   virtual ~BinexAttrs_T() {};

      // initialize tests
   void init();

      // test methods
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int doIsDataTests();
   int doRecordFlagsTests();
   int doRecordIdTests();
   int doMessageCapacityTests();
   int doMessageLengthTests();

   unsigned  verboseLevel;  // amount to display during tests, 0 = least

}; // class BinexAttrs_T


//============================================================
// Initialize Test Data Filenames and Values
//============================================================

void BinexAttrs_T :: init( void )
{
      // empty
}


int BinexAttrs_T :: doIsDataTests()
{
   TestUtil  tester( "BinexData", "isData", __FILE__, __LINE__ );

   BinexData  rec;
   tester.assert( rec.isData(), "BinexData should be 'data'", __LINE__ );

   return tester.countFails();
}


int BinexAttrs_T :: doRecordFlagsTests()
{
   TestUtil  tester( "BinexData", "recordflags", __FILE__, __LINE__ );

   BinexData  rec;
   tester.assert( (rec.getRecordFlags() == BinexData::DEFAULT_RECORD_FLAGS),
                  "default flags expected", __LINE__ );

   rec.setRecordFlags(0);
   tester.assert( (rec.getRecordFlags() == 0),
                  "no flags expected", __LINE__ );

   rec.setRecordFlags(0xFF);
   tester.assert( (rec.getRecordFlags() == BinexData::VALID_RECORD_FLAGS),
                  "valid flags expected", __LINE__ );

   return tester.countFails();
}


int BinexAttrs_T :: doRecordIdTests()
{
   TestUtil  tester( "BinexData", "recordId", __FILE__, __LINE__ );

   BinexData  recA;
   tester.assert( (recA.getRecordID() == BinexData::INVALID_RECORD_ID),
                  "invalid record id expected", __LINE__ );

   BinexData  recB(123);
   tester.assert( (recB.getRecordID() == 123),
                  "record id 123 expected", __LINE__ );

   recB.setRecordID(456);
   tester.assert( (recB.getRecordID() == 456),
                  "record id 456 expected", __LINE__ );

   return tester.countFails();
}


int BinexAttrs_T :: doMessageLengthTests()
{
   TestUtil  tester( "BinexData", "messageLength", __FILE__, __LINE__ );

   BinexData  rec;
   tester.assert( (rec.getMessageLength() == 0),
                  "expected length 0", __LINE__ );

   BinexData::UBNXI  u;
   size_t  offset = 0;
   rec.updateMessageData(offset, u);
   tester.assert( (rec.getMessageLength() > 0),
                  "expected positive length", __LINE__ );

   return tester.countFails();
}


int BinexAttrs_T :: doMessageCapacityTests()
{
   TestUtil  tester( "BinexData", "messageCapacity", __FILE__, __LINE__ );

   BinexData  rec;
   BinexData::UBNXI  u;
   size_t  offset = 0;
   rec.updateMessageData(offset, u);
   tester.assert( (rec.getMessageData().capacity() > 0),
                  "non-zero capacity expected", __LINE__ );

   rec.ensureMessageCapacity(1024);
   tester.assert( (rec.getMessageData().capacity() >= 1024),
                  "expected capacity of at least 1024", __LINE__ );

   rec.ensureMessageCapacity(2048);
   tester.assert( (rec.getMessageData().capacity() >= 2048),
                  "expected capacity of at least 2048", __LINE__ );

   return tester.countFails();
}

   /** Run the program.
    *
    * @return Total error count for all tests
    */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   BinexAttrs_T  testClass;  // test data is loaded here

   errorTotal += testClass.doIsDataTests();

   errorTotal += testClass.doRecordFlagsTests();

   errorTotal += testClass.doRecordIdTests();

   errorTotal += testClass.doMessageCapacityTests();

   errorTotal += testClass.doMessageLengthTests();

   return( errorTotal );

} // main()
