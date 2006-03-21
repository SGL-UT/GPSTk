#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/MDPtools/MDPProcessors.hpp#4 $"

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
//  Copyright 2004, The University of Texas at Austin
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

   bool processBad;
   bool pvtOut, obsOut, navOut, tstOut;

   std::ofstream& out;
   gpstk::MDPStream& in;

   gpstk::DayTime startTime, stopTime;
   double timeSpan;

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
#endif
