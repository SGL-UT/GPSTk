#pragma ident "$Id: $"

/**
 * @file SatOrbit.hpp
 * This Class handle general orbit for different satellite orbit type.
 */

#ifndef GPSTK_SAT_ORBIT_HPP
#define GPSTK_SAT_ORBIT_HPP


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

#include "EquationOfMotion.hpp"
#include "EarthBody.hpp"
#include "Spacecraft.hpp"
#include "ForceModelList.hpp"

#include "MoonForce.hpp"
#include "SunForce.hpp"

#include "JGM3GravityModel.hpp"
#include "EGM96GravityModel.hpp"

#include "HarrisPriesterDrag.hpp"
#include "Msise00Drag.hpp"
#include "CiraExponentialDrag.hpp"

#include "SolarRadiationPressure.hpp"
#include "RelativityEffect.hpp"

namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

  
      /** This Class handle general orbit for different satellite orbit type.
       * 
       */
   class SatOrbit : public EquationOfMotion
   {
   public:
         /// Valid gravity models
      enum GravityModel
      {
         GM_JGM3,
         GM_EGM96,
      };

         /// Valid atmospheric models
      enum AtmosphericModel
      {
         AM_HarrisPriester,
         AM_MSISE00,
         AM_CIRA,
      };

         /// Struct to hold force model setting data
      struct FMCData
      {
         bool geoEarth;
         bool geoSun;
         bool geoMoon;
         bool atmDrag;
         bool relEffect;
         bool solarPressure;

         GravityModel grvModel;
         int grvDegree;
         int grvOrder;

         bool solidTide;
         bool oceanTide;
         bool poleTide;

         AtmosphericModel atmModel;
      };

      struct SpacecraftData
      {
         string   scName;         //< default is 'sc-test01'         
         double   scMass;         //< default is 1000.0 kg
         double   scArea;         //< default is 20 m^2
         double   scAreaSRP;      //< for SRP
         double   scCr;           //< 1.0 Coefficient of Reflectivity
         double   scCd;           //< default is 2.0
      };

   public:

         /// Default constructor
      SatOrbit()
      { init(); }

         /// Default destructor
      virtual ~SatOrbit(){}

       
      virtual Vector<double> getDerivatives(const double&         t,
                                            const Vector<double>& y );

         /// set reference epoch
      SatOrbit& setRefEpoch(UTCTime utc)
      { utc0 = utc; return (*this); }

         /// get reference epoch
      UTCTime getRefEpoch() const
      { return utc0; }

         /// set spacecraft physical parameters
      SatOrbit& setSpacecraftData(std::string name = "sc-test01",
                                  const double& mass = 1000.0,
                                  const double& area = 20.0,
                                  const double& areaSRP = 20.0,
                                  const double& Cr = 1.0,
                                  const double& Cd = 2.0);


         // Methods to config the orbit perturbation force models

      SatOrbit& enableGeopotential(SatOrbit::GravityModel model = SatOrbit::GM_JGM3,
                                   const int& maxDegree = 1,
                                   const int& maxOrder = 1,
                                   const bool& solidTide = false,
                                   const bool& oceanTide = false,
                                   const bool& poleTide = false);

      SatOrbit& enableThirdBodyPerturbation(const bool& bsun = false,
                                            const bool& bmoon = false);


      SatOrbit& enableAtmosphericDrag(SatOrbit::AtmosphericModel model 
                                                 = SatOrbit::AM_HarrisPriester,
                                      const bool& bdrag = false);


      SatOrbit& enableSolarRadiationPressure(bool bsrp = false)
      { forceConfig.solarPressure = bsrp; return (*this);}


      SatOrbit& enableRelativeEffect(const bool& brel = false)
      { forceConfig.relEffect = brel; return (*this);}

         /// For POD , and it's will be improved later
      void setForceModelType(std::set<ForceModel::ForceModelType> fmt)
      { forceList.setForceModelType(fmt); }

   protected:

      virtual void init();

      void updateForceModelConfiguration(const FMCData& fmc);


      void updateSpacecraftData(const SpacecraftData& scd);


         /**
          * Adds a generic force to the list
          */
      void addForce(ForceModel* pForce)
      { forceList.addForce(pForce); }

         /// Reference epoch
      UTCTime utc0;
      
         /// Spacecraft object
      Spacecraft sc;

         /// Spacecraft physical parameter
      SpacecraftData spacecraftConfig;

         /// Object holding force model consiguration
      FMCData forceConfig;
  

         ///  Earth Body
      EarthBody  earthBody;         
      
        
         /// Force Model List
      ForceModelList forceList;

   private:

         // Objects force model besides earth force

      MoonForce              moonForce;
      SunForce               sunForce;

      JGM3GravityModel       grvJGM3Force;
      EGM96GravityModel      grvEGM96Firce;

      HarrisPriesterDrag     dragHPForce;
      Msise00Drag            dragMSISEForce;
      CiraExponentialDrag    dragCiraForce;

      SolarRadiationPressure srpForce;
      RelativityEffect       relForce;

   }; // End of class 'SatOrbit'

}  // End of namespace 'gpstk'

#endif   // GPSTK_SAT_ORBIT_HPP



