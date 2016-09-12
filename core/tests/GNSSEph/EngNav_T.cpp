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
#include "EngNav.hpp"
#include "TestUtil.hpp"
#include "TimeString.hpp"
#include "GPSWeekSecond.hpp"
#include <math.h>
#include <iostream>

using namespace std;

class EngNav_T
{
public:
   EngNav_T() // Default Constructor, set the precision value
   {
         // lower precision value, accuracy of some values is lost in
         // binary conversion
      eps = 1E-10;
      b10 = 10;
   }
   ~EngNav_T() {}


   unsigned getSubframePatternTest(void)
   {
      TUDEF("EngNav", "getSubframePattern");
      string testMesg;

      const uint32_t subframe1P[10] =
         { 0x22c000e4, 0x215ba160, 0x00180012, 0x1fffffc0, 0x3fffffc3,
           0x3ffffffc, 0x3fffc009, 0x16d904f0, 0x003fdbac, 0x247c139c };
      const uint32_t subframe2P[10] =
         { 0x22c000e4, 0x215bc2f0, 0x16c2eb4d, 0x032c41a0, 0x26abc7e0,
           0x0289c0dd, 0x0d5ecc38, 0x036b6842, 0x034f4df0, 0x1904c0b4 };
      const uint32_t subframe3P[10] =
         { 0x22c000e4, 0x215be378, 0x3ffcc344, 0x1a8441f1, 0x3ff80b74,
           0x1c8deb5e, 0x0a34d52d, 0x14a5013e, 0x3fee8c2f, 0x16c35c80 };

      testMesg = "Subframe Pattern obtained was incorrect";
      TUASSERTE(short,1,gpstk::EngNav::getSubframePattern(subframe1P));
      TUASSERTE(short,2,gpstk::EngNav::getSubframePattern(subframe2P));
      TUASSERTE(short,3,gpstk::EngNav::getSubframePattern(subframe3P));

      TURETURN();
   }


   unsigned computeParityTest(void)
   {
      TUDEF("EngNav", "Compute Parity");
      string testMesg;

         // data taken from
         // http://www.gpscreations.com/NewFiles/GPS%20Parity%20Checking.pdf

         //Feed in 30bit word with 0's as the parity
      uint32_t zero =  0x00000000;
      uint32_t data1 = 0x22C000C0;
      uint32_t data2 = 0x17344000;
      uint32_t data3 = 0x2142EF00;
      uint32_t data4 = 0x15E67180;

      testMesg = "Parity computed was incorrect";
      TUASSERTE(uint32_t, 0x24, gpstk::EngNav::computeParity(data1, zero));

      data1 |= 0x24;

      TUASSERTE(uint32_t, 0x22, gpstk::EngNav::computeParity(data2, data1));

      data2 |= 0x22;

      TUASSERTE(uint32_t, 0x1b, gpstk::EngNav::computeParity(data3, data2));

      data3 |= 0x1B;

      TUASSERTE(uint32_t, 0x02,
                gpstk::EngNav::computeParity(data4, data3, false));

      TURETURN();
   }

   unsigned fixParityTest(void)
   {
      TUDEF("EngNav", "Fix Parity");
      string testMesg;

         // 3 cases of regular parity computation
      uint32_t data1 = 0x22C000C0;
      uint32_t data2 = 0x17344000;
      uint32_t data3 = 0x2142EF00;
      uint32_t data4 = 0x15E67180;
         // test word with the non-informational parity bits, set to 0
      uint32_t data5 = 0x32098100; //taken from EngEphemeris
      uint32_t CompareData1 = 0x22C000C0 | 0x0000024;
      uint32_t CompareData2 = 0x17344000 | 0x0000022;
      uint32_t CompareData3 = 0x2142EF00 | 0x000001B;
      uint32_t CompareData4 = 0x15E67180 | 0x0000002;
         // non-informational parity bits included in this
      uint32_t CompareData5 = 0x32098100 | 0x00000DC;

         // (word to overwrite with parity, previous word, add 2
         // parity computation bits(word 2 & 10))
      testMesg = "Parity computed is incorrect";
      TUASSERTE(uint32_t, CompareData1,
                gpstk::EngNav::fixParity(data1, 0, false));
      TUASSERTE(uint32_t, CompareData2,
                gpstk::EngNav::fixParity(data2, CompareData1, false));
      TUASSERTE(uint32_t, CompareData3,
                gpstk::EngNav::fixParity(data3, CompareData2, false));
      TUASSERTE(uint32_t, CompareData4,
                gpstk::EngNav::fixParity(data4, CompareData3, false, false));
      TUASSERTE(uint32_t, CompareData5,
                gpstk::EngNav::fixParity(data5, 0, true));

      TURETURN();
   }


