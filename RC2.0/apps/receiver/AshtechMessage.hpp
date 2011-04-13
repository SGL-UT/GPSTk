#pragma ident "$Id$"


/**
 * @file AshtechMessage.hpp
 * Containers for Ashtech data, conversions to RINEX - declarations.
 */

#ifndef GPSTK_ASHTECH_MESSAGE_HPP
#define GPSTK_ASHTECH_MESSAGE_HPP

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

#include <list>
#include <ostream>
#include "Exception.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"
#include "RinexObsData.hpp"

namespace gpstk
{
   /** @addtogroup AshtechReceiver */
   //@{

   /// Read Ashtech binary and ASCII messages and parse into Rinex. 
   /// Ref.  Ashtech XII GPS Receiver Operating Manual

   class AshtechMessage
   {
   public:

      /// Types of observations are categorized here
      enum ObsType
      {
         UnknownType=0, ///< Unknown observation type
         MCA,           ///< CA only observation (not implemented)
         MCL,           ///< C/A and L2 codeless observations 
         MP1,           ///< P code L1 only (not implemented)
         MP2,           ///< P code L2 only (not implemented)
         MPC,           ///< P code on L1 and L2
         PBEN,          ///< Navigation solution
         SNAV,          ///< Ephemeris data
         EPB,           ///< Ephemeris data
         SALM,          ///< Almanac data
         ALB,           ///< Almanac data
         ION            ///< Ionosphere model and UTC offset data
      };

      /// The source of a message affects its format
      enum ObsSource
      {
         UnknownSource=0, ///< Unknown source of observations
         DATAFILE,        ///< This data comes from a B- or E- file.
         STREAM           ///< This data comes from the serial port.
      };
      
      /// Some messages can be formatted in ASCII or in binary.
      enum ObsFormat
      {
         UnknownFormat=0, ///< Unknown format of observations
         BINARY,          ///< This data is packed, and little endian
         ASCII            ///< This data is comma separated
      };

         /// Default constructor
      AshtechMessage(void) : msgType(UnknownType), 
                             msgSource(UnknownSource),
                             msgFormat(UnknownFormat)
      {}

         /// Created from a input stream of characters.
      AshtechMessage(const std::string& ibuff, ObsSource src, 
                     ObsFormat fmt);

         /// Destructor
      virtual ~AshtechMessage(void) {}

         /// Access the type of this message
      ObsType getType(void) const {return msgType;}
      
         /// Access the epoch associated with the message
      DayTime getEpoch(const DayTime& priorEpoch) const;

        /// Access the sequence number (if applicable)      
      int getSequence(void) const;

        /// Access the PRN number (if applicable)      
      int getPRN(void) const;

        /// Access the tracker number (if applicable)      
      int getTracker(void) const;

         /// True if this record is an ephemeris record
      bool isAlm(void) const;

         /// True if this record is an ephemeris record
      bool isEph(void) const;

         /// True if this record is observation data
      bool isObs(void) const;

         /// True if this record is has ionosphere and time offset data
      bool isIon(void) const
         { return (msgType==ION); }

      int getSize(void) const
         { return buffer.size();}
      
           /// Dump the contents of the record to the ostream \c str.
//      virtual void dump(std::ostream& str) const;

         /**
          * Convert a list of AshtechMessages into a gpstk::RinexObsData. One record must
          * be a PBEN record.
          * @throw if the record is invalid or not an observation (isObs()==false)
          */
      static RinexObsData 
         convertToRinexObsData(
               const std::list<AshtechMessage> obsMsgs, 
               const DayTime& recentEpoch)
         throw(gpstk::Exception);

      static int calculateSequenceNumber(const DayTime& t);

      static RinexNavData convertToRinexNavData(const AshtechMessage& msg, const DayTime& epoch=DayTime());

      static void updateNavHeader(const AshtechMessage& ionMsg, RinexNavHeader& hdr);
      /**
       * Maps SNR to SSI. 
       * The RINEX 2.1 standard allows us to choose whatever mapping is 
       * appropriate, so long as "9" is "strongest" and "5" is usable.
       */
      static short mapSNRtoSSI(float snrL1);
      
   protected:

   private:
      
         /// private data members
      std::string buffer;
            
      ObsSource msgSource; ///< Does this come from a data file or was it streamed off of a serial port?
      ObsFormat msgFormat; ///< Is this an ASCII or a binary message?
         /// public data members
      ObsType   msgType;            ///< What kind of observations are in this message?

         /// Epoch time - may need to be set by the user
      DayTime time;

      static float engSNR(short cnt, float nominalbw);

   }; // end class AshtechData

   //@}

}  // end namespace gpstk

#endif // GPSTK_ASHTECH_MESSAGE_HPP
