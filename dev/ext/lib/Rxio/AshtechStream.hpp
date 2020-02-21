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

#ifndef ASHTECHSTREAM_HPP
#define ASHTECHSTREAM_HPP

/**
 * @file AshtechStream.hpp
 * gpstk::AshtechStream - Decode data from Ashtech Z(Y)-12 recerivers. 
 */

#include "FFBinaryStream.hpp"

namespace gpstk
{
   /** 
    * This is a stream used to decode data in the Ashtech format.
    */
   class AshtechStream : public FFBinaryStream
   {
   public:
      AshtechStream():header(false) {}

      /**
       * @param fn the name of the Ashtech file to be opened
       * @param mode the ios::openmode to be used on \a fn
       */
      AshtechStream(const char* fn, std::ios::openmode mode = std::ios::in)
         : FFBinaryStream(fn, mode)
      {}

      /// destructor per the coding standards
      virtual ~AshtechStream()
      {}

      /// overrides open to reset the header
      virtual void open(const char* fn, std::ios::openmode mode = std::ios::in)
      {
         FFBinaryStream::open(fn, mode); 
      }

      /// The raw bytes read from the file.
      std::string rawData;

      // set true when a header was the last piece read, set false when the
      // a body is read.
      bool header;

      // Offset of the first character in rawData in the file
      std::streampos getRawPos()
      {
         std::streampos t = tellg();
         if (static_cast<long>(t)==-1)
            return -1;
         else
            return t - static_cast<std::streampos>(rawData.length());
      }

   }; // class AshtechStream
} // namespace gpstk

#endif
