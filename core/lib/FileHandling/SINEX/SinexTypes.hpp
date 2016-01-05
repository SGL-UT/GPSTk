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
 * @file SinexData.hpp
 * Encapsulate SINEX data types, including I/O
 */

#ifndef GPSTK_SINEXTYPES_HPP
#define GPSTK_SINEXTYPES_HPP

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

#include "Triple.hpp"
#include "SinexBase.hpp"
#include "SinexHeader.hpp"  /// For Sinex::InputHistory

namespace gpstk
{
   namespace Sinex
   {
         /// @ingroup FileHandling
         //@{

         /**
          * Base class for SINEX block-level data types
          */
      struct DataType
      {

            /// Destructor
         virtual ~DataType() {}

            /// String converters
         virtual operator std::string() const = 0;
         virtual void operator=(const std::string& line) = 0;

            /// Debug output function.
         virtual void dump(std::ostream& s) const {}

      }; // struct DataType


         /**
          * SINEX File Reference data type and methods
          */
      struct FileReference : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 21;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         FileReference() {}
         FileReference(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~FileReference() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  infoType;  ///< Type of information present in next field
         std::string  infoValue; ///< Information of type previously indicated

      }; // struct FileReference


         /**
          * SINEX File Comment data type and methods
          */
      struct FileComment : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 1;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         FileComment() {}
         FileComment(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~FileComment() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  comment;

      }; // struct FileComment


         /**
          * SINEX Input History data type and methods;
          * leverages Sinex::Header functionality
          */
      struct InputHistory : public DataType, Header
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 67;
         static const size_t       MAX_LINE_LEN = 79;

            /// Constructors
         InputHistory() {}
         InputHistory(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~InputHistory() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         char  fileCode;   /// '+' or '='

      }; // struct InputHistory


         /**
          * SINEX Input File data type and methods
          */
      struct InputFile : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 49;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         InputFile() {}
         InputFile(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~InputFile() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  agencyCode;
         Time         creationTime;
         std::string  fileName;
         std::string  fileDesc;

      }; // struct InputFile


         /**
          * SINEX Input Acknowledgment data type and methods
          */
      struct InputAck : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 6;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         InputAck() {}
         InputAck(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~InputAck() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  agencyCode;
         std::string  agencyDesc;

      }; // struct InputAck


         /**
          * SINEX Nutation data type and methods
          */
      struct NutationData : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 11;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         NutationData() {}
         NutationData(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~NutationData() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  nutationCode;
         std::string  nutationDesc;

      }; // struct NutationData


         /**
          * SINEX Precession data type and methods
          */
      struct PrecessionData : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 11;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         PrecessionData() {}
         PrecessionData(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~PrecessionData() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  precessionCode;
         std::string  precessionDesc;

      }; // struct PrecessionData


         /**
          * SINEX Source ID data type and methods
          */
      struct SourceId : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 33;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         SourceId() {}
         SourceId(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SourceId() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  sourceCode;
         std::string  iers;
         std::string  icrf;
         std::string  comment;

      }; // struct SourceId


         /**
          * SINEX Site ID data type and methods
          */
      struct SiteId : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 75;
         static const size_t       MAX_LINE_LEN = 75;

            /// Constructors
         SiteId() {}
         SiteId(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SiteId() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  siteCode;      ///< Call sign for a site
         std::string  pointCode;     ///< Physical monument used at a site
         std::string  monumentId;    ///< Unique monument ID
         char         obsCode;       ///< Observation technique(s): C, D, L, M, P, or R
         std::string  siteDesc;      ///< Site description, typically town and/or country
         uint16_t     longitudeDeg;  ///< Longitude degrees East (positive)
         uint8_t      longitudeMin;  ///< Longitude minutes
         float        longitudeSec;  ///< Longitude seconds
         int16_t      latitudeDeg;   ///< Latitude degrees North/South (+/-)
         uint8_t      latitudeMin;   ///< Latitude minutes
         float        latitudeSec;   ///< Latitude seconds
         double       height;        ///< Site height in meters

      }; // struct SiteId


