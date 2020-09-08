#ifndef NAVMSGDATAPNB_HPP
#define NAVMSGDATAPNB_HPP

#include "NavMsgData.hpp"
#include "PackedNavBits.hpp"

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Define a class for storing NavFilterKey data in a
       * PackedNavBits object.  This class is intended to be used as a
       * template argument for NavFilterKey objects
       * e.g. D1NavFilterData.  It is not expected to be used directly
       * by developers. */
   class NavMsgDataPNB : public NavMsgData
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
      uint32_t getBits(unsigned start, unsigned num) const override;
         /** Dump the contents of this message to the given stream.
          * @param[in,out] s The stream to dump the data to.
          * @param[in] totalBits The total number of bits to dump
          *   (usually the number of bits in the subframe).
          */
      void dump(std::ostream& s, unsigned totalBits) const override;
         /// The subframe contents.
      PackedNavBits *sf;
   };

      //@}
   
} // namespace gpstk

#endif // NAVMSGDATAPNB_HPP
