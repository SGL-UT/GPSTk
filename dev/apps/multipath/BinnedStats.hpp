#pragma ident "$Id:$"

/**
 * @file BinnedStats.hpp
 * Generates statistics over a discrete set of conditions. 
 * Class declarations.
 */

#ifndef BINNED_STATS_HPP
#define BINNED_STATS_HPP

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
   class BinnedStats
   {
   public:

      NEW_EXCEPTION_CLASS(BinnedStatsException, gpstk::Exception);

         /**
          * Constructor.
          * @param boundaryList defines the lower and upper bounds of each bin
          */
      BinnedStats(const std::valarray<T> boundaryList)
            : binBoundaries(boundaryList), rejectedCount(0) 
         {}

      void add(const std::valarray<T>& statData, const std::valarray<T> binData)
         {};

      std::map<int, Stats< T > > bin;
      int rejectedCount;

   protected:

      int computeBin( const T& entry );      
      std::valarray<T> binBoundaries;

   }; // End class BinnedStats
   
      //@}   
} // End namespace gpstk

#endif // BINNED_STATS_HPP 
