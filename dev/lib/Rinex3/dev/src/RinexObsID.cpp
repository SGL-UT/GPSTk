#pragma ident "$Id: RinexObsID.cpp 1161 2008-03-27 17:16:22Z ckiesch $"
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
 * @file RinexObsID.cpp
 * gpstk::RinexObsID - Identifies types of observations
 */

#include "RinexObsID.hpp"

namespace gpstk
{
   RinexObsID::RinexObsID(const RinexObsHeader::RinexObsType& rot)
      : ObsID()
   {
      // Note that the choice of tracking code for L1, L2, S1, S2 are arbitrary 
      // since they are ambiguous in the rinex 2 specifications
      if      (rot == RinexObsHeader::L1) {type=otPhase;   band=cbL1; code=tcP;}
      else if (rot == RinexObsHeader::P1) {type=otRange;   band=cbL1; code=tcP;}
      else if (rot == RinexObsHeader::C1) {type=otRange;   band=cbL1; code=tcCA;}
      else if (rot == RinexObsHeader::S1) {type=otSNR;     band=cbL1; code=tcP;}
      else if (rot == RinexObsHeader::D1) {type=otDoppler; band=cbL1; code=tcP;}

      else if (rot == RinexObsHeader::L2) {type=otPhase;   band=cbL2; code=tcP;}
      else if (rot == RinexObsHeader::P2) {type=otRange;   band=cbL2; code=tcP;}
      else if (rot == RinexObsHeader::C2) {type=otRange;   band=cbL2; code=tcC2LM;}
      else if (rot == RinexObsHeader::S2) {type=otSNR;     band=cbL2; code=tcP;}
      else if (rot == RinexObsHeader::D2) {type=otDoppler; band=cbL2; code=tcP;}
   }
}
