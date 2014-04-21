#pragma ident "$Id$"

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
 * @file RinexClockStream.hpp
 * gpstk::RinexClockStream - RINEX Clock format file stream
 */

#ifndef RINEX_CLOCK_STREAM_INCLUDE
#define RINEX_CLOCK_STREAM_INCLUDE

#include <iostream>
#include <fstream>

#include "FFTextStream.hpp"

namespace gpstk
{
   /// @addtogroup RinexClock
   //@{

       /// This class performs file I/O on an RINEX Clock format file for the
       /// RinexClockHeader and RinexClockData classes.
   class RinexClockStream : public FFTextStream
   {
   public:
         /// Default constructor
      RinexClockStream() 
            : headerRead(false)
         {}
      
         /// Common constructor: open (default: read mode)
         /// @param filename the name of the ASCII RinexClock format file to be opened
         /// @param mode the ios::openmode to be used
      RinexClockStream(const char* filename, std::ios::openmode mode=std::ios::in)
            : FFTextStream(filename, mode), headerRead(false)
            { }

         /// destructor; override to force 'close'
      virtual ~RinexClockStream() { }

         /// override open() to reset the header
         /// @param filename the name of the ASCII RINEX Clock format file
         /// @param mode the ios::openmode to be used
      virtual void open(const char* filename, std::ios::openmode mode)
      {
         FFTextStream::open(filename, mode);
         headerRead = false;
      }

         ///@name data members
         //@{
      bool headerRead;             ///< true if the header has been read
         //@}

   }; // class RinexClockStream
   
   //@}
   
} // namespace gpstk

#endif // RINEX_CLOCK_STREAM_INCLUDE
