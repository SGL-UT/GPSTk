#ifndef NAVMSGDATA_HPP
#define NAVMSGDATA_HPP

#include <iostream>
#include <cstdint>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Define an abstract base class (interface) for the purpose
       * of abstracting how NavFilter-derived classes can look at the
       * data.  This allows the filters to be implemented in a fashion
       * that is completely independent of the internal storage.
       */
   class NavMsgData
   {
   public:
         /** Get a value, up to 32 bits, out of the nav message.
          * @param[in] start The first bit (counting from 1 through
          *   the maximum number of bits in a single subframe) of the
          *   desired bits.
          * @param[in] num The number of consecutive bits to retrieve.
          * @return The value extracted from the nav message starting
          *   at start and ending at (start-1+num).
          */
      virtual uint32_t getBits(unsigned start, unsigned num) const = 0;
         /** Dump the contents of this message to the given stream.
          * @param[in,out] s The stream to dump the data to.
          * @param[in] totalBits The total number of bits to dump
          *   (usually the number of bits in the subframe).
          */
      virtual void dump(std::ostream& s, unsigned totalBits) const = 0;
   };

      //@}
   
} // namespace gpstk

#endif // NAVMSGDATA_HPP
