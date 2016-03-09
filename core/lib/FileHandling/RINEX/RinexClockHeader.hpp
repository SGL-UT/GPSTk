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
//  Copyright 2015, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
// This software developed by Applied Research Laboratories at the
// University of Texas at Austin, under contract to an agency or
// agencies within the U.S.  Department of Defense. The
// U.S. Government retains all rights to use, duplicate, distribute,
// disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file RinexClockHeader.hpp
 * Encapsulate header of Rinex clock file, including I/O
 */

#ifndef GPSTK_RINEXCLOCKHEADER_HPP
#define GPSTK_RINEXCLOCKHEADER_HPP

#include <string>
#include <list>
#include "gpstkplatform.h"
#include "RinexClockBase.hpp"
#include "SatID.hpp"
#include "FFStream.hpp"
#include "StringUtils.hpp"


namespace gpstk
{
      /// @ingroup FileHandling
      //@{
   class RinexClockHeader : public RinexClockBase
   {
      
   public:
         /// A Simple Constructor.
      RinexClockHeader() : valid(0), version(2.00)
      {}
      
         /// Destructor
      virtual ~RinexClockHeader() {}
      
         /// @name RINEX Clock Header Formatting Strings
         //@{
      static const std::string versionString;        ///< "RINEX VERSION / TYPE"
      static const std::string runByString;          ///< "PGM / RUN BY / DATE"
      static const std::string commentString;        ///< "COMMENT"
      static const std::string leapSecondsString;    ///< "LEAP SECONDS"
      static const std::string dataTypesString;      ///< "# / TYPES OF DATA"
      static const std::string stationNameString;    ///< "STATION NAME / NUM"
      static const std::string calibrationClkString; ///< "STATION CLK REF"
      static const std::string acNameString;         ///< "ANALYSIS CENTER"
      static const std::string numRefClkString;      ///< "# OF CLK REF"
      static const std::string analysisClkRefString; ///< "ANALYSIS CLK REF"
      static const std::string numStationsString;    ///< "# OF SOLN STA / TRF"
      static const std::string solnStaNameString;    ///< "SOLN STA NAME / NUM"
      static const std::string numSatsString;        ///< "# OF SOLN SATS"
      static const std::string prnListString;        ///< "PRN LIST"
      static const std::string endOfHeader;          ///< "END OF HEADER"
         //@}
      
         /// Validity bits for the RINEX Clock Header
      enum validBits
      {
         versionValid = 0x01,             ///< "RINEX VERSION / TYPE"
         runByValid = 0x02,               ///< "PGM / RUN BY / DATE"
         commentValid = 0x04,             ///< "COMMENT"
         leapSecondsValid = 0x08,         ///< "LEAP SECONDS"
         dataTypesValid = 0x010,          ///< "# / TYPES OF DATA"
         stationNameValid = 0x020,        ///< "STATION NAME / NUM"
         calibrationClkValid = 0x040,     ///< "STATION CLK REF"
         acNameValid = 0x080,             ///< "ANALYSIS CENTER"
         numRefClkValid = 0x0100,         ///< "# OF CLK REF"
         numStationsValid = 0x0200,       ///< "# OF SOLN STA / TRF"
         solnStaNameValid = 0x0400,       ///< "SOLN STA NAME / NUM"
         numSatsValid = 0x00800,          ///< "# OF SOLN SATS"
         prnListValid = 0x01000,          ///< "PRN LIST"
         
         endValid =   0x080000000,        ///< "END OF HEADER"
         
         allValidAR = 0x080000797,
         allValidAS = 0x080001F97,
         allValidCR = 0x080000073,
         allValidDR = 0x080000033,
         allValidMS = 0x080000093
      };

      struct RefClk
      {
            /// name of the reciever or satellite used as a fixed reference 
            /// in data analysis
         std::string name;
            /// Unique identifier for reference clock (if a reciever), 
            /// preferably the DOMES number for fixed stations
         std::string number;
            /// Optional non-zero value for the apriori clock constraint
         double clkConstraint;
      };
      

