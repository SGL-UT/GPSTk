#pragma ident "$Id: //depot/sgl/gpstk/dev/src/EngNav.hpp#5 $"

/**
 * @file EngNav.hpp
 * Engineering units navigation message abstraction.
 */

#ifndef GPSTK_ENGNAV_HPP
#define GPSTK_ENGNAV_HPP

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






#include <sys/types.h>

#include "gpstkplatform.h"

namespace gpstk
{
   /** @addtogroup ephemcalc */
   //@{

   struct DecodeQuant;

      /**
       * Base class for ICD-GPS-200 navigation messages.  This class
       * provides functions for decoding the bits in navigation
       * message, and is the base class for all "engineering units"
       * types.
       */
   class EngNav
   {
   public:
         /// default constructor
      EngNav() throw();

         /// destructor
      virtual ~EngNav() {}

         /**
          * Perform a parity check on a navigation message subframe.
          * @return true if the parity check is successful.
          */
      static bool subframeParity(const long input[10]);

         /**
          * Given 10 words of a navigation message subframe (as
          * defined in ICD-GPS-200), convert to the "appropriate" 60
          * FIC floating point values.
          * @param input array of ten 30-bit words (stored in the 30
          * least-significant bits of each long.
          * @param gpsWeek full (>10 bits) GPS week number.
          * @param output 60 FIC floating point values as defined in
          * the documentation for FIC blocks 9 and 62.
          * @return true if successful.
          */
      static bool subframeConvert(const long input[10], 
                                  const int gpsWeek,
                                  double output[60])
         throw();

         /** Convert the week number in \c out from 8-bit to full
          * using the full week number \c gpsWeek.
          * @param gpsWeek source full week number.
          * @param out 8-bit week number to convert to full
          * @return true if source and target are within 127 weeks of
          * each other.
          */
      static bool convert8bit(const int gpsWeek, double *out)
         throw();

         /** Convert the week number in \c out from 10-bit to full
          * using the full week number \c gpsWeek.
          * @param gpsWeek source full week number.
          * @param out 10-bit week number to convert to full
          * @return true if source and target are within 511 weeks of
          * each other.
          */
      static bool convert10bit(const int gpsWeek, double *out)
         throw();

         /**
          * Given a navigation message subframe, return the
          * pattern number to be used in converting the
          * subframe to engineering units.  The patterns are
          * defined in the following table.  The numbers correspond
          * to the ordering of the bit definitions in ICD-GPS-200
          * Figure 20-1.
          *
          * Subframe #   SV_id   Pattern #
          *     1         n/a        1
          *     2         n/a        2
          *     3         n/a        3
          *     4        1-24        4
          *     4          25        5
          *     5          57        6
          *     5       58-62        7
          *     5          56        8
          *     5          63        9
          *     5       52-55       10
          *
          * @param input 10 long integers containing the ten words of
          * the navigation message subframe.
          * @return the pattern ID as defined in the above table.
          */
      static short getSubframePattern(const long input[10])
         throw();

   private:

         /**
          * Given 10 words of a navigation message subframe, in, and a
          * structure, p, defining a particular conversion, perform
          * the conversion and store the results in the appropriate
          * location in the FIC F array, out.
          *
          * @param input words of navigation message subframe.  Each
          * nav message word is in the 30 lsbs of the corresponding
          * input[i].
          *
          * @param output array of 60 double which correspond to the
          * section of a FIC F array.  The converted output will be
          * placed in the output array at the location specified in
          * the conversion specification.
          *
          * @param p pointer to structure defining conversion to be
          * performed.
          */
      static void convertQuant(const long input[10], double output[60],
                               DecodeQuant *p)
         throw();
   }; // class EngNav

   //@}

} // namespace

#endif
