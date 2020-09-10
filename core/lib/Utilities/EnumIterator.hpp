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
