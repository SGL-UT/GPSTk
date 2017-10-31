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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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
 * @file IERS.cpp
 * This class ease handling IERS earth orientation data.
 */

#include "IERS.hpp"
#include <string>
#include <fstream>
#include <cmath>
#include "MJD.hpp"
#include "JulianDate.hpp"

namespace gpstk
{
      // PI
   const double IERS::PI = 4.0 * std::atan(1.0);

      // Arcseconds to radius convention 
   const double IERS::ARCSEC2RAD =  PI / 180.0 / 3600.0;

   PlanetEphemeris IERS::jplEphemeris;

      // Load the JPL ephemeris from a binary file
   void IERS::loadBinaryEphemeris(const std::string ephFile)
      throw(Exception)
   {
      int rc = jplEphemeris.initializeWithBinaryFile(ephFile);
      if(rc!=0)
      {
         Exception e("Failed to load the JPL ephemeris '"+ephFile+"'.");
         GPSTK_THROW(e);
      }
   }

      /* Compute planet position and velocity in J2000
       *  
       * @param TT         Time(Modified Julian Date in TT<TAI+32.184>) of interest 
       * @param entity     The planet to be computed
       * @param center     relative to whick the result apply
       * @return           The position and velocity of the planet in km and km/s
       */
   Vector<double> IERS::planetJ2kPosVel( const CommonTime& TT, 
                                         PlanetEphemeris::Planet entity,
                                         PlanetEphemeris::Planet center )
   {
      Vector<double> rvJ2k(6,0.0);

      try
      {
         double rvState[6] = {0.0};
         int rc = jplEphemeris.computeState(JulianDate(TT).jd,entity, center, rvState);

         // change the unit to km/s from km/day
         rvState[3] /= 86400.0;
         rvState[4] /= 86400.0;
         rvState[5] /= 86400.0;

         if(rc == 0)
         {
            rvJ2k = rvState;
            return rvJ2k*1000.0; // km->m
         }
         else
         {
            rvJ2k.resize(6,0.0);

            // failed to compute
            InvalidRequest e("Failed to compute, error code: "
               +StringUtils::asString(rc)+" with meaning\n"
               +"-1 and -2 given time is out of the file \n"
               +"-3 and -4 input stream is not open or not valid,"
               +" or EOF was found prematurely");

            GPSTK_THROW(e);
         }
      }
      catch(...)
      {
         Exception e("Failed to compute positon and velocity from JPL ephemeris.");
         GPSTK_THROW(e);
      }

      return rvJ2k;
   }

   Vector<double> IERS::sunJ2kPosition(const CommonTime& TT)
   {
      Vector<double> pos(3,0.0);
      try
      {
         Vector<double> posvel = planetJ2kPosVel(TT,PlanetEphemeris::Sun);
         for(int i=0;i<3;i++) pos(i) = posvel(i);
         return pos;
      }
      catch(...)
      {
         return gpstk::sunJ2kPosition(TT);
      }
   }

   Vector<double> IERS::moonJ2kPosition(const CommonTime& TT)
   {
      Vector<double> pos(3,0.0);
      try
      {
         Vector<double> posvel = planetJ2kPosVel(TT,PlanetEphemeris::Moon);
         for(int i=0;i<3;i++) pos(i) = posvel(i);
         return pos;
      }
      catch(...)
      {
         return gpstk::moonJ2kPosition(TT);
      }
   }

   Vector<double> IERS::sunECEFPosition(const CommonTime& TT)
   {
      return J2kPosToECEF( GPST2UTC(TT), sunJ2kPosition(TT) );
   }

   Vector<double> IERS::moonECEFPosition(const CommonTime& TT)
   {
      return J2kPosToECEF( GPST2UTC(TT), moonJ2kPosition(TT) );
   }

}  // End of namespace 'gpstk'
