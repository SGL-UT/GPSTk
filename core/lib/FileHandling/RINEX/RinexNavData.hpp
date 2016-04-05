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
 * @file RinexNavData.hpp
 * Encapsulates RINEX Navigation data
 */

#ifndef RINEXNAVDATA_HPP
#define RINEXNAVDATA_HPP

#include <list>

#include "StringUtils.hpp"
#include "CommonTime.hpp"
#include "FFStream.hpp"
#include "RinexNavBase.hpp"
#include "EngEphemeris.hpp"
#include "GPSEphemeris.hpp"

namespace gpstk
{
      /// @ingroup FileHandling
      //@{

      /**
       * This class models a RINEX NAV record.
       *
       * \sa rinex_nav_test.cpp and rinex_nav_read_write.cpp for examples.
       * \sa gpstk::RinexNavHeader and gpstk::RinexNavStream classes.
       */
   class RinexNavData : public RinexNavBase
   {
   public:
         /**
          * Constructor
          * @warning CHECK THE PRNID TO SEE IF THIS DATA IS
          *  VALID BEFORE USING!!
          */
      RinexNavData();

         /// Initializes the nav data with an EngEphemeris
      RinexNavData(const EngEphemeris& ee);

         /// destructor
      virtual ~RinexNavData() {}

         // The next four lines is our common interface
         /// RinexNavData is "data" so this function always returns true.
      virtual bool isData() const {return true;}

         /**
          * Returns a string representation of the data in this record.
          * Interface stability: Committed
          */
      virtual std::string stableText() const;

         /**
          * A debug output function.
          * Prints the PRN id and the IODC for this record.
          * Interface stability: Volatile
          */
      virtual void dump(std::ostream& s) const;

         /**
          * Converts this RinexNavData to an EngEphemeris object.
          */
      operator EngEphemeris() const throw();

         /** Convert this RinexNavData to a GPSEphemeris object.  For
          * backward compatibility only - use Rinex3NavData. */
      operator GPSEphemeris() const;

         /** Round a given seconds of week down to the nearest
          * possible subframe 1 transmit time. */
      static long fixSF1xmitSOW(unsigned long sow)
      { return sow - (sow % 30); }

         /**
          * Get the clock reference time, which for RINEX NAV is
          * defined to be the epoch time of the record (RINEX 2.11
          * Table A4).
          */
      CommonTime getTocTime() const throw()
      { return time; }

         /**
          * Get the clock reference time, which for RINEX NAV is
          * defined to be the epoch time of the record (RINEX 2.11
          * Table A4).
          */
      GPSWeekSecond getTocWS() const
      { return time; }

         /**
          * Get the ephemeris reference time as a CommonTime object.
          */
      CommonTime getToeTime() const
      { return getToeWS(); }

         /**
          * Get the ephemeris reference time as a GPSWeekSecond object.
          */
      GPSWeekSecond getToeWS() const
      { return GPSWeekSecond(toeWeek, Toe, TimeSystem::GPS); }

         /**
          * Get the transmit time of subframe 1 as a CommonTime object.
          */
      CommonTime getXmitTime() const
      { return getXmitWS(); }

         /**
          * Get the transmit time of subframe 1 as a GPSWeekSecond object.
          */
      GPSWeekSecond getXmitWS() const;

         /**
          * Set the sf1XmitTime field using a seconds-of-week value.
          * The value may be a HOW time or perhaps even wildly
          * inaccurate.  This method will adjust the sf1XmitTime such
          * that it actually corresponds to the (rounded down) seconds
          * of week that would be correct for a subframe 1.  If the
          * specified value is a seconds of week that would already
          * correspond to a sf1 transmit time, it is unaltered.
          */
      RinexNavData& setXmitTime(unsigned long sow)
      {
         sf1XmitTime = fixSF1xmitSOW(sow);
         return *this;
      }

         /**
          * Set the transmit week.  Internally, sets the Toe week
          * using half-week tests based on the transmit time and toe.
          * @pre Toe and sf1XmitTime must be set.
          */
      RinexNavData& setXmitWeek(unsigned short fullweek);

         /**
          * Set the transmit time, including week and second of week.
          * @pre Toe must be set.
          */
      RinexNavData& setXmitTime(unsigned short fullweek, unsigned long sow);

         /**
          * Get the handover word time as a CommonTime object.
          */
      CommonTime getHOWTime() const
      { return getXmitTime() + 6; }

         /**
          * Get the handover word time as a GPSWeekSecond object.
          */
      GPSWeekSecond getHOWWS() const
      { return getHOWTime(); }

         /**
          * Converts the (non-CommonTime) data to a list for easy
          * comparison operators.
          */
      std::list<double> toList() const;

