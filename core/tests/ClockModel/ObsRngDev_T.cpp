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
#include "SimpleTropModel.hpp"

//=====================================================================================
// Test Structure
// Begin with generating ObsRngDev objects using every constructor and test the basic
// initialization of these objects. Then verify that each of the objects constructed have
// calculated the ORD correctly, in addition to the elevation and azimuth. By testing
// the value of the ORD, the different calculations such as generating Tropospheric models
// and calculating ionospheric and tropospheric delays are tested implicitly. These tests
// are entirely dependent on the Ephemeris Range class calculating the range correctly.
//======================================================================================

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
   ObsRngDev_T()
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
   };
   
   ~ObsRngDev_T() {}

   int BasicConstructorTest(void)
   {
      TUDEF("ObsRngDev", "BasicConstructor");

      try
      {
         for (int i=0; i < cTimeVec.size(); i++)
         {
            gpstk::ObsRngDev ord(prange[i], id, cTimeVec[i], receiverPos, ephemStore, em);
            ordVec.push_back(ord);
         }
      }
      catch(gpstk::Exception e)
      {
         TUFAIL("unexpected exception");
      }

      failCount = 0;
      for (int i=0; i < ordVec.size(); i++)
         if(!(ordVec[i].obstime == cTimeVec[i]))
            failCount++;
      TUASSERT(failCount == 0);
      TUASSERT(ordVec.size() == 3);

      failCount = 0;
      for (int i=0; i < ordVec.size(); i++)
         if(!(ordVec[i].svid == id))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for (int i=0; i < ordVec.size(); i++)
         if(!(ordVec[i].health.get_value() == 0))
            failCount++;
      TUASSERT(failCount == 0);

      TURETURN();
   }

   int IonosphericConstructorTest(void)
   {
      TUDEF("ObsRngDev", "IonosphericConstructor");

      gpstk::IonoModelStore ims = ionoModelStoreGen(cTimeVec);
      gpstk::IonoModel::Frequency L1 = gpstk::IonoModel::L1;

      try
      {
         for (int i=0; i < cTimeVec.size(); i++)
         {
            gpstk::ObsRngDev ord(prange[i], id, cTimeVec[i], receiverPos,
                                 ephemStore, em, ims, L1);
            ordVecIon.push_back(ord);
         }
      }
      catch (gpstk::Exception e)
      {
         std::cout << e << std::endl;
         TUFAIL("unexpected exception");
      }
      
      failCount = 0;
      for (int i=0; i < ordVecIon.size(); i++)
         if(!(ordVecIon[i].obstime == cTimeVec[i]))
            failCount++;
      TUASSERT(failCount == 0);
      TUASSERT(ordVecIon.size() == 3);
	
      failCount = 0;
      for (int i=0; i < ordVecIon.size(); i++)
         if(!(ordVecIon[i].svid == id))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for (int i=0; i < ordVecIon.size(); i++)
         if(!(ordVecIon[i].health.get_value() == 0))
            failCount++;
      TUASSERT(failCount == 0);

      TURETURN();
   }

   int TroposphericConstructorTest(void)
   {
      TUDEF("ObsRngDev", "TroposphericConstructor");

      gpstk::SimpleTropModel stm(18.8889, 1021.2176, 77.7777); // Celsius, mmBar, %humidity		

      try
      {
         for (int i=0; i < cTimeVec.size(); i++)
         {
            gpstk::ObsRngDev ord(prange[i], id, cTimeVec[i],
                                 receiverPos, ephemStore, em, stm);
            ordVecTrop.push_back(ord);
         }
      }
      catch (gpstk::Exception e)
      {
         TUFAIL("unexpected exception");
      }
		
      failCount = 0;
      for (int i=0; i < ordVecTrop.size(); i++)
            //Have same timeVec for every 10 instances of ordVecTrop
         if(!(ordVecTrop[i].obstime == cTimeVec[i]))
            failCount++;
      TUASSERT(failCount == 0);
      TUASSERT(ordVecTrop.size() == 3);
	
      failCount = 0;
      for (int i=0; i < ordVecTrop.size(); i++)
         if(!(ordVecTrop[i].svid == id))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for (int i=0; i < ordVecTrop.size(); i++)
         if(!(ordVecTrop[i].health.get_value() == 0))
            failCount++;
      TUASSERT(failCount == 0);

      TURETURN();
   }

   int IonosphericTroposphericConstructorTest(void)
   {
      TUDEF("ObsRngDev", "IonosphericTroposphericConstructor");

      gpstk::SimpleTropModel stm(18.8889, 1021.2176, 77.7777);
      gpstk::IonoModelStore ims = ionoModelStoreGen(cTimeVec);
      gpstk::IonoModel::Frequency L1 = gpstk::IonoModel::L1;		

      try
      {
         for (int i=0; i < cTimeVec.size(); i++)
         {
            gpstk::ObsRngDev ord(prange[i], id, cTimeVec[i], receiverPos, 
                                 ephemStore, em, stm, ims, L1);
            ordVecTropIon.push_back(ord);
         }
      }
      catch (gpstk::Exception e)
      {
         TUFAIL("unexpected exception");
      }
      
      failCount = 0;
      for (int i=0; i < ordVecTropIon.size(); i++)
         if(!(ordVecTropIon[i].obstime == cTimeVec[i]))
            failCount++;
      TUASSERT(failCount == 0);
      TUASSERT(ordVecTropIon.size() == 3);
	
      failCount = 0;
      for (int i=0; i < ordVecTropIon.size(); i++)
         if(!(ordVecTropIon[i].svid == id))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for (int i=0; i < ordVecTropIon.size(); i++)
         if(!(ordVecTropIon[i].health.get_value() == 0))
            failCount++;
      TUASSERT(failCount == 0);

      TURETURN();
   }

   int GammaConstructorTest(void)
   {
      TUDEF("ObsRngDev", "GammaConstructor");
      
      try
      {
         for (int i=0; i < cTimeVec.size(); i++)
         {
            gpstk::ObsRngDev ord(prange[i], prange2[i], id, cTimeVec[i],
                                 receiverPos, ephemStore, em);
            ordVecGamma.push_back(ord);
         }
      }
      catch (gpstk::Exception e)
      {
         TUFAIL("unexpected exception");
      }
      
      failCount = 0;
      for (int i=0; i < ordVecGamma.size(); i++)
         if(!(ordVecGamma[i].obstime == cTimeVec[i]))
            failCount++;
      TUASSERT(failCount == 0);
      TUASSERT(ordVecGamma.size() == 3);
	
      failCount = 0;
      for (int i=0; i < ordVecGamma.size(); i++)
         if(!(ordVecGamma[i].svid == id))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for (int i=0; i < ordVecGamma.size(); i++)
         if(!(ordVecGamma[i].health.get_value() == 0))
            failCount++;
      TUASSERT(failCount == 0);

      TURETURN();
   }

   int GammaTroposphericConstructorTest(void)
   {
      TUDEF("ObsRngDev", "GammaTroposphericConstructor");

      gpstk::SimpleTropModel stm(18.8889, 1021.2176, 77.7777);
      try
      {
         for (int i=0; i < cTimeVec.size(); i++)
         {
            gpstk::ObsRngDev ord(prange[i], prange2[i], id, cTimeVec[i],
                                 receiverPos, ephemStore, em, stm);
            ordVecTropGamma.push_back(ord);
         }
      }
      catch (gpstk::Exception e)
      {
         TUFAIL("unexpected exception");
      }

      failCount = 0;
      for (int i=0; i < ordVecTropGamma.size(); i++)
         if(!(ordVecTropGamma[i].obstime == cTimeVec[i]))
            failCount++;
      TUASSERT(failCount == 0);
      TUASSERT(ordVecTropGamma.size() == 3);
	
      failCount = 0;
      for (int i=0; i < ordVecTropGamma.size(); i++)
         if(!(ordVecTropGamma[i].svid == id))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for (int i=0; i < ordVecTropGamma.size(); i++)
         if(!(ordVecTropGamma[i].health.get_value() == 0))
            failCount++;
      TUASSERT(failCount == 0);

      TURETURN();
   }

