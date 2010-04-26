#pragma ident "$Id: $"

/**
* @file EarthBody.hpp
* Class to handle earth planet, it'll be taken as the central
* body of the spacecraft.
*/

#ifndef GPSTK_EARTH_BODY_HPP
#define GPSTK_EARTH_BODY_HPP

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


#include "UTCTime.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /** Class to handle earth planet, it'll be taken as the central
       * body of the spacecraft.
       */
   class EarthBody
   {
   public:
         /// Default constructor
      EarthBody() {}

         /// Default destructor
      virtual ~EarthBody() {}
      
         /**
          * Returnts the dynamic Earth rotation rate. 
          * @t   epoch in UTC
          */
      virtual double getSpinRate(UTCTime t);
   

   protected:

         /// Earth's rotation rate in rad/s.
      static const double omegaEarth;

         /// Equatorial radius of earth in m from WGS-84
      static const double radiusEarth;
      
         /// Flattening factor of earth from WGS-84
      static const double flatEarth;   
      
         /// Earth gravity constant in m^3/s^2 WGS-84
      static const double gmEarth;

   }; // End of class 'EarthBody'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_EARTH_BODY_HPP



