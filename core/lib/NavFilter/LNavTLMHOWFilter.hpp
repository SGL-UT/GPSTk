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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef LNAVTLMHOWFILTER_HPP
#define LNAVTLMHOWFILTER_HPP

#include "NavFilter.hpp"

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS nav subframes with
       * 1. a bad preamble
       * 2. an invalid TOW count
       * 3. an invalid subframe ID, or
       * 4. non-zero parity bits in the HOW bits 29-30.
       * Input data is assumed to be upright.
       *
       * @attention Processing depth = 1 epoch. */
   class LNavTLMHOWFilter : public NavFilter
   {
   public:
      LNavTLMHOWFilter();

         /** Check the TLM and HOW of GPS legacy nav messages
          * (i.e. data fields common to all subframes).
          * @pre LNavFilterData::sf is set
          * @param[in,out] msgBitsIn A list of LNavFilterData* objects
          *   containing GPS legacy navigation messages (id 2).
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);

         /// Filter stores no data, therefore this does nothing.
      virtual void finalize(NavMsgList& msgBitsOut)
      {}

         /// No internal storage of subframe data so return 0.
      virtual unsigned processingDepth() const throw()
      { return 0; }

         /// Return the filter name.
      virtual std::string filterName() const throw()
      { return "TLMHOW"; }
   };

      //@}

}

#endif // LNAVTLMHOWFILTER_HPP
