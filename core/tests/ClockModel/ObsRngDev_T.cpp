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

#include "ObsRngDev.hpp"
#include "TestUtil.hpp"
#include <iostream>

#include "RinexEphemerisStore.hpp"

//obsDataGen
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"

//ionoModelGen
#include "/home/voss/gpstk/core/tests/GNSSEph/AlmanacDataGenerator.hpp"

//Reads Rinex data from file into stl objects for construction of ObsRngDev objects
void obsDataGen(std::vector< std::map<int, float> > &prnPrange,
			 std::vector<gpstk::CommonTime> &cTimeVec,
			 gpstk::Position &receiverPos)
{
//	std::string path = gpstk::getPathData() + "/test_input_rinex_obs_RinexObsFile.06o";
	std::string path = gpstk::getPathData() + "/VossObs.06o";
//==================================================================================
//	If file is changed, be sure to change prn list, length of prn list in for loop, & # of epochs
//==================================================================================

	gpstk::RinexObsStream obsFileStream(path);
	gpstk::RinexObsHeader obsHeader;
	gpstk::RinexObsData obsData;

	obsFileStream >> obsHeader;

	//Index of P1 is 3 in file used
	int indexP1 = 3;
	gpstk::RinexObsType typeP1;
	typeP1 = obsHeader.obsTypeList[indexP1];
	
	int i = 0;

	//Cycles through stored Obs
	while(obsFileStream >> obsData)
	{
		//Loops through all sats for current Obs
		//CAN I MAKE THIS LOOP THROUGH ONLY CURRENT SATS?
		for (int j = 1; j<32; j++)
		{
			gpstk::SatID id = gpstk::SatID(j, gpstk::SatID::systemGPS); 
			prnPrange[i][j] = obsData.obs[id][typeP1].data;
		}

		cTimeVec[i] = obsData.time;

		i++;
	}

	receiverPos.setECEF(obsHeader.antennaPosition);

}

gpstk::IonoModel ionoModelGen(void)
{
	gpstk::EngAlmanac dataStore;

	std::string pathData = gpstk::getPathData();
	std::string almanacLocation = pathData + "/test_input_gps_almanac.txt";
	std::ifstream iAlmanac(almanacLocation.c_str()); // Reads in almanac data from file
	AlmanacData aData(iAlmanac); // Parses file into data objects
	AlmanacSubframes aSubframes(aData); // Takes data objects and generates the subframes needed

	for (int i=0; i<31; i++) dataStore.addSubframe(aSubframes.totalSf[i], 819);

	const long subframe551[10] = {0x22c000e4, 0x00000598, 0x2CD38CC0, 0x00000000, 0x00000FC0,
								  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x11111110};		
	const long subframe447[10] = {0x22c000e4, 0x0000042c, 0x2FE66640, 0x26666640, 0x26666640,
								  0x26666640, 0x26666640, 0x26667000, 0x00000000, 0x00000F00};
	const long subframe456[10] = {0x22c000e4, 0x0000042c, 0x2e37ab40, 0x2fbbf780, 0x2b703780,
								  0x2eb76ac0, 0x32ac2c00, 0x2d5b9680, 0x037f8140, 0x267fff00};
	dataStore.addSubframe(subframe551, 819);
	dataStore.addSubframe(subframe447, 819);
	dataStore.addSubframe(subframe456, 819);
	
	gpstk::IonoModel im(dataStore);


//Need 9 epochs of data spaced by 30 sec each
	//each data is composed of 2 arrays, each with size 4


	return im;
}



class ObsRngDev_T
{
    public: 
	ObsRngDev_T(){ eps = 1E-12; }// Default Constructor, set the precision value
	~ObsRngDev_T() {} // Default Desructor

