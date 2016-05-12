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
 * @file SP3Stream.hpp
 * gpstk::SP3Stream - SP3[abc] format file stream
 */

#ifndef SP3STREAM_INCLUDE
#define SP3STREAM_INCLUDE

#include "FFTextStream.hpp"
#include "SP3Header.hpp"

namespace gpstk
{
      /// @ingroup FileHandling
      //@{

      /** This class performs file I/O on an SP3 file for the SP3Header
       * and SP3Data classes.
       * @note the file format (a, b or c) is stored in the SP3Header
       *   (only).  On input it is set by SP3Header::reallyGetRecord()
       *   by the file content; for output it may be set
       *   (SP3Header::setVersion()) before streaming. */
   class SP3Stream : public FFTextStream
   {
   public:
         /// Default constructor
      SP3Stream();
      
         /** Common constructor: open (default: to read)
          * @param[in] filename the name of the ASCII SP3 format file
          *   to be opened
          * @param[in] mode the ios::openmode to be used */
      SP3Stream(const char* filename,
                std::ios::openmode mode=std::ios::in);

         /// destructor; override to force 'close'
      virtual ~SP3Stream();

         /// override close() to write EOF line
      virtual void close(void) throw(Exception);

         /** override open() to reset the header
          * @param[in] filename the name of the ASCII SP3 format file
          *   to be opened
          * @param[in] mode the ios::openmode to be used */
      virtual void open(const char* filename, std::ios::openmode mode);

      SP3Header header;     ///< SP3Header for this file
      bool wroteEOF;        ///< True if the final 'EOF' has been read.
      bool writingMode;     ///< True if the stream is open in 'out', not 'in', mode
      CommonTime currentEpoch;   ///< Time from last epoch record read
      std::string lastLine;      ///< Last line read, perhaps not yet processed
      std::vector<std::string> warnings; ///< warnings produced by reallyGetRecord()s

   private:
         /// Initialize internal data structures according to file mode
      void init(std::ios::openmode);

   }; // class SP3Stream
   
      //@}
   
} // namespace gpstk

#endif // SP3STREAM_INCLUDE
