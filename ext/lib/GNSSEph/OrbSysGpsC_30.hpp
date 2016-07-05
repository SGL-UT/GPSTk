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
 * @file OrbSysGpsC_30.hpp
 * Designed to support loading GPS CNAV UTC data
 * Message Type 33.  NOTE: The clock data contained in the front half
 * of the message is ignored.   See the orbit handling classes for
 * that data. 
 */

#ifndef SGLTK_ORBSYSGPSC_30_HPP
#define SGLTK_ORBSYSGPSC_30_HPP


#include <string>
#include <iostream>
#include <math.h>

#include "OrbSysGpsC.hpp"

namespace gpstk
{
   class OrbSysGpsC_30 : public OrbSysGpsC
   {
   public:
   
         /// Default constructor
      OrbSysGpsC_30();
  
        /// Constructor for creating directly from a PackedNavBits object
      OrbSysGpsC_30(const PackedNavBits& msg)
         throw( gpstk::InvalidParameter);
      
         /// Destructor
      virtual ~OrbSysGpsC_30() {}

         /// Clone method
      virtual OrbSysGpsC_30* clone() const;
        
         /**
          * Store the contents of Subframe 4, Page 18 in this object.
          * @param msg - 300 bits of Subframe 4, Page 18.
          * @throw InvalidParameter if message data is invalid
          */
      virtual void loadData(const PackedNavBits& msg)
         throw(gpstk::InvalidParameter); 
         
      virtual bool isSameData(const OrbData* right) const;      

      virtual std::string getName() const
      {
         return "ISC";
      }

      virtual std::string getNameLong() const
      {
         return "GPS CNAV ISC/Iono Parameters";
      }

         /** Output the contents of this orbit data to the given stream.
          * @throw Invalid Request if the required data has not been stored.
          */
      virtual void dumpTerse(std::ostream& s = std::cout) const
         throw( InvalidRequest );

      virtual void dumpBody(std::ostream& s = std::cout) const
         throw( InvalidRequest );

         // The following are for the transmitting SV
         // Not really "system" data, but that's where the data are in the message.
      double Tgd;
      double ISC_L1CA;
      double ISC_L2C;
      double ISC_L5I5;
      double ISC_L5Q5;

         // See IS-GPS-705 20.3.3.3.1.2.  If transmitted data is "1000000000000" the
         // term is not available.  The following members are set accordingly.
      bool   avail_Tgd;
      bool   avail_L1CA;
      bool   avail_L2C;
      bool   avail_L5I5;
      bool   avail_L5Q5;

         // NOTE: units are sec, sec/rad, sec/rad**2, and sec/rad**3
      double alpha[4];    
      double beta[4];     

   }; // end class ORBSYSGPSC_30

} // end namespace gpstk

#endif 

