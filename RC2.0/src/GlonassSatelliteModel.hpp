#pragma ident "$Id"

/**
 * @file GlonassSatelliteModel.hpp
 * This class implements a simple model of a GLONASS satellite.
 */

#ifndef GPSTK_GLONASSSATELLITEMODEL_HPP
#define GPSTK_GLONASSSATELLITEMODEL_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011
//
//============================================================================


#include "RK4VehicleModel.hpp"
#include "PZ90Ellipsoid.hpp"


namespace gpstk
{

      /** @defgroup vehicles Tools to model vehicles */
      //@{

      /// This class implements a simple kinematic model of a vehicle.
   class GlonassSatelliteModel : public RK4VehicleModel
   {
   public:


         /** Common constructor.
          *
          * @param initialState     Initial state matrix.
          * @param initialTime      Initial time.
          * @param timeEpsilon      Time epsilon for Runge-Kutta algorithm.
          */
      GlonassSatelliteModel( const Matrix<double>& initialState,
                             double initialTime = 0.0,
                             double timeEpsilon = 1e-18 )
         : RK4VehicleModel(initialState, initialTime, timeEpsilon)
      { Init(); };


   private:


         /** Implements "derivative()". It is based on accelerations.
          *
          * @param time          Time step.
          * @param inState       Internal state matrix.
          * @param inStateDot    Derivative of internal state matrix.
          */
      virtual Matrix<double>& derivative( long double time,
                                          const Matrix<double>& inState,
                                          Matrix<double>& inStateDot );


         // Internal initialization method
      void Init(void);

         // Some internal variables we will need
      PZ90Ellipsoid pz90;
      double j20, mu, ae, we, we2;


   };  // End of class "GlonassSatelliteModel"

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_GLONASSSATELLITEMODEL_HPP
