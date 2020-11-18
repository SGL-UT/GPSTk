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
 * @file EngEphemeris.hpp
 * Ephemeris data encapsulated in engineering terms
 */

/**
 *   This is one of four classes designed to contain GPS navigation
 *   message data.  The classes are
 *
 *      EngEphemeris - Legacy GPS navigation message data from
 *      subframes 1,2,3 ( L1 C/A, L1 P(Y), L2 P(Y) )
 *
 *      CNAVEphemeris - GPS Civil navigation message data from Message
 *      Type 10/11 (L2C and L5)
 *
 *      CNAVClock - GPS Civil navigation message data from the "clock"
 *      portion of Message Types 30-37 (L2C and L5)
 *
 *      CNAV2EphClock - GPS Civil navigation message from subframe 2
 *      of the L1C message
 */

#ifndef GPSTK_EngEphemeris_HPP
#define GPSTK_EngEphemeris_HPP

#include <string>
#include <iostream>

#include "EngNav.hpp"
#include "Exception.hpp"
#include "CommonTime.hpp"
#include "Xvt.hpp"
#include "BrcKeplerOrbit.hpp"
#include "BrcClockCorrection.hpp"


namespace gpstk
{
      /// @ingroup GNSSEph
      //@{

      /**
       * Ephemeris information for a single satellite.  This class
       * encapsulates the ephemeris navigation message (subframes 1-3)
       * and provides functions to decode the as-broadcast
       * ephemerides.
       */
   class EngEphemeris : public EngNav
   {
   public:
         /// Default constructor
      EngEphemeris()
      throw();

         /// Destructor
      virtual ~EngEphemeris() {}

      bool operator==(const EngEphemeris& right) const throw();
      bool operator!=(const EngEphemeris& right) const throw()
      { return !(operator==(right)); }

         /**
          * Store a subframe in this object.
          * @param subframe ten word navigation subframe stored in the
          * 30 least-significant bits of each array index.
          * @param gpsWeek full GPS week number.
          * @param PRN PRN ID of source satellite.
          * @param track tracking number (typically receiver channel number).
          * @return true if successful.
          * @throw InvalidParameter if subframe is valid but not subframe 1-3.
          */
      bool addSubframe(const uint32_t subframe[10],
                       const int   gpsWeek,
                       const short PRN,
                       const short track);

         /**
          * Store a subframe in this object.  This method is provided in
          * order to allow construction of an EngEphemeris object for
          * cases where a receiver only provides the 24 msb of each
          * word and strips the parity.
          * @param subframe ten word navigation subframe (minus the six
          * parity bits) stored in the 24 least-significant bits of
          * each array index.
          * @param gpsWeek full GPS week number.
          * @param PRN PRN ID of source satellite.
          * @param track tracking number (typically receiver channel number).
          * @return true if successful.
          * @throw InvalidParameter if subframe is valid but not subframe 1-3.
          */
      bool addSubframeNoParity(const uint32_t subframe[10],
                               const int   gpsWeek,
                               const short PRN,
                               const short track);

         /**
          * Store a subframe in this object.  This method is provided in
          * order to provide a path to a EngEphemeris object for cases
          * where a receiver only provides words 3-10 of subframes 1-3.
          * @note: It is important to appropriately determine the transmit
          * time of the data.  The parameters sf1TransmitSOW and gpsWeek
          * should be consistent and should represent the time when the
          * data in sf1, sf2, and sf3 were received.  As a general rule,
          * the transmit time should be somewhere in the range of 0-2 hours
          * BEFORE the epoch time of the ephemeris.
          * @note: EngEphemeris objects built via this method will not
          * contain valid TLM message data, A-S flags, and "Alert" bits.
          * Methods getTLMMesage() and getASAlert() should not be used.
          * @param sf1 words 3-10 of navigation subframe1 stored in the
          * 30 least-significant bits of each array index.
          * @param sf2 words 3-10 of navigation subframe 2
          * @param sf3 words 3-10 of navigation subframe 3
          * @param sf1TransmitSOW time (in GPS seconds of week) corresponding
          * to leading edge of first bit of subframe 1
          * @param gpsWeek full GPS week number.
          * @param PRN PRN ID of source satellite.
          * @param track tracker number (typically receiver channel number).
          * @return true if successful.
          */
      bool addIncompleteSF1Thru3(const uint32_t  sf1[8],
                                 const uint32_t  sf2[8],
                                 const uint32_t  sf3[8],
                                 const long  sf1TransmitSOW,
                                 const int   gpsWeek,
                                 const short PRN,
                                 const short track);

