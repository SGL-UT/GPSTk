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

#ifndef RESZILLA_UTIL_HPP
#define RESZILLA_UTIL_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <set>

// gpstk crud
#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "stl_helpers.hpp"
#include "EphemerisStore.hpp"
#include "icd_200_constants.hpp"
#include "GPSGeoid.hpp"
#include "TropModel.hpp"
#include "Geodetic.hpp"
#include "ObsEpochMap.hpp"

#include "Stats.hpp"

#include "RinexObsData.hpp"

#include "ORDEpoch.hpp"
#include "EpochClockModel.hpp"

#include "ElevationRange.hpp"

extern int verbosity;
extern std::string timeFormat;

extern ElevationRangeList elr;

// Just some helpers to ease the transition from rinex 2
extern const gpstk::ObsID C1;
extern const gpstk::ObsID P1;
extern const gpstk::ObsID L1;
extern const gpstk::ObsID D1;
extern const gpstk::ObsID S1;
extern const gpstk::ObsID C2;
extern const gpstk::ObsID P2;
extern const gpstk::ObsID L2;
extern const gpstk::ObsID D2;
extern const gpstk::ObsID S2;


typedef std::map<gpstk::DayTime, double> TimeDoubleMap;

// The key for this map is the prn of the SV
//  PrnElevationMap pem;
//  pem[time][prn] = elevation;

typedef std::map<gpstk::SatID, double> SvDoubleMap;
typedef std::map<gpstk::DayTime, SvDoubleMap > SvElevationMap;

SvElevationMap elevation_map(const gpstk::ORDEpochMap& oem);
SvElevationMap elevation_map(const gpstk::ObsEpochMap& obs,
                             const gpstk::Triple& ap,
                             const gpstk::EphemerisStore& bce);


 // Obs ID Double Map => ROTM
typedef std::map<gpstk::ObsID, double> OIDM;

typedef std::map< gpstk::SatID, OIDM > SvOIDM;
typedef std::map< gpstk::SatID, short > SvShortMap;

void add_clock_to_obs(gpstk::ObsEpochMap& rem, const gpstk::ORDEpochMap& oem);

void check_data(const gpstk::Triple& ap, const gpstk::ObsEpochMap& obs);

struct CycleSlipRecord
{
   gpstk::DayTime t;
   double cycles;
   gpstk::SatID prn, masterPrn;
   gpstk::ObsID oid;
   double elevation;
   long preCount;  // for How many epochs had the bias been stable
   double preGap;  // time between the end of the previous arc and this point
   long postCount; // how long the arc after the slip is

   bool operator<(const CycleSlipRecord& right)
   { return t < right.t; }
};

typedef std::list<CycleSlipRecord> CycleSlipList;

void dump(std::ostream& s, const CycleSlipList& sl);

std::string computeStats(
   const CycleSlipList& csl,
   const ElevationRange& er, 
   const gpstk::ObsID& oid);


// these still need to be re-factored.
struct DD2Epoch
{
   typedef std::map<gpstk::SatID, double> DD2ResidualMap;
   DD2ResidualMap res;

   // Returns true if successfull
   bool compute(gpstk::ObsEpoch rx1,
                gpstk::ObsEpoch rx2);
};

typedef std::map<gpstk::DayTime, DD2Epoch> DD2EpochMap;

void dump(std::ostream& s,
          DD2EpochMap& ddem, 
          SvElevationMap& sem);

void dumpStats(DD2EpochMap& ddem, 
               SvElevationMap& sem);

void computeStats(DD2EpochMap& ddem, 
                  const ElevationRange er, 
                  SvElevationMap& sem);

void computeDD2(const gpstk::ObsEpochMap& rx1,
                const gpstk::ObsEpochMap& rx2,
                DD2EpochMap& ddem);

#endif
