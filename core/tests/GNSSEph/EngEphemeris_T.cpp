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

#include "EngEphemeris.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <sstream>

class EngEphemeris_T //: public gpstk::EngEphemeris
{
    public: 
	EngEphemeris_T() // Default Constructor, set the precision value
	{
		eps = 1E-12; 
		b10 = 10; //need to define as double to solve overload in solaris compiler
	}
	~EngEphemeris_T() {} // Default Destructor

	//Used to initalize a object before rewriting with the valid subframes.
	//Necessary for addSubframe and addSubframeNoParity
	//Makes it seem like it has 3 valid subframes
	gpstk::EngEphemeris fakeEphemerisInit(void)
	{
		gpstk::EngEphemeris fakeEphemeris;

		//array 30 bit words all set to one, an invalid subframe. Word 2 is different, contains SF id
		const uint32_t data1[10] = {0x22FFFFFF, 0x3FFFF930, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF,
							 		0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF};
		const uint32_t data2[10] = {0x22FFFFFF, 0x3FFFFA88, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF,
							 		0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF};
		const uint32_t data3[10] = {0x22FFFFFF, 0x3FFFFBD0, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF,
							  		0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF};

		fakeEphemeris.haveSubframe[0] = true;
		fakeEphemeris.haveSubframe[1] = true;
		fakeEphemeris.haveSubframe[2] = true;
		for (int i=0;i<10;++i) fakeEphemeris.subframeStore[0][i] = data1[i];
		for (int i=0;i<10;++i) fakeEphemeris.subframeStore[1][i] = data2[i];
		for (int i=0;i<10;++i) fakeEphemeris.subframeStore[2][i] = data3[i];

		return fakeEphemeris;
	}

//=======================================================================
//	The following 3 methods are used to see if the data specificly set for
//	each subframe was set correctly
//
//	Data can be stored in EngEphemeris in multiple ways. This ensures
//	the method of storing data was successful
//
//	Doesn't test any of the data stored in the orbit or clock objects
//=======================================================================

	void subframe1Check(gpstk::EngEphemeris dataStore, TestUtil& testFramework, bool skipASAlert = false)
	{
		testMesg = "The flag for Subframe 1 was not set to stored";
		testFramework.assert(dataStore.haveSubframe[0], testMesg, __LINE__);
		testMesg = "The PRN wasn't stored correctly";
		testFramework.assert(dataStore.PRNID == 6, testMesg, __LINE__);
		testMesg = "The Tracker wasn't stored correctly";
		testFramework.assert(dataStore.tracker == 1, testMesg, __LINE__);

		//ASAlert is set to 1 by that addIncompleteSF1Thru3,
		//so bypassing the ASAlerts test only for that function
		//HOWTime is not set by this function either
		if (!skipASAlert)
		{
			testMesg = "The ASAlert wasn't stored correctly";
			testFramework.assert(dataStore.ASalert[0] == 0, testMesg, __LINE__);
			testMesg = "The HOWTime wasn't stored correctly";
			testFramework.assert(dataStore.HOWtime[0] == 409902, testMesg, __LINE__);			
		}

		testMesg = "The FullWeek wasn't stored correctly";
		testFramework.assert(dataStore.weeknum == 1025, testMesg, __LINE__);
		testMesg = "The CodeFlags wasn't stored correctly";
		testFramework.assert(dataStore.codeflags == 2, testMesg, __LINE__);	
		testMesg = "The Health wasn't stored correctly";
		testFramework.assert(dataStore.health == 0, testMesg, __LINE__);
		testMesg = "The L2Pdata wasn't stored correctly";
		testFramework.assert(dataStore.L2Pdata == 0, testMesg, __LINE__);
		testMesg = "The IODC wasn't stored correctly";
		testFramework.assert(dataStore.IODC == 91, testMesg, __LINE__);	
	}

	void subframe2Check(gpstk::EngEphemeris dataStore, TestUtil& testFramework, bool skipASAlert = false)
	{
		testMesg = "The flag for Subframe 2 wasn't stored correctly";
		testFramework.assert(dataStore.haveSubframe[1] == true, testMesg, __LINE__);
		testMesg = "The TLM Message wasn't stored correctly";
		testFramework.assert(dataStore.tlm_message[1] == 0, testMesg, __LINE__);
		
		//ASAlert is set to 1 by that addIncompleteSF1Thru3,
		//so bypassing the ASAlerts test only for that function
		//HOWTime is not set by this function either
		if (!skipASAlert)
		{
			testMesg = "The ASAlert wasn't stored correctly";
			testFramework.assert(dataStore.ASalert[1] == 0, testMesg, __LINE__);
			testMesg = "The HOW time wasn't stored correctly";
			testFramework.assert(dataStore.HOWtime[1] == 409908, testMesg, __LINE__);
		}

		testMesg = "The IODE wasn't stored correctly";
		testFramework.assert(dataStore.IODE == 91, testMesg, __LINE__);
		testMesg = "The Fit Interval wasn't stored correctly";
		testFramework.assert(dataStore.fitint == 0, testMesg, __LINE__);
	}

