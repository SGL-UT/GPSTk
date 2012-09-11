#pragma ident "$Id$"

/**
 * @file RinexClockStream.hpp
 * File stream for RINEX3 clock data file
 */

#ifndef GPSTK_RINEX3CLOCKSTREAM_HPP
#define GPSTK_RINEX3CLOCKSTREAM_HPP

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Octavian Andrei - FGI ( http://www.fgi.fi ). 2008
//
//============================================================================

// system
#include <vector>
#include <list>
#include <map>
#include <string>
// GPSTk
#include "FFTextStream.hpp"
#include "RinexClockHeader.hpp"

namespace gpstk
{

      /** @addtogroup RinexClock */
      //@{

      /** This class reads RINEX3 clock data files.
       *
       * @sa gpstk::RinexClockData and gpstk::RinexClockHeader.
       * @sa rinex_clk_test.cpp and rinex_clk_read_write.cpp for examples.
       */
   class RinexClockStream : public FFTextStream
   {
   public:


         /// Default constructor
      RinexClockStream()
         : headerRead(false) {};


         /** Common constructor.
          *
          * @param fn      the RINEX3 clock data file to open
          * @param mode    how to open \a fn.
          */
      RinexClockStream( const char* fn,
                         std::ios::openmode mode=std::ios::in )
         : FFTextStream(fn, mode), headerRead(false) {};


         /** Common constructor.
          *
          * @param fn      the RINEX3 clock data file to open
          * @param mode    how to open \a fn.
          */
      RinexClockStream( const std::string fn,
                         std::ios::openmode mode=std::ios::in )
         : FFTextStream(fn.c_str(), mode), headerRead(false) {};


         /// Destructor
      virtual ~RinexClockStream() {};


         /** Overrides open to reset the header
          *
          * @param fn      the RINEX3 clock data file to open
          * @param mode    how to open \a fn.
          */
      virtual void open( const char* fn,
                         std::ios::openmode mode )
      {
         FFTextStream::open(fn, mode);
         headerRead = false;
         header = RinexClockHeader();
      };


         /** Overrides open to reset the header
          *
          * @param fn      the RINEX3 clock data file to open
          * @param mode    how to open \a fn.
          */
      virtual void open( const std::string& fn,
                         std::ios::openmode mode )
      { open(fn.c_str(), mode); };


         /// Whether or not the RinexClockHeader has been read
      bool headerRead;


         /// The header for this file.
      RinexClockHeader header;


   }; // End of class 'RinexClockStream'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_RINEX3CLOCKSTREAM_HPP
