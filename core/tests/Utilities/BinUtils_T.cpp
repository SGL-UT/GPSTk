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
#include "TestUtil.hpp"
#include "BinUtils.hpp"
#include "Exception.hpp"
#include <iostream>
#include <cmath>

class BinUtils_T
{
	public:

	//==========================================================
	//	Test Suite: twiddleTest()
	//==========================================================
	//
	//	Stores hex values as every type in big & little endian,
	//	then verifies that twiddle reverses byte order
	//
	//==========================================================
	int twiddleTest(void)
	{
		TestUtil testFramework("BinUtils", "twiddle", __FILE__, __LINE__);

		//std::cout.setf(std::ios_base::hex,std::ios_base::basefield);
		
		unsigned char Store[16] = {0x11,0x33,0x77,0xFF,0xBB,0x22,0xDD,0x44,0x00,0x00,0xDD,0xAA,0x44,0x22,0xBB,0xAA};
		
		char a;
		unsigned char *ptra = (unsigned char *)& a;
		for (int i = sizeof(char)-1;i>=0;i--)
		{
			*ptra = Store[i];
			*ptra++;
		}
		
		short b;
		unsigned char *ptrb = (unsigned char *)& b;
		for (int i = sizeof(short)-1;i>=0;i--)
		{
			*ptrb = Store[i];
			*ptrb++;
		}
		
		int c;
		unsigned char *ptrc = (unsigned char *)& c;
		for (int i = sizeof(int)-1;i>=0;i--)
		{
			*ptrc = Store[i];
			*ptrc++;
		}
		
		long d;
		unsigned char *ptrd = (unsigned char *)& d;
		for (int i = sizeof(long)-1;i>=0;i--)
		{
            *ptrd = Store[i];
            *ptrd++;
		}
		
		float e;
		unsigned char *ptre = (unsigned char *)& e;
		for (int i = sizeof(float)-1;i>=0;i--)
		{
			*ptre = Store[i];
			*ptre++;
		}
		
		double f;
		unsigned char *ptrf = (unsigned char *)& f;
		for (int i = sizeof(double)-1;i>=0;i--)
		{
			*ptrf = Store[i];
			*ptrf++;
		}
		
		long double g;
		unsigned char *ptrg = (unsigned char *)& g;
		for (int i = sizeof(long double)-1;i>=0;i--)
		{
			*ptrg = Store[i];
			*ptrg++;

		}

		char a1;
		unsigned char *ptra1 = (unsigned char *)& a1;
		for (int i =0;i<sizeof(char);i++)
		{
			*ptra1 = Store[i];
			*ptra1++;
		}
		
		short b1;
		unsigned char *ptrb1 = (unsigned char *)& b1;
		for (int i =0;i<sizeof(short);i++)
		{
			*ptrb1 = Store[i];
			*ptrb1++;
		}
		
		int c1;
		unsigned char *ptrc1 = (unsigned char *)& c1;
		for (int i =0;i<sizeof(int);i++)
		{
			*ptrc1 = Store[i];
			*ptrc1++;
		}
		
		long d1;
		unsigned char *ptrd1 = (unsigned char *)& d1;
		for (int i =0;i<sizeof(long);i++)
		{
			*ptrd1 = Store[i];
			*ptrd1++;
		}
		
		float e1;
		unsigned char *ptre1 = (unsigned char *)& e1;
		for (int i =0;i<sizeof(float);i++)
		{
			*ptre1 = Store[i];
			*ptre1++;
		}
		
		double f1;
		unsigned char *ptrf1 = (unsigned char *)& f1;
		for (int i =0;i<sizeof(double);i++)
		{
			*ptrf1 = Store[i];
			*ptrf1++;
		}
		
		long double g1;
		unsigned char *ptrg1 = (unsigned char *)& g1;
		for (int i =0;i<sizeof(long double);i++)
		{
			*ptrg1 = Store[i];
			*ptrg1++;
		}


//Testing twiddle for little Endian

		gpstk::BinUtils::twiddle(a);
		gpstk::BinUtils::twiddle(b);
		gpstk::BinUtils::twiddle(c);
		gpstk::BinUtils::twiddle(d);
		gpstk::BinUtils::twiddle(e);
		gpstk::BinUtils::twiddle(f);
		gpstk::BinUtils::twiddle(g);


		std::string failMesg;
		failMesg = "The twiddle method was not successful";
		testFramework.assert(a1 == a, failMesg, __LINE__);
		testFramework.assert(b1 == b, failMesg, __LINE__);
		testFramework.assert(c1 == c, failMesg, __LINE__);
		testFramework.assert(d1 == d, failMesg, __LINE__);
		testFramework.assert(std::abs(e1-e) < 1e-50, failMesg, __LINE__);
		testFramework.assert(std::abs(f1-f) < 1e-50, failMesg, __LINE__);
		testFramework.assert(std::abs(g1-g) < 1e-50, failMesg, __LINE__);

		gpstk::BinUtils::twiddle(a);
		gpstk::BinUtils::twiddle(b);
		gpstk::BinUtils::twiddle(c);
		gpstk::BinUtils::twiddle(d);
		gpstk::BinUtils::twiddle(e);
		gpstk::BinUtils::twiddle(f);
		gpstk::BinUtils::twiddle(g);

		gpstk::BinUtils::twiddle(a1);
		gpstk::BinUtils::twiddle(b1);
		gpstk::BinUtils::twiddle(c1);
		gpstk::BinUtils::twiddle(d1);
		gpstk::BinUtils::twiddle(e1);
		gpstk::BinUtils::twiddle(f1);
		gpstk::BinUtils::twiddle(g1);

//Testing twiddle for big Endian

		testFramework.assert(a1 == a, failMesg, __LINE__);
		testFramework.assert(b1 == b, failMesg, __LINE__);
		testFramework.assert(c1 == c, failMesg, __LINE__);
		testFramework.assert(d1 == d, failMesg, __LINE__);
		testFramework.assert(std::abs(e1-e) < 1e-50, failMesg, __LINE__);
		testFramework.assert(std::abs(f1-f) < 1e-50, failMesg, __LINE__);
		testFramework.assert(std::abs(g1-g) < 1e-50, failMesg, __LINE__);		

		return testFramework.countFails();
	}

