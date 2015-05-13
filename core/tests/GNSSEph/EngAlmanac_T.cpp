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

#include "EngAlmanac.hpp"
#include "TestUtil.hpp"
#include <iostream>

#include "AlmanacDataGenerator.hpp"

class EngAlmanac_T
{
    public:
    //Reads in AlmanacData object to form comparison data types
	EngAlmanac_T(AlmanacData iAData, AlmanacSubframes iASubframes)
	{ 
		eps = 1E-6; //Low precision, data is extracted from binary subframes, some have 16 bit precision
		aData = iAData;
		aSubframes = iASubframes;

	}
	
	~EngAlmanac_T() {} // Default Desructor


	int addSubframeTest(void)
	{
		TestUtil testFramework("EngAlmanac", "addSubframe", __FILE__, __LINE__);

		gpstk::EngAlmanac dataStore;
		//No parity on these subframes, EngAlmanac doesn't check for a parity

//Wrote a python script that reads in almanac data and converts to binary subframe strings
//Started porting to c++, will finish if time allows.
		const long subframe50[10] = {0x22c000e4, 0x00000598, 0x10488780, 0x13832280, 0x3f520000, 0x28433640, 0x303131c0, 0x03d0a340, 0x21541e00, 0x3fc00200};
		const long subframe51[10] = {0x22c000e4, 0x00000598, 0x109e0140, 0x13bfa740, 0x3f4e0000, 0x28437f00, 0x2fd06180, 0x28d89900, 0x25d33fc0, 0x12800800};
		const long subframe52[10] = {0x22c000e4, 0x00000598, 0x10c1de40, 0x1382c300, 0x3f52c000, 0x28434500, 0x3acb52c0, 0x24b44bc0, 0x355610c0, 0x0a401f00};
		const long subframe53[10] = {0x22c000e4, 0x00000598, 0x1116f780, 0x13bf7fc0, 0x3f4cc000, 0x28434380, 0x2ff84900, 0x0ac3bbc0, 0x1ebc60c0, 0x3f800200};
		const long subframe54[10] = {0x22c000e4, 0x00000598, 0x11487380, 0x1380ae00, 0x3f4fc000, 0x2842f9c0, 0x3ab9bb80, 0x03d9f840, 0x3edf6c80, 0x37c00f00};
		const long subframe55[10] = {0x22c000e4, 0x00000598, 0x11805fc0, 0x13831e00, 0x3f52c000, 0x28435e40, 0x301afdc0, 0x23307140, 0x324c86c0, 0x05001900};
		const long subframe56[10] = {0x22c000e4, 0x00000598, 0x11d0f140, 0x138484c0, 0x3f4ec000, 0x28435740, 0x10617ec0, 0x23ec1440, 0x1ad457c0, 0x0f000b00};
		const long subframe57[10] = {0x22c000e4, 0x00000598, 0x1240e540, 0x13828700, 0x3f570000, 0x28431fc0, 0x056a95c0, 0x1a38c600, 0x3022d0c0, 0x3b3ff100};
		const long subframe58[10] = {0x22c000e4, 0x00000598, 0x129e0d00, 0x13bfd540, 0x3f4f8000, 0x28433dc0, 0x3ae16300, 0x09760e80, 0x02756400, 0x3a400200};
		const long subframe59[10] = {0x22c000e4, 0x00000598, 0x12e02d80, 0x13b7e300, 0x3f454000, 0x28435300, 0x2ca23580, 0x0e440800, 0x1b728840, 0x2d3ffc00};
		const long subframe5a[10] = {0x22c000e4, 0x00000598, 0x130b6800, 0x1387b5c0, 0x3f550000, 0x28434b80, 0x1b4456c0, 0x05ed6740, 0x15678180, 0x09800900};
		const long subframe5b[10] = {0x22c000e4, 0x00000598, 0x134aa300, 0x13853e40, 0x3f5a0000, 0x284349c0, 0x0695d780, 0x153042c0, 0x22856740, 0x3c7ffa00};
		const long subframe5c[10] = {0x22c000e4, 0x00000598, 0x13907d80, 0x13840600, 0x3f584000, 0x28431940, 0x063c0240, 0x2bdf5600, 0x35c4eb40, 0x023ffd00};
		const long subframe5d[10] = {0x22c000e4, 0x00000598, 0x13ce9c40, 0x13be9980, 0x3f524000, 0x28432b80, 0x04fa3f00, 0x03fa5140, 0x2f366e40, 0x37fffc00};
		const long subframe5e[10] = {0x22c000e4, 0x00000598, 0x1410c640, 0x1387cd40, 0x3f554000, 0x284302c0, 0x1b750500, 0x02f72d40, 0x3fe93bc0, 0x3b800e00};
		const long subframe5f[10] = {0x22c000e4, 0x00000598, 0x1454b280, 0x1384e0c0, 0x3f5a4000, 0x28436f80, 0x25e93fc0, 0x2b858e00, 0x369e6800, 0x3a400600};
		const long subframe510[10] = {0x22c000e4, 0x00000598, 0x14a11280, 0x13bd2fc0, 0x3f4b4000, 0x284311c0, 0x3a98a5c0, 0x2bf76240, 0x05c43640, 0x0d000b00};
		const long subframe511[10] = {0x22c000e4, 0x00000598, 0x14d72e00, 0x13846540, 0x3f5a8000, 0x28435d40, 0x266abc00, 0x057cb3c0, 0x2ea2a900, 0x2f400200};
		const long subframe512[10] = {0x22c000e4, 0x00000598, 0x150b97c0, 0x13bd5f40, 0x3f4b4000, 0x28477840, 0x3a0fdd40, 0x0c65af40, 0x37788bc0, 0x0a400d00};
		const long subframe513[10] = {0x22c000e4, 0x00000598, 0x156e3080, 0x13be8580, 0x3f4cc000, 0x28430300, 0x2ff6e3c0, 0x2c837f80, 0x110142c0, 0x317fff00};
		const long subframe514[10] = {0x22c000e4, 0x00000598, 0x15902180, 0x13bcd480, 0x3f4b0000, 0x28433f40, 0x3a9bcb40, 0x2b1769c0, 0x015ca080, 0x0b400c00};
		const long subframe515[10] = {0x22c000e4, 0x00000598, 0x15d4ad80, 0x138110c0, 0x3f55c000, 0x28432840, 0x05818a00, 0x249cb240, 0x2a83af80, 0x3cbff800};
		const long subframe516[10] = {0x22c000e4, 0x00000598, 0x1606a140, 0x1381c8c0, 0x3f4b0000, 0x28433e80, 0x0ff7c740, 0x03ae2340, 0x22dbf140, 0x3e400600};
//start as 4 here, prn 8 is kabut	
		const long subframe42[10] = {0x22c000e4, 0x0000042c, 0x16496500, 0x1385d540, 0x3f520000, 0x28434780, 0x1acb6b00, 0x0740b7c0, 0x0e851e80, 0x3f3ffc00};
		const long subframe43[10] = {0x22c000e4, 0x0000042c, 0x16809280, 0x1382eb40, 0x3f4e0000, 0x28430780, 0x1ac5a340, 0x0bf5a480, 0x3bef1600, 0x00c00d00};
		const long subframe44[10] = {0x22c000e4, 0x0000042c, 0x16c4e700, 0x13840c80, 0x3f598000, 0x28434c40, 0x256d1000, 0x01e84c40, 0x37842980, 0x02400900};
		const long subframe45[10] = {0x22c000e4, 0x0000042c, 0x1727fcc0, 0x13879340, 0x3f558000, 0x28434e00, 0x1b830500, 0x2f0b2980, 0x3f1ed780, 0x0e400b00};
		const long subframe47[10] = {0x22c000e4, 0x0000042c, 0x1742b180, 0x13850380, 0x3f5a4000, 0x28437440, 0x26019d00, 0x39657600, 0x11fdfa80, 0x13c00e00};
		const long subframe48[10] = {0x22c000e4, 0x0000042c, 0x1782b580, 0x13823140, 0x3f4b4000, 0x28436240, 0x10e2f4c0, 0x1d069c80, 0x1c886f00, 0x3dc00e00};
		const long subframe49[10] = {0x22c000e4, 0x0000042c, 0x17d0a280, 0x13855080, 0x3f4fc000, 0x28437440, 0x10757bc0, 0x3abe3d00, 0x19242480, 0x0a400400};
		const long subframe4a[10] = {0x22c000e4, 0x0000042c, 0x18171000, 0x1380c400, 0x3f510000, 0x28436f40, 0x3ba16dc0, 0x00701380, 0x1fafa080, 0x3ac01a00};

		//Subframe and week number mod 1024
		testMesg = "Adding subframe 5 page 1 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[0], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 2 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[1], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 3 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[2], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 4 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[3], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 5 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[4], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 6 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[5], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 7 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[6], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 8 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[7], 819), testMesg, __LINE__);
		//skip this test, no valid almanac data for PRN 8
		// testMesg = "Adding subframe 5 page 9 failed";
		// testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[31], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 10 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[8], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 11 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[9], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 12 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[10], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 13 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[11], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 14 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[12], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 15 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[13], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 16 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[14], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 17 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[15], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 18 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[16], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 19 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[17], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 20 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[18], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 21 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[19], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 22 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[20], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 23 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[21], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 5 page 24 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[22], 819), testMesg, __LINE__);

		testMesg = "Adding subframe 4 page 2 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[23], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 4 page 3 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[24], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 4 page 4 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[25], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 4 page 5 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[26], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 4 page 7 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[27], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 4 page 8 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[28], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 4 page 9 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[29], 819), testMesg, __LINE__);
		testMesg = "Adding subframe 4 page 10 failed";
		testFramework.assert(dataStore.addSubframe(aSubframes.totalSf[30], 819), testMesg, __LINE__);

//subframe 5 page 25
/*
		copy the tlm and how from above for subframe 5
		dataID = 2 (from the IS-GPS-200)
		svID = 51 for this page
		toa = 319488 * 2**-12
		WNa = 819 % 256 = 51
		all sv healths are good except 8, set it to 111111 (or leave it blank, don't think it matters)

		word 3:
		dID svID (51) 	319488*2**-12 = 78 	Wn = 51
		10	110011		01001110 			00110011 = 10 1100 1101 0011 1000 1100 1100 0000
													2CD38CC0

		word 5:
		00 0000 		0000 00 		00 0000 		1111 11 		00 0000

		word 10 is reserved, doesn't matter what set to

*/
		const long subframe551[10] = {0x22c000e4, 0x00000598, 0x2CD38CC0, 0x00000000, 0x00000FC0,
									  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x11111110};
		testMesg = "Adding subframe 5 page 25 failed";
		testFramework.assert(dataStore.addSubframe(subframe551, 819), testMesg, __LINE__);

//subframe 4 page 25
/*
		copy the tlm and the how from above for subframe 4
		dataID = 2 (from the IS-GPS-200)
		svID = 63 for this page

		Assume reserved bits are 1, and all satellites have Anti spoofing on and are in block II//IIA/IIR
		therefore all the sv configs need to be set to 1001

		word 3
		dID 	svID 		4 svconfigs
		10 		1111 01 	10 0110 0110 0110 01 = 0x2F666640

		word 4,5,6,7
		6 sv configs
		10 0110 0110 0110 0110 0110 01 = 0x26666640

		word 8
		4 sv configs	 		2 reserved 	1 sv health
		10 0110 0110 0110 01 	11 			0000 00  	=	0x26667000

		word 9
		5 sv healths, all 0 = 0x00000000

		word 10
		3 sv healths 		 	4 reserved 	2 parity
		00 0000 0000 0000 0000 	1111 		00 			= 0x00000F00

*/
		const long subframe447[10] = {0x22c000e4, 0x0000042c, 0x2F666640, 0x26666640, 0x26666640,
									  0x26666640, 0x26666640, 0x26667000, 0x00000000, 0x00000F00};
		testMesg = "Adding subframe 4 page 25 failed";
		testFramework.assert(dataStore.addSubframe(subframe447, 819), testMesg, __LINE__);

// subframe4 page 18
/*
		copy tlm and how from other subframe4 pages
		data id = 2
		sv id = 56

		//deadbeefdeadc0de
		alpha0 = (222 - 256) * 2**-30 = -3.166496753692627e-08 //Twos compliment
		alpha1 = (173 - 256) * 2**-27 / pi = -1.968422573302286e-07
		alpha2 = (190 - 256) * 2**-24 / pi**2 = -3.985880685087617e-07
		alpha3 = (239 - 256) * 2**-24 / pi**3 = -3.26798013069863e-08
		beta0 = (222 - 256) * 2**11 = -69632 //Twos compliment
		beta1 = (173 - 256) * 2**14 / pi = -432860.7015445238
		beta2 = (192 - 256) * 2**16 / pi**2 = -424971.8458357919
		beta3 = (222 - 256) * 2**16 / pi**3 = -71863.64306088151

		//cabobobs
		A0 = (4294967296 - 163400577205) * 2**-30 = -148.17864625621587
		//baddab
		A1 = (12246443 - 16777216) * 2**-50 = -4.0241348031599955e-09
		dtLS = 13 = 0x0d
		tot = 110 * 2**12 = 450560 = 0x6e
		WNt = 90 = 0x5a
		WNLSF = 254 = 0xFE
		DN (right justified?) = 5 =>0xa0
		dtLSF = (153 - 256) = -103 = 0x99


		word 3
		dataID 	svID 	alpha0 		alpha1 		parity
		10 		111000 	11011110 	10101101    000000 		= 0x2e37ab40

		word 4
		alpha2 		alpha3 		beta0 		parity
		10111110 	11101111 	11011110 	000000 			= 0x2fbbf780

		word 5
		beta1 		beta2 		beta3 		parity
		10101101 	11000000 	11011110 	000000 			= 0x2b703780

		word 6
		A0 									parity
		11001010101100001011000010110101    000000 			= 0x2eb76ac0

		word 7
		A1 (24 msb)							parity
		110010101011000010110000(10110101) 	000000 			= 0x32ac2c00

		word 8
		A0 (8 lsb)	tot 		WNt 		parity
		10110101 	01101110	01011010 	000000 			= 0x2d5b9680

		word 9
		dtLS 		WNLSF 		DN 			parity
		00001101 	11111110 	10100000 	000000 			= 0x037fa800

		word 10
		dtLSF 		14b reserved 	2b 		parity
		10011001 	11111111111111 	00 		000000 			= 0x267fff00
*/
		const long subframe456[10] = {0x22c000e4, 0x0000042c, 0x2e37ab40, 0x2fbbf780, 0x2b703780,
									  0x2eb76ac0, 0x32ac2c00, 0x2d5b9680, 0x037fa800, 0x267fff00};
		testMesg = "Adding subframe 4 page 18 failed";
		testFramework.assert(dataStore.addSubframe(subframe456, 819), testMesg, __LINE__);

		//GPSTK documentation should really say what units these return as
		for(int i=0; i<31; i++)
		{
			testMesg = "getEcc returned the wrong value";
			testFramework.assert(std::abs(dataStore.getEcc(gpstk::SatID(1, gpstk::SatID::systemGPS)) - aData.ecc[0])*1E2 < eps, testMesg, __LINE__);
			testMesg = "getIOffset returned the wrong value";
			testFramework.assert(std::abs(dataStore.getIOffset(gpstk::SatID(1, gpstk::SatID::systemGPS)) - (aData.oi[0] - .3*M_PI)) < eps, testMesg, __LINE__);
			testMesg = "getOmegadot returned the wrong value";
			testFramework.assert(std::abs(dataStore.getOmegadot(gpstk::SatID(1, gpstk::SatID::systemGPS)) - aData.rora[0])*1E8 < eps, testMesg, __LINE__);
			testMesg = "get6bitHealth returned the wrong value";
			testFramework.assert(dataStore.get6bitHealth(gpstk::SatID(1, gpstk::SatID::systemGPS)) == 0 , testMesg, __LINE__);
			testMesg = "getSVHealth returned the wrong value";
			testFramework.assert(dataStore.getSVHealth(gpstk::SatID(1, gpstk::SatID::systemGPS)) == 0, testMesg, __LINE__);
			testMesg = "getSVConfig returned the wrong value";
			//testFramework.assert(std::abs(dataStore.getSVConfig(gpstk::SatID(1, gpstk::SatID::systemGPS)) == aData[0]) < eps, testMesg, __LINE__);
			testMesg = "getAhalf returned the wrong value";
			testFramework.assert(std::abs(dataStore.getAhalf(gpstk::SatID(1, gpstk::SatID::systemGPS)) - aData.sqrta[0])*1E-4 < eps, testMesg, __LINE__);
			testMesg = "getA returned the wrong value";
			testFramework.assert(std::abs(dataStore.getA(gpstk::SatID(1, gpstk::SatID::systemGPS)) - pow(aData.sqrta[0],2))*1E-8 < eps, testMesg, __LINE__);
			testMesg = "getOmega0 returned the wrong value";
			testFramework.assert(std::abs(dataStore.getOmega0(gpstk::SatID(1, gpstk::SatID::systemGPS)) - aData.raaw[0])*1E-1 < eps, testMesg, __LINE__);
			testMesg = "getW returned the wrong value";
			testFramework.assert(std::abs(dataStore.getW(gpstk::SatID(1, gpstk::SatID::systemGPS)) - aData.aop[0]) < eps, testMesg, __LINE__);
			testMesg = "getM0 returned the wrong value";
			testFramework.assert(std::abs(dataStore.getM0(gpstk::SatID(1, gpstk::SatID::systemGPS)) - aData.ma[0])*1E-1 < eps, testMesg, __LINE__);
			testMesg = "getAf0 returned the wrong value";
			testFramework.assert(std::abs(dataStore.getAf0(gpstk::SatID(1, gpstk::SatID::systemGPS)) - aData.af0[0])*1E5 < eps, testMesg, __LINE__);
			testMesg = "getAf1 returned the wrong value";
			testFramework.assert(std::abs(dataStore.getAf1(gpstk::SatID(1, gpstk::SatID::systemGPS)) - aData.af1[0]) < eps, testMesg, __LINE__);
		}

		dataStore.check(std::cout);

		double a[4], b[4];
		dataStore.getIon(a, b);

		testMesg = "getIon returned an incorrect value";
		testFramework.assert(std::abs(a[0] + 3.166496753692627e-08) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(a[1] + 1.968422573302286e-07) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(a[2] + 3.985880685087617e-07) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(a[3] + 3.26798013069863e-08) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(b[0] + 69632) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(b[1] + 432860.7015445238) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(b[2] + 424971.8458357919) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(b[3] + 71863.64306088151) < eps, testMesg, __LINE__);


		double a0, a1, deltaTLS, deltaTLSF;
		long tot;
		int WNt, WNLSF, DN;
		dataStore.getUTC(a0, a1, deltaTLS, tot, WNt, WNLSF, DN, deltaTLSF);
		std::cout<<a0<<std::endl; //wrong
		std::cout<<a1<<std::endl;
		std::cout<<deltaTLS<<std::endl;
		std::cout<<deltaTLSF<<std::endl;
		std::cout<<tot<<std::endl;
		std::cout<<WNt<<std::endl; // out of range of 8 bit
		std::cout<<WNLSF<<std::endl;// out of range of 8 bit
		std::cout<<DN<<std::endl; //is right value, but IS-GPS-200D says right justified

		testMesg = "getUTC returned an incorrect value";
		testFramework.assert(std::abs(a0 + 148.17864625621587) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(a1 + 4.0241348031599955e-09) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(deltaTLS - 13) < eps, testMesg, __LINE__);
		testFramework.assert(std::abs(deltaTLSF + 103) < eps, testMesg, __LINE__);
		testFramework.assert( tot == 450560, testMesg, __LINE__);
		testFramework.assert( WNt == 90, testMesg, __LINE__);
		testFramework.assert( WNLSF == 254, testMesg, __LINE__);
		testFramework.assert( DN == 5, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int getTest(void);

	int isDataTest(void);

	int checkTest(void);

	int dumpTest(void);

	private:
	double eps;
	std::string testMesg;
	AlmanacData aData; //data where the almanac info is stored
	AlmanacSubframes aSubframes; //data where subframes are stored

};


int main() //Main function to initialize and run all tests above
{
	std::ifstream iAlmanac("./data/test_input_gps_almanac.txt");
	AlmanacData iAData(iAlmanac);
	AlmanacSubframes iASubframes(iAData);

	EngAlmanac_T testClass(iAData, iASubframes);
	int check, errorCounter = 0;

	check = testClass.addSubframeTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
