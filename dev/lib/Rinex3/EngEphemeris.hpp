#pragma ident "$Id: EngEphemeris.hpp 876 2007-11-06 14:16:30Z renfroba $"

/**
 * @file EngEphemeris.hpp
 * Ephemeris data encapsulated in engineering terms
 */

#ifndef GPSTK_ENGEPHEMERIS_HPP
#define GPSTK_ENGEPHEMERIS_HPP

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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#include "EngNav.hpp"
#include "Exception.hpp"
#include "CommonTime.hpp"
#include "Xvt.hpp"

using namespace gpstk;

namespace Rinex3
{
   /** @addtogroup ephemcalc */
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
      EngEphemeris() throw();

         /// Destructor
      virtual ~EngEphemeris() {}

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
      bool addSubframe(const long subframe[10], const int gpsWeek,
                       short PRN, short track)
         throw(gpstk::InvalidParameter);
       
         /**
          * Store a subframe in this object.  This method is provided in 
          * order to allow construction of an EngEphemeris object for
          * cases where a receiver only provides the 24 msb of each
          * word and strips the parity.l  
          * @param subframe ten word navigation subframe (minus the six
          * parity bits) stored in the 24 least-significant bits of
          * each array index.
          * @param gpsWeek full GPS week number.
          * @param PRN PRN ID of source satellite.
          * @param track tracking number (typically receiver channel number).
          * @return true if successful.
          * @throw InvalidParameter if subframe is valid but not subframe 1-3.
          */
      bool addSubframeNoParity(const long subframe[10],
                               const long gpsWeek,
                               const short PRN,
                               const short track)
         throw(gpstk::InvalidParameter);

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
      bool addIncompleteSF1Thru3(
         const long sf1[8], const long sf2[8], const long sf3[8], 
         const long sf1TransmitSOW, const int gpsWeek,
         const short PRN, const short track);

         /**
          * Query presence of subframe in this object.
          * @param subframe subframe ID (1-3) to check.
          * @return true if the given subframe is present in this object.
          * @throw InvalidParameter if subframe is not a valid
          *   ephemeris subframe number.
          */
      bool isData(short subframe) const throw(gpstk::InvalidRequest);

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
      void setAccuracy(const double& acc) throw(gpstk::InvalidParameter);
      
         /**
          * This computes and returns the fit interval for the
          * satellite ephemeris from the IODC and the fit interval
          * flag.  This fit interval is centered around the Toe (time
          * of ephemeris) for the ephemeris.  See Table 20-XIA in the
          * ICD-GPS-200 and section 20.3.3.4.3.1 for more information.
          * @return the fit interval in hours (0 = failure).
          * @throw InvalidRequest if data is missing.
          */
      short getFitInterval() const throw(gpstk::InvalidRequest);

         /// Return 0x8b, the upper 5 bits of the 22-bit TLM word.
         // kinda pointless, huh?
      unsigned char getTLMPreamble() const throw() { return 0x8b; }

         /// Return the lower 16 bits of the TLM word for the given subframe.
      unsigned getTLMMessage(short subframe) const 
         throw(gpstk::InvalidRequest);
            
         /// Extracts the epoch time from this ephemeris, correcting
         /// for half weeks and HOW time
      CommonTime getEphemerisEpoch() const throw(InvalidRequest);

         /// Extracts the epoch time (time of clock) from this ephemeris, correcting
         /// for half weeks and HOW time
      CommonTime getEpochTime() const throw(gpstk::InvalidRequest);

         /// Extracts the transmit time from the ephemeris using the Tot
      CommonTime getTransmitTime() const throw(gpstk::InvalidRequest);
      
         /// used for template functions
      CommonTime getTimestamp() const throw(gpstk::InvalidRequest)
         { return getEpochTime(); }
     
         /** This function returns the PRN ID of the SV. */
      short getPRNID() const throw(gpstk::InvalidRequest);
      
