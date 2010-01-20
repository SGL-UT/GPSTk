#pragma ident "$Id: icd_glo_constants.hpp 1709 2009-02-18 20:27:47Z btolman $"

/**
 * @file icd_glo_constants.hpp
 * Constants as defined in the GLONASS ICD v5.0
 */

#ifndef GPSTK_ICD_GLO_CONSTANTS_HPP
#define GPSTK_ICD_GLO_CONSTANTS_HPP

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

#include <map>

namespace gpstk
{
      /**
       * @addtogroup geodeticgroup
       * @name GLONASS Constants
       * Time constants are in the time library.
       */
      //@{

      /// Fundamental chip rate in Hz.
   const double CHIP_FREQ_GLO  = 5.11e6;
      /// Chip rate of the P & Y codes in Hz.
   const double PY_CHIP_FREQ_GLO = CHIP_FREQ_GLO;
      /// Chip rate of the C/A code in Hz.
   const double CA_CHIP_FREQ_GLO = CHIP_FREQ_GLO / 10.0;

      /// Fundamental oscillator freq in Hz.
   const double PSC_FREQ_GLO  = 5.00e6;
      /// Base freq w/o relativisitic effects in Hz.
   const double RSVCLK_GLO    = 4.99999999782e6;

      /// L1 carrier base frequency in Hz.
   const double L1_FREQ_GLO       = 1602.0e6;
      /// L1 carrier frequency step size in Hz.
   const double L1_FREQ_STEP_GLO  = 562.5e3;
      /// L1 carrier wavelength in meters.
   const double L1_WAVELENGTH_GLO = 0.187136365793;
      /// L2 carrier base frequency in Hz.
   const double L2_FREQ_GLO       = 1246.0e6;
      /// L2 carrier frequency step size in Hz.
   const double L2_FREQ_STEP_GLO  = 437.5e3;
      /// L2 carrier wavelength in meters.
   const double L2_WAVELENGTH_GLO = 0.240603898876;
      /// L1 multiplier.
   const double L1_MULT_GLO   = 320.4;
      /// L2 multiplier.
   const double L2_MULT_GLO   = 249.2;
      /// Gamma multiplier.
   const double GAMMA_GLO = 1.653061224490;

      /// Singleton map of < FreqNo, frequency > as < int, double >.
   class GloFreq
   {
   private:
      GloFreq() // Constructor is private, cannot be executed by user.
      {
         for (int n = -7; n <= 12; n++)
         {
            // Frequencies in MHz.
            L1map[n] = 1602.0 + n*562.5e-3;
            L2map[n] = 1246.0 + n*436.5e-3;
         }
      }
      static GloFreq* mInstance; // Object that is created only once.
   public:
      static GloFreq* instance() // Method to get object.
      {
         if (mInstance == NULL) mInstance = new GloFreq();
         return mInstance;
      }
      typedef std::map<int, double> FreqMap;
      FreqMap L1map, L2map;
   };

   GloFreq* GloFreq::mInstance = NULL;

      /// Constant for the max array index in SV accuracy table.
   const int SV_ACCURACY_GLO_INDEX_MAX = 15;
      /// Map from SV accuracy/URA flag to NOMINAL accuracy values in m.
      /// Further details in ICD-GLO-v5.0, Table 4.4 in Section 4.4.
   const double SV_ACCURACY_GLO_INDEX[] = { 1.0,  2.0,   2.5,   4.0,   5.0,
                                            7.0, 10.0,  12.0,  14.0,  16.0,
                                           32.0, 64.0, 128.0, 256.0, 512.0,
                                           9.999999999999e99               };

      //@}
   
} // namespace

#endif
