#pragma ident "$Id$"


#include <iostream>
#include <string>

#include "util.hpp"
#include "RobustLinearEstimator.hpp"

void computeOrds(
   ORDEpochMap& oem,
   const RODEpochMap& rem,
   const gpstk::RinexObsHeader& roh,
   const gpstk::EphemerisStore& bce,
   const gpstk::WxObsData& wod,
   bool svTime, bool keepUnhealty,
   const std::string& ordModeStr);

void estimateClock(
   const ORDEpochMap& oem,
   RobustLinearEstimator& rle);

void dumpOrds(
   std::ostream& s, 
   const ORDEpochMap& oem);

void dumpClock(
   std::ostream& s,
   const ORDEpochMap& oem, 
   const RobustLinearEstimator& rle);

void dumpStats(
   const ORDEpochMap& oem, 
   const std::string& ordMode,
   const double sigmam=5,
   const bool keepUnhealthy=false);

void computeStats(
   const std::string desc,
   const ORDEpochMap& oem,
   const ElevationRange er, 
   const double sigmam=5,
   const bool keepUnhealthy=false);


