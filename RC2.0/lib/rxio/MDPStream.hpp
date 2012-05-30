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

#ifndef MDPSTREAM_HPP
#define MDPSTREAM_HPP

/**
 * @file MDPStream.hpp
 * gpstk::MDPStream - binary MDP file stream container.
 */

#include "FFBinaryStream.hpp"
#include "MDPHeader.hpp"

namespace gpstk
{
   /** 
    * This is a stream used to decode data in the MDP format.
    */
   class MDPStream : public FFBinaryStream
   {
   public:
      MDPStream()
         : streamState(outOfSync),
           headerCount(0)
      {}

         /**
          * @param fn the name of the MDP file to be opened
          * @param mode the ios::openmode to be used on \a fn
          */
      MDPStream(const char* fn,
                std::ios::openmode mode = std::ios::in)
         : FFBinaryStream(fn, mode), 
           streamState(outOfSync),
           headerCount(0)
      {}

      /// destructor per the coding standards
      virtual ~MDPStream()
      {}

      /// overrides open to reset the header
      virtual void open(const char* fn, std::ios::openmode mode = std::ios::in)
      {
         FFBinaryStream::open(fn, mode); 
         streamState = outOfSync;
      }

      /// Used to track what has been retrieved from the stream
      enum {outOfSync, gotHeader, gotBody} streamState;

      /// A copy of the most recent header read
      MDPHeader header;
      unsigned long headerCount;

      /// The raw bytes of the above header
      std::string rawHeader;
   }; // class MDPStream
} // namespace gpstk

#endif
