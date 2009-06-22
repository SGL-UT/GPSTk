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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================
//
//
//#include <stdio.h>
#include <sstream>

// gpstk
#include "DiscreteVisibleCounts.hpp"
#include "StringUtils.hpp"

namespace gpstk
{

   using namespace std; 

   DiscreteVisibleCounts::DiscreteVisibleCounts( ) {}

   void DiscreteVisibleCounts::addCount(const int newCount )
   {
      countMap[newCount]++;
   }

   int DiscreteVisibleCounts::getMaxCount( ) const
   { 
      int max = 0;
      map<int,long>::const_iterator CI;
      for (CI=countMap.begin();CI!=countMap.end();++CI)
      {
         if (CI->first > max) max = CI->first;
      }
      return(max);
   }

   long DiscreteVisibleCounts::getSumOfAllCounts( ) const
   {
      long sum; 
      map<int,long>::const_iterator CI;
      for (CI=countMap.begin();CI!=countMap.end();++CI)
      { 
         sum += CI->second;
      }
      return(sum);
   }

   std::string DiscreteVisibleCounts::dumpCounts( const int max,
                                                  const int width) const
   {
      std::ostringstream ost;
 
      long count;
      map<int,long>::const_iterator CI;
      for (int i=0;i<=max;++i)
      {
         CI = countMap.find(i);
         if (CI!=countMap.end()) count = CI->second;
          else count = 0;
         ost.width(width);
         ost << count; 
      }
      return ost.str();
   }

   std::string DiscreteVisibleCounts::dumpCountsAsPercentages( const int max,
                                                       const int width ) const
   {
      std::ostringstream ost;
      
      double sum = (double) getSumOfAllCounts();
      
         // If there are no counts for this object, output a special string.
      if (sum==0)
      {
         for (int i=0;i<=max;++i)
         {
            ost.width(width);
            ost << "-.-";
         }
         return(ost.str());
      }
      
      double percent = 0.0;
      map<int,long>::const_iterator CI;
      for (int i=0;i<=max;++i)
      {
         CI = countMap.find(i);
         if (CI!=countMap.end())
            percent = 100.0 * (double) CI->second / sum;
          else
            percent = 0.0;
         ost.width(width);
         ost << StringUtils::asString(percent,1);
      }
      return ost.str();
   }
   
   std::string DiscreteVisibleCounts::dumpCumulativeCounts( const int max,
                                                  const int width ) const
   {
      std::ostringstream ost;
 
      long count;
      map<int,long>::const_iterator CI;
      
         // Handle special case of 0
      CI = countMap.find(0);
      if (CI!=countMap.end()) count = CI->second;
       else count = 0; 
      ost.width(width);
      ost << count;
      
         // Outer loop for output
      for (int i=1;i<=max;++i)
      {
            // Inner loop for summations
         long isum = 0;
         int start = i;
         for (int j=start;j<=max;++j)
         {
            CI = countMap.find(j);
            if (CI!=countMap.end()) isum += CI->second;
         }
         ost.width(width);
         ost << isum;
      }
      return ost.str();
   }
   
   std::string DiscreteVisibleCounts::dumpCumulativeCountsAsPercentages( const int max,
                                                  const int width ) const
   {
      std::ostringstream ost;
      
      double sum = (double) getSumOfAllCounts();
 
      map<int,long>::const_iterator CI;
      
         // If there are no counts for this object, output a special string.
      if (sum==0)
      {
         for (int i=0;i<=max;++i)
         {
            ost.width(width);
            ost << "-.-";
         }
         return(ost.str());
      }
      
         // Handle special case of 0
      long total = 0; 
      CI = countMap.find(0);
      if (CI!=countMap.end()) total = CI->second;
      double percent = 100.0 * (double) total / sum;
      ost.width(width);
      ost << StringUtils::asString(percent,1);
      
         // Outer loop for output
      for (int i=1;i<=max;++i)
      {
            // Inner loop for summations
         total = 0;
         int start = i;
         for (int j=start;j<=max;++j)
         {
            CI = countMap.find(j);
            if (CI!=countMap.end()) total += CI->second;
         }
         percent = 100.0 * (double) total / sum;
         ost.width(width);
         ost << StringUtils::asString(percent,1);
      }
      return ost.str();
   }
//
//
}     // end namespace gpstk
