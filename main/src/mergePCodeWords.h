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
