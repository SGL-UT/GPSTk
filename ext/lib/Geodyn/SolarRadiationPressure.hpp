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
 * @file SolarRadiationPressure.hpp
 * This class provides a generic model for the influence
 * of solar radiation pressure on a satellite.
 */

#ifndef GPSTK_SOLAR_RADIATION_PRESSURE_HPP
#define GPSTK_SOLAR_RADIATION_PRESSURE_HPP

#include "ForceModel.hpp"


namespace gpstk
{
      /// @ingroup GeoDynamics 
      //@{

      /**
       * This class provides a generic model for the influence
       * of solar radiation pressure on a satellite.
       * 
       */
   class SolarRadiationPressure : public ForceModel
   {
   public:
      enum ShadowModel 
      {
         SM_CYLINDRICAL,         // cylindrical
         SM_CONICAL               // conical
      };

   public:
         /// Default constructor.
      SolarRadiationPressure() {}

         /// Default destructor
      virtual ~SolarRadiationPressure() {}


         /** Determines if the satellite is in sunlight or shadow.
          * Taken from Montenbruck and Gill p. 80-83
          * @param r ECI position vector of spacecraft [m].
          * @param r_Sun Sun position vector (geocentric) [m].
          * @param r_Moon Moon position vector (geocentric) [m].
          * @return 0.0 if in shadow, 1.0 if in sunlight, 0 to 1.0 if in partial shadow
          */
      double getShadowFunction(Vector<double> r, 
                               Vector<double> r_Sun,
                               Vector<double> r_Moon, 
                               SolarRadiationPressure::ShadowModel sm = SM_CONICAL);

         // this is the real one
      virtual void doCompute(UTCTime t, EarthBody& bRef, Spacecraft& sc);

         /// Return force model name
      virtual std::string modelName() const
      { return "SolarRadiationPressure"; }

         /// return the force model index
      virtual int forceIndex() const
      { return FMI_SRP; }

         /**Compute the acceleration due to a solar radiation pressure.
          * @param r ECI position vector [m].
          * @param r_Sun ECI position vector of Sun in m.
          * @return Acceleration due to solar radiation pressure in m/s^2.
          */
      Vector<double> accelSRP(Vector<double> r, Vector<double> r_Sun);


         /** Determines if the satellite is in sunlight or shadow based on simple cylindrical shadow model.
          * Taken from Montenbruck and Gill p. 80-83
          * @param r ECI position vector of spacecraft [m].
          * @param r_Sun Sun position vector (geocentric) [m].
          * @return 0.0 if in shadow, 1.0 if in sunlight, 0 to 1.0 if in partial shadow
          */
      double partial_illumination(Vector<double> r, Vector<double> r_Sun );

    
   protected:      

      // some local variables

         /// Cross sectional (reflective) area [m^2]
      double crossArea;
      double srpArea;

         /// Mass [kg] of the satellite
      double dryMass;

         /// Satellite coefficient of reflectivity 
      double reflectCoeff;      // CR

         /// Object hold da/dCr
      Vector<double> dadcr;

   }; // End of class 'SolarRadiationPressure'

      // @}

}  // End of namespace 'gsptk'

#endif   // GPSTK_SOLAR_RADIATION_PRESSURE_HPP
