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

#include "EphemerisRange.hpp"

//Reads Rinex data from file into stl objects for construction of ObsRngDev objects

//For hypothetical re-write, this only generats pseudorange 1&2 and commontime objects
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
		std::cout<<obsData.time.getTimeSystem()<<std::endl;
		i++;
	}

	receiverPos.setECEF(obsHeader.antennaPosition);

}

gpstk::IonoModelStore ionoModelStoreGen(std::vector<gpstk::CommonTime>& cTimeVec)
{

//Need 9 epochs of data spaced by 30 sec each
	//each data is composed of 2 arrays, each with size 4

	gpstk::IonoModel im1; gpstk::IonoModel im2; gpstk::IonoModel im3;
	gpstk::IonoModel im4; gpstk::IonoModel im5; gpstk::IonoModel im6;
	gpstk::IonoModel im7; gpstk::IonoModel im8; gpstk::IonoModel im9;

	double a[] = {1,2,3,4}; double b[] = {4,3,2,1};
	im1.setModel(a, b); im2.setModel(a, b); im3.setModel(a, b); im4.setModel(a, b);
	im5.setModel(a, b); im6.setModel(a, b); im7.setModel(a, b); im8.setModel(a, b);
	im9.setModel(a, b);

	gpstk::IonoModelStore ims;

	ims.addIonoModel(cTimeVec[0], im1); ims.addIonoModel(cTimeVec[1], im2);
	ims.addIonoModel(cTimeVec[2], im3); ims.addIonoModel(cTimeVec[3], im4);
	ims.addIonoModel(cTimeVec[4], im5); ims.addIonoModel(cTimeVec[5], im6);
	ims.addIonoModel(cTimeVec[6], im7); ims.addIonoModel(cTimeVec[7], im8);
	ims.addIonoModel(cTimeVec[8], im9);

	return ims;
}



class ObsRngDev_T
{
    public: 
	ObsRngDev_T(){ eps = 1E-12; }// Default Constructor, set the precision value
	~ObsRngDev_T() {} // Default Desructor

