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

/**
 * @file OrbSysGpsC.hpp
 *  This class encapsulates the "least common denominator"
 *  for system-level navigation message data for GPS CNAV.
 */

#ifndef GPSTK_ORBSYSGPSC_HPP
#define GPSTK_ORBSYSGPSC_HPP

#include <string>

#include "OrbDataSys.hpp"
#include "PackedNavBits.hpp"

namespace gpstk
{
   class OrbSysGpsC : public OrbDataSys
   {
   public:
        /// Constructors
	      /// Default constuctor
      OrbSysGpsC();

         /// Destructor
      virtual ~OrbSysGpsC() {}

      virtual bool isSameData(const OrbData* right) const;

      virtual std::list<std::string> compare(const OrbSysGpsC* right) const;

      virtual void dumpHeader(std::ostream& s = std::cout) const
         throw( InvalidRequest );

      void setUID(const PackedNavBits& msg);  

         /// For GPS data, the UID (stored in OrbDataSys) is the Message Type.
      unsigned short getMT() const;

         /// Members
         /// NONE
   }; // end class OrbSysGpsC

} // end namespace

#endif // GPSTK_ORBSYSGPSC_HPP
