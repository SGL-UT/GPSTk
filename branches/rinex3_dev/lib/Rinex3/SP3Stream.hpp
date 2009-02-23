#pragma ident "$Id$"

/**
 * @file SP3Stream.hpp
 * File stream for SP3 format files
 */

#ifndef GPSTK_SP3STREAM_HPP
#define GPSTK_SP3STREAM_HPP

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

#include "CommonTime.hpp"
#include "FFTextStream.hpp"

using namespace gpstk;

namespace Rinex3
{
   /** @addtogroup SP3ephem */
   //@{

      /**
       * This class provides access to SP3 files.
       *
       * @sa gpstk::SP3Header and gpstk::SP3Data for more information.
       * @sa petest.cpp for an example.
       */
   class SP3Stream : public FFTextStream
   {
   public:
      SP3Stream() : buffer(std::string()) {}
      
         /** Constructor
          * Opens file \a fn using ios::openmode \a mode.
          */
      SP3Stream(const char* fn, std::ios::openmode mode=std::ios::in)
            : FFTextStream(fn, mode) {}

         /// destructor
      virtual ~SP3Stream() {}
      
      CommonTime currentEpoch;   ///< Time from last epoch record read
      std::string buffer;        ///< Last line read, not yet processed
   };

   //@}

}  // namespace

#endif
