#pragma ident "$Id$"

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

#ifndef SVELEVATIONMAP_HPP
#define SVELEVATIONMAP_HPP

// gpstk crud
#include "DayTime.hpp"
#include "XvtStore.hpp"
#include "ObsEpochMap.hpp"

// The key for this map is the prn of the SV
//  PrnElevationMap pem;
//  pem[time][prn] = elevation;

typedef std::map<gpstk::SatID, double> SvDoubleMap;
typedef std::map<gpstk::DayTime, SvDoubleMap > SvElevationMap;

SvElevationMap elevation_map(const gpstk::ObsEpochMap& obs,
                             const gpstk::Triple& ap,
                             const gpstk::XvtStore<gpstk::SatID>& bce);
#endif
