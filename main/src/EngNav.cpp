#pragma ident "$Id: //depot/sgl/gpstk/dev/src/EngNav.cpp#4 $"

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






/**
 * @file EngNav.cpp
 * Engineering units navigation message abstraction.
 */

#include <cstdlib>
#include <iostream>
#include <cmath>
#include "EngNav.hpp"
#include "icd_200_constants.hpp"

#ifdef _MSC_VER
#define LDEXP(x,y) ldexp(x,y)
#else
#define LDEXP(x,y) std::ldexp(x,y)
#endif

namespace gpstk
{
   /// DecodeBits .
   struct DecodeBits
   {
      short startBit;
      short numBits;
   };

      /** DecodeQuant contains the information necessary to decode
       * a given quantity for a particular navigation message
       * subframe. */
   struct DecodeQuant
   {
      short outIndex;    ///< Output array index (0-19)
      short pow2;        ///< Power of 2 to scale factor
      short powPI;       ///< Power of PI scale factor
      double scale;      ///< Scalar scale factor
      short signq;       ///< 0 = unsigned, 1 = signed
      DecodeBits fmt[2]; ///< start bit, #bits for up to 2 sections
      DecodeQuant *nxtq; ///< Pointer to next structure in list
   };

      /** Pointers to list of subframe conversions.  There are ten
       * formats numbered 1-10.  Index 0 is unused. */
   static DecodeQuant *subframeList[11];

      /// Table of powers of PI, initialized in EngNav().
   static double PItab[7] = {0,0,0,0,0,0,0};

