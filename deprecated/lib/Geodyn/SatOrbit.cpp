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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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

/**
 * @file SatOrbit.cpp
 * This Class handle general orbit for different satellite orbit type.
 */

#include "SatOrbit.hpp"
#include "JGM3GravityModel.hpp"
#include "EGM96GravityModel.hpp"
#include "HarrisPriesterDrag.hpp"
#include "Msise00Drag.hpp"
#include "CiraExponentialDrag.hpp"

namespace gpstk
{
   // get derivative dy/dt
   Vector<double> SatOrbit::getDerivatives(const double&         t,
                                           const Vector<double>& y)
   {
      if(fmlPrepared == false)
      {
         createFMObjects(forceConfig);
      }

      // import the state vector to sc
      sc.setStateVector(y);

      UTCTime utc = utc0;
      utc += t;
      return forceList.getDerivatives(utc, earthBody, sc);
   }

   void SatOrbit::init()
   {
      setSpacecraftData("sc-test01",1000.0,20.0,20.0,1.0,2.2);
 
      enableGeopotential(GM_JGM3,1,1,false,false,false);
      enableThirdBodyPerturbation(false,false);
      enableAtmosphericDrag(AM_HarrisPriester,false);
      enableSolarRadiationPressure(false);
      enableRelativeEffect(false);

   }  // End of method 'SatOrbit::init()'


   SatOrbit& SatOrbit::setSpacecraftData(std::string name,
                                         const double& mass,
                                         const double& area,
                                         const double& areaSRP,
                                         const double& Cr,
                                         const double& Cd)
   {
      sc.setName(name);
      sc.setDryMass(mass);
      sc.setDragArea(area);
      sc.setSRPArea(areaSRP);
      sc.setDragCoeff(Cd);
      sc.setReflectCoeff(Cr);

      return (*this);

   }  // End of method 'SatOrbit::setSpacecraftData()'

   SatOrbit& SatOrbit::setSpaceData(double dayF107,
                                    double aveF107, 
                                    double dayKp)
   {
      forceConfig.dailyF107 = dayF107;
      forceConfig.averageF107 = aveF107;
      forceConfig.dailyKp = dayKp;

      return (*this);
   }

   SatOrbit& SatOrbit::enableGeopotential(SatOrbit::GravityModel model,
                                          const int& maxDegree,
                                          const int& maxOrder,
                                          const bool& solidTide,
                                          const bool& oceanTide,
                                          const bool& poleTide)
   {
      // DONOT allow to change the configuration 
      if(fmlPrepared) return (*this);    

      forceConfig.geoEarth = true;

      forceConfig.grvModel = model;
      forceConfig.grvDegree = maxDegree;
      forceConfig.grvOrder = maxOrder;

      forceConfig.solidTide = solidTide;
      forceConfig.oceanTide = oceanTide;
      forceConfig.poleTide = poleTide;

      return (*this);

   }  // End of method 'SatOrbit::enableGeopotential()'


   SatOrbit& SatOrbit::enableThirdBodyPerturbation(const bool& bsun,
                                                   const bool& bmoon)
   {
      // DONOT allow to change the configuration 
      if(fmlPrepared) return (*this); 

      forceConfig.geoSun = bsun;
      forceConfig.geoMoon = bmoon;

      return (*this);

   }  // End of method 'SatOrbit::enableThirdBodyPerturbation()'


   SatOrbit& SatOrbit::enableAtmosphericDrag(SatOrbit::AtmosphericModel model,
                                             const bool& bdrag)
   {
      // DONOT allow to change the configuration 
      if(fmlPrepared) return (*this); 

      forceConfig.atmModel = model;
      forceConfig.atmDrag = bdrag;

      return (*this);

   }  // End of method 'SatOrbit::enableAtmosphericDrag()'


   SatOrbit& SatOrbit::enableSolarRadiationPressure(bool bsrp)
   { 
      // DONOT allow to change the configuration 
      if(fmlPrepared) return (*this);  

      forceConfig.solarPressure = bsrp; 
      
      return (*this);
   
   }  // End of method 'SatOrbit::enableSolarRadiationPressure()'


   SatOrbit& SatOrbit::enableRelativeEffect(const bool& brel)
   {
      // DONOT allow to change the configuration 
      if(fmlPrepared) return (*this);  

      forceConfig.relEffect = brel; 

      return (*this);

   }  // End of method 'SatOrbit::enableRelativeEffect()' 

