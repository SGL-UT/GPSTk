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
#ifndef GPSTK_ORBDATASYSFACTORY_HPP
#define GPSTK_ORBDATASYSFACTORY_HPP
/**
 * @file OrbDataSysFactory.hpp
 * Class that accepts a PackedNavBits that may contain a raw data 
 * for a GNSS system-wide message (e.g. GPS LNAV SV ID 51-63), determines
 * the appropriate engineering class for the data, creates/loads the
 * engineering class, and returns a pointer to the new object.
 *
 * Note that OrbDataSysFactory does NO memory management.   Once the 
 * object is created and returned, it is the responsibility of the
 * calling program to see to the deletion of the object. 
 */

#include "Exception.hpp"
#include "PackedNavBits.hpp"
#include "OrbDataSys.hpp"

namespace gpstk
{
   class OrbDataSysFactory
   {
      public:
            /// Default constructor
         OrbDataSysFactory() {};
            ///Destructor
         ~OrbDataSysFactory() {};

         static OrbDataSys* convert(const gpstk::PackedNavBits& pnb)
               throw(gpstk::InvalidParameter);

            // Factory methods for each message type
         static OrbDataSys* GPSLNAV(const gpstk::PackedNavBits& pnb)
               throw(gpstk::InvalidParameter);
         static OrbDataSys* GPSCNAV(const gpstk::PackedNavBits& pnb)
               throw(gpstk::InvalidParameter);

         static int debugLevel;

   }; // end class OrbDataSysFactory
}
#endif // SGLTK_OrbDataSysFactory_HPP
