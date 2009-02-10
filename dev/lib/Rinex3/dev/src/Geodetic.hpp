#pragma ident "$Id: Geodetic.hpp 70 2006-08-01 18:36:21Z ehagen $"



/**
 * @file Geodetic.hpp
 * Geodetic coordinates (lat, lon, alt)
 */

#ifndef GPSTK_GEODETIC_HPP
#define GPSTK_GEODETIC_HPP

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
#include "ECEF.hpp"

namespace gpstk
{
      /** @addtogroup geodeticgroup */
      //@{

      /// Thrown when the internal gpstk::GeoidModel isn't assigned.
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(NoGeoidException, gpstk::Exception);

      /**
       * Specialization of triple that represents geodetic coordinates
       * (latitude in degrees, longitude in degrees, altitude in m).
       */
   class Geodetic : public Triple
   {
   public:
         /// Default constructor.
      Geodetic();

         /// Copy constructor.
      Geodetic(const Geodetic& right);

         /** Construct from three doubles. Be careful you don't
          * specify a geoid that will disappear (e.g. one on the stack
          * where these coordinates will be used outside that context).
          * @param lat latitude in degrees.
          * @param lon longitude in degrees.
          * @param alt altitude in m.
          * @param geo geoid model for transformations.
          */
      Geodetic(const double& lat, const double& lon, const double& alt, 
               GeoidModel* geo = NULL);

         /** Construct from a Triple and a GeoidMode.  The triple is latitude
          * in degrees, longitude in degrees, and altitude in meters.  Be 
          * careful you don't specify a geoid that will disappear (e.g. one 
          * on the stack where these coordinates will be used outside that 
          * context).
          * @param t the Triple to copy from.
          * @param geo geoid model for transformations.
          */
      Geodetic(const Triple& t, GeoidModel* geo = NULL);

         /** Construct from an ECEF and a GeoidModel.  Converts Earth-centered, 
          * Earth-fixed XYZ coordinates (m) to Geodetic coordinates 
          * (lat,lon(E),ht) (deg,degE,m).  Be careful you don't
          * specify a geoid that will disappear (e.g. one on the stack
          * where these coordinates will be used outside that context).
          * @param right the ECEF to construct from.
          * @param geo geoid model for transformations.
          */
      Geodetic(const ECEF& right, GeoidModel* geo)
         throw(NoGeoidException);

         /// destructor
      virtual ~Geodetic() {}

         /// Assignment operator.
      Geodetic& operator=(const Geodetic& right);

         /** Convert to Earth-centered, Earth-fixed XYZ coordinates. */
      gpstk::ECEF asECEF() const throw(NoGeoidException);

      double getLatitude() const throw() { return operator[](0); }
      double getLongitude() const throw() { return operator[](1); }
      double getAltitude() const throw() { return operator[](2); }

   protected:
      GeoidModel *geoid;
   }; // class Geodetic

   //@}

} // namespace gpstk

#endif
