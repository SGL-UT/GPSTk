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

#ifndef MDPNAV_HPP
#define MDPNAV_HPP

#include <map>

#include "EngEphemeris.hpp"
#include "EngAlmanac.hpp"

#include "Histogram.hpp"

#include "MDPProcessors.hpp"


//-----------------------------------------------------------------------------
class MDPNavProcessor : public MDPProcessor
{
public:
   MDPNavProcessor(gpstk::MDPStream& in, std::ofstream& out);
   ~MDPNavProcessor();

   virtual void process(const gpstk::MDPNavSubframe& msg);
   virtual void process(const gpstk::MDPObsEpoch& msg);
   
   bool firstNav;

   static const int maxChannel=12;

   // First time is of the first missed epoch, second time is the last missed epoch
   // (i.e. first = previous + obsRateEst, second=current-ObsRateEst
   typedef std::pair<gpstk::DayTime, gpstk::DayTime> DayTimePair;
   typedef std::list<DayTimePair> DayTimePairList;

   // Used to control whether we process the engineering eph/alms.
   bool ephOut;
   bool almOut;
   bool minimalAlm;  // Set true to allow an alm to be built from a minimal set of pages

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

   NavMap prev, curr;

   std::map<NavIndex, gpstk::AlmanacPages> almPageStore;
   std::map<NavIndex, gpstk::EngAlmanac> almStore;

   std::map<NavIndex, gpstk::EphemerisPages> ephPageStore;
   std::map<NavIndex, gpstk::EngEphemeris> ephStore;

   std::list<gpstk::MDPNavSubframe> badList;
   unsigned long badNavSubframeCount, navSubframeCount;

   std::map<NavIndex, double> snr; // 'current' SNR
   std::map<NavIndex, double> el;  // 'current' elevation

   bool binByElevation;
   std::map<RangeCarrierPair, gpstk::Histogram> peHist;
   gpstk::Histogram::BinRangeList bins;
};
#endif
