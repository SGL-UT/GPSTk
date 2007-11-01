#pragma ident "$Id: $"

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

/**
 * @file constants.hpp
 * Include file for miscellaneous constants for program DDBase.
 */

#ifndef DDBASE_CONSTANTS_INCLUDE
#define DDBASE_CONSTANTS_INCLUDE

//------------------------------------------------------------------------------------
// GPSTk
#include "icd_200_constants.hpp"

//------------------------------------------------------------------------------------
const double CFF=gpstk::C_GPS_M/gpstk::OSC_FREQ;
const double F1=gpstk::L1_MULT;   // 154.0;
const double F2=gpstk::L2_MULT;   // 120.0;
const double f12=F1*F1;
const double f22=F2*F2;
/// wavelength L1
const double wl1=CFF/F1;
/// wavelength L2
const double wl2=CFF/F2;
/// Multiplier for wide lane range(m) : WLR = wl1r * P1 + wl2r * P2
const double wl1r=F1/(F1+F2);
const double wl2r=F2/(F1+F2);
/// Multiplier for wide lane phase(m) : WLP = wl1p * L1 + wl2p * L2 (L1,L2 in meters)
const double wl1p=F1/(F1-F2);
const double wl2p=-F2/(F1-F2);
/// Multiplier for iono-free range(m) : IFR = if1r * P1 + if2r * P2
const double if1r=f12/(f12-f22);
const double if2r=-f22/(f12-f22);
/// Multiplier for iono-free phase(m) : IFP = if1p * L1 + if2p * L2 (L1,L2 in meters)
const double if1p=f12/(f12-f22);
const double if2p=-f22/(f12-f22);
/// Multiplier for geometry-free range(m) : GFR = gf1p * P1 + ff2p * P2
const double gf1r=-1;
const double gf2r=1;
/// Multiplier for geo-free phase(m) : GFP = gf1p * L1 + gf2p * L2 (L1,L2 in meters)
const double gf1p=wl1;
const double gf2p=-wl2;

const double beta=f12/f22;
const double alpha=f12/f22 - 1.0;
/// Frequency at L1 (Hz)
const double FL1=F1*10.23e6;                          // Hz
/// Conversion factor: TEC Units per meter
const double TECUperM=FL1*FL1*1.e-16/(alpha*40.28);

#endif
//------------------------------------------------------------------------------------
