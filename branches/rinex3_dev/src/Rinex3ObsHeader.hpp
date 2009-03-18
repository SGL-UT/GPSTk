#pragma ident "$Id: Rinex3ObsHeader.hpp 1709 2009-02-18 20:27:47Z btolman $"

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
 * Encapsulate header of Rinex observation file, including I/O
 */

#ifndef GPSTK_RINEX3OBSHEADER_HPP
#define GPSTK_RINEX3OBSHEADER_HPP

#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <iomanip>

#include "CivilTime.hpp"
#include "FFStream.hpp"
#include "Rinex3ObsBase.hpp"
#include "Triple.hpp"
#include "RinexSatID.hpp"

namespace gpstk
{
  /** @addtogroup Rinex3Obs */
  //@{

  /**
   * This class models the header for a RINEX 3 Observation File.
   * @sa gpstk::Rinex3ObsData and gpstk::Rinex3ObsStream.
   * @sa rinex_obs_test.cpp and rinex_obs_read_write.cpp for examples.
   */

   class Rinex3ObsHeader : public Rinex3ObsBase
   {
   public:

     /// A Simple Constructor.
     Rinex3ObsHeader()
       : valid(0), version(3.0)
     {}

     /// Clear (empty out) header
     inline void clear()
     {
       version = 3.0;
       valid = 0;
       commentList.clear();
       obsTypeList.clear();
       numObsForSat.clear();
       numObs = 0;
       lastPRN.id = -1;
     }

     /**
      * @name Rinex3ObsHeaderFormatStrings
      * RINEX 3 observation file header formatting strings
      */
     //@{
     static const std::string stringVersion;            ///< "RINEX VERSION / TYPE"
     static const std::string stringRunBy;              ///< "PGM / RUN BY / DATE"
     static const std::string stringComment;            ///< "COMMENT"
     static const std::string stringMarkerName;         ///< "MARKER NAME"
     static const std::string stringMarkerNumber;       ///< "MARKER NUMBER"
     static const std::string stringMarkerType;         ///< "MARKER TYPE"
     static const std::string stringObserver;           ///< "OBSERVER / AGENCY"
     static const std::string stringReceiver;           ///< "REC # / TYPE / VERS"
     static const std::string stringAntennaType;        ///< "ANT # / TYPE"
     static const std::string stringAntennaPosition;    ///< "APPROX POSITION XYZ"
     static const std::string stringAntennaDeltaHEN;    ///< "ANTENNA: DELTA H/E/N"
     static const std::string stringAntennaDeltaXYX;    ///< "ANTENNA: DELTA X/Y/Z"
     static const std::string stringAntennaPhaseCenter; ///< "ANTENNA: PHASECENTER"
     static const std::string stringAntennaBsightXYZ;   ///< "ANTENNA: BORESIGHT XYZ"
     static const std::string stringAntennaZeroDirAzi;  ///< "ANTENNA: ZERODIR AZIMUTH"
     static const std::string stringAntennaZeroDirXYZ;  ///< "ANTENNA: ZERODIR XYZ"
     static const std::string stringAntennaCoM;         ///< "ANTENNA: CENTER OF MASS"
     static const std::string stringSystemNumObs;       ///< "SYS / # / TYPES OF OBSERV"
     static const std::string stringSignalStrengthUnit; ///< "SIGNAL STRENGTH UNITS"
     static const std::string stringInterval;           ///< "INTERVAL"
     static const std::string stringFirstTime;          ///< "TIME OF FIRST OBS"
     static const std::string stringLastTime;           ///< "TIME OF LAST OBS"
     static const std::string stringReceiverOffset;     ///< "RCV CLOCK OFFS APPL"
     static const std::string stringSystemDCBSapplied;  ///< "SYSTEM DCBS APPLIED"
     static const std::string stringSystemPCVSapplied;  ///< "SYSTEM PCVS APPLIED"
     static const std::string stringSystemScaleFac;     ///< "SYSTEM SCALE FACTOR"
     static const std::string stringLeapSeconds;        ///< "LEAP SECONDS"
     static const std::string stringNumSats;            ///< "# OF SATELLITES"
     static const std::string stringPrnObs;             ///< "PRN / # OF OBS"
     static const std::string stringEndOfHeader;        ///< "END OF HEADER"
     //@}

