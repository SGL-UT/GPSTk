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

#include "IQStream.hpp"

using namespace std;

namespace gpstk
{
   void IQStream::init(void)
   {
      frameBuffer = new char[frameLength];
      readPtr = frameLength;
      writePtr = 0;
      sampleCounter = 0;
      metaPtr = frameLength - 4;
   }

   void IQStream::readBuffer(void)
   {
      if (sampleCounter & 0x1)
         cerr << "Uh, we have a problem " << sampleCounter << endl;
      read(frameBuffer, frameLength);
      if (gcount() != frameLength)
         return;
      if (debugLevel>1)
         cout << "Filled frame buffer" << endl;
      readPtr = 0;

      unsigned char sum=0;
      for (int i=0; i<frameLength-1; i++)
         sum+=static_cast<unsigned char>(frameBuffer[i]);
      if (sum != 0x5a)
         cerr << "IQStream::readComplex() checksum error "
              << hex << sum << dec << endl;

      uint16_t fc = frameBuffer[frameLength-3] & 0x00ff   |
         frameBuffer[frameLength-4]<<8 & 0xff00;
      int16_t deltaFc = fc - frameCounter;
         
      if (sampleCounter && deltaFc != 1)
         cerr << "IQStream::readComplex() frame count error, delta:" 
              << deltaFc << endl;
      frameCounter = fc;

      if (debugLevel>1)
         gpstk::StringUtils::hexDumpData(
            cout, string(frameBuffer, frameLength));
   }


   void IQStream::writeBuffer(void)
   {
      frameBuffer[frameLength-3] = frameCounter & 0xff;
      frameBuffer[frameLength-4] = frameCounter>>8 & 0xff;

      unsigned char sum=0;
      for (int i=0; i<frameLength-2; i++)
         sum+=static_cast<unsigned char>(frameBuffer[i]);
      frameBuffer[frameLength-2] = 0x5a-sum;
      frameBuffer[frameLength-1] = 0;

      if (debugLevel>1)
         cout << "Writing frameBuffer" << endl;
      if (debugLevel>1)
         gpstk::StringUtils::hexDumpData(
            cout, string(frameBuffer, frameLength));
            
      write(frameBuffer, frameLength);
      writePtr = 0;
      frameCounter++;
   }


   //-----------------------------------------------------------------------------
   //-----------------------------------------------------------------------------
   void IQ1Stream::readComplex(complex<short>& v)
   {
      if (readPtr == frameLength)
         readBuffer();

      uint8_t byte;
      if (readPtr < metaPtr)
         byte = frameBuffer[readPtr];
      else
         byte = 0;

      if (sampleCounter & 0x1)
      {
         byte = byte >> 4;
         readPtr++;
      }
      sampleCounter++;
         
      int i,q;
      q = (byte & 1) ? +1 : -1;
      i = (byte & 4) ? +1 : -1;

      v = complex<short>(i, q);
   }


   //-----------------------------------------------------------------------------
   void IQ1Stream::readComplex(complex<float>& v)
   {
      complex<short> s;
      readComplex(s);
      v = complex<float>(static_cast<float>(s.real()),
                          static_cast<float>(s.imag()));
   }


   //-----------------------------------------------------------------------------
   void IQ1Stream::writeComplex(const complex<short>& v)
   {
      uint8_t i = v.real()>0 ? 1 : 0;
      uint8_t q = v.imag()>0 ? 1 : 0;
      uint8_t nibble = (q & 0x3) | ((i<<2) & 0xc);

      if (sampleCounter & 0x1)
         frameBuffer[writePtr] = frameBuffer[writePtr] | nibble << 4;
      else
         frameBuffer[writePtr] = nibble;

      if (sampleCounter & 0x1)
         writePtr++;

      sampleCounter++;

      if (writePtr == frameLength)
         writeBuffer();
   }


   //-----------------------------------------------------------------------------
   void IQ1Stream::writeComplex(const complex<float>& v)
   {
      complex<short> s(static_cast<short>(v.real()),
                       static_cast<short>(v.imag()));
      writeComplex(s);
   }



