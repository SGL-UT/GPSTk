#pragma ident "$Id:$"

/**
 * @file OrbElemFIC9.hpp
 * SF 1/2/3 data from an FIC Block 9 encapsulated in engineering terms.
 * Class inherits from OrbElem and adds those items unique to and FIC Block 9
 */

/**
*
*/

#ifndef GPSTK_ORBELEMFIC9_HPP
#define GPSTK_ORBELEMFIC9_HPP

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
#include "FICData.hpp"


namespace gpstk
{
   class OrbElemFIC9 : public OrbElem
   {
   public:
         /// Default constructor
      OrbElemFIC9();
        
        /**
         *Construct an object from an existing FIC 9 data record
         * @throw InvalidParameter if FICData object is not a Block 9 record.
         */ 
      OrbElemFIC9( const FICData& fic9 )
	 throw( InvalidParameter); 

         /// Destructor
      virtual ~OrbElemFIC9() {}

         /// Clone function
      virtual OrbElemFIC9* clone() const;

        /**
         * Load the data FIC 9 data record into an existing object.
         * All data already present are replaced.
         * @throw InvalidParameter if FICData object is not a Block 9 record.
         */
      void loadData( const FICData& fic9 )
	 throw( InvalidParameter); 

         /// Query presence of data in this object.
      bool hasData( ) const;

        /* Returns the upper bound of the URA range
         * @throw Invalid Request if the required data has not been stored
         */
      double getAccuracy()  const
         throw( InvalidRequest );

        /** Generate a formatted human-readable output of the entire contents of
         *  this object and send it to the designated output stream (default to cout).
         *  @throw Invalid Parameter if the object has been instantiated, but not loaded.
         */
      void dump(std::ostream& s = std::cout) const
         throw( InvalidRequest );  
        /** Generate a formatted human-readable one-line output that summarizes
          *  the critical times associated with this object and send it to the
          *  designated output stream (default to cout).
          *  @throw Invalid Parameter if the object has been instantiated, but not loaded.
          */   
      void dumpTerse(std::ostream& s = std::cout) const
         throw( InvalidRequest );

     

     
         /// Ephemeris overhead information
         //@{
      long  HOWtime[3];     /**< Time of subframe 1-3 (sec of week) */
      short ASalert[3];     /**< A-S and "alert" flags for each subframe. 
			        2 bit quantity with Alert flag the high
                                order bit and the A-S flag low order bit */
      CommonTime transmitTime; /** Estimated beginning time of this sample */

      short  codeflags;     /**< L2 codes */
      short  accFlag;       /**< User Range Accuracy flag */ 
      short  health;        /**< SV health */
      short  L2Pdata;       /**< L2 P data flag */
      short  IODC;          /**< Index of data-clock  */
      short  IODE;          /**< Index of data-eph    */
      short  fitint;        /**< Fit interval flag */
      double Tgd;           /**< L1 and L2 correction term */
      long   AODO;          /**< Age of Data offset from subframe 2.  Note: This field may not be present
                                 in older FIC data records.  A valid value will be greater than zero.  
                                 A value  of zero indicates the AODO is not available in this record. */ 
         //@}

      friend std::ostream& operator<<(std::ostream& s, 
                                      const OrbElemFIC9& eph);
   protected:
   
      void dumpFIC9(std::ostream& s = std::cout) const;
        
    
       

   }; // class OrbElemFIC9

   //@}

} // namespace

#endif
