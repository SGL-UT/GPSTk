#pragma ident "$Id$"


#ifndef MDPSCREEN_HPP
#define MDPSCREEN_HPP

#include "EngEphemeris.hpp"

#include "MDPProcessors.hpp"

#include <signal.h>
#ifdef LINUX
#include <bits/signum.h>
#endif

// This prevents forte from using the macro implementation of many of
// the curses calls. They tend to interfere with parts of the STL.
#define NOMACROS
#include <curses.h>
#if !defined( __linux__) && !defined(NCURSES_ATTR_T)
typedef int attr_t;
#endif


class MDPScreenProcessor : public MDPProcessor
{
public:
   MDPScreenProcessor(gpstk::MDPStream& in, std::ofstream& out);
   ~MDPScreenProcessor();

   virtual void process(const gpstk::MDPObsEpoch& obs);
   virtual void process(const gpstk::MDPPVTSolution& pvt);
   virtual void process(const gpstk::MDPNavSubframe& sf);
   virtual void process(const gpstk::MDPSelftestStatus& sts);

   void redraw();

   void drawChan(int chan=0);
   void drawPVT();
   void drawSTS();
   void drawBase();

   std::string host;
   float updateRate;
   double obsRate, pvtRate;
   WINDOW *win;
   int prev_curs;

   static bool gotWench;

   int elDir[13];
   gpstk::MDPObsEpoch currentObs[13];
   gpstk::MDPPVTSolution currentPvt;
   gpstk::MDPSelftestStatus currentSts;
   gpstk::DayTime lastUpdateTime;

   // This is really a triple: prn, RangeCode, CarrierCode
   typedef std::pair<gpstk::RangeCode, gpstk::CarrierCode> RangeCarrierPair;
   typedef std::pair<RangeCarrierPair, short> NavIndex;

   // This class can keep track of a subframe and where it came from
   typedef std::map<NavIndex, gpstk::MDPNavSubframe> NavMap;
   
   NavMap prev, curr;

   typedef std::map<NavIndex, gpstk::EphemerisPages> EphPageStore;
   EphPageStore ephPageStore;

   typedef std::map<NavIndex, gpstk::EngEphemeris> EphStore;
   EphStore ephStore;

   std::map<NavIndex, unsigned long> parErrCnt;
};

#endif
