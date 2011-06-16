#pragma ident "$Id$"

/**
 * @file Rinex3NavHeader.hpp
 * Encapsulate header of RINEX 3 navigation file.
 */

#ifndef GPSTK_RINEX3NAVHEADER_HPP
#define GPSTK_RINEX3NAVHEADER_HPP

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

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "StringUtils.hpp"
#include "FFStream.hpp"
#include "Rinex3NavBase.hpp"

namespace gpstk
{
  /** @addtogroup Rinex3Nav */
  //@{

  /**
   * This class models the RINEX 3 Nav header for a RINEX 3 Nav file.
   *
   * \sa Rinex3NavData and Rinex3NavStream classes.
   */

  class Rinex3NavHeader : public Rinex3NavBase
  {
  public:

     /// Constructor
     Rinex3NavHeader(void)
       : valid(0), version(3.0)
     {}

     /// Destructor
     virtual ~Rinex3NavHeader()
     {}

     /// Rinex3NavHeader is a "header" so this function always returns true.
     virtual bool isHeader(void) const {return true;}

     /// This function dumps the contents of the header.
     virtual void dump(std::ostream& s) const;

     /// All 'valid..' bits found in this header
     unsigned long valid;

     /// These are validity bits used in checking the RINEX NAV header.
     enum validBits
     {
       validVersion     = 0x01,         ///< Set if RINEX 3 version is valid.
       validRunBy       = 0x02,         ///< Set if Run-by value is valid.
       validComment     = 0x04,         ///< Set if Comments are valid
       validIonoCorrGPS = 0x08,         ///< Set if GPS Iono Correction data is valid.
       validIonoCorrGal = 0x010,        ///< Set if Gal Iono Correction data is valid.
       validTimeSysCorr = 0x020,        ///< Set if Time System Correction is valid.
       validLeapSeconds = 0x040,        ///< Set if the Leap Seconds value is valid.
       validEoH         = 0x080000000,  ///< Set if the End of Header is valid.

       /// This bitset checks that all required header items are available
       /// for a Rinex 3 (3.00 or 3.01) version file.
       allValid30 = 0x080000003,
       allValid301 = 0x080000003,
     };

     /// Enum of Time System Correction types.
     enum TimeSysCorrEnum
     {
       GAUT,  /// GAL  to UTC using A0, A1
       GPUT,  /// GPS  to UTC using A0, A1
       SBUT,  /// SBAS to UTC using A0, A1
       GLUT,  /// GLO  to UTC using A0 = TauC  , A1 = 0
       GPGA,  /// GPS  to GAL using A0 = A0G   , A1 = A1G
       GLGP   /// GLO  to GPS using A0 = TauGPS, A1 = 0
     };

     /** @name HeaderValues
      */
     //@{
     double version;           ///< RINEX Version
     std::string fileType, satSys, fileProgram, fileAgency, date;
     std::vector<std::string> commentList;
     std::string ionoCorrType;
     double ionoParam1[4], ionoParam2[4], ionoParamGal[3];
     long leapSeconds;
     std::string timeSysCorrType;
     TimeSysCorrEnum timeSysCorrEnum;
     double A0, A1;
     long timeSysRefTime, timeSysRefWeek;
     std::string timeSysCorrSBAS;
     long timeSysUTCid;
     struct TimeSysCorrInfo
     {
       std::string timeSysCorrType;
       double A0, A1;
       long timeSysRefTime, timeSysRefWeek;
       std::string timeSysCorrSBAS;
       long timeSysUTCid;
     };
     //@}

     /// Map for Time System Correction info.
     typedef std::map<TimeSysCorrEnum, TimeSysCorrInfo> TimeSysCorrMap;

     /// Instance of the map.
     TimeSysCorrMap tscMap;

     /** @name FormattingStd::Strings
      */
     //@{
     static const std::string stringVersion;     // "RINEX VERSION / TYPE"
     static const std::string stringRunBy;       // "PGM / RUN BY / DATE"
     static const std::string stringComment;     // "COMMENT"
     static const std::string stringIonoCorr;    // "IONOSPHERIC CORR"
     static const std::string stringTimeSysCorr; // "TIME SYSTEM CORR"
     static const std::string stringCorrSysTime; // "CORR TO SYSTEM TIME"  // R2.10
     static const std::string stringLeapSeconds; // "LEAP SECONDS"
     static const std::string stringEoH;         // "END OF HEADER"
     //@}

     /// Define timeSysCorrType based on input string from RINEX 3 file read.
     /// @param str input string, expected to match output string for given system
     void setTimeSysCorrFromString(const std::string str)
       throw();
      
     /// Add a TimeSysCorrInfo object to the map.
     /// @param tsci the TimeSysCorrInfo to add
     void addTimeSysCorr( const TimeSysCorrInfo& tsci )
       throw();

   protected:

     /// Writes a correctly formatted record from this data to stream \a s.
     virtual void reallyPutRecord(FFStream& s) const
       throw(std::exception, FFStreamError,
             gpstk::StringUtils::StringException);

     /**
      * This function reads the RINEX 3 NAV header from the given FFStream.
      * If an error is encountered in reading from the stream, the stream
      * is reset to its original position and its fail-bit is set.
      * @throws StringException when a StringUtils function fails
      * @throws FFStreamError when exceptions(failbit) is set and a read
      *         or formatting error occurs.  This also resets the stream
      *         to its pre-read position.
      */
     virtual void reallyGetRecord(FFStream& s)
       throw(std::exception, FFStreamError,
             gpstk::StringUtils::StringException);

  private:

    static const std::string timeSysCorrStrings[];

  }; // class Rinex3NavHeader

  //@}

} // namespace

#endif // GPSTK_RINEX3NAVHEADER_HPP