         /**
          * Checks all quantities (present or not) to determine
          * whether they fall within the effective range as described
          * in the IS-GPS-200.
          * @return true if all values are within their effective range.
          */
      bool isValid() const throw();

         /**
          * Query presence of subframe in this object.
          * @param subframe subframe ID (1-3) to check.
          * @return true if the given subframe is present in this object.
          * @throw InvalidParameter if subframe is not a valid
          *   ephemeris subframe number.
          */
      bool isData(short subframe) const;

         /**
          * Return whether the ephemeris contains a complete data set,
          * i.e. whether the ephemeris contains subframes 1-3 with matching
          * IODC and IODE values.
          * @return true if the ephemeris is a complete data set,
          *         false if a data set cutover has occured
          */
      bool isDataSet() const;

         /**
          * Set the value of the SV accuracy (in meters).  This is the only
          * "set" method and is provided so that one can set the SV
          * accuracy directly and thereby override the conversion of the
          * SV accuracy flag to meters.  This is usually used when entering
          * data from some source other than an SV nav message.
          * @note Subsequent use of the addSubframe(), with subframe 1 info,
          * or the setSF1() methods will overwrite any value set using this
          * method.
          * @note This function converts the given accuracy to the appropriate
          * SV accuracy flag and stores it in the accFlag data member.
          * @param acc the new value of SV accuracy in meters.
          * @throw InvalidParameter if the given accuracy value is invalid.
          */
      void setAccuracy(double acc);

         /**
          * This computes and returns the fit interval for the
          * satellite ephemeris from the IODC and the fit interval
          * flag.  This fit interval is (typically) centered around the
          * Toe (time of ephemeris) for the ephemeris.  See IS-GPS-200
          * section 20.3.4.4, Table 20-XIA and section 20.3.3.4.3.1 for
          * more information.
          * @return the fit interval in hours (0 = failure).
          * @throw InvalidRequest if data is missing.
          */
      short getFitInterval() const;

         /**
          * Static version of the above.
          * @param iodc the IODC of the ephemeris.
          * @param fiti the fit interval flag of same ephemeris.
          * @return the fit interval in hours (0 = failure).
          * @throw InvalidRequest if data is missing.
          */
      static short getFitInterval(short iodc, short fiti);

         /** Compute satellite position & velocity at the given time
          * using this ephemeris.
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      Xvt svXvt(const CommonTime& t) const;


         /// Return 0x8b, the upper 5 bits of the 22-bit TLM word.
         // kinda pointless, huh?
      unsigned char getTLMPreamble() const
         throw()
      { return 0x8b; }

         /** Return the lower 16 bits of the TLM word for the given subframe.
          * @throw InvalidRequest
          */
      unsigned getTLMMessage(short subframe) const;

         /** Extracts the epoch time from this ephemeris, correcting
          * for half weeks and HOW time
          * @throw InvalidRequest
          */
      CommonTime getEphemerisEpoch() const;

         /** Extracts the epoch time (time of clock) from this
          * ephemeris, correcting for half weeks and HOW time
          * @throw InvalidRequest
          */
      CommonTime getEpochTime() const;

         /** Extracts the transmit time from the ephemeris using the Tot
          * @throw InvalidRequest
          */
      CommonTime getTransmitTime() const;

         /// used for template functions
      CommonTime getTimestamp() const
         throw()
      { return getEpochTime(); }

         /** This functions returns the GNSS type (satellite system code) */
      std::string getSatSys() const
         throw()
      { return satSys; }

         /** This function returns the PRN ID of the SV.
          * @throw InvalidRequest
          */
      short getPRNID() const;

         /** This function returns the tracker number.
          * @throw InvalidRequest
          */
      short getTracker() const;

         /** This function returns the time of the HOW in subframe
          * 1 or 2 or 3 in seconds of week.
          * @throw InvalidRequest
          */
      double getHOWTime(short subframe) const;

         /** This function returns the A-S alert flag for either
          * subframe 1 or 2 or 3.
          * @throw InvalidRequest
          */
      short getASAlert(short subframe) const;

         /** This function returns the GPS week number contained in
          * subframe 1.  this is the full GPS week (ie > 10 bits).
          * @throw InvalidRequest
          */
      short getFullWeek() const;

         /** This function returns the values of the L2 codes.
          * @throw InvalidRequest
          */
      short getCodeFlags() const;

