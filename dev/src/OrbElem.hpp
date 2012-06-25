#pragma ident "$Id: $"

/**
 * @file OrbElem.hpp
 * Orbital element data (including clock corrections) broadcast by a GNSS in pseudo-Keplerian elements 
 * encapsulated in engineering terms
 * BrcKeplerOrbit is designed to address all the GNSS navigation message
 * formats that are based on pseudo-Keplerian elements. 
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
   /** @addtogroup ephemcalc */
   //@{

      /**
       * This class encapsulates the orbit parameters in any of several
       * navigation message formats (basically those that use 
       * Keplerian elements), provides functions to decode the
       * as-broadcast bit-encodings, and generate SV positions 
       * as a function of time.
       */
   class OrbElem
   {
   public:
         /// Constructors
	      /// Default constuctor
      OrbElem( ) throw();

      
         /// Add other constructors for other navigation message formats here....

         /// Destructor
      virtual ~OrbElem() {}

         /** 
          * Returns the epoch time (time of ephemeris) from this ephemeris, correcting
          * for half weeks and HOW time. */
      CommonTime getEphEpoch() const throw(InvalidRequest);
      
      CommonTime getClockEpoch() const throw(InvalidRequest);
         /** Returns the time at the beginning of the fit interval. */
      CommonTime getBeginningOfValidity() const throw(InvalidRequest);

         /** Returns the time at the end of the fit interval. */
      CommonTime getEndOfValidity() const throw(InvalidRequest);

      bool isValid(const CommonTime& ct) const throw(InvalidRequest);

	      /** Return true if orbit data has been loaded */
      bool hasData( ) const;

         /** Return satellite ID */
     SatID getSatID() const throw(gpstk::InvalidRequest);

     ObsID getObsID() const throw(gpstk::InvalidRequest);

         /** This function returns the health status of the SV. */
      bool isHealthy() const throw(gpstk::InvalidRequest);

         /** Compute the satellite clock bias (sec) at the given time
          * @throw InvalidRequest if a required data has not been stored.
          */
      double svClockBias(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** Compute the satellite clock bias (meters) at the given time
          * @throw InvalidRequest if a required data has not been stored.
          */
      double svClockBiasM(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** Compute the satellite clock drift (sec/sec) at the given time
          * @throw InvalidRequest if a required data has not been stored.
          */
      double svClockDrift(const CommonTime& t) const throw(gpstk::InvalidRequest);

      
         /** Compute satellite position at the given time
          * using this orbit data.
          * @throw InvalidRequest if a required data has not been stored.
          */
      Xvt svXvt(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** Compute satellite relativity correction (sec) at the given time
          * @throw InvalidRequest if a required data has not been stored.
          */
      double svRelativity(const CommonTime& t) const throw( gpstk::InvalidRequest );
      
         /** Output the contents of this orbit data to the given stream. */
      void dump(std::ostream& s = std::cout) const 
	 throw( InvalidRequest );

   
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
         //@}
         //Comparison methods
      bool operator==(const OrbElem& a);
      bool operator<(const OrbElem& a);
      friend std::ostream& operator<<(std::ostream& s, 
                                      const OrbElem& eph);

   }; // class OrbElem

   //@}

} // namespace

#endif
