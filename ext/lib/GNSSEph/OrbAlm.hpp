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
  *  @file OrbAlm.hpp
  *  This class is an "organizational convenience" for grouping all the
  *  classes that contain almanac data.  It is a descendent of OrbElemBase
  *  as all the almanac (orbit) data share the characteristics of OrbElemBase.
  *  (See OrbDataSys for non-orbit constellation data.)  The critical piece
  *  added by OrbAlm is the concept that there is "Subject SatID".  That is to
  *  say, the data were transmitted by the SV denoted by OrbData::satID, but the
  *  almanac is relevant to the subjectSatID contained in OrbAlm.  This allows
  *  the the possibiity that there may be multiple different almanac orbits
  *  available for a given subject SV at a given moment with each version.
  *
  *  It would have been nice to have this class descend from OrbElem and 
  *  thereby have access to the svXvt( ) in OrbElem.  However, the Glonass
  *  almanac data doesn't conform to the structure enforced by OrbElem.  It
  *  was felt that the need to group all almanacs together outweighed the 
  *  convenience of reusing svXvt( ).
  *
  *  Even though it has a member, OrbAlm is still pure virtual and no
  *  objects of this type may be created. 
  */ 

#ifndef GPSTK_ORBALM_HPP
#define GPSTK_ORBALM_HPP

#include <string>
#include <iostream>

#include "OrbElem.hpp"
#include "PackedNavBits.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   class OrbAlm : public OrbElemBase
   {
   public:
         /// Default constructor
      OrbAlm();

         /// Destructor
      virtual ~OrbAlm() {}

        /// Clone method
      virtual OrbAlm* clone() const = 0;

         /**  Load an existing object from a PackedNavBits object.
           *  @throw InvalidParameter if the data are not consistent.
           */ 
      void loadData( const gpstk::PackedNavBits& pnbr )
	       throw(gpstk::InvalidParameter); 

      virtual std::string getName() const
      {
         return "OrbAlm";
      }
 
      virtual std::string getNameLong() const
      {
         return "SV Almanac Orbit";
      }

      virtual bool isSameData(const OrbElemBase* right) const = 0;      
      
      virtual void dumpHeader(std::ostream& s = std::cout) const
         throw( gpstk::InvalidRequest );

         /** Generate a formatted human-readable output of the entire contents of
          *  this object and send it to the designated output stream (default to cout).
          *  @throw Invalid Parameter if the object has been instantiated, but not loaded.
          */
      virtual void dump(std::ostream& s = std::cout) const
         throw( gpstk::InvalidRequest ) {} 

         /** Generate a formatted human-readable one-line output that summarizes
          *  the critical times associated with this object and send it to the
          *  designated output stream (default to cout).
          *  @throw Invalid Parameter if the object has been instantiated, but not loaded.
          */   
      virtual void dumpTerse(std::ostream& s = std::cout) const
         throw( gpstk::InvalidRequest ) {}   

         // subjectSV stores the satellite identifier for the satellite orbit which
         // is desribed in thie data set.   OrbData::satID stors the satellite 
         // identifier for the satellite that TRANSMITTED the data set contained 
         // herein. 
      gpstk::SatID subjectSV; 

   }; // end class OrbAlm

   //@}

   std::ostream& operator<<(std::ostream& s, 
                                     const OrbAlm& eph);

} // end namespace 

#endif // GPSTK_ORBALM_HPP
