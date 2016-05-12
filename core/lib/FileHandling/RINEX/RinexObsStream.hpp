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
 * @file RinexObsStream.hpp
 * File stream for Rinex observation file data
 */

#ifndef RINEXOBSSTREAM_HPP
#define RINEXOBSSTREAM_HPP

#include <vector>
#include <list>
#include <map>
#include <string>

#include "FFTextStream.hpp"
#include "RinexObsHeader.hpp"

namespace gpstk
{

      /// @ingroup FileHandling
      //@{

      /**
       * This class reads RINEX files.
       *
       * @sa gpstk::RinexObsData and gpstk::RinexObsHeader.
       * @sa rinex_obs_test.cpp and rinex_obs_read_write.cpp for examples.
       */
   class RinexObsStream : public FFTextStream
   {
   public:
         /// Default constructor
      RinexObsStream();

         /** Common constructor.
          *
          * @param[in] fn the RINEX file to open
          * @param[in] mode how to open \a fn.
          */
      RinexObsStream( const char* fn,
                      std::ios::openmode mode=std::ios::in );

         /** Common constructor.
          *
          * @param[in] fn the RINEX file to open
          * @param[in] mode how to open \a fn.
          */
      RinexObsStream( const std::string fn,
                      std::ios::openmode mode=std::ios::in );

         /// Destructor
      virtual ~RinexObsStream();

         /** Overrides open to reset the header
          *
          * @param[in] fn the RINEX file to open
          * @param[in] mode how to open \a fn.
          */
      virtual void open( const char* fn,
                         std::ios::openmode mode );

         /** Overrides open to reset the header
          *
          * @param[in] fn the RINEX file to open
          * @param[in] mode how to open \a fn.
          */
      virtual void open( const std::string& fn,
                         std::ios::openmode mode );

         /// Whether or not the RinexObsHeader has been read
      bool headerRead;

         /// The header for this file.
      RinexObsHeader header;

         /// Check if the input stream is the kind of RinexObsStream
      static bool isRinexObsStream(std::istream& i);

   private:
      void init();
   }; // End of class 'RinexObsStream'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_RINEXOBSSTREAM_HPP
