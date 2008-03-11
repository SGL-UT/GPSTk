#pragma ident "$Id: //depot/msn/prototype/brent/coverage/StaStats.cpp#9 $"
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
 * @file StaStats.cpp
 */
#include <iostream>           // debug
#include "gps_constants.hpp"
#include "StaStats.hpp"

namespace gpstk
{
   using namespace std;
   using namespace gpstk;

   StaStats::StaStats( const string stationName, 
                       const int maxSVsAtOneTime,
                       const int minStaAtOneTime )
   {
      staName = stationName; 
      maxNumSimultaneousSVs = maxSVsAtOneTime;
      minNumSimultaneousSta = minStaAtOneTime;
      
      maxSVCount = 0;
      maxSVDuration = 0;
      maxSVOccurrances = 0;
      lastMaxSVEpoch = -1;
      
      minSVCount = gpstk::MAX_PRN+2;
      minSVDuration = 0;
      minSVOccurrances = 0;
      lastMinSVEpoch = -1;
      
      numEpochsGreaterThenMaxSVs = 0;
      numEpochsLessThanMinStas = 0;
      dataEntered = false;
      
      totalObsCount = 0;
      for (int i=0;i<18;++i) obsCountByBin[i] = 0;  
   }
 
      // Had to add this in order to force the minimum station count
      // for the "stats across all SVs" object in compSatVis
   void StaStats::updateMinStations( const int minStaAtOneTime )
   {
      minNumSimultaneousSta = minStaAtOneTime;
   }
   
   void StaStats::addToElvBins( const double elevation )
   {
      totalObsCount++;
      double scaledElv = elevation;
      scaledElv /= 5.0;
      int ndx = (int) scaledElv;
      //cout << "elv, scaledElv, ndx = " << elevation << ", " << scaledElv << ", " << ndx << endl;
      if (ndx>17) ndx = 17;
      if (ndx<0) ndx = 0;
      obsCountByBin[ndx]++;
   }
   
   void StaStats::addEpochInfo( const int count, const int epochID )
   {
      dataEntered = true;
      stats.Add( (double) count);
      if (count>maxNumSimultaneousSVs) numEpochsGreaterThenMaxSVs++;
      if (count<minNumSimultaneousSta) numEpochsLessThanMinStas++;

      if (count<minSVCount)
      {
         minSVCount = count;
         minSVDuration = 0;
         minSVOccurrances = 0;
         lastMinSVEpoch = epochID;
      }
      if (count==minSVCount)
      {
         minSVDuration++;
         if (epochID==lastMinSVEpoch ||
             epochID!= (lastMinSVEpoch+1) ) minSVOccurrances++;
         lastMinSVEpoch = epochID;
      }
      
      if (count>maxSVCount)
      {
         maxSVCount = count;
         maxSVDuration = 0;
         maxSVOccurrances = 0;
         lastMaxSVEpoch = epochID;
      }
      if (count==maxSVCount)
      {
         maxSVDuration++;
         if (epochID==lastMaxSVEpoch ||
             epochID!=(lastMaxSVEpoch+1) ) maxSVOccurrances++;
         lastMaxSVEpoch = epochID;
      }
      //cout << "maxSVCount, minSVCount =  " << maxSVCount << ", " << minSVCount << endl;
   }
   
   std::string StaStats::getStr(int intervalSize) const
   {
      int maxMinutes = (maxSVDuration * intervalSize) / 60;
      int minMinutes = (minSVDuration * intervalSize) / 60;
      char line[100];
      sprintf( line, "  %5s    %5.2f |   %2d     %4d     %3d |   %2d     %4d     %3d |    %4d",
               staName.c_str(),stats.Average(),
               minSVCount,minMinutes,minSVOccurrances,
               maxSVCount,maxMinutes,maxSVOccurrances,
               numEpochsGreaterThenMaxSVs);
      std::string retStr(line);
      return(retStr);
   }
   
   std::string StaStats::getSatStr(int intervalSize) const
   {
      int maxMinutes = (maxSVDuration * intervalSize) / 60;
      int minMinutes = (minSVDuration * intervalSize) / 60;
      char line[100];
      sprintf( line, "%5s    %5.2f !   %2d     %4d     %3d !   %2d     %4d     %3d !  %4d",
               staName.c_str(),stats.Average(),
               minSVCount,minMinutes,minSVOccurrances,
               maxSVCount,maxMinutes,maxSVOccurrances,
               numEpochsLessThanMinStas);
      std::string retStr(line);
      return(retStr);
   }

   std::string StaStats::getSatAvgStr() const
   {
      char line[100];
      sprintf( line, "  Avg    %5.2f !                       !                       !  %4d\n",
               stats.Average(),
               numEpochsLessThanMinStas);
      std::string retStr(line);
      return(retStr);
   }

   std::string StaStats::getElvBinValues() const
   {
      char line[180];
      //cout << "StaNum, Total Obs = " << staNum << ", " << totalObsCount << endl;
      sprintf( line, " %5s  %6d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d",
         staName.c_str(),totalObsCount,
         obsCountByBin[ 0],obsCountByBin[ 1],obsCountByBin[ 2],obsCountByBin[ 3],
         obsCountByBin[ 4],obsCountByBin[ 5],obsCountByBin[ 6],obsCountByBin[ 7],
         obsCountByBin[ 8],obsCountByBin[ 9],obsCountByBin[10],obsCountByBin[11],
         obsCountByBin[12],obsCountByBin[13],obsCountByBin[14],obsCountByBin[15],
         obsCountByBin[16],obsCountByBin[17] );
      std::string retStr(line);
      return(retStr);
   }
   
}   // namespace
