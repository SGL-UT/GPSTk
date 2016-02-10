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

/**
 * @file DenseBinnedStats.hpp
 * Generates binned stats in two dimenstions.
 * Class declarations.
 */

#ifndef DENSE_BINNED_STATS_HPP
#define DENSE_BINNED_STATS_HPP

#include <vector>

#include "Stats.hpp"
#include "Exception.hpp"

namespace gpstk
{
   /// @ingroup math 
   //@{

   /**
    * This class provides the ability to compute statistics for
    * data mapped to bins. The stats for a sample
    * can only map to one bin (the bins cannot overlap). The
    * mapping is two-dimensional (one-dimensional bins can be
    * made ignoring one dimension).
    */

   template <class T>
      class DenseBinnedStats
   {
      public:

         NEW_EXCEPTION_CLASS(DenseBinnedStatsException, gpstk::Exception);

         /**
          * Constructor.
          * @param boundaryList defines the lower and upper bounds of each bin
          */
         DenseBinnedStats(size_t iNumX, T iMinX, T iMaxX,
            size_t iNumY, T iMinY, T iMaxY)
            : rejectedCount(0), usedCount(0),
            minX(iMinX), maxX(iMaxX), minY(iMinY), maxY(iMaxY)
         {
            stats.resize(iNumX);
            for (size_t i=0; i<iNumX; i++)
               stats[i].resize(iNumY);
         }

         void addData(const std::valarray<T>& statData,
            const std::valarray<T>& binDataX,
            const std::valarray<T>& binDataY)
         {
            size_t s = statData.size();

            if ( (s!=binDataX.size()) || (s!=binDataY.size()) )
            {
               DenseBinnedStatsException e("Input arrays not the same length.");
               GPSTK_THROW(e);
            }

            T thisX, thisY;
            for (size_t i=0; i<s; i++)
            {
               thisX = binDataX[i];
               thisY = binDataY[i];

               if ( (thisX < minX) || (thisX > maxX ) ||
                  (thisY < minY) || (thisY > maxY) )
                  rejectedCount++;
               else
               {
                  size_t ibin = static_cast<size_t>(std::floor((thisX - minX)*stats.size()/(maxX-minX)));
                  size_t jbin = static_cast<size_t>(std::floor((thisY-minY)*(stats[ibin].size()/(maxY-minY))));
                  stats[ibin][jbin].Add(statData[i]);
                  // find right bin and Add()
                  usedCount++;
               }
            }

         };

         void dumpMeans(std::ostream& ostr)
         {
            for (size_t i=0; i< stats.size(); i++)
            {
               for(size_t j=0; j< stats[i].size(); j++)
                  ostr << stats[i][j].Average() << " ";
               ostr << std::endl;
            }
         };

         void dumpCounts(std::ostream& ostr)
         {
            for (size_t i=0; i< stats.size(); i++)
            {
               for(size_t j=0; j< stats[i].size(); j++)
                  ostr << stats[i][j].N() << " ";
               ostr << std::endl;
            }
         };

         void dumpStdDev(std::ostream& ostr)
         {
            for (size_t i=0; i< stats.size(); i++)
            {
               for(size_t j=0; j< stats[i].size(); j++)
                  ostr << stats[i][j].StdDev() << " ";
               ostr << std::endl;
            }
         };

         int rejectedCount, usedCount;

         /// Two dimensional vector of bins
         std::vector< std::vector< Stats< T > > > stats;

         /// Range values for each dimension.
         T minX, maxX, minY, maxY;

   };                            // End class DenseBinnedStats

   //@}
}                                // End namespace gpstk
#endif                           // DENSE_BINNED_STATS_HPP