         /** @name Epoch data
          */
         //@{
      CommonTime time;        ///< Clock reference time (toc).
      short PRNID;            ///< SV PRN ID.
      long sf1XmitTime;       ///< Transmit time (seconds of week) of SF 1.
         // Toe is kept in separate fields rather than in a
         // GPSWeekSecond due partially to history but mostly due to
         // the fact that the two fields are in separate "broadcast
         // orbit" lines in the file.
      short toeWeek;          ///< The full GPS week associated with Toe.
      short codeflgs;         ///< L2 codes.
      double accuracy;        ///< SV accuracy (m).
      short health;           ///< SV health.
      short L2Pdata;          ///< L2 P data flag.
      double IODC;            ///< Index of data-clock.
      double IODE;            ///< Index of data-eph.
         //@}

         /** @name Clock Information
          */
         //@{
      double   af0;           ///< SV clock error (sec).
      double   af1;           ///< SV clock drift (sec/sec).
      double   af2;           ///< SV clock drift rate (sec/sec**2).
      double   Tgd;           ///< Group delay differential (sec).
         //@}

         /** @name Harmonic Perturbations
          */
         //@{
      double   Cuc;           ///< Cosine latitude (rad).
      double   Cus;           ///< Sine latitude (rad).
      double   Crc;           ///< Cosine radius (m).
      double   Crs;           ///< Sine radius (m).
      double   Cic;           ///< Cosine inclination (rad).
      double   Cis;           ///< Sine inclination (rad).
         //@}

         /**  @name Major Ephemeris Parameters
          */
         //@{
      double   Toe;           ///< Ephemeris epoch (sec of week).
      double   M0;            ///< Mean anomaly (rad).
      double   dn;            ///< Correction to mean motion (rad/sec).
      double   ecc;           ///< Eccentricity.
      double   Ahalf;         ///< SQRT of semi-major axis (m**1/2).
      double   OMEGA0;        ///< Rt ascension of ascending node (rad).
      double   i0;            ///< Inclination (rad).
      double   w;             ///< Argument of perigee (rad).
      double   OMEGAdot;      ///< Rate of Rt ascension (rad/sec).
      double   idot;          ///< Rate of inclination angle (rad/sec).
      double   fitint;        ///< Fit interval.
         //@}

   private:
         /// Writes the CommonTime object into RINEX format.
      std::string writeTime(const CommonTime& dt) const
         throw(gpstk::StringUtils::StringException);
         /// Parses string \a currentLine to obtain PRN id and epoch.
      void getPRNEpoch(const std::string& currentLine)
         throw(gpstk::StringUtils::StringException, FFStreamError);
         /// Reads line 1 of the Nav Data record
      void getBroadcastOrbit1(const std::string& currentLine)
         throw(gpstk::StringUtils::StringException, FFStreamError);
         /// Reads line 2 of the Nav Data record
      void getBroadcastOrbit2(const std::string& currentLine)
         throw(gpstk::StringUtils::StringException, FFStreamError);
         /// Reads line 3 of the Nav Data record
      void getBroadcastOrbit3(const std::string& currentLine)
         throw(gpstk::StringUtils::StringException, FFStreamError);
         /// Reads line 4 of the Nav Data record
      void getBroadcastOrbit4(const std::string& currentLine)
         throw(gpstk::StringUtils::StringException, FFStreamError);
         /// Reads line 5 of the Nav Data record
      void getBroadcastOrbit5(const std::string& currentLine)
         throw(gpstk::StringUtils::StringException, FFStreamError);
         /// Reads line 6 of the Nav Data record
      void getBroadcastOrbit6(const std::string& currentLine)
         throw(gpstk::StringUtils::StringException, FFStreamError);
         /// Reads line 7 of the Nav Data record
      void getBroadcastOrbit7(const std::string& currentLine)
         throw(gpstk::StringUtils::StringException, FFStreamError);

         /// generates a line to be output to a file for the PRN/epoch line
      std::string putPRNEpoch() const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 7 of the Nav Data record
      std::string putBroadcastOrbit1() const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 7 of the Nav Data record
      std::string putBroadcastOrbit2() const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 7 of the Nav Data record
      std::string putBroadcastOrbit3() const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 7 of the Nav Data record
      std::string putBroadcastOrbit4() const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 7 of the Nav Data record
      std::string putBroadcastOrbit5() const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 7 of the Nav Data record
      std::string putBroadcastOrbit6() const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 7 of the Nav Data record
         /// @warning Pass in version to decide wheter or not
         /// to write fit interval
      std::string putBroadcastOrbit7(const double ver) const
         throw(gpstk::StringUtils::StringException);

   protected:
         /// Outputs the record to the FFStream \a s.
      virtual void reallyPutRecord(FFStream& s) const
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);

         /**
          * This function retrieves a RINEX NAV record from the given FFStream.
          * If an error is encountered in reading from the stream, the stream
          * is returned to its original position and its fail-bit is set.
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s)
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);
   };  // class RinexNavData

      //@}

} // namespace

#endif
