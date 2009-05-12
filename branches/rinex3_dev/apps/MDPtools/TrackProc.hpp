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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Copyright 2007, The University of Texas at Austin
//
//============================================================================

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
