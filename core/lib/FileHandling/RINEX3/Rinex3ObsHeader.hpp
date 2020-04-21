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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file Rinex3ObsHeader.hpp
 * Encapsulate header of Rinex observation file, including I/O
 */

#ifndef GPSTK_RINEX3OBSHEADER_HPP
#define GPSTK_RINEX3OBSHEADER_HPP

#include <vector>
#include <list>
#include <map>
#include <set>
#include <iostream>
#include <iomanip>

#include "CivilTime.hpp"
#include "FFStream.hpp"
#include "Rinex3ObsBase.hpp"
#include "Triple.hpp"
#include "RinexSatID.hpp"
#include "RinexObsID.hpp"


namespace gpstk
{

      /// @ingroup FileHandling
      //@{

      /** @page rinobshead RINEX OBS header implementation
       *
       * For each header line type, the following are listed in the
       * table below:
       *   1. The label
       *   2. Its usage in RINEX 2
       *   3. Its usage in RINEX 3
       *   4. The name for validity checking and header variable
       *      storage (see below)
       *   5. Variables (data members) storing the header information
       *
       * | Header Field         | v2  | v3  | Name              | Variables         |
       * | :------------------- | :-- | :-- | :---------------- | :---------------- |
       * | RINEX VERSION / TYPE | req | req | Version           | version           |
       * |                    ^ |   ^ |   ^ |                 ^ | fileType          |
       * |                    ^ |   ^ |   ^ |                 ^ | fileSys           |
       * |                    ^ |   ^ |   ^ |                 ^ | fileSysSat        |
       * |                    ^ |   ^ |   ^ |                 ^ | preserveVerType   |
       * | PGM / RUN BY / DATE  | req | req | RunBy             | fileProgram       |
       * |                    ^ |   ^ |   ^ |                 ^ | fileAgency        |
       * |                    ^ |   ^ |   ^ |                 ^ | date              |
       * |                    ^ |   ^ |   ^ |                 ^ | preserveDate      |
       * | COMMENT              | opt | opt | Comment           | commentList       |
       * | MARKER NAME          | req | opt | MarkerName        | markerName        |
       * | MARKER NUMBER        | opt | opt | MarkerNumber      | markerNumber      |
       * | MARKER TYPE          |  -  | req | MarkerType        | markerType        |
       * | OBSERVER / AGENCY    | req | req | Observer          | observer          |
       * |                    ^ |   ^ |   ^ |                 ^ | agency            |
       * | REC # / TYPE / VERS  | req | req | Receiver          | recNo             |
       * |                    ^ |   ^ |   ^ |                 ^ | recType           |
       * |                    ^ |   ^ |   ^ |                 ^ | recVers           |
       * | ANT # / TYPE         | req | req | AntennaType       | antNo             |
       * |                    ^ |   ^ |   ^ |                 ^ | antType           |
       * | APPROX POSITION XYZ  | req | req | AntennaPosition   | antennaPosition   |
       * | ANTENNA: DELTA H/E/N | req | req | AntennaDeltaHEN   | antennaDeltaHEN   |
       * | ANTENNA: DELTA X/Y/Z |  -  | opt | AntennaDeltaXYZ   | antennaDeltaXYZ   |
       * | ANTENNA: PHASECENTER |  -  | opt | AntennaPhaseCtr   | antennaSatSys     |
       * |                    ^ |   ^ |   ^ |                 ^ | antennaObsCode    |
       * |                    ^ |   ^ |   ^ |                 ^ | antennaPhaseCtr   |
       * | ANTENNA: B.SIGHT XYZ |  -  | opt | AntennaBsightXYZ  | antennaBsightXYZ  |
       * | ANTENNA: ZERODIR AZI |  -  | opt | AntennaZeroDirAzi | antennaZeroDirAzi |
       * | ANTENNA: ZERODIR XYZ |  -  | opt | AntennaZeroDirXYZ | antennaZeroDirXYZ |
       * | CENTER OF MASS: XYZ  |  -  | opt | CenterOfMass      | centerOfMass      |
       * | # / TYPES OF OBSERV  | req | n/a | NumObs            |                   |
       * | SYS / # / OBS TYPES  | n/a | req | SystemNumObs      | mapObsTypes       |
       * | WAVELENGTH FACT L1/2 | opt |  -  | WaveFact          | wavelengthFactor  |
       * |                    ^ |   ^ |   ^ |                 ^ | extraWaveFactList |
       * | SIGNAL STRENGTH UNIT |  -  | opt | SigStrengthUnit   | sigStrengthUnit   |
       * | INTERVAL             | opt | opt | Interval          | interval          |
       * | TIME OF FIRST OBS    | req | req | FirstTime         | firstObs          |
       * | TIME OF LAST OBS     | opt | opt | LastTime          | lastObs           |
       * | RCV CLOCK OFFS APPL  | opt | opt | ReceiverOffset    | receiverOffset    |
       * | SYS / DCBS APPLIED   |  -  | opt | SystemDCBSapplied | infoDCBS          |
       * | SYS / PCVS APPLIED   |  -  | opt | SystemPCVSapplied | infoPCVS          |
       * | SYS / SCALE FACTOR   |  -  | opt | SystemScaleFac    | sysSfacMap        |
       * | SYS / PHASE SHIFT    |  -  | req | SystemPhaseShift  | sysPhaseShift     |
       * | GLONASS SLOT / FRQ # |  -  | req | GlonassSlotFreqNo | glonassFreqNo     |
       * | GLONASS COD/PHS/BIS  |  -  | req | GlonassCodPhsBias | glonassCodPhsBias |
       * | LEAP SECONDS         | opt | opt | LeapSeconds       | leapSeconds       |
       * | # OF SATELLITES      | opt | opt | NumSats           | numSVs            |
       * | PRN / # OF OBS       | opt | opt | PrnObs            | numObsForSat      |
       * | END OF HEADER        | req | req | EoH (header only) | -                 |
       *
       * The "Name" column in the above table is used for both
       * validity flags and header strings.  For the header strings,
       * prepend "hs", e.g. "hsVersion" will give you the header field
       * label.  For validity flags, prepend valid,
       * e.g. "validVersion" would indicate the presence of the "RINEX
       * VERSION / TYPE" header record.
       */

