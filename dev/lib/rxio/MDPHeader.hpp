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

#ifndef MDPHEADER_HPP
#define MDPHEADER_HPP

/**
 * @file MDPHeader.hpp
 * gpstk::MDPHeader - base cass for MDP formatted data. Includes the MDP header.
 */

#include <map>
#include <iostream>

#include <Exception.hpp>
#include <DayTime.hpp>
#include <StringUtils.hpp>
#include <BinUtils.hpp>

#include <FFData.hpp>
#include <FFStream.hpp>
#include "DataStatus.hpp"

#include <RinexObsData.hpp>

#if !defined( __SUNPRO_CC ) && !defined( _MSC_VER )
#include <stdint.h>
#endif


namespace gpstk
{

   class MDPStream;

   /// This class contains header information for all data types
   /// transmitted by receiver meeting the MDP specification.
   class MDPHeader : public FFData, public CRCDataStatus
   {
   public:
      MDPHeader() throw() : id(0),length(0),crc(0)
      {length=myLength;}

      // Returns whether or not this MDPData is valid.
      bool isValid() const {return good();}

      //! This class is "data" so this function always returns "true".
      virtual bool isData() const {return true;}

      /** Encode this object to a string.
       * @return this object as a string.
       */
      virtual std::string encode() const throw();
         
      /** Decode this object from a string.
       * @param str the string to read from.
       * @note Decoded data is *NOT* removed from the string.
       */
      virtual void decode(std::string str) throw();

      /// Simple accessors for various static thangs.
      virtual std::string getName() const {return "hdr";}

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out) const throw();

      /// Compute the CRC over the string and set the CRC in that
      /// string and in the header.
      void setCRC(std::string& str) const throw(FFStreamError);
         
      /// Checks the crc of the message and set crc_err if it fails.
      void checkCRC(std::string str) throw();

      unsigned short id;              ///< Identifies this data's type.
      gpstk::DayTime time;            ///< The time associated with this message
      unsigned short freshnessCount;  ///< A message counter

      // These items need to be mutable so they can be set when an object
      // is sent to a stream.
      mutable unsigned short length;  ///< number of bytes in this message (includes the length of the header)
      mutable unsigned short crc;     ///< the 16 bit CCITT crc covering the header and body of the message

      static const unsigned myLength;  ///< = 16
      static const unsigned frameWord; ///< = 0x9c9c
      static int debugLevel;
      static bool hexDump;

      void readHeader(MDPStream& s)
         throw(FFStreamError, EndOfFile);

      std::string readBody(MDPStream& s)
         throw(FFStreamError, EndOfFile);

   protected:
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, StringUtils::StringException, 
               FFStreamError);

      virtual void reallyGetRecord(FFStream& s)
         throw(std::exception, StringUtils::StringException, 
               FFStreamError, EndOfFile);
   }; // class MDPHeader

} // namespace gpstk

#endif // MDPHEADER_HPP
