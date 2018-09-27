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
#ifndef GPSTK_ORBALMFACTORY_HPP
#define GPSTK_ORBALMFACTORY_HPP
/**
 * @file OrbAlmFactory.hpp
 * Class that accepts a PackedNavBits that may contain a raw data 
 * for a GNSS orbit-almanac (e.g. GPS LNAV SV ID 1-32), determines
 * the appropriate OrbAlm-descendent class for the data, creates/loads the
 * class, and returns a pointer to the new object.
 *
 * Note that OrbAlmFactory does NO memory management.   Once the 
 * object is created and returned, it is the responsibility of the
 * calling program to see to the deletion of the object. 
 */

#include "Exception.hpp"
#include "PackedNavBits.hpp"
#include "OrbAlm.hpp"

namespace gpstk
{
   class OrbAlmFactory
   {
   public:
         /// Default constructor
      OrbAlmFactory();
            ///Destructor
      ~OrbAlmFactory() {};

      OrbAlm* convert(const gpstk::PackedNavBits& pnb)
               throw(gpstk::InvalidParameter);

            // Factory methods for each message type
      OrbAlm* GPSLNAV(const gpstk::PackedNavBits& pnb)
               throw(gpstk::InvalidParameter);
      OrbAlm* GPSCNAV(const gpstk::PackedNavBits& pnb)
               throw(gpstk::InvalidParameter);

            // The following are stubs at this time.  The may be implemented in the future.
            // They are declared virtual in case they need to be defined in a subclass 
            // independent of the GPSTk. 
      OrbAlm* BeiDou_D1(const gpstk::PackedNavBits& pnb)
               throw(gpstk::InvalidParameter);
      OrbAlm* BeiDou_D2(const gpstk::PackedNavBits& pnb)
               throw(gpstk::InvalidParameter);
      OrbAlm* GloCivilF(const gpstk::PackedNavBits& pnb)
               throw(gpstk::InvalidParameter);

      int debugLevel;

   }; // end class OrbAlmFactory
}
#endif // SGLTK_OrbAlmFactory_HPP
