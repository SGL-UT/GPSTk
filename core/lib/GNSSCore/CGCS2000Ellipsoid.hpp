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

/**
 * @file CGCS2000Ellipsoid.hpp
 * China Geodetic Coordinate System 2000 model of the Ellipsoid
 * Definition from BeiDou SIS ICD-2.0, Dec. 2013
 */

#ifndef GPSTK_CGCS2000ELLIPSOID_HPP
#define GPSTK_CGCS2000ELLIPSOID_HPP

#include "EllipsoidModel.hpp"

namespace gpstk
{
    /** @addtogroup geodeticgroup */
    //@{

      /// This class represents the ellipsoid model of the 
      /// China Geodetic Coordinate System 2000 model as defined
      /// in the BeiDou SIS ICD-2.0, Dec. 2013
   class CGCS2000Ellipsoid : public EllipsoidModel
   {
   public:
   
         /// Defined in BDS ICD Section 3.2
         /// @return semi-major axis of Earth in meters.
      virtual double a() const throw()
      { return 6378137.0; }

         /// Derived from BDS ICD Section 3.2
         /// @return semi-major axis of Earth in km.
      virtual double a_km() const throw()
      { return a() / 1000.0; }

         /// Defined in BDS ICD Section 3.2
         /// @return flattening (ellipsoid parameter).
      virtual double flattening() const throw()
      { return 0.335281068118e-2; }

         /// Unstated in BDS ICD. Derived as e = sqrt(2f - f*f)
         /// based on NGA TR8350.2 Section 7.4
         /// @return eccentricity (ellipsoid parameter).
      virtual double eccentricity() const throw()
      { return 8.1819191042816e-2; }

         /// Unstated in BDS ICD. Derived as e^2 = 2f - f*f
         /// based on NGA TR8350.2 Section 7.4
         /// @return eccentricity squared (ellipsoid parameter).
      virtual double eccSquared() const throw()
      { return 6.69438002290e-3; }

         /// Defined in BDS ICD Section 3.2
         /// @return angular velocity of Earth in radians/sec.
      virtual double angVelocity() const throw()
      { return 7.292115e-5; }

         /// Defined in BDS ICD Section 3.2
         /// @return geocentric gravitational constant in m**3 / s**2
      virtual double gm() const throw()
      { return 3986004.418e8; }

         /// @return geocentric gravitational constant in km**3 / s**2
      virtual double gm_km() const throw()
      { return 398600.4418; }

         /// Defined in BDS ICD Section 5.2.4.10
         /// @return Speed of light in m/s.
      virtual double c() const throw()
      { return 299792458; }

         /// Derived from BDS ICD Section 5.2.4.10
         /// @return Speed of light in km/s
      virtual double c_km() const throw()
      { return c()/1000.0; }

      /// Destructor.
      virtual ~CGCS2000Ellipsoid() throw() {};

   }; // class CGCS2000Ellipsoid

   //@}

} // namespace

#endif