   static DecodeQuant formats[] = {
      { 0,   0,  0,  1.0L, 0,{ { 1, 8} , { 0, 0} },  0 },     /* Preamble */
      { 1,   0,  0,  1.0L, 0,{ { 9, 14}, { 0, 0} },  0 },     /* Message  */
      { 2,   0,  0,  6.0L, 0,{ { 31,17}, { 0, 0} },  0 },     /* HOW      */
      { 3,   0,  0,  1.0L, 0,{ { 48, 2}, { 0, 0} },  0 },     /* "alert"  */
      { 4,   0,  0,  1.0L, 0,{ { 50, 3}, { 0, 0} },  0 },    /* SF ID    */
      { 5,   0,  0,  1.0L, 0,{ { 61, 10},{ 0, 0} },  0 },    /* week #   */
      { 6,   0,  0,  1.0L, 0,{ { 71,  2},{ 0, 0} },  0}, /* L2 code  */
      { 7,   0,  0,  1.0L, 0,{ { 73,  4},{ 0, 0} },  0}, /* accuracy */
      { 8,   0,  0,  1.0L, 0,{ { 77,  6},{ 0, 0} },  0}, /* health   */
      { 9,  11,  0,  1.0L, 0,{ { 83,  2},{ 211,8}},  0}, /* AODC     */
      { 10,   0,  0, 1.0L, 0,{ { 91,  1},{ 0,  0}},  0}, /* L2 P     */
      { 11, -31,  0, 1.0L, 1,{ {197,  8},{ 0,  0}},  0}, /* Tgd      */
      { 12,   4,  0,  1.0L, 0,{ {219, 16},{ 0,  0}},  0}, /* Toc      */
      { 13, -55,  0,  1.0L, 1,{ {241,  8},{ 0,  0}},  0}, /* Af2      */
      { 14, -43,  0,  1.0L, 1,{ {249, 16},{ 0,  0}},  0}, /* Af1      */
      { 15, -31,  0,  1.0L, 1,{ {271, 22}, {0,  0}},  0}, /* Af0      */
         /* Pattern 2 */
      {0,   0,  0,  1.0L, 0,{ {  1 , 8},  {0,  0  }},  0 }, /* Preamble */
      {1,   0,  0,  1.0L, 0,{ {  9, 14},  { 0, 0  }},  0 }, /* Message  */
      {2,   0,  0,  6.0L, 0,{ {  31,17},  { 0, 0  }},  0 }, /* HOW      */
      {3,   0,  0,  1.0L, 0,{ {  48, 2},  { 0, 0  }},  0 }, /* "alert"  */
      {4,   0,  0,  1.0L, 0,{ { 50,  3},  { 0, 0  }},  0 }, /* SF ID    */
      {5,  11,  0,  1.0L, 0,{ {  61, 8 }, { 0, 0  }},  0 }, /* AODE     */
      {6,  -5,  0,  1.0L, 1,{ {  69, 16 }, { 0, 0  }},  0 }, /* Crs      */
      {7, -43,  1,  1.0L, 1,{ {  91,16 }, { 0, 0  }}, 0 }, /* delta n  */
      {8, -31,  1,  1.0L, 1,{ { 107, 8 }, {121,24 }}, 0 }, /* M0       */
      {9, -29,  0,  1.0L, 1,{ { 151, 16}, { 0,  0 }}, 0}, /* Cuc      */
      {10, -33, 0,  1.0L, 0,{ { 167,  8}, {181,24 }},  0}, /* ecc      */
      {11, -29, 0,  1.0L, 1,{ {  211,16}, { 0, 0  }},  0}, /* Cus      */
      {12, -19, 0,  1.0L, 0,{ {  227, 8}, {241, 24 }},  0}, /* sqrt(A)  */
      {13,   4, 0,  1.0L, 0,{ {  271,16}, { 0,  0 }},  0}, /* Toe      */
      {14,   0, 0,  1.0L, 0,{ {  287, 1}, { 0,  0 }},  0}, /* fit init */
         /* Pattern 3 */
      {0,   0,  0,  1.0L, 0,{ {  1,  8},  { 0,  0 }},  0}, /* Preamble */
      {1,   0,  0,  1.0L, 0,{ {  9, 14},  { 0,  0 }},  0}, /* Message  */
      {2,   0,  0,  6.0L, 0,{ {  31,17},  { 0,  0 }},  0}, /* HOW      */
      {3,   0,  0,  1.0L, 0,{ {  48, 2},  { 0,  0 }},  0}, /* "alert"  */
      {4,   0,  0,  1.0L, 0,{ {  50, 3},  { 0,  0 }},  0}, /* SF ID    */
      {5, -29,  0,  1.0L, 1,{ {  61,16},  { 0,  0 }},  0}, /* Cic      */
      {6, -31,  1,  1.0L, 1,{ {   77,8},  { 91, 24}},  0}, /* OMEGA0   */
      {7, -29,  0,  1.0L, 1,{ { 121,16},  { 0,  0 }},  0}, /* Cis      */
      {8, -31,  1,  1.0L, 1,{ { 137, 8},  {151, 24}},  0}, /* i0       */
      {9,  -5,  0,  1.0L, 1,{ { 181,16},  { 0,  0 }},  0}, /* Crc      */
      {10, -31, 1,  1.0L, 1,{ { 197, 8},  {211, 24}},  0}, /* w        */
      {11, -43, 1,  1.0L, 1,{ { 241,24},  { 0,  0 }},  0}, /* OMEGAdot */
      {12,  11, 0,  1.0L, 0,{ { 271, 8},  { 0,  0 }},  0}, /* AODE     */
      {13, -43, 1,  1.0L, 1,{ { 279,14},  { 0,  0 }},  0}, /* idot     */
         /* Pattern 4 */
      {0,   0,  0,  1.0L, 0,{ { 1,  8},  { 0,  0}},  0 }, /* Preamble */
      {1,   0,  0,  1.0L, 0,{ { 9, 14},  { 0,  0}},   0}, /* Message  */
      {2,   0,  0,  6.0L, 0,{ { 31,17},  { 0,  0}},   0}, /* HOW      */
      {3,   0,  0,  1.0L, 0,{ { 48, 2},  { 0,  0}},   0}, /* "alert"  */
      {4,   0,  0,  1.0L, 0,{ { 50, 3},  { 0,  0}},   0}, /* SF ID    */
      {5,   0,  0,  1.0L, 0,{ { 61, 2},  { 0,  0}},   0}, /* Dataflag */
      {6,   0,  0,  1.0L, 0,{ { 63, 6},  { 0,  0}},   0}, /* Page ID  */
      {7, -21,  0,  1.0L, 0,{ { 69,16},  { 0,  0}},   0}, /* e        */
      {8,  12,  0,  1.0L, 0,{ { 91, 8},  { 0,  0}},   0}, /* time ep  */
      {9, -19,  1,  1.0L, 1,{ { 99,16},  { 0,  0}},   0}, /* i offset */
      {10, -38,  1, 1.0L, 1,{ {121,16},  { 0,  0}},   0}, /* OMEGADOT */
      {11,   0,  0, 1.0L, 0,{ {137, 8},  { 0,  0}},   0}, /* Health   */
      {12, -11,  0, 1.0L, 0,{ {151,24},  { 0,  0}},  0}, /* SQRT(a)  */
      {13, -23,  1, 1.0L, 1,{ {181,24},  { 0,  0}},  0}, /* OMEGA    */
      {14, -23,  1, 1.0L, 1,{ {211,24},  { 0,  0}},  0}, /* w        */
      {15, -23,  1, 1.0L, 1,{ {241,24},  { 0,  0}},  0}, /* Mean Ano */
      {16, -20,  0, 1.0L, 1,{ {271, 8},  {290, 3}},  0}, /* AF0      */
      {17, -38,  0, 1.0L, 1,{ {279,11},  { 0,  0}},  0}, /* AF1      */
      {18,   0,  0, 1.0L, 0,{ { 0,  0},  { 0,  0}},  0}, /* REF WEEK */
      {19,   0,  0, 1.0L, 0,{ { 63, 6},  { 0,  0}},  0}, /* PRN #    */
      
         /* Pattern 5 */
      {0,   0,  0,  1.0L,  0,{ { 1,  8},   { 0,  0}},  0}, /* Preamble */
      {1,   0,  0,  1.0L,  0,{ { 9, 14},   { 0,  0}},  0}, /* Message  */
      {2,   0,  0,  6.0L,  0,{ {31, 17},   { 0,  0}},  0}, /* HOW      */
      {3,   0,  0,  1.0L,  0,{ {48,  2},   { 0,  0}},  0}, /* "alert"  */
      {4,   0,  0,  1.0L,  0,{ {50,  3},   { 0,  0}},  0}, /* SF ID    */
      {5,   0,  0,  1.0L,  0,{ { 61, 2},   { 0,  0}},  0}, /* Dataflag */
      {6,   0,  0,  1.0L,  0,{ { 63, 6},   { 0,  0}},  0}, /* Page ID  */
      {7,   0,  0,  1.0L,  0,{ { 77, 8},   { 0,  0}},  0}, /* Refweek  */
      {8,   0,  0,  1.0L,  0,{ { 91, 6},   { 0,  0}},  0}, /* SV1 Hlth */
      {9,   0,  0,  1.0L,  0,{ { 97, 6},   { 0,  0}},  0}, /* SV2 Hlth */
      {10,   0,  0,  1.0L, 0,{ { 103,6},   { 0,  0}},  0}, /* SV3 Hlth */
      {11,   0,  0,  1.0L, 0,{ { 109,6},   { 0,  0}},  0}, /* SV4 Hlth */
      {12,   0,  0,  1.0L, 0,{ { 121,6},   { 0,  0}},  0}, /* SV5 Hlth */
      {13,   0,  0,  1.0L, 0,{ { 127,6},   { 0,  0}},  0}, /* SV6 Hlth */
      {14,   0,  0,  1.0L, 0,{ { 133,6},   { 0,  0}},  0}, /* SV7 Hlth */
      {15,   0,  0,  1.0L, 0,{ { 139,6},   { 0,  0}},  0}, /* SV8 Hlth */
      {16,   0,  0,  1.0L, 0,{ { 151,6},   { 0,  0}},  0}, /* SV9 Hlth */
      {17,   0,  0,  1.0L, 0,{ { 157,6},   { 0,  0}},  0}, /* SV10 Hlth*/
      {18,   0,  0,  1.0L, 0,{ { 163,6},   { 0,  0}},  0}, /* SV11 Hlth*/
      {19,   0,  0,  1.0L, 0,{ { 169,6},   { 0,  0}},  0}, /* SV12 Hlth*/
      {20,   0,  0,  1.0L, 0,{ { 181,6},   { 0,  0}},  0}, /* SV13 Hlth*/
      {21,   0,  0,  1.0L, 0,{ { 187,6},   { 0,  0}},  0}, /* SV14 Hlth*/
      {22,   0,  0,  1.0L, 0,{ { 193,6},   { 0,  0}},  0}, /* SV15 Hlth*/
      {23,   0,  0,  1.0L, 0,{ { 199,6},   { 0,  0}},  0}, /* SV16 Hlth*/
      {24,   0,  0,  1.0L, 0,{ { 211,6},   { 0,  0}},  0}, /* SV17 Hlth*/
      {25,   0,  0,  1.0L, 0,{ { 217,6},   { 0,  0}},  0}, /* SV18 Hlth*/
      {26,   0,  0,  1.0L, 0,{ { 223,6},   { 0,  0}},  0}, /* SV19 Hlth*/
      {27,   0,  0,  1.0L, 0,{ { 229,6},   { 0,  0}},  0}, /* SV20 Hlth*/
      {28,   0,  0,  1.0L, 0,{ { 241,6},   { 0,  0}},  0}, /* SV21 Hlth*/
      {29,   0,  0,  1.0L, 0,{ { 247,6},   { 0,  0}},  0}, /* SV22 Hlth*/
      {30,   0,  0,  1.0L, 0,{ { 253,6},   { 0,  0}},  0}, /* SV23 Hlth*/
      {31,   0,  0,  1.0L, 0,{ { 259,6},   { 0,  0}},  0}, /* SV24 Hlth*/
         /* Pattern 6 */
      {0,   0,  0,   1.0L, 0, { {  1,   8},{  0, 0}},  0}, /* Preamble */
      {1,   0,  0,   1.0L, 0, { {  9,  14},{  0, 0}},  0}, /* Message  */
      {2,   0,  0,   6.0L, 0, { {  31, 17},{  0, 0}},  0}, /* HOW      */
      {3,   0,  0,   1.0L, 0, { {  48,  2},{  0, 0}},  0}, /* "alert"  */
      {4,   0,  0,   1.0L, 0, { {  50,  3},{  0, 0}},  0}, /* SF ID    */
      {5,   0,  0,   1.0L, 0, { {  61,  2},{  0, 0}},  0}, /* Dataflag */
      {6,   0,  0,   1.0L, 0, { {  63,  6},{  0, 0}},  0}, /* Page ID  */
      {7,   0,  0,   1.0L, 0, { {  69, 16},{  0, 0}},  0}, /* Reserved */
      {8,   0,  0,   1.0L, 0, { {  91, 24},{  0, 0}},  0}, /* Reserved */
      {9,   0,  0,   1.0L, 0, { { 121, 24},{  0, 0}},  0}, /* Reserved */
      {10,  0,  0,   1.0L, 0, { { 151, 24},{  0, 0}},  0}, /* Reserved */
      {11,  0,  0,   1.0L, 0, { { 181, 24},{  0, 0}},  0}, /* Reserved */
      {12,  0,  0,   1.0L, 0, { { 211, 24},{  0, 0}},  0}, /* Reserved */
      {13,  0,  0,   1.0L, 0, { { 241,  8},{  0, 0}},  0}, /* Reserved */
      {14,  0,  0,   1.0L, 0, { {  249,16},{  0, 0}},  0}, /* Reserved */
         /* Pattern 7 */
      {0,   0,  0,  1.0L, 0, { { 1,   8}, { 0,  0} }, 0}, /* Preamble */
      {1,   0,  0,  1.0L, 0, { { 9,  14}, { 0,  0} }, 0}, /* Message  */
      {2,   0,  0,  6.0L, 0, { { 31, 17}, { 0,  0} }, 0}, /* HOW      */
      {3,   0,  0,  1.0L, 0, { { 48,  2}, { 0,  0} }, 0}, /* "alert"  */
      {4,   0,  0,  1.0L, 0, { { 50,  3}, { 0,  0} }, 0}, /* SF ID    */
      {5,   0,  0,  1.0L, 0, { { 61,  2}, { 0,  0} }, 0}, /* Dataflag */
      {6,   0,  0,  1.0L, 0, { { 63,  6}, { 0,  0} }, 0}, /* Page ID  */
      {7,   0,  0,  1.0L, 0, { { 69, 16}, { 0,  0} }, 0}, /* Reserved */
      {8,   0,  0,  1.0L, 0, { { 91, 24}, { 0,  0} }, 0}, /* Reserved */
      {9,   0,  0,  1.0L, 0, { {121, 24}, { 0,  0} }, 0}, /* Reserved */
      {10,  0,  0,  1.0L, 0, { {151, 24}, { 0,  0} }, 0}, /* Reserved */
      {11,  0,  0,  1.0L, 0, { {181, 24}, { 0,  0} }, 0}, /* Reserved */
      {12,  0,  0,  1.0L, 0, { {211, 24}, { 0,  0} }, 0}, /* Reserved */
      {13,  0,  0,  1.0L, 0, { {241,  8}, { 0,  0} }, 0}, /* Reserved */
      {14,  0,  0,  1.0L, 0, { {249, 16}, { 0,  0} }, 0}, /* Reserved */
         /* Pattern 8 */
      {0,   0,  0,   1.0L, 0,{ { 1,   8},{  0, 0}},  0}, /* Preamble */
      {1,   0,  0,   1.0L, 0,{ { 9,  14},{  0, 0}},  0}, /* Message  */
      {2,   0,  0,   6.0L, 0,{ { 31, 17},{  0, 0}},  0}, /* HOW      */
      {3,   0,  0,   1.0L, 0,{ { 48,  2},{  0, 0}},  0}, /* "alert"  */
      {4,   0,  0,   1.0L, 0,{ { 50,  3},{  0, 0}},  0}, /* SF ID    */
      {5,   0,  0,   1.0L, 0,{ { 61,  2},{  0, 0}},  0}, /* Dataflag */
      {6,   0,  0,   1.0L, 0,{ { 63,  6},{  0, 0}},  0}, /* Page ID  */
      {7, -30,  0,   1.0L, 1,{ { 69,  8},{  0, 0}},  0}, /* ALPHA0   */
      {8, -27, -1,   1.0L, 1,{ { 77,  8},{  0, 0}},  0}, /* ALPHA1   */
      {9, -24, -2,   1.0L, 1,{ { 91,  8},{  0, 0}},  0}, /* ALPHA2   */
      {10, -24, -3,  1.0L, 1,{ { 99,  8},{  0, 0}},  0}, /* ALPHA3   */
      {11,  11,  0,  1.0L, 1,{ { 107, 8},{  0, 0}},  0}, /* BETA0    */
      {12,  14, -1,  1.0L, 1,{ { 121, 8},{  0, 0}},  0}, /* BETA1    */
      {13,  16, -2,  1.0L, 1,{ { 129, 8},{  0, 0}},  0}, /* BETA2    */
      {14,  16, -3,  1.0L, 1,{ { 137, 8},{  0, 0}},  0}, /* BETA3    */
      {15, -30,  0,  1.0L, 1,{ { 181,24},{211, 8}},  0}, /* A0       */
      {16, -50,  0,  1.0L, 1,{ { 151,24},{  0, 0}},  0}, /* A1       */
      {17,  12,  0,  1.0L, 0,{ { 219, 8},{  0, 0}},  0}, /* Tot      */
      {18,   0,  0,  1.0L, 0,{ { 227, 8},{  0, 0}},  0}, /* wnt      */
      {19,   0,  0,  1.0L, 1,{ { 241, 8},{  0, 0}},  0}, /* DELTATLS */
      {20,   0,  0,  1.0L, 0,{ { 249, 8},{  0, 0}},  0}, /* WN LSF   */
      {21,   0,  0,  1.0L, 0,{ { 257, 8},{  0, 0}},  0}, /* DN       */
      {22,   0,  0,  1.0L, 1,{ { 271, 8},{  0, 0}},  0}, /* DELTALSF */
         /* Pattern 9 */
      {0,  0,  0,  1.0L, 0, { {   1,  8}, { 0, 0}},  0}, /* Preamble */
      {1,  0,  0,  1.0L, 0, { {   9, 14}, { 0, 0}},  0}, /* Message  */
      {2,  0,  0,  6.0L, 0, { {  31, 17}, { 0, 0}},  0}, /* HOW      */
      {3,  0,  0,  1.0L, 0, { {  48,  2}, { 0, 0}},  0}, /* "alert"  */
      {4,  0,  0,  1.0L, 0, { {  50,  3}, { 0, 0}},  0}, /* SF ID    */
      {5,  0,  0,  1.0L, 0, { {  61,  2}, { 0, 0}},  0}, /* Dataflag */
      {6,  0,  0,  1.0L, 0, { {  63,  6}, { 0, 0}},  0}, /* Page ID  */
      {7,  0,  0,  1.0L, 0, { {  69,  4}, { 0, 0}},  0}, /* SV1 cnfig*/
      {8,  0,  0,  1.0L, 0, { {  73,  4}, { 0, 0}},  0}, /* SV2 cnfig*/
      {9,  0,  0,  1.0L, 0, { {  77,  4}, { 0, 0}},  0}, /* SV3 cnfig*/
      {10, 0,  0,  1.0L, 0, { {  81,  4}, { 0, 0}},  0}, /* SV4 cnfig*/
      {11, 0,  0,  1.0L, 0, { {  91,  4}, { 0, 0}},  0}, /* SV5 cnfig*/
      {12, 0,  0,  1.0L, 0, { {  95,  4}, { 0, 0}},  0}, /* SV6 cnfig*/
      {13, 0,  0,  1.0L, 0, { {  99,  4}, { 0, 0}},  0}, /* SV7 cnfig*/
      {14, 0,  0,  1.0L, 0, { { 103,  4}, { 0, 0}},  0}, /* SV8 cnfig*/
      {15, 0,  0,  1.0L, 0, { { 107,  4}, { 0, 0}},  0}, /* SV9 cnfig*/
      {16, 0,  0,  1.0L, 0, { { 111,  4}, { 0, 0}},  0}, /* SV10 cnfig*/
      {17, 0,  0,  1.0L, 0, { { 121,  4}, { 0, 0}},  0}, /* SV11 cnfig*/
      {18, 0,  0,  1.0L, 0, { { 125,  4}, { 0, 0}},  0}, /* SV12 cnfig*/
      {19, 0,  0,  1.0L, 0, { { 129,  4}, { 0, 0}},  0}, /* SV13 cnfig*/
      {20, 0,  0,  1.0L, 0, { { 133,  4}, { 0, 0}},  0}, /* SV14 cnfig*/
      {21, 0,  0,  1.0L, 0, { { 137,  4}, { 0, 0}},  0}, /* SV15 cnfig*/
      {22, 0,  0,  1.0L, 0, { { 141,  4}, { 0, 0}},  0}, /* SV16 cnfig*/
      {23, 0,  0,  1.0L, 0, { { 151,  4}, { 0, 0}},  0}, /* SV17 cnfig*/
      {24, 0,  0,  1.0L, 0, { { 155,  4}, { 0, 0}},  0}, /* SV18 cnfig*/
      {25, 0,  0,  1.0L, 0, { { 159,  4}, { 0, 0}},  0}, /* SV19 cnfig*/
      {26, 0,  0,  1.0L, 0, { { 163,  4}, { 0, 0}},  0}, /* SV20 cnfig*/
      {27, 0,  0,  1.0L, 0, { { 167,  4}, { 0, 0}},  0}, /* SV21 cnfig*/
      {28, 0,  0,  1.0L, 0, { { 171,  4}, { 0, 0}},  0}, /* SV22 cnfig*/
      {29, 0,  0,  1.0L, 0, { { 181,  4}, { 0, 0}},  0}, /* SV23 cnfig*/
      {30, 0,  0,  1.0L, 0, { { 185,  4}, { 0, 0}},  0}, /* SV24 cnfig*/
      {31, 0,  0,  1.0L, 0, { { 189,  4}, { 0, 0}},  0}, /* SV25 cnfig*/
      {32, 0,  0,  1.0L, 0, { { 193,  4}, { 0, 0}},  0}, /* SV26 cnfig*/
      {33, 0,  0,  1.0L, 0, { { 197,  4}, { 0, 0}},  0}, /* SV27 cnfig*/
      {34, 0,  0,  1.0L, 0, { { 201,  4}, { 0, 0}},  0}, /* SV28 cnfig*/
      {35, 0,  0,  1.0L, 0, { { 211,  4}, { 0, 0}},  0}, /* SV29 cnfig*/
      {36, 0,  0,  1.0L, 0, { { 215,  4}, { 0, 0}},  0}, /* SV30 cnfig*/
      {37, 0,  0,  1.0L, 0, { { 219,  4}, { 0, 0}},  0}, /* SV31 cnfig*/
      {38, 0,  0,  1.0L, 0, { { 223,  4}, { 0, 0}},  0}, /* SV32 cnfig*/
      {39, 0,  0,  1.0L, 0, { { 229,  6}, { 0, 0}},  0}, /* SV25 Hlth */
      {40, 0,  0,  1.0L, 0, { { 241,  6}, { 0, 0}},  0}, /* SV26 Hlth */
      {41, 0,  0,  1.0L, 0, { { 247,  6}, { 0, 0}},  0}, /* SV27 Hlth */
      {42, 0,  0,  1.0L, 0, { { 253,  6}, { 0, 0}},  0}, /* SV28 Hlth */
      {43, 0,  0,  1.0L, 0, { { 259,  6}, { 0, 0}},  0}, /* SV29 Hlth */
      {44, 0,  0,  1.0L, 0, { { 271,  6}, { 0, 0}},  0}, /* SV30 Hlth */
      {45, 0,  0,  1.0L, 0, { { 277,  6}, { 0, 0}},  0}, /* SV31 Hlth */
      {46, 0,  0,  1.0L, 0, { { 283,  6}, { 0, 0}},  0}, /* SV32 Hlth */
         /* Pattern 10 */
      {0,   0,  0,  1.0L, 0,{ {  1,   8}, {  0, 0}},  0}, /* Preamble */
      {1,   0,  0,  1.0L, 0,{ {  9,  14}, {  0, 0}},  0}, /* Message  */
      {2,   0,  0,  6.0L, 0,{ {  31, 17}, {  0, 0}},  0}, /* HOW      */
      {3,   0,  0,  1.0L, 0,{ {  48,  2}, {  0, 0}},  0}, /* "alert"  */
      {4,   0,  0,  1.0L, 0,{ {  50,  3}, {  0, 0}},  0}, /* SF ID    */
      {5,   0,  0,  1.0L, 0,{ {  61,  2}, {  0, 0}},  0}, /* Dataflag */
      {6,   0,  0,  1.0L, 0,{ {  63,  6}, {  0, 0}},  0}, /* Page ID  */
      {7,   0,  0,  1.0L, 0,{ {  69,  8}, {  0, 0}},  0}, /* ASCII    */
      {8,   0,  0,  1.0L, 0,{ {  77,  8}, {  0, 0}},  0}, /* ASCII    */
      {9,   0,  0,  1.0L, 0,{ {  91,  8}, {  0, 0}},  0}, /* ASCII    */
      {10,  0,  0,  1.0L, 0,{ {  99,  8}, {  0, 0}},  0}, /* ASCII    */
      {11,  0,  0,  1.0L, 0,{ { 107,  8}, {  0, 0}},  0}, /* ASCII    */
      {12,  0,  0,  1.0L, 0,{ { 121,  8}, {  0, 0}},  0}, /* ASCII    */
      {13,  0,  0,  1.0L, 0,{ { 129,  8}, {  0, 0}},  0}, /* ASCII    */
      {14,  0,  0,  1.0L, 0,{ { 137,  8}, {  0, 0}},  0}, /* ASCII    */
      {15,  0,  0,  1.0L, 0,{ { 151,  8}, {  0, 0}},  0}, /* ASCII    */
      {16,  0,  0,  1.0L, 0,{ { 159,  8}, {  0, 0}},  0}, /* ASCII    */
      {17,  0,  0,  1.0L, 0,{ { 167,  8}, {  0, 0}},  0}, /* ASCII    */
      {18,  0,  0,  1.0L, 0,{ { 181,  8}, {  0, 0}},  0}, /* ASCII    */
      {19,  0,  0,  1.0L, 0,{ { 189,  8}, {  0, 0}},  0}, /* ASCII    */
      {20,  0,  0,  1.0L, 0,{ { 197,  8}, {  0, 0}},  0}, /* ASCII    */
      {21,  0,  0,  1.0L, 0,{ { 211,  8}, {  0, 0}},  0}, /* ASCII    */
      {22,  0,  0,  1.0L, 0,{ { 219,  8}, {  0, 0}},  0}, /* ASCII    */
      {23,  0,  0,  1.0L, 0,{ { 227,  8}, {  0, 0}},  0}, /* ASCII    */
      {24,  0,  0,  1.0L, 0,{ { 241,  8}, {  0, 0}},  0}, /* ASCII    */
      {25,  0,  0,  1.0L, 0,{ { 249,  8}, {  0, 0}},  0}, /* ASCII    */
      {26,  0,  0,  1.0L, 0,{ { 257,  8}, {  0, 0}},  0}, /* ASCII    */
      {27,  0,  0,  1.0L, 0,{ { 271,  8}, {  0, 0}},  0}, /* ASCII    */
      {28,  0,  0,  1.0L, 0,{ { 279,  8}, {  0, 0}},  0}, /* ASCII    */
         /* Dummy pattern for constructor */
      {0,   0,  0,  1.0L, 0,{{  0,    0}, {  0, 0}},  0}
   };