   unsigned checkParityTest(void)
   {
      TUDEF("EngNav", "Check Parity");
      string testMesg;

         //Data is from EngEphemeris addSubframe test

      const uint32_t subframe1P[10] =
         { 0x22c000e4, 0x215ba160, 0x00180012, 0x1fffffc0, 0x3fffffc3,
           0x3fffffff, 0x3fffc035, 0x16d904f3, 0x003fdb90, 0x247c1339 };
      const uint32_t subframe2P[10] =
         { 0x22c000e4, 0x215bc2f0, 0x16c2eb4d, 0x032c41a3, 0x26abc7dc,
           0x0289c0dd, 0x0d5ecc3b, 0x0036b67f, 0x034f4de5, 0x1904c0a1 };
      const uint32_t subframe3P[10] =
         { 0x22c000e4, 0x215be378, 0x3ffcc344, 0x1a8441f1, 0x3ff80b61,
           0x1c8deb4b, 0x0a34d530, 0x14a50138, 0x3fee8c2f, 0x16c35c83 };

      testMesg = "Parity computed is incorrect";
      testFramework.assert(gpstk::EngNav::checkParity(subframe1P, false),
                           testMesg, __LINE__);
      testFramework.assert(gpstk::EngNav::checkParity(subframe2P, false),
                           testMesg, __LINE__);
      testFramework.assert(gpstk::EngNav::checkParity(subframe3P, false),
                           testMesg, __LINE__);

      TURETURN();
   }


   unsigned getHOWTimeTest(void)
   {
         //wrong, fix later
      TUDEF("EngNav", "getHOWTime");
      string testMesg;

      uint32_t how1 = 0x215ba160;
      uint32_t how2 = 0x215bc2f0;
      uint32_t how3 = 0x215be378;

      testMesg = "Returned TOW time from the HOW is incorrect";
      TUASSERTE(unsigned long, 409902, gpstk::EngNav::getHOWTime(how1));
      TUASSERTE(unsigned long, 409908, gpstk::EngNav::getHOWTime(how2));
      TUASSERTE(unsigned long, 409914, gpstk::EngNav::getHOWTime(how3));

      TURETURN();
   }


   unsigned getSFIDTest(void)
   {
      TUDEF("EngNav", "getSFID");
      string testMesg;

      uint32_t how1 = 0x215ba160;
      uint32_t how2 = 0x215bc2f0;
      uint32_t how3 = 0x215be378;

      testMesg = "Returned subframe ID was incorrect";
      TUASSERTE(short, 1, gpstk::EngNav::getSFID(how1));
      TUASSERTE(short, 2, gpstk::EngNav::getSFID(how2));
      TUASSERTE(short, 3, gpstk::EngNav::getSFID(how3));

      TURETURN();
   }