         /** This function returns the value of the SV accuracy (m)
          * computed from the accuracy flag in the nav message, or
          * as set by the setAccuracy() method.
          * @throw InvalidRequest
          */
      double getAccuracy() const;

         /** This function returns the flag based on the SV accuracy
          * flag as it appears in the nav message.
          * @throw InvalidRequest
          */
      short getAccFlag() const;

         /** This function returns the value of the SV health flag.
          * @throw InvalidRequest
          */
      short getHealth() const;

         /** This function returns the value of the L2 P-code data
          * flag.
          * @throw InvalidRequest
          */
      short getL2Pdata() const;

         /** This function returns the IODC for the given PRN.
          * @throw InvalidRequest
          */
      short getIODC() const;

         /** This function return the IODE for the ephemeris.
          * @throw InvalidRequest
          */
      short getIODE() const;

         /** This function return the AODO for the ephemeris.
          * @throw InvalidRequest
          */
      long getAODO() const;

         /** This function returns the clock epoch in GPS seconds of
          * week.
          * @throw InvalidRequest
          */
      double getToc() const;

         /** This function returns the SV clock error in seconds.
          * @throw InvalidRequest
          */
      double getAf0() const;

         /** This function returns the SV clock drift in
          * seconds/seconds.
          * @throw InvalidRequest
          */
      double getAf1() const;

         /** This function returns the SV clock rate of change of the
          * drift in seconds/(seconds*seconds).
          * @throw InvalidRequest
          */
      double getAf2() const;

         /** This function returns the value of the group delay
          * differential in seconds.
          * @throw InvalidRequest
          */
      double getTgd() const;

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

         /** This function returns the value of the time of ephemeris
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

         /** This function returns the value of the eccentricity.
          * @throw InvalidRequest
          */
      double getEcc() const;

         /** This function returns the value of the square root of the
          * semi-major axis in square root of meters.
          * @throw InvalidRequest
          */
      double getAhalf() const;

         /** This function returns the value of the semi-major axis in
          * meters.
          * @throw InvalidRequest
          */
      double getA() const;

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

         /** Compute satellite relativity correction (sec) at the given time
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      double svRelativity(const CommonTime& t) const;

         /** Compute the satellite clock bias (sec) at the given time
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      double svClockBias(const CommonTime& t) const;

         /** Compute the satellite clock drift (sec/sec) at the given time
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      double svClockDrift(const CommonTime& t) const;

         /** This function returns the value of the fit interval
          * flag.
          * @throw InvalidRequest
          */
      short getFitInt() const;

         /** This function returns the value of the ephemeris key that
          * is used to sort the ephemerides when they are stored in
          * the bcetable. */
         /** @todo Determine if this function is needed, as it is never used */
         //double getEphkey() const

         /** This function returnst the value of the Time of Transmit.
          * Basically just the earliest of the HOWs.
          * @throw InvalidRequest
          */
      long getTot() const;

         /**
          * @throw InvalidRequest
          */
      BrcKeplerOrbit getOrbit() const;

         /**
          * @throw InvalidRequest
          */
      BrcClockCorrection getClock() const;

         /** Set the values contained in SubFrame 1,2 and 3.
          *
          * Values contained in SubFrame 1.
          * @param tlm the new value for the TLM word
          * @param how the new value for the HOW
          * @param asalert the new value for the A-S alert flag
          * @param fullweek the new value for the full GPS week
          * @param cflags the nve values for the L2 code flags
          * @param acc the new value for the SV accuracy flag
          * @param svhealth the new value for the SV health
          * @param iodc the new value for the IODC
          * @param l2pdata the new value for the L2 P-code data flag
          * @param tgd the new value for the Tgd
          * @param toc the new value for the clock epoch
          * @param Af2 the new value for Af2
          * @param Af1 the new value for Af1
          * @param Af0 the new value for Af0
          * @param Tracker the new value of the tracker number
          * @param prn the new value of the PRN ID
          * @return a reference to the modified EngEphemeris object
          *
          * Values contained in SubFrame 2.
          * @param tlm the new value for the TLM word
          * @param how the new value for the HOW
          * @param asalert the new falue for the A-S alert flag
          * @param iode the new value for the IODE
          * @param crs the new value for the Crs
          * @param m0 the new value for M0
          * @param Dn the new value for Dn
          * @param cuc the new value for the Cuc
          * @param Ecc the new value for the Ecc
          * @param cus the new value for the Cus
          * @param ahalf the new value for Ahalf
          * @param toe the new value for the Toe
          * @param fitInt the new value for the fit interval flag
          * @return a reference to the modified EngEphemeris object
          *
          * Values contained in SubFrame 3.
          * @param tlm the new value for the TLM word
          * @param how the new value for the HOW
          * @param asalert the new falue for the A-S alert flag
          * @param cic the new value for the Cic
          * @param Omega0 the enw value for Omega0
          * @param cis the new value for the Cis
          * @param I0 the new value for I0
          * @param crc the new value for the Crc
          * @param W the new value for W
          * @param OmegaDot the new value for OmegaDot
          * @param IDot the new value for IDot
          * @return a reference to the modified EngEphemeris object
          * @throw InvalidRequest
          */

