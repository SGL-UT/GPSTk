//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file StatsFilterHit.hpp
/// A class used by stats filters (FirstDiffFilter, WindowFilter and FDiffFilter)
/// to hold results of the filters. StatsFilterHit is a simple class that contains
/// an index into the data array, the type of event (beginning-of-data, outlier,
/// slip, etc), the number of points in data arrays (just a delta index) and
/// the number of good points following the event. It has a
/// member function asString() that is used in the filter dump() routines.

#ifndef STATISTICAL_FILTER_HIT_INCLUDE
#define STATISTICAL_FILTER_HIT_INCLUDE

//------------------------------------------------------------------------------------
// TD NB pffrac is never used.
// NB WindowFilter does not find outliers
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/// The 'results' object used by the filters to indicate presence of 'events' :
/// outlier(s), a slip or the beginning-of-data (BOD).
/// Each filter's analyze() function returns a std::vector of one or more these
/// objects; always with at least one element: the first is usually a BOD, but if
/// there are outliers at the start, it can be type outlier.
/// Calling the filter's getStats(FilterHit) will return statistics on the filter
/// quantities (not the data) in the segment that _begins_ at the event.
template <class T> class FilterHit
{
public:
   /// enum used to indicate the kind of event in this result
   typedef enum EventType {
      BOD = 0,          ///< beginning of data
      outlier,          ///< outlier(s) - npts is the number of outliers
      slip,             ///< slip (discontinuity)
      other             ///< never used?
   } event;

   /// empty and only constructor
   FilterHit() : index(-1), type(BOD), npts(0), ngood(0), score(0),
                 step(T(0)), sigma(T(0)), dx(T(0)),
                 haveStats(false) { }

   // member data
   event type;          ///< type of event: BOD, outlier(s), slip, other

   unsigned int index;  ///< index in the data array(s) at which this event occurs
   unsigned int npts;   ///< number of data points in this segment (= a delta index)
   unsigned int ngood;  ///< number of good (flag==0) points in this segment
   unsigned int score;  ///< weight of slip (=100 except >=lim for getMaybeSlips(lim))

   T step;              ///< for a slip, an estimate of the step in the data
   T sigma;             ///< for a slip, RSS future and past sigma on the data
   T dx;                ///< step in xdata: before SLIP or after OUT

   bool haveStats;      ///< set true when getStats() is called
   // see getStats() - meanings depend on filter
   T min,max,med,mad;   ///< robust stats on the filter quantities (not data)

   std::string msg;     ///< message from analysis

   /// convenience routines
   bool isBOD(void) { return (type == BOD); }
   bool isOutlier(void) { return (type == outlier); }
   bool isSlip(void) { return (type == slip); }

   /// return the analysis message
   std::string analMsg(void) const
      { return msg; }

   /// return as a single string with just type, index and npts
   std::string asString(const int osp=3) const
   {
      std::stringstream oss;
      switch(type) {
         case BOD: oss << "BOD"; break;
         case outlier: oss << "OUT"; break;
         case slip: oss << "SLIP"; break;
         default: case other: oss << "other"; break;
      }
      oss << std::fixed << std::setprecision(osp);

      // ind npts step dx sig score - be sure this order matches asStringRead()
      oss << " " << index << " " << npts << " " << dx;
      if(type == slip) oss << " " << step << " " << sigma << " " << score;
      else             oss << " ?" << " ?" << " ?";

      return oss.str();
   }

   /// return as a single human-readable string giving all the relevant info
   std::string asStringRead(const int osp=3) const
   {
      std::stringstream oss;
      switch(type) {
         case BOD: oss << "BOD"; break;
         case outlier: oss << "OUT"; break;
         case slip: oss << "SLIP"; break;
         default: case other: oss << "other"; break;
      }
      oss << " ind=" << index << " npts=" << npts
            << std::fixed << std::setprecision(osp)
            << " x_gap=" << dx;
      if(type == slip) oss << " step=" << step << " sig=" << sigma
                           << " score=" << score << (score<100 ? " SMALL":"");

      return oss.str();
   }

   /// return as a longer single string containing asString() plus stats
   /// @param osp precision for fixed format
   std::string asStatsString(const int osp=3) const
   {
      std::stringstream oss;
      oss << asString(osp) << std::fixed << std::setprecision(osp);
      if(!haveStats)
         oss << "; NoSt";
      else
         oss << " min=" << min << " max=" << max << " med=" << med << " mad=" << mad;
      return oss.str();
   }

}; // end class FilterHit

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#endif   // #define STATISTICAL_FILTER_HIT_INCLUDE
