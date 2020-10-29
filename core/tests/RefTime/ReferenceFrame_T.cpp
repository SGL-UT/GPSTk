//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================


#include "ReferenceFrame.hpp"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

class ReferenceFrame_T
{
public:
   unsigned asStringTest()
   {
      TUDEF("ReferenceFrame", "asString");

      string pz("PZ90");
      string wgs("WGS84");
      string unk("Unknown");

      ReferenceFrame rf1(ReferenceFrame::PZ90);
      TUASSERTE(std::string, pz, gpstk::StringUtils::asString(rf1));

      ReferenceFrame rf2(ReferenceFrame::WGS84);
      TUASSERTE(std::string, wgs, gpstk::StringUtils::asString(rf2));

      ReferenceFrame rf3(ReferenceFrame::Unknown);
      TUASSERTE(std::string, unk, gpstk::StringUtils::asString(rf3));

      TURETURN();
   }


   unsigned equalityTest()
   {
      TUDEF("ReferenceFrame", "operator==");

      ReferenceFrame rf1(ReferenceFrame::PZ90);
      ReferenceFrame rf2(ReferenceFrame::WGS84);
      ReferenceFrame rf3(ReferenceFrame::Unknown);
      ReferenceFrame rf4 = gpstk::StringUtils::asReferenceFrame("PZ90");
      ReferenceFrame rf5 = gpstk::StringUtils::asReferenceFrame("WGS84");
      ReferenceFrame rf6 = gpstk::StringUtils::asReferenceFrame("Junk");

         //PZ90 Enum with...
      TUASSERT( rf1 == rf1);
      TUASSERT(!(rf1 == rf2) );
      TUASSERT(!(rf1 == rf3) );
      TUASSERT( rf1 == rf4);
      TUASSERT(!(rf1 == rf5) );
      TUASSERT(!(rf1 == rf6) );

         //WGS84 Enum with...
      TUASSERT(!(rf2 == rf1) );
      TUASSERT( rf2 == rf2);
      TUASSERT(!(rf2 == rf3) );
      TUASSERT(!(rf2 == rf4) );
      TUASSERT( rf2 == rf5);
      TUASSERT(!(rf2 == rf6));

         //Unknown Enum with... (Should fail every one)
      TUASSERT(!(rf3 == rf1) );
      TUASSERT(!(rf3 == rf2) );
      TUASSERT( (rf3 == rf3) );
      TUASSERT(!(rf3 == rf4) );
      TUASSERT(!(rf3 == rf5) );
      TUASSERT( (rf3 == rf6) );

      TURETURN();
   }


   unsigned inequalityTest()
   {
      TUDEF("ReferenceFrame", "operator!=");

      ReferenceFrame rf1(ReferenceFrame::PZ90);
      ReferenceFrame rf2(ReferenceFrame::WGS84);
      ReferenceFrame rf3(ReferenceFrame::Unknown);
      ReferenceFrame rf4 = gpstk::StringUtils::asReferenceFrame("PZ90");
      ReferenceFrame rf5 = gpstk::StringUtils::asReferenceFrame("WGS84");
      ReferenceFrame rf6 = gpstk::StringUtils::asReferenceFrame("Junk");

         //PZ90 Enum with...
      TUASSERT( !(rf1 != rf1) );
      TUASSERT( (rf1 != rf2) );
      TUASSERT( (rf1 != rf3) );
      TUASSERT( !(rf1 != rf4) );
      TUASSERT( (rf1 != rf5) );
      TUASSERT( (rf1 != rf6) );

         //WGS84 Enum with...
      TUASSERT( (rf2 != rf1) );
      TUASSERT( !(rf2 != rf2) );
      TUASSERT( (rf2 != rf3) );
      TUASSERT( (rf2 != rf4) );
      TUASSERT( !(rf2 != rf5) );
      TUASSERT( (rf2 != rf6) );

         //Unknown Enum with...
      TUASSERT( (rf3 != rf1) );
      TUASSERT( (rf3 != rf2) );
      TUASSERT( !(rf3 != rf3) );
      TUASSERT( (rf3 != rf4) );
      TUASSERT( (rf3 != rf5) );
      TUASSERT( !(rf3 != rf6) );

      TURETURN();
   }


