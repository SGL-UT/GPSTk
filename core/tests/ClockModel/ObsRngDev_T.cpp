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

#include "EphemerisRange.hpp"

gpstk::IonoModelStore ionoModelStoreGen(std::vector<gpstk::CommonTime>& cTimeVec)
{
	double a[] = {1,2,3,4}; double b[] = {4,3,2,1};

	gpstk::IonoModelStore ims;

	for (int i=0; i < cTimeVec.size(); i++)	
		ims.addIonoModel(cTimeVec[i], gpstk::IonoModel(a,b));

	return ims;
}

class ObsRngDev_T
{
    public: 
	ObsRngDev_T(){ eps = 1E-12; }// Default Constructor, set the precision value
	~ObsRngDev_T() {} // Default Desructor

	void initialization(void)
	{
		receiverPos.setGeodetic(30.387577, -97.727607, 240);

		prange.push_back(21665483.747); prange.push_back(21662553.318); prange.push_back(21659650.826);
		prange2.push_back(21665487.640); prange2.push_back(21662557.419); prange2.push_back(21659655.019);

		id.id = 1;
		id.system = gpstk::SatID::systemGPS;

		gpstk::CommonTime ct1 = gpstk::CivilTime(2006, 1, 31, 2, 0, 0, gpstk::TimeSystem::GPS).convertToCommonTime();
		gpstk::CommonTime ct2 = gpstk::CivilTime(2006, 1, 31, 4, 0, 0, gpstk::TimeSystem::GPS).convertToCommonTime();
		gpstk::CommonTime ct3 = gpstk::CivilTime(2006, 1, 31, 6, 0, 0, gpstk::TimeSystem::GPS).convertToCommonTime();
		cTimeVec.push_back(ct1);
		cTimeVec.push_back(ct2);
		cTimeVec.push_back(ct3);

		std::string path = gpstk::getPathData() + "/test_input_rinex_nav_ephemerisData.031";
		ephemStore.loadFile(path);
	}	

//============================================================================
//Don't know where, but wherever the timeTables in the satTable of OrbitEphStore
//	is generated, the commontime object is unknown instead of GPS
//============================================================================


