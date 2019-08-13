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
 * @file SatOrbit.hpp
 * This Class handle general orbit for different satellite orbit type.
 */

#ifndef GPSTK_SAT_ORBIT_HPP
#define GPSTK_SAT_ORBIT_HPP

#include "EquationOfMotion.hpp"
#include "EarthBody.hpp"
#include "Spacecraft.hpp"
#include "ForceModelList.hpp"

#include "SphericalHarmonicGravity.hpp"
#include "SunForce.hpp"
#include "MoonForce.hpp"
#include "AtmosphericDrag.hpp"
#include "SolarRadiationPressure.hpp"
#include "RelativityEffect.hpp"

namespace gpstk
{

      /// @ingroup GeoDynamics 
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
         GM_EGM96
      };

         /// Valid atmospheric models
      enum AtmosphericModel
      {
         AM_HarrisPriester,
         AM_MSISE00,
         AM_CIRA
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

         // We'll allocate memory in the heap for some of the models are memory
         // consuming
         
         SphericalHarmonicGravity* pGeoEarth;
         
         SunForce* pGeoSun;
         
         MoonForce* pGeoMoon;
         
         AtmosphericDrag* pAtmDrag;
         
         SolarRadiationPressure* pSolarPressure;
         
         RelativityEffect* pRelEffect;

         double dailyF107;
         double averageF107;
         double dailyKp;

         FMCData()
         {
            geoEarth = true;
            geoSun = geoMoon = false;
            atmDrag = false;
            relEffect = false;
            solarPressure = false;

            grvModel = GM_JGM3;
            grvDegree = 1;
            grvOrder = 1;

            solidTide = oceanTide = poleTide = false;

            atmModel = AM_HarrisPriester;

            pGeoEarth = NULL;
            pGeoSun   = NULL;
            pGeoMoon  = NULL;
            pAtmDrag  = NULL;
            pSolarPressure  = NULL;
            pRelEffect      = NULL;

            dailyF107 = 150.0;
            averageF107 = 150.0;
            dailyKp = 3.0;
         }
      };

   public:

         /// Default constructor
      SatOrbit() : fmlPrepared(false)
      { reset(); }

         /// Default destructor
      virtual ~SatOrbit()
      { deleteFMObjects(forceConfig); }

       
      virtual Vector<double> getDerivatives(const double&         t,
                                            const Vector<double>& y );

         /// Restore the default setting
      SatOrbit& reset()
      {deleteFMObjects(forceConfig);fmlPrepared = false;init();return(*this);}

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
                                  const double& Cd = 2.2);

         /// set space data
      SatOrbit& setSpaceData(double dayF107 = 150.0,
                             double aveF107 = 150.0, 
                             double dayKp = 3.0);
      

         // Methods to config the orbit perturbation force models
         // call 'reset()' before call these methods

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


      SatOrbit& enableSolarRadiationPressure(bool bsrp = false);


      SatOrbit& enableRelativeEffect(const bool& brel = false);


         /// For POD , and it's will be improved later
      void setForceModelType(std::set<ForceModel::ForceModelType> fmt)
      { forceList.setForceModelType(fmt); }


   protected:

      virtual void init();

      void createFMObjects(FMCData& fmc);

      void deleteFMObjects(FMCData& fmc);

         /**
          * Adds a generic force to the list
          */
      void addForce(ForceModel* pForce)
      { forceList.addForce(pForce); }

         /// Reference epoch
      UTCTime utc0;
      
         /// Spacecraft object
      Spacecraft sc;

      ///  Earth Body
      EarthBody  earthBody;     

         /// Object holding force model consiguration
      FMCData forceConfig;
  
         /// Flag indicate if the ForceModelList has been prepared
         /// 'forceConfig' can't be change when 'fmlPrepared' is true
      bool fmlPrepared;

         /// Force Model List
      ForceModelList forceList;

         // Objected

   }; // End of class 'SatOrbit'

}  // End of namespace 'gpstk'

#endif   // GPSTK_SAT_ORBIT_HPP
