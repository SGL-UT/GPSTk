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
 * @file FFBinaryStream.hpp
 * An FFStream for binary file reading
 */

#ifndef GPSTK_FFBINARYSTREAM_HPP
#define GPSTK_FFBINARYSTREAM_HPP

#include "FFStream.hpp"
#include "BinUtils.hpp"

namespace gpstk
{
      /// @ingroup FileHandling
      //@{
 
      /**
       * This is an FFStream that is required to be binary.  It also includes
       * functions for reading and writing binary file.  Otherwise, this
       * is the same as FFStream.
       */
   class FFBinaryStream : public FFStream
   {
   public:
         /// Default constructor
      FFBinaryStream();

         /// destructor
      virtual ~FFBinaryStream();
      
         /**
          * Constructor - opens the stream in binary mode if not set.
          * @param[in] fn file name.
          * @param[in] mode file open mode
          */
      FFBinaryStream(const char* fn, 
                     std::ios::openmode mode=std::ios::in|std::ios::binary);

         /// Overrides open to ensure binary mode opens
      virtual void open(const char* fn, std::ios::openmode mode);

         /**
          * Reads typed data directly from the stream in binary form.
          * @throw FFStreamError when the size of the data read
          *   from this stream doesn't match the size of a T-object.
          * @return the decoded data
          */
      inline void getData(uint8_t& v) throw(EndOfFile, FFStreamError);
      inline void getData(uint16_t& v) throw(EndOfFile, FFStreamError);
      inline void getData(uint32_t& v) throw(EndOfFile, FFStreamError);
      inline void getData(uint64_t& v) throw(EndOfFile, FFStreamError);
      inline void getData(int8_t& v) throw(EndOfFile, FFStreamError);
      inline void getData(int16_t& v) throw(EndOfFile, FFStreamError);
      inline void getData(int32_t& v) throw(EndOfFile, FFStreamError);
      inline void getData(int64_t& v) throw(EndOfFile, FFStreamError);
      inline void getData(float& v) throw(EndOfFile, FFStreamError);
      inline void getData(double& v) throw(EndOfFile, FFStreamError);

         /** Read raw data into a buffer.
          * @param[out] buff the buffer to store the stream data
          *   into. Must be pre-allocated to at least length bytes.
          * @param[in] length the number of bytes to read from the stream. */
      void getData(char* buff, size_t length)
         throw(EndOfFile, FFStreamError);

         /**
          * Writes a T-object directly from the stream in binary form.
          * @param[in] data the data to be written.
          * @throw FFStreamError when the size of the data written
          *   to this stream doesn't match the size of a T-object.
          * @return a T-object
          */
      inline void writeData(uint8_t v) throw(FFStreamError);
      inline void writeData(uint16_t v) throw(FFStreamError);
      inline void writeData(uint32_t v) throw(FFStreamError);
      inline void writeData(uint64_t v) throw(FFStreamError);
      inline void writeData(int8_t v) throw(FFStreamError);
      inline void writeData(int16_t v) throw(FFStreamError);
      inline void writeData(int32_t v) throw(FFStreamError);
      inline void writeData(int64_t v) throw(FFStreamError);
      inline void writeData(float v) throw(FFStreamError);
      inline void writeData(double v) throw(FFStreamError);

      void writeData(const char* buff, size_t length)
         throw(FFStreamError);

         /** Child classes must defined this method to determine how
          * decodeData and encodeData behave with respect to byte
          * ordering. This defines the byte ordering of the file
          * format. */
      virtual bool isStreamLittleEndian() const throw() = 0;

   };
      //@}


// Macro because all the getData functions are basically the same.
// Use a character buffer because typed data can have bad side effects
// causing data corruption.
#define FFBIN_GET_DATA(ITOH_FN,NTOH_FN)         \
   char buf[sizeof(v)];                         \
   getData(buf, sizeof(v));                     \
   if (isStreamLittleEndian())                  \
      BinUtils::ITOH_FN(buf, v);                \
   else                                         \
      BinUtils::NTOH_FN(buf, v);

#define FFBIN_WRITE_DATA(HTOI_FN,HTON_FN)       \
   char buf[sizeof(v)];                         \
   if (isStreamLittleEndian())                  \
      BinUtils::HTOI_FN(buf, v);                \
   else                                         \
      BinUtils::HTON_FN(buf, v);                \
   writeData(buf, sizeof(v));


