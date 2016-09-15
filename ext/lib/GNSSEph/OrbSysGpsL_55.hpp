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
 * @file OrbSysGpsL_55.hpp
 * Designed to support loading GPS LNAV Subframe 4, Page 17, SV ID 55
 */

#ifndef SGLTK_ORBSYSGPSL_55_HPP
#define SGLTK_ORBSYSGPSL_55_HPP


#include <string>
#include <iostream>
#include <math.h>

#include "OrbSysGpsL.hpp"

namespace gpstk
{
   class OrbSysGpsL_55 : public OrbSysGpsL
   {
   public:
   
         /// Default constructor
      OrbSysGpsL_55();
  
        /// Constructor for creating directly from a PackedNavBits object
      OrbSysGpsL_55(const PackedNavBits& msg)
         throw( gpstk::InvalidParameter);
      
         /// Destructor
      virtual ~OrbSysGpsL_55() {}

         /// Clone method
      virtual OrbSysGpsL_55* clone() const;
        
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
         return "Text";
      }

      virtual std::string getNameLong() const
      {
         return "GPS LNAV Subframe 4 Page 17";
      }

         /** Output the contents of this orbit data to the given stream.
          * @throw Invalid Request if the required data has not been stored.
          */
      virtual void dumpTerse(std::ostream& s = std::cout) const
         throw( InvalidRequest );

      virtual void dumpBody(std::ostream& s = std::cout) const
         throw( InvalidRequest );

         // 182 reserved bits from words 3-10, without parity
      std::string textMsg;
      unsigned long reserved;

   protected:
      char validChar(const unsigned short testShort); 

   }; // end class OrbSysGpsL_55

} // end namespace gpstk

#endif 

