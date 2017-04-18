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
 * @file OrbSysGpsC_32.hpp
 * Designed to support loading GPS CNAV EOP data
 * Message Type 32.  NOTE: The clock data contained in the front half
 * of the message is ignored.   See the orbit handling classes for
 * that data. 
 */
#ifndef SGLTK_ORBSYSGPSC_32_HPP
#define SGLTK_ORBSYSGPSC_32_HPP


#include <string>
#include <iostream>
#include <math.h>

#include "OrbSysGpsC.hpp"
#include "OrbSysGpsC_33.hpp"

namespace gpstk
{
   class OrbSysGpsC_32 : public OrbSysGpsC
   {
   public:
      static const double SIX_HOURS;
   
         /// Default constructor
      OrbSysGpsC_32();
  
        /// Constructor for creating directly from a PackedNavBits object
      OrbSysGpsC_32(const PackedNavBits& msg)
         throw( gpstk::InvalidParameter);
      
         /// Destructor
      virtual ~OrbSysGpsC_32() {}

         /// Clone method
      virtual OrbSysGpsC_32* clone() const;
        
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
         return "EOP";
      }

      virtual std::string getNameLong() const
      {
         return "GPS CNAV EOP Parameters";
      }

        /**
         * Determine if EOP values are valid based on limitations
         * expressed in IS-GPS-200 Table 30- Karl Kovach's
         * interpretation of same following UTC Offset Error
         * anomaly of Jan 25-26, 2016
         */
      virtual bool isEopValid(const CommonTime& ct,
                              const bool initialXMit=false) const;

         /**
          * Compute UT1  as per IS-GPS-200 30.3.3.5 Table 30=VIII
          * NOTE: See preceding method, isUtcValid( ) to determine
          * if provided parameters are OK to use.  
          */
      virtual CommonTime getUT1(const CommonTime& ct, 
                            const CommonTime& tutc) const
        throw( InvalidRequest );
      virtual CommonTime getUT1(const CommonTime& ct,
                            const OrbSysGpsC_33* mt33) const
        throw( InvalidRequest );
      virtual double getxp(const CommonTime& ct) const;
      virtual double getyp(const CommonTime& ct) const;

         /** Output the contents of this orbit data to the given stream.
          * @throw Invalid Request if the required data has not been stored.
          */
      virtual void dumpTerse(std::ostream& s = std::cout) const
         throw( InvalidRequest );

      virtual void dumpBody(std::ostream& s = std::cout) const
         throw( InvalidRequest );

         // UTC Parameters
      gpstk::CommonTime ctEpoch; 
      unsigned long tEOP; 
      double PM_X;
      double PM_X_dot; 
      double PM_Y; 
      double PM_Y_dot; 
      double delta_UT1;
      double delta_UT1_dot;
      double delta_UT1_dot_per_sec;   // Convenience translation
      
   }; // end class ORBSYSGPSC_33

} // end namespace gpstk

#endif 

