//============================================================================
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

/// @file SatPassIterator.hpp
/// Iterate over a vector of SatPass in time order.

#ifndef GPSTK_SATELLITE_PASS_ITERATOR_INCLUDE
#define GPSTK_SATELLITE_PASS_ITERATOR_INCLUDE

// -------------------------------------------------------------------------------
#include "SatPass.hpp"

namespace gpstk {
// -------------------------------------------------------------------------------
/// Iterate over a list (vector) of SatPass using this class. NB. this class ignores
/// passes that have Status less than zero, but does not change any Status.
class SatPassIterator {
public:
   /// Explicit (only) constructor. Check the list for consistency (else throw)
   /// and find common time step and obs types, as well as first and last times.
   /// Ignore passes with Status less than zero.
   /// After building a vector<SatPass> (in time order), declare an iterator
   /// using this constructor, then call member function next(RinexObsData&) to
   /// access the data in time order, until it returns non-zero.
   /// @param splist   Vector of (consistent) SatPass objects
   /// @param rev      If true, interate in reverse time order
   /// @param dbug     If true, print debug info in next()
   /// @return 1 for success, 0 at the end of the dataset.
   /// @throw if input list is empty, or
   ///        if elements in the list have differing data interval or obs types, or
   ///        if any observation type is not registered (cf. RinexUtilities.hpp)
   explicit SatPassIterator(std::vector<SatPass>& splist,
      bool rev=false, bool dbug=false) throw(Exception);

   /// Restart the iteration, i.e. return to the initial time
   void reset(bool rev=false, bool dbug=false) throw();

   /// Access (all of) the data for the next epoch. As long as this function
   /// returns non-zero, there is more data to be accessed.
   /// Ignore passes with Status less than zero.
   /// @param indexMap  map<unsigned int, unsigned int> defined so that all the
   ///                  data in the current iteration is found at
   ///                  SatPassList[i].data(j) where indexMap[i] = j.
   /// @return 1 for success, 0 at the end of the dataset.
   /// @throw if time tags are out of order.
   int next(std::map<unsigned int, unsigned int>& indexMap) throw(Exception);

   /// Access (all of) the data for the next epoch. As long as this function
   /// returns non-zero, there is more data to be accessed.
   /// Ignore passes with Status less than zero.
   /// NB. If SatPass obs types are not registered (cf. RinexUtilities.hpp), then
   /// data will NOT be added to RinexObsData.
   /// NB. This assumes that all the SatPasses have the same obs in the same order!!
   /// @param robs  RinexObsData in which data is returned.
   /// @return 1 for success, 0 at the end of the dataset.
   /// @throw if time tags are out of order
   int next(RinexObsData& robs) throw(Exception);

   /// Get the first (earliest) time found in the SatPass list.
   Epoch getFirstTime(void) throw() { return FirstTime; }

   /// Get the last (latest) time found in the SatPass list.
   Epoch getLastTime(void) throw() { return LastTime; }

   /// @return the earliest time of good data in this SatPass list
   Epoch getFirstGoodTime(void) const throw() {
      Epoch ttag = LastTime;
      for(int i=0; i<SPList.size(); i++)
         if(SPList[i].getFirstGoodTime() < ttag)
            ttag = SPList[i].getFirstGoodTime();
      return ttag;
   }

   /// @return the latest time of good data in this SatPass list
   Epoch getLastGoodTime(void) const throw() {
      Epoch ttag = FirstTime;
      for(int i=0; i<SPList.size(); i++)
         if(SPList[i].getLastGoodTime() > ttag)
            ttag = SPList[i].getLastGoodTime();
      return ttag;
   }

   /// Get the time interval, which is common to all the SatPass in the list.
   double getDT(void) throw() { return DT; }

   /// get a map of pairs of indexes for the current epoch. call this after calling
   /// next() to get pairs (i,j) where the data returned by next() is the same as
   /// SatPassList[i].data(j,<obstype>), for each i in the map, and j=map[i].
   std::map<unsigned int,unsigned int> getIndexes(void) throw()
      { return nextIndexMap; }

private:
   SatPassIterator(const SatPassIterator&);              // DO NOT implement
   SatPassIterator& operator=(const SatPassIterator&);   // DO NOT implement

   /// if true, print debug info in nex()
   bool debug;

   /// if true, iterate in reverse time order
   bool timeReverse;

   /// count of the current epoch, = 0,1,...
   int currentN;                    // current time = FirstTime + currentN*DT

   /// time step (seconds) found in all objects in the list (constructor throws
   /// if they are not identical).
   double DT;

   /// first (earliest) start time (getFirstGoodTime()) of the passes in the list, and
   /// last (latest) end time (getLastGoodTime()) of the passes in the list.
   Epoch FirstTime,LastTime;

   /// index of the current object in the list for this satellite
   std::map<RinexSatID,int> listIndex;

   /// index of the data vector (spdvector) of the current object in the list
   /// for this satellite
   std::map<RinexSatID,int> dataIndex;

   /// offset in count of the current object in the list for this satellite
   std::map<RinexSatID,int> countOffset;

   /// vector parallel to SPList used to mark current use by iteration;
   /// value is "-1,0,1" as SPList[i] is "yet to be used, in use, done".
   /// This has nothing to do with SatPass::Status()
   std::vector<int> indexStatus;

   /// reference to the vector of passes being processed
   std::vector<SatPass>& SPList;

   /// map of indexes i,j, created by next(), such that data returned by next() is
   /// found at SatPassList[i].spdvector[j] where map[i]=j.
   std::map<unsigned int,unsigned int> nextIndexMap;

}; // end class SatPassIterator

}  // end namespace

#endif // GPSTK_SATELLITE_PASS_ITERATOR_INCLUDE
