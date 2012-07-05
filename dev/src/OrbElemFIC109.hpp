#pragma ident "$Id:$"

/**
 * @file OrbElemFIC109.hpp
 */

/**
*
*/

#ifndef GPSTK_ORBELEMFIC109_HPP
#define GPSTK_ORBELEMFIC109_HPP

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

#include "OrbElemFIC9.hpp"
#include "FICData.hpp"



namespace gpstk
{
   /** @addtogroup ephemcalc */
   //@{

      /**
       * Ephemeris information for a single SF 1/2/3.  This class
       * encapsulates the ephemeris navigation message (subframes 1-3)
       */
   class OrbElemFIC109 : public OrbElemFIC9
   {
   public:
         /// Default constructor
      OrbElemFIC109();

      OrbElemFIC109( const long SF1[10],
                     const long SF2[10],
                     const long SF3[10],
                     const short PRNID,
                     const short XmitGPSWeek ) 
         throw( InvalidParameter);

      OrbElemFIC109( const FICData& fic109 )
	 throw( InvalidParameter); 

      void loadData( const long SF1[10],
                     const long SF2[10],
                     const long SF3[10],
                     const short PRNID,
                     const short XmitGPSWeek ) 
         throw( InvalidParameter);

         /// Destructor
      virtual ~OrbElemFIC109() {}

         /// Load a FIC 9 into an existing object
      void loadData( const FICData& fic109 )
	 throw( InvalidParameter); 

         /// Output the contents of this ephemeris to the given stream.
      void dump(std::ostream& s = std::cout) const
         throw( InvalidRequest );    
     
         /// Ephemeris overhead information
         //@{ 
         //@}

      


      friend std::ostream& operator<<(std::ostream& s, 
                                      const OrbElemFIC109& eph);

    
       

   }; // class OrbElemFIC109

   //@}

} // namespace

#endif
