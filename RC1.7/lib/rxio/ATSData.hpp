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

#ifndef ATSData_HPP
#define ATSData_HPP

/**
 * @file ATSData.hpp
 * gpstk::ATSData - Class to contain a single ATS Binary data record.
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

#include <RinexObsData.hpp>

#if !defined( __SUNPRO_CC ) && !defined( _MSC_VER )
#include <stdint.h>
#endif


namespace gpstk
{

   class ATSStream;

   /// This class contains header information for all data types
   /// transmitted by receiver meeting the MDP specification.
   class ATSData : public FFData
   {
   public:
      ATSData() throw()
      	{}

      // Returns whether or not this MDPData is valid.
      bool isValid() const {return true;}

      //! This class is "data" so this function always returns "true".
      virtual bool isData() const {return true;}

      /// Simple accessors for various static thangs.
      virtual std::string getName() const {return "ats";}

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out, int detail=0) const throw();

      struct SubChannelBlock 
      {
         double pseudorange;
         double phase;
         double rangeRate;
         double cn0;
         // bit0: loss of lock, bit1: code tracking, bit2 carrier tracking
         // bit3: gps time, bit4: tbd, bits5-7: Data rate
         uint8_t flags;
         uint32_t navMSB[4], navLSB[4];
      };

      struct ChannelBlock
      {
         gpstk::SatID svid;
         double absTime; // seconds since the GPS epoch
         std::vector<SubChannelBlock> subChannels;
      };

      uint8_t numChan;
      uint8_t numSubChan;
      static const uint8_t MaxNumChan;
      static const uint8_t MaxNumSubChan;

      std::vector<ChannelBlock> channels;
         
      static int debugLevel;
      static bool hexDump;

   protected:
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, StringUtils::StringException, 
               FFStreamError);

      virtual void reallyGetRecord(FFStream& s)
         throw(std::exception, StringUtils::StringException, 
               FFStreamError, EndOfFile);
   }; // class ATSData

} // namespace gpstk

#endif // ATSData_HPP
