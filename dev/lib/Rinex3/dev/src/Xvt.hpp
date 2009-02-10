#pragma ident "$Id: Xvt.hpp 148 2006-09-26 16:03:15Z architest $"



/**
 * @file Xvt.hpp
 * Position, velocity, and clock representation as ECEF, Triple and double
 */

#ifndef GPSTK_XVT_HPP
#define GPSTK_XVT_HPP

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
#include "ECEF.hpp"
#include "GeoidModel.hpp"

namespace gpstk
{
    /** @addtogroup geodeticgroup */
    //@{

      /// An Earth-Centered, Earth-Fixed position/velocity/clock representation
   class Xvt
   {
   public:
         /// Default constructor
      Xvt() { }

      ECEF x;         ///< SV position (x,y,z). Earth-fixed. meters
      Triple v;       ///< SV velocity. Earth-fixed, including rotation. meters/sec
      double dtime;   ///< SV clock correction in seconds
      double ddtime;  ///< SV clock drift in sec/sec

         /**
          * Given the position of a ground location, compute the range
          * to the spacecraft position.
          * @param rxPos ground position at broadcast time in ECEF.
          * @param geoid geodetic parameters.
          * @param correction offset in meters (include any factors other
          * than the SV clock correction).
          * @return Range in meters
          */
      double preciseRho(const ECEF& rxPos, 
                        const GeoidModel& geoid,
                        double correction = 0) const
         throw();
   }; 

   //@}

}

/**
 * Output operator for Xvt
 * @param s output stream to which \c xvt is sent
 * @param xvt Xvt that is sent to \c s
 */
std::ostream& operator<<( std::ostream& s, 
                          const gpstk::Xvt& xvt );

#endif
