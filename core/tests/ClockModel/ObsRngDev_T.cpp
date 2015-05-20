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

//Reads Rinex data from file into stl objects for construction of ObsRngDev objects
void dataGen(std::vector< std::map<int, float> > &prnPrange,
			 std::vector<gpstk::CommonTime> &cTimeVec,
			 gpstk::Position &receiverPos)
{
	std::string path = gpstk::getPathData() + "/test_input_rinex_obs_RinexObsFile.06o";
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
	while( obsFileStream >> obsData)
	{
		//Loops through all sats for current Obs
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

void commonTimeGen()
{

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

		//6 epochs are in file used
		std::vector< std::map<int, float> > prnPrange(6);
		std::vector<gpstk::CommonTime> cTimeVec(6);
		gpstk::Position receiverPos;
		gpstk::RinexEphemerisStore ephemStore;
		std::string path = gpstk::getPathData() + "/test_input_rinex_nav_ephemerisData.031";

		dataGen(prnPrange, cTimeVec, receiverPos);
		ephemStore.loadFile(path);
std::cout<<__LINE__<<std::endl;
		gpstk::WGS84Ellipsoid em;
std::cout<<__LINE__<<std::endl;
		//list of prn's in file used
		int prnList[] = {1, 5, 11, 14, 15, 18, 22, 25, 30};
		std::vector<gpstk::ObsRngDev> ordVec;

		int prn;
		for (int i=0; i < cTimeVec.size(); i++)
			//length of prnList
			for (int j=0; j < 9; j++)
			{
				prn = prnList[j];
				gpstk::SatID id(prn, gpstk::SatID::systemGPS);
				std::cout<<__LINE__<<std::endl;
				try {
					gpstk::ObsRngDev ord(prnPrange[i][prn], id, cTimeVec[i],
					receiverPos, ephemStore, em);
				}
				catch (gpstk::Exception e)
				{
					std::cout<<e<<std::endl;
				} 
				std::cout<<__LINE__<<std::endl;
				ordVec.push_back(ord);
			};
		// gpstk::ObsRngDev(21665483.802, gpstk::SatID(9, gpstk::SatID::systemGPS),  )


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