//===================================================================================
// Begin Get Functions Test
//===================================================================================

   int getFunctionsTest(void)
   {
      TUDEF("ObsRngDev", "Get Methods");

      TUASSERT(ordVec.size() > 0);

      failCount = 0;
      for(int i=0; i<ordVec.size(); i++)
         if (!(ordVec[i].getTime() == ordVec[i].obstime))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for(int i=0; i<ordVec.size(); i++)
         if (!(ordVec[i].getSvID() == ordVec[i].svid))
            failCount++;
      TUASSERT(failCount == 0);
      
      failCount = 0;
      for(int i=0; i<ordVec.size(); i++)
         if (!(ordVec[i].getAzimuth().get_value() == ordVec[i].azimuth.get_value()))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for(int i=0; i<ordVec.size(); i++)
         if (!(ordVec[i].getElevation().get_value() == ordVec[i].elevation.get_value()))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for(int i=0; i<ordVec.size(); i++)
         if (!(ordVec[i].getHealth().get_value() == ordVec[i].health.get_value()))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for(int i=0; i<ordVec.size(); i++)
         if (!(ordVec[i].getIODC().get_value() == ordVec[i].iodc.get_value()))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for(int i=0; i<ordVec.size(); i++)
         if (!(ordVec[i].getORD() == ordVec[i].ord))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for(int i=0; i<ordVec.size(); i++)
         if (!(ordVec[i].getIono().get_value() == ordVec[i].iono.get_value()))
            failCount++;
      TUASSERT(failCount == 0);

      failCount = 0;
      for(int i=0; i<ordVec.size(); i++)
         if (!(ordVec[i].getTrop().get_value() == ordVec[i].trop.get_value()))
            failCount++;
      TUASSERT(failCount == 0);

      TURETURN();
   }
	
   int BasicCalculationTest(void) 
   {
      TUDEF("ObsRngDev", "BasicCalculation");      
      TUASSERT(ordVec.size() > 0);

      for (int i=0; i < ordVec.size(); i++)
      {
         gpstk::CorrectedEphemerisRange cer;
         double rho = cer.ComputeAtTransmitTime(ordVec[i].obstime, prange[i], receiverPos, ordVec[i].svid, ephemStore);
         TUASSERTFE(prange[i] - rho - ordVec[i].trop, ordVec[i].ord);
         TUASSERTFE(ordVec[i].rho, rho);
         TUASSERTE(int, ordVec[i].azimuth, cer.azimuth);
         TUASSERTE(int, ordVec[i].elevation, cer.elevation);
      }

      TURETURN();
   }

   int IonosphericCalculationTest(void)
   {
      TUDEF("ObsRngDev", "IonosphericCalculation");      
      TUASSERT(ordVecIon.size() > 0);

      for (int i=0; i < ordVecIon.size(); i++)
      {
         gpstk::CorrectedEphemerisRange cer;
         double rho = cer.ComputeAtTransmitTime(ordVecIon[i].obstime, prange[i], receiverPos, ordVecIon[i].svid, ephemStore);
         TUASSERTFEPS(prange[i] - rho - ordVecIon[i].trop - ordVecIon[i].iono, ordVecIon[i].ord, 1e-6);
         TUASSERTFE(ordVecIon[i].rho, rho);
         TUASSERTE(int, ordVecIon[i].azimuth, cer.azimuth);
         TUASSERTE(int, ordVecIon[i].elevation, cer.elevation);
      }

      TURETURN();
   }

   int TroposphericCalculationTest(void)
   {
      TUDEF("ObsRngDev", "TroposphericCalculation");      
      TUASSERT(ordVecTrop.size() > 0);

      for (int i=0; i < ordVecTrop.size(); i++)
      {
         gpstk::CorrectedEphemerisRange cer;
         double rho = cer.ComputeAtTransmitTime(ordVecTrop[i].obstime, prange[i], receiverPos, ordVecTrop[i].svid, ephemStore);
         double CompareOrd = prange[i] - rho - ordVec[i].trop;
         TUASSERTFEPS(prange[i] - rho - ordVecTrop[i].trop, ordVecTrop[i].ord, 1e-6);
         TUASSERTFE(ordVecTrop[i].rho, rho);
         TUASSERTE(int, ordVecTrop[i].azimuth, cer.azimuth);
         TUASSERTE(int, ordVecTrop[i].elevation, cer.elevation);
      }

      TURETURN();
   }

   int IonosphericTroposphericCalculationTest(void)
   {
      TUDEF("ObsRngDev", "IonosphericTroposphericCalculation");      
      TUASSERT(ordVecTropIon.size() > 0);

      for (int i=0; i < ordVecTropIon.size(); i++)
      {
         gpstk::CorrectedEphemerisRange cer;
         double rho = cer.ComputeAtTransmitTime(ordVecTropIon[i].obstime, prange[i], receiverPos, ordVecTropIon[i].svid, ephemStore);
         TUASSERTFEPS(prange[i] - rho - ordVecTropIon[i].trop - ordVecTropIon[i].iono, ordVecTropIon[i].ord, 1e-6);
         TUASSERTFE(ordVecTropIon[i].rho, rho);
         TUASSERTE(int, ordVecTropIon[i].azimuth, cer.azimuth);
         TUASSERTE(int, ordVecTropIon[i].elevation, cer.elevation);
      }

      TURETURN();
   }

   int GammaCalculationTest(void)
   {
      TUDEF("ObsRngDev", "GammaCalculation");      
      TUASSERT(ordVecGamma.size() > 0);

      for (int i=0; i < ordVecGamma.size(); i++)
      {
         gpstk::CorrectedEphemerisRange cer;
         double rho = cer.ComputeAtTransmitTime(ordVecGamma[i].obstime, prange[i], receiverPos, ordVecGamma[i].svid, ephemStore);
         TUASSERTFEPS(prange[i] - rho - ordVecGamma[i].trop - ordVecGamma[i].iono, ordVecGamma[i].ord, 1e-4);
         TUASSERTFEPS(ordVecGamma[i].rho, rho, 1e-4);
         TUASSERTE(int, ordVecGamma[i].azimuth, cer.azimuth);
         TUASSERTE(int, ordVecGamma[i].elevation, cer.elevation);
      }

      TURETURN();
   }

   int TroposphericGammaCalculationTest(void)
   {
      TUDEF("ObsRngDev", "TroposphericGammaCalculation");      
      TUASSERT(ordVecTropGamma.size() > 0);

      for (int i=0; i < ordVecTropGamma.size(); i++)
      {
         gpstk::CorrectedEphemerisRange cer;
         double rho = cer.ComputeAtTransmitTime(ordVecTropGamma[i].obstime, prange[i], receiverPos, ordVecTropGamma[i].svid, ephemStore);
         TUASSERTFEPS(prange[i] - rho - ordVecTropGamma[i].trop - ordVecTropGamma[i].iono, ordVecTropGamma[i].ord, 1e-4);
         TUASSERTFEPS(ordVecTropGamma[i].rho, rho, 1e-4);
         TUASSERTE(int, ordVecTropGamma[i].azimuth, cer.azimuth);
         TUASSERTE(int, ordVecTropGamma[i].elevation, cer.elevation);
      }

      TURETURN();
   }

