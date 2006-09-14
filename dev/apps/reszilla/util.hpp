#pragma ident "$Id$"


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

#include "Stats.hpp"

#include "RinexObsData.hpp"

#include "ORDEpoch.hpp"
#include "EpochClockModel.hpp"

#include "ElevationRange.hpp"

extern int verbosity;
extern std::string timeFormat;

typedef gpstk::RinexObsHeader::RinexObsType  RinexObsType;
typedef gpstk::RinexObsData::RinexSatMap     RinexPrnMap;
typedef gpstk::RinexObsData::RinexObsTypeMap RinexObsTypeMap;

extern const RinexObsType& C1;
extern const RinexObsType& C2;
extern const RinexObsType& D1;
extern const RinexObsType& D2;
extern const RinexObsType& L1;
extern const RinexObsType& L2;
extern const RinexObsType& P1;
extern const RinexObsType& P2;
extern const RinexObsType& S1;
extern const RinexObsType& S2;

extern ElevationRangeList elr;

// a store of epochs for a single receiver/antenna
typedef std::map<gpstk::DayTime, gpstk::RinexObsData> RODEpochMap;

// this is a store of ORDs over time
typedef std::map<gpstk::DayTime, gpstk::ORDEpoch> ORDEpochMap;


typedef std::map<gpstk::DayTime, double> TimeDoubleMap;

// The key for this map is the prn of the SV
//  PrnElevationMap pem;
//  pem[time][prn] = elevation;

typedef std::map<gpstk::SatID, double> PrnDoubleMap;
typedef std::map<gpstk::DayTime, PrnDoubleMap > PrnElevationMap;
PrnElevationMap elevation_map(const ORDEpochMap& oem);
PrnElevationMap elevation_map(const RODEpochMap& rem,
                  const gpstk::RinexObsHeader& roh,
                  const gpstk::EphemerisStore& bce);


 // Rinex Obs Type Double Map => ROTM
typedef std::map<RinexObsType, double> ROTDM;

typedef std::map< gpstk::SatID, ROTDM> PrnROTDM;
typedef std::map< gpstk::SatID, short> PrnShortMap;

void add_clock_to_rinex(RODEpochMap& rem, const ORDEpochMap& oem);

void check_data(const gpstk::RinexObsHeader& roh, const RODEpochMap& rem);

struct CycleSlipRecord
{
   gpstk::DayTime t;
   double cycles;
   gpstk::SatID prn, masterPrn;
   RinexObsType rot;
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
   const RinexObsType& rot);


// these still need to be re-factored.
struct DD2Epoch
{
   typedef std::map<gpstk::SatID, double> DD2ResidualMap;
   DD2ResidualMap res;

   // Returns true if successfull
   bool compute(gpstk::RinexObsData rx1,
                gpstk::RinexObsData rx2);
};

typedef std::map<gpstk::DayTime, DD2Epoch> DD2EpochMap;

void dump(std::ostream& s,
          DD2EpochMap& ddem, 
          PrnElevationMap& pem);

void dumpStats(DD2EpochMap& ddem, 
               PrnElevationMap& pem);

void computeStats(DD2EpochMap& ddem, 
                  const ElevationRange er, 
                  PrnElevationMap& pem);

void computeDD2(const RODEpochMap& rx1,
               const RODEpochMap& rx2,
               DD2EpochMap& ddem);


#endif
