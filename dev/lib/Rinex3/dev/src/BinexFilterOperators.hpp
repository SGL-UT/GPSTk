#pragma ident "$Id$"

/**
 * @file BinexFilterOperators.hpp
 * Operators for FileFilter using Binex data
 */

#ifndef GPSTK_BINEXFILTEROPERATORS_HPP
#define GPSTK_BINEXFILTEROPERATORS_HPP

#include "DayTime.hpp"
#include "FileFilter.hpp"
#include "BinexData.hpp"

#include <set>

namespace gpstk
{
   /** @addtogroup Binex */
   //@{

   typedef std::binary_function<BinexData, BinexData, bool> BinexDataBinaryOperator;


      /// Determine if two BinexData objects are equal.
   struct BinexDataOperatorEquals : 
      public BinexDataBinaryOperator
   {
   public:
      bool operator()(const BinexData& l,
                      const BinexData& r) const
         {
            return (l == r);
         }
   };

   //@}

}

#endif // GPSTK_BINEXFILTEROPERATORS_HPP