	//==========================================================
	//	Test Suite: decodeVarTest()
	//==========================================================
	//
	//	Tests if item was removed from string and output bytes
	//	are in host byte order
	//
	//==========================================================
	int decodeVarTest(void)
	{
		TestUtil testFramework("BinUtils", "decodeVar", __FILE__, __LINE__);

		//std::cout.setf(std::ios_base::hex,std::ios_base::basefield);

		std::string stringTest = "Random";
		//char out =
        gpstk::BinUtils::decodeVar<char>(stringTest);
		
		std::string failMesg;

		failMesg = "The method did not remove the first byte of the string";
		testFramework.assert(stringTest == "andom", failMesg, __LINE__);

		std::string stringTest0 = "Random";
		char out0 = gpstk::BinUtils::decodeVar<char>(stringTest0, 0);

		failMesg = "The method did not parse the proper value";
		testFramework.assert(out0 == 0x52, failMesg, __LINE__);

		std::string stringTest1 = "I am 5000.";
		int out1 = gpstk::BinUtils::decodeVar<int>(stringTest1, 5);

		unsigned iexpected = 0x35303030;  // '5000' as ascii bytes
		testFramework.assert(out1 == iexpected, failMesg, __LINE__); // '5000'

		std::string stringTest2 = "The word 'this' should be read";
		float out2 = gpstk::BinUtils::decodeVar<float>(stringTest2, 10);

		unsigned bytes = 0x74686973;  // 'this' as ascii bytes
		float fexpected = *(float*)&bytes;  // interpret bytes as a float
		testFramework.assert(std::abs(out2 - fexpected) < 1e-12, failMesg, __LINE__);

		return testFramework.countFails();
	}


	//==========================================================
	//	Test Suite: encodeVarTest()
	//==========================================================
	//
	//	Tests if bytes are in network byte order
	//
	//==========================================================
	int encodeVarTest(void)
	{
		TestUtil testFramework("BinUtils", "encodeVar", __FILE__, __LINE__);

		char test1 = 'H';
		std::string stringTest1 = gpstk::BinUtils::encodeVar<char>(test1);

		std::string failMesg;

		failMesg = "The method did not output the correct string";
		testFramework.assert(stringTest1 == "H", failMesg, __LINE__);

		int test2 = 0x41424344; // "ABCD"
		std::string stringTest2 = gpstk::BinUtils::encodeVar<int>(test2);

//Should be same string coming out, unless it's a little endian system.
//		If so, byte order reversed		
		testFramework.assert(stringTest2 == "ABCD", failMesg, __LINE__);

		float test3 = 0x4D336C316F; // "M3l10"
		std::string stringTest3 = gpstk::BinUtils::encodeVar<float>(test3);

//Should be same string coming out, unless it's a little endian system.
//		If so, byte order reversed		
		testFramework.assert(stringTest3 == "\x52\x9A\x66\xD8", failMesg, __LINE__);

		return testFramework.countFails();
	}