	int  basicConstructorTest(void)
	{
		TestUtil testFramework("ObsRngDev", "basicConstructorTest", __FILE__, __LINE__);

		//same prn for different ranges & different time
		testMesg = "Generation of ORDs with the basic constructor failed";
		for (int i=0; i < cTimeVec.size(); i++)
		{
			try
			{
				gpstk::ObsRngDev ord(prange[i], id, cTimeVec[i], receiverPos, ephemStore, em);
				ordVec.push_back(ord);
				testFramework.assert(true, testMesg, __LINE__);
			}
			catch(gpstk::Exception e)
			{
				std::cout<<e<<std::endl;
				testFramework.assert(false, testMesg, __LINE__);
			}
		}

		testMesg = "obstime value was not set correctly in the basic constructor";
		//testFramework.assert(ordVec[i].obstime == cTimeVec[i], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVec.size(); i++)
			if(!(ordVec[i].obstime == cTimeVec[i]))
				failCount++;
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the basic constructor";
		//testFramework.assert(ordVec[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVec.size(); i++)
			if(!(ordVec[i].svid == id))
				failCount++;
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the basic constructor";
		//testFramework.assert(ordVec[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVec.size(); i++)
			if(!(ordVec[i].health.get_value() == 0))
				failCount++;
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int ionosphericConstructorTest(void)
	{
		TestUtil testFramework("ObsRngDev", "ionosphericConstructorTest", __FILE__, __LINE__);

		gpstk::IonoModelStore ims = ionoModelStoreGen(cTimeVec);
		gpstk::IonoModel::Frequency L1 = gpstk::IonoModel::L1;
		std::vector<gpstk::ObsRngDev> ordVecIon;
		testMesg = "Generation of ORDs with the Ionospheric constructor failed";
		for (int i=0; i < cTimeVec.size(); i++)
		{
			try
			{
				gpstk::ObsRngDev ord(prange[i], id, cTimeVec[i],
									receiverPos, ephemStore, em, ims, L1);
				ordVecIon.push_back(ord);
				testFramework.assert(true, testMesg, __LINE__);
			}
			catch (gpstk::Exception e)
			{
				std::cout<<e<<std::endl;
				testFramework.assert(false, testMesg, __LINE__);
			}
		}
		testMesg = "obstime value was not set correctly in the Ionospheric constructor";
		//testFramework.assert(ordVecIon[i].obstime == cTimeVec[i], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecIon.size(); i++)
			//Have same timeVec for every 10 instances of ordVecIon
			if(!(ordVecIon[i].obstime == cTimeVec[i]))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecIon.size() != 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the Ionospheric constructor";
		//testFramework.assert(ordVecIon[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecIon.size(); i++)
			if(!(ordVecIon[i].svid == id))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecIon.size() != 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the Ionospheric constructor";
		//testFramework.assert(ordVecIon[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecIon.size(); i++)
			if(!(ordVecIon[i].health.get_value() == 0))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecIon.size() != 0, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int troposphericConstructorTest(void)
	{
		TestUtil testFramework("ObsRngDev", "troposphericConstructorTest", __FILE__, __LINE__);

		gpstk::SimpleTropModel stm(18.8889, 1021.2176, 77.7777); // Celsius, mmBar, %humidity		
		std::vector<gpstk::ObsRngDev> ordVecTrop;
		testMesg = "Generation of ORDS with the Tropospheric constructor failed";
		for (int i=0; i < cTimeVec.size(); i++)
		{
			try 
			{
				gpstk::ObsRngDev ord(prange[i], id, cTimeVec[i],
									receiverPos, ephemStore, em, stm);
				ordVecTrop.push_back(ord);
				testFramework.assert(true, testMesg, __LINE__);
			}
			catch (gpstk::Exception e)
			{
				std::cout<<e<<std::endl;
				testFramework.assert(false, testMesg, __LINE__);
			}
		}
		
		testMesg = "obstime value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTrop[i].obstime == cTimeVec[i], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTrop.size(); i++)
			//Have same timeVec for every 10 instances of ordVecTrop
			if(!(ordVecTrop[i].obstime == cTimeVec[i]))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecTrop.size() != 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTrop[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTrop.size(); i++)
			if(!(ordVecTrop[i].svid == id))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecTrop.size() != 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTrop[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTrop.size(); i++)
			if(!(ordVecTrop[i].health.get_value() == 0))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecTrop.size() != 0, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int ionosphericTroposphericConstructorTest(void)
	{
		TestUtil testFramework("ObsRngDev", "ionosphericTroposphericConstructorTest", __FILE__, __LINE__);

		gpstk::SimpleTropModel stm(18.8889, 1021.2176, 77.7777);
		gpstk::IonoModelStore ims = ionoModelStoreGen(cTimeVec);
		gpstk::IonoModel::Frequency L1 = gpstk::IonoModel::L1;		
		std::vector<gpstk::ObsRngDev> ordVecTropIon;
		testMesg = "Generation of ORDs with the Ionospheric and Tropospheric constructor failed";
		for (int i=0; i < cTimeVec.size(); i++)
		{
			try
			{
				gpstk::ObsRngDev ord(prange[i], id, cTimeVec[i], receiverPos, 
									ephemStore, em, stm, ims, L1);
				ordVecTropIon.push_back(ord);
				testFramework.assert(true, testMesg, __LINE__);
			}
			catch (gpstk::Exception e)
			{
				std::cout<<e<<std::endl;
				testFramework.assert(false, testMesg, __LINE__);
			}
		}
		testMesg = "obstime value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTropIon[i].obstime == cTimeVec[i], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTropIon.size(); i++)
			//Have same timeVec for every 10 instances of ordVecTropIon
			if(!(ordVecTropIon[i].obstime == cTimeVec[i]))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecTropIon.size() != 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTropIon[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTropIon.size(); i++)
			if(!(ordVecTropIon[i].svid == id))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecTropIon.size() != 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the basic constructor";
		//testFramework.assert(ordVecTropIon[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTropIon.size(); i++)
			if(!(ordVecTropIon[i].health.get_value() == 0))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecTropIon.size() != 0, testMesg, __LINE__);

		return testFramework.countFails();
	}

//-----------------------------------------------------------------------------------------
	int gammaConstructorTest(void)
	{
		TestUtil testFramework("ObsRngDev", "gammaConstructorTest", __FILE__, __LINE__);

		std::vector<gpstk::ObsRngDev> ordVecGamma;
		testMesg = "Generation of ORDs with Gamma constructor failed";
		for (int i=0; i < cTimeVec.size(); i++)
		{
			try
			{
				gpstk::ObsRngDev ord(prange[i], prange2[i], id, cTimeVec[i],
									receiverPos, ephemStore, em);
				ordVecGamma.push_back(ord);
				testFramework.assert(true, testMesg, __LINE__);
			}
			catch (gpstk::Exception e)
			{
				std::cout<<e<<std::endl;
				testFramework.assert(false, testMesg, __LINE__);
			}
		}
		testMesg = "obstime value was not set correctly in the Gamma constructor";
		//testFramework.assert(ordVecGamma[i].obstime == cTimeVec[i], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecGamma.size(); i++)
			//Have same timeVec for every 10 instances of ordVecGamma
			if(!(ordVecGamma[i].obstime == cTimeVec[i]))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecGamma.size() != 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the Gamma constructor";
		//testFramework.assert(ordVecGamma[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecGamma.size(); i++)
			if(!(ordVecGamma[i].svid == id))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecGamma.size() != 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the Gamma constructor";
		//testFramework.assert(ordVecGamma[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecGamma.size(); i++)
			if(!(ordVecGamma[i].health.get_value() == 0))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecGamma.size() != 0, testMesg, __LINE__);

		return testFramework.countFails();
	}
//-------------------------------------------------------------------------------------------
	int gammaTroposphericConstructorTest(void)
	{
		TestUtil testFramework("ObsRngDev", "gammaTroposphericConstructorTest", __FILE__, __LINE__);

		gpstk::SimpleTropModel stm(18.8889, 1021.2176, 77.7777);
		std::vector<gpstk::ObsRngDev> ordVecTropGamma;
		testMesg = "Generation of ORDs with Gamma and Tropospheric constructor failed";
		for (int i=0; i < cTimeVec.size(); i++)
		{
			try
			{
				gpstk::ObsRngDev ord(prange[i], prange2[i], id, cTimeVec[i],
									receiverPos, ephemStore, em, stm);
				ordVecTropGamma.push_back(ord);
				testFramework.assert(true, testMesg, __LINE__);
			}
			catch (gpstk::Exception e)
			{
				std::cout<<e<<std::endl;
				testFramework.assert(false, testMesg, __LINE__);
			}
		}
		testMesg = "obstime value was not set correctly in the Gamma and Tropospheric constructor";
		//testFramework.assert(ordVecTropGamma[i].obstime == cTimeVec[i], testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTropGamma.size(); i++)
			//Have same timeVec for every 10 instances of ordVecTropGamma
			if(!(ordVecTropGamma[i].obstime == cTimeVec[i]))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecTropGamma.size() != 0, testMesg, __LINE__);
	
		testMesg = "svid value was not set correctly in the Gamma and Tropospheric constructor";
		//testFramework.assert(ordVecTropGamma[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTropGamma.size(); i++)
			if(!(ordVecTropGamma[i].svid == id))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecTropGamma.size() != 0, testMesg, __LINE__);