     /// Validity bits for the RINEX Observation Header
     enum validBits
     {
       validVersion = 0x01,           ///< "RINEX VERSION / TYPE"
       validRunBy = 0x02,             ///< "PGM / RUN BY / DATE"
       validComment = 0x04,           ///< "COMMENT"
       validMarkerName = 0x08,        ///< "MARKER NAME"
       validMarkerNumber = 0x010,     ///< "MARKER NUMBER"
       validMarkerType = 0x,         ///< "MARKER TYPE"
       validObserver = 0x020,         ///< "OBSERVER / AGENCY"
       validReceiver = 0x040,         ///< "REC # / TYPE / VERS"
       validAntennaType = 0x080,      ///< "ANT # / TYPE"
       validAntennaPosition = 0x0100, ///< "APPROX POSITION XYZ"
       validAntennaDeltaHEN = 0x0200, ///< "ANTENNA: DELTA H/E/N"
       validAntennaDeltaXYZ = 0x0400, ///< "ANTENNA: DELTA X/Y/Z"
       validAntennaPhaseCenter = 0x, ///< "ANTENNA: PHASECENTER"
       validAntennaBsightXYZ = 0x,   ///< "ANTENNA: BORESIGHT XYZ"
       validAntennaZeroDirAzi = 0x,        ///< "ANTENNA: ZERODIR AZIMUTH"
       validAntennaZeroDirXYZ = 0x,        ///< "ANTENNA: ZERODIR XYZ"
       validAntennaCoM = 0x,         ///< "ANTENNA: CENTER OF MASS"
       validObsType = 0x0800,         ///< "SYS / # / TYPES OF OBSERV"
       validSigStrengthUnit = 0x,  ///< "SIGNAL STRENGTH UNITS"
       validInterval = 0x01000,       ///< "INTERVAL"
       validFirstTime = 0x02000,      ///< "TIME OF FIRST OBS"
       validLastTime = 0x04000,       ///< "TIME OF LAST OBS"
       validReceiverOffset = 0x08000, ///< "RCV CLOCK OFFS APPL"
       validSystemDCBSapplied = 0x,  ///< "SYSTEM DCBS APPLIED"
       validSystemPCVSapplied = 0x,  ///< "SYSTEM PCVS APPLIED"
       validSystemScaleFac = 0x,     ///< "SYSTEM SCALE FACTOR"
       validLeapSeconds = 0x0100000,  ///< "LEAP SECONDS"
       validNumSats = 0x0200000,      ///< "# OF SATELLITES"
       validPrnObs = 0x0400000,       ///< "PRN / # OF OBS"

       validEoH = 0x080000000,        ///< "END OF HEADER"

       /// This mask is for all required valid fields for RINEX 3.0 (3.00)
       allValid30 = 0x080002FEB,
     };

     /// RINEX 3 observation types
     struct Rinex3ObsType
     {
       std::string type;          ///<  3-char type                  ; e.g. C1C, D2P, L5Q, S2M, etc.
       std::string description;   ///< 20-char description (optional); e.g. "L1 pseudorange"
       std::string units;         ///< 10-char units       (optional); e.g. "meters"
       int scaleFactor;           ///< factor to divide stored observations with before use
       Rinex3ObsType()
         : type(std::string("UN")),description(std::string("Unknown or Invalid")),
           units(std::string("")),kscaleFactor(1)
       {}
       Rinex3ObsType(std::string t, std::string d, std::string u, int sf = 1)
         : type(t),description(d),units(u),scaleFactor(sf)
       {}
     };

     /// RINEX 3 DCBS info (differential code bias corrections)
     struct Rinex3DCBSinfo
     {
       std::string satSys,  ///< 1-char SV system (G/R/E/S)
                   name,    ///< program name used to apply corrections
                   source;  ///< source of corrections (URL)
     };

