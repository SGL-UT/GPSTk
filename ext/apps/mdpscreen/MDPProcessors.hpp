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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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
#include <CommonTime.hpp>
#include <TimeString.hpp>

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
   MDPProcessor( sgltk::MDPStream& in, std::ofstream& out );
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
   sgltk::MDPStream& in;

   gpstk::CommonTime startTime, stopTime;
   double timeSpan;

   unsigned long msgCount, fcErrorCount;
   unsigned short firstFC, lastFC;

   // A bitmask of bugs to not report
   // bit 0: SV count mismatch
   // 
   unsigned long bugMask;

protected:
   void processFC( const sgltk::MDPHeader& header );

private:
   /// All processors are defined so that subclasses don't have to
   /// define process methods for messages they don't care about.
   virtual void process(const sgltk::MDPObsEpoch& oe)        {};
   virtual void process(const sgltk::MDPPVTSolution& pvt)    {};
   virtual void process(const sgltk::MDPNavSubframe& sf)     {};
   virtual void process(const sgltk::MDPSelftestStatus& sts) {};
};


//-----------------------------------------------------------------------------
class MDPTableProcessor : public MDPProcessor
{
   void process(const sgltk::MDPObsEpoch& oe);
   void process(const sgltk::MDPPVTSolution& pvt);
   void process(const sgltk::MDPNavSubframe& sts);
   void process(const sgltk::MDPSelftestStatus& sts);

   bool headerDone;
   void outputHeader();

public:
   MDPTableProcessor( sgltk::MDPStream& in, std::ofstream& out );
};


//-----------------------------------------------------------------------------
class MDPBriefProcessor : public MDPProcessor
{
   void process(const sgltk::MDPObsEpoch& oe)
   {out << "o " << std::flush; };

   void process(const sgltk::MDPPVTSolution& pvt)
   {out << "p " << std::flush; };

   void process(const sgltk::MDPNavSubframe& sf)
   {out << "n " << std::flush; };

   void process(const sgltk::MDPSelftestStatus& sts)
   {out << "s " << std::flush; };

public:
   MDPBriefProcessor(sgltk::MDPStream& in, std::ofstream& out) :
      MDPProcessor(in, out)
   {};
};

//-----------------------------------------------------------------------------
class MDPHeaderProcessor : public MDPProcessor
{
   void process(const sgltk::MDPObsEpoch& oe)
   {ohr(oe);};

   void process(const sgltk::MDPPVTSolution& pvt)
   {ohr(pvt);};

   void process(const sgltk::MDPNavSubframe& sf)
   {ohr(sf);};

   void process(const sgltk::MDPSelftestStatus& sts)
   {ohr(sts);};

   void ohr(const sgltk::MDPHeader& h)
   {
      out << printTime(h.time, timeFormat)
          << std::fixed
          << ", " << std::setw(3) << h.id
          << ", " << std::setw(10) << in.headerCount
          << std::endl;
   }

public:
   MDPHeaderProcessor(sgltk::MDPStream& in, std::ofstream& out) :
      MDPProcessor(in, out)
   {};
};


//-----------------------------------------------------------------------------
class MDPVerboseProcessor : public MDPProcessor
{
   void process(const sgltk::MDPObsEpoch& oe);
   void process(const sgltk::MDPPVTSolution& pvt);
   void process(const sgltk::MDPNavSubframe& sf);
   void process(const sgltk::MDPSelftestStatus& sts);

public:
   MDPVerboseProcessor(sgltk::MDPStream& in, std::ofstream& out) :
      MDPProcessor(in, out)
   {}
};


//-----------------------------------------------------------------------------
class MDPNullProcessor : public MDPProcessor
{
public:
   MDPNullProcessor(sgltk::MDPStream& in, std::ofstream& out) :
      MDPProcessor(in, out)
   {}
};


//-----------------------------------------------------------------------------
class MDPCSVProcessor : public MDPProcessor
{
   void process(const sgltk::MDPObsEpoch& oe);
   void process(const sgltk::MDPPVTSolution& pvt);
   void process(const sgltk::MDPNavSubframe& sts);
   void process(const sgltk::MDPSelftestStatus& sts);

   bool headerDone;
   void outputHeader();
   char buff0[256],buff1[256];

public:
   MDPCSVProcessor(sgltk::MDPStream& in, std::ofstream& out);
};

#endif