   EngNav::EngNav()
      throw()
   {
      short i=0, n=0;
      static short initialized = 0;

      if (initialized)
         return;

         // Set up pointers within subframe format structure.
      for (i=1; i<=10; i++)
      {
         subframeList[i] = &formats[n];
         while (formats[n+1].outIndex != 0)
         {
            formats[n].nxtq = &formats[n+1];
            n++;
         }
         formats[n].nxtq = NULL;
         n++;
      }

         // Calculate powers of PI table.
      PItab[3] = 1.0L;
      for (i=0;i<=2;i++)
      {
         PItab[4+i] = PItab[3+i] * PI;
         PItab[2-i] = PItab[3-i] / PI;
      }

      initialized = 1;
   }

   bool EngNav :: subframeConvert(const long input[10], 
                                  const int gpsWeek,
                                  double output[60])
      throw()
   {
      short patId = -2, i = 2;
      struct DecodeQuant *p=NULL;

      for (i=0; i< 20; i++)
         output[i] = 0.0L;

         // Get subframe and page id and derive pattern number.
      if ((patId = getSubframePattern(input)) == 0)
         return false;

         // convert each quantity in the list
      p = subframeList[patId];
      while (p != NULL)
      {
         convertQuant(input, output, p);
         if(p->nxtq)
            p = p->nxtq;
         else
            p = NULL;
      }

         // Almanac does not contain a reference week
         // However we need to put one in the FIC version of the Almanac
      if (patId==4)
      {
         output[18]= static_cast<double>( gpsWeek );
      }

         // Modify the 8-bit week numbers to be the full week numbers
      if (patId == 8)
      {
         if (!convert8bit(gpsWeek, &output[18]))
            return false;

         if (!convert8bit(gpsWeek, &output[20]))
            return false;
      }

         // Change the 10 bit week number in subframe 1 to full weeks
      if (patId == 1)
      {
         if (!convert10bit(gpsWeek, &output[5]))
            return false;
      }

      return true;
   }

