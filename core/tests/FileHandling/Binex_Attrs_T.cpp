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
   BinexAttrs_T() : verboseLevel(0) { init(); };

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
   tester.assert(rec.isData() == true);

   return tester.countFails();
}


int BinexAttrs_T :: doRecordFlagsTests()
{
   TestUtil  tester( "BinexData", "recordflags", __FILE__, __LINE__ );

   BinexData  rec;
   tester.assert(rec.getRecordFlags() == BinexData::DEFAULT_RECORD_FLAGS);
   tester.next();

   rec.setRecordFlags(0);
   tester.assert(rec.getRecordFlags() == 0);
   tester.next();

   rec.setRecordFlags(0xFF);
   tester.assert(rec.getRecordFlags() == BinexData::VALID_RECORD_FLAGS);

   return tester.countFails();
}


int BinexAttrs_T :: doRecordIdTests()
{
   TestUtil  tester( "BinexData", "recordId", __FILE__, __LINE__ );

   BinexData  recA;
   tester.assert(recA.getRecordID() == BinexData::INVALID_RECORD_ID);
   tester.next();

   BinexData  recB(123);
   tester.assert(recB.getRecordID() == 123);
   tester.next();

   recB.setRecordID(456);
   tester.assert(recB.getRecordID() == 456);

   return tester.countFails();
}


int BinexAttrs_T :: doMessageCapacityTests()
{
   TestUtil  tester( "BinexData", "messageCapacity", __FILE__, __LINE__ );

   BinexData  rec;
   tester.assert(rec.getMessageCapacity() == 0);
   tester.next();

   BinexData::UBNXI  u;
   size_t  offset = 0;
   rec.updateMessageData(offset, u);
   tester.assert(rec.getMessageCapacity() > 0);
   tester.next();

   rec.ensureMessageCapacity(1024);
   tester.assert(rec.getMessageCapacity() == 1024);

   return tester.countFails();
}


int BinexAttrs_T :: doMessageLengthTests()
{
   TestUtil  tester( "BinexData", "messageLength", __FILE__, __LINE__ );

   BinexData  rec;
   tester.assert(rec.getMessageLength() == 0);
   tester.next();

   BinexData::UBNXI  u;
   size_t  offset = 0;
   rec.updateMessageData(offset, u);
   tester.assert(rec.getMessageLength() > 0);

   return tester.countFails();
}


/** Run the program.
 *
 * @return Total error count for all tests
 */
main(int argc, char *argv[])
{
   int  errorCount = 0;
   int  errorTotal = 0;

   BinexAttrs_T  testClass;  // test data is loaded here

   errorCount = testClass.doIsDataTests();
   errorTotal = errorTotal + errorCount;

   errorCount = testClass.doRecordFlagsTests();
   errorTotal = errorTotal + errorCount;

   errorCount = testClass.doRecordIdTests();
   errorTotal = errorTotal + errorCount;

   errorCount = testClass.doMessageCapacityTests();
   errorTotal = errorTotal + errorCount;

   errorCount = testClass.doMessageLengthTests();
   errorTotal = errorTotal + errorCount;

   return( errorTotal );
   
} // main()
