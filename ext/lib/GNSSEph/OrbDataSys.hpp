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
 * @file OrbDataSys.hpp
 *  This class encapsulates the "least common denominator"
 *  for system-level navigation message data across multiple GNSS.
 *  This primarily means defining that there must exist a unique
 *  identifier for each message type that contains different 
 *  contents.  That unique identifier will be used in the container
 *  class that stores set of objects derived from this base class.
 *
 */

#ifndef GPSTK_ORBDATASYS_HPP
#define GPSTK_ORBDATASYS_HPP

#include <string>

#include "OrbData.hpp"

namespace gpstk
{
   class OrbDataSys : public OrbData
   {
   public:
         /// Constructors
	      /// Default constuctor
      OrbDataSys();

         /// Destructor
      virtual ~OrbDataSys() {}

         // Must be implemented by descendent concrete classes.
      virtual OrbDataSys* clone() const = 0;

      virtual bool isSameData(const OrbData* right) const;

      virtual std::list<std::string> compare(const OrbDataSys* right) const;

      virtual void dumpFooter(std::ostream& s = std::cout) const
         throw( InvalidRequest );

         /// Members
         /// Only member is the unique identifier.  The 
         /// definition varies by navigation message 
         /// type. 
      unsigned long UID; 

   }; // end class OrbDataSys

   std::ostream& operator<<(std::ostream& s,
                            const OrbDataSys& eph);

} // end namespace

#endif // GPSTK_ORBDATASYS_HPP
