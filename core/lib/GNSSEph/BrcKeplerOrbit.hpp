//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file BrcKeplerOrbit.hpp
 * Orbit data broadcast by a GNSS in pseudo-Keplerian elements 
 * encapsulated in engineering terms
 * BrcKeplerOrbit is designed to address all the GNSS navigation message
 * formats that are based on pseudo-Keplerian elements. 
 */

#ifndef GPSTK_BRCKEPLERORBIT_HPP
#define GPSTK_BRCKEPLERORBIT_HPP

#include <string>
#include "Xvt.hpp"
#include "ObsID.hpp"
#include "EngNav.hpp"
#include "Exception.hpp"
#include "CommonTime.hpp"
#include "StringUtils.hpp"
#include "GNSSconstants.hpp"
#include "GPSEllipsoid.hpp"
#include "MathBase.hpp"
#include "TimeSystem.hpp"
#include "GPSWeekSecond.hpp"
#include "YDSTime.hpp"
#include "CivilTime.hpp"
#include "GPS_URA.hpp"

class BrcKeplerOrbit_T;

namespace gpstk
{
      /// @ingroup GNSSEph
      //@{

      /**
       * Orbit information for a single satellite.  This class
       * encapsulates the orbit parameters in any of several
       * navigation message formats (basically those that use 
       * Keplerian elements), provides functions to decode the
       * as-broadcast bit-encodings, and generate SV positions 
       * as a function of time.
       */
   class BrcKeplerOrbit : public EngNav
   {
   public:
         /// Constructors
         /// Default constuctor
      BrcKeplerOrbit( ) throw();

         /**
          * All constructors and loadData methods assume weeknumArg
          * is the full GPS week number associated with the epoch
          * time.
          */

         /// General purpose constructor
      BrcKeplerOrbit( const std::string satSysArg, const ObsID obsIDArg, 
                      const short PRNIDArg, const CommonTime beginFitArg, 
                      const CommonTime endFitArg, const CommonTime ToeArg, 
                      const short URAoeArg, 
                      const bool healthyArg, const double CucArg, 
                      const double CusArg, const double CrcArg, 
                      const double CrsArg, const double CicArg, 
                      const double CisArg, const double M0Arg, 
                      const double dnArg, const double dndotArg,
                      const double eccArg, const double AArg, 
                      const double AhalfArg, const double AdotArg,
                      const double OMEGA0Arg, const double i0Arg, 
                      const double wArg, const double OMEGAdotARg, 
                      const double idotArg );

         /// Legacy GPS Subframe 1-3  
      BrcKeplerOrbit( const ObsID obsIDArg,  
                      const short PRNID,
                      const short fullweeknum,
                      const long subframe1[10],
                      const long subframe2[10],
                      const long subframe3[10] );

         // Add other constructors for other navigation message formats here...

         /// Destructor
      virtual ~BrcKeplerOrbit() {}

      bool operator==(const BrcKeplerOrbit& right) const throw();
      bool operator!=(const BrcKeplerOrbit& right) const throw()
      { return !(operator==(right)); }

         /// General purpose means to load data into object
      void loadData(const std::string satSysArg, const ObsID obsIDArg,
                    const short PRNIDArg, const CommonTime beginFitArg,
                    const CommonTime endFitArg, const CommonTime ToeArg,
                    const short URAoeArg, const bool healthyArg,
                    const double CucArg,
                    const double CusArg, const double CrcArg,
                    const double CrsArg, const double CicArg,
                    const double CisArg, const double M0Arg,
                    const double dnArg, const double dndotArg,
                    const double eccArg, const double AArg,
                    const double AhalfArg, const double AdotArg,
                    const double OMEGA0Arg, const double i0Arg,
                    const double wArg, const double OMEGAdotARg,
                    const double idotArg );

         /** Load data based on the GPS Legacy message
          * @throw InvalidParameter
          */
      void loadData( const ObsID obsIDArg, 
                     const short PRNID, 
                     const short fullweeknum,
                     const long subframe1[10],
                     const long subframe2[10],
                     const long subframe3[10] );

         /** 
          * Returns the epoch time (time of ephemeris) from this
          * ephemeris, correcting for half weeks and HOW time.
          * @throw InvalidRequest
          */
      CommonTime getOrbitEpoch() const;

         /** Returns the time at the beginning of the fit interval.
          * @throw InvalidRequest
          */
      CommonTime getBeginningOfFitInterval() const;

         /** Returns the time at the end of the fit interval.
          * @throw InvalidRequest
          */
      CommonTime getEndOfFitInterval() const;

         /** Return true if orbit data has been loaded */
      bool hasData( ) const;

         /** Return satellite system ID */
         //@note Determine if this function is needed, as it is never used
         //std::string getSatSystem() const;

         /** Return signal type associated with this orbit */
         //@note Determine if this function is needed, as it is never used
         //std::string getSignal() const;

         /** This function returns the PRN ID of the SV.
          * @throw InvalidRequest
          */
      short getPRNID() const;

         /** This function returns the OBS ID of the orbit.
          * @throw InvalidRequest
          */
      ObsID getObsID() const;

         /** This function returns the health status of the SV.
          * @throw InvalidRequest
          */
      bool isHealthy() const;

         /** Return true if fit interval is valid.
          * @throw InvalidRequest
          */
      bool withinFitInterval(const CommonTime) const;

         /** This function return the GPS week number for the
          * orbit.  this is the full GPS week (ie > 10 bits).
          * @throw InvalidRequest
          */
      short getFullWeek() const;
      