         /**
          * SINEX Site Data data type and methods
          */
      struct SiteData : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 71;
         static const size_t       MAX_LINE_LEN = 71;

            /// Constructors
         SiteData() {}
         SiteData(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SiteData() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  siteCodeSol;    ///< Site code for solved station coords
         std::string  pointCodeSol;   ///< Point code for solved station coords
         std::string  solutionIdSol;  ///< Solution number for solved station coords
         std::string  siteCodeInp;    ///< Site code from an input SINEX file
         std::string  pointCodeInp;   ///< Point code from an input SINEX file
         std::string  solutionIdInp;  ///< Solution number from an input SINEX file
         char         obsCode;        ///< Observation technique(s): C, D, L, M, P, or R
         Time         timeStart;      ///< Start time of data for the input SINEX file
         Time         timeEnd;        ///< End time of data for the input SINEX file
         std::string  agencyCode;     ///< Creation agency code for the input SINEX file
         Time         creationTime;   ///< Creation time for the input SINEX file

      }; // struct SiteData


         /**
          * SINEX Site Receiver data type and methods
          */
      struct SiteReceiver : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 70;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         SiteReceiver() {}
         SiteReceiver(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SiteReceiver() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  siteCode;      ///< Call sign for a site
         std::string  pointCode;     ///< Physical monument used at a site
         std::string  solutionId;    ///< Solution number at a site
         char         obsCode;       ///< Observation technique(s): C, D, L, M, P, or R
         Time         timeSince;
         Time         timeUntil;
         std::string  rxType;        ///< Receiver name and model
         std::string  rxSerialNo;    ///< Receiver serial number
         std::string  rxFirmware;    ///< Receiver firmware

      }; // struct SiteReceiver


         /**
          * SINEX Site Antenna data type and methods
          */
      struct SiteAntenna : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 64;
         static const size_t       MAX_LINE_LEN = 68;

            /// Constructor.
         SiteAntenna() {}
         SiteAntenna(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SiteAntenna() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  siteCode;         ///< Call sign for a site
         std::string  pointCode;        ///< Physical monument used at a site
         std::string  solutionId;       ///< Solution number at a site
         char         obsCode;          ///< Observation technique(s): C, D, L, M, P, or R
         Time         timeSince;
         Time         timeUntil;
         std::string  antennaType;      ///< Antenna name and model
         std::string  antennaSerialNo;  ///< Receiver serial number

      }; // struct SiteAntenna


         /**
          * SINEX Site Phase Center base class; use a class derived from this
          */
      struct SitePhaseCenter : public DataType
      {
         static const size_t       MIN_LINE_LEN = 71;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         SitePhaseCenter() {}
         SitePhaseCenter(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SitePhaseCenter() {}

            /// String converters
         virtual operator std::string() const;
         virtual void operator=(const std::string& line);

            /// Debug output function.
         virtual void dump(std::ostream& s) const;

         std::string  antennaType;         ///< Antenna name and model
         std::string  antennaSerialNo;     ///< Antenna serial number
         Triple       offsetA;             ///< First antenna offset
         Triple       offsetB;             ///< Second antenna offset
         std::string  antennaCalibration;  ///< Antenna calibration model

      }; // struct SitePhaseCenter


         /**
          * SINEX Site GPS Phase Center data type and methods
          */
      struct SiteGpsPhaseCenter : public SitePhaseCenter
      {
         static const std::string  BLOCK_TITLE;

         enum Frequencies
         {
            L1L2 = 0   /// offsetA = L1, offsetB = L2

         } frequencies;

            /// Constructors
         SiteGpsPhaseCenter()
               : frequencies(L1L2) {}
         SiteGpsPhaseCenter(const std::string& line, size_t num = 0)
               : SitePhaseCenter(line), frequencies( (Frequencies)(num % 1) ) {}

            /// Destructor
         virtual ~SiteGpsPhaseCenter() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      }; // struct SiteGpsPhaseCenter


