#pragma ident "$Id$"

/**
 * @file Xvt.hpp
 * Position and velocity as Triples, clock bias and drift as doubles.
 */

#ifndef GPSTK_XVT_INCLUDE
#define GPSTK_XVT_INCLUDE

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


#include <iostream>
#include "Triple.hpp"
#include "ReferenceFrame.hpp"
#include "EllipsoidModel.hpp"
#include "GNSSconstants.hpp"

namespace gpstk
{
  /** @addtogroup geodeticgroup */
  //@{

  /// Earth-Centered, Earth-Fixed Cartesian position, velocity, clock bias and drift
  class Xvt
  {
  public:

   /// Default constructor
   Xvt()
     : v(0.,0.,0.), clkdrift(0.), relcorr(0.)
   {}

   /// Destructor.
   virtual ~Xvt()
   {}

    Triple getPos()
      throw()
    { return x; }

    double getClockBias()
      throw()
    { return clkbias; }

   double preciseRho(const Triple& rxPos, 
                     const EllipsoidModel& ellipsoid,
                     double correction = 0) const
     throw();

   Triple x;        ///< Sat position ECEF Cartesian (X,Y,Z) meters
   double clkbias;  ///< Sat clock correction in seconds
   ReferenceFrame frame;

   Triple getVel() throw()
   { return v; }

   double getClockDrift() throw()
   { return clkdrift; }

   double getRelativityCorr() throw()
   { return relcorr; }

   /// Compute the relativity correction (-2R dot V/c^2) in seconds
   double computeRelativityCorrection(void)
   {
     // dtr = -2*dot(R,V)/(c*c) = -4.4428e-10(s/sqrt(m)) * ecc * sqrt(A(m)) * sinE
     relcorr = (-2.0*(x[0]*v[0] + x[1]*v[1] + x[2]*v[2])/C_GPS_MPS)/ C_GPS_MPS;
     return relcorr;
   }

//  protected:

   Triple v;        ///< satellite velocity in ECEF Cartesian, meters/second
   double clkdrift; ///< satellite clock drift in seconds/second
   double relcorr;  ///< relativity correction (standard ICD 2R.V/c^2 term), seconds
  };

  //@}

}

/**
 * Output operator for Xvt
 * @param s output stream to which \c xvt is sent
 * @param xvt Xvt that is sent to \c os
 */
std::ostream& operator<<(std::ostream& os, const gpstk::Xvt& xvt) throw();

#endif // GPSTK_XVT_INCLUDE
