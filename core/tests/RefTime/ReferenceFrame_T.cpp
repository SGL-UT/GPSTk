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


#include "ReferenceFrame.hpp"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

class ReferenceFrame_T
{
   public:
   ReferenceFrame_T(){}
   ~ReferenceFrame_T(){}

int getFrameTest(void)
{
   TestUtil testFramework("ReferenceFrame", "getReferenceFrame", __FILE__, __LINE__);
   std::string testMesg;

   testMesg = "Get ReferenceFrame function failed";
   ReferenceFrame rf1((ReferenceFrame::Frames)0);
   ReferenceFrame::Frames frame1 = (ReferenceFrame::Frames)0;
   testFramework.assert(rf1.getReferenceFrame() == frame1, testMesg, __LINE__);

   ReferenceFrame rf2((ReferenceFrame::Frames)1);
   ReferenceFrame::Frames frame2 = (ReferenceFrame::Frames)1;
   testFramework.assert(rf2.getReferenceFrame() == frame2, testMesg, __LINE__);

   ReferenceFrame rf3((ReferenceFrame::Frames)2);
   ReferenceFrame::Frames frame3 = (ReferenceFrame::Frames)2;
   testFramework.assert(rf3.getReferenceFrame() == frame3, testMesg, __LINE__);

      //Fails to ReferenceFrame::Unknown
   ReferenceFrame rf4((ReferenceFrame::Frames)-1);
   ReferenceFrame::Frames frame4 = (ReferenceFrame::Frames)3;
   testFramework.assert(rf4.getReferenceFrame() != frame4, testMesg, __LINE__);
   testFramework.assert(rf4.getReferenceFrame() == frame1, testMesg, __LINE__);

   return testFramework.countFails();
}
int asStringTest(void)
{
   TestUtil testFramework("ReferenceFrame", "asString", __FILE__, __LINE__);
   std::string testMesg;

   string pz("PZ90");
   string wgs("WGS84");
   string unk("Unknown");

   testMesg = "asString function failed";
   ReferenceFrame rf1(ReferenceFrame::PZ90);
   testFramework.assert(rf1.asString() == pz, testMesg, __LINE__);

   ReferenceFrame rf2(ReferenceFrame::WGS84);
   testFramework.assert(rf2.asString() == wgs, testMesg, __LINE__);

   ReferenceFrame rf3(ReferenceFrame::Unknown);
   testFramework.assert(rf3.asString() == unk, testMesg, __LINE__);

   return testFramework.countFails();
}
int equalityTest(void)
{
   TestUtil testFramework("ReferenceFrame", "== Operator", __FILE__, __LINE__);
   std::string testMesg;

   ReferenceFrame rf1(ReferenceFrame::PZ90);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::Unknown);
   ReferenceFrame rf4("PZ90");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("Junk String That Will Never Match Up");

      //PZ90 Enum with...
   testMesg = " == operator failed with PZ90";
   testFramework.assert( rf1 == rf1, testMesg, __LINE__);
   testFramework.assert(!(rf1 == rf2) , testMesg, __LINE__);
   testFramework.assert(!(rf1 == rf3) , testMesg, __LINE__);
   testFramework.assert( rf1 == rf4, testMesg, __LINE__);
   testFramework.assert(!(rf1 == rf5) , testMesg, __LINE__);
   testFramework.assert(!(rf1 == rf6) , testMesg, __LINE__);

      //WGS84 Enum with...
   testMesg = " == operator failed with WGS84";
   testFramework.assert(!(rf2 == rf1) , testMesg, __LINE__);
   testFramework.assert( rf2 == rf2, testMesg, __LINE__);
   testFramework.assert(!(rf2 == rf3) , testMesg, __LINE__);
   testFramework.assert(!(rf2 == rf4) , testMesg, __LINE__);
   testFramework.assert( rf2 == rf5, testMesg, __LINE__);
   testFramework.assert(!(rf2 == rf6), testMesg, __LINE__);

      //Unknown Enum with... (Should fail every one)
   testMesg = " == operator failed with Unknown";
   testFramework.assert(!(rf3 == rf1) , testMesg, __LINE__);
   testFramework.assert(!(rf3 == rf2) , testMesg, __LINE__);
   testFramework.assert( (rf3 == rf3) , testMesg, __LINE__);
   testFramework.assert(!(rf3 == rf4) , testMesg, __LINE__);
   testFramework.assert(!(rf3 == rf5) , testMesg, __LINE__);
   testFramework.assert( (rf3 == rf6) , testMesg, __LINE__);

