#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/PhaseCleaner.hpp#1 $"

#ifndef PHASECLEANER_HPP
#define PHASECLEANER_HPP

#include <set>

#include "DDEpoch.hpp"
#include "PhaseResidual.hpp"

class PhaseCleaner
{
public:
   PhaseCleaner();
   void addData(const RODEpochMap& rx1, const RODEpochMap& rx2, const DDEpochMap& ddem);

   void debias(void);

   void getSlips(CycleSlipList& csl) const;

   void dump(std::ostream& s) const;

   void getPhaseDD(DDEpochMap& ddem) const;

   typedef std::set<RinexObsType> RinexObsTypeSet;
   RinexObsTypeSet phaseObsTypes;

   mutable ROTDM lamda;

   // And an set of arcs for each PRN
   typedef std::map<gpstk::RinexPrn, PhaseResidual::ArcList> PraPrn;

   // And a set of those for each obs type
   typedef std::map<gpstk::RinexObsHeader::RinexObsType, PraPrn> PraPrnOt;

   PraPrnOt pot;
};
#endif