         /**
          * SINEX Site Galileo Phase Center data type and methods
          */
      struct SiteGalPhaseCenter : public SitePhaseCenter
      {
         static const std::string  BLOCK_TITLE;

         enum Frequencies
         {
            L1L5 = 0,  /// offsetA = L1, offsetB = L5
            L6L7,      /// offsetA = L6, offsetB = L7
            L8x        /// offsetA = L8, offsetB = reserved

         } frequencies;

            /// Constructors
         SiteGalPhaseCenter()
               : frequencies(L1L5) {}
         SiteGalPhaseCenter(const std::string& line, size_t num = 0)
               : SitePhaseCenter(line), frequencies( (Frequencies)(num % 3) ) {}

            /// Destructor
         virtual ~SiteGalPhaseCenter() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      }; // struct SiteGalPhaseCenter


         /**
          * SINEX Site Eccentricity data type and methods
          */
      struct SiteEccentricity : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 72;
         static const size_t       MAX_LINE_LEN = 72;

            /// Constructors
         SiteEccentricity() {}
         SiteEccentricity(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SiteEccentricity() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  siteCode;     ///< Call sign for a site
         std::string  pointCode;    ///< Physical monument used at a site
         std::string  solutionId;   ///< Solution number at a site
         char         obsCode;      ///< Observation technique(s): C, D, L, M, P, or R
         Time         timeSince;
         Time         timeUntil;
         std::string  refSystem;    ///< UNE / XYZ
         Triple       eccentricity; ///< UNE / XYZ offset from marker to antenna (m)

      }; // struct SiteEccentricity


         /**
          * SINEX Satellite ID data type and methods
          */
      struct SatelliteId : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 48;
         static const size_t       MAX_LINE_LEN = 67;

            /// Constructors
         SatelliteId() {}
         SatelliteId(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SatelliteId() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  svCode;       ///< Satellite code "CNNN": C=GNSS code, NNN=SVN/GLONASS Number
         std::string  prn;          ///< Pseudorandom noise number (GPS/Galileo) or slot number (GLONASS)
         std::string  cosparId;     ///< NASA-assigned ID "YYYY-XXXA": YYYY=Year, XXX=sequence, A=launch
         char         obsCode;      ///< Observation technique(s): C, D, L, M, P, or R
         Time         timeSince;
         Time         timeUntil;
         std::string  antennaType;  ///< Strict IGS rcvr_ant.tab satellite antenna name

      }; // struct SatelliteId


         /**
          * SINEX Satellite Phase Center data type and methods
          */
      struct SatellitePhaseCenter : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 66;
         static const size_t       MAX_LINE_LEN = 66;

            /// Constructors
         SatellitePhaseCenter() {}
         SatellitePhaseCenter(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SatellitePhaseCenter() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  svCode;     ///< Satellite code "CNNN": C=GNSS code, NNN=SVN/GLONASS Number
         char         freqCodeA;  ///< Frequency for which the phase center offset is given
         Triple       offsetA;    ///< Offset from CM to phase center (x, y, z)
         char         freqCodeB;  ///< Frequency for which the phase center offset is given
         Triple       offsetB;    ///< Offset from CM to phase center (x, y, z)
         std::string  antennaCalibration;  ///< Antenna calibration model
         char         pcvType;    ///< Phase center variation type: A, R
         char         pcvModel;   ///< Phase center variation model application: F, E

      }; // struct SatellitePhaseCenter


         /**
          * SINEX Bias Epoch data type and methods
          */
      struct BiasEpoch : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 54;
         static const size_t       MAX_LINE_LEN = 54;

            /// Constructors
         BiasEpoch() {}
         BiasEpoch(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~BiasEpoch() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  siteCode;    ///< Call sign for a site
         std::string  pointCode;   ///< Physical monument used at a site
         std::string  solutionId;  ///< Solution number at a site
         char         biasType;    ///< Bias Type: R, T, S, Z
         Time         firstTime;
         Time         lastTime;
         Time         meanTime;

      }; // struct BiasEpoch