      /**
       * This class models the header for a RINEX 3 Observation File.
       * @sa gpstk::Rinex3ObsData and gpstk::Rinex3ObsStream.
       * @sa rinex_obs_test.cpp and rinex_obs_read_write.cpp for examples.
       *
       * RINEX 2 is also supported.
       */

   class Rinex3ObsHeader : public Rinex3ObsBase
   {
   public:

         /// A Simple Constructor.
      Rinex3ObsHeader();

         /** Clear (empty out) header, setting all data members to
          * default values */
      void clear();

         /// @name RINEX observation file header formatting strings
         ///@{
      static const std::string hsVersion;           ///< RINEX VERSION / TYPE
      static const std::string hsRunBy;             ///< PGM / RUN BY / DATE
      static const std::string hsComment;           ///< COMMENT
      static const std::string hsMarkerName;        ///< MARKER NAME
      static const std::string hsMarkerNumber;      ///< MARKER NUMBER
      static const std::string hsMarkerType;        ///< MARKER TYPE
      static const std::string hsObserver;          ///< OBSERVER / AGENCY
      static const std::string hsReceiver;          ///< REC # / TYPE / VERS
      static const std::string hsAntennaType;       ///< ANT # / TYPE
      static const std::string hsAntennaPosition;   ///< APPROX POSITION XYZ
      static const std::string hsAntennaDeltaHEN;   ///< ANTENNA: DELTA H/E/N
      static const std::string hsAntennaDeltaXYZ;   ///< ANTENNA: DELTA X/Y/Z
      static const std::string hsAntennaPhaseCtr;   ///< ANTENNA: PHASECENTER
      static const std::string hsAntennaBsightXYZ;  ///< ANTENNA: B.SIGHT XYZ
      static const std::string hsAntennaZeroDirAzi; ///< ANTENNA: ZERODIR AZI
      static const std::string hsAntennaZeroDirXYZ; ///< ANTENNA: ZERODIR XYZ
      static const std::string hsCenterOfMass;      ///< CENTER OF MASS: XYZ
      static const std::string hsNumObs;            ///< # / TYPES OF OBSERV
      static const std::string hsSystemNumObs;      ///< SYS / # / OBS TYPES
      static const std::string hsWaveFact;          ///< WAVELENGTH FACT L1/2
      static const std::string hsSigStrengthUnit;   ///< SIGNAL STRENGTH UNIT
      static const std::string hsInterval;          ///< INTERVAL
      static const std::string hsFirstTime;         ///< TIME OF FIRST OBS
      static const std::string hsLastTime;          ///< TIME OF LAST OBS
      static const std::string hsReceiverOffset;    ///< RCV CLOCK OFFS APPL
      static const std::string hsSystemDCBSapplied; ///< SYS / DCBS APPLIED
      static const std::string hsSystemPCVSapplied; ///< SYS / PCVS APPLIED
      static const std::string hsSystemScaleFac;    ///< SYS / SCALE FACTOR
      static const std::string hsSystemPhaseShift;  ///< SYS / PHASE SHIFT
      static const std::string hsGlonassSlotFreqNo; ///< GLONASS SLOT / FRQ #
      static const std::string hsGlonassCodPhsBias; ///< GLONASS COD/PHS/BIS
      static const std::string hsLeapSeconds;       ///< LEAP SECONDS
      static const std::string hsNumSats;           ///< # OF SATELLITES
      static const std::string hsPrnObs;            ///< PRN / # OF OBS
      static const std::string hsEoH;               ///< END OF HEADER
         ///@}

