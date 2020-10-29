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

         /**
          * @throw InvalidParameter
          */
      OrbAlm* convert(const gpstk::PackedNavBits& pnb);

            // Factory methods for each message type
         /**
          * @note It is up to the caller to free the memory pointed to
          *   by the return value.
          * @throw InvalidParameter
          */
      OrbAlm* GPSLNAV(const gpstk::PackedNavBits& pnb);
         /**
          * @note It is up to the caller to free the memory pointed to
          *   by the return value.
          * @throw InvalidParameter
          */
      OrbAlm* GPSCNAV(const gpstk::PackedNavBits& pnb);

      int debugLevel;

   }; // end class OrbAlmFactory
}
#endif // SGLTK_OrbAlmFactory_HPP