	//====================================================================
	//	Test Suite: computeCRCTest()
	//====================================================================	
	//
	//	Tests if computeCRC is generating valid Cyclic Redundancy Checks
	//
	//	WARNING! Current method is UNVERIFIED! Unable to replicate
	//	computeCRCTest output. Potential bug in this method
	//
	//=====================================================================
	int computeCRCTest(void)
	{
		TestUtil testFramework("BinUtils", "computeCRC", __FILE__, __LINE__);
		std::string failMesg;

//====================================================================================
//
//		Using http://www.zorc.breitbandkatze.de/crc.html for generating comparison for 
//		CRCs, in addition to writing out the calculations by hand
//
//====================================================================================

//initially test basic crc computation
//verified by hand-written calculation

		gpstk::BinUtils::CRCParam params(1, 0x1, 0, 0, true, false, false);

		int data = 0xc;

		int * pdata = &data;

//Same syntax for the computeCRC method as used in BinData.cpp 
		int crc = gpstk::BinUtils::computeCRC((const unsigned char *)pdata,4,params);
		std::cout<<"1: "<<crc<<std::endl;
		//1
		failMesg = "CRC computation with user defined parameters on 4 bits";	
		testFramework.assert(crc == 0, failMesg, __LINE__);

//test crc computation on 1 ASCII char

//using user-generated parameters, all 4th order polynomials

//polynomial is written without leading '1' bit,
//			i.e. 0xD (0b1101) used in computeCRC() as 0x5 (0b101)

		gpstk::BinUtils::CRCParam params1(3, 0x2, 0, 0, true, false, false); //0xb polynomial
		int data1 = 0x72;

		pdata = &data1;

		int crc1 = gpstk::BinUtils::computeCRC((const unsigned char *)pdata,8,params1);
		std::cout<<"2: "<<crc1<<std::endl;
		failMesg = "CRC computation with user defined parameters on 8 bits. NOTE: CRC COMPUTATION IS NOT VERIFIED";	
		testFramework.assert(crc1 == 6, failMesg, __LINE__);



//Now reflecting input data bytes
//(defined in each instance of CRCParam)


//Below should be 0 according to online calculator and hand-calculations
//------------------------------------------------------------------------------------------
		gpstk::BinUtils::CRCParam params2(3, 0x6, 0, 0, true, true, false); //0xe polynomial
		int data2 = 0x48;

		pdata= &data2;

		int crc2 = gpstk::BinUtils::computeCRC((const unsigned char *) pdata,8,params2);
		std::cout<<"2.1: "<<crc2<<std::endl;			
		testFramework.assert(crc2 == 0, failMesg, __LINE__);

//Below should be 5 according to online calculator and hand-calculations
//-----------------------------------------------------------------------------------
		gpstk::BinUtils::CRCParam params3(3, 0x5, 0, 0, true, true, false); //0xd polynomial
		int data3 = 0x59;//"Y"

		pdata = &data3;


		int crc3 = gpstk::BinUtils::computeCRC((const unsigned char *)pdata,8,params3);
		std::cout<<"2.2: "<<crc3<<std::endl;	
		testFramework.assert(crc3 == 5, failMesg, __LINE__);

//Below should be 7 according to online calculator and hand-calculations
//-----------------------------------------------------------------------------------------

		gpstk::BinUtils::CRCParam params17(3, 0x7, 0, 0 , true, true, false); //0xf polynomial
		int data17 = 0x61;

		pdata = &data17;

		int crc17 = gpstk::BinUtils::computeCRC((const unsigned char *)pdata,8,params17);
		std::cout<<"2.25: "<<crc17<<std::endl;
		testFramework.assert(crc17 == 7, failMesg, __LINE__);


//Online calculator and hand-calculations are not consistent, unknown correct value
//--------------------------------------------------------------------------------------
		gpstk::BinUtils::CRCParam params4(3, 0x3, 0, 0, true, true, false); //0xb polynomial
		int data4 = 0x94;//""

		pdata = &data4;

		int crc4 = gpstk::BinUtils::computeCRC((const unsigned char *)pdata,8,params4);
		std::cout<<"2.3: "<<crc4<<std::endl;	
		testFramework.assert(crc4 == 1000, failMesg, __LINE__);

//Online calculator and hand-calculations are not consistent, unknown correct value
//---------------------------------------------------------------------------
		gpstk::BinUtils::CRCParam params5(3, 0x5, 0, 0, true, true, false); //0xd polynomial
		int data5 = 0xA4;//""

		pdata = &data5;

		int crc5 = gpstk::BinUtils::computeCRC((const unsigned char *)pdata,8,params5);
		std::cout<<"2.4: "<<crc5<<std::endl;	
		testFramework.assert(crc5 == 2, failMesg, __LINE__);


//=====================================================================================
//testing with defined parameters, still using online calculator for comparison.
//		Calculations below have NOT been verified by hand

		//CRCCCITT
		int data6 = 0x78ab20f1;
		pdata = &data6;
		int crc6 = gpstk::BinUtils::computeCRC((const unsigned char *)pdata,32,gpstk::BinUtils::CRCCCITT);
		std::cout<<"3: "<<std::hex<<crc6<<std::endl;
		failMesg = "CRC computation with CRCCCITT parameters on 32 bits. NOTE: CRC COMPUTATION IS NOT VERIFIED";			
		testFramework.assert(crc6 == 0xF7C3,failMesg, __LINE__);

		//CRC16
		int data7 = 0x56c8913a;
		pdata = &data7;
		int crc7 = gpstk::BinUtils::computeCRC((const unsigned char *)pdata,32,gpstk::BinUtils::CRC16);
		std::cout<<"3.1: "<<std::hex<<crc7<<std::endl;
		failMesg = "CRC computation with CRC16 parameters on 32 bits. NOTE: CRC COMPUTATION IS NOT VERIFIED";				
		testFramework.assert(crc7 == 0xF57D, failMesg, __LINE__);

		//CRC32
		int data8 = 0x387a13b5;
		pdata = &data8;
		int crc8 = gpstk::BinUtils::computeCRC((const unsigned char *)pdata,32,gpstk::BinUtils::CRC32);
		std::cout<<"3.2: "<<std::hex<<crc8<<std::endl;
		failMesg = "CRC computation with CRC32 parameters on 32 bytes. NOTE: CRC COMPUTATION IS NOT VERIFIED";				
		testFramework.assert(crc8 == 0x9759E445, failMesg, __LINE__);

		//CRC24Q
		//unable to use CRC24Q in online calculator for comparison. Final XOR value is
		//			returned as "invalid"
		//Is the CRCParam CRC24Q able to be used successfully?

		return testFramework.countFails();
	}