   bool EngNav :: convert8bit(const int gpsWeek, double *output)
      throw()
   {
      long high8bits, low8bits;
      long target, diff;

      high8bits = gpsWeek & 0xFFFFFF00L;
      low8bits = gpsWeek & 0x000000FFL;

      target = static_cast<long>( *output ) | high8bits;
 
      diff = static_cast<long>( *output ) - low8bits;  
      if (diff > 127)
         target -= 256;
      else if (diff < -127)
         target += 256;

      *output = static_cast<double>( target );

      if ((target - gpsWeek > 127) || (target - gpsWeek < -127))
         return false;

      return true;
   }

   bool EngNav :: convert10bit(const int gpsWeek, double *output)
      throw()
   {
      long high10bits, low10bits;
      long target, diff;

      high10bits = gpsWeek & 0xFFFFFC00L;
      low10bits  = gpsWeek & 0x000003FFL;

      target = static_cast<long>( *output ) | high10bits;
 
      diff = static_cast<long>( *output ) - low10bits;  
      if (diff > 511)
         target -= 1024;
      else if (diff < -511)
         target += 1024;

      *output = static_cast<double>( target );

      if ((target - gpsWeek > 511) || (target - gpsWeek < -511))
         return false;

      return true;
   }

   short EngNav :: getSubframePattern(const long input[10])
      throw()
   {
      short iret, svid;
      long  itemp;

      short patId[]
         = {   5,   6,  6,   6, 10,  8,   6,  7,   7,  7,   7,  7,   9 };
         /*SVid   51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 */

         //  Get subframe id.  If 1-3 return as patId.
      
      itemp = input[1];         /* move HOW to temp storage         */
      itemp >>= 8;           /* shift so subframe id is in 3 lsb */
      itemp &= 0x00000007L;  /* and mask off msbs                */
      iret = static_cast<short>( itemp );
         /* Not a valid sf id */
      if ( iret < 1 || iret > 5 )
         return(0);
      if ( iret < 4 )
         return(iret);


         /*   If subframe 1-5, get page id and look up patId      */
      itemp = input[2];      /* move word 3 into temp storage    */
      itemp >>= 22;          /* shift so SV ID is in 6 lsbs      */
      itemp &= 0x0000003FL;  /* and mask off msbs                */
      svid  = static_cast<short>( itemp );
      if ( svid <= 32 )
         iret = 4;            /* PRN orbit data */
      else
         iret = patId[svid-51];  /* look up pat id for almanac overhead
                                    information */
      return iret;
   }