         /** This function returns the value of the SV accuracy (m)
          * computed from the accuracy information contained in the
          * nav message
          * @throw InvalidRequest
          */
      double getAccuracy() const;


         /**
          * @throw InvalidRequest
          */
      void setAccuracy(const double& acc);
   
         /**
          * @throw InvalidRequest
          */
      short getURAoe() const;

         /** This function returns the value of the sine latitude
          * harmonic perturbation in radians.
          * @throw InvalidRequest
          */
      double getCus() const;
      
         /** This function returns the value of the sine radius
          * harmonic perturbation in meters.
          * @throw InvalidRequest
          */
      double getCrs() const;
      
         /** This function returns the value of the sine inclination
          * harmonic perturbation in radians.
          * @throw InvalidRequest
          */
      double getCis() const;
      
         /** This function returns the value of the cosine radius
          * harmonic perturbation in meters.
          * @throw InvalidRequest
          */
      double getCrc() const;
      
         /** This function returns the value of the cosine latitude
          * harmonic perturbation in radians.
          * @throw InvalidRequest
          */
      double getCuc() const;
      
         /** This function returns the value of the cosine inclination
          * harmonic perturbation in radians.
          * @throw InvalidRequest
          */
      double getCic() const;
      
         /** This function returns the value of the time of orbit
          * in GPS seconds of week.
          * @throw InvalidRequest
          */
      double getToe() const;
      
         /** This function returns the value of the mean anomaly in
          * radians.
          * @throw InvalidRequest
          */
      double getM0() const;
      
         /** This function returns the value of the correction to the
          * mean motion in radians/second.
          * @throw InvalidRequest
          */
      double getDn() const;

         /** This function returns the value of the rate correction to the
          * mean motion in radians/second**2.
          * @throw InvalidRequest
          */
      double getDnDot() const;
      
         /** This function returns the value of the eccentricity.
          * @throw InvalidRequest
          */
      double getEcc() const;
      
         /** This function returns the value of the 
          * semi-major axis in meters.
          * @throw InvalidRequest
          */
      double getA() const;
      
         /** This function returns the value of the 
          * square root of the semi-major axis in meters**.5.
          * @throw InvalidRequest
          */
      double getAhalf() const;

         /** This function returns the value of the rate of the
          * semi-major axis in meters/sec.
          * @throw InvalidRequest
          */
      double getAdot() const;
   
         /** This function returns the value of the right ascension of
          * the ascending node in radians.
          * @throw InvalidRequest
          */
      double getOmega0() const;
      
         /** This function returns the value of the inclination in
          * radians.
          * @throw InvalidRequest
          */
      double getI0() const;
      
         /** This function returns the value of the argument of
          * perigee in radians.
          * @throw InvalidRequest
          */
      double getW() const;
      
         /** This function returns the value of the rate of the right
          * ascension of the ascending node in radians/second.
          * @throw InvalidRequest
          */
      double getOmegaDot() const;
      
         /** This function returns the value of the rate of the
          * inclination in radians/second.
          * @throw InvalidRequest
          */
      double getIDot() const;
      
         /** Compute satellite position at the given time
          * using this orbit data.
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      Xvt svXvt(const CommonTime& t) const;

         /** Compute satellite relativity correction (sec) at the given time
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      double svRelativity(const CommonTime& t) const;
      
         /** Output the contents of this orbit data to the given stream. */
      void dump(std::ostream& s = std::cout) const throw();

   protected:
         /// @name Overhead information
         //@{
      bool    dataLoaded;     /**< True if data is present, False otherwise */
      std::string satSys;     /**< Satellite system ID (as per Rinex) */
      ObsID   obsID;          /**< Defines carrier and tracking code */
      short   PRNID;          /**< SV PRN ID */
      CommonTime Toe;         /**< Orbit epoch */
      short   URAoe;          /**< SV accuracy Index */
      bool    healthy;        /**< SV health (healthy=true, other=false */
         //@}

	 /// @name Harmonic perturbations
         //@{
      double   Cuc;           /**< Cosine latitude (rad) */
      double   Cus;           /**< Sine latitude (rad) */
      double   Crc;           /**< Cosine radius (m) */
      double   Crs;           /**< Sine radius (m) */
      double   Cic;           /**< Cosine inclination (rad) */
      double   Cis;           /**< Sine inclination (rad) */
         //@}

         /// @name Major orbit parameters
         //@{
      double   M0;            /**< Mean anomaly (rad) */
      double   dn;            /**< Correction to mean motion (rad/sec) */
      double   dndot;	      /**< Rate of correction to mean motion (rad/sec/sec) */
      double   ecc;           /**< Eccentricity */
      double   A;             /**< Semi-major axis (m) */
      double   Ahalf;         /**< Square Root of semi-major axis (m**.5) */
      double   Adot;          /**< Rate of semi-major axis (m/sec) */ 
      double   OMEGA0;        /**< Rt ascension of ascending node (rad) */
      double   i0;            /**< Inclination (rad) */
      double   w;             /**< Argument of perigee (rad) */
      double   OMEGAdot;      /**< Rate of Rt ascension (rad/sec) */
      double   idot;          /**< Rate of inclination angle (rad/sec) */
         //@}

         /// @name Fit Interval Definition
         //@{
      CommonTime beginFit;    /**< Time at beginning of fit interval */
      CommonTime endFit;      /**< Time at end of fit interval */
         //@}

      friend class ::BrcKeplerOrbit_T;
      friend std::ostream& operator<<(std::ostream& s, 
                                      const BrcKeplerOrbit& eph);

   }; // class BrcKeplerOrbit

      //@}

} // namespace

#endif
