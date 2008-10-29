#pragma ident "$Id$"

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

/**
 * @file Rinex3ObsHeader.hpp
 * Encapsulate header of Rinex3 observation file, including I/O
 */

#ifndef GPSTK_RINEX3OBSHEADER_HPP
#define GPSTK_RINEX3OBSHEADER_HPP

#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <iomanip>

#include "CommonTime.hpp"
#include "FFStream.hpp"
#include "Rinex3ObsBase.hpp"
#include "Triple.hpp"
#include "Rinex3SatID.hpp"

namespace gpstk
{
   /** @addtogroup Rinex3Obs */
   //@{

      /**
       * This class models the header for a RINEX3 Observation File.
       * @sa gpstk::Rinex3ObsData and gpstk::Rinex3ObsStream.
       * @sa rinex_obs_test.cpp and rinex_obs_read_write.cpp for examples.
       */
   class Rinex3ObsHeader : public Rinex3ObsBase
   {
   public:

         /// A Simple Constructor.
      Rinex3ObsHeader() : version(2.1), valid()
         {}

         /// Clear (empty out) header
      inline void clear()
      {
         version = 3.0;
         valid = 0;
         commentList.clear();
         wavelengthFactor[0] = wavelengthFactor[1] = 1;
         extraWaveFactList.clear();
         obsIDList.clear();
         numObsForSat.clear();
         numObs = 0;
         lastPRN.id = -1;
      }

         /**
          * @name Rinex3ObsHeaderFormatStrings
          * RINEX3 Observation Header Formatting Strings
          */
         //@{
      static const std::string versionString;         ///< "RINEX3 VERSION / TYPE"
      static const std::string runByString;           ///< "PGM / RUN BY / DATE"
      static const std::string commentString;         ///< "COMMENT"
      static const std::string markerNameString;      ///< "MARKER NAME"
      static const std::string markerNumberString;    ///< "MARKER NUMBER"
      static const std::string observerString;        ///< "OBSERVER / AGENCY"
      static const std::string receiverString;        ///< "REC # / TYPE / VERS"
      static const std::string antennaTypeString;     ///< "ANT # / TYPE"
      static const std::string antennaPositionString; ///< "APPROX POSITION XYZ"
      static const std::string antennaOffsetString;   ///< "ANTENNA: DELTA H/E/N"
      static const std::string antennaReferncePoint;  ///< "ANTENNA: DELTA X/Y/Z"
      static const std::string antennaPhaseCenter;    ///< "ANTENNA: PHASECENTER"
      static const std::string antennaBSight;         ///< "ANTENNA: B.SIGHT XYZ"
      static const std::string antennaZeroDirAZI;     ///< "ANTENNA: ZERODIR AZI"
      static const std::string antennaZeroDirXYZ;     ///< "ANTENNA: ZERODIR XYZ"
      static const std::string centerOfMass;          ///< "CENTER OF MASS: XYZ"
      static const std::string numObsString;          ///< "SYS / # / OBS TYPES"
      static const std::string signalStrengthUnit;    ///< "SISNGAL STRENGTH UNIT"
      static const std::string intervalString;        ///< "INTERVAL"
      static const std::string firstTimeString;       ///< "TIME OF FIRST OBS"
      static const std::string lastTimeString;        ///< "TIME OF LAST OBS"
      static const std::string receiverOffsetString;  ///< "RCV CLOCK OFFS APPL"
      static const std::string sysDCBS;               ///< "SYS / DCBS APPLIED"
      static const std::string sysPCVS;               ///< "SYS / PCVS APPLIED"
      static const std::string sysScaleFactor;        ///< "SYS / SCALE FACTOR"
      static const std::string leapSecondsString;     ///< "LEAP SECONDS"
      static const std::string numSatsString;         ///< "# OF SATELLITES"
      static const std::string prnObsString;          ///< "PRN / # OF OBS"
      static const std::string endOfHeader;           ///< "END OF HEADER"
         //@}

