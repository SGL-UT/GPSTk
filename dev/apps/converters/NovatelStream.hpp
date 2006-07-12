#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/converters/NovatelStream.hpp#1 $"

/**
 * @file NovatelStream.hpp
 * gpstk::NovatelStream - binary Novatel file stream container.
 */

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






#ifndef GPSTK_NOVATELSTREAM_HPP
#define GPSTK_NOVATELSTREAM_HPP

#include <vector>
#include <map>

#include "FFBinaryStream.hpp"

namespace gpstk
{
      /** @defgroup NovatelGroup Novatel receiver utilities */
      //@{

      /** 
       * The stream used to obtain data from a binary Novatel File.
       * \sa NovatelData
       */
   class NovatelStream : public FFBinaryStream
   {
   public:
         /// default constructor
      NovatelStream() {}

         /**
          * Constructor
          * @param fn the name of the Novatel file to be opened
          * @param mode the ios::openmode to be used on \a fn
          */
      NovatelStream(const char* fn,
                    std::ios::openmode mode=std::ios::in|std::ios::binary)
            : FFBinaryStream(fn, mode)
         {}

         /// destructor per the coding standards
      virtual ~NovatelStream() {}

         /// overrides open
      virtual void open(const char* fn,
                        std::ios::openmode mode=std::ios::in|std::ios::binary)
         { FFBinaryStream::open(fn, mode); }

   }; // class NovatelStream

   //@}

} // namespace gpstk

#endif