      EngEphemeris& loadData( const std::string satSysArg,
                              const unsigned short tlm[3], const long how[3],
                              const short asalert[3],
                              const short Tracker, const short prn,
                              const short fullweek, const short cflags,
                              const short acc, const short svhealth,
                              const short iodc, const short l2pdata,
                              const long Aodo, const double tgd,
                              const double toc, const double Af2,
                              const double Af1, const double Af0,
                              const short iode, const double crs,
                              const double Dn, const double m0,
                              const double cuc, const double Ecc,
                              const double cus, const double ahalf,
                              const double toe, const short fitInt,
                              const double cic, const double Omega0,
                              const double cis, const double I0,
                              const double crc, const double W,
                              const double OmegaDot, const double IDot );

      EngEphemeris& setSF1( unsigned tlm, double how, short asalert,
                            short fullweek, short cflags, short acc,
                            short svhealth, short iodc,
                            short l2pdata, double tgd, double toc, double Af2,
                            double Af1, double Af0, short Tracker, short prn );

      EngEphemeris& setSF2( unsigned tlm, double how, short asalert,
                            short iode, double crs, double Dn, double m0,
                            double cuc, double Ecc, double cus, double ahalf,
                            double toe, short fitInt );

      EngEphemeris& setSF3( unsigned tlm, double how, short asalert,
                            double cic, double Omega0, double cis, double I0,
                            double crc, double W, double OmegaDot, double IDot);

         /** Output the contents of this ephemeris to the given stream.
          * @throw InvalidRequest
          */
      void dump(std::ostream& s = std::cout) const;

      void setFIC(const bool arg);

         /**
          * @throw InvalidRequest
          */
      void dumpTerse(std::ostream& s = std::cout) const;


      bool haveSubframe[3];/**< flags indicating presence of a subframe */

      bool unifiedConvert( const int gpsWeek,
                           const short PRN,
                           const short track);
      uint32_t subframeStore[3][10];
         // True if initialized with FIC data
      bool isFIC;

         /// @name Ephemeris overhead information
         //@{
      unsigned short tlm_message[3];
      std::string satSys;  /**< GNSS (satellite system) */
      short PRNID;         /**< SV PRN ID */
      short tracker;       /**< Tracker number */
      long HOWtime[3];     /**< Time of subframe 1-3 (sec of week) */
      short ASalert[3];    /**< A-S and "alert" flags for each
                            * subframe. 2 bit quantity with alert flag
                            * the high order bit and the A-S flag
                            * low order bit */
      short weeknum;       /**< GPS full week number that corresponds
                            * to the HOWtime of SF1 */
      short codeflags;     /**< L2 codes */
      short health;        /**< SV health */
      short L2Pdata;       /**< L2 P data flag */
      short IODC;          /**< Index of data-clock  */
      short IODE;          /**< Index of data-eph    */
      long  AODO;          /**< Age of Data Offset for NMCT */
      short  fitint;       /**< Fit interval flag */
      double  Tgd;         /**< L1 and L2 correction term */
         //@}

         // The following is an odd, special case.
      short accFlagTmp;    // Accuracy flag (URA index).  NOTE:  We should
                           // use orbit.getURAoe( ) for this value.  However
                           // we had to have someplace to temporarily store
                           // this value between setSF1( ) and setSF2( ).
                           // Frankly, are the ONLY two methods that should
                           // make use of this member. Its status is NOT
                           // GUARANTEED outside the time setSF1( ) loads it
                           // and setSF2( ) uses it.

         /// Clock information
      BrcClockCorrection bcClock;

         /// Orbit parameters
      BrcKeplerOrbit orbit;

      friend std::ostream& operator<<(std::ostream& s,
                                      const EngEphemeris& eph);

   }; // class EngEphemeris

      //@}

} // namespace

#endif