         /// Validity bits for the RINEX3 Observation Header
      enum validBits
      {
         versionValid = 0x01,           ///< "RINEX3 VERSION / TYPE"
         runByValid = 0x02,             ///< "PGM / RUN BY / DATE"
         commentValid = 0x04,           ///< "COMMENT"
         markerNameValid = 0x08,        ///< "MARKER NAME"
         markerNumberValid = 0x010,     ///< "MARKER NUMBER"
         observerValid = 0x020,         ///< "OBSERVER / AGENCY"
         receiverValid = 0x040,         ///< "REC # / TYPE / VERS"
         antennaTypeValid = 0x080,      ///< "ANT # / TYPE"
         antennaPositionValid = 0x0100, ///< "APPROX POSITION XYZ"
         antennaOffsetValid = 0x0200,   ///< "ANTENNA: DELTA H/E/N"
         waveFactValid = 0x0400,        ///< "WAVELENGTH FACT L1/2"
         obsTypeValid = 0x0800,         ///< "# / TYPES OF OBSERV"
         intervalValid = 0x01000,       ///< "INTERVAL"
         firstTimeValid = 0x02000,      ///< "TIME OF FIRST OBS"
         lastTimeValid = 0x04000,       ///< "TIME OF LAST OBS"
         receiverOffsetValid = 0x08000, ///< "RCV CLOCK OFFS APPL"
         leapSecondsValid = 0x0100000,  ///< "LEAP SECONDS"
         numSatsValid = 0x0200000,      ///< "# OF SATELLITES"
         prnObsValid = 0x0400000,       ///< "PRN / # OF OBS"

         endValid = 0x080000000,        ///< "END OF HEADER"

            /// This mask is for all required valid fields for RINEX3 2.0
         allValid20 = 0x080002FEB,
            /// This mask is for all required valid fields for RINEX3 2.1
         allValid21 = 0x080002FEB,
            /// This mask is for all required valid fields for RINEX3 2.11
         allValid211 = 0x080002FEB
      };

         /** @name Standard RINEX3 observation types
          */
         //@{
      static const ObsID UN;
      // GPS      
      static const ObsID C1C;
      static const ObsID L1C;
      static const ObsID D1C;
      static const ObsID S1C;
      static const ObsID C1P;
      static const ObsID L1P;
      static const ObsID D1P;
      static const ObsID S1P;
      // And so on....

      // Galileo only
      static const ObsID C1A;
      static const ObsID L1A;
      static const ObsID D1A;
      static const ObsID S1A;
      static const ObsID C1B;
      static const ObsID L1B;
      static const ObsID D1B;
      static const ObsID S1B;
      /// And so on...

         //@}

      static const std::vector<ObsID> StandardRinex3ObsIDs;
      static std::vector<ObsID> RegisteredRinex3ObsIDs;

         /// Holds the data for the extra Wavelength Factor lines
      struct ExtraWaveFact
      {
            /// List of PRNs with this wavelength factor.
         std::vector<SatID> satList;
            /// The vector of wavelength factor values.
         short wavelengthFactor[2];
      };