         /** Validity bits for the RINEX Observation Header - please
          * keep ordered as strings above */
      enum Field
      {
         validInvalid = 0,                    ///< Not a valid field
         validVersion,                        ///< RINEX VERSION / TYPE
         validFirst = validVersion,           ///< Used fore testing only
         validRunBy,                          ///< PGM / RUN BY / DATE
         validComment,                        ///< COMMENT
         validMarkerName,                     ///< MARKER NAME
         validMarkerNumber,                   ///< MARKER NUMBER
         validMarkerType,                     ///< MARKER TYPE
         validObserver,                       ///< OBSERVER / AGENCY
         validReceiver,                       ///< REC # / TYPE / VERS
         validAntennaType,                    ///< ANT # / TYPE
         validAntennaPosition,                ///< APPROX POSITION XYZ
         validAntennaDeltaHEN,                ///< ANTENNA: DELTA H/E/N
         validAntennaDeltaXYZ,                ///< ANTENNA: DELTA X/Y/Z
         validAntennaPhaseCtr,                ///< ANTENNA: PHASECENTER
         validAntennaBsightXYZ,               ///< ANTENNA: B.SIGHT XYZ
         validAntennaZeroDirAzi,              ///< ANTENNA: ZERODIR AZI
         validAntennaZeroDirXYZ,              ///< ANTENNA: ZERODIR XYZ
         validCenterOfMass,                   ///< CENTER OF MASS: XYZ
         validNumObs,                         ///< # / TYPES OF OBSERV
         validSystemNumObs,                   ///< SYS / # / OBS TYPES
         validWaveFact,                       ///< WAVELENGTH FACT L1/2
         validSigStrengthUnit,                ///< SIGNAL STRENGTH UNIT
         validInterval,                       ///< INTERVAL
         validFirstTime,                      ///< TIME OF FIRST OBS
         validLastTime,                       ///< TIME OF LAST OBS
         validReceiverOffset,                 ///< RCV CLOCK OFFS APPL
         validSystemDCBSapplied,              ///< SYSTEM DCBS APPLIED
         validSystemPCVSapplied,              ///< SYSTEM PCVS APPLIED
         validSystemScaleFac,                 ///< SYSTEM SCALE FACTOR
         validSystemPhaseShift,               ///< SYS / PHASE SHIFT
         validGlonassSlotFreqNo,              ///< GLONASS SLOT / FRQ #
         validGlonassCodPhsBias,              ///< GLONASS COD/PHS/BIS
         validLeapSeconds,                    ///< LEAP SECONDS
         validNumSats,                        ///< # OF SATELLITES
         validPrnObs,                         ///< PRN / # OF OBS
         validLast                            ///< Used for testing only.
      };
   
