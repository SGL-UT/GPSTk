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
 * @file GNSSconstants.hpp
 * Constants as defined in the GPS-ICD-200D and by all RINEX GNSSs
 */

#ifndef GPSTK_GNSS_CONSTANTS_HPP
#define GPSTK_GNSS_CONSTANTS_HPP

#include "Exception.hpp"
#include "SatID.hpp"
#include "DeprecatedConsts.hpp"

namespace gpstk
{
      /**
       * @ingroup Geodetic
       * @name GNSS Constants
       * Time constants are in TimeConstants.hpp
       */
      //@{

      // ---------------- independent of GNSS ----------------------
      /// GPS value of PI; also specified by GAL
   const double PI        = 3.141592653589793238462643383280;
      /// GPS value of PI*2
   const double TWO_PI    = 6.283185307179586476925286766559;
      /// GPS value of PI**0.5
   const double SQRT_PI   = 1.772453850905516027298167483341;
      /// relativity constant (sec/sqrt(m))
   const double REL_CONST = -4.442807633e-10;
      /// m/s, speed of light; this value defined by GPS but applies to GAL and GLO.
   const double C_MPS = 2.99792458e8;
      /// Conversion Factor from degrees to radians (units: degrees^-1)
   static const double DEG_TO_RAD = 1.7453292519943e-2;
      /// Conversion Factor from radians to degrees (units: degrees)
   static const double RAD_TO_DEG = 57.295779513082;

      // ---------------- GPS --------------------------------------
      /// Hz, GPS Oscillator or chip frequency
   const double OSC_FREQ_GPS  = 10.23e6;
      /// Hz, GPS chip rate of the P & Y codes
   const double PY_CHIP_FREQ_GPS = OSC_FREQ_GPS;
      /// Hz, GPS chip rate of the C/A code
   const double CA_CHIP_FREQ_GPS = OSC_FREQ_GPS / 10.0;
      /// Hz, GPS Base freq w/o relativisitic effects
   const double RSVCLK_GPS    = 10.22999999543e6;
      /// GPS L1 frequency in units of oscillator frequency
   const double L1_MULT_GPS   = 154.0;
      /// GPS L2 frequency in units of oscillator frequency
   const double L2_MULT_GPS   = 120.0;
      /// GPS L5 frequency in units of oscillator frequency.
   const double L5_MULT_GPS   = 115.0;
      /// GPS Gamma constant
   const double GAMMA_GPS = 1.646944444;
      /// Reference Semi-major axis. From IS-GPS-800 Table 3.5-2 in meters.
   const double A_REF_GPS = 26559710.0;
      /// Omega reference value from Table 30-I converted to radians
   const double OMEGADOT_REF_GPS = -2.6e-9 * PI;

   inline
   short getLegacyFitInterval(const short iodc, const short fiti)
   {
         /* check the IODC */
      if (iodc < 0 || iodc > 1023)
      {
            /* error in iodc, return minimum fit */
         return 4;
      }

      if ( ( ( (fiti == 0) && (iodc & 0xFF) < 240 )
             || (iodc & 0xFF) > 255 ) )
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
         else if( ( (iodc >= 248) && (iodc <= 255) ) || iodc == 496 )
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
         else if( iodc == 511 || ( (iodc >= 752) && (iodc <= 756) ) )
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

      // ---------------- GLONASS ----------------------------------
      /// GLO Fundamental chip rate in Hz.
   const double OSC_FREQ_GLO  = 5.11e6;
      /// GLO Chip rate of the P & Y codes in Hz.
   const double PY_CHIP_FREQ_GLO = OSC_FREQ_GLO;
      /// GLO Chip rate of the C/A code in Hz.
   const double CA_CHIP_FREQ_GLO = OSC_FREQ_GLO / 10.0;

      /// GLO Fundamental oscillator freq in Hz.
   const double PSC_FREQ_GLO  = 5.00e6;
      /// GLO Base freq w/o relativisitic effects in Hz.
   const double RSVCLK_GLO    = 4.99999999782e6;

      /// GLO L1 multiplier.
   const double L1_MULT_GLO   = 320.4;
      /// GLO L2 multiplier.
   const double L2_MULT_GLO   = 249.2;

      /// Constant for the max array index in SV accuracy table.
   const int SV_ACCURACY_GLO_INDEX_MAX = 15;
      /// Map from SV accuracy/URA flag to NOMINAL accuracy values in m.
      /// Further details in ICD-GLO-v5.0, Table 4.4 in Section 4.4.
   const double SV_ACCURACY_GLO_INDEX[] = { 1.0,  2.0,   2.5,   4.0,   5.0,
                                            7.0, 10.0,  12.0,  14.0,  16.0,
                                            32.0, 64.0, 128.0, 256.0, 512.0,
                                            9.999999999999e99               };

      // ---------------- BeiDou ----------------------------------
      /// The maximum number of active satellites in the Compass constellation.
   const long MAX_PRN_COM     = 30;

      // ---------------- QZSS ----------------------------------
      // PRN range defined in QZSS ICD 5.1.1.11.1
      // PRN 198-202 reserved for maint./test, not to be used 
      // by users. 
      // These need to be defined in order to differentiate
      // QZSS from GPS in cases where QZSS is broadcasting 
      // GPS-similar signals and navigation messages.
      // Defined as int to be compatible with gpstk::SatID.id member.
   const int MIN_PRN_QZS = 193;    
   const int MAX_PRN_QZS = 202; 

      //@}
   
} // namespace

// this is at the end because it needs some of the constants defined above
#include "FreqConsts.hpp"

#endif //GPSTK_GNSS_CONSTANTS_HPP
