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
#ifndef CNAVEMPTYFILTER_HPP
#define CNAVEMPTYFILTER_HPP

#include <NavFilter.hpp>

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS CNAV subframes with empty contents.  In
       *  this case "empty" means bits 38-276 are all zero
       *  or bits 38-276 are alternating 1/0.  See IS-GPS-200
       *  Section 30.3.3.
       *
       * @attention Processing depth = 1 epoch. */
   class CNavEmptyFilter : public NavFilter
   {
   public:
      CNavEmptyFilter();

         /** Filter subframes in msgBitsIn that are empty.
          * @pre CNavFilterData::sf is set
          * @param[in,out] msgBitsIn A list of CNavFilterData* objects
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
      { return "Empty"; }
   };

      //@}

}

#endif // CNAVEMPTYFILTER_HPP
