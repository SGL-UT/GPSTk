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
#ifndef LNAVFILTERDATA_HPP
#define LNAVFILTERDATA_HPP

#include <stdint.h>
#include <NavFilterKey.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Class for filtering GPS legacy nav (id 2) data.
       */
   class LNavFilterData : public NavFilterKey
   {
   public:
         /// Set data fields to reasonable defaults, i.e. sf to NULL.
      LNavFilterData();
         /** Must be set before use in a filter.  The data is expected
          * to be an array of 10 uint32_t values where each array
          * index corresponds to a subframe word (sf[0] is word 1, and
          * so on).  The words are right-aligned meaning that the LSB
          * of word 1 is in the LSB of sf[0].
          *
          * This is stored as a pointer rather than array so that data
          * need not be moved in order to perform the filtering.
          *
          * @note The data contents pointed to by sf may be modified
          *   by some filters. */
      uint32_t *sf;

      virtual void dump(std::ostream& s) const;      
   };


      /// Sort LNavFilterData pointers by navigation message bits
   struct LNavMsgSort
      : std::binary_function<LNavFilterData*,LNavFilterData*,bool>
   {
      inline bool operator()(const LNavFilterData*const& l,
                             const LNavFilterData*const& r)
         const;
   };

      //@}


   bool LNavMsgSort ::
   operator()(const LNavFilterData*const& l, const LNavFilterData*const& r)
      const
   {
      for (unsigned sfword = 0; sfword < 10; sfword++)
      {
         if (l->sf[sfword] < r->sf[sfword])
            return true;
         if (l->sf[sfword] > r->sf[sfword])
            return false;
      }
      return false;
   }


      // Write to output stream
   std::ostream& operator<<(std::ostream& s, const LNavFilterData& nfd); 

}

#endif // LNAVFILTERDATA_HPP
