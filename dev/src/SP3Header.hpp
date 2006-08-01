#pragma ident "$Id$"


/**
 * @file SP3Header.hpp
 * Encapsulate header of SP3 file data, including I/O
 */

#ifndef GPSTK_SP3HEADER_HPP
#define GPSTK_SP3HEADER_HPP

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






#include <string>
#include <map>
#include <vector>
#include "DayTime.hpp"
#include "SP3Base.hpp"

namespace gpstk
{
   /** @addtogroup SP3ephem */
   //@{

      /**
       * This class models the header for a SP3 file.
       *
       * @note A valid header does not need to be read before 
       *  data can be read from an sp3 file because a large part of the
       *  SP3 file header is skipped and not processed.
       *
       * @sa gpstk::SP3Stream and gpstk::SP3Data for more information.
       * @sa sp3_test.cpp for an example.
       */
   class SP3Header : public SP3Base
   {
   public:
         /// constructor
      SP3Header() : version(0), numberOfEpochs(0) {}

         /// destructor
      virtual ~SP3Header() {}

         // The next four lines is our common interface
         /// SP3Header is a "header" so this function always returns true.
      virtual bool isHeader() const {return true;}
     
         /// Debug output operator.
      virtual void dump(std::ostream& s) const;

      char   version;           ///< Version of the SP3 File.
      char   pvFlag;            ///< File contains positions (P) or positions and velocities (V)
      DayTime time;             ///< Time in header (year, month, dom, hour, min, sec, week, sow)
      double epochInterval;     ///< Duration of Epoch in seconds
      int    numberOfEpochs;    ///< Number of Epochs in this file
      std::string dataUsed;     ///< Types of data input into the positions
      std::string coordSystem;  ///< Coordinate System of the data
      std::string orbitType;    ///< Type of Orbit Estimate
      std::string agency;       ///< Agency generating the Orbit

      std::map<short, short> svList;     ///< Which SVs are present in the data and their accuracy flags
      std::vector<std::string> comments; ///< vector of 4 comment lines

   protected:
         /// Writes the record formatted to the FFStream \a s.
         /// @warning This function is currently unimplemented.
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);

         /** 
          * This function retrieves the SP3 header from the given FFStream.
          * If an error is encountered in the retrieval of the header, the
          * stream is reset to its original position and its fail-bit is set.
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s) 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);
   };

   //@}

}  // namespace

#endif

