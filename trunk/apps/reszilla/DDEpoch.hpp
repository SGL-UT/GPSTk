#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/DDEpoch.hpp#1 $"

#ifndef DDEPOCH_HPP
#define DDEPOCH_HPP

#include "DayTime.hpp"
#include "stl_helpers.hpp"
#include "icd_200_constants.hpp"
#include "Stats.hpp"

#include "util.hpp"

struct DDEpoch
{
   DDEpoch() : valid(false){};
   PrnROTDM dd;
   double clockOffset;
   gpstk::RinexPrn masterPrn;
   bool valid;

   // Computes a single difference between two sets of obs
   ROTDM singleDifference(
      const gpstk::RinexObsData::RinexObsTypeMap& rx1obs,
      const gpstk::RinexObsData::RinexObsTypeMap& rx2obs);
   
   // Sets the valid flag true if successfull
   // also sets the masterPrn to the one actually used
   void doubleDifference(
      const gpstk::RinexObsData& rx1,
      const gpstk::RinexObsData& rx2);

   void selectMasterPrn(
      const gpstk::RinexObsData& rx1, 
      const gpstk::RinexObsData& rx2,
      PrnElevationMap& pem);
};

typedef std::map<gpstk::DayTime, DDEpoch> DDEpochMap;

void computeDDEpochMap(
   RODEpochMap& rx1,
   RODEpochMap& rx2,
   PrnElevationMap& pem,
   DDEpochMap& ddem);

void dump(std::ostream& s,
          DDEpochMap& ddem,
          PrnElevationMap& pem);

void dumpStats(
   DDEpochMap& oem,
   const CycleSlipList& csl,
   PrnElevationMap& pem);

std::string computeStats(
   const RinexObsType rot,
   DDEpochMap& oem,
   const ElevationRange er,
   PrnElevationMap& pem);

#endif