         /**
          * SINEX Solution Statistics data type and methods
          */
      struct SolutionStatistics : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 54;
         static const size_t       MAX_LINE_LEN = 54;

            /// Constructors
         SolutionStatistics() {}
         SolutionStatistics(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SolutionStatistics() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  infoType;   ///< Type of information present in next field
         long double  infoValue;  ///< Information of type previously indicated

      }; // struct SolutionStatistics


         /**
          * SINEX Solution Epoch data type and methods
          */
      struct SolutionEpoch : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 54;
         static const size_t       MAX_LINE_LEN = 54;

            /// Constructors
         SolutionEpoch() {}
         SolutionEpoch(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SolutionEpoch() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         std::string  siteCode;    ///< Call sign for a site
         std::string  pointCode;   ///< Physical monument used at a site
         std::string  solutionId;  ///< Solution number at a site
         char         obsCode;     ///< Observation technique(s): C, D, L, M, P, or R
         Time         startTime;
         Time         endTime;
         Time         meanTime;

      }; // struct SolutionEpoch


         /**
          * SINEX Solution Estimate data type and methods
          */
      struct SolutionEstimate : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 80;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         SolutionEstimate() {}
         SolutionEstimate(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SolutionEstimate() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         uint32_t     paramIndex;
         std::string  paramType;
         std::string  siteCode;    ///< Call sign for a site
         std::string  pointCode;   ///< Physical monument used at a site
         std::string  solutionId;  ///< Solution number at a site
         Time         epoch;
         std::string  paramUnits;
         char         constraintCode;
         long double  paramEstimate;
         double       paramStdDev;

      }; // struct SolutionEstimate


         /**
          * SINEX Solution Apriori data type and methods
          */
      struct SolutionApriori : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 80;
         static const size_t       MAX_LINE_LEN = 80;

            /// Constructors
         SolutionApriori() {}
         SolutionApriori(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SolutionApriori() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         uint32_t     paramIndex;
         std::string  paramType;
         std::string  siteCode;    ///< Call sign for a site
         std::string  pointCode;   ///< Physical monument used at a site
         std::string  solutionId;  ///< Solution number at a site
         Time         epoch;
         std::string  paramUnits;
         char         constraintCode;
         long double  paramApriori;
         double       paramStdDev;

      }; // struct SolutionApriori


         /**
          * Base class for SINEX Solution Matrix Estimate data type
          * and methods
          */
      struct SolutionMatrixEstimate : public DataType
      {
         static const size_t       MIN_LINE_LEN = 78;
         static const size_t       MAX_LINE_LEN = 78;

            /// Constructor.
         SolutionMatrixEstimate() {}
         SolutionMatrixEstimate(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SolutionMatrixEstimate() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         uint32_t     row;
         uint32_t     col;
         long double  val1;
         long double  val2;
         long double  val3;

      }; // struct SolutionMatrixEstimate


         /**
          * SINEX Solution Matrix Estimate L CORR data type and methods
          */
      struct SolutionMatrixEstimateLCorr : public SolutionMatrixEstimate
      {
         static const std::string  BLOCK_TITLE;

