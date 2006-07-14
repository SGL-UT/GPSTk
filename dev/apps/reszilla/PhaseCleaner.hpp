#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/PhaseCleaner.hpp#4 $"

#ifndef PHASECLEANER_HPP
#define PHASECLEANER_HPP

#include <set>

#include "DDEpoch.hpp"
#include "PhaseResidual.hpp"

class PhaseCleaner
{
public:
   PhaseCleaner(long al, double at, double gt);

   void addData(const RODEpochMap& rx1, const RODEpochMap& rx2);

   void debias(PrnElevationMap& pem);

   void selectMasters(
      const RinexObsType& rot, 
      const gpstk::RinexPrn& prn,
      PrnElevationMap& pem);

   void doubleDifference(
      const RinexObsType& rot, 
      const gpstk::RinexPrn& prn,
      PrnElevationMap& pem);

   void getSlips(CycleSlipList& csl,
                 PrnElevationMap& pem) const;

   void getPhaseDD(DDEpochMap& ddem) const;

   void dump(std::ostream& s) const;

   typedef std::set<RinexObsType> RinexObsTypeSet;
   RinexObsTypeSet phaseObsTypes;

   mutable ROTDM lamda;

   // And an set of arcs for each PRN
   typedef std::map<gpstk::RinexPrn, PhaseResidual::ArcList> PraPrn;

   // And a set of those for each obs type
   typedef std::map<gpstk::RinexObsHeader::RinexObsType, PraPrn> PraPrnOt;

   // Rx1 - Rx2 clock, in meters.
   TimeDoubleMap clockOffset;
   
   // SV line-of-sight motion, in meters/second
   typedef std::map<gpstk::RinexPrn, TimeDoubleMap> PrnTimeDoubleMap;
   PrnTimeDoubleMap rangeRate;

   PraPrnOt pot;
   
   long minArcLen;
   double minArcTime, maxGapTime;

   typedef std::map<gpstk::DayTime, gpstk::RinexPrn> TimePrnMap;

   class goodMaster
   {
   public:
      goodMaster(double v, const gpstk::RinexPrn& p, const gpstk::DayTime& t, PrnTimeDoubleMap& rr)
         : minVal(v), prn(p), time(t), rangeRate(rr){};

      const double minVal; // Above this elevation
      const gpstk::RinexPrn& prn;  // Not this prn
      const gpstk::DayTime& time;  // time to evaluate range rate
      PrnTimeDoubleMap& rangeRate;

      double bestElev;
      gpstk::RinexPrn bestPrn;
      bool operator()(const PrnDoubleMap::value_type& pdm);
   };
};
#endif
