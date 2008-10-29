#pragma ident "$Id$"

/**
 * @file Rinex3ObsStream.hpp
 * File stream for Rinex3 observation file data
 */

#ifndef GPSTK_RINEX3OBSSTREAM_HPP
#define GPSTK_RINEX3OBSSTREAM_HPP

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



#include <vector>
#include <list>
#include <map>
#include <string>

#include "FFTextStream.hpp"
#include "Rinex3ObsHeader.hpp"

namespace gpstk
{

      /** @addtogroup Rinex3Obs */
      //@{

      /**
       * This class reads RINEX3 files.
       *
       * @sa gpstk::Rinex3ObsData and gpstk::Rinex3ObsHeader.
       * @sa rinex_obs_test.cpp and rinex_obs_read_write.cpp for examples.
       */
   class Rinex3ObsStream : public FFTextStream
   {
   public:


         /// Default constructor
      Rinex3ObsStream()
         : headerRead(false) {};


         /** Common constructor.
          *
          * @param fn the RINEX3 file to open
          * @param mode how to open \a fn.
          */
      Rinex3ObsStream( const char* fn,
                      std::ios::openmode mode=std::ios::in )
         : FFTextStream(fn, mode), headerRead(false) {};


         /** Common constructor.
          *
          * @param fn the RINEX3 file to open
          * @param mode how to open \a fn.
          */
      Rinex3ObsStream( const std::string fn,
                      std::ios::openmode mode=std::ios::in )
         : FFTextStream(fn.c_str(), mode), headerRead(false) {};


         /// Destructor
      virtual ~Rinex3ObsStream() {};


         /** Overrides open to reset the header
          *
          * @param fn the RINEX3 file to open
          * @param mode how to open \a fn.
          */
      virtual void open( const char* fn,
                         std::ios::openmode mode )
      {
         FFTextStream::open(fn, mode);
         headerRead = false;
         header = Rinex3ObsHeader();
      };


         /** Overrides open to reset the header
          *
          * @param fn the RINEX3 file to open
          * @param mode how to open \a fn.
          */
      virtual void open( const std::string& fn,
                         std::ios::openmode mode )
      { open(fn.c_str(), mode); };


         /// Whether or not the Rinex3ObsHeader has been read
      bool headerRead;


         /// The header for this file.
      Rinex3ObsHeader header;


   }; // End of class 'Rinex3ObsStream'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_RINEX3OBSSTREAM_HPP