   void SatOrbit::createFMObjects(FMCData& fmc)
   {
      // First, we release the memory
      deleteFMObjects(fmc);

      // 

      // GeoEarth
      if(fmc.grvModel == GM_JGM3)
      {
         fmc.pGeoEarth = new JGM3GravityModel();
      }
      else if(fmc.grvModel == GM_EGM96)
      {
         fmc.pGeoEarth = new EGM96GravityModel();
      }
      else
      {
         // Unexpected, never go here
      }

      // GeoSun
      fmc.pGeoSun = new SunForce();

      // GeoMoon
      fmc.pGeoMoon = new MoonForce();

      // AtmDrag
      if(fmc.atmModel == AM_HarrisPriester)
      {
         fmc.pAtmDrag = new HarrisPriesterDrag();
      }
      else if(fmc.atmModel == AM_MSISE00)
      {
         fmc.pAtmDrag = new Msise00Drag();
      }
      else if(fmc.atmModel == AM_CIRA)
      {
         fmc.pAtmDrag = new CiraExponentialDrag();
      }
      else
      {
         // Unexpected, never go here
      }

      // SRP
      fmc.pSolarPressure = new SolarRadiationPressure();
     
      // Rel
      fmc.pRelEffect = new RelativityEffect();
      
      // Now, it's time to check if we create these objects successfully
      if( !fmc.pGeoEarth || !fmc.pGeoSun || !fmc.pGeoMoon ||
          !fmc.pAtmDrag || !fmc.pSolarPressure || !fmc.pRelEffect )
      {
         // deallocate allocated memory
         deleteFMObjects(fmc);

         Exception e("Failed to allocate memory for force models !");
         GPSTK_THROW(e);
      }

      // At last, prepare the force model list
      fmc.pGeoEarth->setDesiredDegree(fmc.grvDegree,fmc.grvOrder);
      fmc.pGeoEarth->enableSolidTide(fmc.solidTide);
      fmc.pGeoEarth->enableOceanTide(fmc.oceanTide);
      fmc.pGeoEarth->enablePoleTide(fmc.poleTide);

      fmc.pAtmDrag->setSpaceData(fmc.dailyF107,
         fmc.averageF107, fmc.dailyKp);

      forceList.clear();

      if(fmc.geoEarth) forceList.addForce(fmc.pGeoEarth);
      if(fmc.geoSun) forceList.addForce(fmc.pGeoSun);
      if(fmc.geoMoon) forceList.addForce(fmc.pGeoMoon);
      if(fmc.atmDrag) forceList.addForce(fmc.pAtmDrag);
      if(fmc.solarPressure) forceList.addForce(fmc.pSolarPressure);
      if(fmc.relEffect) forceList.addForce(fmc.pRelEffect);

      // set the flag
      fmlPrepared = true;

   }  // End of method 'SatOrbit::createFMObjects()'


   void SatOrbit::deleteFMObjects(FMCData& fmc)
   {
      // GeoEarth
      if(fmc.pGeoEarth)
      {
         if(fmc.grvModel == GM_JGM3)
         {
            delete (JGM3GravityModel*) fmc.pGeoEarth;
            fmc.pGeoEarth = NULL;
         }
         else if(fmc.grvModel == GM_EGM96)
         {
            delete (EGM96GravityModel*) fmc.pGeoEarth;
            fmc.pGeoEarth = NULL;
         }
         else
         {
            delete fmc.pGeoEarth;
            fmc.pGeoEarth = NULL;
         }
      }
      
      // GeoSun
      if(fmc.pGeoSun)
      {
         delete fmc.pGeoSun;
         fmc.pGeoSun = NULL;
      }
      
      // GeoMoon
      if(fmc.pGeoMoon)
      {
         delete fmc.pGeoMoon;
         fmc.pGeoMoon = NULL;
      }

      // AtmDrag
      if(fmc.pAtmDrag)
      {
         if(fmc.atmModel == AM_HarrisPriester)
         {
            delete (HarrisPriesterDrag*) fmc.pAtmDrag;
            fmc.pAtmDrag = NULL;
         }
         else if(fmc.atmModel == AM_MSISE00)
         {
            delete (Msise00Drag*) fmc.pAtmDrag;
            fmc.pAtmDrag = NULL;
         }
         else if(fmc.atmModel == AM_CIRA)
         {
            delete (CiraExponentialDrag*) fmc.pAtmDrag;
            fmc.pAtmDrag = NULL;
         }
         else
         {
            delete fmc.pAtmDrag;
            fmc.pAtmDrag = NULL;
         }
      }

      // SRP
      if(fmc.pSolarPressure)
      {
         delete fmc.pSolarPressure;
         fmc.pSolarPressure = NULL;
      }

      // Rel
      if(fmc.pRelEffect)
      {
         delete fmc.pRelEffect;
         fmc.pRelEffect = NULL;
      }

      

      // set the flag
      fmlPrepared = false;

   }  // End of method 'SatOrbit::uninstallForceModelList()'


}  // End of namespace 'gpstk'
