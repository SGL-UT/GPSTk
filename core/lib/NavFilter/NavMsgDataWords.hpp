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

#ifndef NAVMSGDATAWORDS_HPP
#define NAVMSGDATAWORDS_HPP

#include "NavMsgData.hpp"
#include <math.h>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Define a class for storing NavFilterKey data in an array of
       * 32-bit words.  The template argument WordSize indicates how
       * many of the 32-bit words contain data, e.g. 30 bits for GPS
       * LNav and BeiDou D1 and D2 nav.  WordSize cannot be larger
       * than 32.  This class is intended to be used as a template
       * argument for NavFilterKey objects e.g. D1NavFilterData.  It
       * is not expected to be used directly by developers. */
   template <std::size_t WordSize = 30>
   class NavMsgDataWords : public NavMsgData
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
         /// The subframe contents, an array of words of WordSize bits.
      uint32_t *sf;
   };

      //@}
   
   template <std::size_t WordSize>
   uint32_t NavMsgDataWords<WordSize> ::
   getBits(unsigned start, unsigned num) const
   {
      if (num == 0)
         return 0;
         // mask and shift the desired bits
      uint32_t fbmodN = (start % WordSize);
      return ((sf[start/WordSize] & (((uint32_t)-1) >> (fbmodN+1))) >>
              (31-fbmodN-num));
   }


   template <std::size_t WordSize>
   void NavMsgDataWords<WordSize> ::
   dump(std::ostream& s, unsigned totalBits) const
   {
      unsigned numWords = ceil((double)totalBits / (double)WordSize);
      s << std::hex << std::setfill('0');
      for (unsigned j=0;j<numWords;j++)
      {
         s << "0x" << std::setw(8) << sf[j] << " ";
      }
      s << std::dec << std::setfill(' ') << " ";
   }

} // namespace gpstk

#endif // NAVMSGDATAWORDS_HPP
