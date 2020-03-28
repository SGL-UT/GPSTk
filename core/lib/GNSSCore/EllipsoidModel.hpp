//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file EllipsoidModel.hpp
 * Abstract base class modeling a Ellipsoid
 */

#ifndef GPSTK_ELLIPSOIDMODEL_HPP
#define GPSTK_ELLIPSOIDMODEL_HPP

namespace gpstk
{
      /// @ingroup Geodetic
      //@{

      /**
       * This abstract class encapsulates ellipsoid models (e.g. WGS84,
       * GPS, etc).
       */
   class EllipsoidModel
   {
   public:
         /// @return semi-major axis of Earth in meters.
      virtual double a() const noexcept = 0;

         /// @return semi-major axis of Earth in km.
      virtual double a_km() const noexcept = 0;

         /// @return flattening (ellipsoid parameter).
      virtual double flattening() const noexcept = 0;

         /// @return eccentricity (ellipsoid parameter).
      virtual double eccentricity() const noexcept = 0;

         /// @return eccentricity squared (ellipsoid parameter).
      virtual double eccSquared() const noexcept
      { return eccentricity() * eccentricity(); }

         /// @return angular velocity of Earth in radians/sec.
      virtual double angVelocity() const noexcept = 0;

         /// @return geocentric gravitational constant in m**3 / s**2
      virtual double gm() const noexcept = 0;

         /// @return geocentric gravitational constant in m**3 / s**2
      virtual double gm_km() const noexcept = 0;

         /// @return Speed of light in m/s.
      virtual double c() const noexcept = 0;

         /// @return Speed of light in km/s
      virtual double c_km() const noexcept = 0;

         /// Destructor.
      virtual ~EllipsoidModel() noexcept {};

   }; // class EllipsoidModel

      //@}

} // namespace

#endif
