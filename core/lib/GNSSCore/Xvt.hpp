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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file Xvt.hpp
 * Position and velocity as Triples, clock bias and drift as doubles.
 */

#ifndef GPSTK_XVT_INCLUDE
#define GPSTK_XVT_INCLUDE

#include <iostream>
#include "Triple.hpp"
#include "EllipsoidModel.hpp"
#include "ReferenceFrame.hpp"
#include "GNSSconstants.hpp"

namespace gpstk
{
      /** @addtogroup geodeticgroup */
      //@{

      /** Earth-Centered, Earth-Fixed Cartesian position, velocity,
       * clock bias and drift */
   class Xvt
   {
   public:
         /** Health of satellite at the time of the PVT computation.
          * A more detailed explanation of each enumeration is as follows:
          * \li Uninitialized - This is used to indicate that the
          *     health status has never been properly set.  This
          *     generally indicates a coding error in the library.
          * \li Unavailable - This is used to indicate that orbit (or
          *     health, in the case of getSVHealth calls) information
          *     is not available for the satellite at the time of
          *     interest.
          * \li Unused - This indicates that the XvtStore class or
          *     other class generating this Xvt or health status does
          *     not actually contain or provide health information.
          *     An example of this is the SP3EphemerisStore.
          * \li Unknown - This is used to indicate that the store was
          *     able to provide a PVT, but for whatever reason was not
          *     able to determine the health state of the satellite.
          * \li Unhealthy - This indicates that the satellite is known
          *     to be in an unhealthy state at the time of interest.
          * \li Degraded - This indicates that the satellite is known
          *     to be in a degraded state at the time of interest
          *     (currently only Galileo supports such a state).
          * \li Healthy - This indicates that the satellite is known
          *     to be in a healthy state.
          */
      enum HealthStatus
      {
         MinValue,
         Uninitialized = MinValue, ///< Health status has not been set.
         Unavailable, ///< Orbit information was not available, PVT invalid.
         Unused,      ///< Sat health is not used in computing this PVT. 
         Unknown,     ///< Health state is unknown.
         Unhealthy,   ///< Sat is marked unhealthy, do not use PVT.
         Degraded,    ///< Sat is in a degraded state, recommend do not use.
         Healthy,     ///< Satellite is healthy, PVT valid.
         MaxValue = Healthy
      };

         /// Default constructor
      Xvt() : x(0.,0.,0.), v(0.,0.,0.),
              clkbias(0.), clkdrift(0.),
              relcorr(0.), frame(ReferenceFrame::Unknown),
              health(Uninitialized)
      {};

         /// Destructor.
      virtual ~Xvt()
      {}

         /// access the position, ECEF Cartesian in meters
      Triple getPos() throw()
      { return x; }

         /// access the velocity in m/s
      Triple getVel() throw()
      { return v; }

         /// access the clock bias, in second
      double getClockBias() throw()
      { return clkbias; }

         /// access the clock drift, in second/second
      double getClockDrift() throw()
      { return clkdrift; }

         /// access the relativity correction, in seconds
      double getRelativityCorr() throw()
      { return relcorr; }

         /** Compute and return the relativity correction (-2R dot
          * V/c^2) in seconds.  
          * @note -2*dot(R,V)/(c*c) =
          *   -4.4428e-10(s/sqrt(m)) * ecc * sqrt(A(m)) * sinE
          */
      virtual double computeRelativityCorrection(void);

         /** Given the position of a ground location, compute the range
          * to the spacecraft position.
          * @param[in] rxPos ground position at broadcast time in ECEF.
          * @param[in] ellipsoid geodetic parameters.
          * @param[in] correction offset in meters (include any factors other
          *   than the SV clock correction and the relativity correction).
          * @return Range in meters */
      double preciseRho(const Triple& rxPos, 
                        const EllipsoidModel& ellipsoid,
                        double correction = 0) const throw();

         // member data

      Triple x;        ///< Sat position ECEF Cartesian (X,Y,Z) meters
      Triple v;        ///< satellite velocity in ECEF Cartesian, meters/second
      double clkbias;  ///< Sat clock correction in seconds
      double clkdrift; ///< satellite clock drift in seconds/second
      double relcorr;  ///< relativity correction (standard 2R.V/c^2 term), seconds
      ReferenceFrame frame;   ///< reference frame of this data
      HealthStatus health;    ///< Health status of satellite at ref time.

   }; // end class Xvt

      //@}


      /**
       * Output operator for Xvt
       * @param[in,out] os output stream to which \c xvt is sent
       * @param[in] xvt Xvt that is sent to \c os
       */
   std::ostream& operator<<(std::ostream& os, const Xvt& xvt) throw();

      /**
       * Output operator for Xvt health status.
       * @param[in,out] os output stream to which \c health is sent
       * @param[in] health Health status that is sent to \c os
       */
   std::ostream& operator<<(std::ostream& os, const Xvt::HealthStatus& health)
      throw();

}  // end namespace gpstk

#endif // GPSTK_XVT_INCLUDE
