#pragma ident "$Id$"



/**
 * @file icd_200_constants.hpp
 * Constants as defined in the GPS-ICD-200
 */

#ifndef GPSTK_ICD_200_CONSTANTS_HPP
#define GPSTK_ICD_200_CONSTANTS_HPP

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






namespace gpstk
{
      /**
       * @addtogroup geodeticgroup
       * @name GPS Constants
       * Time constants are in the time library.
       */
      //@{
      /// GPS value of PI
   const double PI        = 3.1415926535898;
      /// GPS value of PI*2
   const double TWO_PI    = 6.2831853071796;
      /// GPS value of PI**0.5
   const double SQRT_PI   = 1.7724539;
      /// relativity constant for GPS (sec/sqrt(m))
   const double REL_CONST = -4.442807633e-10;
      /// Hz
   const double OSC_FREQ  = 10.23e6;
      /// speed of light (m/s)
   const double C_GPS_M = 2.99792458e8;
      /// Base freq w/o relativisitic effects, Hz.
   const double RSVCLK    = 10.22999999545e6;
      /// L1 carrier frequency in Hz.
   const double L1_FREQ   = 1575.42e6;
      /// L2 carrier frequency in Hz.
   const double L2_FREQ   = 1227.60e6;
      /// L1 multiplier.
   const double L1_MULT   = 154.0;
      /// L2 multiplier.
   const double L2_MULT   = 120.0;
      /// constant for the max array index in sv accuracy table
   const int SV_ACCURACY_MAX_INDEX_VALUE = 15;
      /// map from SV accuracy/URA flag to minimum accuracy values in m
   const double SV_ACCURACY_MIN_INDEX[] = {0, 2.4, 3.4, 4.85, 6.85, 9.65,
                                           13.65, 24.0, 48.0, 96.0, 192.0,
                                           384.0, 768.0, 1536.0, 3072.0,
                                           6144.0};
      /// map from SV accuracy/URA flag to maximum accuracy values in m
   const double SV_ACCURACY_MAX_INDEX[] = {2.4, 3.4, 4.85, 6.85, 9.65,
                                           13.65, 24.0, 48.0, 96.0, 192.0,
                                           384.0, 768.0, 1536.0, 3072.0,
                                           6144.0, 9.999999999999e99};

   inline
   short accuracy2ura(const double& acc) throw()
   {
      short ura = 0;
      while ( (ura <= SV_ACCURACY_MAX_INDEX_VALUE) &&
              (acc > SV_ACCURACY_MAX_INDEX[ura]))
         ura++;
      if (ura > SV_ACCURACY_MAX_INDEX_VALUE)
         ura = SV_ACCURACY_MAX_INDEX_VALUE;
      return ura;
   }
   
   inline
   double ura2accuracy(const short& ura) throw()
   {
      if(ura < 0)
         return SV_ACCURACY_MAX_INDEX[0];
      if(ura > SV_ACCURACY_MAX_INDEX_VALUE)
         return SV_ACCURACY_MAX_INDEX[SV_ACCURACY_MAX_INDEX_VALUE];
      return SV_ACCURACY_MAX_INDEX[ura];
   }
      //@}
   
} // namespace

#endif
