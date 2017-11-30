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

#ifndef CNAVCOOKFILTER_HPP
#define CNAVCOOKFILTER_HPP

#include <NavFilter.hpp>
#include <CNavFilterData.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** "Cook" GPS civil nav messages by turning words upright.
       * Data in CNavFilterData::sf are modified.
       *
       * @attention Processing depth = 1 epoch. */
   class CNavCookFilter : public NavFilter
   {
   public:
      CNavCookFilter();

         /** Turn words in a GPS CNAV message upright.  This results
          * in the expected 0x8b preamble in the TLM and subsequent
          * bits as expected per the IS-GPS-200 Section 30.
          * @pre CNavFilterData::sf is set
          * @param[in,out] msgBitsIn a list of CNavFilterData* objects
          *   containing GPS CNAV data.
          * @param[out] msgBitsOut the "cooked" subframes.  All
          *   contents of msgBitsIn will be immediately seen in
          *   msgBitsOut. */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);

         /// Filter stores no data, therefore this does nothing.
      virtual void finalize(NavMsgList& msgBitsOut)
      {}

         /// No internal storage of subframe data so return 0.
      virtual unsigned processingDepth() const throw()
      { return 0; }

         /// Turn an CNAV subframe data upright.
      static void cookSubframe(CNavFilterData* fd);

         /// Return the filter name.
      virtual std::string filterName() const throw()
      { return "Cook"; }
   };

      //@}
}

#endif // LNAVCOOKFILTER_HPP
