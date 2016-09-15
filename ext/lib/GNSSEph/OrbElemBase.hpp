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
 * @file OrbElemBase.hpp
 *  This class encapsulates the "least common denominator" interface
 *  to GNSS orbits.  Note that OrbElem was defined PRIOR to OrbElemBase.
 *  However, we then needed to handle GLONASS, which uses a tabular
 *  orbit rather than pseudo-Keplerian elements.   As a result, we
 *  need a virtual base class that defined a common interface to 
 *  key functionality without in any way defining how that functionality
 *  is implemented.   This was necessary in order that OrbElemStore could
 *  serve as a storage base for all the derived orbit classes.
 *
 *  Obviously, user will need to instantiate a descendent of this
 *  class as opposed to instantiating this class directly.  The
 *  descendent classes provide the functionality to load the
 *  coefficients from various navigation message formats
 *  and types and then manipulate the data to produce SV positions,
 *  clock offsets, and status information. 
 */

#ifndef GPSTK_ORBELEMBASE_HPP
#define GPSTK_ORBELEMBASE_HPP

#include "Exception.hpp"
#include "CommonTime.hpp"
#include "Xvt.hpp"
#include "GNSSconstants.hpp"
#include "SatID.hpp"
#include "ObsID.hpp"
#include "Rinex3NavData.hpp"
//#include "MathBase.hpp"

namespace gpstk
{
   class OrbElemBase
   {
   public:
         /// Constructors
	      /// Default constuctor
      OrbElemBase( );


         /// Destructor
      virtual ~OrbElemBase() {}

         /// Clone method.
	 /// Return pointer to new copy of this type.
	 /// Implication of the "= 0" at this end is that this is a
	 /// "pure virtual" method and that makes OrbElemBase an abstract
	 /// class.  That is to say no objects of type OrbElemBase may
	 /// be constructed.   This is a good thing since OrbElemBase
	 /// doesn't even provide methods to load its' members.
	 /// Only its' descendents may be instantiated.
      virtual OrbElemBase* clone() const = 0;

         /**
          * Returns true if the time, ct, is within the period of validity of this
	  * OrbElemBase object.
          * @throw Invalid Request if the required data has not been stored.
          */
      virtual bool isValid(const CommonTime& ct) const throw(InvalidRequest);

	 /**
          *   Return true if orbit data have been loaded.
          *   Returns false if the object has been instantiated,
          *   but no data have been loaded.
          */
      virtual bool dataLoaded( ) const;

      virtual std::string getName() const = 0;

      virtual std::string getNameLong() const = 0;

         /** This function returns the health status of the SV.
          * @throw Invalid Request if the required data has not been stored.
          */
      bool isHealthy() const throw(gpstk::InvalidRequest);

         /** Compute the satellite clock bias (sec) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      virtual double svClockBias(const CommonTime& t) const throw(gpstk::InvalidRequest) = 0;

         /** Compute the satellite clock bias (meters) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      virtual double svClockBiasM(const CommonTime& t) const throw(gpstk::InvalidRequest) = 0;

         /** Compute the satellite clock drift (sec/sec) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      virtual double svClockDrift(const CommonTime& t) const throw(gpstk::InvalidRequest) = 0;


         /** Compute satellite position at the given time
          * using this orbit data.
          * @throw Invalid Request if the required data has not been stored.
          */
      virtual Xvt svXvt(const CommonTime& t) const throw(gpstk::InvalidRequest) = 0;

         /** Compute satellite relativity correction (sec) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      virtual double svRelativity(const CommonTime& t) const throw( gpstk::InvalidRequest ) = 0;

          /** Returns true if this two objects are 
           *   a.) same concrete type, and
           *   b.) same data contents.
           * This is intended as a "data uniqueness test" to allow
           * detection of successive transmissions of same data
           * and avoid duplicate storage.  The exact rules for 
           * uniqueness will vary by descendent class. 
           * NOTE: This would be better as a pure virtual method.
           * However, it came about late in the process, so it is
           * implemented here as a stub that returns false.
           */
      virtual bool isSameData(const OrbElemBase* right) const;

          /** Compare two OrbElemBase descendent objects.
           *  Any differences are summarized and written to the output stream
           *  provided.
           *  The base method compares the members common to the base.  The descendent classes
           *  should add additional member comparisions as needed in order   
           *  to provide a complete comparison of members of interest to be compared.   
           *  The return list is a list of the members that disagreed.  
           */
      virtual std::list<std::string> compare(const OrbElemBase* right) const;

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

      virtual Rinex3NavData makeRinex3NavData() const
         throw( InvalidRequest );

         /// Overhead information
         //@{
      bool    dataLoadedFlag;  /**< True if data is present, False otherwise */
      SatID   satID;	       /**< Define satellite system and specific SV */
      ObsID   obsID;           /**< Defines carrier and tracking code */
      CommonTime ctToe;        /**< Orbit epoch in commontime format */
      bool    healthy;         /**< SV health (healthy=true, other=false */
              //@}

         // Fit Interval Definition
         // The beginning and end of validity are derived quantities that specify
         // the bounds between which the data in OrbElemBase are valid.
	 // The manner in which these are derived varies by satellite system,
	 // but the bounds are required for proper operation of the navigation
	 // message storage classes. 

         //@{
      CommonTime beginValid;    /**< Time at beginning of validity */
      CommonTime endValid;      /**< Time at end of fit validity */



   }; // end class OrbElemBase

   //@}

} // end namespace

#endif // GPSTK_ORBELEMBASE_HPP
