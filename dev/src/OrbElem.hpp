#pragma ident "$Id: $"

/**
 * @file OrbElem.hpp
 *  This class encapsulates the "least common denominator"
 *  orbit parameters defined in the GPS signal interface specifications.
 *  That is to say, the clock correction coefficients, the pseudo-
 *  Keplerian orbit parameters, the harmonic perturbations, and
 *  the associated times.
 *
 *  Generally, the user will want to instantiate a descendent of this
 *  class as opposed to instantiating this class directly.  The
 *  descendent classes provide the functionality to load the
 *  coefficients from various navigation message formats
 *  and types.
 */

#ifndef GPSTK_ORBELEM_HPP
#define GPSTK_ORBELEM_HPP

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


#include "ObsID.hpp"
#include "Exception.hpp"
#include "CommonTime.hpp"
#include "Xvt.hpp"
#include "GNSSconstants.hpp"
#include "SatID.hpp"
#include "ObsID.hpp"
#include "MathBase.hpp"
#include "GPSWeekSecond.hpp"

namespace gpstk
{
   class OrbElem
   {
   public:
         /// Constructors
	      /// Default constuctor
      OrbElem( );


         /// Destructor
      virtual ~OrbElem() {}

         /**
          * Returns true if the time, ct, is within the period of validity of this OrbElem object.
          * @throw Invalid Request if the required data has not been stored.
          */ 
      bool isValid(const CommonTime& ct) const throw(InvalidRequest);

	 /**
          *   Return true if orbit data have been loaded.
          *   Returns false if the object has been instantiated,
          *   but no data have been loaded.
          */ 
      bool hasData( ) const;
      
         /** This function returns the health status of the SV.
          * @throw Invalid Request if the required data has not been stored.
          */
      bool isHealthy() const throw(gpstk::InvalidRequest);

         /** Compute the satellite clock bias (sec) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      double svClockBias(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** Compute the satellite clock bias (meters) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      double svClockBiasM(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** Compute the satellite clock drift (sec/sec) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      double svClockDrift(const CommonTime& t) const throw(gpstk::InvalidRequest);

      
         /** Compute satellite position at the given time
          * using this orbit data.
          * @throw Invalid Request if the required data has not been stored.
          */
      Xvt svXvt(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** Compute satellite relativity correction (sec) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      double svRelativity(const CommonTime& t) const throw( gpstk::InvalidRequest );
      
         /** Output the contents of this orbit data to the given stream. 
          * @throw Invalid Request if the required data has not been stored.
          */
      virtual void dump(std::ostream& s = std::cout) const 
	 throw( InvalidRequest );

         // Enumeration of descendents of OrbElem
      enum OrbElemType
      {
         OrbElemFIC9,
         OrbElemFIC109,
         OrbElemRinex,
         Unknown
      };
   
         /// Overhead information
         //@{
      bool    dataLoaded;     /**< True if data is present, False otherwise */
      SatID   satID;	      /**< Define satellite system and specific SV */
      ObsID   obsID;          /**< Defines carrier and tracking code */
      CommonTime ctToe;         /**< Orbit epoch */
      bool    healthy;        /**< SV health (healthy=true, other=false */
              //@}

	 /// Harmonic perturbations
         //@{
      double   Cuc;           /**< Cosine latitude (rad) */
      double   Cus;           /**< Sine latitude (rad) */
      double   Crc;           /**< Cosine radius (m) */
      double   Crs;           /**< Sine radius (m) */
      double   Cic;           /**< Cosine inclination (rad) */
      double   Cis;           /**< Sine inclination (rad) */
         //@}

         /// Major orbit parameters
         //@{
      double   M0;            /**< Mean anomaly (rad) */
      double   dn;            /**< Correction to mean motion (rad/sec) */
      double   dndot;	      /**< Rate of correction to mean motion (rad/sec/sec) */
      double   ecc;           /**< Eccentricity */
      double   A;             /**< Semi-major axis (m) */
      double   Adot;          /**< Rate of semi-major axis (m/sec) */ 
      double   OMEGA0;        /**< Rt ascension of ascending node (rad) */
      double   i0;            /**< Inclination (rad) */
      double   w;             /**< Argument of perigee (rad) */
      double   OMEGAdot;      /**< Rate of Rt ascension (rad/sec) */
      double   idot;          /**< Rate of inclination angle (rad/sec) */
         //@}
          
         /// Clock information
         //@{
      CommonTime ctToc;	    /**< Clock Epoch */
      double af0;           /**< SV clock error (sec) */
      double af1;           /**< SV clock drift (sec/sec) */
      double af2;           /**< SV clock drift rate (sec/sec**2) */
         //@}

         /// Fit Interval Definition
         //@{
      CommonTime beginValid;    /**< Time at beginning of validity */
      CommonTime endValid;      /**< Time at end of fit validity */
         
      friend std::ostream& operator<<(std::ostream& s, 
                                      const OrbElem& eph);
         // Type of this OrbElem object
      OrbElemType type;
  

   }; // class OrbElem

   //@}

} // namespace

#endif