      //converts subframe binary data to FIC.
   unsigned subframeConvertTest(void) //calls getsubframePattern and convertQuant
   {
      TUDEF("EngNav", "Subframe Convert");
      string testMesg;

      double output1[60], output2[60], output3[60];
      const uint32_t subframe1P[10] =
         { 0x22c000e4, 0x215ba160, 0x00180012, 0x1fffffc0, 0x3fffffc3,
           0x3ffffffc, 0x3fffc009, 0x16d904f0, 0x003fdbac, 0x247c139c };
      const uint32_t subframe2P[10] =
         { 0x22c000e4, 0x215bc2f0, 0x16c2eb4d, 0x032c41a0, 0x26abc7e0,
           0x0289c0dd, 0x0d5ecc38, 0x036b6842, 0x034f4df0, 0x1904c0b4 };
      const uint32_t subframe3P[10] =
         { 0x22c000e4, 0x215be378, 0x3ffcc344, 0x1a8441f1, 0x3ff80b76,
           0x1c8deb5e, 0x0a34d52d, 0x14a5013e, 0x3fee8c2f, 0x16c35c80 };
      gpstk::EngNav EngNavThing;

      testMesg = "Subframe Convert function failed";
      testFramework.assert(
         EngNavThing.subframeConvert(subframe1P, 1025, output1),
         testMesg, __LINE__);

      testMesg = "TLM Preamble is incorrect";
      testFramework.assert(output1[0] == 0x8B, testMesg, __LINE__);
      testMesg = "TLM Message is incorrect";
      testFramework.assert(output1[1] == 0, testMesg, __LINE__);
      testMesg = "How Word (time?) is incorrect";
      testFramework.assert(output1[2] == 409902, testMesg, __LINE__);
      testMesg = "Alert flag is incorrect";
      testFramework.assert(output1[3] == 0, testMesg, __LINE__);
      testMesg = "Subframe ID is incorrect";
      testFramework.assert(output1[4] == 1, testMesg, __LINE__);
      testMesg = "Transmit Week Number is incorrect";
      testFramework.assert(output1[5] == 1025, testMesg, __LINE__);
      testMesg = "L2 code flag is incorrect";
      testFramework.assert(output1[6] == 2, testMesg, __LINE__);
      testMesg = "SV Accuracy is incorrect";
      testFramework.assert(output1[7] == 0, testMesg, __LINE__);
      testMesg = "SV Health is incorrect";
      testFramework.assert(output1[8] == 0, testMesg, __LINE__);
      testMesg = "IODC flag is incorrect";
      testFramework.assert(output1[9]/2048 == 0x5B, testMesg,
                           __LINE__); //AODC to IODC conversion, pg 15 of GR-SGL-99-14 FIC Definiton file
      testMesg = "L2 code flag is incorrect";
      testFramework.assert(output1[10] == 0, testMesg, __LINE__);
      testMesg = "Group Delay Differential is incorrect";
      testFramework.assert(output1[11] == 0, testMesg, __LINE__);
      testMesg = "Clock Epoch is incorrect";
      testFramework.assert(output1[12] == 409904, testMesg, __LINE__);
      testMesg = "Clock Drift Rate is incorrect";
      testFramework.assert(output1[13] == 0, testMesg, __LINE__);
      testMesg = "Clock Drift is incorrect";
      testFramework.assert(abs(output1[14] + .165982783074E-10)*pow(b10,10) < eps,
                           testMesg, __LINE__);
      testMesg = "Clock Bias is incorrect";
      testFramework.assert(abs(output1[15] + .839701388031E-03)*pow(b10,3) < eps,
                           testMesg, __LINE__);

      testMesg = "Subframe Convert function failed";
      testFramework.assert(EngNavThing.subframeConvert(subframe2P, 1025, output2),
                           testMesg, __LINE__);

      testMesg = "TLM Preamble is incorrect";
      testFramework.assert(output2[0] == 0x8B, testMesg, __LINE__);
      testMesg = "TLM Message is incorrect";
      testFramework.assert(output2[1] == 0, testMesg, __LINE__);
      testMesg = "How Word (time?) is incorrect";
      testFramework.assert(output2[2] == 409908, testMesg, __LINE__);
      testMesg = "Alert flag is incorrect";
      testFramework.assert(output2[3] == 0, testMesg, __LINE__);
      testMesg = "Subframe ID is incorrect";
      testFramework.assert(output2[4] == 2, testMesg, __LINE__);
      testMesg = "IODE is incorrect";
      testFramework.assert(output2[5]/2048 == 91, testMesg,
                           __LINE__); //AODE to IODE conversion, pg 15 of GR-SGL-99-14 FIC Definiton file
      testMesg = "CRS is incorrect";
      testFramework.assert(abs(output2[6] - 93.40625) < eps, testMesg, __LINE__);
      testMesg = "Correction to Mean Motion is incorrect";
      testFramework.assert(abs(output2[7] - (.11604054784E-8))*pow(b10,8) < eps,
                           testMesg, __LINE__);
      testMesg = "Mean Anomaly at Epoch is incorrect";
      testFramework.assert(abs(output2[8] - 0.162092304801) < eps, testMesg,
                           __LINE__);
      testMesg = "CUC is incorrect";
      testFramework.assert(abs(output2[9] - .484101474285E-5)*pow(b10,5) < eps,
                           testMesg, __LINE__);
      testMesg = "Eccentricity is incorrect";
      testFramework.assert(abs(output2[10] - .626740418375E-2)*pow(b10,2) < eps,
                           testMesg, __LINE__);
      testMesg = "CUS is incorrect";
      testFramework.assert(abs(output2[11] - .652112066746E-5)*pow(b10,5) < eps,
                           testMesg, __LINE__);
      testMesg = "Square Root of Semi-Major Axis is incorrect";
      testFramework.assert(abs(output2[12] - .515365489006E4)*pow(b10,-4) < eps,
                           testMesg, __LINE__);
      testMesg = "Time of Epoch is incorrect";
      testFramework.assert(output2[13] == 409904, testMesg, __LINE__);
      testMesg = "Fit interval flag is incorrect";
      testFramework.assert(output2[14] == 0, testMesg, __LINE__);

      testMesg = "Subframe Convert function failed";
      testFramework.assert(EngNavThing.subframeConvert(subframe3P, 1025, output3),
                           testMesg, __LINE__);

      testMesg = "TLM Preamble is incorrect";
      testFramework.assert(output3[0] == 0x8B, testMesg, __LINE__);
      testMesg = "TLM Message is incorrect";
      testFramework.assert(output3[1] == 0, testMesg, __LINE__);
      testMesg = "How Word (time?) is incorrect";
      testFramework.assert(output3[2] == 409914, testMesg, __LINE__);
      testMesg = "Alert flag is incorrect";
      testFramework.assert(output3[3] == 0, testMesg, __LINE__);
      testMesg = "Subframe ID is incorrect";
      testFramework.assert(output3[4] == 3, testMesg, __LINE__);
      testMesg = "CIC is incorrect";
      testFramework.assert(abs(output3[5] + .242143869400E-7)*pow(b10,7) < eps,
                           testMesg, __LINE__);
      testMesg = "Right ascension of ascending node is incorrect";
      testFramework.assert(abs(output3[6] - .329237003460) < eps, testMesg,
                           __LINE__);
      testMesg = "CIS is incorrect";
      testFramework.assert(abs(output3[7] + .596046447754E-7)*pow(b10,7) < eps,
                           testMesg, __LINE__);
      testMesg = "Inclination is incorrect";
      testFramework.assert(abs(output3[8] - 1.11541663136) < eps, testMesg,
                           __LINE__);
      testMesg = "CRC is incorrect";
      testFramework.assert(abs(output3[9] - 326.59375)*pow(b10, -3) < eps, testMesg,
                           __LINE__);
      testMesg =
         "Arguement of perigee is incorrect"; // All other values needed to be converted to semi-circles, IDK why this one wasn't
      testFramework.assert(abs(output3[10] - 2.06958726335)*pow(b10, -1) < eps,
                           testMesg, __LINE__);
      testMesg = "Right ascension of ascending node time derivative is incorrect";
      testFramework.assert(abs(output3[11] + .638312302555E-8)*pow(b10,10) < eps,
                           testMesg, __LINE__);
      testMesg = "AODE? is incorrect";
      testFramework.assert(output3[12]/2048 == 91, testMesg, __LINE__);
      testMesg = "Inclination time derivative is incorrect";
      testFramework.assert(abs(output3[13] - .307155651409E-9)*pow(b10,9) < eps,
                           testMesg, __LINE__);//AODE to IODE conversion, pg 15 of GR-SGL-99-14 FIC Definiton file

      TURETURN();
   }


