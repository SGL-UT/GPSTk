#pragma ident "$Id$"

/**
 * @file RinexGloNavStream.hpp
 * File stream for Rinex GLONASS navigation file data
 */

#ifndef GPSTK_RINEXGLONAVSTREAM_HPP
#define GPSTK_RINEXGLONAVSTREAM_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011
//
//============================================================================


#include "FFTextStream.hpp"
#include "RinexGloNavHeader.hpp"

namespace gpstk
{
   /** @addtogroup RinexNav */

   //@{

      /**
       * This class performs file i/o on a RINEX GLONASS NAV file.
       *
       * \sa gpstk::RinexGloNavHeader and gpstk::RinexGloNavData classes.
       */
   class RinexGloNavStream : public FFTextStream
   {
   public:

         /// Default constructor
      RinexGloNavStream()
            : headerRead(false)
         {}
      
         /** Constructor 
          * Opens a file named \a fn using ios::openmode \a mode.
          */
      RinexGloNavStream( const char* fn,
                         std::ios::openmode mode=std::ios::in )
         : FFTextStream(fn, mode), headerRead(false) {}
      
         /// Destructor
      virtual ~RinexGloNavStream() {}
      
         /// Overrides open to reset the header
      virtual void open( const char* fn,
                         std::ios::openmode mode )
         { 
            FFTextStream::open(fn, mode); 
            headerRead = false; 
            header = RinexGloNavHeader();
         }

         /// RINEX GLONASS NAV header for this file.
      RinexGloNavHeader header;
     
         /// Flag showing whether or not the header has been read.
      bool headerRead;
   };

   //@}

}  // End of namespace gpstk

#endif   // GPSTK_RINEXGLONAVSTREAM_HPP