         /** This function returns the tracker number. */
      short getTracker() const throw(gpstk::InvalidRequest);

         /** This function returns the time of the HOW in subframe
          * 1 or 2 or 3 in seconds of week. */
      double getHOWTime(short subframe) const throw(gpstk::InvalidRequest);
      
         /** This function returns the A-S alert flag for either
          * subframe 1 or 2 or 3. */
      short getASAlert(short subframe) const throw(gpstk::InvalidRequest);
      
         /** This function return the GPS week number for the
          * ephemeris.  this is the full GPS week (ie > 10 bits). */
      short getFullWeek() const throw(gpstk::InvalidRequest);
      
         /** This function returns the values of the L2 codes. */
      short getCodeFlags() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the SV accuracy (m)
          * computed from the accuracy flag in the nav message, or
          * as set by the setAccuracy() method. */
      double getAccuracy() const throw(gpstk::InvalidRequest);

         /** This function returns the flag based on the SV accuracy
          * flag as it appears in the nav message. */
      short getAccFlag() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the SV health flag. */
      short getHealth() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the L2 P-code data
          * flag. */
      short getL2Pdata() const throw(gpstk::InvalidRequest);
      
         /** This function returns the IODC for the given PRN. */
      short getIODC() const throw(gpstk::InvalidRequest);
      
         /** This function return the IODE for the ephemeris. */
      short getIODE() const throw(gpstk::InvalidRequest);
      
         /** This function return the AODO for the ephemeris. */
      long getAODO() const throw(gpstk::InvalidRequest);
      
         /** This function returns the clock epoch in GPS seconds of
          * week. */
      double getToc() const throw(gpstk::InvalidRequest);
      
         /** This function returns the SV clock error in seconds. */
      double getAf0() const throw(gpstk::InvalidRequest);
      
         /** This function returns the SV clock drift in
          * seconds/seconds. */
      double getAf1() const throw(gpstk::InvalidRequest);
      
         /** This function returns the SV clock rate of change of the
          * drift in seconds/(seconds*seconds). */
      double getAf2() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the group delay
          * differential in seconds. */
      double getTgd() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the sine latitude
          * harmonic perturbation in radians. */
      double getCus() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the sine radius
          * harmonic perturbation in meters. */
      double getCrs() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the sine inclination
          * harmonic perturbation in radians. */
      double getCis() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the cosine radius
          * harmonic perturbation in meters. */
      double getCrc() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the cosine latitude
          * harmonic perturbation in radians. */
      double getCuc() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the cosine inclination
          * harmonic perturbation in radians. */
      double getCic() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the time of ephemeris
          * in GPS seconds of week. */
      double getToe() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the mean anomaly in
          * radians. */
      double getM0() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the correction to the
          * mean motion in radians/second. */
      double getDn() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the eccentricity. */
      double getEcc() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the square root of the
          * semi-major axis in square root of meters. */
      double getAhalf() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the semi-major axis in
          * meters. */
      double getA() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the right ascension of
          * the ascending node in radians. */
      double getOmega0() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the inclination in
          * radians. */
      double getI0() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the argument of
          * perigee in radians. */
      double getW() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the rate of the right
          * ascension of the ascending node in radians/second. */
      double getOmegaDot() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the rate of the
          * inclination in radians/second. */
      double getIDot() const throw(gpstk::InvalidRequest);
      
         /** Compute satellite velocity/position at the given time
          * using this ephemeris.
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      Xvt svXvt(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** Compute satellite relativity correction (sec) at the given time
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      double svRelativity(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** Compute the satellite clock bias (sec) at the given time
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      double svClockBias(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** Compute the satellite clock drift (sec/sec) at the given time
          * @throw InvalidRequest if a required subframe has not been stored.
          */
      double svClockDrift(const CommonTime& t) const throw(gpstk::InvalidRequest);