      /** Reads a subframe 2 along with truth data from a file and
       * makes sure everything matches up. */
   unsigned nmctValidityTest()
   {
      using gpstk::StringUtils::x2uint;
      using gpstk::StringUtils::word;
      using gpstk::StringUtils::numWords;
      using gpstk::StringUtils::asUnsigned;
      using gpstk::StringUtils::asDouble;
      TUDEF("EngNav", "getNMCTValidity");
         // Here's the input file to look at for the actual test data
      string infilename = gpstk::getPathData() +
         gpstk::getFileSep() + "test_getNMCTValidity.txt";
      ifstream infile(infilename.c_str());
      string line;
      uint32_t sf2[10];
      unsigned lineNo = 0;
      unsigned howWeek;
      uint32_t aodoExp, aodoGot;
      gpstk::CommonTime tnmctExp, tnmctGot, toeExp, toeGot, totGot;
      bool gotData = false; // make sure something was processed
         // tmp values for reading from file
      unsigned tmpU;
      double tmpD;
      while (infile)
      {
         lineNo++;
         getline(infile, line);
         if ((line[0] == '#') || (numWords(line) != 16))
            continue;
            //cout << "Line # " << lineNo << endl;
         gotData = true;
            // process the subframe data 
         for (unsigned wordnum = 0; wordnum < 10; wordnum++)
         {
            sf2[wordnum] = x2uint(word(line, wordnum));
         }
            // process the week number and expected values
         howWeek  = asUnsigned(word(line, 10));
         aodoExp  = asUnsigned(word(line, 11));
         tmpU     = asUnsigned(word(line, 12));
         tmpD     = asDouble(word(line, 13));
         tnmctExp = gpstk::GPSWeekSecond(tmpU, tmpD);
         tmpU     = asUnsigned(word(line, 14));
         tmpD     = asDouble(word(line, 15));
         toeExp   = gpstk::GPSWeekSecond(tmpU, tmpD);
            // Compare the truth data with the results from the function
         if (aodoExp == 27900)
         {
            testFramework.assert(
               !gpstk::EngNav::getNMCTValidity(
                  sf2, howWeek, aodoGot, tnmctGot, toeGot, totGot),
               "Unexpected return value", __LINE__);
            TUASSERTE(uint32_t, aodoExp, aodoGot);
            TUASSERTE(gpstk::CommonTime, toeExp, toeGot);
         }
         else
         {
            testFramework.assert(
               gpstk::EngNav::getNMCTValidity(
                  sf2, howWeek, aodoGot, tnmctGot, toeGot, totGot),
               "Unexpected return value", __LINE__);
            TUASSERTE(uint32_t, aodoExp, aodoGot);
            TUASSERTE(gpstk::CommonTime, tnmctExp, tnmctGot);
            TUASSERTE(gpstk::CommonTime, toeExp, toeGot);
         }
      }
      testFramework.assert(gotData, "Did not test any NMCT data", __LINE__);
         
      TURETURN();
   }


private:
   double eps;
   double b10;
};


int main() //Main function to initialize and run all tests above
{
   EngNav_T testClass;
   unsigned errorTotal = 0;

   errorTotal += testClass.computeParityTest();
   errorTotal += testClass.fixParityTest();
   errorTotal += testClass.getHOWTimeTest();
   errorTotal += testClass.getSFIDTest();
   errorTotal += testClass.checkParityTest();
   errorTotal += testClass.getSubframePatternTest();
   errorTotal += testClass.subframeConvertTest();
   errorTotal += testClass.nmctValidityTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal
        << endl;

   return errorTotal;
}