   unsigned greaterThanTest()
   {
      TUDEF("ReferenceFrame", "operator>");

      ReferenceFrame rf1(ReferenceFrame::Unknown);
      ReferenceFrame rf2(ReferenceFrame::WGS84);
      ReferenceFrame rf3(ReferenceFrame::PZ90);
      ReferenceFrame rf4 = gpstk::StringUtils::asReferenceFrame("Junk");
      ReferenceFrame rf5 = gpstk::StringUtils::asReferenceFrame("WGS84");
      ReferenceFrame rf6 = gpstk::StringUtils::asReferenceFrame("PZ90");

      TUASSERT( !(rf1 > rf1) );
      TUASSERT( !(rf1 > rf2) );
      TUASSERT( !(rf1 > rf3) );
      TUASSERT( !(rf1 > rf4) );
      TUASSERT( !(rf1 > rf5) );
      TUASSERT( !(rf1 > rf6) );

      TUASSERT( (rf2 > rf1) );
      TUASSERT( !(rf2 > rf2) );
      TUASSERT( !(rf2 > rf3) );
      TUASSERT( (rf2 > rf4) );
      TUASSERT( !(rf2 > rf5) );
      TUASSERT( !(rf2 > rf6) );

      TUASSERT( (rf3 > rf1) );
      TUASSERT( (rf3 > rf2) );
      TUASSERT( !(rf3 > rf3) );
      TUASSERT( (rf3 > rf4) );
      TUASSERT( (rf3 > rf5) );
      TUASSERT( !(rf3 > rf6) );

      TURETURN();
   }


   unsigned lessThanTest()
   {
      TUDEF("ReferenceFrame", "operator<");

      ReferenceFrame rf1(ReferenceFrame::Unknown);
      ReferenceFrame rf2(ReferenceFrame::WGS84);
      ReferenceFrame rf3(ReferenceFrame::PZ90);
      ReferenceFrame rf6 = gpstk::StringUtils::asReferenceFrame("PZ90");
      ReferenceFrame rf5 = gpstk::StringUtils::asReferenceFrame("WGS84");
      ReferenceFrame rf4 = gpstk::StringUtils::asReferenceFrame("Junk");

      TUASSERT( !(rf1 < rf1) );
      TUASSERT( rf1 < rf2 );
      TUASSERT( rf1 < rf3 );
      TUASSERT( !(rf1 < rf4) );
      TUASSERT( rf1 < rf5 );
      TUASSERT( rf1 < rf6 );

      TUASSERT( !(rf2 < rf1) );
      TUASSERT( !(rf2 < rf2) );
      TUASSERT( (rf2 < rf3) );
      TUASSERT( !(rf2 < rf4) );
      TUASSERT( !(rf2 < rf5) );
      TUASSERT( (rf2 < rf6) );

      TUASSERT( !(rf3 < rf1) );
      TUASSERT( !(rf3 < rf2) );
      TUASSERT( !(rf3 < rf3) );
      TUASSERT( !(rf3 < rf4) );
      TUASSERT( !(rf3 < rf5) );
      TUASSERT( !(rf3 < rf6) );

      TURETURN();
   }


   unsigned greaterThanOrEqualToTest()
   {
      TUDEF("ReferenceFrame", "operator>=");

      ReferenceFrame rf1(ReferenceFrame::Unknown);
      ReferenceFrame rf2(ReferenceFrame::WGS84);
      ReferenceFrame rf3(ReferenceFrame::PZ90);
      ReferenceFrame rf6 = gpstk::StringUtils::asReferenceFrame("PZ90");
      ReferenceFrame rf5 = gpstk::StringUtils::asReferenceFrame("WGS84");
      ReferenceFrame rf4 = gpstk::StringUtils::asReferenceFrame("Junk");

         //Unknown with...
      TUASSERT( rf1 >= rf1 );
      TUASSERT( !(rf1 >= rf2) );
      TUASSERT( !(rf1 >= rf3) );
      TUASSERT( rf1 >= rf4 );
      TUASSERT( !(rf1 >= rf5) );
      TUASSERT( !(rf1 >= rf6) );

      TUASSERT( rf2 >= rf1 );
      TUASSERT( rf2 >= rf2 );
      TUASSERT( !(rf2 >= rf3) );
      TUASSERT( rf2 >= rf4 );
      TUASSERT( rf2 >= rf5 );
      TUASSERT( !(rf2 >= rf6) );

      TUASSERT( rf3 >= rf1 );
      TUASSERT( rf3 >= rf2 );
      TUASSERT( rf3 >= rf3 );
      TUASSERT( rf3 >= rf4 );
      TUASSERT( rf3 >= rf5 );
      TUASSERT( rf3 >= rf6 );

      TURETURN();
   }