         /** RINEX 3 DCBS/PCVS info (for differential code bias and
          * phase center variations corr.) */
      class Rinex3CorrInfo
      {
      public:
         std::string satSys;  ///< 1-char SV system (G/R/E/S)
         std::string name;    ///< program name used to apply corrections
         std::string source;  ///< source of corrections (URL)
      };

         /// RINEX 2 extra "WAVELENGTH FACT" lines
      class ExtraWaveFact
      {
      public:
            /// List of Sats with this wavelength factor
         std::vector<SatID> satList;
            /// vector of wavelength factor values
         short wavelengthFactor[2];
      };

         /// Commonly used vector of strings
      typedef std::vector<std::string> StringVec;
         /// Simple vector of ints
      typedef std::vector<int> IntVec;
         /// @todo document me
      typedef std::map<RinexSatID, IntVec> PRNNumObsMap;
         /** Scale Factor corrections for observations
          * map <ObsType, ScaleFactor> */
      typedef std::map<RinexObsID, int> ScaleFacMap;
         /** Per-system scale factor corrections.
          * Satellite system map of scale factor maps
          * <(G/R/E/S), <Rinex3ObsType, scalefactor>> */
      typedef std::map<std::string, ScaleFacMap> SysScaleFacMap;
         /// Vector of obervables
      typedef std::vector<RinexObsID> RinexObsVec;
         /** Map system to observables
          * map <sys char, vec<ObsID> >;
          * NB defines data vec in ObsData */
      typedef std::map<std::string, RinexObsVec> RinexObsMap;
         /// @todo document me
      typedef std::map<std::string, RinexObsID> ObsIDMap;
         /// @todo document me
      typedef std::map<std::string, ObsIDMap> VersionObsMap;
         /// Map satellite ID to phase shift
      typedef std::map<RinexSatID, double> SVPhsShftMap;
         /// Map observation type to sv-phase shift map
      typedef std::map<RinexObsID, SVPhsShftMap> ObsPhsShftMap;
         /// Map system to observation type phase shift map
      typedef std::map<std::string, ObsPhsShftMap> SysPhsShftMap;
         /// Map GLONASS SV ID to frequency number
      typedef std::map<RinexSatID, int> GLOFreqNumMap;
         /// Map GLONASS SV observable to code phase bias
      typedef std::map<RinexObsID, double> GLOCodPhsBias;
         /// Map SysChar + R2 Obs Type to Obs ID
      typedef std::map<std::string,std::string> DisAmbMap;
         /// Corrections (e.g. code bias) vector
      typedef std::vector<Rinex3CorrInfo> CorrVec;
         /// Vector of wavelength factors
      typedef std::vector<ExtraWaveFact> FactorVector;
      typedef std::set<Field> FieldSet;

