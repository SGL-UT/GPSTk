#pragma ident "$Id: xReferenceFrame.cpp 2011-06-24 14:11:10 nwu $"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "xReferenceFrame.hpp"

using namespace std;
using namespace gpstk;

CPPUNIT_TEST_SUITE_REGISTRATION (xReferenceFrame);

void xReferenceFrame::getFrameTest()
{
   ReferenceFrame rf1(0);
   ReferenceFrame::FramesEnum frame1 = (ReferenceFrame::FramesEnum)0;
   CPPUNIT_ASSERT(rf1.getFrame() == frame1);

   ReferenceFrame rf2(1);
   ReferenceFrame::FramesEnum frame2 = (ReferenceFrame::FramesEnum)1;
   CPPUNIT_ASSERT(rf2.getFrame() == frame2);

   ReferenceFrame rf3(2);
   ReferenceFrame::FramesEnum frame3 = (ReferenceFrame::FramesEnum)2;
   CPPUNIT_ASSERT(rf3.getFrame() == frame3);

      //Fails to ReferenceFrame::Unknown
   ReferenceFrame rf4(-1);
   ReferenceFrame::FramesEnum frame4 = (ReferenceFrame::FramesEnum)3;
   CPPUNIT_ASSERT(rf4.getFrame() != frame4);
   CPPUNIT_ASSERT(rf4.getFrame() == frame1);
}
void xReferenceFrame::asStringTest()
{
   string pz("PZ90");
   string wgs("WGS84");
   string unk("Unknown");

   ReferenceFrame rf1(ReferenceFrame::PZ90);
   CPPUNIT_ASSERT(rf1.asString() == pz);

   ReferenceFrame rf2(ReferenceFrame::WGS84);
   CPPUNIT_ASSERT(rf2.asString() == wgs);

   ReferenceFrame rf3(ReferenceFrame::Unknown);
   CPPUNIT_ASSERT(rf3.asString() == unk);
}
void xReferenceFrame::equalityTest()
{
   ReferenceFrame rf1(ReferenceFrame::PZ90);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::Unknown);
   ReferenceFrame rf4("PZ90");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("Junk String That Will Never Match Up");

      //PZ90 Enum with...
   CPPUNIT_ASSERT(rf1 == rf1);
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 == rf2 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 == rf3 ));
   CPPUNIT_ASSERT(rf1 == rf4);
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 == rf5 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 == rf6 ));

      //WGS84 Enum with...
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 == rf1 ));
   CPPUNIT_ASSERT( rf2 == rf2);
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 == rf3 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 == rf4 ));
   CPPUNIT_ASSERT( rf2 == rf5);
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 == rf6 ));

      //Unknown Enum with... (Should fail every one)
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 == rf1 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 == rf2 ));
   CPPUNIT_ASSERT( rf3 == rf3 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 == rf4 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 == rf5 ));
   CPPUNIT_ASSERT( rf3 == rf6 );
}
void xReferenceFrame::inequalityTest()
{
   ReferenceFrame rf1(ReferenceFrame::PZ90);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::Unknown);
   ReferenceFrame rf4("PZ90");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("Junk String That Will Never Match Up");

      //PZ90 Enum with...
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 != rf1 ));
   CPPUNIT_ASSERT( rf1 != rf2 );
   CPPUNIT_ASSERT( rf1 != rf3 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 != rf4 ));
   CPPUNIT_ASSERT( rf1 != rf5 );
   CPPUNIT_ASSERT( rf1 != rf6 );

      //WGS84 Enum with...
   CPPUNIT_ASSERT( rf2 != rf1 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 != rf2 ));
   CPPUNIT_ASSERT( rf2 != rf3 );
   CPPUNIT_ASSERT( rf2 != rf4 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 != rf5 ));
   CPPUNIT_ASSERT( rf2 != rf6 );

      //Unknown Enum with...
   CPPUNIT_ASSERT( rf3 != rf1 );
   CPPUNIT_ASSERT( rf3 != rf2 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 != rf3 ));
   CPPUNIT_ASSERT( rf3 != rf4 );
   CPPUNIT_ASSERT( rf3 != rf5 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 != rf6 ));
}
void xReferenceFrame::greaterThanTest()
{
   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);
   ReferenceFrame rf4("Junk String That Will Never Match Up");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("PZ90");

   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 > rf1 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 > rf2 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 > rf3 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 > rf4 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 > rf5 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 > rf6 ));

   CPPUNIT_ASSERT( rf2 > rf1 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 > rf2 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 > rf3 ));
   CPPUNIT_ASSERT( rf2 > rf4 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 > rf5 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 > rf6 ));

   CPPUNIT_ASSERT( rf3 > rf1 );
   CPPUNIT_ASSERT( rf3 > rf2 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 > rf3 ));
   CPPUNIT_ASSERT( rf3 > rf4 );
   CPPUNIT_ASSERT( rf3 > rf5 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 > rf6 ));
}
void xReferenceFrame::lessThanTest()
{
   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);
   ReferenceFrame rf4("Junk String That Will Never Match Up");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("PZ90");

   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 < rf1 ));
   CPPUNIT_ASSERT( rf1 < rf2 );
   CPPUNIT_ASSERT( rf1 < rf3 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf1 < rf4 ));
   CPPUNIT_ASSERT( rf1 < rf5 );
   CPPUNIT_ASSERT( rf1 < rf6 );

   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 < rf1 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 < rf2 ));
   CPPUNIT_ASSERT( rf2 < rf3 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 < rf4 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 < rf5 ));
   CPPUNIT_ASSERT( rf2 < rf6 );

   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 < rf1 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 < rf2 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 < rf3 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 < rf4 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 < rf5 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 < rf6 ));
}
void xReferenceFrame::greaterThanOrEqualToTest()
{
   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);
   ReferenceFrame rf4("Junk String That Will Never Match Up");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("PZ90");

      //Unknown with...
   CPPUNIT_ASSERT( rf1 >= rf1 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT(( rf1 >= rf2 )));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT(( rf1 >= rf3 )));
   CPPUNIT_ASSERT( rf1 >= rf4 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT(( rf1 >= rf5 )));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT(( rf1 >= rf6 )));

   CPPUNIT_ASSERT( rf2 >= rf1 );
   CPPUNIT_ASSERT( rf2 >= rf2 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT(( rf2 >= rf3 )));
   CPPUNIT_ASSERT( rf2 >= rf4 );
   CPPUNIT_ASSERT( rf2 >= rf5 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT(( rf2 >= rf6 )));

   CPPUNIT_ASSERT( rf3 >= rf1 );
   CPPUNIT_ASSERT( rf3 >= rf2 );
   CPPUNIT_ASSERT( rf3 >= rf3 );
   CPPUNIT_ASSERT( rf3 >= rf4 );
   CPPUNIT_ASSERT( rf3 >= rf5 );
   CPPUNIT_ASSERT( rf3 >= rf6 );
}
void xReferenceFrame::lesserThanOrEqualToTest()
{
   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);
   ReferenceFrame rf4("Junk String That Will Never Match Up");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("PZ90");

   CPPUNIT_ASSERT( rf1 <= rf1 );
   CPPUNIT_ASSERT( rf1 <= rf2 );
   CPPUNIT_ASSERT( rf1 <= rf3 );
   CPPUNIT_ASSERT( rf1 <= rf4 );
   CPPUNIT_ASSERT( rf1 <= rf5 );
   CPPUNIT_ASSERT( rf1 <= rf6 );

   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 <= rf1 ));
   CPPUNIT_ASSERT( rf2 <= rf2 );
   CPPUNIT_ASSERT( rf2 <= rf3 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf2 <= rf4 ));
   CPPUNIT_ASSERT( rf2 <= rf5 );
   CPPUNIT_ASSERT( rf2 <= rf6 );

   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 <= rf1 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 <= rf2 ));
   CPPUNIT_ASSERT( rf3 <= rf3 );
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 <= rf4 ));
   CPPUNIT_ASSERT_ASSERTION_FAIL(CPPUNIT_ASSERT( rf3 <= rf5 ));
   CPPUNIT_ASSERT( rf3 <= rf6 );
}
void xReferenceFrame::setReferenceFrameTest()
{
   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);

   ReferenceFrame frame(ReferenceFrame::Unknown);
   CPPUNIT_ASSERT(frame == rf1);
   CPPUNIT_ASSERT(frame != rf2);
   CPPUNIT_ASSERT(frame != rf3);

   frame = ReferenceFrame(0);
   CPPUNIT_ASSERT(frame == rf1);
   CPPUNIT_ASSERT(frame != rf2);
   CPPUNIT_ASSERT(frame != rf3);

   frame = ReferenceFrame("Unknown");
   CPPUNIT_ASSERT(frame == rf1);
   CPPUNIT_ASSERT(frame != rf2);
   CPPUNIT_ASSERT(frame != rf3);

   frame = ReferenceFrame("A Junk String that won't match up");
   CPPUNIT_ASSERT(frame == rf1);
   CPPUNIT_ASSERT(frame != rf2);
   CPPUNIT_ASSERT(frame != rf3);

   frame = ReferenceFrame(-1);
   CPPUNIT_ASSERT(frame == rf1);
   CPPUNIT_ASSERT(frame != rf2);
   CPPUNIT_ASSERT(frame != rf3);

   frame = ReferenceFrame((ReferenceFrame::FramesEnum)-1);
   CPPUNIT_ASSERT(frame == rf1);
   CPPUNIT_ASSERT(frame != rf2);
   CPPUNIT_ASSERT(frame != rf3);

   frame = ReferenceFrame(ReferenceFrame::WGS84);
   CPPUNIT_ASSERT(frame != rf1);
   CPPUNIT_ASSERT(frame == rf2);
   CPPUNIT_ASSERT(frame != rf3);

   frame = ReferenceFrame("WGS84");
   CPPUNIT_ASSERT(frame != rf1);
   CPPUNIT_ASSERT(frame == rf2);
   CPPUNIT_ASSERT(frame != rf3);

   frame = ReferenceFrame(1);
   CPPUNIT_ASSERT(frame != rf1);
   CPPUNIT_ASSERT(frame == rf2);
   CPPUNIT_ASSERT(frame != rf3);

   frame = ReferenceFrame(ReferenceFrame::PZ90);
   CPPUNIT_ASSERT(frame != rf1);
   CPPUNIT_ASSERT(frame != rf2);
   CPPUNIT_ASSERT(frame == rf3);

   frame = ReferenceFrame("PZ90");
   CPPUNIT_ASSERT(frame != rf1);
   CPPUNIT_ASSERT(frame != rf2);
   CPPUNIT_ASSERT(frame == rf3);

   frame = ReferenceFrame(2);
   CPPUNIT_ASSERT(frame != rf1);
   CPPUNIT_ASSERT(frame != rf2);
   CPPUNIT_ASSERT(frame == rf3);
}
void xReferenceFrame::createReferenceFrameTest()
{
   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);
   ReferenceFrame rf4("Junk String That Will Never Match Up");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("PZ90");

   ReferenceFrame test;
   test = test.createReferenceFrame("PZ90");
      //We want it to assert that test is the same as rf6 and rf3
   CPPUNIT_ASSERT(test == rf3);
   CPPUNIT_ASSERT(test == rf6);
   CPPUNIT_ASSERT(test != rf1);

      //Made up string
   test = test.createReferenceFrame("ZP84");
   CPPUNIT_ASSERT(test != rf1);
   CPPUNIT_ASSERT(test != rf2);
   CPPUNIT_ASSERT(test != rf3);
   CPPUNIT_ASSERT(test != rf4);
   CPPUNIT_ASSERT(test != rf5);
   CPPUNIT_ASSERT(test != rf6);

   //Make a new frame of type zp84
   ReferenceFrame test1("ZP84");
   CPPUNIT_ASSERT(test == test1);
   CPPUNIT_ASSERT(test1 != rf1);
}