private:
   int failCount;
   gpstk::SatID id;
   std::vector<float> prange;
   std::vector<float> prange2;

   char *buff;

   std::vector<gpstk::ObsRngDev> ordVec;
   std::vector<gpstk::ObsRngDev> ordVecIon;
   std::vector<gpstk::ObsRngDev> ordVecTrop;
   std::vector<gpstk::ObsRngDev> ordVecTropIon;
   std::vector<gpstk::ObsRngDev> ordVecGamma;
   std::vector<gpstk::ObsRngDev> ordVecTropGamma;

   std::vector< std::map<int, float> > prnPrange;
   std::vector<gpstk::CommonTime> cTimeVec;
   gpstk::Position receiverPos;
   gpstk::RinexEphemerisStore ephemStore;
   gpstk::WGS84Ellipsoid em;
};


int main() //Main function to initialize and run all tests above
{
   int errorCounter = 0;
   ObsRngDev_T testClass;

   errorCounter += testClass.BasicConstructorTest();
   errorCounter += testClass.IonosphericConstructorTest();
   errorCounter += testClass.TroposphericConstructorTest();
   errorCounter += testClass.IonosphericTroposphericConstructorTest();
   errorCounter += testClass.GammaConstructorTest();
   errorCounter += testClass.GammaTroposphericConstructorTest();
   errorCounter += testClass.getFunctionsTest();
   errorCounter += testClass.BasicCalculationTest();
   errorCounter += testClass.IonosphericCalculationTest();
   errorCounter += testClass.TroposphericCalculationTest();
   errorCounter += testClass.IonosphericTroposphericCalculationTest();
   errorCounter += testClass.GammaCalculationTest();
   errorCounter += testClass.TroposphericGammaCalculationTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

   return errorCounter;
}
