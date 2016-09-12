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
 * @file OrbData.hpp
 *  This class encapsulates the "least common denominator" interface
 *  to GNSS navigation message data.   The concepts included are
 *     - transmitting SV
 *     - carrier/code combination
 *     - time of first transmission
 *     - bool isSameData( )  - is the "data payload" of two objects of
 *       the same subclass identical
 *     - operator<<( ) 
 *     - dump(std::ostream& out=std::cout) 
 * 
 *  This is a pure virtual class and designed to support polymorphic behavior.
 *  Therefore the will need
 *  to instantiate a descendent of this
 *  class as opposed to instantiating this class directly.  The
 *  descendent classes provide the functionality to load 
 *  data from various navigation message formats
 *  and types and then manipulate the data to produce 
 *  useful information. 
 *
 *  The existence of a common base class enables the storage of objects
 *  in a common store.
 */

#ifndef GPSTK_ORBDATA_HPP
#define GPSTK_ORBDATA_HPP
 
#include <list>

#include "Exception.hpp"
#include "CommonTime.hpp"
#include "GNSSconstants.hpp"
#include "SatID.hpp"
#include "ObsID.hpp"

namespace gpstk
{
   class OrbData
   {
   public:
         /// Constructors
	      /// Default constuctor
      OrbData( );


         /// Destructor
      virtual ~OrbData() {}

         /// Clone method.
	 /// Return pointer to new copy of this type.
	 /// Implication of the "= 0" at this end is that this is a
	 /// "pure virtual" method and that makes OrbData an abstract
	 /// class.  That is to say no objects of type OrbData may
	 /// be constructed.   This is a good thing since OrbData
	 /// doesn't even provide methods to load its' members.
	 /// Only its' descendents may be instantiated.
      virtual OrbData* clone() const = 0;

	 /**
          *   Return true if orbit data have been loaded.
          *   Returns false if the object has been instantiated,
          *   but no data have been loaded.
          */
      virtual bool dataLoaded( ) const;

      virtual std::string getName() const = 0;

      virtual std::string getNameLong() const = 0;

          /** Returns true if this two objects are 
           *   a.) same concrete type, and
           *   b.) same data contents.
           * This is intended as a "data uniqueness test" to allow
           * detection of successive transmissions of same data
           * and avoid duplicate storage.  The exact rules for 
           * uniqueness will vary by descendent class. 
           */
      virtual bool isSameData(const OrbData* right) const = 0;

          /** Compare two OrbData descendent objects.
           *  Any differences are summarized and written to the output stream
           *  provided.
           *  The base method compares the members common to the base.  The descendent classes
           *  should add additional member comparisions as needed in order   
           *  to provide a complete comparison of members of interest to be compared.   
           *  The return list is a list of the members that disagreed.  
           */
      virtual std::list<std::string> compare(const OrbData* right) const;

         /** Output the contents of this orbit data to the given stream.
          * @throw Invalid Request if the required data has not been stored.
          */
      virtual void dumpTerse(std::ostream& s = std::cout) const
         throw( InvalidRequest ) = 0;

      virtual void dumpHeader(std::ostream& s = std::cout) const
         throw( InvalidRequest ) = 0;

      virtual void dumpBody(std::ostream& s = std::cout) const
         throw( InvalidRequest ) = 0;

      virtual void dumpFooter(std::ostream& s = std::cout) const
         throw( InvalidRequest ) = 0;

      virtual void dump(std::ostream& s = std::cout) const
	       throw( InvalidRequest );

         /// Overhead information
         //@{
      bool    dataLoadedFlag;   /**< True if data is present, False otherwise */
      SatID   satID;	          /**< Define satellite system and specific SV */
      ObsID   obsID;            /**< Defines carrier and tracking code */
      CommonTime beginValid;    /**< Time at beginning of validity */

   }; // end class OrbData

   //@}

} // end namespace

#endif // GPSTK_OrbData_HPP
