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

#ifndef ASHTECHDATA_HPP
#define ASHTECHDATA_HPP

/**
 * @file AshtechData.hpp
 * gpstk::AshtechData - base cass for Ashtech formatted data.
 */

#include <map>
#include <iostream>

#include <Exception.hpp>
#include <DayTime.hpp>
#include <StringUtils.hpp>
#include <BinUtils.hpp>

#include <FFData.hpp>
#include <FFStream.hpp>
#include <DataStatus.hpp>

namespace gpstk
{

   class AshtechStream;

   class AshtechData : public FFData, public CRCDataStatus
   {
   public:
      AshtechData() throw() {}

      // Returns whether or not this AshtechData is valid.
      bool isValid() const {return good();}

      //! This class is "data" so this function always returns "true".
      virtual bool isData() const {return true;}

      /** Encode this object to a string.
       * @return this object as a string.
       */
      virtual std::string encode() const throw() { return std::string(); }
         
      /** Decode this object from a string.
       * @param str the string to read from.
       */
      virtual void decode(const std::string& str)
         throw(std::exception, FFStreamError)
      {std::cout<<"AshtechData::decode()"<<std::endl;}

      /// Simple accessors for various static thangs.
      virtual std::string getName() const {return "hdr";}

      /// Returns true when the provided id is valid for this message
      virtual bool checkId(std::string hdrId) const {return false;}

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out) const throw();

      /// Compute the CRC over the string and set the CRC in that
      /// string and in the header.
      void setCRC(std::string& str) const throw() {}

      /// Checks the crc of the message and set crc_err if it fails.
      void checkCRC(std::string str) throw();

      static int debugLevel;
      static bool hexDump;

      std::string id;
      bool ascii;

      static const char* preamble;

   protected:
      virtual void reallyPutRecord(FFStream& ffs) const
         throw(std::exception, StringUtils::StringException, 
               FFStreamError)
      {
         gpstk::FFStreamError e("Writing of AshtecData is not supported.");
         GPSTK_THROW(e);
      }

      virtual void reallyGetRecord(FFStream& ffs)
         throw(std::exception, FFStreamError, EndOfFile);

      virtual void readHeader(AshtechStream& stream)
         throw(FFStreamError, EndOfFile);

      virtual void readBody(AshtechStream& stream)
         throw(FFStreamError, EndOfFile);
      
   }; // class AshtechData
} // namespace gpstk

#endif
