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
  
         /** Constructor for creating directly from a PackedNavBits object
          * @throw InvalidParameter
          */
      OrbSysGpsL_55(const PackedNavBits& msg);
      
         /// Destructor
      virtual ~OrbSysGpsL_55() {}

         /// Clone method
      virtual OrbSysGpsL_55* clone() const;
        
         /**
          * Store the contents of Subframe 5, Page 25 in this object.
          * @param msg - 300 bits of Subframe 5, Page 25.
          * @throw InvalidParameter if message data is invalid
          */
      virtual void loadData(const PackedNavBits& msg);
         
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
          * @throw InvalidRequest if the required data has not been stored.
          */
      virtual void dumpTerse(std::ostream& s = std::cout) const;

         /**
          * @throw InvalidRequest
          */
      virtual void dumpBody(std::ostream& s = std::cout) const;

         // 182 reserved bits from words 3-10, without parity
      std::string textMsg;
      unsigned long reserved;

   protected:
      char validChar(const unsigned short testShort); 

   }; // end class OrbSysGpsL_55

} // end namespace gpstk

#endif 

