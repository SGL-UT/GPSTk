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
#include <math.h> 
#include <iostream>

class EngNav_T
{
    public: 
	EngNav_T() // Default Constructor, set the precision value
	{
		eps = 1E-10; // lower precision value, accuracy of some values is lost in binary conversion
		b10 = 10;
	}
	~EngNav_T() {} // Default Desructor


	int getSubframePatternTest(void)
	{
		TestUtil testFramework("EngNav", "getSubframePattern", __FILE__, __LINE__);
		std::string testMesg;

		const uint32_t subframe1P[10] = {0x22c000e4, 0x215ba160, 0x00180012, 0x1fffffc0, 0x3fffffc3,
							  			 0x3ffffffc, 0x3fffc009, 0x16d904f0, 0x003fdbac, 0x247c139c};
		const uint32_t subframe2P[10] = {0x22c000e4, 0x215bc2f0, 0x16c2eb4d, 0x032c41a0, 0x26abc7e0,
							  			 0x0289c0dd, 0x0d5ecc38, 0x036b6842, 0x034f4df0, 0x1904c0b4};
		const uint32_t subframe3P[10] = {0x22c000e4, 0x215be378, 0x3ffcc344, 0x1a8441f1, 0x3ff80b74, 
							  			 0x1c8deb5e, 0x0a34d52d, 0x14a5013e, 0x3fee8c2f, 0x16c35c80};

		testMesg = "Subframe Pattern obtained was incorrect";
		testFramework.assert(gpstk::EngNav::getSubframePattern(subframe1P) == 1, testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::getSubframePattern(subframe2P) == 2, testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::getSubframePattern(subframe3P) == 3, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int computeParityTest(void)
	{
		TestUtil testFramework("EngNav", "Compute Parity", __FILE__, __LINE__);
		std::string testMesg;

		//data taken from http://www.gpscreations.com/NewFiles/GPS%20Parity%20Checking.pdf

		//Feed in 30bit word with 0's as the parity
		uint32_t zero =  0x00000000;
		uint32_t data1 = 0x22C000C0;
		uint32_t data2 = 0x17344000;
		uint32_t data3 = 0x2142EF00;
		uint32_t data4 = 0x15E67180;

		testMesg = "Parity computed was incorrect";
		testFramework.assert(gpstk::EngNav::computeParity(data1, zero) == 0x24, testMesg, __LINE__);

		data1 |= 0x24;

		testFramework.assert(gpstk::EngNav::computeParity(data2, data1) == 0x22, testMesg, __LINE__); 

		data2 |= 0x22;

		testFramework.assert(gpstk::EngNav::computeParity(data3, data2) == 0x1B, testMesg, __LINE__); 

		data3 |= 0x1B;

		testFramework.assert(gpstk::EngNav::computeParity(data4, data3, false) == 0x02, testMesg, __LINE__);
		
		return testFramework.countFails();
	}

	int fixParityTest(void)
	{
		TestUtil testFramework("EngNav", "Fix Parity", __FILE__, __LINE__);
		std::string testMesg;

		//3 cases of regular parity computation
		uint32_t data1 = 0x22C000C0; 
		uint32_t data2 = 0x17344000;
		uint32_t data3 = 0x2142EF00;
		uint32_t data4 = 0x15E67180;
		//test word with the non-informational parity bits, set to 0
		uint32_t data5 = 0x32098100; //taken from EngEphemeris
		uint32_t CompareData1 = 0x22C000C0 | 0x0000024;
		uint32_t CompareData2 = 0x17344000 | 0x0000022;
		uint32_t CompareData3 = 0x2142EF00 | 0x000001B;
		uint32_t CompareData4 = 0x15E67180 | 0x0000002;
		//non-informational parity bits included in this
		uint32_t CompareData5 = 0x32098100 | 0x00000DC;

		//(word to overwrite with parity, previous word, add 2 parity computation bits(word 2 & 10))
		testMesg = "Parity computed is incorrect";
		testFramework.assert(gpstk::EngNav::fixParity(data1, 0, false) == CompareData1, testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::fixParity(data2, CompareData1, false) == CompareData2, testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::fixParity(data3, CompareData2, false) == CompareData3, testMesg, __LINE__);
//===============================================================
//	Test below FAILS!
//
//	fixParity method doesn't compliment the sf word when D*30 is 1
//	computeParity does this, but can't calculate non-information
//	parity bits for words 2 and 10

//	Why do we have two different functions which each do half the
//	work they need to? If fixParity is fixed for the compliment issue above,
//	why ever use compute parity instead of fix parity? They do the same thing
//================================================================
		testFramework.assert(gpstk::EngNav::fixParity(data4, CompareData3, false) == CompareData4, testMesg, __LINE__);

		testFramework.assert(gpstk::EngNav::fixParity(data5, 0, true) == CompareData5, testMesg, __LINE__);

		return testFramework.countFails();
	}
	int checkParityTest(void)
	{
		TestUtil testFramework("EngNav", "Check Parity", __FILE__, __LINE__);
		std::string testMesg;

		//Data is from EngEphemeris addSubframe test

		const uint32_t subframe1P[10] = {0x22c000e4, 0x215ba160, 0x00180012, 0x1fffffc0, 0x3fffffc3, 
										 0x3fffffff, 0x3fffc035, 0x16d904f3, 0x003fdb90, 0x247c1339};
		const uint32_t subframe2P[10] = {0x22c000e4, 0x215bc2f0, 0x16c2eb4d, 0x032c41a3, 0x26abc7dc,
										 0x0289c0dd, 0x0d5ecc3b, 0x0036b67f, 0x034f4de5, 0x1904c0a1};
		const uint32_t subframe3P[10] = {0x22c000e4, 0x215be378, 0x3ffcc344, 0x1a8441f1, 0x3ff80b61,
										 0x1c8deb4b, 0x0a34d530, 0x14a50138, 0x3fee8c2f, 0x16c35c83};

		testMesg = "Parity computed is incorrect";
		testFramework.assert(gpstk::EngNav::checkParity(subframe1P, false), testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::checkParity(subframe2P, false), testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::checkParity(subframe3P, false), testMesg, __LINE__);
		
		return testFramework.countFails();
	}

	int getHOWTimeTest(void)
	{
		//wrong, fix later
		TestUtil testFramework("EngNav", "getHOWTime", __FILE__, __LINE__);
		std::string testMesg;

		uint32_t how1 = 0x215ba160;
		uint32_t how2 = 0x215bc2f0;
		uint32_t how3 = 0x215be378;

		testMesg = "Returned TOW time from the HOW is incorrect";
		testFramework.assert(gpstk::EngNav::getHOWTime(how1) == 409902, testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::getHOWTime(how2) == 409908, testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::getHOWTime(how3) == 409914, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int getSFIDTest(void)
	{
		TestUtil testFramework("EngNav", "getSFID", __FILE__, __LINE__);
		std::string testMesg;

		uint32_t how1 = 0x215ba160;
		uint32_t how2 = 0x215bc2f0;
		uint32_t how3 = 0x215be378;

		testMesg = "Returned subframe ID was incorrect";
		testFramework.assert(gpstk::EngNav::getSFID(how1) == 1, testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::getSFID(how2) == 2, testMesg, __LINE__);
		testFramework.assert(gpstk::EngNav::getSFID(how3) == 3, testMesg, __LINE__);

		return testFramework.countFails();
	}
	
	//converts subframe binary data to FIC. 
	int subframeConvertTest(void) //calls getsubframePattern and convertQuant
	{
		TestUtil testFramework("EngNav", "Subframe Convert", __FILE__, __LINE__);
		std::string testMesg;

		double output1[60], output2[60], output3[60];
		const uint32_t subframe1P[10] = {0x22c000e4, 0x215ba160, 0x00180012, 0x1fffffc0, 0x3fffffc3,
							  			 0x3ffffffc, 0x3fffc009, 0x16d904f0, 0x003fdbac, 0x247c139c};
		const uint32_t subframe2P[10] = {0x22c000e4, 0x215bc2f0, 0x16c2eb4d, 0x032c41a0, 0x26abc7e0,
							  			 0x0289c0dd, 0x0d5ecc38, 0x036b6842, 0x034f4df0, 0x1904c0b4};
		/*const uint32_t subframe3P[10] = {0x22c000c0, 0x215be300, 0x3ffcc340, 0x1a8441c0, 0x3ff80b40, 
							  			 0x1c8deb40, 0x0a34d500, 0x14a50100, 0x3fee8c00, 0x16c35c00};*/
		const uint32_t subframe3P[10] = {0x22c000e4, 0x215be378, 0x3ffcc344, 0x1a8441f1, 0x3ff80b76, 
							  			 0x1c8deb5e, 0x0a34d52d, 0x14a5013e, 0x3fee8c2f, 0x16c35c80};
		gpstk::EngNav EngNavThing;

		testMesg = "Subframe Convert function failed";
		testFramework.assert(EngNavThing.subframeConvert(subframe1P, 1025, output1), testMesg, __LINE__);
		
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
		testFramework.assert(output1[9]/2048 == 0x5B, testMesg, __LINE__); //AODC to IODC conversion, pg 15 of GR-SGL-99-14 FIC Definiton file
		testMesg = "L2 code flag is incorrect";
		testFramework.assert(output1[10] == 0, testMesg, __LINE__);
		testMesg = "Group Delay Differential is incorrect";
		testFramework.assert(output1[11] == 0, testMesg, __LINE__);
		testMesg = "Clock Epoch is incorrect";
		testFramework.assert(output1[12] == 409904, testMesg, __LINE__);
		testMesg = "Clock Drift Rate is incorrect";
		testFramework.assert(output1[13] == 0, testMesg, __LINE__);
		testMesg = "Clock Drift is incorrect";
		testFramework.assert(abs(output1[14] + .165982783074E-10)*pow(b10,10) < eps, testMesg, __LINE__);
		testMesg = "Clock Bias is incorrect";
		testFramework.assert(abs(output1[15] + .839701388031E-03)*pow(b10,3) < eps, testMesg, __LINE__);

		testMesg = "Subframe Convert function failed";
		testFramework.assert(EngNavThing.subframeConvert(subframe2P, 1025, output2), testMesg, __LINE__);

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
		testFramework.assert(output2[5]/2048 == 91, testMesg, __LINE__); //AODE to IODE conversion, pg 15 of GR-SGL-99-14 FIC Definiton file
		testMesg = "CRS is incorrect";
		testFramework.assert(abs(output2[6] - 93.40625) < eps, testMesg, __LINE__);
		testMesg = "Correction to Mean Motion is incorrect";
		testFramework.assert(abs(output2[7] - (.11604054784E-8))*pow(b10,8) < eps, testMesg, __LINE__);
		testMesg = "Mean Anomaly at Epoch is incorrect";
		testFramework.assert(abs(output2[8] - 0.162092304801) < eps, testMesg, __LINE__);		
		testMesg = "CUC is incorrect";
		testFramework.assert(abs(output2[9] - .484101474285E-5)*pow(b10,5) < eps, testMesg, __LINE__);
		testMesg = "Eccentricity is incorrect";
		testFramework.assert(abs(output2[10] - .626740418375E-2)*pow(b10,2) < eps, testMesg, __LINE__);
		testMesg = "CUS is incorrect";
		testFramework.assert(abs(output2[11] - .652112066746E-5)*pow(b10,5) < eps, testMesg, __LINE__);
		testMesg = "Square Root of Semi-Major Axis is incorrect";
		testFramework.assert(abs(output2[12] - .515365489006E4)*pow(b10,-4) < eps, testMesg, __LINE__);
		testMesg = "Time of Epoch is incorrect";
		testFramework.assert(output2[13] == 409904, testMesg, __LINE__);
		testMesg = "Fit interval flag is incorrect";
		testFramework.assert(output2[14] == 0, testMesg, __LINE__);

		testMesg = "Subframe Convert function failed";
		testFramework.assert(EngNavThing.subframeConvert(subframe3P, 1025, output3), testMesg, __LINE__);

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
		testFramework.assert(abs(output3[5] + .242143869400E-7)*pow(b10,7) < eps, testMesg, __LINE__);
		testMesg = "Right ascension of ascending node is incorrect";
		testFramework.assert(abs(output3[6] - .329237003460) < eps, testMesg, __LINE__);
		testMesg = "CIS is incorrect";
		testFramework.assert(abs(output3[7] + .596046447754E-7)*pow(b10,7) < eps, testMesg, __LINE__);		
		testMesg = "Inclination is incorrect";
		testFramework.assert(abs(output3[8] - 1.11541663136) < eps, testMesg, __LINE__);
		testMesg = "CRC is incorrect";
		testFramework.assert(abs(output3[9] - 326.59375)*pow(b10, -3) < eps, testMesg, __LINE__);
		testMesg = "Arguement of perigee is incorrect"; // All other values needed to be converted to semi-circles, IDK why this one wasn't
		testFramework.assert(abs(output3[10] - 2.06958726335)*pow(b10, -1) < eps, testMesg, __LINE__);
		testMesg = "Right ascension of ascending node time derivative is incorrect";
		testFramework.assert(abs(output3[11] + .638312302555E-8)*pow(b10,10) < eps, testMesg, __LINE__);
		testMesg = "AODE? is incorrect";
		testFramework.assert(output3[12]/2048 == 91, testMesg, __LINE__); 
		testMesg = "Inclination time derivative is incorrect";
		testFramework.assert(abs(output3[13] - .307155651409E-9)*pow(b10,9) < eps, testMesg, __LINE__);//AODE to IODE conversion, pg 15 of GR-SGL-99-14 FIC Definiton file	

		return testFramework.countFails();
	}

	private:
	double eps;
	double b10;
};


int main() //Main function to initialize and run all tests above
{
	EngNav_T testClass;
	int check, errorCounter = 0;

	check = testClass.computeParityTest();
	errorCounter += check;

	check = testClass.fixParityTest();
	errorCounter += check;

	check = testClass.getHOWTimeTest();
	errorCounter += check;

	check = testClass.getSFIDTest();
	errorCounter += check;

	check = testClass.checkParityTest();
	errorCounter += check;

	check = testClass.getSubframePatternTest();
	errorCounter += check;

	check = testClass.subframeConvertTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
