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
   TUDEF("BinexData", "isData");

   BinexData  rec;
   TUASSERT(rec.isData());

   TURETURN();
}


int BinexAttrs_T :: doRecordFlagsTests()
{
   TUDEF("BinexData", "getRecordFlags");

   BinexData  rec;
   TUASSERTE(BinexData::SyncByte, BinexData::DEFAULT_RECORD_FLAGS, rec.getRecordFlags());

   TUCSM("setRecordFlags");
   rec.setRecordFlags(0);
   TUASSERTE(BinexData::SyncByte, 0, rec.getRecordFlags());

   rec.setRecordFlags(0xFF);
   TUASSERTE(BinexData::SyncByte, BinexData::VALID_RECORD_FLAGS, rec.getRecordFlags());

   TURETURN();
}


int BinexAttrs_T :: doRecordIdTests()
{
   TUDEF("BinexData", "getRecordID");

   BinexData  recA;
   TUASSERTE(BinexData::RecordID, BinexData::INVALID_RECORD_ID, recA.getRecordID());

   BinexData  recB(123);
   TUASSERTE(BinexData::RecordID, 123, recB.getRecordID());

   TUCSM("setRecordID");
   recB.setRecordID(456);
   TUASSERTE(BinexData::RecordID, 456, recB.getRecordID());

   TURETURN();
}


int BinexAttrs_T :: doMessageLengthTests()
{
   TUDEF("BinexData", "getMessageLength");

   BinexData  rec(1);  // a record id is required
   TUASSERTE(size_t, 0, rec.getMessageLength());
   TUCSM("getHeadLength");
   TUASSERTE(size_t, 3, rec.getHeadLength());
   TUCSM("getTailLength");
   TUASSERTE(size_t, 1, rec.getTailLength());
   TUCSM("getRecordSize");
   TUASSERTE(size_t, 4, rec.getRecordSize());

   std::string  s("1");
   size_t  offset = 0;
   rec.updateMessageData(offset, s, s.size());
   TUCSM("getMessageLength");
   TUASSERTE(size_t, 1, rec.getMessageLength());
   TUCSM("getHeadLength");
   TUASSERTE(size_t, 3, rec.getHeadLength());
   TUCSM("getTailLength");
   TUASSERTE(size_t, 1, rec.getTailLength());
   TUCSM("getRecordSize");
   TUASSERTE(size_t, 5, rec.getRecordSize());

   s.assign(199, '2');
   rec.updateMessageData(offset, s, s.size());
   TUCSM("getMessageLength");
   TUASSERTE(size_t, 200, rec.getMessageLength());
   TUCSM("getHeadLength");
   TUASSERTE(size_t, 4, rec.getHeadLength());
   TUCSM("getTailLength");
   TUASSERTE(size_t, 2, rec.getTailLength());
   TUCSM("getRecordSize");
   TUASSERTE(size_t, 206, rec.getRecordSize());

   s.assign(17000, '3');
   rec.updateMessageData(offset, s, s.size());
   TUCSM("getMessageLength");
   TUASSERTE(size_t, 17200, rec.getMessageLength());
   TUCSM("getHeadLength");
   TUASSERTE(size_t, 5, rec.getHeadLength());
   TUCSM("getTailLength");
   TUASSERTE(size_t, 4, rec.getTailLength());
   TUCSM("getRecordSize");
   TUASSERTE(size_t, 17209, rec.getRecordSize());

   s.assign(2100800, '4');
   rec.updateMessageData(offset, s, s.size());
   TUCSM("getMessageLength");
   TUASSERTE(size_t, 2118000, rec.getMessageLength());
   TUCSM("getHeadLength");
   TUASSERTE(size_t, 6, rec.getHeadLength());
   TUCSM("getTailLength");
   TUASSERTE(size_t, 16, rec.getTailLength());
   TUCSM("getRecordSize");
   TUASSERTE(size_t, 2118022, rec.getRecordSize());

   TURETURN();
}


int BinexAttrs_T :: doMessageCapacityTests()
{
   TUDEF("BinexData", "getMessageCapacity");

   BinexData  rec;
   BinexData::UBNXI  u;
   size_t  offset = 0;
   rec.updateMessageData(offset, u);
   TUASSERT(rec.getMessageData().capacity() >= 1);

   TUCSM("ensureMessageCapacity");
   rec.ensureMessageCapacity(1024);
   TUASSERT(rec.getMessageData().capacity() >= 1024);
   rec.ensureMessageCapacity(2048);
   TUASSERT(rec.getMessageData().capacity() >= 2048);

   TURETURN();
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
