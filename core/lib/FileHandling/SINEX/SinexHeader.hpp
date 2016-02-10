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
 * @file SinexHeader.hpp
 * Encapsulate header of SINEX file data, including I/O
 */

#ifndef GPSTK_SINEXHEADER_HPP
#define GPSTK_SINEXHEADER_HPP

#include <string>
#include "SinexBase.hpp"

namespace gpstk
{
   namespace Sinex
   {

         /// @ingroup FileHandling
         //@{

         /**
          * This class models the header for a SINEX file.
          *
          * @sa gpstk::SinexStream and gpstk::SinexData for more information.
          */
      class Header
      {
      public:

         static const size_t  MIN_LINE_LEN = 67;
         static const size_t  MAX_LINE_LEN = 79;

            /// Constructors
         Header() : version(VERSION) {};
         Header(const std::string& line) : version(VERSION)
         {
            *this = line;
         };

            /// Destructor
         virtual ~Header() {};

            /// String converters
         operator std::string() const;
         void operator=(const std::string& other);

            /// Debug output operator.
         void dump(std::ostream& s) const;

         float        version;         ///< Format version of the file ##.##.
         std::string  creationAgency;  ///< Agency creating the file
         Time         creationTime;    ///< Creation time
         std::string  dataAgency;      ///< Agency supplying data for the file
         Time         dataTimeStart;
         Time         dataTimeEnd;
         char         obsCode;
         uint32_t     paramCount;
         char         constraintCode;
         std::string  solutionTypes;   ///< Solution types in this file: S,O,E,T,C,A, or ' '
      }; // class Header

         //@}

   }  // namespace Sinex

}  // namespace gpstk

#endif // GPSTK_SINEXHEADER_HPP
