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

#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"


class ObsRngDev_T
{
    public: 
	ObsRngDev_T(){ eps = 1E-12; }// Default Constructor, set the precision value
	~ObsRngDev_T() {} // Default Desructor

	int initializationTest(void)
	{
		TestUtil testFramework("ObsRngDev", "initializationTest", __FILE__, __LINE__);

// Normal
		//need ephem data in form of Xvtstore for one sat
		gpstk::RinexEphemerisStore dataStore;
		std::string path = gpstk::getPathData() + "/test_input_rinex_nav_ephemerisData.031";
		dataStore.loadFile(path);

// Can either attempt a value by value constructor as below, or use RINEX heavily
						//psuedorange cp from ...
		// gpstk::ObsRngDev(21665483.802, gpstk::SatID(9, gpstk::SatID::systemGPS),  )

// RINEX Attempt
		path = gpstk::getPathData() + "/test_input_rinex_obs_RinexObsFile.06o";
		gpstk::RinexObsStream obsFileStream(path);
		gpstk::RinexObsHeader obsHeader;
		gpstk::RinexObsData obsData;

		obsFileStream >> obsHeader;
		obsHeader.dump(std::cout);

		//Function shown in example4, but doesn't exist.
		//int indexP1 = obsHeader.getObsIndex("P1");
		int indexP1 = 3;
		std::cout<<obsHeader.obsTypeList[indexP1]<<std::endl;
		gpstk::RinexObsType typeP1;
		typeP1 = obsHeader.obsTypeList[indexP1];
		
		//gpstk::RinexObsData::DataMap::const_iterator it;

		std::vector<std::vector<float> > prange(6, std::vector<float>(31));

		int i = 0;

		//Cycles through stored Obs
		while( obsFileStream >> obsData)
		{
/*
		//loops through each satellite
		for (it = obsData.obs.begin(); it != obsData.obs.end(); it++)
		{
			try prnPrange[(*it).first] = obsData.getObs((*it).first, indexP1);
			catch(...) continue; // Ignore if psuedorange not found
		};
*/
			//Loops through all sats for current Obs
			for (int j = 1; j<32; j++)
			{
				gpstk::SatID id = gpstk::SatID(j, gpstk::SatID::systemGPS); 
				prange[i][j-1] = obsData.obs[id][typeP1].data;
				std::cout<<prange[i][j-1]<<std::endl;
			}
			i++;
		}

		// std::cout<<prange[0][6]<<std::endl;

// IonoModel
// TropModel
// Iono&Trop
// gamma
// gamma&Trop
		return testFramework.countFails();
	}

	int getMethodTest(void);

	int operatorTest(void);


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
