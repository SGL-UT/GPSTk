#pragma ident "$Id:$"
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
/**
 * @file StaStats.hpp
 */

#ifndef GPSTK_STASTATS_HPP
#define GPSTK_STASTATS_HPP

#include "Stats.hpp"

namespace gpstk
{
   class StaStats
   {
   public:
         /// Default constructor
      StaStats( const std::string stationName, 
                const int maxSVsAtOneTime,
                const int minStaAtOneTime );
      
         /// Destructor
      virtual ~StaStats() {}

      std::string getStaName() const;
      bool atLeastOneEntry() const;
      std::string getStr(int intervalSize) const;
      std::string getSatStr(int intervalSize) const;
      std::string getSatAvgStr( ) const;
      void addEpochInfo( const int count, const int epochID );
      void addToElvBins( const double elevation );
      std::string getElvBinValues() const;
      void updateMinStations( const int minStaAtOneTime );
      
   protected:
      gpstk::Stats<double> stats;
      std::string staName;
      bool dataEntered;
      
      int maxSVCount;
      int maxSVDuration;
      int maxSVOccurrances;
      int lastMaxSVEpoch;
      
      int numEpochsGreaterThenMaxSVs;
      int numEpochsLessThanMinStas;
      
      int minSVCount;
      int minSVDuration;
      int minSVOccurrances;
      int lastMinSVEpoch;
      
      int maxNumSimultaneousSVs;
      int minNumSimultaneousSta;
      
      long totalObsCount;
      long obsCountByBin[18];   // [0-5), [5-10), [10-15),....[85-90]
   }; // class StaStats

   inline std::string StaStats::getStaName() const {return(staName);};
   inline bool StaStats::atLeastOneEntry() const {return(dataEntered);};
   
   
} // namespace

#endif