   return testFramework.countFails();
}
int inequalityTest(void)
{
   TestUtil testFramework("ReferenceFrame", "!= Operator", __FILE__, __LINE__);
   std::string testMesg;

   ReferenceFrame rf1(ReferenceFrame::PZ90);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::Unknown);
   ReferenceFrame rf4("PZ90");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("Junk String That Will Never Match Up");

      //PZ90 Enum with...
   testMesg = " != operator failed with PZ90";
   testFramework.assert( !(rf1 != rf1) , testMesg, __LINE__);
   testFramework.assert( (rf1 != rf2) , testMesg, __LINE__);
   testFramework.assert( (rf1 != rf3) , testMesg, __LINE__);
   testFramework.assert( !(rf1 != rf4) , testMesg, __LINE__);
   testFramework.assert( (rf1 != rf5) , testMesg, __LINE__);
   testFramework.assert( (rf1 != rf6) , testMesg, __LINE__);

      //WGS84 Enum with...
   testMesg = " != operator failed with WGS84";
   testFramework.assert( (rf2 != rf1) , testMesg, __LINE__);
   testFramework.assert( !(rf2 != rf2) , testMesg, __LINE__);
   testFramework.assert( (rf2 != rf3) , testMesg, __LINE__);
   testFramework.assert( (rf2 != rf4) , testMesg, __LINE__);
   testFramework.assert( !(rf2 != rf5) , testMesg, __LINE__);
   testFramework.assert( (rf2 != rf6) , testMesg, __LINE__);

      //Unknown Enum with...
   testMesg = " != operator failed with Unknown";
   testFramework.assert( (rf3 != rf1) , testMesg, __LINE__);
   testFramework.assert( (rf3 != rf2) , testMesg, __LINE__);
   testFramework.assert( !(rf3 != rf3) , testMesg, __LINE__);
   testFramework.assert( (rf3 != rf4) , testMesg, __LINE__);
   testFramework.assert( (rf3 != rf5) , testMesg, __LINE__);
   testFramework.assert( !(rf3 != rf6) , testMesg, __LINE__);

   return testFramework.countFails();
}
int greaterThanTest(void)
{
   TestUtil testFramework("ReferenceFrame", "> Operator", __FILE__, __LINE__);
   std::string testMesg;

   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);
   ReferenceFrame rf4("Junk String That Will Never Match Up");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("PZ90");

   testMesg = " > operator failed with Unknown";
   testFramework.assert( !(rf1 > rf1) , testMesg, __LINE__);
   testFramework.assert( !(rf1 > rf2) , testMesg, __LINE__);
   testFramework.assert( !(rf1 > rf3) , testMesg, __LINE__);
   testFramework.assert( !(rf1 > rf4) , testMesg, __LINE__);
   testFramework.assert( !(rf1 > rf5) , testMesg, __LINE__);
   testFramework.assert( !(rf1 > rf6) , testMesg, __LINE__);

   testMesg = " > operator failed with WGS84";
   testFramework.assert( (rf2 > rf1) , testMesg, __LINE__);
   testFramework.assert( !(rf2 > rf2) , testMesg, __LINE__);
   testFramework.assert( !(rf2 > rf3) , testMesg, __LINE__);
   testFramework.assert( (rf2 > rf4) , testMesg, __LINE__);
   testFramework.assert( !(rf2 > rf5) , testMesg, __LINE__);
   testFramework.assert( !(rf2 > rf6) , testMesg, __LINE__);

   testMesg = " > operator failed with PZ90";
   testFramework.assert( (rf3 > rf1) , testMesg, __LINE__);
   testFramework.assert( (rf3 > rf2) , testMesg, __LINE__);
   testFramework.assert( !(rf3 > rf3) , testMesg, __LINE__);
   testFramework.assert( (rf3 > rf4) , testMesg, __LINE__);
   testFramework.assert( (rf3 > rf5) , testMesg, __LINE__);
   testFramework.assert( !(rf3 > rf6) , testMesg, __LINE__);

   return testFramework.countFails();
}
int lessThanTest(void)
{
   TestUtil testFramework("ReferenceFrame", "< Operator", __FILE__, __LINE__);
   std::string testMesg;

   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);
   ReferenceFrame rf4("Junk String That Will Never Match Up");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("PZ90");

   testMesg = " < operator failed with Unknown";
   testFramework.assert( !(rf1 < rf1) , testMesg, __LINE__);
   testFramework.assert( rf1 < rf2 , testMesg, __LINE__);
   testFramework.assert( rf1 < rf3 , testMesg, __LINE__);
   testFramework.assert( !(rf1 < rf4) , testMesg, __LINE__);
   testFramework.assert( rf1 < rf5 , testMesg, __LINE__);
   testFramework.assert( rf1 < rf6 , testMesg, __LINE__);

   testMesg = " < operator failed with WGS84";
   testFramework.assert( !(rf2 < rf1) , testMesg, __LINE__);
   testFramework.assert( !(rf2 < rf2) , testMesg, __LINE__);
   testFramework.assert( (rf2 < rf3) , testMesg, __LINE__);
   testFramework.assert( !(rf2 < rf4) , testMesg, __LINE__);
   testFramework.assert( !(rf2 < rf5) , testMesg, __LINE__);
   testFramework.assert( (rf2 < rf6) , testMesg, __LINE__);

   testMesg = " < operator failed with PZ90";
   testFramework.assert( !(rf3 < rf1) , testMesg, __LINE__);
   testFramework.assert( !(rf3 < rf2) , testMesg, __LINE__);
   testFramework.assert( !(rf3 < rf3) , testMesg, __LINE__);
   testFramework.assert( !(rf3 < rf4) , testMesg, __LINE__);
   testFramework.assert( !(rf3 < rf5) , testMesg, __LINE__);
   testFramework.assert( !(rf3 < rf6) , testMesg, __LINE__);

   return testFramework.countFails();
}
int greaterThanOrEqualToTest(void)
{
   TestUtil testFramework("ReferenceFrame", ">= Operator", __FILE__, __LINE__);
   std::string testMesg;

   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);
   ReferenceFrame rf4("Junk String That Will Never Match Up");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("PZ90");

      //Unknown with...
   testMesg = " >= operator failed with Unknown";
   testFramework.assert( rf1 >= rf1 , testMesg, __LINE__);
   testFramework.assert( !(rf1 >= rf2) , testMesg, __LINE__);
   testFramework.assert( !(rf1 >= rf3) , testMesg, __LINE__);
   testFramework.assert( rf1 >= rf4 , testMesg, __LINE__);
   testFramework.assert( !(rf1 >= rf5) , testMesg, __LINE__);
   testFramework.assert( !(rf1 >= rf6) , testMesg, __LINE__);

   testMesg = " >= operator failed with WGS84";
   testFramework.assert( rf2 >= rf1 , testMesg, __LINE__);
   testFramework.assert( rf2 >= rf2 , testMesg, __LINE__);
   testFramework.assert( !(rf2 >= rf3) , testMesg, __LINE__);
   testFramework.assert( rf2 >= rf4 , testMesg, __LINE__);
   testFramework.assert( rf2 >= rf5 , testMesg, __LINE__);
   testFramework.assert( !(rf2 >= rf6) , testMesg, __LINE__);

   testMesg = " >= operator failed with PZ90";
   testFramework.assert( rf3 >= rf1 , testMesg, __LINE__);
   testFramework.assert( rf3 >= rf2 , testMesg, __LINE__);
   testFramework.assert( rf3 >= rf3 , testMesg, __LINE__);
   testFramework.assert( rf3 >= rf4 , testMesg, __LINE__);
   testFramework.assert( rf3 >= rf5 , testMesg, __LINE__);
   testFramework.assert( rf3 >= rf6 , testMesg, __LINE__);

   return testFramework.countFails();
}
int lesserThanOrEqualToTest(void)
{
   TestUtil testFramework("ReferenceFrame", "<= Operator", __FILE__, __LINE__);
   std::string testMesg;

   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);
   ReferenceFrame rf4("Junk String That Will Never Match Up");
   ReferenceFrame rf5("WGS84");
   ReferenceFrame rf6("PZ90");

   testMesg = " <= operator failed with Unknown";
   testFramework.assert( rf1 <= rf1 , testMesg, __LINE__);
   testFramework.assert( rf1 <= rf2 , testMesg, __LINE__);
   testFramework.assert( rf1 <= rf3 , testMesg, __LINE__);
   testFramework.assert( rf1 <= rf4 , testMesg, __LINE__);
   testFramework.assert( rf1 <= rf5 , testMesg, __LINE__);
   testFramework.assert( rf1 <= rf6 , testMesg, __LINE__);

   testMesg = " <= operator failed with WGS84";
   testFramework.assert( !(rf2 <= rf1) , testMesg, __LINE__);
   testFramework.assert( rf2 <= rf2 , testMesg, __LINE__);
   testFramework.assert( rf2 <= rf3 , testMesg, __LINE__);
   testFramework.assert( !(rf2 <= rf4) , testMesg, __LINE__);
   testFramework.assert( rf2 <= rf5 , testMesg, __LINE__);
   testFramework.assert( rf2 <= rf6 , testMesg, __LINE__);

   testMesg = " <= operator failed with PZ90";
   testFramework.assert( !(rf3 <= rf1) , testMesg, __LINE__);
   testFramework.assert( !(rf3 <= rf2) , testMesg, __LINE__);
   testFramework.assert( rf3 <= rf3 , testMesg, __LINE__);
   testFramework.assert( !(rf3 <= rf4) , testMesg, __LINE__);
   testFramework.assert( !(rf3 <= rf5) , testMesg, __LINE__);
   testFramework.assert( rf3 <= rf6 , testMesg, __LINE__);

   return testFramework.countFails();
}
int setReferenceFrameTest(void)
{
   TestUtil testFramework("ReferenceFrame", "ReferenceFrame", __FILE__, __LINE__);
   std::string testMesg;

   ReferenceFrame rf1(ReferenceFrame::Unknown);
   ReferenceFrame rf2(ReferenceFrame::WGS84);
   ReferenceFrame rf3(ReferenceFrame::PZ90);

   testMesg = "setReferenceFrame failed";
   ReferenceFrame frame(ReferenceFrame::Unknown);
   testFramework.assert(frame == rf1, testMesg, __LINE__);
   testFramework.assert(frame != rf2, testMesg, __LINE__);
   testFramework.assert(frame != rf3, testMesg, __LINE__);

   frame = ReferenceFrame((ReferenceFrame::Frames)0);
   testFramework.assert(frame == rf1, testMesg, __LINE__);
   testFramework.assert(frame != rf2, testMesg, __LINE__);
   testFramework.assert(frame != rf3, testMesg, __LINE__);

   frame = ReferenceFrame("Unknown");
   testFramework.assert(frame == rf1, testMesg, __LINE__);
   testFramework.assert(frame != rf2, testMesg, __LINE__);
   testFramework.assert(frame != rf3, testMesg, __LINE__);

   frame = ReferenceFrame("A Junk String that won't match up");
   testFramework.assert(frame == rf1, testMesg, __LINE__);
   testFramework.assert(frame != rf2, testMesg, __LINE__);
   testFramework.assert(frame != rf3, testMesg, __LINE__);

   frame = ReferenceFrame((ReferenceFrame::Frames)-1);
   testFramework.assert(frame == rf1, testMesg, __LINE__);
   testFramework.assert(frame != rf2, testMesg, __LINE__);
   testFramework.assert(frame != rf3, testMesg, __LINE__);

   frame = ReferenceFrame((ReferenceFrame::Frames)-1);
   testFramework.assert(frame == rf1, testMesg, __LINE__);
   testFramework.assert(frame != rf2, testMesg, __LINE__);
   testFramework.assert(frame != rf3, testMesg, __LINE__);

   frame = ReferenceFrame(ReferenceFrame::WGS84);
   testFramework.assert(frame != rf1, testMesg, __LINE__);
   testFramework.assert(frame == rf2, testMesg, __LINE__);
   testFramework.assert(frame != rf3, testMesg, __LINE__);

   frame = ReferenceFrame("WGS84");
   testFramework.assert(frame != rf1, testMesg, __LINE__);
   testFramework.assert(frame == rf2, testMesg, __LINE__);
   testFramework.assert(frame != rf3, testMesg, __LINE__);

   frame = ReferenceFrame((ReferenceFrame::Frames)1);
   testFramework.assert(frame != rf1, testMesg, __LINE__);
   testFramework.assert(frame == rf2, testMesg, __LINE__);
   testFramework.assert(frame != rf3, testMesg, __LINE__);

   frame = ReferenceFrame(ReferenceFrame::PZ90);
   testFramework.assert(frame != rf1, testMesg, __LINE__);
   testFramework.assert(frame != rf2, testMesg, __LINE__);
   testFramework.assert(frame == rf3, testMesg, __LINE__);

   frame = ReferenceFrame("PZ90");
   testFramework.assert(frame != rf1, testMesg, __LINE__);
   testFramework.assert(frame != rf2, testMesg, __LINE__);
   testFramework.assert(frame == rf3, testMesg, __LINE__);

   frame = ReferenceFrame((ReferenceFrame::Frames)6);
   testFramework.assert(frame != rf1, testMesg, __LINE__);
   testFramework.assert(frame != rf2, testMesg, __LINE__);
   testFramework.assert(frame == rf3, testMesg, __LINE__);

   return testFramework.countFails();
}
};

int main(void)
{
   ReferenceFrame_T testClass;
   int check, errorCounter = 0;

   check = testClass.getFrameTest();
   errorCounter += check;

   check = testClass.asStringTest();
   errorCounter += check;

   check = testClass.equalityTest();
   errorCounter += check;

   check = testClass.inequalityTest();
   errorCounter += check;

   check = testClass.greaterThanTest();
   errorCounter += check;

   check = testClass.lessThanTest();
   errorCounter += check;

   check = testClass.greaterThanOrEqualToTest();
   errorCounter += check;

   check = testClass.lesserThanOrEqualToTest();
   errorCounter += check;

   check = testClass.setReferenceFrameTest();
   errorCounter += check;

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

   return errorCounter;
}

