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

/**
 * @file AntexStream.hpp
 * File stream for Rinex observation file data
 */

#ifndef GPSTK_ANTEX_STREAM_HPP
#define GPSTK_ANTEX_STREAM_HPP

#include <vector>
#include <list>
#include <map>

#include "FFTextStream.hpp"
#include "AntexHeader.hpp"

namespace gpstk
{
   /** @addtogroup Antex */
   //@{

      /// This class reads an Antex files.
      ///
      /// @sa gpstk::AntexData and gpstk::AntexHeader.
   class AntexStream : public FFTextStream
   {
   public:
         /// Default constructor
      AntexStream() : headerRead(false) {}
      
         /// Constructor given a file name
         /// @param fn the Antex file to open
         /// @param mode how to open \a fn.
      AntexStream(const char* fn, std::ios::openmode mode=std::ios::in)
            : FFTextStream(fn, mode), headerRead(false) {}

         /// Destructor
      virtual ~AntexStream() {}
      
         /// overrides open to reset the header
      virtual void open(const char* fn, std::ios::openmode mode)
      { 
         FFTextStream::open(fn, mode); 
         headerRead = false; 
         header = AntexHeader();
      }

         /// Whether or not the AntexHeader has been read
      bool headerRead;

         /// The header for this file.
      AntexHeader header;
   };

   //@}

} // namespace

#endif