	//==========================================================
	//	Test Suite: xorChecksumTest()
	//==========================================================
	//
	//	Computes xorChecksums with 1, 2 and 3 words for chars,
	//	shorts, and ints
	//
	//==========================================================
	int xorChecksumTest(void)
	{
		TestUtil testFramework("BinUtils", "xorChecksum", __FILE__, __LINE__);

//=====================================================================================
//
//		Below try/catch block doesn't catch gpstk::Exception.
//		gpstk::InvalidParameter seems to terminate program regardless if I catch it or not
/*
		try {gpstk::BinUtils::xorChecksum<int>("Hello");testFramework.failTest("The word 'Hello' should of an improper length for an int xor checksum");}
		catch(gpstk::Exception e) {testFramework.passTest();}
		catch(...) {testFramework.failTest();}	
*/
//
//
//		ALSO, all tests below depend on constant lengths of the data types to parse
//			the string correctly. Is there another way around this?
//
//=====================================================================================

		std::string failMesg;
 		
 		failMesg = "The checksum for a single word char was not computed correctly";
		char xc = gpstk::BinUtils::xorChecksum<char>("7");
		testFramework.assert(xc == '7', failMesg, __LINE__);

		char xc1 = gpstk::BinUtils::xorChecksum<char>("Bc");
		failMesg = "The checksum for a double word char was not computed correctly";
		testFramework.assert(xc1 == '!', failMesg, __LINE__);

		char xc2 = gpstk::BinUtils::xorChecksum<char>("P/Q");
		failMesg = "The checksum for a triple word char was not computed correctly";
		testFramework.assert(xc2 == '.', failMesg, __LINE__);

//=====================================================================================
//
//		Reverses the order of bits in xorChecksum. Is this desired?
//
//=====================================================================================

		short xc3 = gpstk::BinUtils::xorChecksum<short>("mn");
		failMesg = "The checksum for a single word short was not computed correctly";		
		testFramework.assert(xc3 == 0x6E6D, failMesg, __LINE__); //'nm'

		short xc4 = gpstk::BinUtils::xorChecksum<short>("59WZ");
		failMesg = "The checksum for a double word short was not computed correctly";		
		testFramework.assert(xc4 == 0x6362, failMesg, __LINE__); // 'cb'

		short xc5 = gpstk::BinUtils::xorChecksum<short>("am+*09");
		failMesg = "The checksum for a triple word short was not computed correctly";		
		testFramework.assert(xc5 == 0x7E7A, failMesg, __LINE__); // '~z'

		int xc6 = gpstk::BinUtils::xorChecksum<int>("97Bg");
		failMesg = "The checksum for a single word int was not computed correctly";		
		testFramework.assert(xc6 == 0x67423739, failMesg, __LINE__); // 'gB79'

		int xc7 = gpstk::BinUtils::xorChecksum<int>("ABCD!#$%");
		failMesg = "The checksum for a double word int was not computed correctly";		
		testFramework.assert(xc7 == 0x61676160, failMesg, __LINE__); // 'aga`'

		int xc8 = gpstk::BinUtils::xorChecksum<int>("+a0.ehZ64xYN");
		failMesg = "The checksum for a triple word int was not computed correctly";		
		testFramework.assert(xc8 == 0x5633717A, failMesg, __LINE__); // 'V3qz'

		return testFramework.countFails();

	}

