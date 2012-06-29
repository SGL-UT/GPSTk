#pragma ident "$Id:$"

/**
 * @file OrbElemFIC9.hpp
 * SF 1/2/3 data from an FIC Block 9 encapsulated in engineering terms.
 * Class inherits from OrbElem and adds those items unique to and FIC Block 9
 */

/**
*
*/

#ifndef GPSTK_ORBELEMRINEX9_HPP
#define GPSTK_ORBELEMRINEX9_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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

#include <string>
#include <iostream>

#include "OrbElem.hpp"
#include "RinexNavData.hpp"


namespace gpstk
{
   /** @addtogroup ephemcalc */
   //@{

      /**
       * Ephemeris information for a single SF 1/2/3.  This class
       * encapsulates the ephemeris navigation message (subframes 1-3)
       */
   class OrbElemRinex : public OrbElem
   {
   public:
         /// Default constructor
      OrbElemRinex();

      OrbElemRinex( const RinexNavData& rinNav )
	 throw( InvalidParameter); 

         /// Destructor
      virtual ~OrbElemRinex() {}

         /// Load a FIC 9 into an existing object
      void loadData( const RinexNavData& rinNav )
	 throw( InvalidParameter); 

         /// Query presence of data in this object.
      bool hasData( ) const;
       

         /// Returns the transmit time
      CommonTime getTransmitTime() const;

         /// Returns the upper bound of the URA range
      double getAccuracy()  const
         throw( InvalidRequest );

         /// Output the contents of this ephemeris to the given stream.
      void dump(std::ostream& s = std::cout) const
         throw( InvalidRequest );    
      void dumpTerse(std::ostream& s = std::cout) const
         throw( InvalidRequest );

     
         /// Ephemeris overhead information
         //@{
      
      CommonTime transmitTime; /** Estimated beginning time of this sample */

      short  codeflags;     /**< L2 codes */
      double accuracyValue; /**< User Range Accuracy (meters) */ 
      short  health;        /**< SV health */
      short  L2Pdata;       /**< L2 P data flag */
      short  IODC;          /**< Index of data-clock  */
      short  fitDuration;   /**< Fit duration (hours) */
      double Tgd;           /**< L1 and L2 correction term */
      long   HOWtime;        /**< Handover Word time */
         //@}

      friend std::ostream& operator<<(std::ostream& s, 
                                      const OrbElemRinex& eph);

    
       

   }; // class OrbElemRinex

   //@}

} // namespace

#endif
