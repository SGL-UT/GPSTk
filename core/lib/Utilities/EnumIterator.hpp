//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================

// Adapted from a post on stackoverflow.com

#ifndef GPSTK_ENUMITERATOR_HPP
#define GPSTK_ENUMITERATOR_HPP

#include <type_traits>

namespace gpstk
{
      /** This class simplifies the process of iterating over strongly
       * typed enums.
       * Example:
       *   typedef EnumIterator<enum, enum::firstVal, enum::lastVal> Iterator;
       *   for (enum x : Iterator()) { ... }
       * Typically, the iterator will be defined in the include file
       * that defines the enum.  The endVal value should be first
       * value that will NOT be processed in a loop.  This means
       * defining a final "Last" enumeration value in the enum that
       * won't be iterated over.  This is done to facilitate adding
       * additional enumeration values without having to change the
       * code that defines the iterator.
       *
       * @warning Do not attempt to use this on enumerations that have
       * assigned values resulting in gaps.  This will result in
       * iterating over invalid enumeration values.
       *
       * @see CarrierBand.hpp
       */
   template <typename C, C beginVal, C endVal>
   class EnumIterator
   {
         /// Value type as derived from the enum typename.
      typedef typename std::underlying_type<C>::type ValType;
         /// Current iterator value
      ValType val;
   public:
         /** Default iterator initializes to the beginVal specified in
          * the template instantiation. */
      EnumIterator()
            : val(static_cast<ValType>(beginVal))
      {}
         /// Initialize the iterator to a specific value.
      EnumIterator(const C& f)
            : val(static_cast<ValType>(f))
      {}
         /** Increment the iterator to the next enum
          * @note This assumes that there are no gaps between enum
          *   values, otherwise it could not have a valid
          *   enumeration.
          * @note This is the prefix operator. */
      EnumIterator& operator++()
      {
         val++;
         return *this;
      }
         /// Dereference the iterator by returning the current enum value
      C operator*()
      { return static_cast<C>(val); }
         /** Iterator start value, which can be the value initialized
          * from the value constructor. */
      EnumIterator begin()
      { return *this; }
         /// Iterator end value.
      EnumIterator end()
      {
         static const EnumIterator endIter = EnumIterator(endVal);
         return endIter;
      }
         /// Comparison to assist in iteration.
      bool operator!=(const EnumIterator& i)
      { return val != i.val; }
   };
}

#endif // GPSTK_ENUMITERATOR_HPP
