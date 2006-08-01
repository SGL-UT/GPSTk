#pragma ident "$Id$"


#ifndef MDPNAV_HPP
#define MDPNAV_HPP

#include "EngEphemeris.hpp"
#include "EngAlmanac.hpp"

#include "MDPProcessors.hpp"

//-----------------------------------------------------------------------------
class MDPNavProcessor : public MDPProcessor
{
public:
   MDPNavProcessor(gpstk::MDPStream& in, std::ofstream& out);
   ~MDPNavProcessor();

   virtual void process(const gpstk::MDPNavSubframe& msg);
   
   bool firstNav;

   static const int maxChannel=12;

   // First time is of the first missed epoch, second time is the last missed epoch
   // (i.e. first = previous + obsRateEst, second=current-ObsRateEst
   typedef std::pair<gpstk::DayTime, gpstk::DayTime> DayTimePair;
   typedef std::list<DayTimePair> DayTimePairList;

   // Used to control whether we process the engineering eph/alms.
   bool ephOut;
   bool almOut;

   // This is really a triple: prn, RangeCode, CarrierCode
   typedef std::pair<gpstk::RangeCode, gpstk::CarrierCode> RangeCarrierPair;
   typedef std::pair<RangeCarrierPair, short> NavIndex;

   // This class can keep track of a subframe and where it came from
   typedef std::map<NavIndex, gpstk::MDPNavSubframe> NavMap;
   
   // A note on nomenclature. A navigation subframe is as defined in the
   // '200 and refers a set of 300 bits of the navigation that can be modulated
   // on the various codes. Think of it as the raw bits.  A navigation message
   // is a logical set of these subframes. For an ephemeris it will always consist
   // of three subframes, with subframe ids of 1, 2, and 3. For an almanac it will
   // consist of an undefined number of subframes, all with subframe ids of 4 and 5.

   NavMap prev, curr;

   std::map<NavIndex, gpstk::AlmanacPages> almPageStore;
   std::map<NavIndex, gpstk::EngAlmanac> almStore;

   std::map<NavIndex, gpstk::EphemerisPages> ephPageStore;
   std::map<NavIndex, gpstk::EngEphemeris> ephStore;

   std::list<gpstk::MDPNavSubframe> badList;
   unsigned long badNavSubframeCount, navSubframeCount;
};
#endif
