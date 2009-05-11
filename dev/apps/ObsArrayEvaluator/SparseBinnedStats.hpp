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
//============================================================================

/**
 * @file SparseBinnedStats.hpp
 * Generates statistics over a discrete set of conditions. 
 * Class declarations. 
 */

#ifndef SPARSE_BINNED_STATS_HPP
#define SPARSE_BINNED_STATS_HPP

#include <map>

#include "Stats.hpp"
#include "Exception.hpp"

namespace gpstk
{
   /** @addtogroup math */
   //@{
 
      /**
       * This class provides the ability to compute statistics for
       * data sorted into one or more bins.
       */  

   template <class T>
   class SparseBinnedStats
   {
   public:

      NEW_EXCEPTION_CLASS(SparseBinnedStatsException, gpstk::Exception);

      struct binLimits
      {
         double lowerBound;
         double upperBound;
         bool within(double var)
            {return ((var>=lowerBound)&&(var<upperBound)); }
      };

         /**
          * Constructor.
          * @param boundaryList defines the lower and upper bounds of each bin
          */
      SparseBinnedStats(void)
            : rejectedCount(0), usedCount(0)
         {}

      size_t addBin( double lower, double upper)
      {
         binLimits newBin;
         newBin.lowerBound = lower;
         newBin.upperBound = upper;
         size_t entryNo = bins.size();
         bins.push_back(newBin);
         stats.push_back(Stats<T>());
         return entryNo;
      } 
      

      void addData(const std::valarray<T>& statData, 
                   const std::valarray<T> binData)
      {
         size_t s = statData.size();
         
         if (s!=binData.size())
         {
            SparseBinnedStatsException e("Input arrays not the same length.");
            GPSTK_THROW(e);
         }

         bool thisRejected;
         
         for (size_t i=0; i<s; i++)
         {
            thisRejected=true;
            for (size_t j=0; j<bins.size(); j++)
            {
               if ( bins[j].within(binData[i]) )
               {
                  stats[j].Add(statData[i]);
                  thisRejected = false;
               }
               
            }
            if (thisRejected)
               rejectedCount++;
            else 
               usedCount++;
         }
         
         
      };

      int rejectedCount, usedCount;

      std::vector<binLimits> bins;
      std::vector< Stats< T > > stats;

   }; // End class SparseBinnedStats
   
      //@}   
} // End namespace gpstk

#endif // SPARSE_BINNED_STATS_HPP 