      class Fields
      {
      public:
         Fields() = default;
         Fields(const FieldSet& fields)
               : fieldsSet(fields)
         {}
            /// Compare field sets.
         bool operator==(const Fields& rhs) const
         { return fieldsSet == rhs.fieldsSet; }
            /// Return true if the set of fields differs between this and rhs.
         bool operator!=(const Fields& rhs) const
         { return !(fieldsSet == rhs.fieldsSet); }
            /// Set a specific field.
         Fields& set(Field f)
         { fieldsSet.insert(f); return *this; }
            /// Clear a specific field.
         Fields& clear(Field f)
         { fieldsSet.erase(f); return *this; }
            /// Return true if the specific field is set (preferred to operator&).
         bool isSet(Field f) const
         { return fieldsSet.count(f) > 0; }
            /// Clear all set fields.
         void clear()
         { fieldsSet.clear(); }
            /// Return true if fieldsSet is empty, false otherwise.
         bool empty() const
         { return fieldsSet.empty(); }
            /** Backwards compatibility method for code still implemented
             * around bit flags.
             */
         Fields operator&(const Fields& rhs) const;
            /** Backwards compatibility method for code still implemented
             * around bit flags.
             * @deprecated Please use isSet() instead. */
         Field operator&(Field rhs) const;
            /** Alias for set() for backwards compatibility.
             * @deprecated Please use set() instead. */
         Fields& operator|=(Field rhs)
         { return set(rhs); }
            /** Alias for clear(Field)
             * @deprecated Please use clear(Field) instead. */
         Fields& operator^=(Field rhs)
         { return clear(rhs); }
            /** Determine if a header is valid.
             * @note This object is assumed to be a set of required
             *   fields.  Generally you would call this method on one of
             *   the allValid2, etc. static objects defined above.
             * @see isValid(double,const Fields&)
             * @code{.cpp}
             * if (Fields::allValid2.isValid(flags))
             * @endcode
             * @param[in] present The set of flags present in an existing
             *   header to be compared with this set of required flags.
             * @return true If all the fields in fieldsSet are also
             *   contained in present.fieldsSet.
             */
         bool isValid(const Fields& present) const;
            /** Determine if a header is valid.
             * @param[in] version The RINEX version being validated.
             * @param[in] present The set of flags present in an existing
             *   header to be compared with this set of required flags.
             * @return true If all the fields in the fieldsSet value
             *   appropriate for the specified version are also contained
             *   in present.fieldsSet.
             */
         static bool isValid(double version, const Fields& present)
         { return getRequired(version).isValid(present); }
            /** Add descriptive information to an exception about which
             * header fields are missing.  This object is assumed to be
             * the set of header fields that are expected.
             * @param[in] valid The set of present header fields.
             * @param[out] exc The exception to add descriptive text to.
             * @post exc will have text entries added to it for each of
             *   the missing header fields, i.e. each field set in
             *   allValid that is not set in valid.
             */
         void describeMissing(const Fields& valid, Exception& exc);
            /** Get a reference to the set of required header fields for
             * a given RINEX version. */
         static Fields getRequired(double version);

            /// All the header fields set in this object.
         FieldSet fieldsSet;
      };

      static const Fields allValid2, allValid30, allValid301, allValid302,
         allValid303;

         /** Storage for R2 <-> R3 conversion of obstypes during
          * reallyGet/Put Vector of strings containing ver 2 obs types
          * (e.g. "C1" "L2") defined in reallyGet; also defined in
          * PrepareVer2Write() from R3 ObsIDs */
      StringVec R2ObsTypes;

         /** map between RINEX ver 3 ObsIDs and ver 2 obstypes for
          * each system: reallyPut */
      VersionObsMap mapSysR2toR3ObsID;

         /** map Sys + R2ot to their ObsID origins*/
      DisAmbMap R2DisambiguityMap;