	int initializationTest(void)
	{
		TestUtil testFramework("ObsRngDev", "initializationTest", __FILE__, __LINE__);
		std::string testMesg;
		int failCount;

prnPrange.resize(10, std::map<int, float>() );
cTimeVec.resize(10);
// Normal

		//9 epochs are in file used
		//Moving to global class members, need for other tests
		// std::vector< std::map<int, float> > prnPrange(9);
		// std::vector<gpstk::CommonTime> cTimeVec(9);
		// gpstk::Position receiverPos;
		// gpstk::RinexEphemerisStore ephemStore;
//		std::string path = gpstk::getPathData() + "/test_input_rinex_nav_ephemerisData.031";
		std::string path = gpstk::getPathData() + "/VossNav.06o";
//==================================================================================
//	If file is changed, be sure to change prn list, length of prn list in for loop, & # of epochs
//==================================================================================

		obsDataGen(prnPrange, cTimeVec, receiverPos);
		receiverPos.asGeodetic();

		ephemStore.loadFile(path);
		std::cout<<std::endl<<ephemStore.getTimeSystem()<<std::endl; // Time system of the ENTIRE store

		gpstk::WGS84Ellipsoid em;

		//list of prn's in file used
		//int prnList[] = {1, 5, 11, 14, 15, 18, 22, 25, 30};
		int prnList[] = {9, 2, 5, 6, 10, 21, 24, 26, 29, 30};

//-----------------------------------------------------------------------------------------

//============================================================================
//Don't know where, but wherever the timeTables in the satTable of OrbitEphStore
//	is generated, the commontime object is unknown instead of GPS
//============================================================================


//Basic ORD Generator

		int prn;
		//std::vector<gpstk::ObsRngDev> ordVec;
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

		testMesg = "obstime value was not set correctly in the basic constructor";
		//testFramework.assert(ordVec[i].obstime == cTimeVec[floor(i/10)], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVec.size(); i++)
		{
			//Have same timeVec for every 10 instances of ordVec
			if(!(ordVec[i].obstime == cTimeVec[floor(i/10)]))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the basic constructor";
		//testFramework.assert(ordVec[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVec.size(); i++)
		{
			//Loop through prnList for every 10 instances of ordVec
			prn = prnList[i % 10];
			gpstk::SatID id(prn, gpstk::SatID::systemGPS);

			if(!(ordVec[i].svid == id))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the basic constructor";
		//testFramework.assert(ordVec[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVec.size(); i++)
		{
			if(!(ordVec[i].health.get_value() == 0))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);
//------------------------------------------------------------------------------------------

// IonoModel
// Need valid IonoModel object, how get? Copying Almanac generation from EngAlmanac_T

		gpstk::IonoModelStore ims = ionoModelStoreGen(cTimeVec);
		gpstk::IonoModel::Frequency L1 = gpstk::IonoModel::L1;
		std::vector<gpstk::ObsRngDev> ordVecIon;
		for (int i=0; i < cTimeVec.size(); i++)
			//length of prnList
			for (int j=0; j < 10; j++)
			{
				prn = prnList[j];
				gpstk::SatID id(prn, gpstk::SatID::systemGPS);
				try {
					gpstk::ObsRngDev ord(prnPrange[i][prn], id, cTimeVec[i],
										receiverPos, ephemStore, em, ims, L1);
					ordVecIon.push_back(ord);
				}
				catch (gpstk::Exception e)
				{
					std::cout<<e<<std::endl;
				}
			};
		testMesg = "obstime value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecIon[i].obstime == cTimeVec[floor(i/10)], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecIon.size(); i++)
		{
			//Have same timeVec for every 10 instances of ordVecIon
			if(!(ordVecIon[i].obstime == cTimeVec[floor(i/10)]))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecIon[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecIon.size(); i++)
		{
			//Loop through prnList for every 10 instances of ordVecIon
			prn = prnList[i % 10];
			gpstk::SatID id(prn, gpstk::SatID::systemGPS);

			if(!(ordVecIon[i].svid == id))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecIon[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecIon.size(); i++)
		{
			if(!(ordVecIon[i].health.get_value() == 0))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);
//------------------------------------------------------------------------------------------

// TropModel

		gpstk::SimpleTropModel stm(18.8889, 1021.2176, 77.7777); // Celsius, mmBar, %humidity		
		std::vector<gpstk::ObsRngDev> ordVecTrop;
		for (int i=0; i < cTimeVec.size(); i++)
			//length of prnList
			for (int j=0; j < 10; j++)
			{
				prn = prnList[j];
				gpstk::SatID id(prn, gpstk::SatID::systemGPS);
				try {
					gpstk::ObsRngDev ord(prnPrange[i][prn], id, cTimeVec[i],
										receiverPos, ephemStore, em, stm);
					ordVecTrop.push_back(ord);
				}
				catch (gpstk::Exception e)
				{
					std::cout<<e<<std::endl;
				}
			};
			testMesg = "obstime value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTrop[i].obstime == cTimeVec[floor(i/10)], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTrop.size(); i++)
		{
			//Have same timeVec for every 10 instances of ordVecTrop
			if(!(ordVecTrop[i].obstime == cTimeVec[floor(i/10)]))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTrop[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTrop.size(); i++)
		{
			//Loop through prnList for every 10 instances of ordVecTrop
			prn = prnList[i % 10];
			gpstk::SatID id(prn, gpstk::SatID::systemGPS);

			if(!(ordVecTrop[i].svid == id))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTrop[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTrop.size(); i++)
		{
			if(!(ordVecTrop[i].health.get_value() == 0))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

//------------------------------------------------------------------------------------------

// Iono&Trop

		// Previously Declared, here for reference
//		gpstk::SimpleTropModel stm(18.8889, 1021.2176, 77.7777);
//		gpstk::IonoModelStore ims = ionoModelStoreGen(cTimeVec);
//		gpstk::IonoModel::Frequency L1 = gpstk::IonoModel::L1;		
		std::vector<gpstk::ObsRngDev> ordVecTropIon;
		for (int i=0; i < cTimeVec.size(); i++)
			//length of prnList
			for (int j=0; j < 10; j++)
			{
				prn = prnList[j];
				gpstk::SatID id(prn, gpstk::SatID::systemGPS);
				try {
					gpstk::ObsRngDev ord(prnPrange[i][prn], id, cTimeVec[i],
										receiverPos, ephemStore, em, stm, ims, L1);
					ordVecTropIon.push_back(ord);
				}
				catch (gpstk::Exception e)
				{
					std::cout<<e<<std::endl;
				}
			};
		testMesg = "obstime value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTropIon[i].obstime == cTimeVec[floor(i/10)], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTropIon.size(); i++)
		{
			//Have same timeVec for every 10 instances of ordVecTropIon
			if(!(ordVecTropIon[i].obstime == cTimeVec[floor(i/10)]))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTropIon[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTropIon.size(); i++)
		{
			//Loop through prnList for every 10 instances of ordVecTropIon
			prn = prnList[i % 10];
			gpstk::SatID id(prn, gpstk::SatID::systemGPS);

			if(!(ordVecTropIon[i].svid == id))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTropIon[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTropIon.size(); i++)
		{
			if(!(ordVecTropIon[i].health.get_value() == 0))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);



//NEED PRANGE 1 AND 2 TO USE GAMMA IN CONSTRUCTOR
//-----------------------------------------------------------------------------------------

// gamma
/*
		double gamma = 1.2345;
		std::vector<gpstk::ObsRngDev> ordVecGamma;
		for (int i=0; i < cTimeVec.size(); i++)
			//length of prnList
			for (int j=0; j < 10; j++)
			{
				prn = prnList[j];
				gpstk::SatID id(prn, gpstk::SatID::systemGPS);
				try {
					gpstk::ObsRngDev ord(prnPrange[i][prn], id, cTimeVec[i],
										receiverPos, ephemStore, em, gamma);
					ordVecTropIon.push_back(ord);
				}
				catch (gpstk::Exception e)
				{
					std::cout<<e<<std::endl;
				}
			};

//-------------------------------------------------------------------------------------------

// gamma&Trop

		// Previously Declared, here for reference	
//		gpstk::SimpleTropModel stm(18.8889, 1021.2176, 77.7777);
//		double gamma = 1.2345;
		std::vector<gpstk::ObsRngDev> ordVecTropIonGamma;

		for (int i=0; i < cTimeVec.size(); i++)
			//length of prnList
			for (int j=0; j < 10; j++)
			{
				prn = prnList[j];
				gpstk::SatID id(prn, gpstk::SatID::systemGPS);
				try {
					gpstk::ObsRngDev ord(prnPrange[i][prn], id, cTimeVec[i],
										receiverPos, ephemStore, em, stm, gamma);
					ordVecTropIonGamma.push_back(ord);
				}
				catch (gpstk::Exception e)
				{
					std::cout<<e<<std::endl;
					//failCount++;
				}
			};
*/
//------------------------------------------------------------------------------------------

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

		return testFramework.countFails();
	}

	int getFunctionsTest(void)
	{
		TestUtil testFramework("ObsRngDev", "Get Methods", __FILE__, __LINE__);
		std::string testMesg;
		int failCount;

		failCount = 0;
		testMesg = "getTime returned an incorrect value";
		//testFramework.assert(ordVec[i].getTime() == ordVec[i].obstime, testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getTime() == ordVec[i].obstime))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getSvID returned an incorrect value";
		//testFramework.assert(ordVec[i].getSvID() == ordVec[i].svid, testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getSvID() == ordVec[i].svid))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getAzim returned an incorrect value";
		//testFramework.assert(ordVec[i].getAzimuth().get_value() == ordVec[i].azimuth.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getAzimuth().get_value() == ordVec[i].azimuth.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getElev returned an incorrect value";
		//testFramework.assert(ordVec[i].getElevation().get_value() == ordVec[i].elevation.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getElevation().get_value() == ordVec[i].elevation.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getHealth returned an incorrect value";
		//testFramework.assert(ordVec[i].getHealth().get_value() == ordVec[i].health.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getHealth().get_value() == ordVec[i].health.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getIODC returned an incorrect value";
		//testFramework.assert(ordVec[i].getIODC().get_value() == ordVec[i].iodc.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getIODC().get_value() == ordVec[i].iodc.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getORD( returned an incorrect value";
			//testFramework.assert(ordVec[i].getORD() == ordVec[i].ord, testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getORD() == ordVec[i].ord))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getIono returned an incorrect value";
		//testFramework.assert(ordVec[i].getIono().get_value() == ordVec[i].iono.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getIono().get_value() == ordVec[i].iono.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getTrop returned an incorrect value";
		//testFramework.assert(ordVec[i].getTrop().get_value() == ordVec[i].trop.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getTrop().get_value() == ordVec[i].trop.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int operatorTest(void); //purely a dump method. How necessary is this?

	int calculationTest(void) 
	{
		//need to test computeOrdRx, computeOrdTx, and computeTrop
		//none of the math is actually done in this class, is entirely reliant on calls to other files.
		//Therefore, how test? Calculation at it's core in Xvt::preciseRho

		TestUtil testFramework("ObsRngDev", "ComputeOrdRx", __FILE__, __LINE__);
		std::string testMesg;

		gpstk::CorrectedEphemerisRange cer;
		double rho;
		double prange;
		for (int i=0; i<ordVec.size(); i++)
		{
			prange = prnPrange[floor(i/10)][ordVec[i].svid.id];
			rho = cer.ComputeAtTransmitTime(ordVec[i].obstime, prange, receiverPos, ordVec[i].svid, ephemStore);
			testMesg = "Incorrect value for ord";
			testFramework.assert(std::abs(ordVec[i].ord - (prnPrange[floor(i/10)][ordVec[i].svid.id] - rho)) < eps, testMesg, __LINE__);
			testMesg = "Incorrect value for rho";
			testFramework.assert(std::abs(ordVec[i].rho - rho) < eps, testMesg, __LINE__);
			testMesg = "Incorrect value for azimuth";
			testFramework.assert(std::abs(ordVec[i].azimuth - cer.azimuth) < eps, testMesg, __LINE__);
			testMesg = "Incorrect value for elevation";
			testFramework.assert(std::abs(ordVec[i].elevation - cer.elevation) < eps, testMesg, __LINE__);
			std::cout<<ordVec[i].ord<<std::endl;
			printf("%10.10f\n", ordVec[i].ord);
			printf("%10.10f\n", (prnPrange[floor(i/10)][ordVec[i].svid.id] - rho));
		}

		return testFramework.countFails();
	}

	private:
		double eps;
		std::vector<gpstk::ObsRngDev> ordVec;
		std::vector< std::map<int, float> > prnPrange;
		std::vector<gpstk::CommonTime> cTimeVec;
		gpstk::Position receiverPos;
		gpstk::RinexEphemerisStore ephemStore;
};


int main() //Main function to initialize and run all tests above
{
	int check = 0, errorCounter = 0;
	ObsRngDev_T testClass;

	check = testClass.initializationTest();
	errorCounter += check;

	check = testClass.getFunctionsTest();
	errorCounter += check;

	check = testClass.calculationTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
