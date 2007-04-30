#pragma ident "$Id$"

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

#ifndef IQSTREAM_HPP
#define IQSTREAM_HPP

#include <vector>
#include <map>
#include <complex>

#include <gpstkplatform.h>
#include <FFBinaryStream.hpp>

namespace gpstk
{
   /** 
    * This is a stream used to parse an IQ data file
    */
   class IQStream : public FFBinaryStream
   {
   public:
      IQStream()
         : debugLevel(0),
           frameCounter(0),
           sampleCounter(0),
           frameLength(500),
           bands(1),
           frameBuffer(NULL)
      { init(); }


      IQStream(const char* fn, std::ios::openmode mode = std::ios::in)
         : FFBinaryStream(fn, mode),
           debugLevel(0),
           frameCounter(0),
           sampleCounter(0),
           frameLength(500),
           bands(1),
           frameBuffer(NULL)
      { init(); }


      /// destructor per the coding standards
      virtual ~IQStream()
      { delete frameBuffer; }

      /// Just a common place to set up a default object
      virtual void init(void);

      /// overrides open to reset the header
      virtual void open(const char* fn, std::ios::openmode mode = std::ios::in)
      {
         FFBinaryStream::open(fn, mode); 
         readPtr = frameLength;
         writePtr = 0;
         frameCounter = 0;
         sampleCounter = 0;
      }

      unsigned frameLength;

      /// The frame count that is at the end of each block
      unsigned frameCounter;

      /// The current frame of data
      char* frameBuffer;

      /// Used to keep track of where we are in the frame
      unsigned readPtr;
      unsigned writePtr;

      /// used to figure out which nibble of readPtr we are in...
      unsigned long sampleCounter;

      /// This is where the meta data of the frame starts
      unsigned metaPtr;

      int debugLevel;

      std::string desc;

      /// The number of bands of data in this file.
      /// This can't be determined from the input stream at the moment
      int bands;
      
      /// These are used to read and write the buffer
      void readBuffer(void);
      void writeBuffer(void);

      /// Returns single complex sample
      virtual void readComplex(std::complex<short>& v) = 0;
      virtual void readComplex(std::complex<float>& v) = 0;

      /// Writes a single complex sample, 
      virtual void writeComplex(const std::complex<short>& v) = 0;
      virtual void writeComplex(const std::complex<float>& v) = 0;
   }; // class IQStream


   inline IQStream& operator>>(IQStream& s, std::complex<short>& v)
   { s.readComplex(v); return s; };

   inline IQStream& operator>>(IQStream& s, std::complex<float>& v)
   { s.readComplex(v); return s; };

   inline IQStream& operator<<(IQStream& s, const std::complex<short>& v)
   { s.writeComplex(v); return s; };

   inline IQStream& operator<<(IQStream& s, const std::complex<float>& v)
   { s.writeComplex(v); return s; };


   class IQ1Stream : public IQStream
   {
   public:
      IQ1Stream() : IQStream() {init();}

      IQ1Stream(const char* fn, std::ios::openmode mode = std::ios::in)
         : IQStream(fn, mode)
      {init(); desc="1 bit";}

      /// destructor per the coding standards
      virtual ~IQ1Stream() {};

      /// Just a common place to set up a default object
      virtual void init(void) {};

      /// Returns single complex sample
      virtual void readComplex(std::complex<short>& v);
      virtual void readComplex(std::complex<float>& v);

      /// Writes a single complex sample, 
      virtual void writeComplex(const std::complex<short>& v);
      virtual void writeComplex(const std::complex<float>& v);
   }; // class IQ1Stream


   class IQ2Stream : public IQStream
   {
   public:
      IQ2Stream() : IQStream() {init();desc="2 bit";}

      IQ2Stream(const char* fn, std::ios::openmode mode = std::ios::in)
         : IQStream(fn, mode)
      {init();desc="2 bit";}

      /// destructor per the coding standards
      virtual ~IQ2Stream() {};

      /// Just a common place to set up a default object
      virtual void init(void);

      /// Maps the bits of each sample to the actual levels
      std::vector<short> sample2Level;

      /// Encodes levels into the appropriate bits
      template<class T>
      uint8_t l2s(T v);

      /// Returns single complex sample
      virtual void readComplex(std::complex<short>& v);
      virtual void readComplex(std::complex<float>& v);

      /// Writes a single complex sample, 
      virtual void writeComplex(const std::complex<short>& v);
      virtual void writeComplex(const std::complex<float>& v);
   private:
      void writeNibble(uint8_t i, uint8_t q);
   }; // class IQ2Stream


   class IQFloatStream : public IQStream
   {
   public:
      IQFloatStream() : IQStream() {init();desc="float";}

      IQFloatStream(const char* fn, std::ios::openmode mode = std::ios::in)
         : IQStream(fn, mode)
      {init();desc="float";}

      /// destructor per the coding standards
      virtual ~IQFloatStream() {};

      /// Just a common place to set up a default object
      virtual void init(void) {};

      /// Returns single complex sample
      virtual void readComplex(std::complex<short>& v);
      virtual void readComplex(std::complex<float>& v);

      /// Writes a single complex sample, 
      virtual void writeComplex(const std::complex<short>& v);
      virtual void writeComplex(const std::complex<float>& v);
   }; // class IQ2Stream

} // namespace gpstk

#endif