   inline void FFBinaryStream :: getData(uint8_t& v)
      throw(EndOfFile, FFStreamError)
   {
      char *buf = reinterpret_cast<char*>(&v);
      getData(buf, sizeof(v));
   }

   inline void FFBinaryStream :: getData(uint16_t& v)
      throw(EndOfFile, FFStreamError)
   {
      FFBIN_GET_DATA(buitohs,buntohs);
   }

   inline void FFBinaryStream :: getData(uint32_t& v)
      throw(EndOfFile, FFStreamError)
   {
      FFBIN_GET_DATA(buitohl,buntohl);
   }

   inline void FFBinaryStream :: getData(uint64_t& v)
      throw(EndOfFile, FFStreamError)
   {
      FFBIN_GET_DATA(buitohll,buntohll);
   }

   inline void FFBinaryStream :: getData(int8_t& v)
      throw(EndOfFile, FFStreamError)
   {
      char *buf = reinterpret_cast<char*>(&v);
      getData(buf, sizeof(v));
   }

   inline void FFBinaryStream :: getData(int16_t& v)
      throw(EndOfFile, FFStreamError)
   {
      FFBIN_GET_DATA(buitohss,buntohss);
   }

   inline void FFBinaryStream :: getData(int32_t& v)
      throw(EndOfFile, FFStreamError)
   {
      FFBIN_GET_DATA(buitohsl,buntohsl);
   }

   inline void FFBinaryStream :: getData(int64_t& v)
      throw(EndOfFile, FFStreamError)
   {
      FFBIN_GET_DATA(buitohsll,buntohsll);
   }

   inline void FFBinaryStream :: getData(float& v)
      throw(EndOfFile, FFStreamError)
   {
      FFBIN_GET_DATA(buitohf,buntohf);
   }

   inline void FFBinaryStream :: getData(double& v)
      throw(EndOfFile, FFStreamError)
   {
      FFBIN_GET_DATA(buitohd,buntohd);
   }


   inline void FFBinaryStream :: writeData(uint8_t v)
      throw(FFStreamError)
   {
      char *buf = reinterpret_cast<char*>(&v);
      writeData(buf, sizeof(v));
   }

   inline void FFBinaryStream :: writeData(uint16_t v)
      throw(FFStreamError)
   {
      FFBIN_WRITE_DATA(buhtois,buhtons);
   }

   inline void FFBinaryStream :: writeData(uint32_t v)
      throw(FFStreamError)
   {
      FFBIN_WRITE_DATA(buhtoil,buhtonl);
   }

   inline void FFBinaryStream :: writeData(uint64_t v)
      throw(FFStreamError)
   {
      FFBIN_WRITE_DATA(buhtoill,buhtonll);
   }

   inline void FFBinaryStream :: writeData(int8_t v)
      throw(FFStreamError)
   {
      char *buf = reinterpret_cast<char*>(&v);
      writeData(buf, sizeof(v));
   }

   inline void FFBinaryStream :: writeData(int16_t v)
      throw(FFStreamError)
   {
      FFBIN_WRITE_DATA(buhtoiss,buhtonss);
   }

   inline void FFBinaryStream :: writeData(int32_t v)
      throw(FFStreamError)
   {
      FFBIN_WRITE_DATA(buhtoisl,buhtonsl);
   }

   inline void FFBinaryStream :: writeData(int64_t v)
      throw(FFStreamError)
   {
      FFBIN_WRITE_DATA(buhtoisll,buhtonsll);
   }

   inline void FFBinaryStream :: writeData(float v)
      throw(FFStreamError)
   {
      FFBIN_WRITE_DATA(buhtoif,buhtonf);
   }

   inline void FFBinaryStream :: writeData(double v)
      throw(FFStreamError)
   {
      FFBIN_WRITE_DATA(buhtoid,buhtond);
   }

}
#endif