	//==========================================================
	//	Test Suite: countBitsTest()
	//==========================================================
	//
	//	Counts the number of set bits in 32 bit unsigned int
	//
	//==========================================================
	int countBitsTest(void)
	{
		TestUtil testFramework("BinUtils", "countBits", __FILE__, __LINE__);
		std::string failMesg;

		failMesg  = "The number of bits counted is incorrect";
		testFramework.assert(gpstk::BinUtils::countBits(5) == 2, failMesg, __LINE__);

//testing if bit count is constant in a left-shift operation		
		testFramework.assert(gpstk::BinUtils::countBits(10) == 2, failMesg, __LINE__);

		failMesg = "The number of bits counted is incorrect after left-shift operation";
		testFramework.assert(gpstk::BinUtils::countBits(20) == 2, failMesg, __LINE__);

//same but for right bit
		failMesg = "The number of bits counted is incorrect";

		testFramework.assert(gpstk::BinUtils::countBits(16) == 1, failMesg, __LINE__);

		failMesg = "The number of bits counted is incorrect after a right-shift operation";		
		testFramework.assert(gpstk::BinUtils::countBits(8) == 1, failMesg, __LINE__);

//random case
		failMesg = "The number of bits counted is incorrect";		
		testFramework.assert(gpstk::BinUtils::countBits(15) == 4, failMesg, __LINE__);

		return testFramework.countFails();	
	}

};
	

int main (void)
{

//=====================================================================================
//
//	Not testing inteltoHost, host to intel
//		They only ensure the byte order is little-endian
//
//	Same for netToHost, or hostToNet, but they ensure byte order is big-endian
//
//=====================================================================================

	int check = 0, errorCounter = 0;
	BinUtils_T testClass;

	check = testClass.twiddleTest();
	errorCounter += check;

	check = testClass.decodeVarTest();
	errorCounter += check;

	check = testClass.encodeVarTest();
	errorCounter += check;

	check = testClass.computeCRCTest(); //unable to verify, see comments in computeCRCTest
	errorCounter += check;

	check = testClass.xorChecksumTest(); //dependent on size of variables, may fail on other platforms
	errorCounter += check;

	check = testClass.countBitsTest();
	errorCounter += check;	

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
