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
 * @file OrbSysGpsL_51.hpp
 * Designed to support loading GPS LNAV reserved pages
 * Subframe 4, Page 14, SV ID 52
 *             Page 15, SV ID 53
 *             Page 12,19,20,22,23, & 24, SV ID 58-62
 *             Page  1, 6,11,16,21, SV ID 57
 */

#ifndef SGLTK_ORBSYSGPSL_RESERVED_HPP
#define SGLTK_ORBSYSGPSL_RESERVED_HPP


#include <string>
#include <iostream>
#include <math.h>

#include "OrbSysGpsL.hpp"

namespace gpstk
{
   class OrbSysGpsL_Reserved : public OrbSysGpsL
   {
   public:
   
         /// Default constructor
      OrbSysGpsL_Reserved();
  
        /// Constructor for creating directly from a PackedNavBits object
      OrbSysGpsL_Reserved(const PackedNavBits& msg)
         throw( gpstk::InvalidParameter);
      
         /// Destructor
      virtual ~OrbSysGpsL_Reserved() {}

         /// Clone method
      virtual OrbSysGpsL_Reserved* clone() const;
        
         /**
          * Store the contents of Subframe 5, Page 25 in this object.
          * @param msg - 300 bits of Subframe 5, Page 25.
          * @throw InvalidParameter if message data is invalid
          */
      virtual void loadData(const PackedNavBits& msg)
         throw(gpstk::InvalidParameter); 
         
      virtual bool isSameData(const OrbData* right) const;      

      virtual std::string getName() const
      {
         return "Res";
      }

      virtual std::string getNameLong() const
      {
         return "GPS LNAV Reserved Page";
      }

         /** Output the contents of this orbit data to the given stream.
          * @throw Invalid Request if the required data has not been stored.
          */
      virtual void dumpTerse(std::ostream& s = std::cout) const
         throw( InvalidRequest );

      virtual void dumpBody(std::ostream& s = std::cout) const
         throw( InvalidRequest );

         // 182 reserved bits from words 3-10, without parity
      PackedNavBits pnb;

   }; // end class OrbSysGpsL_Reserved

} // end namespace gpstk

#endif 

