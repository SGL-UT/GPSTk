#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/MDPtools/ScreenProc.hpp#2 $"

#ifndef MDPSCREEN_HPP
#define MDPSCREEN_HPP

#include "MDPProcessors.hpp"

#include <signal.h>
#ifdef LINUX
#include <bits/signum.h>
#endif

// This prevents forte from using the macro implementation of many of the curses
// calls. They tend to interfere with parts of the STL.
#define NOMACROS
#include <curses.h>
#ifndef __linux__
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

   void redraw();

   void drawChan(int chan=0);
   void drawPVT();
   void drawBase();

   std::string host;
   float updateRate;
   double obsRate, pvtRate;
   WINDOW *win;
   int prev_curs;

   int verboseLevel;
   bool die;
   static bool gotWench;

   int elDir[13];
   gpstk::MDPObsEpoch currentObs[13];
   gpstk::MDPPVTSolution currentPvt;
   gpstk::DayTime lastUpdateTime;
};

#endif
