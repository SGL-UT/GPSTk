#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/ordUtils.hpp#1 $"

#include <iostream>
#include <string>

#include "util.hpp"

void compute_ords(ORDEpochMap& oem,
                  const RODEpochMap& rem,
                  const gpstk::RinexObsHeader& roh,
                  const gpstk::EphemerisStore& bce,
                  const gpstk::WxObsData& wod,
                  bool svTime,
                  const std::string& ordModeStr,
                  const std::string& clkModelStr);

void dump(std::ostream& s, const ORDEpochMap& oem);

void dumpStats(const ORDEpochMap& oem, 
               const std::string& ordMode,
               const double sigmam=5);

void computeStats(const std::string desc,
                  const ORDEpochMap& oem,
                  const ElevationRange er, 
                  const double sigmam);