   unsigned lesserThanOrEqualToTest()
   {
      TUDEF("ReferenceFrame", "operator<=");

      ReferenceFrame rf1(ReferenceFrame::Unknown);
      ReferenceFrame rf2(ReferenceFrame::WGS84);
      ReferenceFrame rf3(ReferenceFrame::PZ90);
      ReferenceFrame rf6 = gpstk::StringUtils::asReferenceFrame("PZ90");
      ReferenceFrame rf5 = gpstk::StringUtils::asReferenceFrame("WGS84");
      ReferenceFrame rf4 = gpstk::StringUtils::asReferenceFrame("Junk");

      TUASSERT( rf1 <= rf1 );
      TUASSERT( rf1 <= rf2 );
      TUASSERT( rf1 <= rf3 );
      TUASSERT( rf1 <= rf4 );
      TUASSERT( rf1 <= rf5 );
      TUASSERT( rf1 <= rf6 );

      TUASSERT( !(rf2 <= rf1) );
      TUASSERT( rf2 <= rf2 );
      TUASSERT( rf2 <= rf3 );
      TUASSERT( !(rf2 <= rf4) );
      TUASSERT( rf2 <= rf5 );
      TUASSERT( rf2 <= rf6 );

      TUASSERT( !(rf3 <= rf1) );
      TUASSERT( !(rf3 <= rf2) );
      TUASSERT( rf3 <= rf3 );
      TUASSERT( !(rf3 <= rf4) );
      TUASSERT( !(rf3 <= rf5) );
      TUASSERT( rf3 <= rf6 );

      TURETURN();
   }


   unsigned setReferenceFrameTest()
   {
      TUDEF("ReferenceFrame", "ReferenceFrame");

      ReferenceFrame rf1(ReferenceFrame::Unknown);
      ReferenceFrame rf2(ReferenceFrame::WGS84);
      ReferenceFrame rf3(ReferenceFrame::PZ90);

      ReferenceFrame frame(ReferenceFrame::Unknown);
      TUASSERT(frame == rf1);
      TUASSERT(frame != rf2);
      TUASSERT(frame != rf3);

      frame = ReferenceFrame((ReferenceFrame)0);
      TUASSERT(frame == rf1);
      TUASSERT(frame != rf2);
      TUASSERT(frame != rf3);

      frame = gpstk::StringUtils::asReferenceFrame("Unknown");
      TUASSERT(frame == rf1);
      TUASSERT(frame != rf2);
      TUASSERT(frame != rf3);

      frame = gpstk::StringUtils::asReferenceFrame("Junk");
      TUASSERT(frame == rf1);
      TUASSERT(frame != rf2);
      TUASSERT(frame != rf3);

      frame = ReferenceFrame((ReferenceFrame)-1);
      TUASSERT(frame != rf1);
      TUASSERT(frame != rf2);
      TUASSERT(frame != rf3);

      frame = ReferenceFrame(ReferenceFrame::WGS84);
      TUASSERT(frame != rf1);
      TUASSERT(frame == rf2);
      TUASSERT(frame != rf3);

      frame = gpstk::StringUtils::asReferenceFrame("WGS84");
      TUASSERT(frame != rf1);
      TUASSERT(frame == rf2);
      TUASSERT(frame != rf3);

      frame = ReferenceFrame((ReferenceFrame)1);
      TUASSERT(frame != rf1);
      TUASSERT(frame == rf2);
      TUASSERT(frame != rf3);

      frame = ReferenceFrame(ReferenceFrame::PZ90);
      TUASSERT(frame != rf1);
      TUASSERT(frame != rf2);
      TUASSERT(frame == rf3);

      frame = gpstk::StringUtils::asReferenceFrame("PZ90");
      TUASSERT(frame != rf1);
      TUASSERT(frame != rf2);
      TUASSERT(frame == rf3);

      frame = ReferenceFrame((ReferenceFrame)6);
      TUASSERT(frame != rf1);
      TUASSERT(frame != rf2);
      TUASSERT(frame == rf3);

      TURETURN();
   }
};


int main()
{
   ReferenceFrame_T testClass;
   unsigned errorTotal = 0;

   errorTotal += testClass.asStringTest();
   errorTotal += testClass.equalityTest();
   errorTotal += testClass.inequalityTest();
   errorTotal += testClass.greaterThanTest();
   errorTotal += testClass.lessThanTest();
   errorTotal += testClass.greaterThanOrEqualToTest();
   errorTotal += testClass.lesserThanOrEqualToTest();
   errorTotal += testClass.setReferenceFrameTest();
   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal;
}