   //-----------------------------------------------------------------------------
   //-----------------------------------------------------------------------------
   void IQ2Stream::init(void)
   {
      sample2Level.resize(4);
      sample2Level[0] = -1;
      sample2Level[1] = -3;
      sample2Level[2] = 1;
      sample2Level[3] = 3;
   }


   //-----------------------------------------------------------------------------
   void IQ2Stream::readComplex(complex<short>& v)
   {
      if (readPtr == frameLength)
         readBuffer();

      uint8_t byte;
      if (readPtr < metaPtr)
         byte = frameBuffer[readPtr];
      else
         byte = 0;

      if (sampleCounter & 0x1)
      {
         byte = byte >> 4;
         readPtr++;
      }
      sampleCounter++;
         
      int i,q;
      q = (byte & 0x3);
      i = ((byte >> 2) & 0x3);

      v = complex<short>(sample2Level[i], sample2Level[q]);
   }


   //-----------------------------------------------------------------------------
   void IQ2Stream::readComplex(complex<float>& v)
   {
      complex<short> s;
      readComplex(s);
      v = complex<float>(static_cast<float>(s.real()),
                          static_cast<float>(s.imag()));
   }


   //-----------------------------------------------------------------------------
   template<class T>
   uint8_t IQ2Stream::l2s(T v)
   {
      uint8_t s=0;
      if (v>0)
         s |= 2;
      else
         v *= -1;

      if (v>1)
         s |=1;

      return s;
   }


   //-----------------------------------------------------------------------------
   void IQ2Stream::writeNibble(uint8_t i, uint8_t q)
   {
      uint8_t nibble = (q & 0x3) | ((i<<2) & 0xc);
      if (sampleCounter & 0x1)
         frameBuffer[writePtr] = frameBuffer[writePtr] | nibble << 4;
      else
         frameBuffer[writePtr] = nibble;

      if (sampleCounter & 0x1)
         writePtr++;

      sampleCounter++;

      if (writePtr == frameLength)
         writeBuffer();
      
   }


   //-----------------------------------------------------------------------------
   void IQ2Stream::writeComplex(const complex<short>& v)
   {
      uint8_t i = l2s(v.real());
      uint8_t q = l2s(v.imag());
      writeNibble(i, q);
   }


   //-----------------------------------------------------------------------------
   void IQ2Stream::writeComplex(const complex<float>& v)
   {
      uint8_t i = l2s(v.real());
      uint8_t q = l2s(v.imag());
      writeNibble(i, q);
   }



   //-----------------------------------------------------------------------------
   //-----------------------------------------------------------------------------
   void IQFloatStream::readComplex(complex<float>& v)
   {
      const size_t size = 2*sizeof(float);
         
      if (readPtr + size >metaPtr)
         readBuffer();

      float i = *(reinterpret_cast<float*>(&frameBuffer[readPtr]));
      readPtr += sizeof(float);

      float q = *(reinterpret_cast<float*>(&frameBuffer[readPtr]));
      readPtr += sizeof(float);
      sampleCounter++;

      v = complex<float>(i, q);
   }

   //-----------------------------------------------------------------------------
   void IQFloatStream::readComplex(complex<short>& v)
   {
      complex<float> s;
      readComplex(s);
      v = complex<short>(static_cast<short>(s.real()),
                          static_cast<short>(s.imag()));
   }


   //-----------------------------------------------------------------------------
   void IQFloatStream::writeComplex(const complex<float>& v)
   {
      const size_t size = 2*sizeof(float);

      if (writePtr + size > metaPtr)
         writeBuffer();

      *(reinterpret_cast<float*>(&frameBuffer[writePtr])) = v.real();
      writePtr += sizeof(float);

      *(reinterpret_cast<float*>(&frameBuffer[writePtr])) = v.imag();
      writePtr += sizeof(float);

      sampleCounter++;
   }

   //-----------------------------------------------------------------------------
   void IQFloatStream::writeComplex(const complex<short>& v)
   {
      complex<float> s(static_cast<float>(v.real()),
                       static_cast<float>(v.imag()));
      writeComplex(s);
   }
} // namespace gpstk
