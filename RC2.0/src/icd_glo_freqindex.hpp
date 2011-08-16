#pragma ident "$Id: icd_glo_freqindex.hpp 1709 2009-02-18 20:27:47Z btolman $"

/**
 * @file icd_glo_freqindex.hpp
 * Constants as defined in the GLONASS ICD v5.0
 */

#ifndef GPSTK_ICD_GLO_FREQINDEX_HPP
#define GPSTK_ICD_GLO_FREQINDEX_HPP

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
// This software developed by Applied Research Laboratories at the University
// of Texas at Austin, under contract to an agency or agencies within the U.S. 
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software. 
//
// Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include <map>

#include "GNSSconstants.hpp"

namespace gpstk
{
      /**
       * @addtogroup geodeticgroup
       * @name GLONASS Constants
       * Time constants are in the time library.
       */
      //@{

      /// Singleton maps of < FreqNo, frequency > as < int, double >.
   class GloFreq
   {

   private:

      // Constructor must be private so it cannot be executed by user.
      GloFreq()
      {
         for (int n = -7; n <= 12; n++)
         {
            // Frequencies in MHz.
            L1map[n] = (L1_FREQ_GLO + n*L1_FREQ_STEP_GLO)/1e6;
            L2map[n] = (L2_FREQ_GLO + n*L2_FREQ_STEP_GLO)/1e6;
         }
      }

      // Object that is created only once.
      static GloFreq* mInstance;

      // The maps of frequency by index, also private.
      std::map<int, double> L1map, L2map;

   public:

      // Method to get reference to object.
      static GloFreq* instance()
      {
         if (mInstance == NULL) mInstance = new GloFreq();
         return mInstance;
      }

      // Method to get L1 frequency(index).
      double getL1(const int& index)
      { return L1map[index]; }

      // Method to get L2 frequency(index).
      double getL2(const int& index)
      { return L2map[index]; }

   };

   // Have to initialize the pointer reference as null.
   GloFreq* GloFreq::mInstance = NULL;

      //@}
   
} // namespace

#endif