     /// RINEX 3 PCVS info (phase center variation corrections)
     struct Rinex3PCVSinfo
     {
       std::string satSys,  ///< 1-char SV system (G/R/E/S)
                   name,    ///< program name used to apply corrections
                   source;  ///< source of corrections (URL)
     };

     /** @name Standard RINEX observation types
      */
     //@{
     static const Rinex3ObsType UN;
     static const Rinex3ObsType L1;
     static const Rinex3ObsType L2;
     static const Rinex3ObsType C1;
     static const Rinex3ObsType C2;
     static const Rinex3ObsType P1;
     static const Rinex3ObsType P2;
     static const Rinex3ObsType D1;
     static const Rinex3ObsType D2;
     static const Rinex3ObsType S1;
     static const Rinex3ObsType S2;
     static const Rinex3ObsType T1;
     static const Rinex3ObsType T2;
     static const Rinex3ObsType C5;
     static const Rinex3ObsType L5;
     static const Rinex3ObsType D5;
     static const Rinex3ObsType S5;
     // Galileo only
     static const Rinex3ObsType C6;
     static const Rinex3ObsType L6;
     static const Rinex3ObsType D6;
     static const Rinex3ObsType S6;
     static const Rinex3ObsType C7;
     static const Rinex3ObsType L7;
     static const Rinex3ObsType D7;
     static const Rinex3ObsType S7;
     static const Rinex3ObsType C8;
     static const Rinex3ObsType L8;
     static const Rinex3ObsType D8;
     static const Rinex3ObsType S8;

     //@}

     static const std::vector<Rinex3ObsType> StandardRinex3ObsTypes;
     static       std::vector<Rinex3ObsType> RegisteredRinex3ObsTypes;

     /** @name Rinex3ObsHeaderValues
      */ 
     //@{
     double version;                                ///< RINEX 3 VERSION & TYPE
     std::string fileType,                          ///< RINEX 3 FILETYPE
                 satSys;                            ///< RINEX 3 SATELLITE SYSTEM
     //std::string system_str;                      ///< The string (for file i/o) giving the RINEX system 
     RinexSatID system;                             ///< The RINEX satellite system (derived type, enum)
     std::string fileProgram,                       ///< The program used to generate this file
                 fileAgency,                        ///< Who ran the program.
                 date;                              ///< When the program was run.
     std::vector<std::string> commentList;          ///< Comments in header (optional)
     std::string markerName,                        ///< MARKER NAME
                 markerNumber,                      ///< MARKER NUMBER (optional)
                 markerType;                        ///< MARKER TYPE   (optional)
     std::string observer,                          ///< OBSERVER : who collected the data
                 agency;                            ///< OBSERVER'S AGENCY
     std::string recNo,                             ///< RECEIVER NUMBER
                 recType,                           ///< RECEIVER TYPE
                 recVers;                           ///< RECEIVER VERSION
     std::string antNo,                             ///< ANTENNA NUMBER
                 antType;                           ///< ANTENNA TYPE
     gpstk::Triple antennaPosition,                 ///< APPROXIMATE POSITION XYZ
                   antennaDeltaHEN,                 ///< ANTENNA: DELTA H/E/N
                   antennaDeltaXYZ;                 ///< ANTENNA: DELTA X/Y/Z
     std::string antennaSatSys,                     ///< ANTENNA PHASECENTER BLOCK: SAT SYSTEM
                 antennaObsCode;                    ///< ANTENNA PHASECENTER BLOCK: OBS CODE
     gpstk::Triple antennaPhaseCtr;                 ///< ANTENNA PHASECENTER BLOCK: PHASE CENTER POSITION
     gpstk::Triple antennaBsight;                   ///< ANTENNA BORESIGHT XYZ
     double        antennaZerodirAzi;               ///< ANTENNA ZERO DIRECTION AZIMUTH (deg from north)
     gpstk::Triple antennaZerodirXYZ;               ///< ANTENNA ZERO DIRECTION XYZ
     gpstk::Triple centerOfMass;                    ///< VEHICLE CENTER OF MASS XYZ
     std::vector<Rinex3ObsType> obsTypeList;        ///< NUMBER & TYPES OF OBSERV
     std::string sigStrengthUnit;                   ///< SIGNAL STRENGTH UNIT (optional)
     double interval;                               ///< INTERVAL (optional)
     CivilTime firstObs,                            ///< TIME OF FIRST OBS
                lastObs;                            ///< TIME OF LAST OBS (optional)
     int receiverOffset;                            ///< RCV CLOCK OFFS APPL (optional)
     Rinex3DCBSinfo infoDCBS;                       ///< DCBS INFO
     Rinex3PCVSinfo infoPCVS;                       ///< PCVS INFO
     int leapSeconds;                               ///< LEAP SECONDS (optional)
     short numSVs;                                  ///< NUMBER OF SATELLITES in following map (optional)
     std::map<SatID,std::vector<int>> numObsForSat; ///< PRN / # OF OBS (optional)
     unsigned long valid;                           ///< Bits set when individual header members are present and valid
     int numObs;                                    ///< used to save the number of obs on # / TYPES continuation lines
     RinexSatID lastPRN;                            ///< used to save the current PRN while reading PRN/OBS continuation lines
     //@}

