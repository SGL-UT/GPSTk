#pragma ident "$Id$"


/**
 * @file Rinex3NavData.hpp
 * Encapsulates RINEX 3 Navigation data
 */

#ifndef RINEXNAVDATA_HPP
#define RINEXNAVDATA_HPP

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


#include <list>

#include "StringUtils.hpp"
#include "CommonTime.hpp"
#include "FFStream.hpp"
#include "Rinex3NavBase.hpp"
#include "EngEphemeris.hpp"

namespace gpstk
{
   /** @addtogroup Rinex3Nav */
   //@{

      /**
       * This class models a RINEX 3 NAV record.
       *
       * \sa rinex_nav_test.cpp and rinex_nav_read_write.cpp for examples.
       * \sa gpstk::Rinex3NavHeader and gpstk::Rinex3NavStream classes.
       */

   class Rinex3NavData : public Rinex3NavBase
   {
   public:

         /**
          * Constructor
          * @warning CHECK THE PRNID TO SEE IF THIS DATA IS VALID BEFORE USING!!
          */
      Rinex3NavData(void)
        : time(gpstk::CommonTime::BEGINNING_OF_TIME), PRNID(-1), fitint(4)
      {}

         /// Initializes the nav data with an EngEphemeris
      Rinex3NavData(const EngEphemeris& ee);

         /// destructor
      virtual ~Rinex3NavData() {}

         // The next four lines are our common interface.
         /// Rinex3NavData is "data" so this function always returns true.
      virtual bool isData(void) const {return true;}

         /**
          * A debug output function.
          * Prints the PRN id and the IODC for this record.
          */ 
      virtual void dump(std::ostream& s) const;

         /**
          * Converts this Rinex3NavData to an EngEphemeris object.
          */
      operator EngEphemeris() const throw();

         /**
          * Converts the (non-CommonTime) data to an easy list for comparison operators.
          */
      std::list<double> toList() const;

         /** @name EpochDataGeneral
          */
         //@{
      CommonTime time;     ///< Time according to the record
      std::string satSys;  ///< Satellite system of Epoch
      short PRNID;         ///< SV PRN ID
      long HOWtime;        ///< Time of subframe 1-3 (sec of week)
      short weeknum;       ///< GPS full week number that corresponds to the HOWtime of SF1
                           ///< (N.B.: in RINEX files, week number corresponds to ToE.)
      double accuracy;     ///< SV accuracy (m)
      short health;        ///< SV health
         //@}

         /** @name EpochDataGPS
          */
         //@{
      short   codeflgs;    ///< L2 codes
      short   L2Pdata;     ///< L2 P data flag 
      double  IODC;        ///< Index of data-clock
      double  IODE;        ///< Index of data-eph
         //@}

         /** @name EpochDataGLO
          */
         //@{
      short   freqNum;     ///< Frequency number (-7..+12)
      double  ageOfInfo;   ///< Age of oper. information (days)
         //@}

         /** @name EpochDataGAL
          */
         //@{
      short   datasources; ///< Data sources
      double  IODnav;      ///< Index of data-eph
         //@}

         /** @name ClockInformation
          */
         //@{
      double  Toc;         ///< Clock epoch (sec of week) (found in epoch line of RINEX 3 Nav file)
      double  af0;         ///< SV clock error (sec)
      double  af1;         ///< SV clock drift (sec/sec)
      double  af2;         ///< SV clock drift rate (sec/sec**2)
      double  Tgd;         ///< Group delay differential (sec) (GPS)
      double  BGDa, BGDb;  ///< SV clock parameters for E5a/E1 and E5b/E1 combinations (Galileo)
         //@}

         /** @name HarmonicPerturbations
          */
         //@{
      double  Cuc;         ///< Cosine latitude (rad)
      double  Cus;         ///< Sine latitude (rad)
      double  Crc;         ///< Cosine radius (m)
      double  Crs;         ///< Sine radius (m)
      double  Cic;         ///< Cosine inclination (rad)
      double  Cis;         ///< Sine inclination (rad)
         //@}

         /** @name MajorEphemerisParameters
          */
         //@{
      double  Toe;         ///< Ephemeris epoch (sec of week)
      double  M0;          ///< Mean anomaly (rad)
      double  dn;          ///< Correction to mean motion (rad/sec)
      double  ecc;         ///< Eccentricity
      double  Ahalf;       ///< SQRT of semi-major axis (m**1/2)
      double  OMEGA0;      ///< Rt ascension of ascending node (rad)
      double  i0;          ///< Inclination (rad)
      double  w;           ///< Argument of perigee (rad)
      double  OMEGAdot;    ///< Rate of Rt ascension (rad/sec)
      double  idot;        ///< Rate of inclination angle (rad/sec)
      double  fitint;      ///< Fit interval
         //@}

         /** @name TabularEphemerisParameters
          */
         //@{
      double   px, py, pz; ///< SV position
      double   vx, vy, vz; ///< SV velocity
      double   ax, ay, az; ///< SV acceleration
         //@}

   private:

         /// Parses string \a currentLine to obtain PRN id and epoch.
      void getPRNEpoch(const std::string& currentLine)
         throw(gpstk::StringUtils::StringException, FFStreamError);

         /** @name OrbitParameters
          * Obtain orbit parameters from strint \a currentLine.
          */
         //@{
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
         //@}

         /// generates a line to be output to a file for the PRN/epoch line
      std::string putPRNEpoch(void) const
         throw(gpstk::StringUtils::StringException);

         /** @name OrbitParameters
          * Generate orbit parameter lines from data to be output to a file
          */
         //@{
         /// Writes line 1 of the Nav Data record
      std::string putBroadcastOrbit1(void) const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 2 of the Nav Data record
      std::string putBroadcastOrbit2(void) const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 3 of the Nav Data record
      std::string putBroadcastOrbit3(void) const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 4 of the Nav Data record
      std::string putBroadcastOrbit4(void) const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 5 of the Nav Data record
      std::string putBroadcastOrbit5(void) const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 6 of the Nav Data record
      std::string putBroadcastOrbit6(void) const
         throw(gpstk::StringUtils::StringException);
         /// Writes line 7 of the Nav Data record
         /// @warning Pass in version to decide wheter or not
         ///          to write fit interval.
      std::string putBroadcastOrbit7(const double ver) const
         throw(gpstk::StringUtils::StringException);
         //@}

   protected:

         /// Outputs the record to the FFStream \a s.
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);

         /**
          * This function retrieves a RINEX 3 NAV record from the given FFStream.
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

   };  // class Rinex3NavData

   //@}

} // namespace


#endif
