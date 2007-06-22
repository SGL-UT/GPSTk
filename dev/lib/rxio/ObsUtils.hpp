#pragma ident "$Id: FormatConversionFunctions.hpp 171 2006-10-02 02:24:10Z ocibu $"

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

#ifndef OBSUTILS_HPP
#define OBSUTILS_HPP

#include "RinexObsData.hpp"
#include "RinexNavData.hpp"
#include "RinexMetData.hpp"

#include "EngAlmanac.hpp"
#include "ObsEpochMap.hpp"
#include "WxObsMap.hpp"
#include "SMODFData.hpp"

#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"
#include "MDPPVTSolution.hpp"

#include "AshtechMBEN.hpp"
#include "AshtechPBEN.hpp"

namespace gpstk
{
   /// Conversion Function from MDP data
   SvObsEpoch makeSvObsEpoch(const MDPObsEpoch& mdp) throw();

   ObsEpoch makeObsEpoch(const MDPEpoch& mdp) throw();

   MDPObsEpoch makeMDPObsEpoch(const SvObsEpoch& soe) throw();

   /// Conversion functions from Rinex data
   SvObsEpoch makeSvObsEpoch(const RinexObsData::RinexObsTypeMap& rotm) throw();

   ObsEpoch makeObsEpoch(const RinexObsData& rod) throw();

   WxObservation makeWxObs(const RinexMetData& rmd) throw();

   /// Conversion functions from SMODFData objects
   /// Note that there is no makeObsEpoch since each SMODFData object just
   /// has a single observation to a single sv.
   WxObservation makeWxObs(const SMODFData& smod) throw();

   /// Conversion functions from AshtechData
   /// hint is used to provide the data that is not natively in the MBEN data
   MDPObsEpoch makeMDPObsEpoch(
      const AshtechMBEN& mben,
      const MDPObsEpoch& hint) throw();

   void addMDPObservation(
      MDPObsEpoch& moe,
      const AshtechMBEN::code_block& cb,
      CarrierCode cc,
      RangeCode rc,
      const MDPObsEpoch& moe_hint) throw();

   MDPPVTSolution makeMDPPVTSolution(
      const AshtechPBEN& pben,
      const unsigned week) throw();
}
#endif
