#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S.
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software.
//
//Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

#ifndef SGLTK_MDPSCREEN_HPP
#define SGLTK_MDPSCREEN_HPP

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

namespace sgltk
{

class MDPScreenProcessor : public MDPProcessor
{
public:
   MDPScreenProcessor(sgltk::MDPStream& in, std::ofstream& out);
   ~MDPScreenProcessor();

   virtual void process( const sgltk::MDPObsEpoch&       obs );
   virtual void process( const sgltk::MDPPVTSolution&    pvt );
   virtual void process( const sgltk::MDPNavSubframe&    sf  );
   virtual void process( const sgltk::MDPSelftestStatus& sts );

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
   sgltk::MDPObsEpoch       currentObs[13];
   sgltk::MDPPVTSolution    currentPvt;
   sgltk::MDPSelftestStatus currentSts;
   gpstk::CommonTime        lastUpdateTime;

   // This is really a triple: prn, RangeCode, CarrierCode
   typedef std::pair<gpstk::RangeCode, gpstk::CarrierCode> RangeCarrierPair;
   typedef std::pair<RangeCarrierPair, short> NavIndex;

   // This class can keep track of a subframe and where it came from
   typedef std::map<NavIndex, sgltk::MDPNavSubframe> NavMap;

   NavMap prev, curr;

   typedef std::map<NavIndex, sgltk::EphemerisPages> EphPageStore;
   EphPageStore ephPageStore;

   typedef std::map<NavIndex, gpstk::EngEphemeris> EphStore;
   EphStore ephStore;

   std::map<NavIndex, unsigned long> parErrCnt;
};

} // namespace sgltk

#endif
