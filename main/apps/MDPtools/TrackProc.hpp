#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/MDPtools/TrackProc.hpp#2 $"

#ifndef MDPTRACK_HPP
#define MDPTRACK_HPP

#include "MDPProcessors.hpp"

//-----------------------------------------------------------------------------
class MDPTrackProcessor : public MDPProcessor
{
public:
   MDPTrackProcessor(gpstk::MDPStream& in, std::ofstream& out);
   ~MDPTrackProcessor();

   virtual void process(const gpstk::MDPObsEpoch& oe);
   
   // Used to determine how many of each type of obs we get
   typedef std::pair<gpstk::RangeCode, gpstk::CarrierCode> rcpair;
   typedef std::set<rcpair> rc_set;

   struct ChanRec
   {
      int prn;
      float elevation;
      rc_set obs;
      std::string codes;
   };

   // This is a list of what is being received for each channel
   typedef std::vector<ChanRec> ChanVector;
   ChanVector currCv, prevCv;
   gpstk::DayTime currTime, prevTime;

   void printChanges();
};
#endif
