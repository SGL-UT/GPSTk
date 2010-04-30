
/**
 * @file SatOrbit.cpp
 * This Class handle general orbit for different satellite orbit type.
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================


#include "SatOrbit.hpp"

namespace gpstk
{

   void SatOrbit::updateForceModelConfiguration(const FMCData& fmc)
   {
      forceList.clear();

         // Geopotential force
      SphericalHarmonicGravity* pGravityModel = & grvJGM3Force;

      if( fmc.grvModel == GM_EGM96) pGravityModel = &grvEGM96Firce;
 
      
      pGravityModel->setDesiredDegree(fmc.grvDegree, fmc.grvOrder);

      pGravityModel->enableSolidTide(fmc.solidTide);
      pGravityModel->enableOceanTide(fmc.oceanTide);
      pGravityModel->enablePoleTide(fmc.poleTide);

      forceList.addForce(pGravityModel);

         // Three body
      if(fmc.geoSun) forceList.addForce(&sunForce);
      if(fmc.geoMoon) forceList.addForce(&moonForce);

         // Atmospheric drag force
      AtmosphericDrag* pAtmDrag = &dragHPForce;

      if(fmc.atmModel == AM_MSISE00) pAtmDrag = &dragMSISEForce;
      if(fmc.atmModel == AM_CIRA) pAtmDrag = &dragCiraForce;

      if(fmc.atmDrag) forceList.addForce(pAtmDrag);

         // Solar Radiation Pressure
      if(fmc.solarPressure) forceList.addForce(&srpForce);

         // Relative Effect
      if(fmc.relEffect) forceList.addForce(&relForce);

   }  // End of method 'SatOrbit::updateForceModelConfiguration()'


   void SatOrbit::updateSpacecraftData(const SpacecraftData& scd)
   {
      sc.setName(scd.scName);
      sc.setDryMass(scd.scMass);
      sc.setDragArea(scd.scArea);
      sc.setSRPArea(scd.scAreaSRP);
      sc.setDragCoeff(scd.scCd);
      sc.setReflectCoeff(scd.scCr);

   }  // End of method 'SatOrbit::updateSpacecraftData()'

   // get derivative dy/dt
   Vector<double> SatOrbit::getDerivatives(const double&         t,
                                           const Vector<double>& y)
   {
      // inport the state vector to sc
      sc.setStateVector(y);

      UTCTime utc = utc0 + t;
      return forceList.getDerivatives(utc, earthBody, sc);
   }

   void SatOrbit::init()
   {
      setSpacecraftData("sc-test01",1000.0,20.0,20.0,1.0,2.0);
      updateSpacecraftData(spacecraftConfig);


      enableGeopotential(GM_JGM3,1,1,false,false,false);
      enableThirdBodyPerturbation(false,false);
      enableAtmosphericDrag(AM_HarrisPriester,false);
      enableSolarRadiationPressure(false);
      enableRelativeEffect(false);
     
      updateForceModelConfiguration(forceConfig);
     

   }  // End of method 'SatOrbit::init()'


   SatOrbit& SatOrbit::setSpacecraftData(std::string name,
                                         const double& mass,
                                         const double& area,
                                         const double& areaSRP,
                                         const double& Cr,
                                         const double& Cd)
   {
      spacecraftConfig.scName = name;
      spacecraftConfig.scMass = mass;
      spacecraftConfig.scArea = area;
      spacecraftConfig.scAreaSRP = areaSRP;
      spacecraftConfig.scCr = Cr;
      spacecraftConfig.scCd = Cd;

      return (*this);

   }  // End of method 'SatOrbit::setSpacecraftData()'


   SatOrbit& SatOrbit::enableGeopotential(SatOrbit::GravityModel model,
                                          const int& maxDegree,
                                          const int& maxOrder,
                                          const bool& solidTide,
                                          const bool& oceanTide,
                                          const bool& poleTide)
   {
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
      forceConfig.geoSun = bsun;
      forceConfig.geoMoon = bmoon;

      return (*this);

   }  // End of method 'SatOrbit::enableThirdBodyPerturbation()'


   SatOrbit& SatOrbit::enableAtmosphericDrag(SatOrbit::AtmosphericModel model,
                                             const bool& bdrag)
   {
      forceConfig.atmModel = model;
      forceConfig.atmDrag = bdrag;

      return (*this);

   }  // End of method 'SatOrbit::enableAtmosphericDrag()'

}