            /// Constructor.
         SolutionMatrixEstimateLCorr()
               : SolutionMatrixEstimate() {}
         SolutionMatrixEstimateLCorr(const std::string& line, size_t num = 0)
               : SolutionMatrixEstimate(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixEstimateLCorr() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Estimate U CORR data type and methods
          */
      struct SolutionMatrixEstimateUCorr : public SolutionMatrixEstimate
      {
         static const std::string  BLOCK_TITLE;

            /// Constructor.
         SolutionMatrixEstimateUCorr()
               : SolutionMatrixEstimate() {}
         SolutionMatrixEstimateUCorr(const std::string& line, size_t num = 0)
               : SolutionMatrixEstimate(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixEstimateUCorr() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Estimate L COVA data type and methods
          */
      struct SolutionMatrixEstimateLCova : public SolutionMatrixEstimate
      {
         static const std::string  BLOCK_TITLE;

            /// Constructor.
         SolutionMatrixEstimateLCova()
               : SolutionMatrixEstimate() {}
         SolutionMatrixEstimateLCova(const std::string& line, size_t num = 0)
               : SolutionMatrixEstimate(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixEstimateLCova() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Estimate U COVA data type and methods
          */
      struct SolutionMatrixEstimateUCova : public SolutionMatrixEstimate
      {
         static const std::string  BLOCK_TITLE;

            /// Constructor.
         SolutionMatrixEstimateUCova()
               : SolutionMatrixEstimate() {}
         SolutionMatrixEstimateUCova(const std::string& line, size_t num = 0)
               : SolutionMatrixEstimate(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixEstimateUCova() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Estimate L INFO data type and methods
          */
      struct SolutionMatrixEstimateLInfo : public SolutionMatrixEstimate
      {
         static const std::string  BLOCK_TITLE;

            /// Constructor.
         SolutionMatrixEstimateLInfo()
               : SolutionMatrixEstimate() {}
         SolutionMatrixEstimateLInfo(const std::string& line, size_t num = 0)
               : SolutionMatrixEstimate(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixEstimateLInfo() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Estimate U INFO data type and methods
          */
      struct SolutionMatrixEstimateUInfo : public SolutionMatrixEstimate
      {
         static const std::string  BLOCK_TITLE;

            /// Constructor.
         SolutionMatrixEstimateUInfo()
               : SolutionMatrixEstimate() {}
         SolutionMatrixEstimateUInfo(const std::string& line, size_t num = 0)
               : SolutionMatrixEstimate(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixEstimateUInfo() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * Base class for SINEX Solution Matrix Apriori data type
          * and methods
          */
      struct SolutionMatrixApriori : public DataType
      {
         static const size_t       MIN_LINE_LEN = 78;
         static const size_t       MAX_LINE_LEN = 78;

            /// Constructors
         SolutionMatrixApriori() {}
         SolutionMatrixApriori(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SolutionMatrixApriori() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         uint32_t     row;
         uint32_t     col;
         long double  val1;
         long double  val2;
         long double  val3;

      }; // struct SolutionMatrixApriori


         /**
          * SINEX Solution Matrix Apriori L CORR data type and methods
          */
      struct SolutionMatrixAprioriLCorr : public SolutionMatrixApriori
      {
         static const std::string  BLOCK_TITLE;

            /// Constructors
         SolutionMatrixAprioriLCorr()
               : SolutionMatrixApriori() {}
         SolutionMatrixAprioriLCorr(const std::string& line, size_t num = 0)
               : SolutionMatrixApriori(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixAprioriLCorr() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Apriori U CORR data type and methods
          */
      struct SolutionMatrixAprioriUCorr : public SolutionMatrixApriori
      {
         static const std::string  BLOCK_TITLE;

            /// Constructors
         SolutionMatrixAprioriUCorr()
               : SolutionMatrixApriori() {}
         SolutionMatrixAprioriUCorr(const std::string& line, size_t num = 0)
               : SolutionMatrixApriori(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixAprioriUCorr() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Apriori L COVA data type and methods
          */
      struct SolutionMatrixAprioriLCova : public SolutionMatrixApriori
      {
         static const std::string  BLOCK_TITLE;

            /// Constructors
         SolutionMatrixAprioriLCova()
               : SolutionMatrixApriori() {}
         SolutionMatrixAprioriLCova(const std::string& line, size_t num = 0)
               : SolutionMatrixApriori(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixAprioriLCova() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Apriori U COVA data type and methods
          */
      struct SolutionMatrixAprioriUCova : public SolutionMatrixApriori
      {
         static const std::string  BLOCK_TITLE;

            /// Constructors
         SolutionMatrixAprioriUCova()
               : SolutionMatrixApriori() {}
         SolutionMatrixAprioriUCova(const std::string& line, size_t num = 0)
               : SolutionMatrixApriori(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixAprioriUCova() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Apriori L INFO data type and methods
          */
      struct SolutionMatrixAprioriLInfo : public SolutionMatrixApriori
      {
         static const std::string  BLOCK_TITLE;

            /// Constructors
         SolutionMatrixAprioriLInfo()
               : SolutionMatrixApriori() {}
         SolutionMatrixAprioriLInfo(const std::string& line, size_t num = 0)
               : SolutionMatrixApriori(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixAprioriLInfo() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Matrix Apriori U INFO data type and methods
          */
      struct SolutionMatrixAprioriUInfo : public SolutionMatrixApriori
      {
         static const std::string  BLOCK_TITLE;

            /// Constructors
         SolutionMatrixAprioriUInfo()
               : SolutionMatrixApriori() {}
         SolutionMatrixAprioriUInfo(const std::string& line, size_t num = 0)
               : SolutionMatrixApriori(line, num) {}

            /// Destructor
         virtual ~SolutionMatrixAprioriUInfo() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Normal Equation Vector data type and methods
          */
      struct SolutionNormalEquationVector : public DataType
      {
         static const std::string  BLOCK_TITLE;
         static const size_t       MIN_LINE_LEN = 68;
         static const size_t       MAX_LINE_LEN = 68;

            /// Constructors
         SolutionNormalEquationVector() {}
         SolutionNormalEquationVector(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SolutionNormalEquationVector() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         uint32_t     paramIndex;
         std::string  paramType;
         std::string  siteCode;       ///< Call sign for a site
         std::string  pointCode;      ///< Physical monument used at a site
         std::string  solutionId;     ///< Solution number at a site
         Time         epoch;
         std::string  paramUnits;
         char         constraintCode;
         long double  value;          ///< Right hand side of the normal equation

      }; // struct SolutionNormalEquationVector


         /**
          * Base class for SINEX Solution Normal Equation Matrix data type
          * and methods
          */
      struct SolutionNormalEquationMatrix : public DataType
      {
         static const size_t       MIN_LINE_LEN = 78;
         static const size_t       MAX_LINE_LEN = 78;

            /// Constructors
         SolutionNormalEquationMatrix() {}
         SolutionNormalEquationMatrix(const std::string& line, size_t num = 0)
         { *this = line; }

            /// Destructor
         virtual ~SolutionNormalEquationMatrix() {}

            /// String converters
         operator std::string() const;
         void operator=(const std::string& line);

            /// Debug output function.
         void dump(std::ostream& s) const;

         uint32_t     row;
         uint32_t     col;
         long double  val1;
         long double  val2;
         long double  val3;

      }; // struct SolutionNormalEquationMatrix


         /**
          * SINEX Solution Normal Equation Matrix L data type and methods
          */
      struct SolutionNormalEquationMatrixL : public SolutionNormalEquationMatrix
      {
         static const std::string  BLOCK_TITLE;

            /// Constructors
         SolutionNormalEquationMatrixL()
               : SolutionNormalEquationMatrix() {}

         SolutionNormalEquationMatrixL(const std::string& line, size_t num = 0)
               : SolutionNormalEquationMatrix(line, num) {}

            /// Destructor
         virtual ~SolutionNormalEquationMatrixL() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };


         /**
          * SINEX Solution Normal Equation Matrix U data type and methods
          */
      struct SolutionNormalEquationMatrixU : public SolutionNormalEquationMatrix
      {
         static const std::string  BLOCK_TITLE;

            /// Constructors
         SolutionNormalEquationMatrixU()
               : SolutionNormalEquationMatrix() {}
         SolutionNormalEquationMatrixU(const std::string& line, size_t num = 0)
               : SolutionNormalEquationMatrix(line, num) {}

            /// Destructor
         virtual ~SolutionNormalEquationMatrixU() {}

            /// Debug output function.
         void dump(std::ostream& s) const;

      };

         //@}

   }  // namespace Sinex

}  // namespace gpstk

#endif // GPSTK_SINEXTYPES_HPP
