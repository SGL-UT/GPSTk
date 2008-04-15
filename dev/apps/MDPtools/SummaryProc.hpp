#pragma ident "$Id$"


#ifndef MDPSUMMARY_HPP
#define MDPSUMMARY_HPP

#include "MDPProcessors.hpp"

//-----------------------------------------------------------------------------
class MDPSummaryProcessor : public MDPProcessor
{
public:
   MDPSummaryProcessor(gpstk::MDPStream& in, std::ofstream& out);
   ~MDPSummaryProcessor();

   virtual void process(const gpstk::MDPObsEpoch& msg);
   virtual void process(const gpstk::MDPPVTSolution& msg);
   virtual void process(const gpstk::MDPNavSubframe& msg);
   virtual void process(const gpstk::MDPSelftestStatus& msg);
   
   unsigned long numEpochs;
   unsigned long numObsEpochMsg;

   std::string secondsAsHMS(double s) const;

   typedef std::pair<int, int> elevationPair;  // first is min, second is max
   typedef std::list<elevationPair> elevBinList;
   elevBinList elevBins;

   typedef std::pair<gpstk::RangeCode, gpstk::CarrierCode> rcpair;
   typedef std::set<rcpair> rc_set;
   typedef std::map<rc_set, unsigned long> ocm;
   typedef std::map<elevationPair, ocm> ebocm;
   ebocm whack;  // records how many of each type of obs we get

   typedef std::map<rcpair, unsigned long> rcpm;
   typedef std::map<elevationPair, rcpm> eprcpm;
   eprcpm lol;    // records how many lock count resets we get

   // How many epochs that have the incorrect number of SVs in them.
   unsigned long svCountErrorCount;

   // the time of the first epoch processed and the last epoch processed
   gpstk::DayTime firstObsTime, lastObsTime;
   gpstk::DayTime firstPvtTime, lastPvtTime;
   gpstk::DayTime firstNavTime, lastNavTime;

   bool firstObs, firstPvt, firstNav, firstSelftest;
   double obsRateEst, pvtRateEst;
   gpstk::DayTime prevEpochTime;

   static const int maxChannel=12;

   // First time is of the first missed epoch, second time is the last missed epoch
   // (i.e. first = previous + obsRateEst, second=current-ObsRateEst
   typedef std::pair<gpstk::DayTime, gpstk::DayTime> DayTimePair;
   typedef std::list<DayTimePair> DayTimePairList;
   DayTimePairList epochGapList;
   std::vector<DayTimePairList> chanGapList;

   // This is used to record the previous obs on each channel
   typedef std::vector<gpstk::MDPObsEpoch> ObsEpochVector;
   ObsEpochVector prevObs;

   gpstk::MDPPVTSolution prevPvt;
   gpstk::MDPSelftestStatus prevSelftestStatus;

   // A list of all messages that had an error decoding them...
   typedef std::list<gpstk::MDPHeader> MDPList;
   MDPList badMessages;

};
#endif