     /// Destructor
     virtual ~Rinex3ObsHeader()
     {}

     // The next four lines is our common interface.

     /// Rinex3ObsHeader is a "header" so this function always returns true.
     virtual bool isHeader() const
     { return true; }

     /**
      * This is a simple Debug output function.
      * It simply outputs the version, name and antenna number of this
      * RINEX header.
      */
     virtual void dump(std::ostream& s) const;

     /**
      * This function converts the string in \a oneObs to a Rinex3ObsType.
      */
     static Rinex3ObsType convertObsType(const std::string& oneObs)
       throw(FFStreamError);

     /**
      * This function converts the Rinex3ObsType in \a oneObs to a string.
      */
     static std::string convertObsType(const Rinex3ObsType& oneObs) 
       throw(FFStreamError);

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

     /// Return boolean : is this a valid Rinex header?
     bool isValid() const
     { return ((valid & allValid30) == allValid30); }

   protected:

     /**
      * outputs this record to the stream correctly formatted.
      */
      virtual void reallyPutRecord(FFStream& s) const
        throw(std::exception, FFStreamError,
              gpstk::StringUtils::StringException);

     /**
      * This function retrieves the RINEX Header from the given FFStream.
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

     /// Converts the daytime \a dt into a Rinex Obs time
     /// string for the header
     std::string writeTime(const CommonTime& dt) const;

     /**
      * This function sets the time for this header.
      * It looks at \a line to obtain the needed information.
      */
     CommonTime parseTime(const std::string& line) const;

   }; // end class Rinex3ObsHeader

   /// operator == for Rinex3ObsHeader::Rinex3ObsType
   inline bool operator==(const Rinex3ObsHeader::Rinex3ObsType& x,
                          const Rinex3ObsHeader::Rinex3ObsType& y )
   { return (x.type == y.type); }

   /// operator < for Rinex3ObsHeader::Rinex3ObsType
   inline bool operator<(const Rinex3ObsHeader::Rinex3ObsType& x,
                         const Rinex3ObsHeader::Rinex3ObsType& y )
   { return (x.type < y.type); }

   /// operator << for Rinex3ObsHeader::Rinex3ObsType
   inline std::ostream& operator<<(std::ostream& s,
                                   const Rinex3ObsHeader::Rinex3ObsType rot)
   {
     return s << "Type=" << rot.type
              << ", Description=" << rot.description
              << ", Units=" << rot.units;
   }

   /// Function to allow user to define a new RINEX observation type
   int RegisterExtendedRinex3ObsType(std::string t,
                                     std::string d = std::string("(undefined)"),
                                     std::string u = std::string("undefined"),
                                     unsigned int dep = 0                       );

   /// Pretty print a list of standard Rinex observation types
   void DisplayStandardRinex3ObsTypes(std::ostream& s);

   /// Pretty print a list of registered extended Rinex observation types
   void DisplayExtendedRinex3ObsTypes(std::ostream& s);

  //@}

} // namespace

#endif
