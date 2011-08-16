#pragma ident "$Id: GNSSconstants.hpp 10 2011-06-15 21:38:12Z btolman $"

/**
 * @file GNSSconstants.hpp
 * Constants as defined in the GPS-ICD-200D and by all RINEX GNSSs
 */

#ifndef GPSTK_GNSS_CONSTANTS_HPP
#define GPSTK_GNSS_CONSTANTS_HPP

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
       * @name GNSS Constants                   // BWT ? have one @name for each GNSS?
       * Time constants are in the time library.
       */
      //@{

   // ---------------- independent of GNSS ----------------------
      /// GPS value of PI
   const double PI        = 3.1415926535898;
      /// GPS value of PI*2
   const double TWO_PI    = 6.2831853071796;
      /// GPS value of PI**0.5
   const double SQRT_PI   = 1.7724539;
      /// relativity constant (sec/sqrt(m))        // BWT change rmv for GPS
   const double REL_CONST = -4.442807633e-10;
      /// m/s, speed of light; this value defined by ICD_GPS_200C and applies to GALILEO and GLONASS.
   const double C_GPS_MPS = 2.99792458e8;	   // CNQ renamed from C_GPS_M

   // ---------------- GPS --------------------------------------
      /// Hz, GPS Oscillator or chip frequency
   const double OSC_FREQ_GPS  = 10.23e6;           // BWT change add _GPS everywhere
      /// Hz, GPS chip rate of the P & Y codes
   const double PY_CHIP_FREQ_GPS = OSC_FREQ_GPS;
      /// Hz, GPS chip rate of the C/A code
   const double CA_CHIP_FREQ_GPS = OSC_FREQ_GPS / 10.0;
      /// Hz, GPS Base freq w/o relativisitic effects
   const double RSVCLK_GPS    = 10.22999999543e6;
      /// GPS L1 carrier frequency in Hz
   const double L1_FREQ_GPS   = 1575.42e6;
      /// GPS L2 carrier frequency in Hz
   const double L2_FREQ_GPS   = 1227.60e6;
      /// GPS L5 carrier frequency in Hz.
   const double L5_FREQ_GPS   = 1176.45e6;
      /// GPS L1 carrier wavelength in meters
   const double L1_WAVELENGTH_GPS  = 0.190293672798;
      /// GPS L2 carrier wavelength in meters
   const double L2_WAVELENGTH_GPS  = 0.244210213425;
      /// GPS L5 carrier wavelength in meters.
   const double L5_WAVELENGTH_GPS  = 0.254828049;
      /// GPS L1 frequency in units of oscillator frequency
   const double L1_MULT_GPS   = 154.0;
      /// GPS L2 frequency in units of oscillator frequency
   const double L2_MULT_GPS   = 120.0;
      /// GPS L5 frequency in units of oscillator frequency.
   const double L5_MULT_GPS   = 115.0;
      /// GPS Gamma constant
   const double GAMMA_GPS = 1.646944444;   // BWT this is for freq 1,2: keep? rename?
     ///Reference Semi-major axis. From IS-GPS-800 Table 3.5-2 in meters.
   const double A_REF_GPS = 26559710.0;
     ///WGS-84 value of the earth's rotation rate.  From ICD-GPS-200C pp. 98 in rad/sec.
   const double OMEGADOT_REF_GPS=7.2021151467e-5;

   // BWT add _GPS after _ACCURACY
      /// constant for the max array index in sv accuracy table
   const int SV_ACCURACY_GPS_MAX_INDEX_VALUE = 15;
      /// map from SV accuracy/URA flag to minimum accuracy values in m
   const double SV_ACCURACY_GPS_MIN_INDEX[] = {0.0, 2.4, 3.4, 4.85, 6.85, 9.65,
                                               13.65, 24.0, 48.0, 96.0, 192.0,
                                               384.0, 768.0, 1536.0, 3072.0,
                                               6144.0};
      /// Map from SV accuracy/URA flag to NOMINAL accuracy values in m
      /// Further details in ICD-GPS-200C, section 20.3.3.3.1.3
   const double SV_ACCURACY_GPS_NOMINAL_INDEX[] = {2.0, 2.8, 4.0, 5.7, 8.0,
                                                   11.3, 16.0, 32.0, 64.0, 128.0,
                                                   256.0, 512.0, 1024.0, 2048.0,
                                                   4096.0, 9.999999999999e99};
      /// map from SV accuracy/URA flag to maximum accuracy values in m
   const double SV_ACCURACY_GPS_MAX_INDEX[] = {2.4, 3.4, 4.85, 6.85, 9.65,
                                               13.65, 24.0, 48.0, 96.0, 192.0,
                                               384.0, 768.0, 1536.0, 3072.0,
                                               6144.0, 9.999999999999e99};
      /// constant for the max array index in sv accuracy table
   const int SV_CNAV_ACCURACY_GPS_MAX_INDEX_VALUE = 15;

      /// map from SV accuracy/URA flag to minimum accuracy values in m
   const double SV_CNAV_ACCURACY_GPS_MIN_INDEX[] = {0.0, 0.01, 0.02, 0.03, 0.04, 0.06,
                                           0.08, 0.11, 0.15, 0.21, 0.30,
                                           0.43, 0.60, 0.85, 1.2, 1.7,
                                           2.4, 3.4, 4.85, 6.85, 9.65,
                                           13.65, 24.0, 48.0, 96.0, 192.0,
                                           384.0, 768.0, 1536.0, 3072.0,
                                           6144.0};

      /// map from SV accuracy/URA flag to maximum accuracy values in m
   const double SV_CNAV_ACCURACY_GPS_MAX_INDEX[] = {0.01, 0.02, 0.03, 0.04, 0.06,
                                           0.08, 0.11, 0.15, 0.21, 0.30,
                                           0.43, 0.60, 0.85, 1.20, 1.7,
                                           2.4, 3.4, 4.85, 6.85, 9.65,
                                           13.65, 24.0, 48.0, 96.0, 192.0,
                                           384.0, 768.0, 1536.0, 3072.0,
                                           6144.0, 9.999999999999e99};


   // BWT ? add GPS to function names?
   inline
   short accuracy2ura(const double& acc) throw()
   {
      short ura = 0;
      while ( (ura <= SV_ACCURACY_GPS_MAX_INDEX_VALUE) &&
              (acc > SV_ACCURACY_GPS_MAX_INDEX[ura]))
         ura++;
      if (ura > SV_ACCURACY_GPS_MAX_INDEX_VALUE)
         ura = SV_ACCURACY_GPS_MAX_INDEX_VALUE;
      return ura;
   }
   
   inline
   double ura2accuracy(const short& ura) throw()
   {
      if(ura < 0)
         return SV_ACCURACY_GPS_MAX_INDEX[0];
      if(ura > SV_ACCURACY_GPS_MAX_INDEX_VALUE)
         return SV_ACCURACY_GPS_MAX_INDEX[SV_ACCURACY_GPS_MAX_INDEX_VALUE];
      return SV_ACCURACY_GPS_MAX_INDEX[ura];
   }

   inline
   short nominalAccuracy2ura(const double& acc) throw()
   {
      short ura = 0;
      while ( (ura <= SV_ACCURACY_GPS_MAX_INDEX_VALUE) &&
              (acc > SV_ACCURACY_GPS_NOMINAL_INDEX[ura]))
         ura++;
      if (ura > SV_ACCURACY_GPS_MAX_INDEX_VALUE)
         ura = SV_ACCURACY_GPS_MAX_INDEX_VALUE;
      return ura;
   }
   
   inline
   double ura2nominalAccuracy(const short& ura) throw()
   {
      if(ura < 0)
         return SV_ACCURACY_GPS_NOMINAL_INDEX[0];
      if(ura > SV_ACCURACY_GPS_MAX_INDEX_VALUE)
         return SV_ACCURACY_GPS_NOMINAL_INDEX[SV_ACCURACY_GPS_MAX_INDEX_VALUE];
      return SV_ACCURACY_GPS_NOMINAL_INDEX[ura];
   }

   inline
   short accuracy2CNAVura(const double& acc) throw()
   {
      short ura = -15;
      while ( (ura <= SV_CNAV_ACCURACY_GPS_MAX_INDEX_VALUE) &&
              (acc > SV_CNAV_ACCURACY_GPS_MAX_INDEX[ura+15]))
         ura++;
      if (ura > SV_CNAV_ACCURACY_GPS_MAX_INDEX_VALUE)
         ura = SV_CNAV_ACCURACY_GPS_MAX_INDEX_VALUE;
      return ura;
   }

   inline
   double ura2CNAVaccuracy(const short& ura) throw()
   {
      if(ura > SV_CNAV_ACCURACY_GPS_MAX_INDEX_VALUE)
         return SV_CNAV_ACCURACY_GPS_MAX_INDEX[SV_CNAV_ACCURACY_GPS_MAX_INDEX_VALUE+15];
      return SV_CNAV_ACCURACY_GPS_MAX_INDEX[ura+15];
   }
   // ---------------- GLONASS ----------------------------------
                                                // BWT from icd_glo_constants.hpp
      /// GLO Fundamental chip rate in Hz.
   const double OSC_FREQ_GLO  = 5.11e6;         // BWT change CHIP to OSC
      /// GLO Chip rate of the P & Y codes in Hz.
   const double PY_CHIP_FREQ_GLO = OSC_FREQ_GLO;
      /// GLO Chip rate of the C/A code in Hz.
   const double CA_CHIP_FREQ_GLO = OSC_FREQ_GLO / 10.0;

      /// GLO Fundamental oscillator freq in Hz.
   const double PSC_FREQ_GLO  = 5.00e6;
      /// GLO Base freq w/o relativisitic effects in Hz.
   const double RSVCLK_GLO    = 4.99999999782e6;

      /// GLO L1 carrier base frequency in Hz.
   const double L1_FREQ_GLO       = 1602.0e6;
      /// GLO L1 carrier frequency step size in Hz.
   const double L1_FREQ_STEP_GLO  = 562.5e3;
      /// GLO L1 carrier wavelength in meters.
   const double L1_WAVELENGTH_GLO = 0.187136365793;
      /// GLO L2 carrier base frequency in Hz.
   const double L2_FREQ_GLO       = 1246.0e6;
      /// GLO L2 carrier frequency step size in Hz.
   const double L2_FREQ_STEP_GLO  = 437.5e3;
      /// GLO L2 carrier wavelength in meters.
   const double L2_WAVELENGTH_GLO = 0.240603898876;
      /// GLO L1 multiplier.
   const double L1_MULT_GLO   = 320.4;
      /// GLO L2 multiplier.
   const double L2_MULT_GLO   = 249.2;
      /// GLO Gamma multiplier.
   const double GAMMA_GLO = 1.653061224490;

      /// Constant for the max array index in SV accuracy table.
   const int SV_ACCURACY_GLO_INDEX_MAX = 15;
      /// Map from SV accuracy/URA flag to NOMINAL accuracy values in m.
      /// Further details in ICD-GLO-v5.0, Table 4.4 in Section 4.4.
   const double SV_ACCURACY_GLO_INDEX[] = { 1.0,  2.0,   2.5,   4.0,   5.0,
                                            7.0, 10.0,  12.0,  14.0,  16.0,
                                           32.0, 64.0, 128.0, 256.0, 512.0,
                                           9.999999999999e99               };

   // ---------------- Galileo ----------------------------------
      /// GAL L1 carrier frequency in Hz
   const double L1_FREQ_GAL   = L1_FREQ_GPS;    // BWT add 1 and 5
      /// GAL L5 carrier frequency in Hz.
   const double L5_FREQ_GAL   = L5_FREQ_GPS;
      /// GAL L6 carrier frequency in Hz.
   const double L6_FREQ_GAL   = 1278.75e6;      // BWT add _GAL everywhere
      /// GAL L7 carrier frequency in Hz.
   const double L7_FREQ_GAL   = 1207.140e6;
      /// GAL L8 carrier frequency in Hz.
   const double L8_FREQ_GAL   = 1191.795e6;
      /// GAL L1 carrier wavelength in meters
   const double L1_WAVELENGTH_GAL  = L1_WAVELENGTH_GPS;
      /// GAL L5 carrier wavelength in meters.
   const double L5_WAVELENGTH_GAL  = L5_WAVELENGTH_GPS;
      /// GAL L6 carrier wavelength in meters.
   const double L6_WAVELENGTH_GAL  = 0.234441805;
      /// GAL L7 carrier wavelength in meters.
   const double L7_WAVELENGTH_GAL  = 0.24834937;
      /// GAL L8 carrier wavelength in meters.
   const double L8_WAVELENGTH_GAL  = 0.251547001;

   // ---------------- Geostationary (SBAS) ---------------------   // BWT add section
      /// GEO L1 carrier frequency in Hz
   const double L1_FREQ_GEO   = L1_FREQ_GPS;
      /// GEO L5 carrier frequency in Hz.
   const double L5_FREQ_GEO   = L5_FREQ_GPS;
      /// GEO L1 carrier wavelength in meters
   const double L1_WAVELENGTH_GEO  = L1_WAVELENGTH_GPS;
      /// GEO L5 carrier wavelength in meters.
   const double L5_WAVELENGTH_GEO  = L5_WAVELENGTH_GPS;

   // ---------------- COMPASS ----------------------------------   // BWT add section
      /// COM L1 carrier frequency in Hz
   const double L1_FREQ_COM   = 1589.74e6;
      /// COM L2 carrier frequency in Hz.
   const double L2_FREQ_COM   = 1561.098e6;
   

   inline
   short getLegacyFitInterval(const short iodc, const short fiti)
      throw(gpstk::InvalidRequest )
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


   /// COM L6 carrier frequency in Hz.
   const double L6_FREQ_COM   = 1268.52e6;
      /// COM L7 carrier frequency in Hz.
   const double L7_FREQ_COM   = L7_FREQ_GAL;
      /// COM L1 carrier wavelength in meters
   const double L1_WAVELENGTH_COM  = 0.188579552631247;
      /// COM L2 carrier wavelength in meters.
   const double L2_WAVELENGTH_COM  = 0.192039486310276;
      /// COM L6 carrier wavelength in meters.
   const double L6_WAVELENGTH_COM  = 0.236332246460442;
      /// COM L7 carrier wavelength in meters.
   const double L7_WAVELENGTH_COM  = L7_WAVELENGTH_GAL;

      //@}
   
} // namespace

#endif //GPSTK_GNSS_CONSTANTS_HPP

