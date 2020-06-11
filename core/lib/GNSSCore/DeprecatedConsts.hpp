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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file DeprecatedConsts.hpp
 * Constants that have been removed from GNSSconstants.hpp for
 * deprecation in favor of those in the generated file FreqConsts.hpp
 */

#ifndef GPSTK_DEPRECATEDCONSTS_HPP
#define GPSTK_DEPRECATEDCONSTS_HPP

namespace gpstk
{
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
      // GLO Frequency(Hz)
      //   f1 is 1602.0e6 + n*562.5e3 Hz = 9 * (178 + n*0.0625) MHz
      //   f2    1246.0e6 + n*437.5e3 Hz = 7 * (178 + n*0.0625) MHz
      // where n is the time- and satellite-dependent 'frequency channel'
      // -7 <= n <= 7
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
      /// GLO L3 carrier frequency in Hz.
   const double L3_FREQ_GLO       = 1202.025e6;
      /// GLO L3 carrier wavelength in meters.
   const double L3_WAVELENGTH_GLO = 0.249406175412;
      // ---------------- Galileo ----------------------------------
      /// GAL L1 (E1) carrier frequency in Hz
   const double L1_FREQ_GAL   = L1_FREQ_GPS;
      /// GAL L5 (E5a) carrier frequency in Hz.
   const double L5_FREQ_GAL   = L5_FREQ_GPS;
      /// GAL L6 (E6) carrier frequency in Hz.
   const double L6_FREQ_GAL   = 1278.75e6;
      /// GAL L7 (E5b) carrier frequency in Hz.
   const double L7_FREQ_GAL   = 1207.140e6;
      /// GAL L8 (E5a+E5b) carrier frequency in Hz.
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

      // ---------------- Geostationary (SBAS) ---------------------
      /// GEO L1 carrier frequency in Hz
   const double L1_FREQ_GEO   = L1_FREQ_GPS;
      /// GEO L5 carrier frequency in Hz.
   const double L5_FREQ_GEO   = L5_FREQ_GPS;

      /// GEO L1 carrier wavelength in meters
   const double L1_WAVELENGTH_GEO  = L1_WAVELENGTH_GPS;
      /// GEO L5 carrier wavelength in meters.
   const double L5_WAVELENGTH_GEO  = L5_WAVELENGTH_GPS;

      // ---------------- BeiDou ----------------------------------
      /// BDS L1 (B1) carrier frequency in Hz.
   const double L1_FREQ_BDS   = 1561.098e6;
      /// BDS L2 (B2) carrier frequency in Hz.
   const double L2_FREQ_BDS   = L7_FREQ_GAL;
      /// BDS L3 (B3) carrier frequency in Hz.
   const double L3_FREQ_BDS   = 1268.52e6;

      /// BDS L1 carrier wavelength in meters.
   const double L1_WAVELENGTH_BDS  = 0.192039486310276;
      /// BDS L2 carrier wavelength in meters.
   const double L2_WAVELENGTH_BDS  = L7_WAVELENGTH_GAL;
      /// BDS L3 carrier wavelength in meters.
   const double L3_WAVELENGTH_BDS  = 0.236332464604421;

      // ---------------- QZSS ----------------------------------
      /// QZS L1 carrier frequency in Hz.
   const double L1_FREQ_QZS   = L1_FREQ_GPS;
      /// QZS L2 carrier frequency in Hz.
   const double L2_FREQ_QZS   = L2_FREQ_GPS;
      /// QZS L5 carrier frequency in Hz.
   const double L5_FREQ_QZS   = L5_FREQ_GPS;
      /// QZS LEX(6) carrier frequency in Hz.
   const double L6_FREQ_QZS   = L6_FREQ_GAL;

      /// QZS L1 carrier wavelength in meters.
   const double L1_WAVELENGTH_QZS   = L1_WAVELENGTH_GPS;
      /// QZS L2 carrier wavelength in meters.
   const double L2_WAVELENGTH_QZS   = L2_WAVELENGTH_GPS;
      /// QZS L5 carrier wavelength in meters.
   const double L5_WAVELENGTH_QZS   = L5_WAVELENGTH_GPS;
      /// QZS LEX(6) carrier wavelength in meters.
   const double L6_WAVELENGTH_QZS   = L6_WAVELENGTH_GAL;
}

#endif // GPSTK_DEPRECATEDCONSTS_HPP