	void subframe3Check(gpstk::EngEphemeris dataStore, TestUtil& testFramework, bool skipASAlert = false)
	{
		testMesg = "The flag for Subframe 3 wasn't stored correctly";
		testFramework.assert(dataStore.haveSubframe[2] == true, testMesg, __LINE__);
		testMesg = "The TLM Message wasn't stored correctly";
		testFramework.assert(dataStore.tlm_message[2] == 0, testMesg, __LINE__);
		
		//ASAlert is set to 1 by that addIncompleteSF1Thru3,
		//so bypassing the ASAlerts test only for that function
		//HOWTime is not set by this function either
		if (!skipASAlert)
		{
			testMesg = "The ASAlert wasn't stored correctly";
			testFramework.assert(dataStore.ASalert[2] == 0, testMesg, __LINE__);
			testMesg = "The HOW time wasn't stored correctly";
			testFramework.assert(dataStore.HOWtime[2] == 409914, testMesg, __LINE__);
		}
	}

	int initializationTest(void)
	{
		TestUtil testFramework("EngEphemeris", "Default Constructor", __FILE__, __LINE__);
		int badCount = 0;
		std::stringstream testStream;

		gpstk::EngEphemeris Empty;

		testMesg = "PRNID was not initialized to 0";
		testFramework.assert(Empty.PRNID == 0, testMesg, __LINE__);
		testMesg = "Tracker was not initialized to 0";
		testFramework.assert(Empty.tracker == 0, testMesg, __LINE__);
		testMesg = "IODC was not initialized to 0";
		testFramework.assert(Empty.IODC == 0, testMesg, __LINE__);
		testMesg = "IODE was not initialized to 0";
		testFramework.assert(Empty.IODE == 0, testMesg, __LINE__);
		testMesg = "Weeknum was not initialized to 0";
		testFramework.assert(Empty.weeknum == 0, testMesg, __LINE__);
		testMesg = "Codeflags was not initialized to 0";
		testFramework.assert(Empty.codeflags == 0, testMesg, __LINE__);
		testMesg = "Health was not initialized to 0";
		testFramework.assert(Empty.health == 0, testMesg, __LINE__);
		testMesg = "L2Pdata was not initialized to 0";
		testFramework.assert(Empty.L2Pdata == 0, testMesg, __LINE__);
		testMesg = "SatSys was not initialized to \"\"";
		testFramework.assert(Empty.satSys == "", testMesg, __LINE__);
		testMesg = "Tgd was not initialized to 0";
		testFramework.assert(Empty.Tgd < eps, testMesg, __LINE__);
		testMesg = "isFIC was not initialized to 0";
		testFramework.assert(Empty.isFIC == true, testMesg, __LINE__);
		testMesg = "ASalert[0] was not initialized to 0";
		testFramework.assert(Empty.ASalert[0] == 0, testMesg, __LINE__);
		testMesg = "ASalert[1] was not initialized to 0";
		testFramework.assert(Empty.ASalert[1] == 0, testMesg, __LINE__);
		testMesg = "ASalert[2] was not initialized to 0";
		testFramework.assert(Empty.ASalert[2] == 0, testMesg, __LINE__);
		testMesg = "HOWtime[0] was not initialized to 0";
		testFramework.assert(Empty.HOWtime[0] == 0, testMesg, __LINE__);
		testMesg = "HOWtime[1] was not initialized to 0";
		testFramework.assert(Empty.HOWtime[1] == 0, testMesg, __LINE__);
		testMesg = "HOWtime[2] was not initialized to 0";
		testFramework.assert(Empty.HOWtime[2] == 0, testMesg, __LINE__);
		testMesg = "HaveSubframe[0] was not initialized to 0";
		testFramework.assert(Empty.haveSubframe[0] == false, testMesg, __LINE__);
		testMesg = "HaveSubframe[1] was not initialized to 0";
		testFramework.assert(Empty.haveSubframe[1] == false, testMesg, __LINE__);
		testMesg = "HaveSubframe[2] was not initialized to 0";
		testFramework.assert(Empty.haveSubframe[2] == false, testMesg, __LINE__);

		for (int i=0; i<3; i++)
			for (int j=0; j<10; j++)
				if (Empty.subframeStore[i][j] > eps) badCount++;
		testStream << "Check if subframeStore is filled with 0s. " << badCount << " of the elements are incorrect.";
		testMesg = testStream.str(); testStream.str("");
		testFramework.assert(badCount == 0, testMesg, __LINE__);
		badCount = 0;


		for (int i=0; i<3; i++)
			if (Empty.haveSubframe[i] != false) badCount++;
		testStream << "Check if haveSubframe[] is set to false. " << badCount << " of the elements are incorrect.";
		testMesg = testStream.str(); testStream.str("");
		testFramework.assert(badCount == 0, testMesg, __LINE__);

		testStream << "Empty ephemeris was incorrectly considered valid.";
		testMesg = testStream.str(); testStream.str("");
		testFramework.assert(Empty.isValid() == false, testMesg, __LINE__);

		testStream << "Empty ephemeris was incorrectly considered complete.";
		testMesg = testStream.str(); testStream.str("");
		testFramework.assert(Empty.isDataSet() == false, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int addSubframeTest(void)
	{
		TestUtil testFramework("EngEphemeris", "addSubframe", __FILE__, __LINE__);

		gpstk::EngEphemeris dataStore;

		//Same values as for addSubframeNoParityTest below, just added correct parity
		//Parity was calculated using the verified functions from EngNav, bad use of time to
		//			do at least 30 binary calculations with take >5 minutes each

		const long subframe1P[10] = {0x22c000e4, 0x215ba160, 0x00180012, 0x1fffffc0, 0x3fffffc3,
									 0x3ffffffc, 0x3fffc009, 0x16d904f0, 0x003fdbac, 0x1b83ed54};
		const long subframe2P[10] = {0x22c000e4, 0x215bc2f0, 0x16c2eb4d, 0x09f7c524, 0x2fdc3384,
							   		 0x0289c0dd, 0x0d5ecc38, 0x036b6842, 0x034f4df0, 0x1904c0b4};
		const long subframe3P[10] = {0x22c000e4, 0x215be378, 0x3ffcc344, 0x1a8441d8, 0x3ff80b74, 
							   		 0x1c8deb5e, 0x0a34d525, 0x14a5012e, 0x3fee8c06, 0x16c35c80};
		dataStore = fakeEphemerisInit();

        testMesg = "Subframe was not added successfully";
		testFramework.assert(dataStore.addSubframe(subframe1P, 1025, 6, 1), testMesg, __LINE__); //Week: 1025, PRN: 6, tracker:1

		subframe1Check(dataStore, testFramework);

        testMesg = "Subframe was not added successfully";
		testFramework.assert(dataStore.addSubframe(subframe2P, 1025, 6, 1), testMesg, __LINE__); //Week: 1025, PRN: 6, tracker:1							

		subframe2Check(dataStore, testFramework);

        testMesg = "Subframe was not added successfully";
		testFramework.assert(dataStore.addSubframe(subframe3P, 1025, 6, 1), testMesg, __LINE__); //Week: 1025, PRN: 6, tracker:1							

		subframe3Check(dataStore, testFramework);

		return testFramework.countFails();
	}

	int addSubframeNoParityTest(void) //calls add subframe'
	{
		TestUtil testFramework("EngEphemeris", "addSubframeNoParity", __FILE__, __LINE__);

		gpstk::EngEphemeris dataStore;
		dataStore = fakeEphemerisInit();

/*
		The following huge comments describe how the broadcast subframe data is reconstructed
		from data/test_input_rinex_nav_FilterTest2.99n


		Feeding in raw binary values output by satellite (without the parity bits)
		Without parity each word in 24 bits instead of 30
		All reserved bits taken to be 1

		Raw SV data is reconstructed from data/test_input_rinex_nav_FilterTest2.99n
		using the data format in fig 20-1 of IS-GPS-200D

		Word 1 formed by TLM preamble followed by TLM mesg (Msg defined by Control Segment and Space segment (?), so left blank)
		ref. IS-GPS-200D p 82 fig 20-2
		preamble	msg 			RESERVED
		10001011  00000000000000	11 			= 0x8B0003

		Word 2 formed by 17b truncated TOW, 1b Alert Flag and 1b Spoof Flag
				followed by 3b subframe ID (p 81) and 2 parity computation bits 
		ref. IS-GPS-200D p 82 fig 20-2
		TOW was modified to be evenly divisable by 6
		17b truncated TOW (409902/6)	Alert 	AntiSpoof	SubframeID	Parity Computation
		10000101011011101				0 		0 			001			00 					= 0x856E84

		Word 3 formed by 10b Week Number (mod 1024), 2b codeflag, 4b URA index, 6b SV health,
				and first 2 bits of IODC
		ref. IS-GPS-200D 20.3.3.3 (p 82)
		Week Num (1025)	CodeFlag (C/A code must exist, and P code flag set to 0)	URA 	svhealth 	IODC 2 MSB
		0000000001		10 															0000 	000000 		00 			= 0x006000

		Word 4 is L2 P code flag (0) followed by 23 reserved bits, setting them to 1
		0x7FFFFF

		Words 5 and 6 are composed on 24 reserved bits, setting them to 1
		0xFFFFFF

		Word 7 formed by 16 reserved bits, setting them to 1, followed by 8b TGD
		TGD is represented by integer scaled by a factor of 2^-31 (IS-GPS-200D table 20-I)
		Reserved 			TGD * 2^-31 (0)
		1111111111111111	00000000 		= 0xFFFF00

		Word 8 formed by 8 LSBs of IODC and 16b toc scaled by 2^4
		Toc is obtained by converting UTC time from Rinex Nav, scaled to GPS week second 
				(=25619)
		(IS-GPS-200D table 20-I)
		8 LSBs of IODC 	toc * 2^-4
		0101 1011		0110 0100 0001 0011 	= 0x5B6413

		Word 9 formed by 8b of af2 scaled by 2^-55, and 16b of af1 scaled by 2^-43 (IS-GPS-200D table 20-I)
		af2 * 2^52 		af1 * 2^43
		0000 0000 		1111 1111 0110 1110	= 0x00FF6E

		Word 10 formed by 22b of af0 scaled by 2^-31 and 2 reserved parity bits (marked 0)
		af * 2^31. # is signed, so found by 2^22 + af0*2^31
		-.839701388031E-03 * 2^31 =  
		1001000111110000010011 + 00 = 1001 0001 1111 0000 0100 1100 = 0x91F04C
*/

		//	Is this function designed to just update already stored subframes?
		//	Must be, because loop to see if all 3 subframes stored

		const long subframe1[10] = {0x8B0003, 0x856E84, 0x006000, 0x7FFFFF, 0xFFFFFF,
									0xFFFFFF, 0xFFFF00, 0x5B6413, 0x00FF6E, 0x91F04C};

		//gpstk::EngEphemeris testGDataStore;							

        testMesg = "Subframe was not added successfully";
		testFramework.assert(dataStore.addSubframeNoParity(subframe1, 1025, 6, 1), testMesg, __LINE__); //Week: 1025, PRN: 6, tracker:1

		subframe1Check(dataStore, testFramework);
/*

		Feeding in raw binary values output by satellite (without the parity bits)
		Without parity each word in 24 bits instead of 30
		All reserved bits taken to be 1

		Raw SV data is reconstructed from data/test_input_rinex_nav_FilterTest2.99n
		using the data format in fig 20-1 of IS-GPS-200D

		Word 1 formed by TLM preamble followed by TLM mesg (Msg defined by Control Segment and Space segment (?), so left blank)
		ref. IS-GPS-200D p 82 fig 20-2
		preamble	msg 			RESERVED
		10001011  00000000000000	11 			= 0x8B0003

		Word 2 formed by 17b truncated TOW, 1b Alert Flag and 1b Spoof Flag
				followed by 3b subframe ID (p 81) and 2 parity computation bits 
		ref. IS-GPS-200D p 82 fig 20-2
		Previous TOW + 6 for transmit time
		17b truncated TOW (409908/6)	Alert 	AntiSpoof	SubframeID	Parity Computation
		10000101011011110				0 		0 			010			00 					= 0x856F08

		Word 3 formed by 8b IODE and signed 16b Crs, scaled by 2^-5
		IODE (91)	Crs * 2^5 (93.40625 * 2^5)	
		01011011	0000101110101101		= 0x5B0BAD

		Word 4 is signed 16b delta N scaled by 2^-43 with the 8 msbs of M0 scaled by 2^-31
		Delta N * 2^43 (.11604054784E-8 * 2^43 / pi = 3249)
		0000 1100 1011 0001
		M0 * 2^31 (0.162092304801 * 2^31 / pi = 110800671)
		0000 0110 (1001 1010 1010 1111 0001 1111) = 0x0CB106

		Word 5 is the other 24 bits of M0 listed above
		M0
		1001 1010 1010 1111 0001 1111 = 0x9AAF1F

		Word 6 is signed 16b Cuc scaled by 2^-29 with the signed 8 msbs of e scaled by 2^-33
		Cuc*2^29 (.484101474285E-5*2^29)	e*2^33 (.626740418375E-2*2^33)
		0000 1010 0010 0111 				0000 0011 (0011 0101 0111 1011 0011 0000) = 0x0A2703

		Word 7 is the other 24 bits of e listed above 
		e
		0011 0101 0111 1011 0011 0000 	= 0x357B30

		Word 8 is signed 16b of Cus scaled by 2^-29 and the unsigned 8mbs of sqrtA scaled by 2^-19
		Cus*2^29 (.652112066746E-5*2^29)	sqrtA*2^19(.515365489006E4*2^19)
		0000 1101 1010 1101 				1010 0001 (0000 1101 0011 1101 0011 0111) = 0x0DADA1

		Word 9 is the other 24 bits of sqrtA listed above
		A
		0000 1101 0011 1101 0011 0111 = 0x0D3D37

		Word 10 is 16b toe scaled by 2^4 with 1b fit interval flag and 5b AODO
		Toe (409902*2^-4)		fitInt 	AODO (age of almanac observations, doesn't matter)
		0110 0100 0001 0011 	0 		000 00  		00 = 0x641300
*/

		const long subframe2[10] = {0x8B0003, 0x856F08, 0x5B0BAD, 0x0CB106, 0x9AAF1F,
									0x0A2703, 0x357B30, 0x0DADA1, 0x0D3D37, 0x641300};

        testMesg = "Subframe was not added successfully";
		testFramework.assert(dataStore.addSubframeNoParity(subframe2, 1025, 6, 1), testMesg, __LINE__); //Week: 1025, PRN: 6, tracker:1	

		subframe2Check(dataStore, testFramework);

		/*
		Feeding in raw binary values output by satellite (without the parity bits)
		Without parity each word in 24 bits instead of 30
		All reserved bits taken to be 1

		Raw SV data is reconstructed from data/test_input_rinex_nav_FilterTest2.99n
		using the data format in fig 20-1 of IS-GPS-200D

		Word 1 formed by TLM preamble followed by TLM mesg (Msg defined by Control Segment and Space segment (?), so left blank)
		ref. IS-GPS-200D p 82 fig 20-2
		preamble	msg 			RESERVED
		10001011  00000000000000	11 			= 0x8B0003

		Word 2 formed by 17b truncated TOW, 1b Alert Flag and 1b Spoof Flag
				followed by 3b subframe ID (p 81) and 2 parity computation bits 
		ref. IS-GPS-200D p 82 fig 20-2
		Previous TOW + 6 for transmit time
		17b truncated TOW (409914/6)	Alert 	AntiSpoof	SubframeID	Parity Computation
		10000101011011111				0 		0 			011			00 					= 0x856F0C

		Word 3 is signed 16b Cic scaled by 2^-29 with the signed 8 msbs of omega0 scaled by 2^-31
		Cic*2^29 (-.242143869400E-7*2^29)	omega0*2^31 (.329237003460*2^31/pi)
		1111 1111 1111 0010					0000 1101 (0110 1010 0001 0001 0000 0111) = 0xFFF30D

		Word 4 is the other 24 bits of omega0 listed above
		omega0
		0110 1010 0001 0001 0000 0111 = 0x6A1107

		Word 5 is signed 16b Cis scaled by 2^-29 with the signed 8 msbs of I0 scaled by 2^-31
		Cis*2^29 (-.596046447754E-7*2^29)	I0*2^31 (1.11541663136*2^31/pi)
		1111 1111 1110 0000					0010 1101 (0111 0010 0011 0111 1010 1101) = 0xFFE02D

		Word 6 is the other 24 bits of I0 listed above 
		I0
		0111 0010 0011 0111 1010 1101	= 0x7237AD

		Word 7 is signed 16b of Crc scaled by 2^-5 and the unsigned 8mbs of omega scaled by 2^-31
		Crc*2^5 (326.59375*2^5)	omega*2^31(2.06958726335*2^31/pi)
		0010 1000 1101 0011		0101 0100 (0101 0010 1001 0100 0000 0100) = 0x28D354

		Word 8 is the other 24 bits of omega listed above
		omega
		0101 0010 1001 0100 0000 0100 = 0x529404

		Word 9 is signed 24b of OMEGADOT scaled by 2^-43
		OMEGADOT (-.638312302555E-8*2^43/pi)
		1111 1111 1011 1010 0011 0000 = 0xFFBA30

		Word 10 is 8b IODE with signed 14b IDOT scaled by 2^-43
		IODE (91)		IDOT*2^43 (.307155651409E-9*2^43/pi) parity comp
		0101 1011		0000 1101 0111 00					00 		 = 0x5B0D70
*/

		const long subframe3[10] = {0x8B0003, 0x856F8C, 0xFFF30D, 0x6A1107, 0xFFE02D,
									0x7237AD, 0x28D354, 0x529404, 0xFFBA30, 0x5B0D70};

        testMesg = "Subframe was not added successfully";
		testFramework.assert(dataStore.addSubframeNoParity(subframe3, 1025, 6, 1), testMesg, __LINE__); //Week: 1025, PRN: 6, tracker:1

		subframe3Check(dataStore, testFramework);

		return testFramework.countFails();
	}

	int setSF1Test(gpstk::EngEphemeris& dataStore)
	{
		TestUtil testFramework("EngEphemeris", "setSF1", __FILE__, __LINE__);

		//The following values were taken from data/test_input_rinex_nav_FilterTest2.99n
		//Rinex documentation found at http://igscb.jpl.nasa.gov/igscb/data/format/rinex211.txt
		//helpful animation at http://emedia.rmit.edu.au/satellite/node/21

		//TLM value just taken to be the TLM msg, which is defined by the CS and SS, so left blank
		//HOW value taken to be Time of Week in seconds

		//More detailed info on each data type can be found in IS-GPS-200D

						//tlm, how,	
		dataStore.setSF1(0,		409902,
						//ASalert,	week,	cflags,	acc 	svhealth
						0,			1025,	2,		0,		0,
						//IODC 	l2pdata,	tgd
						91, 	0, 			0.0,
						//Toc, 	af2		af1							af0
						25619, 	0.0,	-.165982783074*pow(b10,-10),-.839701388031*pow(b10,-3),
						//tracker, 	prn
						1,			6);

		subframe1Check(dataStore, testFramework);

		std::stringstream testStream;
		
		testStream << "Partially empty ephemeris was incorrectly considered valid.";
		testMesg = testStream.str(); testStream.str("");
		testFramework.assert(dataStore.isValid() == false, testMesg, __LINE__);

		testStream << "Partially empty ephemeris was incorrectly considered complete.";
		testMesg = testStream.str(); testStream.str("");
		testFramework.assert(dataStore.isDataSet() == false, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int setSF2Test(gpstk::EngEphemeris& dataStore)
	{
		TestUtil testFramework("EngEphemeris", "setSF2", __FILE__, __LINE__);

		//The following values were taken from data/test_input_rinex_nav_FilterTest2.99n
		//Rinex documentation found at http://igscb.jpl.nasa.gov/igscb/data/format/rinex211.txt
		//helpful animation at http://emedia.rmit.edu.au/satellite/node/21

		//TLM value just taken to be the TLM msg, which is defined by the CS and SS, so left blank
		//HOW value taken to be Time of Week in seconds

		//More detailed info on each data type can be found in IS-GPS-200D

						//tlm, 	how,		ASalert
		dataStore.setSF2(0,		409908.0,	0,
						//IODE,	crs,		Dn 	
						91,		93.40625,	.11604054784 * pow(b10,-8),
						//m0			cuc 						Ecc
						0.162092304801, .484101474285*pow(b10,-5), 	.626740418375*pow(b10,-2),
						//cus 						aHalf
						.652112066746*pow(b10,-5), 	.515365489006*pow(b10,4),
						//toe (just HOW, should it be diff?)		fitInit
					 	409902, 									0);

		subframe2Check(dataStore, testFramework);

		std::stringstream testStream;
		
		testStream << "Partially empty ephemeris was incorrectly considered complete.";
		testMesg = testStream.str(); testStream.str("");
		testFramework.assert(dataStore.isDataSet() == false, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int setSF3Test(gpstk::EngEphemeris& dataStore)
	{
		TestUtil testFramework("EngEphemeris", "setSF3", __FILE__, __LINE__);

		//The following values were taken from data/test_input_rinex_nav_FilterTest2.99n
		//Rinex documentation found at http://igscb.jpl.nasa.gov/igscb/data/format/rinex211.txt
		//helpful animation at http://emedia.rmit.edu.au/satellite/node/21

		//TLM value just taken to be the TLM msg, which is defined by the CS and SS, so left blank
		//HOW value taken to be Time of Week in seconds

		//More detailed info on each data type can be found in IS-GPS-200D

						//tlm, 	how,		ASalert
		dataStore.setSF3(0,		409914.0,	0,
						//cic,						Omega0 (aka OMEGA),	 cis
						-.242143869400*pow(b10,-7),	.10479939309884491,		-.596046447754*pow(b10,-7),
						//I0		   		crc 		W (aka omega)
						0.3550481409757088, 326.59375, 	0.6587700862443613,
						//OmegaDot 							IDot
						-2.0318111637599545*pow(b10,-9), 	.307155651409*pow(b10,-9));		

		subframe3Check(dataStore, testFramework);

		std::stringstream testStream;
		
		testStream << "Complete, valid ephemeris was incorrectly considered incomplete.";
		testMesg = testStream.str(); testStream.str("");
		testFramework.assert(dataStore.isDataSet() == true, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int getTest(gpstk::EngEphemeris& dataStore)
	{
		TestUtil testFramework("EngEphemeris", "Get Methods", __FILE__, __LINE__);

		testMesg = "The getPRN method didn't function correctly";
		testFramework.assert(dataStore.getPRNID() == 6, testMesg, __LINE__);

		testMesg = "The getTracker method didn't function correctly";
		testFramework.assert(dataStore.getTracker() == 1, testMesg, __LINE__);

		testMesg = "The getHOWTime method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getHOWTime(1) - 409902) < eps, testMesg, __LINE__);

		testMesg = "The getASAlert method didn't function correctly";
		testFramework.assert(dataStore.getASAlert(1) == 0, testMesg, __LINE__);

		testMesg = "The getFullWeek method didn't function correctly";
		testFramework.assert(dataStore.getFullWeek() == 1025, testMesg, __LINE__);

		testMesg = "The getCodeFlags method didn't function correctly";
		testFramework.assert(dataStore.getCodeFlags() == 2, testMesg, __LINE__);

		testMesg = "The getToc method didn't function correctly";
		testFramework.assert(dataStore.getToc() == 25619, testMesg, __LINE__);

		testMesg = "The getAf0 method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getAf0() - -.839701388031*pow(b10,-3)) < eps, testMesg, __LINE__);

		testMesg = "The getAf1 method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getAf1() - -.165982783074*pow(b10,-10)) < eps, testMesg, __LINE__);

		testMesg = "The getAf2 method didn't function correctly";
		testFramework.assert(dataStore.getAf2() < eps, testMesg, __LINE__);		

		testMesg = "The getHealth method didn't function correctly";
		testFramework.assert(dataStore.getHealth() == 0, testMesg, __LINE__);

		testMesg = "The getL2Pdata method didn't function correctly";
		testFramework.assert(dataStore.getL2Pdata() == 0, testMesg, __LINE__);

		testMesg = "The getIODC method didn't function correctly";
		testFramework.assert(dataStore.getIODC() == 91, testMesg, __LINE__);

		testMesg = "The getIODE method didn't function correctly";
		testFramework.assert(dataStore.getIODE() == 91, testMesg, __LINE__);

		//setSF# doesn't set AODO, is only set by loadData which is not tested by getMethods. Skipping
//		testMesg = "The getAODO method didn't function correctly";
//		testFramework.assert(dataStore.getAODO() == 0/*VALUE NOT SET BY SETSF3*/, testMesg, __LINE__);		

		testMesg = "The getCus method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getCus() - .652112066746*pow(b10,-5)) < eps, testMesg, __LINE__);

		testMesg = "The getCrs method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getCrs() - 93.40625) < eps, testMesg, __LINE__);		

		testMesg = "The getCuc method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getCuc() - .484101474285*pow(b10,-5)) < eps, testMesg, __LINE__);

		testMesg = "The getToe method didn't function correctly";
		testFramework.assert(dataStore.getToe() == 409902, testMesg, __LINE__);

		testMesg = "The getM0 method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getM0() - 0.162092304801) < eps/*?*/, testMesg, __LINE__);

		testMesg = "The getDn method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getDn() - .11604054784*pow(b10,-8)) < eps, testMesg, __LINE__);

		testMesg = "The getEcc method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getEcc() - .626740418375*pow(b10,-2)) < eps, testMesg, __LINE__);

		testMesg = "The getAhalf method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getAhalf() - .515365489006*pow(b10,4)) < eps, testMesg, __LINE__);

		testMesg = "The getA method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getA() - pow(.515365489006*pow(b10,4),2)) < eps, testMesg, __LINE__);

		testMesg = "The getCis method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getCis() - -.596046447754*pow(b10,-7)) < eps, testMesg, __LINE__);

		testMesg = "The getCrc method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getCrc() - 326.59375) < eps, testMesg, __LINE__);

		testMesg = "The getCic method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getCic() - -.242143869400*pow(b10,-7)) < eps, testMesg, __LINE__);

		testMesg = "The getOmega0 method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getOmega0() - 0.10479939309884491) < eps, testMesg, __LINE__);

		testMesg = "The getI0 method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getI0() - 0.3550481409757088) < eps, testMesg, __LINE__);

		testMesg = "The getW method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getW() - 0.6587700862443613) < eps, testMesg, __LINE__);

		testMesg = "The getOmegaDot method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getOmegaDot() - -2.0318111637599545*pow(b10,-9)) < eps, testMesg, __LINE__);

		testMesg = "The getIDot method didn't function correctly";
		testFramework.assert(std::abs(dataStore.getIDot() - .307155651409*pow(b10,-9)) < eps, testMesg, __LINE__);

		testMesg = "The getTot method didn't function correctly";
		testFramework.assert(dataStore.getTot() == 409890, testMesg, __LINE__);
		//earliest of the HOW's (409902) rounded down to nearest multiple of 30

		return testFramework.countFails();	
	}

	int loadDataTest(void)
	{
		TestUtil testFramework("EngEphemeris", "loadData", __FILE__, __LINE__);

		gpstk::EngEphemeris dataStore;

		unsigned short tlm[3] = {0,0,0};
		const long how[3] = {409902, 409908, 409914};
		const short ASalert[3] = {0,0,0};

						  							//tlm,	how, ASalert  	Tracker PRN 
		dataStore.loadData((std::string) "No Idea", tlm,	how, ASalert,	1, 		6,
						//Week, cflags	acc 	svhealth 	IODC 	l2p 		Aodo
						1025,	2,		0,		0,			91, 	0, 			0.0,
						//tgd 	Toc, 	af2		af1							af0
						0.0, 	25619, 	0.0,	-.165982783074*pow(b10,-10),-.839701388031*pow(b10,-3),
						//IODE,	crs,		Dn 	
						91,		93.40625,	.11604054784 * pow(b10,-8),
						//m0			cuc 						Ecc
						0.162092304801, .484101474285*pow(b10,-5), 	.626740418375*pow(b10,-2),
						//cus 						aHalf
						.652112066746*pow(b10,-5), 	.515365489006*pow(b10,4),
						//toe (just HOW, should it be diff?)		fitInit
					 	409902, 									0,
					 	//cic,						Omega0 (aka OMEGA),		 cis
						-.242143869400*pow(b10,-7),	.10479939309884491,		-.596046447754*pow(b10,-7),
						//I0		   		crc 		W (aka omega)
						0.3550481409757088, 326.59375, 	0.6587700862443613,
						//OmegaDot 							IDot
						-2.0318111637599545*pow(b10,-9), 	.307155651409*pow(b10,-9));

		subframe1Check(dataStore, testFramework);

		subframe2Check(dataStore, testFramework);

		subframe3Check(dataStore, testFramework);

		return testFramework.countFails();
	}

	int addIncompleteTest(void)
	{
		TestUtil testFramework("EngEphemeris", "addIncomplete", __FILE__, __LINE__);

		gpstk::EngEphemeris dataStore;

		const long subframe1P[8] = {0x00180012, 0x1fffffc0, 0x3fffffc3, 0x3ffffffc,
									0x3fffc009, 0x16d904f0, 0x003fdbac, 0x1b83ed54};
		const long subframe2P[8] = {0x16c2eb4d, 0x09f7c524, 0x2fdc3384, 0x0289c0dd,
									0x0d5ecc38, 0x036b6842, 0x034f4df0, 0x1904c0b4};
		const long subframe3P[8] = {0x3ffcc344, 0x1a8441d8, 0x3ff80b74, 0x1c8deb5e,
									0x0a34d525, 0x14a5012e, 0x3fee8c06, 0x16c35c80};

		testFramework.assert(dataStore.addIncompleteSF1Thru3(subframe1P, subframe2P, subframe3P, 444, 1025, 6, 1), testMesg, __LINE__);

		//ASAlert tests fail, data is not included in the incomplete subframe.
		subframe1Check(dataStore, testFramework, true);

		subframe2Check(dataStore, testFramework, true);

		subframe3Check(dataStore, testFramework, true);

		return testFramework.countFails();
	}

	int dumpTest(void);

	std::string testMesg;
	
	private:
	double eps;
	double b10;
};

int main() //Main function to initialize and run all tests above
{

	EngEphemeris_T testClass;
	int check, errorCounter = 0;

	//Used to have a running ephemeris for functions that only set part of it
	gpstk::EngEphemeris dataStore;

	check = testClass.initializationTest();
	errorCounter += check;

	//setSF# require previous subframes to be set, and get needs a valid ephemeris object
	//Passing one between the tests
	//If one of the setSF# tests fail, all other setSF# and get tests will fail too
	check = testClass.setSF1Test(dataStore);
	errorCounter += check;

	check = testClass.setSF2Test(dataStore);
	errorCounter += check;

	check = testClass.setSF3Test(dataStore);
	errorCounter += check;	

	check = testClass.getTest(dataStore);
	errorCounter += check;

	check = testClass.addSubframeNoParityTest();
	errorCounter += check;

	check = testClass.addSubframeTest();
	errorCounter += check;

	check = testClass.loadDataTest();
	errorCounter += check;

	check = testClass.addIncompleteTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}