         /** This function returns the value of the fit interval
          * flag. */
      short getFitInt() const throw(gpstk::InvalidRequest);
      
         /** This function returns the value of the ephemeris key that
          * is used to sort the ephemerides when they are stored in
          * the bcetable. */
      double getEphkey() const throw(gpstk::InvalidRequest);

         /** This function returnst the value of the Time of Transmit.
          * Basically just the earliest of the HOWs. */
      long getTot() const throw(gpstk::InvalidRequest);
      
         /** Set the values contained in SubFrame 1.
          * @param tlm the new value for the TLM word
          * @param how the new value for the HOW
          * @param asalert the new falue for the A-S alert flag
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
          */
      EngEphemeris& setSF1( unsigned tlm, double how, short asalert, short fullweek,
                            short cflags, short acc, short svhealth, short iodc,
                            short l2pdata, double tgd, double toc, double Af2,
                            double Af1, double Af0, short Tracker, short prn )
         throw();

      
         /** Set the values contained in SubFrame 2.
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
          */
      EngEphemeris& setSF2( unsigned tlm, double how, short asalert, short iode,
                            double crs, double Dn, double m0, double cuc, double Ecc,
                            double cus, double ahalf, double toe, short fitInt )
         throw();
      
         /** Set the values contained in SubFrame 2.
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
          */
      EngEphemeris& setSF3( unsigned tlm, double how, short asalert, double cic,
                            double Omega0, double cis, double I0, double crc,
                            double W, double OmegaDot, double IDot )
         throw();
      
         /// Output the contents of this ephemeris to the given stream.
      void dump(std::ostream& s = std::cout) const;

   protected:
      bool haveSubframe[3];/**< flags indicating presence of a subframe */

         /// Ephemeris overhead information
         //@{
      unsigned short tlm_message[3];
      short PRNID;         /**< SV PRN ID */
      short tracker;       /**< Tracker number */
      long HOWtime[3];     /**< Time of subframe 1-3 (sec of week) */
      short ASalert[3];    /**< A-S and "alert" flags for each subframe */
      short weeknum;       /**< GPS full week number that corresponds to the HOWtime of SF1 */
      short codeflags;     /**< L2 codes */
      double accuracy;      /**< SV accuracy (m)*/
      short accFlag;       /**< User Range Accuracy (URA) the accuracy flag */
      short health;        /**< SV health */
      short L2Pdata;       /**< L2 P data flag */
      short IODC;         /**< Index of data-clock  */
      short IODE;         /**< Index of data-eph    */
      long  AODO;         /**< Age of Data Offset for NMCT */
         //@}
      
         /// Clock information
         //@{
      double   Toc;           /**< Clock epoch (sec of week) */
      double   af0;           /**< SV clock error (sec) */
      double   af1;           /**< SV clock drift (sec/sec) */
      double   af2;           /**< SV clock drift rate (sec/sec**2) */
      double   Tgd;           /**< Group delay differential (sec) */
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

         /// Major ephemeris parameters
         //@{
      double   Toe;           /**< Ephemeris epoch (sec of week) */
      double   M0;            /**< Mean anomaly (rad) */
      double   dn;            /**< Correction to mean motion (rad/sec) */
      double   ecc;           /**< Eccentricity */
      double   Ahalf;         /**< SQRT of semi-major axis (m**1/2) */
      double   OMEGA0;        /**< Rt ascension of ascending node (rad) */
      double   i0;            /**< Inclination (rad) */
      double   w;             /**< Argument of perigee (rad) */
      double   OMEGAdot;      /**< Rate of Rt ascension (rad/sec) */
      double   idot;          /**< Rate of inclination angle (rad/sec) */
      short fitint;           /**< Fit interval flag */
         //@}

      friend std::ostream& operator<<(std::ostream& s, 
                                      const EngEphemeris& eph);

   }; // class EngEphemeris

   //@}

} // namespace

#endif
