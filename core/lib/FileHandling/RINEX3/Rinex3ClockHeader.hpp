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
 * @file Rinex3ClockHeader.hpp
 * Encapsulate header of Rinex3Clock file data, including I/O
 */

#ifndef RINEX3CLOCKHEADER_HPP
#define RINEX3CLOCKHEADER_HPP

#include <string>
#include <vector>
#include <map>
#include "Rinex3ClockBase.hpp"
#include "RinexSatID.hpp"
#include "RinexObsID.hpp"
#include "TimeSystem.hpp"

namespace gpstk
{
      /// @ingroup FileHandling
      //@{

      /// This class models the header for a RINEX Clock file.
      /// @sa gpstk::Rinex3ClockStream and gpstk::Rinex3ClockData for more information.
      /// @sa ?.cpp for an example.
   class Rinex3ClockHeader : public Rinex3ClockBase
   {
   public:

         /// constructor
      Rinex3ClockHeader() : version(3.0), leapSeconds(0), timeSystem(TimeSystem::Any),
                            numSolnStations(0), numSolnSatellites(0)
      {}

         /// @name RINEX Clock Header Formatting Strings
         //@{
      static const std::string versionString;         ///< "RINEX VERSION / TYPE"
      static const std::string runByString;           ///< "PGM / RUN BY / DATE"
      static const std::string commentString;         ///< "COMMENT"
      static const std::string sysString;             ///< "SYS / # / OBS TYPES"
      static const std::string timeSystemString;      ///< "TIME SYSTEM ID"
      static const std::string leapSecondsString;     ///< "LEAP SECONDS"
      static const std::string sysDCBString;          ///< "SYS / DCBS APPLIED"
      static const std::string sysPCVString;          ///< "SYS / PCVS APPLIED"
      static const std::string numDataString;         ///< "# / TYPES OF DATA"
      static const std::string stationNameString;     ///< "STATION NAME / NUM"
      static const std::string stationClockRefString; ///< "STATION CLK REF"
      static const std::string analysisCenterString;  ///< "ANALYSIS CENTER"
      static const std::string numClockRefString;     ///< "# OF CLK REF"
      static const std::string analysisClkRefrString; ///< "ANALYSIS CLK REF"
      static const std::string numReceiversString;    ///< "# OF SOLN STA / TRF"
      static const std::string solnStateString;       ///< "SOLN STA NAME / NUM"
      static const std::string numSolnSatsString;     ///< "# OF SOLN SATS"
      static const std::string prnListString;         ///< "PRN LIST"
      static const std::string endOfHeaderString;     ///< "END OF HEADER"
         //@}

         /// Validity bits for the RINEX Clock Header (** optional)
      enum validBits
      {
         versionValid            = 0x01, ///< "RINEX VERSION / TYPE"
         runByValid              = 0x02, ///< "PGM / RUN BY / DATE"
         commentValid            = 0x04, ///< "COMMENT" **
         sysValid                = 0x08, ///< "SYS / # / OBS TYPES" **

         timeSystemValid        = 0x010, ///< "TIME SYSTEM ID" **
         leapSecondsValid       = 0x020, ///< "LEAP SECONDS" **
         sysDCBValid            = 0x040, ///< "SYS / DCBS APPLIED" **
         sysPCVValid            = 0x080, ///< "SYS / PCVS APPLIED" **

         numDataValid          = 0x0100, ///< "# / TYPES OF DATA"
         stationNameValid      = 0x0200, ///< "STATION NAME / NUM" **
         stationClockRefValid  = 0x0400, ///< "STATION CLK REF" **
         analysisCenterValid   = 0x0800, ///< "ANALYSIS CENTER"

         numClockRefValid     = 0x01000, ///< "# OF CLK REF" **
         analysisClkRefrValid = 0x02000, ///< "ANALYSIS CLK REF" **
         numReceiversValid    = 0x04000, ///< "# OF SOLN STA / TRF"
         solnStateValid       = 0x08000, ///< "SOLN STA NAME / NUM"

         numSolnSatsValid    = 0x010000, ///< "# OF SOLN SATS"
         prnListValid        = 0x020000, ///< "PRN LIST"
         endOfHeaderValid    = 0x040000, ///< "END OF HEADER"

