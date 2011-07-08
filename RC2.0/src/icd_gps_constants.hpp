#pragma ident "$Id: icd_gps_constants.hpp 1709 2009-02-18 20:27:47Z btolman $"

/**
 * @file icd_gps_constants.hpp
 * Constants as defined in the GPS-ICD-200D
 */

#ifndef GPSTK_ICD_GPS_CONSTANTS_HPP
#define GPSTK_ICD_GPS_CONSTANTS_HPP

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
// This software developed by Applied Research Laboratories at the University
// of Texas at Austin, under contract to an agency or agencies within the U.S. 
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software. 
//
// Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================
#include "Exception.hpp"

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
      /// Hz, chip rate of the P & Y codes
   const double PY_CHIP_FREQ = OSC_FREQ;
      /// Hz, chip rate of the C/A code
   const double CA_CHIP_FREQ = OSC_FREQ / 10.0;
      /// speed of light (m/s)
   const double C_GPS_M = 2.99792458e8;
      /// Base freq w/o relativisitic effects, Hz.
   const double RSVCLK    = 10.22999999543e6;
      /// L1 carrier frequency in Hz.
   const double L1_FREQ   = 1575.42e6;
      /// L1 carrier wavelength in meters.
   const double L1_WAVELENGTH  = 0.190293672798;
      /// L2 carrier frequency in Hz.
   const double L2_FREQ   = 1227.60e6;
      /// L2 carrier wavelength in meters.
   const double L2_WAVELENGTH  = 0.244210213425;
      /// L1 multiplier.
   const double L1_MULT   = 154.0;
      /// L2 multiplier.
   const double L2_MULT   = 120.0;
      /// Gamma multiplier.
   const double GAMMA_GPS = 1.646944444;
     ///Reference Semi-major axis. From IS-GPS-800 Table 3.5-2 in meters.
   const double A_REF = 26559710.0;

   // Modernized GPS
      /// L5 carrier frequency in Hz.
   const double L5_FREQ   = 1176.45e6;
      /// L5 carrier wavelength in meters.
   const double L5_WAVELENGTH  = 0.254828049;
      /// L5 multiplier.
   const double L5_MULT   = 115.0;

   // Galileo-related
      /// L6 carrier frequency in Hz.
   const double L6_FREQ   = 1278.75e6;
      /// L6 carrier wavelength in meters.
   const double L6_WAVELENGTH  = 0.234441805;
      /// L7 carrier frequency in Hz.
   const double L7_FREQ   = 1207.140e6;
      /// L7 carrier wavelength in meters.
   const double L7_WAVELENGTH  = 0.24834937;
      /// L8 carrier frequency in Hz.
   const double L8_FREQ   = 1191.795e6;
      /// L8 carrier wavelength in meters.
   const double L8_WAVELENGTH  = 0.251547001;

      /// constant for the max array index in sv accuracy table
   const int SV_ACCURACY_MAX_INDEX_VALUE = 15;

      /// map from SV accuracy/URA flag to minimum accuracy values in m
   const double SV_ACCURACY_MIN_INDEX[] = {0, 2.4, 3.4, 4.85, 6.85, 9.65,
                                           13.65, 24.0, 48.0, 96.0, 192.0,
                                           384.0, 768.0, 1536.0, 3072.0,
                                           6144.0};
      /// Map from SV accuracy/URA flag to NOMINAL accuracy values in m
      /// Further details in ICD-GPS-200C, section 20.3.3.3.1.3
   const double SV_ACCURACY_NOMINAL_INDEX[] = {2.0, 2.8, 4.0, 5.7, 8.0,
                                           11.3, 16.0, 32.0, 64.0, 128.0,
                                           256.0, 512.0, 1024.0, 2048.0,
                                           4096.0, 9.999999999999e99};
      /// map from SV accuracy/URA flag to maximum accuracy values in m
   const double SV_ACCURACY_MAX_INDEX[] = {2.4, 3.4, 4.85, 6.85, 9.65,
                                           13.65, 24.0, 48.0, 96.0, 192.0,
                                           384.0, 768.0, 1536.0, 3072.0,
                                           6144.0, 9.999999999999e99};

      /// Accuracy for CNAV and CNAV2

      /// constant for the max array index in sv accuracy table
   const int SV_CNAV_ACCURACY_MAX_INDEX_VALUE = 15;

      /// map from SV accuracy/URA flag to minimum accuracy values in m
   const double SV_CNAV_ACCURACY_MIN_INDEX[] = {0.0, 0.01, 0.02, 0.03, 0.04, 0.06, 
                                           0.08, 0.11, 0.15, 0.21, 0.30,
                                           0.43, 0.60, 0.85, 1.2, 1.7,
                                           2.4, 3.4, 4.85, 6.85, 9.65,
                                           13.65, 24.0, 48.0, 96.0, 192.0,
                                           384.0, 768.0, 1536.0, 3072.0,
                                           6144.0};

      /// map from SV accuracy/URA flag to maximum accuracy values in m
   const double SV_CNAV_ACCURACY_MAX_INDEX[] = {0.01, 0.02, 0.03, 0.04, 0.06,
                                           0.08, 0.11, 0.15, 0.21, 0.30,
                                           0.43, 0.60, 0.85, 1.20, 1.7,
                                           2.4, 3.4, 4.85, 6.85, 9.65,
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

   inline
   short nominalAccuracy2ura(const double& acc) throw()
   {
      short ura = 0;
      while ( (ura <= SV_ACCURACY_MAX_INDEX_VALUE) &&
              (acc > SV_ACCURACY_NOMINAL_INDEX[ura]))
         ura++;
      if (ura > SV_ACCURACY_MAX_INDEX_VALUE)
         ura = SV_ACCURACY_MAX_INDEX_VALUE;
      return ura;
   }
   
   inline
   double ura2nominalAccuracy(const short& ura) throw()
   {
      if(ura < 0)
         return SV_ACCURACY_NOMINAL_INDEX[0];
      if(ura > SV_CNAV_ACCURACY_MAX_INDEX_VALUE)
         return SV_ACCURACY_NOMINAL_INDEX[SV_ACCURACY_MAX_INDEX_VALUE];
      return SV_ACCURACY_NOMINAL_INDEX[ura];
   }

   inline
   short accuracy2CNAVura(const double& acc) throw()
   {
      short ura = -15;
      while ( (ura <= SV_CNAV_ACCURACY_MAX_INDEX_VALUE) &&
              (acc > SV_CNAV_ACCURACY_MAX_INDEX[ura+15]))
         ura++;
      if (ura > SV_CNAV_ACCURACY_MAX_INDEX_VALUE)
         ura = SV_CNAV_ACCURACY_MAX_INDEX_VALUE;
      return ura;
   }
   
   inline
   double ura2CNAVaccuracy(const short& ura) throw()
   {
      if(ura > SV_CNAV_ACCURACY_MAX_INDEX_VALUE)
         return SV_CNAV_ACCURACY_MAX_INDEX[SV_CNAV_ACCURACY_MAX_INDEX_VALUE+15];
      return SV_CNAV_ACCURACY_MAX_INDEX[ura+15];
   }

   inline
   short getLegacyFitInterval(const short iodc, const short fiti)
      throw( InvalidRequest )
   {
         /* check the IODC */
      if (iodc < 0 || iodc > 1023)
      {
            /* error in iodc, return minimum fit */
         return 4;
      }
      
      if (((fiti == 0) &&
           (iodc & 0xFF) < 240 || (iodc & 0xFF) > 255 ))
      {
            /* fit interval of 4 hours */
         return 4;
      }
      else if (fiti == 1)
      {
         if( ((iodc & 0xFF) < 240 || (iodc & 0xFF) > 255))
         {
               /* fit interval of 6 hours */
            return 6;
         }
         else if(iodc >=240 && iodc <=247)
         {
               /* fit interval of 8 hours */
            return 8;
         }
         else if(iodc >= 248 && iodc <= 255 || iodc == 496)
         {
               /* fit interval of 14 hours */
            return 14;
         }
         else if((iodc >= 497 && iodc <=503) || (iodc >= 1021 && iodc <= 1023))
         {
               /* fit interval of 26 hours */
            return 26;
         }
         else if(iodc >= 504 && iodc <=510)
         {
               /* fit interval of 50 hours */
            return 50;
         }
         else if(iodc == 511 || iodc >= 752 && iodc <= 756)
         {
               /* fit interval of 74 hours */
            return 74;
         }
         else if(iodc == 757)
         {
               /* fit interval of 98 hours */
            return 98;
         }
         else
         {
            InvalidRequest exc("Invalid IODC Value For sv Block");
            GPSTK_THROW(exc);
         }
      }
      else
      {
            /* error in ephemeris/iodc, return minimum fit */
         return 4;
      }
      
      return 0; // never reached
   }

      //@}
   
} // namespace

#endif
