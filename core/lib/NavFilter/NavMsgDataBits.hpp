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

#ifndef NAVMSGDATABITS_HPP
#define NAVMSGDATABITS_HPP

#include "NavMsgData.hpp"

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Define a class for storing NavFilterKey data in an array of
       * 0s and 1s as in the MDH format.  This class is intended to be
       * used as a template argument for NavFilterKey objects
       * e.g. D1NavFilterData.  It is not expected to be used directly
       * by developers. */
   class NavMsgDataBits : public NavMsgData
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
         /** The subframe contents, an array of bit values which are
          * expected to be either 0 or 1. */
      int8_t *sf;
   };

      //@}
   
} // namespace gpstk

#endif // NAVMSGDATABITS_HPP