      double version;                  ///< RINEX 3 version/type
      std::string fileType;            ///< RINEX 3 file type
         /// file sys char: RinexSatID system OR Mixed
      std::string fileSys;
         /** If false, the file type and system will be re-generated
          * in the gpstk preferred format when writing the header,
          * otherwise the strings fileType fileSys will be written
          * unaltered */
      bool preserveVerType;
      SatID fileSysSat;                ///< fileSys as a SatID
      std::string fileProgram;         ///< program used to generate file
      std::string fileAgency;          ///< who ran program
      std::string date;                ///< when program was run
         /** If false, the current system time will be used when
          * writing the header, otherwise the value in date will be
          * used. */
      bool preserveDate;
      StringVec commentList;           ///< comments in header
      std::string markerName;          ///< MARKER NAME
      std::string markerNumber;        ///< MARKER NUMBER
      std::string markerType;          ///< MARKER TYPE
      std::string observer;            ///< who collected the data
      std::string agency;              ///< observer's agency
      std::string recNo;               ///< receiver number
      std::string recType;             ///< receiver type
      std::string recVers;             ///< receiver version
      std::string antNo;               ///< antenna number
      std::string antType;             ///< antenna type
      gpstk::Triple antennaPosition;   ///< APPROX POSITION XYZ
      gpstk::Triple antennaDeltaHEN;   ///< ANTENNA: DELTA H/E/N
      gpstk::Triple antennaDeltaXYZ;   ///< ANTENNA: DELTA X/Y/Z
      std::string antennaSatSys;       ///< ANTENNA P.CTR BLOCK: SAT SYS
      std::string antennaObsCode;      ///< ANTENNA P.CTR BLOCK: OBS CODE
      gpstk::Triple antennaPhaseCtr;   ///< ANTENNA P.CTR BLOCK: PCTR POS
      gpstk::Triple antennaBsightXYZ;  ///< ANTENNA B.SIGHT XYZ
      double        antennaZeroDirAzi; ///< ANTENNA ZERODIR AZI
      gpstk::Triple antennaZeroDirXYZ; ///< ANTENNA ZERODIR XYZ
      gpstk::Triple centerOfMass;      ///< vehicle CENTER OF MASS: XYZ
      RinexObsMap mapObsTypes;         ///< SYS / # / OBS TYPES
      short wavelengthFactor[2];       ///< WAVELENGTH FACT (system-wide)
      FactorVector extraWaveFactList;  ///< WAVELENGTH FACT (per SV)
      std::string sigStrengthUnit;     ///< SIGNAL STRENGTH UNIT
      double interval;                 ///< INTERVAL
      CivilTime firstObs;              ///< TIME OF FIRST OBS
      CivilTime lastObs;               ///< TIME OF LAST OBS
      int receiverOffset;              ///< RCV CLOCK OFFS APPL
      CorrVec infoDCBS;                ///< DCBS INFO
      CorrVec infoPCVS;                ///< PCVS INFO
      SysScaleFacMap sysSfacMap;       ///< SYS / SCALE FACTOR
      SysPhsShftMap sysPhaseShift;     ///< SYS / PHASE SHIFT
      GLOFreqNumMap glonassFreqNo;     ///< GLONASS SLOT / FRQ #
      GLOCodPhsBias glonassCodPhsBias; ///< GLONASS COD/PHS/BIS
      int leapSeconds;                 ///< LEAP SECONDS
      short numSVs;                    ///< # OF SATELLITES
      PRNNumObsMap numObsForSat;       ///< PRN / # OF OBS


         /// number & types of observations R2 only
         ///@bug  this is being used but is not actually being filled
      RinexObsVec obsTypeList;

         /// bits set when header rec.s present & valid
      Fields valid;
         /// true if found END OF HEADER
      bool validEoH;
         /// Map P to Y code observations in RINEX 2 files
      bool PisY;

         /// Used to help debug this class
      static int debug;

         /// Destructor
      virtual ~Rinex3ObsHeader()
      {}

         // The next four lines comprise our common interface.

         /// Rinex3ObsHeader is a "header" so this function always
         /// returns true.
      virtual bool isHeader() const
      { return true; }

         /// This is a simple Debug output function.
         /// It simply outputs the version, name and antenna number of this
         /// RINEX header.
      virtual void dump(std::ostream& s) const;


         /** This method returns the numerical index of a given observation
          *
          * @param[in] type String representing the observation type.
          * @throw InvalidRequest
          */
      virtual std::size_t getObsIndex(const std::string& type ) const;

         /** This method returns the numerical index of a given observation
          *
          * @param[in] sys   GNSS system character for the obs
          * @param[in] obsID RinexObsID of the observation
          * @throw InvalidRequest
          */
      virtual std::size_t getObsIndex(const std::string& sys,
                                      const RinexObsID& obsID ) const;

