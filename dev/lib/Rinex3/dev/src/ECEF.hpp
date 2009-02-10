#pragma ident "$Id: ECEF.hpp 155 2006-09-26 18:13:37Z architest $"



/**
 * @file ECEF.hpp
 * Encapsulate Earth-centered, Earth-fixed Cartesian coordinates
 */

#ifndef GPSTK_ECEF_HPP
#define GPSTK_ECEF_HPP

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






#include "Triple.hpp"
#include "Exception.hpp"
#include "GeoidModel.hpp"

namespace gpstk
{
   class Geodetic;

      /** @addtogroup geodeticgroup */
      //@{

      /**
       * Earth centered, earth fixed geodetic coordinates in meters.
       */
   class ECEF : public Triple
   {
   public:
         /// Default constructor.
      ECEF();

         /// Copy constructor.
      ECEF(const ECEF& right);

         /// Construct from three doubles.
         /// @param x x coordinate in meters
         /// @param y y coordinate in meters
         /// @param z z coordinate in meters
      ECEF(const double& x, const double& y, const double& z)
            : Triple(x, y, z)
      { }

         /// Construct from a Triple.  The Triple consists of the x coordinate, 
         /// the y coordinate, and the z coordinate.  All in meters.
         /// @param t the Triple to copy from.
      ECEF(const Triple& t)
            : Triple(t)
      { }

         /// destructor
      virtual ~ECEF() {}

         /// Assignment operator.
      ECEF& operator=(const ECEF& right);

         /** \deprecated Convert to Geodetic
          *  Geodetic has a constructor that takes an ECEF and GeoidModel.
          *  \sa Geodetic, GeoidModel
          */
      Geodetic asGeodetic(GeoidModel *g);

   }; // class ECEF

      //@}

} // namespace gpstk

#endif