		testMesg = "health value was not set correctly in the Gamma and Tropospheric constructor";
		//testFramework.assert(ordVecTropGamma[i].svid == id, testMesg, __LINE__);
		failCount = 0;
		for (int i=0; i < ordVecTropGamma.size(); i++)
			if(!(ordVecTropGamma[i].health.get_value() == 0))
				failCount++;
		testFramework.assert(failCount == 0 && ordVecTropGamma.size() != 0, testMesg, __LINE__);

		return testFramework.countFails();
	}

	int getFunctionsTest(void)
	{
		TestUtil testFramework("ObsRngDev", "Get Methods", __FILE__, __LINE__);
		std::string testMesg;
		int failCount;

		//Fails all tests if ordVec isn't generated
		if (ordVec.size() == 0)
		{
			testMesg = "ORDs never generated, impossible to do any get function test";
			for (int i=0; i<=9; i++)
				testFramework.assert(false, testMesg, __LINE__);
			return testFramework.countFails();
		}

		failCount = 0;
		testMesg = "getTime returned an incorrect value";
		//testFramework.assert(ordVec[i].getTime() == ordVec[i].obstime, testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getTime() == ordVec[i].obstime))
				failCount++;
		}
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getSvID returned an incorrect value";
		//testFramework.assert(ordVec[i].getSvID() == ordVec[i].svid, testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getSvID() == ordVec[i].svid))
				failCount++;
		}
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getAzim returned an incorrect value";
		//testFramework.assert(ordVec[i].getAzimuth().get_value() == ordVec[i].azimuth.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getAzimuth().get_value() == ordVec[i].azimuth.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getElev returned an incorrect value";
		//testFramework.assert(ordVec[i].getElevation().get_value() == ordVec[i].elevation.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getElevation().get_value() == ordVec[i].elevation.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getHealth returned an incorrect value";
		//testFramework.assert(ordVec[i].getHealth().get_value() == ordVec[i].health.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getHealth().get_value() == ordVec[i].health.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getIODC returned an incorrect value";
		//testFramework.assert(ordVec[i].getIODC().get_value() == ordVec[i].iodc.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getIODC().get_value() == ordVec[i].iodc.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getORD( returned an incorrect value";
			//testFramework.assert(ordVec[i].getORD() == ordVec[i].ord, testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getORD() == ordVec[i].ord))
				failCount++;
		}
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getIono returned an incorrect value";
		//testFramework.assert(ordVec[i].getIono().get_value() == ordVec[i].iono.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getIono().get_value() == ordVec[i].iono.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

		failCount = 0;
		testMesg = "getTrop returned an incorrect value";
		//testFramework.assert(ordVec[i].getTrop().get_value() == ordVec[i].trop.get_value(), testMesg, __LINE__);
		for(int i=0; i<ordVec.size(); i++)
		{
			if (!(ordVec[i].getTrop().get_value() == ordVec[i].trop.get_value()))
				failCount++;
		}
		testFramework.assert(failCount == 0 && ordVec.size() != 0, testMesg, __LINE__);

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

		//Fails all tests if ordVec isn't generated
		if (ordVec.size() == 0)
		{
			testMesg = "ORDs never generated, impossible to do calculation test";
			for (int i=0; i<=4; i++)
				testFramework.assert(false, testMesg, __LINE__);
		}

		for (int i=0; i < ordVec.size(); i++)
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
		}

		return testFramework.countFails();
	}

	private:
		double eps;
		std::string testMesg;
		int failCount;
		gpstk::SatID id;
		std::vector<float> prange;
		std::vector<float> prange2;
		std::vector<gpstk::ObsRngDev> ordVec;
		std::vector< std::map<int, float> > prnPrange;
		std::vector<gpstk::CommonTime> cTimeVec;
		gpstk::Position receiverPos;
		gpstk::RinexEphemerisStore ephemStore;
		gpstk::WGS84Ellipsoid em;
};


int main() //Main function to initialize and run all tests above
{
	int check = 0, errorCounter = 0;
	ObsRngDev_T testClass;

	testClass.initialization();

	check = testClass.basicConstructorTest();
	errorCounter += check;

	check = testClass.ionosphericConstructorTest();
	errorCounter += check;

	check = testClass.troposphericConstructorTest();
	errorCounter += check;

	check = testClass.ionosphericTroposphericConstructorTest();
	errorCounter += check;

	check = testClass.gammaConstructorTest();
	errorCounter += check;

	check = testClass.gammaTroposphericConstructorTest();
	errorCounter += check;

	check = testClass.getFunctionsTest();
	errorCounter += check;

	check = testClass.calculationTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