         /** Parse a single header record, and modify valid
          * accordingly.  Used by reallyGetRecord for both
          * Rinex3ObsHeader and Rinex3ObsData. 
          * @throw FFStreamError
          */
      void parseHeaderRecord(std::string& line);

         /** Compute number of valid header records that
          * writeHeaderRecords() will write */
      int numberHeaderRecordsToBeWritten(void) const throw();

         /** Write all valid header records to the given stream.  Used
          * by reallyPutRecord for both Rinex3ObsHeader and
          * Rinex3ObsData.
          * @throw FFStreamError
          * @throw StringUtils::StringException
          */
      void writeHeaderRecords(FFStream& s) const;

         /// Return boolean : is this a valid Rinex header?
      bool isValid() const
      { return (validEoH && Fields::isValid(version, valid)); }

         /** Compute map of obs types for use in writing version 2
          * header and data */
      void prepareVer2Write(void);

         /** Compare this header with another.
          * @param[in] right the header to compare this with.
          * @param[out] diffs The header strings/identifiers that are
          *   different between this and right.
          * @param[in] inclExclList a list of header strings to be ignored
          *   or used when making the comparison (e.g. "RINEX VERSION / TYPE").
          * @param[in] incl When true, only header lines listed in
          *   inclExclList will be compared.  When false, differences in
          *   header lines in inclExclList will be ignored.
          * @return true if this and right match given the
          *   included/excluded headers. */
      bool compare(const Rinex3ObsHeader& right,
                   StringVec& diffs,
                   const StringVec& inclExclList,
                   bool incl = false);

         /// Return the RINEX header label for the given field enumeration.
      static std::string asString(Field b);
         /// Convert a RINEX header field label string into its matching enum.
      static Field asField(const std::string& s);

   protected:


         /** outputs this record to the stream correctly formatted.
          * @throw std::exception
          * @throw FFStreamError
          * @throw StringUtils::StringException
          */
      virtual void reallyPutRecord(FFStream& s) const;

         /** This function retrieves the RINEX Header from the given FFStream.
          * If an stream error is encountered, the stream is reset to its
          *  original position and its fail-bit is set.
          * @throw std::exception
          * @throw StringException when a StringUtils function fails
          * @throw FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position. */
      virtual void reallyGetRecord(FFStream& s);


         /** Helper methods
          * The conversion between RINEX v2.11 to RINEX v3 observation
          * type is fraught with system-specific idiosyncracies.   These 
          * methods read the list of v2.11 obs types stored in R2ObsTypes
          * and attempt to build a corresponding list of v3 observation
          * types where appropriate.
          * @throw FFStreamError */
      std::vector<RinexObsID> mapR2ObsToR3Obs_G();
      std::vector<RinexObsID> mapR2ObsToR3Obs_R();
      std::vector<RinexObsID> mapR2ObsToR3Obs_E();
      std::vector<RinexObsID> mapR2ObsToR3Obs_S();

      friend class Rinex3ObsData;


   private:

         /** Converts the daytime \a dt into a Rinex Obs time
          * string for the header */
      std::string writeTime(const CivilTime& civtime) const;

         /** This function sets the time for this header.
          * It looks at \a line to obtain the needed information. */
      CivilTime parseTime(const std::string& line) const;

         /// save ObsID for cont. "PHASE SHIFT" R3.01
      RinexObsID sysPhaseShiftObsID;
         /// save the syschar while reading ScaleFactor
      std::string satSysTemp;
         /// recall the prev sat. sys for continuation lines.
      std::string satSysPrev;
         /// save OBS # / TYPES and Sys / SCALE FACTOR for continuation lines.
      int numObs;
         /// recall the prev # obs for cont. lines
      int numObsPrev;
         /// save PRN while reading PRN/OBS cont. lines
      RinexSatID lastPRN;
         /// Scale factor holding data for continuation lines.
      int factor, factorPrev;

   }; // end class Rinex3ObsHeader

   std::ostream& operator<<(std::ostream& s, const Rinex3ObsHeader::Fields& v);

      //@}

} // namespace

#endif // GPSTK_RINEX3OBSHEADER_HPP