   static short countSubframeOnes(long bits)
   {
      short i;
      short count = 0;
      long mask = 0x00000001;
      
      for (i=0;i < 30;i++)
      {
         if (bits&mask) count++;
         mask <<= 1;
      }
      return count;
   }

   bool EngNav :: subframeParity(const long input[10])
   {
         /*
           This function is largely table-driven.  The first table is bmask.
           There is one element in bmask for each of the six parity bits.
           Each element is a bit mask with bits set corresponding to the bits
           which are to be exclusive-OR'd together to form the parity check
           bit.  The following bit maps define the bmask array.  They were
           drawn from table 20-XIV of ICD-GPS-200C (10 OCT 1993).
           
           Bit in navigation message
      bit1                             bit 30
           bit    12 3456 789. 1234 5678 9.12 3456 789.
           ---    -------------------------------------
           D25    11 1011 0001 1111 0011 0100 1000 0000
           D26    01 1101 1000 1111 1001 1010 0100 0000
           D27    10 1110 1100 0111 1100 1101 0000 0000
           D28    01 0111 0110 0011 1110 0110 1000 0000
           D29    10 1011 1011 0001 1111 0011 0100 0000
           D30    00 1011 0111 1010 1000 1001 1100 0000
         */
      long bmask[6] = { 0x3B1F3480L, 0x1D8F9A40L, 0x2EC7CD00L,
                        0x1763E680L, 0x2BB1F340L, 0x0B7A89C0L };
         /*
           the pmask array defines whether bit 29 or bit 30 of the
           previous word is used in determining a given parity bit.
           As with bmask, pmask is ordered from D25 to D30 and drawn
           from Table 20-XIV of ICD-GPS-200C (10 OCT 1993).
         */
      long pmask[6] = {0x00000002, 0x00000001, 0x00000002,
                           0x00000001, 0x00000001, 0x00000002 };
      
      long parity_bits;
      long test_bits,temp;
      short i,j,k,count;
      
         /*
           Test word 1 for proper preamble
         */
      if ( (input[0] & 0x22C00000L) !=0x22C00000L )
         return false;

         /*
           For each nav message word from 2-10.
         */
      for (i=1;i < 10;i++)
      {
         test_bits = 0x0L;        /* Clear the test parity word */
            /* For each parity bit...     */
         for (j=25;j<=30;j++)
         {
            k = j-25;
               /* Build a single word which contains all
                  bits which are to be XOR's together to
                  determine this parity bit */
            temp = input[i] & bmask[k];
            
               /* Then add in the state of *D29 or *D30
                  as appropriate.  (Note that this info
                  may be stored in bits 29/30 of temp
                  since the parity bits (25-30) are never
                  used in the parity calculation */
            temp |=input[i-1] & pmask[k];
            
               /* Count the number of bits set in temp */
            count = countSubframeOnes(temp);
               /* Shift the test word left 1 bit  and add 
                  in the status of the new bit.
                  If count is odd, new bit is one.  If
                  count is even, new bit is zero. */
            test_bits <<= 1;
            if (count%2) test_bits |= 0x00000001;
         }
            /* At this point, the six lsbs of test_bits are equal
               to D25-D30 as determined by the parity algorithm.
               Move D25-D30 as received into parity_bits.  Compare
               the determined to the received.  If they do not match,
               a parity failure has occurred.   */
         parity_bits = input[i] & 0x0000003F;

         if (parity_bits != test_bits)
            return false;
      }
         /*
           If words 2-10 have all passed parity check, return indicating
           success.
         */
      return true;
   }

