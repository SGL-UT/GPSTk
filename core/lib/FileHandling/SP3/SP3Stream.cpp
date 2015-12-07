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
 * @file SP3Stream.cpp
 * gpstk::SP3Stream - SP3[abc] format file stream
 */

#include "SP3Stream.hpp"

namespace gpstk
{
   SP3Stream ::
   SP3Stream() 
         : wroteEOF(false),
           writingMode(false),
           lastLine(std::string())
   {
   }
      

   SP3Stream ::
   SP3Stream(const char* filename,
             std::ios::openmode mode)
         : FFTextStream(filename, mode)
   {
      init(mode);
   }


   SP3Stream ::
   ~SP3Stream()
   {
      close();
   }


   void SP3Stream ::
   close(void) throw(Exception)
   {
      if (is_open())
      {
         try {
               // if writing, add the final line
            if(writingMode && !wroteEOF) {
               (*this) << "EOF\n"; 
               wroteEOF = true;
            }
            FFTextStream::close();
         }
         catch(std::exception& e) {
            Exception ge(e.what());
            GPSTK_THROW(ge);
         }
      }
   }


   void SP3Stream ::
   open(const char* filename, std::ios::openmode mode)
   {
      FFTextStream::open(filename, mode);
      init(mode);
   }


   void SP3Stream :: init(std::ios::openmode mode)
   {
      header = SP3Header();
      warnings.clear();

         // for close() later
      wroteEOF = writingMode = false;
      if( (mode & std::ios::out) && !(mode & std::ios::in) )
         writingMode = true;
   }

} // namespace
