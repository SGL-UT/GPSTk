#pragma ident "$Id: //depot/sgl/gpstk/dev/src/FFBinaryStream.hpp#1 $"

/**
 * @file FFBinaryStream.hpp
 * An FFStream for binary file reading
 */

#ifndef GPSTK_FFBINARYSTREAM_HPP
#define GPSTK_FFBINARYSTREAM_HPP

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






#include "FFStream.hpp"

namespace gpstk
{
   /** @defgroup formattedfile Formatted file I/O */
   //@{
 
      /**
       * This is an FFStream that is required to be binary.  It also includes
       * functions for reading and writing binary file.  Otherwise, this
       * is the same as FFStream.
       */
   class FFBinaryStream : public FFStream
   {
   public:
         /// destructor
      virtual ~FFBinaryStream() {};
      
         /// Default constructor
      FFBinaryStream() {}

         /**
          * Constructor - opens the stream in binary mode if not set.
          * @param fn file name.
          * @param mode file open mode (std::ios)
          */
      FFBinaryStream(const char* fn, 
                     std::ios::openmode mode=std::ios::in|std::ios::binary)
         : FFStream(fn, mode|std::ios::binary) {}

         /// Overrides open to ensure binary mode opens
      virtual void open(const char* fn, std::ios::openmode mode)
         { FFStream::open(fn, mode|std::ios::binary); }

         /**
          * Reads a T-object directly from the stream
          * in binary form.
          * @throw FFStreamError when the size of the data read
          * from this stream doesn't match the size of a T-object.
          * @return a T-object
          */
      template <class T> T getData() throw(FFStreamError)
      {
         T data;
         try
         {
            read((char*)&data, sizeof(T));
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
      
         if (gcount() != sizeof(T))
         {
            FFStreamError err("Error reading data");
            GPSTK_THROW(err);
         }
         return data;
      } // end of getData(FFStream& strm)

         /**
          * Writes a T-object directly from the stream
          * in binary form.
          * @param data the data to be written.
          * @throw FFStreamError when the size of the data written
          * to this stream doesn't match the size of a T-object.
          * @return a T-object
          */
      template <class T> void writeData(const T& data)
         throw(FFStreamError)
      {
         T temp = data;
         try
         {
            write((char*)&temp, sizeof(T));
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
      } // end of writeData(FFStream& strm, const T& data)
   };

   //@}

}

/*
namespace gpstk
{
   template <class T> 
   T FFBinaryStream::getData() throw(FFStreamError)
   {
      T data;
      try
      {
         read((char*)&data, sizeof(T));
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
      
      if (gcount() != sizeof(T))
      {
         FFStreamError err("Error reading data");
         GPSTK_THROW(err);
      }
      return data;
   } // end of getData(FFStream& strm)

   template <class T> 
   void FFBinaryStream::writeData(const T& data)
      throw(FFStreamError)
   {
      T temp = data;
      try
      {
         write((char*)&temp, sizeof(T));
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
   } // end of writeData(FFStream& strm, const T& data)
}
*/

#endif
