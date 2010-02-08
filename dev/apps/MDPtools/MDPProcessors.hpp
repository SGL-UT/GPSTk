#pragma ident "$Id$"

/** @file Various presentations/analysis on MDP streams */

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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Copyright 2007, The University of Texas at Austin
//
//============================================================================

#ifndef MDPPROCESSORS_HPP
#define MDPPROCESSORS_HPP

#include <vector>
#include <map>
#include <set>

#include <StringUtils.hpp>
#include <Stats.hpp>
#include <DayTime.hpp>

#include "MDPStream.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"
#include "MDPPVTSolution.hpp"
#include "MDPSelftestStatus.hpp"

//-----------------------------------------------------------------------------
class MDPProcessor
{
public:

   MDPProcessor();
   MDPProcessor(gpstk::MDPStream& in, std::ofstream& out);
   virtual ~MDPProcessor() {};

   void process();

   std::string timeFormat;
   int debugLevel;
   int verboseLevel;
   bool die;
   bool followEOF;

   bool processBad;
   bool pvtOut, obsOut, navOut, tstOut;

   std::ofstream& out;
   gpstk::MDPStream& in;

   gpstk::DayTime startTime, stopTime;
   double timeSpan;

   unsigned long msgCount, fcErrorCount;
   unsigned short firstFC, lastFC;

   // A bitmask of bugs to not report
   // bit 0: SV count mismatch
   // 
   unsigned long bugMask;

protected:
   void processFC(const gpstk::MDPHeader& hedaer);

private:
   /// All processors are defined so that subclasses don't have to
   /// define process methods for messages they don't care about.
   virtual void process(const gpstk::MDPObsEpoch& oe)        {};
   virtual void process(const gpstk::MDPPVTSolution& pvt)    {};
   virtual void process(const gpstk::MDPNavSubframe& sf)     {};
   virtual void process(const gpstk::MDPSelftestStatus& sts) {};
};


//-----------------------------------------------------------------------------
class MDPTableProcessor : public MDPProcessor
{
   void process(const gpstk::MDPObsEpoch& oe);
   void process(const gpstk::MDPPVTSolution& pvt);
   void process(const gpstk::MDPNavSubframe& sts);
   void process(const gpstk::MDPSelftestStatus& sts);

   bool headerDone;
   void outputHeader();

public:
   MDPTableProcessor(gpstk::MDPStream& in, std::ofstream& out);
};


//-----------------------------------------------------------------------------
class MDPBriefProcessor : public MDPProcessor
{
   void process(const gpstk::MDPObsEpoch& oe)
   {out << "o " << std::flush; };

   void process(const gpstk::MDPPVTSolution& pvt)
   {out << "p " << std::flush; };

   void process(const gpstk::MDPNavSubframe& sf)
   {out << "n " << std::flush; };

   void process(const gpstk::MDPSelftestStatus& sts)
   {out << "s " << std::flush; };

public:
   MDPBriefProcessor(gpstk::MDPStream& in, std::ofstream& out) :
      MDPProcessor(in, out)
   {};
};


//-----------------------------------------------------------------------------
class MDPVerboseProcessor : public MDPProcessor
{
   void process(const gpstk::MDPObsEpoch& oe);
   void process(const gpstk::MDPPVTSolution& pvt);
   void process(const gpstk::MDPNavSubframe& sf);
   void process(const gpstk::MDPSelftestStatus& sts);

public:
   MDPVerboseProcessor(gpstk::MDPStream& in, std::ofstream& out) :
      MDPProcessor(in, out)
   {}
};


//-----------------------------------------------------------------------------
class MDPNullProcessor : public MDPProcessor
{
public:
   MDPNullProcessor(gpstk::MDPStream& in, std::ofstream& out) :
      MDPProcessor(in, out)
   {}
};


//-----------------------------------------------------------------------------
class MDPCSVProcessor : public MDPProcessor
{
   void process(const gpstk::MDPObsEpoch& oe);
   void process(const gpstk::MDPPVTSolution& pvt);
   void process(const gpstk::MDPNavSubframe& sts);
   void process(const gpstk::MDPSelftestStatus& sts);

   bool headerDone;
   void outputHeader();
   char buff0[256],buff1[256];

public:
   MDPCSVProcessor(gpstk::MDPStream& in, std::ofstream& out);
};

#endif
