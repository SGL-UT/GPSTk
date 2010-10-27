#pragma ident "$Id$"

#ifndef SUBFRAMENAV_HPP
#define SUBFRAMENAV_HPP

#include <map>

#include "Histogram.hpp"

#include "MDPProcessors.hpp"


//-----------------------------------------------------------------------------
class MDPSubframeProcessor : public MDPProcessor
{
public:
   MDPSubframeProcessor(gpstk::MDPStream& in, std::ofstream& out);
   ~MDPSubframeProcessor();

   virtual void process(const gpstk::MDPNavSubframe& msg);
   virtual void process(const gpstk::MDPObsEpoch& msg);
   
   // This is really a triple: RangeCode, CarrierCode, prn
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

   // This preserves a 12.5 minute buffer of subframes
   NavMap sfHistory[750];

   std::list<gpstk::MDPNavSubframe> badList;
   unsigned long badNavSubframeCount, navSubframeCount;

   std::map<NavIndex, double> snr; // 'current' SNR
   std::map<NavIndex, double> el;  // 'current' elevation

   bool binByElevation;
   std::map<RangeCarrierPair, gpstk::Histogram> peHist;
   gpstk::Histogram::BinRangeList bins;
};
#endif