   void EngNav :: convertQuant(const long input[10], double output[60],
                               DecodeQuant *p)
      throw()
   {
      double dval;
      short i, n, bit1, nword, nbit, lsb;
      union equ
      {
         unsigned long u;
         long s;
      } temp;
      long *b;
      long mask;

         // Convert starting bit number to word/bit pair
      temp.u = 0x0L;
      for (n=0; n<=1; n++)
      {
         if (p->fmt[n].startBit == 0)
            break;
         bit1 = p->fmt[n].startBit;
         nword = (bit1-1) / 30;
         nbit  = (bit1 % 30) + 1;

         b = const_cast<long *>( input ) + nword;
         for (i=0;i<p->fmt[n].numBits;i++)
         {
            temp.u <<= 1;
            mask = 0x80000000L >> nbit++;
            if (*b & mask)
               temp.u++;
            if (nbit>=32)
            {
               b++;
               nbit = 0;
            }
         }
      }

         // Convert to double and scale
      if (p->signq)
      {
         nbit = 32 - (p->fmt[0].numBits + p->fmt[1].numBits);
         temp.u <<= nbit; // Move sign bit to msb
         temp.s >>= nbit; // Move lsb back to right spot with sign extend
         dval = temp.s;
      }
      else
      {
         if (temp.s >= 0)
            dval = temp.u; // msb = 0
         else              // msb = 1
         {                 // This is a special case due to an Alliant bug
            lsb = temp.u & 0x00000001L; // Store lsb value
            temp.u >>= 1;               // Right shift 1 bit to clear msb
            dval = temp.u;              // Move value to double
            dval = LDEXP(dval,1);       // Multiply by 2 (left shift 1 bit)
            dval = dval + lsb;          // Add lsb value back in
         }
      }
      dval = dval * p->scale;             // Scale by scalar
      dval = dval * PItab[ p->powPI+3 ];  // Scale by power of PI
      dval = LDEXP(dval,p->pow2);         // Scale by power of 2
      output[p->outIndex] = dval;         // Store result in output array
   }

} // namespace
