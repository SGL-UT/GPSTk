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
#ifndef CNAVFILTERDATA_HPP
#define CNAVFILTERDATA_HPP

#include <stdint.h>
#include "NavFilterKey.hpp"
#include "PackedNavBits.hpp"

namespace gpstk
{
   class CNavFilterData : public NavFilterKey
   {
   public:
      CNavFilterData();
      CNavFilterData(PackedNavBits* pnb);

      void loadData(PackedNavBits* pnb); 

         // The actual message data is stored in a PackedNavBits object
      PackedNavBits* pnb;

      virtual void dump(std::ostream& s) const;      
   };

      // Sort CNavFilterData objects by navigation message bits
   struct CNavMsgSort
      : std::binary_function<CNavFilterData, CNavFilterData, bool>
   {
      inline bool operator() (const CNavFilterData* l,
                              const CNavFilterData* r) const;
   };

   bool CNavMsgSort::
   operator() (const CNavFilterData* l,
               const CNavFilterData* r) const
   {
      return *(l->pnb) < *(r->pnb);
   }

      // Write to output stream
   std::ostream& operator<<(std::ostream& s, const CNavFilterData& nfd); 
}

#endif