	int initializationTest(void)
	{
		TestUtil testFramework("ObsRngDev", "initializationTest", __FILE__, __LINE__);

// Normal

//NEED TO USE NEW DATA FILES!
	// current ones have no overlap

		//9 epochs are in file used
		std::vector< std::map<int, float> > prnPrange(9);
		std::vector<gpstk::CommonTime> cTimeVec(9);
		gpstk::Position receiverPos;
		gpstk::RinexEphemerisStore ephemStore;
//		std::string path = gpstk::getPathData() + "/test_input_rinex_nav_ephemerisData.031";
		std::string path = gpstk::getPathData() + "/VossNav.06o";
//==================================================================================
//	If file is changed, be sure to change prn list, length of prn list in for loop, & # of epochs
//==================================================================================

		obsDataGen(prnPrange, cTimeVec, receiverPos);
		receiverPos.asGeodetic();

		ephemStore.loadFile(path);

		gpstk::WGS84Ellipsoid em;

		//list of prn's in file used
		//int prnList[] = {1, 5, 11, 14, 15, 18, 22, 25, 30};
		int prnList[] = {9, 2, 5, 6, 10, 21, 24, 26, 29, 30};

//Basic ORD Generator
//-----------------------------------------------------------------------------------------
		int prn;
		std::vector<gpstk::ObsRngDev> ordVec;
		for (int i=0; i < cTimeVec.size(); i++)
			//length of prnList
			for (int j=0; j < 10; j++)
			{
				prn = prnList[j];
				gpstk::SatID id(prn, gpstk::SatID::systemGPS);
				try {
					gpstk::ObsRngDev ord(prnPrange[i][prn], id, cTimeVec[i],
										receiverPos, ephemStore, em);
					ordVec.push_back(ord);
				}
				catch (gpstk::Exception e)
				{
					std::cout<<e<<std::endl;
				} 
			};

		std::string testMesg = "Test Failed";
		for (int i=0; i < ordVec.size(); i++)
		{
			//Have same timeVec for every 10 instances of ordVec
			testFramework.assert(ordVec[i].obstime == cTimeVec[floor(i/10)], testMesg, __LINE__);
			//Loop through prnList for every 10 instances of ordVec
			prn = prnList[i % 10];
			gpstk::SatID id(prn, gpstk::SatID::systemGPS);
			testFramework.assert(ordVec[i].svid == id, testMesg, __LINE__);
			std::cout<<ordVec[i]<<std::endl;
			// std::cout<<ordVec[i].health<<std::endl;
		}
//------------------------------------------------------------------------------------------

// IonoModel
// Need valid IonoModel object, how get? Copying Almanac generation from EngAlmanac_T

		gpstk::IonoModel im = ionoModelGen();
		std::vector<gpstk::ObsRngDev> ordVecIon;
		for (int i=0; i < cTimeVec.size(); i++)
			//length of prnList
			for (int j=0; j < 10; j++)
			{
				prn = prnList[j];
				gpstk::SatID id(prn, gpstk::SatID::systemGPS);
				try {
					gpstk::ObsRngDev ord(prnPrange[i][prn], id, cTimeVec[i],
										receiverPos, ephemStore, em);
					ordVecIon.push_back(ord);
				}
				catch (gpstk::Exception e)
				{
					std::cout<<e<<std::endl;
				}
			};

//Where go from here? I have a few options
// 		1. Continue where I'm at, just create a dummy IonoStore object
				//	Does it neeed to have the same data as the satellites already stored?
// 		2. Rewrite the above rinex stuff so that I'm only testing a few ords.
				//	Possible, simplier, but boring. Probably the best option though.
				// 	The rinex stuff is unneccesary, and don't need to run same test
				//	 On 100 ords. Creates problems for math checking though.
					//	But who cares? No ones going to write a test for math checking
//		Regardless, will still need IonomodelStore object, only have one EngAlmanac
				//	need to generate Ionomodel's by hand

// TropModel
// Iono&Trop
// gamma
// gamma&Trop
		return testFramework.countFails();
	}

	int computeOrdRx(void);

	int computeOrdTx(void);

	int getMethodTest(void);

	int operatorTest(void);

	int computeTrop(void);


	private:
		double eps;
};


int main() //Main function to initialize and run all tests above
{
	int check = 0, errorCounter = 0;
	ObsRngDev_T testClass;

	check = testClass.initializationTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
