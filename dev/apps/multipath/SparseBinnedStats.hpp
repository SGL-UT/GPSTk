#pragma ident "$Id:$"

/**
 * @file SparseBinnedStats.hpp
 * Generates statistics over a discrete set of conditions. 
 * Class declarations.
 */

#ifndef SPARSE_BINNED_STATS_HPP
#define SPARSE_BINNED_STATS_HPP

#include <map>

#include "Stats.hpp"

namespace gpstk
{
   /** @addtogroup math */
   //@{
 
      /**
       * This class provides the ability to compute statistics that
       * are sorted conditionally. The simplest application would be
       * to compute stats for only points that meet an editing criteria.
       * A more advanced application would be to compute stats binned
       * w.r.t. to a single variable. 
       */  

   template <class T>
   class SparseBinnedStats
   {
   public:

      NEW_EXCEPTION_CLASS(SparseBinnedStatsException, gpstk::Exception);

         /**
          * Constructor.
          * @param boundaryList defines the lower and upper bounds of each bin
          */
      SparseBinnedStats(const std::valarray<T> boundaryList)
            : binBoundaries(boundaryList), rejectedCount(0) 
         {}

      void add(const std::valarray<T>& statData, const std::valarray<T> binData)
         {};

      std::map<int, Stats< T > > bin;
      int rejectedCount;

   protected:

      int computeBin( const T& entry );      
      std::valarray<T> binBoundaries;

   }; // End class SparseBinnedStats
   
      //@}   
} // End namespace gpstk

#endif // SPARSE_BINNED_STATS_HPP 
