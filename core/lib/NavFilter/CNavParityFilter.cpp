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
#include "CNavParityFilter.hpp"
#include "CNavFilterData.hpp"

namespace gpstk
{

   class CRC24Q
   {
   public:
      static const std::size_t bit_count = 24;

      // 0,1,3,4,5,6,7,10,11,14,17,18,23,24
      //1000 0110 0100 1100 1111 1011: 0x864cfb
      CRC24Q()
         : rem(0) , poly(0x864cfb)
      {};

      void process_bit( bool bit)
      {
         rem ^= ( bit ? 0x00800000 : 0 );
         bool const  pdiv = static_cast<bool>( rem & 0x00800000 );
         rem <<= 1;
         if ( pdiv )
            rem ^= poly;
      };

      void process_bits( uint8_t bits, std::size_t bit_count = 8)
      {
         bits <<= 8 - bit_count;

         for ( std::size_t i = bit_count ; i > 0u ; --i, bits <<= 1u )
            process_bit( static_cast<bool>(bits & 0x80) );
      };

      void process_bytes( void const *buffer, std::size_t byte_count)
      {
         uint8_t const *b = static_cast<uint8_t const *>(buffer);
         for (int i=0; i<byte_count; i++)
            process_bits(*b++);
      };

      uint32_t checksum() const
      { return rem & 0x00ffffff; };

   private:
      uint32_t  rem;
      const uint32_t poly;
   };


   CNavParityFilter ::
   CNavParityFilter()
   {
   }

   void CNavParityFilter ::
   validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut)
   {
      NavMsgList::iterator i;
         // check parity of each subframe and put the valid ones in
         // the output
      for (i = msgBitsIn.begin(); i != msgBitsIn.end(); i++)
      {
         CNavFilterData *fd = dynamic_cast<CNavFilterData*>(*i);

         CRC24Q crc;
         int numBits = fd->pnb->getNumBits();
         for (int n=0; n < numBits; n++)
            crc.process_bit(fd->pnb->asBool(n));

         if (crc.checksum()==0)
            accept(*i, msgBitsOut);
         else
            reject(*i);
      }
   }
}
