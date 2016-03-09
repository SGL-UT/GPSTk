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
 * @file SinexStream.hpp
 * File stream for Sinex format files
 */

#ifndef SINEXSTREAM_HPP
#define SINEXSTREAM_HPP

#include "FFTextStream.hpp"

namespace gpstk
{
   namespace Sinex
   {
         /// @ingroup FileHandling
         //@{

         /**
          * This class provides access to Sinex files.
          *
          * @sa gpstk::SinexHeader and gpstk::SinexData for more information.
          * @sa sinex_test.cpp for an example.
          */
      class Stream : public FFTextStream
      {
      public:
         Stream()
         {}

            /** Constructor
             * Opens file \a fn using ios::openmode \a mode.
             */
         Stream(const char* fn, std::ios::openmode mode=std::ios::in)
               : FFTextStream(fn, mode)
         {}

            /**
             * Destructor
             */
         virtual ~Stream()
         {}
      };

         //@}

   }  // namespace Sinex

}  // namespace gpstk

#endif  // GPSTK_SINEXSTREAM_HPP
