#pragma ident "$Id: $"

/**
 * @file SunForce.hpp
 * This class calculate the gravitational effect of the Sun.
 */

#ifndef GPSTK_SUN_FORCE_HPP
#define GPSTK_SUN_FORCE_HPP


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
       * This class calculate the gravitational effect of the Sun.
       *
       *  This Model is checked on Sep 25th,2009, OK!!!
       */
   class SunForce : public ForceModel
   {
   public:
         /// Default constructor
      SunForce();

         /// Default destructor
      ~SunForce(void){};


         /** Call the relevant methods to compute the acceleration.
          * @param t  Time reference class
          * @param rb Reference Body class
          * @param sc Spacecraft parameters and state
          * @return the acceleration [m/s^s]
          */
      virtual void doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc);


         /// Return force model name
      virtual std::string modelName()  const
      { return "SunForce"; }


         /// return the force model index
      virtual int forceIndex() const
      { return FMI_GEOSUN; }

      void test();

   protected:

         /// Gravitational constant for the body. [m^3/s^2]
      double mu;


   }; // End of class 'SunForce'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_SUN_FORCE_HPP


