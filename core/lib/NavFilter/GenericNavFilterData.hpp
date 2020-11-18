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

#ifndef GENERICNAVFILTERDATA_HPP
#define GENERICNAVFILTERDATA_HPP

#include "NavFilterKey.hpp"

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Abstract base class for any nav data to be used by NavFilter.
       * @todo in the long term it might be preferable to put this
       * functionality into NavFilterKey and eliminate this class, but
       * that would require redoing the existing filters to reflect
       * the more abstract implementation using getBits. */
   class GenericNavFilterData : public NavFilterKey
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
         /** Similar to getBits, but aggregates bits that are split
          * across multiple locations (in a single subframe).
          * @param[in] firstBit1 The position in the D1 nav message of
          *   the first bit of the numBits1 MSBs to return.  This is
          *   numbered 1-300.
          * @param[in] numBits1 The number of bits to extract from the
          *   subframe starting at firstBit1.  This can be numbered 1-30.
          * @param[in] firstBit2 The position in the D1 nav message of
          *   the first bit of the numBits2 LSBs to return.  This is
          *   numbered 1-300.
          * @param[in] numBits2 The number of bits to extract from the
          *   subframe starting at firstBit2.  This can be numbered 1-30.
          * @throw AssertionFailure if numBits1 or numBits2 is >30.
          * @return bits ((firstBit1..firstBit1+numBits1) << numBits2 +
          *   (firstBit2..firstBit2+numBits2).
          */
      uint32_t getBitsSplit(unsigned firstBit1, unsigned numBits1,
                            unsigned firstBit2, unsigned numBits2,
                            unsigned firstBit3 = 0, unsigned numBits3 = 0)
         const
      {
         uint32_t rv = ((getBits(firstBit1,numBits1) << numBits2) +
                        (getBits(firstBit2,numBits2)));
         if ((numBits3 > 0) && (firstBit3 > 0))
            rv = (rv << numBits3) + getBits(firstBit3, numBits3);
         return rv;
      }
         /** Dump the contents of this message to the given stream.
          * @param[in,out] s The stream to dump the data to.
          */
      void dump(std::ostream& s) const override
      { NavFilterKey::dump(s); }
   };

      //@}
   
} // namespace gpstk

#endif // GENERICNAVFILTERDATA_HPP
