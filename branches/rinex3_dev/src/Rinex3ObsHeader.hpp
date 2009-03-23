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
       commentList.clear();
       obsTypeList.clear();
       numObsForSat.clear();
       valid  = 0;
       numObs = 0;
       lastPRN.id = -1;
     }

     /**
      * @name Rinex3ObsHeaderFormatStrings
      * RINEX 3 observation file header formatting strings
      */
     //@{
     static const std::string stringVersion;           ///< "RINEX VERSION / TYPE"
     static const std::string stringRunBy;             ///< "PGM / RUN BY / DATE"
     static const std::string stringComment;           ///< "COMMENT"               (optional)
     static const std::string stringMarkerName;        ///< "MARKER NAME"
     static const std::string stringMarkerNumber;      ///< "MARKER NUMBER"         (optional)
     static const std::string stringMarkerType;        ///< "MARKER TYPE"
     static const std::string stringObserver;          ///< "OBSERVER / AGENCY"
     static const std::string stringReceiver;          ///< "REC # / TYPE / VERS"
     static const std::string stringAntennaType;       ///< "ANT # / TYPE"
     static const std::string stringAntennaPosition;   ///< "APPROX POSITION XYZ"   (optional if moving)
     static const std::string stringAntennaDeltaHEN;   ///< "ANTENNA: DELTA H/E/N"
     static const std::string stringAntennaDeltaXYZ;   ///< "ANTENNA: DELTA X/Y/Z"  (optional)
     static const std::string stringAntennaPhaseCtr;   ///< "ANTENNA: PHASECENTER"  (optional)
     static const std::string stringAntennaBsightXYZ;  ///< "ANTENNA: B.SIGHT XYZ"  (optional)
     static const std::string stringAntennaZeroDirAzi; ///< "ANTENNA: ZERODIR AZI"  (optional)
     static const std::string stringAntennaZeroDirXYZ; ///< "ANTENNA: ZERODIR XYZ"  (optional)
     static const std::string stringCenterOfMass;      ///< "CENTER OF MASS: XYZ"   (optional)
     static const std::string stringSystemNumObs;      ///< "SYS / # / OBS TYPES"
     static const std::string stringSigStrengthUnit;   ///< "SIGNAL STRENGTH UNIT"  (optional)
     static const std::string stringInterval;          ///< "INTERVAL"              (optional)
     static const std::string stringFirstTime;         ///< "TIME OF FIRST OBS"
     static const std::string stringLastTime;          ///< "TIME OF LAST OBS"      (optional)
     static const std::string stringReceiverOffset;    ///< "RCV CLOCK OFFS APPL"   (optional)
     static const std::string stringSystemDCBSapplied; ///< "SYS / DCBS APPLIED"    (optional)
     static const std::string stringSystemPCVSapplied; ///< "SYS / PCVS APPLIED"    (optional)
     static const std::string stringSystemScaleFac;    ///< "SYS / SCALE FACTOR"    (optional)
     static const std::string stringLeapSeconds;       ///< "LEAP SECONDS"          (optional)
     static const std::string stringNumSats;           ///< "# OF SATELLITES"       (optional)
     static const std::string stringPrnObs;            ///< "PRN / # OF OBS"        (optional)
     static const std::string stringEoH;               ///< "END OF HEADER"
     //@}

     /// Validity bits for the RINEX Observation Header
     enum validBits
     {
       validVersion           = 0x01,        ///< "RINEX VERSION / TYPE"
       validRunBy             = 0x02,        ///< "PGM / RUN BY / DATE"
       validComment           = 0x04,        ///< "COMMENT"               (optional)
       validMarkerName        = 0x08,        ///< "MARKER NAME"
       validMarkerNumber      = 0x010,       ///< "MARKER NUMBER"         (optional)
       validMarkerType        = 0x020,       ///< "MARKER TYPE"
       validObserver          = 0x040,       ///< "OBSERVER / AGENCY"
       validReceiver          = 0x080,       ///< "REC # / TYPE / VERS"
       validAntennaType       = 0x0100,      ///< "ANT # / TYPE"
       validAntennaPosition   = 0x0200,      ///< "APPROX POSITION XYZ"   (optional if moving)
       validAntennaDeltaHEN   = 0x0400,      ///< "ANTENNA: DELTA H/E/N"
       validAntennaDeltaXYZ   = 0x0800,      ///< "ANTENNA: DELTA X/Y/Z"  (optional)
       validAntennaPhaseCtr   = 0x01000,     ///< "ANTENNA: PHASECENTER"  (optional)
       validAntennaBsightXYZ  = 0x02000,     ///< "ANTENNA: B.SIGHT XYZ"  (optional)
       validAntennaZeroDirAzi = 0x04000,     ///< "ANTENNA: ZERODIR AZI"  (optional)
       validAntennaZeroDirXYZ = 0x08000,     ///< "ANTENNA: ZERODIR XYZ"  (optional)
       validCenterOfMass      = 0x010000,    ///< "CENTER OF MASS: XYZ"   (optional)
       validObsType           = 0x020000,    ///< "SYS / # / OBS TYPES"
       validSigStrengthUnit   = 0x040000,    ///< "SIGNAL STRENGTH UNIT"  (optional)
       validInterval          = 0x080000,    ///< "INTERVAL"
       validFirstTime         = 0x0100000,   ///< "TIME OF FIRST OBS"
       validLastTime          = 0x0200000,   ///< "TIME OF LAST OBS"      (optional)
       validReceiverOffset    = 0x0400000,   ///< "RCV CLOCK OFFS APPL"   (optional)
       validSystemDCBSapplied = 0x0800000,   ///< "SYSTEM DCBS APPLIED"   (optional)
       validSystemPCVSapplied = 0x01000000,  ///< "SYSTEM PCVS APPLIED"   (optional)
       validSystemScaleFac    = 0x02000000,  ///< "SYSTEM SCALE FACTOR"   (optional)
       validLeapSeconds       = 0x04000000,  ///< "LEAP SECONDS"          (optional)
       validNumSats           = 0x08000000,  ///< "# OF SATELLITES"       (optional)
       validPrnObs            = 0x010000000, ///< "PRN / # OF OBS"        (optional)
       validEoH               = 0x080000000, ///< "END OF HEADER"

       /// This mask is for all required valid fields for RINEX 3.0 (3.00)
       allValid30 = 0x0801A07EB,  // case for static receivers
//       allValid30 = 0x0801A05EB,  // case for moving receivers
     };

     /// RINEX 3 observation types (struct declaration)
     struct Rinex3ObsType
     {
       std::string type,          ///<  3-char type                  ; e.g. C1C, D2P, L5Q, S2M, etc.
                   description,   ///< 20-char description (optional); e.g. "L1 pseudorange"
                   units;         ///< 10-char units       (optional); e.g. "meters"
       int scaleFactor;           ///< factor to divide stored observations with before use

       Rinex3ObsType()
         : type(std::string("UN")),description(std::string("Unknown or Invalid")),
           units(std::string("")),scaleFactor(1)
       {}

       Rinex3ObsType(std::string t, std::string d, std::string u, int sf = 1)
         : type(t),description(d),units(u),scaleFactor(sf)
       {}
     };

     /// RINEX 3 DCBS info (for differential code bias and phase center variations corrections)
     struct Rinex3CorrInfo
     {
       std::string satSys,  ///< 1-char SV system (G/R/E/S)
                   name,    ///< program name used to apply corrections
                   source;  ///< source of corrections (URL)
     };


     /** @name Standard RINEX observation types
      */
     //@{

     //@}

     static const std::vector<Rinex3ObsType> StandardRinex3ObsTypes;
     static       std::vector<Rinex3ObsType> RegisteredRinex3ObsTypes;

     /** @name Rinex3ObsHeaderValues
      */ 
     //@{
     double version;                                 ///< RINEX 3 version/type
     std::string fileType,                           ///< RINEX 3 file type
                 satSys;                             ///< RINEX 3 satellite system
     RinexSatID system;                              ///< RINEX satellite system (enum)
     std::string fileProgram,                        ///< program used to generate file
                 fileAgency,                         ///< who ran program
                 date;                               ///< when program was run
     std::vector<std::string> commentList;           ///< comments in header             (optional)
     std::string markerName,                         ///< MARKER NAME
                 markerNumber,                       ///< MARKER NUMBER                  (optional)
                 markerType;                         ///< MARKER TYPE
     std::string observer,                           ///< who collected the data
                 agency;                             ///< observer's agency
     std::string recNo,                              ///< receiver number
                 recType,                            ///< receiver type
                 recVers;                            ///< receiver version
     std::string antNo,                              ///< antenna number
                 antType;                            ///< antenna type
     gpstk::Triple antennaPosition,                  ///< APPROX POSITION XYZ            (optional if moving)
                   antennaDeltaHEN,                  ///< ANTENNA: DELTA H/E/N
                   antennaDeltaXYZ;                  ///< ANTENNA: DELTA X/Y/Z           (optional)
     std::string antennaSatSys,                      ///< ANTENNA P.CTR BLOCK: SAT SYS   (optional)
                 antennaObsCode;                     ///< ANTENNA P.CTR BLOCK: OBS CODE  (optional)
     gpstk::Triple antennaPhaseCtr;                  ///< ANTENNA P.CTR BLOCK: PCTR POS  (optional)
     gpstk::Triple antennaBsightXYZ;                 ///< ANTENNA B.SIGHT XYZ            (optional)
     double        antennaZeroDirAzi;                ///< ANTENNA ZERODIR AZI            (optional)
     gpstk::Triple antennaZeroDirXYZ;                ///< ANTENNA ZERODIR XYZ            (optional)
     gpstk::Triple centerOfMass;                     ///< vehicle CENTER OF MASS: XYZ    (optional)
     std::vector<Rinex3ObsType> obsTypeList;         ///< number & types of observations
     std::string sigStrengthUnit;                    ///< SIGNAL STRENGTH UNIT           (optional)
     double interval;                                ///< INTERVAL                       (optional)
     CivilTime firstObs,                             ///< TIME OF FIRST OBS
                lastObs;                             ///< TIME OF LAST OBS               (optional)
     int receiverOffset;                             ///< RCV CLOCK OFFS APPL            (optional)
     std::vector<Rinex3CorrInfo> infoDCBS;           ///< DCBS INFO                      (optional)
     std::vector<Rinex3CorrInfo> infoPCVS;           ///< PCVS INFO                      (optional)
     int factor;                                     ///< scale factor (temp holder)
     int leapSeconds;                                ///< LEAP SECONDS                   (optional)
     short numSVs;                                   ///< # OF SATELLITES                (optional)
     std::map<SatID,std::vector<int> > numObsForSat; ///< PRN / # OF OBS                 (optional)
     unsigned long valid;                            ///< bits set when header members present & valid
     std::string tempSatSys;                         ///< used to save the Sat Sys char while reading Scale Factor lines
     int numObs;                                     ///< used to save number of obs on # / TYPES and Sys / SCALE FACTOR continuation lines
     RinexSatID lastPRN;                             ///< used to save current PRN while reading PRN/OBS continuation lines
     //@}

     /// Destructor
     virtual ~Rinex3ObsHeader()
     {}

     // The next four lines comprise our common interface.

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
     std::string writeTime(const CivilTime& civtime) const;

     /**
      * This function sets the time for this header.
      * It looks at \a line to obtain the needed information.
      */
     CivilTime parseTime(const std::string& line) const;

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