         /** @name Rinex3ObsHeaderValues
          */ 
         //@{
      double version;                        ///< RINEX3 VERSION & TYPE
      std::string fileType;                  ///< RINEX3 FILETYPE (Observation Navigation etc)
      //std::string system_str;              ///< The string (for file i/o) giving the RINEX3 system 
      Rinex3SatID system;                     ///< The RINEX3 satellite system
      std::string fileProgram,               ///< The program used to generate this file
         fileAgency,                         ///< Who ran the program.
         date;                               ///< When the program was run.
      std::vector<std::string> commentList;  ///< Comments in header (optional)
      std::string markerName,                ///< MARKER NAME
         markerNumber;                       ///< MARKER NUMBER (optional)
      std::string observer,                  ///< OBSERVER : who collected the data
         agency;                             ///< OBSERVER'S AGENCY
      std::string recNo,                     ///< RECEIVER NUMBER
         recType,                            ///< RECEIVER TYPE
         recVers;                            ///< RECEIVER VERSION
      std::string antNo,                     ///< ANTENNA NUMBER
         antType;                            ///< ANTENNA TYPE
      gpstk::Triple antennaPosition;         ///< APPROXIMATE POSITION XYZ
      gpstk::Triple antennaOffset;           ///< ANTENNA: DELTA H/E/N
      gpstk::Triple antennaRefPoint;         ///< ANTENNA: DELTA X/Y/Z
      gpstk::Triple antennaPhasecenter;      ///< ANTENNA: PHASECENTER
      gpstk::Triple antennaBSight;           ///< ANTENNA: B.SIGHT XYZ
      gpstk::Triple antennaZeroDirAZI;       ///< ANTENNA: ZERODIR AZI
      gpstk::Triple antennaZeroDirXYZ;       ///< ANTENNA: ZERODIR XYZ
      gpstk::Triple centerOfMass;            ///< CENTER OF MASS: XYZ
      short wavelengthFactor[2];             ///< default WAVELENGTH FACTORS
      std::vector<ExtraWaveFact> extraWaveFactList; ///< extra (per PRN) WAVELENGTH FACTORS
      std::vector<ObsID> obsIDList; ///< NUMBER & TYPES OF OBSERV
      double interval;                       ///< INTERVAL (optional)
      CommonTime firstObs ;                     ///< TIME OF FIRST OBS
      Rinex3SatID firstSystem;                ///< RINEX3 satellite system of FIRST OBS timetag
      CommonTime lastObs ;                      ///< TIME OF LAST OBS (optional)
      Rinex3SatID lastSystem;                 ///< RINEX3 satellite system of LAST OBS timetag
      int receiverOffset;                    ///< RCV CLOCK OFFS APPL (optional)
      int leapSeconds;                       ///< LEAP SECONDS (optional)
      short numSVs;                          ///< NUMBER OF SATELLITES in following map (optional)
      std::map<SatID, std::vector<int> > numObsForSat; ///<  PRN / # OF OBS (optional)
      unsigned long valid; ///< Bits set when individual header members are present and valid
      int numObs; ///< used to save the number of obs on # / TYPES continuation lines.
      Rinex3SatID lastPRN; ///< used to save the current PRN while reading PRN/OBS continuation lines.
      

   //@}
     
         /// Destructor
      virtual ~Rinex3ObsHeader() {}

         // The next four lines is our common interface
         /// Rinex3ObsHeader is a "header" so this function always returns true.
      virtual bool isHeader() const {return true;}
     
         /**
          * This is a simple Debug output function.
          * It simply outputs the version, name and antenna number of this
          * RINEX3 header.
          */
      virtual void dump(std::ostream& s) const;

         /**
          * Parse a single header record, and modify valid accordingly.
          * Used by reallyGetRecord for both Rinex3ObsHeader and Rinex3ObsData.
          */
      void ParseHeaderRecord(std::string& line)
         throw(FFStreamError);

         /// Compute the number of valid header records which WriteHeaderRecords() will write
      int NumberHeaderRecordsToBeWritten(void) const throw();

         /**
          * Write all valid header records to the given stream.
          * Used by reallyPutRecord for both Rinex3ObsHeader and Rinex3ObsData.
          */
      void WriteHeaderRecords(FFStream& s) const
         throw(FFStreamError, gpstk::StringUtils::StringException);

         /// Return boolean : is this a valid Rinex3 header?
      bool isValid() const { return ((valid & allValid20) == allValid20); }
         
   protected:
         /**
          * outputs this record to the stream correctly formatted.
          */
      virtual void reallyPutRecord(FFStream& s) const
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);

         /**
          * This function retrieves the RINEX3 Header from the given FFStream.
          * If an stream error is encountered, the stream is reset to its
          *  original position and its fail-bit is set.
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s) 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);

      friend class Rinex3ObsData;

   private:
         /// Converts the daytime \a dt into a Rinex3 Obs time
         /// string for the header
      std::string writeTime(const CommonTime& dt) const;

         /**
          * This function sets the time for this header.
          * It looks at \a line to obtain the needed information.
          */
      CommonTime parseTime(const std::string& line) const;

   }; // end class Rinex3ObsHeader

   //@}

} // namespace

#endif