            //allRequiredValid    = 0x07F9A3, ///< this mask if for all required fields
            //allRequiredValid    = 0x07F903, ///< this mask if for all required fields
         allRequiredValid    = 0x07C903, ///< this mask if for all required fields
         allValid            = 0x07FFFF  ///< all the bits
      };

         /// destructor
      virtual ~Rinex3ClockHeader() {}

         // The next four lines is our common interface
         /// Rinex3ClockHeader is a "header" so this function always returns true.
      virtual bool isHeader() const { return true; }
     
         /** Dump information about the header to an ostream.
          * @param[in] os ostream to receive the output; defaults to std::cout
          * @param[in] detail integer level of detail to provide;
          *   allowed values are:
          *    0: all the header string except stations and satellites, but
          *       their num.
          *    1: above plus all the stations and satellites
          *    2: above plus all invalid header strings (dumpValid) */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
      virtual void dump(std::ostream& s=std::cout, short detail = 0) const throw();
#pragma clang diagnostic pop

         /// Dump validity bits -> header strings
      void dumpValid(std::ostream& s=std::cout) const throw();

      double version;                        ///< Rinex3Clock Version or file format
      std::string program;                   ///< Program name
      std::string runby;                     ///< Run by string
      std::string fileSys;                   ///< GNSS system OR Mixed
      std::vector<std::string> dataTypes;    ///< list of data types

      /// recall the prev sat. sys for continuation lines.
      std::string satSysPrev;
      /// save OBS # / TYPES and Sys / SCALE FACTOR for continuation lines.
      int numObs;
      typedef std::map<std::string, std::vector<RinexObsID> > RinexObsMap;
      RinexObsMap sysObsTypes;               ///< list of obs types used for each GNSS

      int leapSeconds;                       ///< Leap seconds
      TimeSystem timeSystem;                 ///< Time system

      std::string analCenterDesignator;      ///< Analysis center designator (3 char)
      std::string analysisCenter;            ///< Analysis center
      std::string terrRefFrame;              ///< Terr Ref Frame or SINEX solution
         // system letter, program name, and source of code bias corrections
      typedef std::pair<std::string,std::string> stringPair;
      std::map<std::string, stringPair > dcbsMap;
         // system letter, program name, and source of phase center corrections
      std::map<std::string, stringPair > pcvsMap;


      int numSolnStations;                   ///< Number of stations in the solution
      std::map<std::string,std::string> stationID; ///< 4-char name, station id
         // NB these coordinates are often more than 32 bits -- cannot store as number!
      std::map<std::string,std::string> stationX;  ///< name, station X coord in mm
      std::map<std::string,std::string> stationY;  ///< name, station Y coord in mm
      std::map<std::string,std::string> stationZ;  ///< name, station Z coord in mm

      int numSolnSatellites;                  ///< Number of satellites in the soln
      std::vector<RinexSatID> satList;        ///< List of sats (PRN LIST)

      std::vector<std::string> commentList;   ///< comments

      unsigned long valid;                    ///< valid bits for this header

   protected:
         /// clear out the member data
      void clear(void)
      {
         version = 3.0;
         program = std::string();
         runby = std::string();
         dataTypes.clear();
         leapSeconds = 0;
         analysisCenter = std::string();
         terrRefFrame = std::string();
         timeSystem = TimeSystem::Any;
         dcbsMap.clear();
         pcvsMap.clear();
         numSolnStations = 0;
         stationID.clear();
         stationX.clear();
         stationY.clear();
         stationZ.clear();
         numSolnSatellites = 0;
         satList.clear();
         commentList.clear();

         valid = 0;
      }

         /// Writes the record formatted to the FFStream \a s.
         /// @throws StringException when a StringUtils function fails
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
               StringUtils::StringException);

         /** This function retrieves the Rinex3Clock header from the
          * given FFStream.  If an error is encountered in the
          * retrieval of the header, the stream is reset to its
          * original position and its fail-bit is set.
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position. */
      virtual void reallyGetRecord(FFStream& s) 
         throw(std::exception, FFStreamError,
               StringUtils::StringException);

   }; // end class Rinex3ClockHeader

      //@}

}  // namespace

#endif // RINEX3CLOCKHEADER_HPP
