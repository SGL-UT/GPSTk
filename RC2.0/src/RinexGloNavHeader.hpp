#pragma ident "$Id"

/**
 * @file RinexGloNavHeader.hpp
 * Encapsulate header of Rinex GLONASS navigation file
 */

#ifndef GPSTK_RINEXGLONAVHEADER_HPP
#define GPSTK_RINEXGLONAVHEADER_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011
//
//============================================================================


#include "FFStream.hpp"
#include "RinexNavBase.hpp"

namespace gpstk
{
   /** @addtogroup RinexNav */

   //@{

      /**
       * This class models the RINEX NAV header for a RINEX GLONASS NAV file.
       *
       * \sa gpstk::RinexGloNavData and gpstk::RinexNavStream classes.
       *
       * \warning Currently this class is only valid for Rinex 2.1 files.
       */
   class RinexGloNavHeader : public RinexNavBase
   {
   public:

         /// Constructor
      RinexGloNavHeader(void) : valid(0), version(2.1) {}
      
         /// Destructor
      virtual ~RinexGloNavHeader() {}

         /// RinexGloNavHeader is a "header" so this method always returns true.
      virtual bool isHeader(void) const {return true;}

         /// This function dumps the contents of the header.
      virtual void dump(std::ostream& s) const;

         /// Tell me, Am I valid?
      unsigned long valid;

         /// Validity bits used in checking the RINEX GLONASS NAV header.
      enum validBits
      {
         versionValid = 0x01,        ///< Set if the RINEX version is valid.
         runByValid = 0x02,          ///< Set if the Run-by value is valid.
            /// Set if the comments are valid. Very subjective
         commentValid = 0x04,
            /// Set if the correction to system time scale is valid.
         corrToSystemTimeValid = 0x08,
         leapSecondsValid = 0x040,   ///< Set if the Leap Seconds value is valid.
         endValid = 0x080000000,           ///< Set if the end value is valid.

            /// This bitset checks that all required header items are available
            /// for a Rinex 2.0 version file
         allValid20 = 0x080000003,
            /// This bitset checks that all required header items are available
            /// for a Rinex 2.1 version file
         allValid21 = 0x080000005,
            /// This bitset checks that all required header items are available
            /// for a Rinex 2.11 version file
         allValid211 = 0x080000007
      };

         /** @name HeaderValues
          */
         //@{
      double version;           ///< RINEX Version

      std::string fileType;            
      std::string fileProgram;
      std::string fileAgency;
      std::string date;
      std::vector<std::string> commentList;
      int yearRefTime;
      int monthRefTime;
      int dayRefTime;
      double minusTauC;
      long leapSeconds;
         //@}

         /** @name FormattingStd::Strings
          */
         //@{
      static const std::string versionString; //"RINEX VERSION / TYPE"
      static const std::string runByString; //"PGM / RUN BY / DATE"
      static const std::string commentString; // "COMMENT"
      static const std::string corrToSystemTime; // "CORR TO SYSTEM TIME"
      static const std::string leapSecondsString; //"LEAP SECONDS"
      static const std::string endOfHeader;  //"END OF HEADER"
         //@}


   protected:
      

         /// Writes a correctly formatted record from this data to stream \a s.
      virtual void reallyPutRecord(FFStream& s) const
         throw( std::exception, FFStreamError, 
                gpstk::StringUtils::StringException );

         /**
          * This function reads the RINEX GLONASS NAV header from the given
          * FFStream. If an error is encountered in reading from the stream,
          * the stream is reset to its original position and fail-bit is set.
          * 
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s) 
         throw( std::exception, FFStreamError, 
                gpstk::StringUtils::StringException );
         
   }; // End of class 'RinexGloNavHeader'

   //@}

}  // End of namespace gpstk

#endif   // GPSTK_RINEXGLONAVHEADER_HPP
