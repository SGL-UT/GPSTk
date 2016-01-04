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
 * @file FFTextStream.cpp
 * An FFStream for text files
 */

#include "FFTextStream.hpp"

namespace gpstk
{
   FFTextStream ::
   FFTextStream()
   {
      init();
   }


   FFTextStream ::
   ~FFTextStream()
   {
   }


   FFTextStream ::
   FFTextStream( const char* fn,
                 std::ios::openmode mode )
         : FFStream(fn, mode)
   {
      init();
   }


   FFTextStream ::
   FFTextStream( const std::string& fn,
                 std::ios::openmode mode )
         : FFStream( fn.c_str(), mode )
   {
      init();
   }


   void FFTextStream ::
   open( const char* fn,
         std::ios::openmode mode )
   {
      FFStream::open(fn, mode);
      init();
   }


   void FFTextStream ::
   open( const std::string& fn,
         std::ios::openmode mode )
   {
      open(fn.c_str(), mode);
   }


   void FFTextStream ::
   init()
   {
      lineNumber = 0;
   }


   void FFTextStream ::
   tryFFStreamGet(FFData& rec)
      throw(FFStreamError, gpstk::StringUtils::StringException)
   {
      unsigned int initialLineNumber = lineNumber;

      try
      {
         FFStream::tryFFStreamGet(rec);
      }
      catch(gpstk::Exception& e)
      {
         e.addText( std::string("Near file line ") +
                    gpstk::StringUtils::asString(lineNumber) );
         lineNumber = initialLineNumber;
         mostRecentException = e;
         conditionalThrow();
      }
   }


   void FFTextStream ::
   tryFFStreamPut(const FFData& rec)
      throw(FFStreamError, gpstk::StringUtils::StringException)
   {
      unsigned int initialLineNumber = lineNumber;

      try
      {
         FFStream::tryFFStreamPut(rec);
      }
      catch(gpstk::Exception& e)
      {
         e.addText( std::string("Near file line ") +
                    gpstk::StringUtils::asString(lineNumber) );
         lineNumber = initialLineNumber;
         mostRecentException = e;
         conditionalThrow();
      }
   }


      // the reason for checking ffs.eof() in the try AND catch block is
      // because if the user enabled exceptions on the stream with exceptions()
      // then eof could throw an exception, in which case we need to catch it
      // and rethrow an EOF or FFStream exception.  In any event, EndOfFile
      // gets thrown whenever there's an EOF and expectEOF is true
   void FFTextStream ::
   formattedGetLine( std::string& line,
                     const bool expectEOF )
      throw(EndOfFile, FFStreamError, gpstk::StringUtils::StringException)
   {
      try
      {
            // The following constant used to be 256, but with the change to
            // RINEX3 formats the possible length of a line increased
            // considerably. A RINEX3 observation file line for Galileo may
            // be 1277 characters long (taking into account all the possible
            // types of observations available, plus the end of line
            // characters), so this constant was conservatively set to
            // 1500 characters. Dagoberto Salazar.
         const int MAX_LINE_LENGTH = 1500;
         char templine[MAX_LINE_LENGTH + 1];
         getline(templine, MAX_LINE_LENGTH);
         lineNumber++;
            //check if line was longer than 256 characters, if so error
         if(fail() && !eof())
         {
            FFStreamError err("Line too long");
            GPSTK_THROW(err);
         }
         line = templine;
         gpstk::StringUtils::stripTrailing(line, '\r');
            // catch EOF when stream exceptions are disabled
         if ((gcount() == 0) && eof())
         {
            if (expectEOF)
            {
               EndOfFile err("EOF encountered");
               GPSTK_THROW(err);
            }
            else
            {
               FFStreamError err("Unexpected EOF encountered");
               GPSTK_THROW(err);
            }
         }
      }
      catch(std::exception &e)
      {
            // catch EOF when exceptions are enabled
         if ( (gcount() == 0) && eof())
         {
            if (expectEOF)
            {
               EndOfFile err("EOF encountered");
               GPSTK_THROW(err);
            }
            else
            {
               FFStreamError err("Unexpected EOF");
               GPSTK_THROW(err);
            }
         }
         else
         {
            FFStreamError err("Critical file error: " +
                              std::string(e.what()));
            GPSTK_THROW(err);
         }  // End of 'if ( (gcount() == 0) && eof())'

      }  // End of 'try-catch' block

   }  // End of method 'FFTextStream::formattedGetLine()'
}  // End of namespace gpstk
