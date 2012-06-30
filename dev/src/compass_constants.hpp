#pragma ident "$Id$"

/**
 * @file gps_constants.hpp
 * Miscellaneous constants for Compass.
 */

#ifndef GPSTK_COMPASS_CONSTANTS_HPP
#define GPSTK_COMPASS_CONSTANTS_HPP

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
//  Wei Yan - R & D Center, TJISM. 2012
//
//============================================================================


namespace gpstk
{
   /** @addtogroup ephemcalc */
   //@{

      /** The maximum number of active satellites in the Compass
       * constellation. */
   const long MAX_PRN_COM     = 30;

   // ---------------- COMPASS ----------------------------------
      /// COM B1 carrier frequency in Hz.   
   const double B1_FREQ_COM   = 1561.098e6;  // B1-E2-L2
      /// COM B2 carrier frequency in Hz.   
   const double B2_FREQ_COM   = 1207.140e6;  // B2-E5b-L7
      /// COM B3 carrier frequency in Hz.   
   const double B3_FREQ_COM   = 1268.520e6;  // B3-E6-L6
      
      /// COM B1 carrier wavelength in meters
   const double B1_WAVELENGTH_COM  = 0.192039486310276;
      /// COM B2 carrier wavelength in meters.
   const double B2_WAVELENGTH_COM  = 0.248349369584307; //0.24834937;
      /// COM B3 carrier wavelength in meters.
   const double B3_WAVELENGTH_COM  = 0.236332464604421; //0.236332246460442;


   //@}

} // namespace 

#endif