      struct RefClkRecord
      {
            /// number of analysis clock references (satellite or reciever
            /// clocks) listed under "ANALYSIS CLK REF"
         int numClkRef;
            /// Start/Stop epochs (in GPS time) 
         CivilTime startEpoch;
         CivilTime stopEpoch;
            /// List of RefClks to appear as "ANALYSIS CLK REF"
         std::list<RefClk> clocks;
      };
      
      
      struct SolnSta
      {
            /// 4-character station/reciever name
         std::string name;
            /// Unique station/reciever identifier, preferably the DOMES number
            /// for fixed stations
         std::string number;
            /// Geocentric XYZ station coordinates corresponding to the 
            /// analysis clock values reported (in millimeters!)
         int64_t posX;
         int64_t posY;
         int64_t posZ;
      };

         /// Format version (2.00)
      double version;
         /// File type ("C" for Clock Data)
      std::string fileType;                  
         /// Name of program creating current file
      std::string fileProgram;
         /// Name of agency creating current file
      std::string fileAgency;
         /// Date of file creation, no specified format
      std::string date;
         /// Comments line(s)
      std::list<std::string> commentList;
         /// Leap second (optional)
      int leapSeconds;
         /// Number of different clock data types stored in the file
      int numType;
         /// List of clock data types
      std::list<RinexClkType> dataTypeList;  
         /// 4-character reciever name designator
      std::string stationName;               
         /// Unique reciever identifier, preferably the DOMES number 
         /// for fixed station
      std::string stationNumber;
         /// Unique identifier for external reference clock being used as 
         /// the standard for calibration
      std::string stationClkRef;             
         /// 3-character IGS AC designator
      std::string ac;                        
         /// Full name of Analysis Center
      std::string acName;
         /// List of RefClkRecords that make up the "# OF CLK REF" 
         /// "ANALYSIS CLK REF" groups
      std::list<RefClkRecord> refClkList;
         /// Number of recievers included in the clock data records 
         /// (including the analysis reference clock even if it has zero 
         /// values and is not given in the data records) 
      int numSta;
         /// Terrestrial reference frame or SINEX solution for the 
         /// station/reciever coordinates which match the clock solution
      std::string trf;
         /// List of each station/reciever included in the clock data records,
         /// as well as the analysis reference clock even if it has zero 
         /// values and is not included in the data records
      std::list<SolnSta> solnStaList;
         /// Number of different satellites in the clock data records and 
         /// listed in following header records
      int numSats;  
         /// List of PRNs
      std::list<SatID> prnList;           
         /// Bits set when individual header members are present and valid
      unsigned long valid;                

         /** RinexClockHeader is a "header" so this function always
          * returns true */
      virtual bool isHeader(void) const {return true;}
    
         /// A debug function that outputs the header to \a s.
      virtual void dump(std::ostream& s) const;

         /// Return boolean : is this a valid Rinex clock header?
      bool isValid() const;


   protected:
         /// outputs this record to the stream correctly formatted.
      virtual void reallyPutRecord(FFStream& s) const
         throw(std::exception, FFStreamError, StringUtils::StringException);
      
         /**
          * This function retrieves the RINEX Clock Header from the 
          * given FFStream.
          * If an stream error is encountered, the stream is reset to its
          * original position and its fail-bit is set.
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          * a read or formatting error occurs.  This also resets the
          * stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s)
         throw(std::exception, FFStreamError,StringUtils::StringException);

         /// Clears all header values and lists.
      void clear();


   private:
         /**
          * Parse a single header record, and modify valid accordingly.
          * Used by reallyGetRecord
          */
      void ParseHeaderRecord(const std::string& line)
         throw(FFStreamError);
    
   }; // RinexClockHeader

      //@}
  
}  // namespace

#endif
