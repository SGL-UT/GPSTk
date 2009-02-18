#pragma ident "$Id$"

/**
 * @file Rinex3NavHeader.hpp
 * Encapsulate header of Rinex navigation file
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


#include "FFStream.hpp"
#include "Rinex3NavBase.hpp"

namespace gpstk
{
   /** @addtogroup Rinex3Nav */
   //@{

      /**
       * This class models the RINEX 3 NAV header for a RINEX 3 NAV file.
       *
       * \sa rinex3_nav_test.cpp and rinex3_nav_read_write.cpp for examples.
       * \sa gpstk::Rinex3NavData and gpstk::Rinex3NavStream classes.
       */
   class Rinex3NavHeader : public Rinex3NavBase
   {
   public:

         /// Constructor
     Rinex3NavHeader(void) : valid(0), version(3.0) {}

         /// Destructor
     virtual ~Rinex3NavHeader() {}

         /// Rinex3NavHeader is a "header" so this function always returns true.
     virtual bool isHeader(void) const {return true;}

         /// This function dumps the contents of the header.
     virtual void dump(std::ostream& s) const;

         /// Tell me, Am I valid?
     unsigned long valid;

         /// These are validity bits used in checking the RINEX NAV header.
     enum validBits
     {
       versionValid     = 0x01,   ///< Set if the RINEX 3 version is valid.
       runByValid       = 0x02,   ///< Set if the Run-by value is valid.
       commentValid     = 0x04,   ///< Set if the Comments are valid -- very subjective.
       ionoCorrValid    = 0x08,   ///< Set if the Iono Correction data is valid.
       timeSysCorrValid = 0x010,  ///< Set if the Time System Correction is valid.
       leapSecondsValid = 0x020,  ///< Set if the Leap Seconds value is valid.
       endValid = 0x080000000,    ///< Set if the end value is valid.

            /// This bitset checks that all required header items are available
            /// for a Rinex 3.0 version file
       allValid30 = 0x080000003
     };

         /** @name HeaderValues
          */
         //@{
     double version;           ///< RINEX Version
     std::string fileType, satSys, fileProgram, fileAgency, date;
     std::vector<std::string> commentList;
     std::string ionoCorrType;
     std::string timeSysCorrType;
     double ionoParam1[4], ionoParam2[4];
     double A0, A1;
     long timeSysRefTime, timeSysRefWeek;
     std::string timeSysCorrSBAS;
     long timeSysUTCid;
     long leapSeconds;
         //@}

         /** @name FormattingStd::Strings
          */
         //@{
     static const std::string versionString;     // "RINEX VERSION / TYPE"
     static const std::string runByString;       // "PGM / RUN BY / DATE"
     static const std::string commentString;     // "COMMENT"
     static const std::string ionoCorrString;    // "IONOSPHERIC CORR"
     static const std::string timeSysCorrString; // "TIME SYSTEM CORR"
     static const std::string leapSecondsString; // "LEAP SECONDS"
     static const std::string endOfHeader;       // "END OF HEADER"
         //@}

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

   }; // class Rinex3NavHeader

   //@}

} // namespace

#endif
