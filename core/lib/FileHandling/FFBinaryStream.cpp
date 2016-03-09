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
// This software developed by Applied Research Laboratories at the
// University of Texas at Austin, under contract to an agency or
// agencies within the U.S.  Department of Defense. The
// U.S. Government retains all rights to use, duplicate, distribute,
// disclose, or release this software.
//
// Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file FFBinaryStream.cpp
 * An FFStream for binary file reading
 */

#include "FFBinaryStream.hpp"

namespace gpstk
{
   FFBinaryStream ::
   FFBinaryStream()
   {
         // nothing to do
   }


   FFBinaryStream ::
   ~FFBinaryStream()
   {
         // nothing to do
   }


   FFBinaryStream ::
   FFBinaryStream(const char* fn, 
                  std::ios::openmode mode)
         : FFStream(fn, mode|std::ios::binary)
   {
   }


   void FFBinaryStream ::
   open(const char* fn, std::ios::openmode mode)
   {
      FFStream::open(fn, mode|std::ios::binary);
   }


   void FFBinaryStream ::
   getData(char* buff, size_t length) throw(EndOfFile, FFStreamError)
   {
      try
      {
         read(buff, length);
      }
      catch(std::exception& exc)
      {
         if (gcount() != (std::streamsize)length && eof())
         {
            EndOfFile err("EOF encountered");
            GPSTK_THROW(err);
         }
         else
         {
            FFStreamError err(exc.what());
            std::cout << err << std::endl;
            GPSTK_THROW(err);
         }
      }
      catch(...)
      {
         FFStreamError err("Unknown exception");
         GPSTK_THROW(err);
      }
   } // end of getData(char*, size_t))


   void FFBinaryStream ::
   writeData(const char* buff, size_t length)
      throw(FFStreamError)
   {
      try
      {
         write(buff, length);
      }
      catch(std::exception& exc)
      {
         FFStreamError err(exc.what());
         GPSTK_THROW(err);
      }
      catch(...)
      {
         FFStreamError err("Unknown exception");
         GPSTK_THROW(err);
      }
      
      if (fail() || bad())
      {
         FFStreamError err("Error writing data");
         GPSTK_THROW(err);
      }
      return;
   } // end of writeData(const char*, size_t)
} // namespace gpstk
