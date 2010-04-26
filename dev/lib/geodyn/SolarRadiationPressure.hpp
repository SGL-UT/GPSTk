#pragma ident "$Id: $"

/**
 * @file SolarRadiationPressure.hpp
 * This class provides a generic model for the influence
 * of solar radiation pressure on a satellite.
 */

#ifndef GPSTK_SOLAR_RADIATION_PRESSURE_HPP
#define GPSTK_SOLAR_RADIATION_PRESSURE_HPP


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

#include "ForceModel.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
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
      virtual std::string modelName()
      { return "SolarRadiationPressure"; }

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


