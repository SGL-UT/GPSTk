


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



/*
*   mergePCodeWords - Helper function used in X2Sequence and GenXSequence

         Given two bit-packed integers, merge the lower portion of the
         first and the higher portion of the second into a new bit-packed
         integer.  The first_bit argument defines the most significant
         bit of the merged word.  The bits are numbered from 0 - MAX_BIT-1.
*/
#ifndef MERGEPCODEWORDS_H
#define MERGEPCODEWORDS_H

   inline unsigned long merge( unsigned long w1,
                        unsigned long w2,
                        int first_bit )
   {
      unsigned long outword = w1;
      if (first_bit==0) return(outword);
      
      outword <<= first_bit;
      outword |= w2 >> (gpstk::MAX_BIT - first_bit);
      
      return(outword);
   }

#endif   // end of MERGEPCODEWORDS_H
