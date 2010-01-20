#pragma ident "$Id: GloFreqIndex.cpp 1858 2009-04-10 16:10:58Z raindave $"

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
//============================================================================

/**
 * @file GloFreqIndex.cpp
 * Calculate GLONASS SV frequency index from range & phase data and store it.
 */

#include "GloFreqIndex.hpp"
#include "icd_glo_constants.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   int GloFreqIndex::calcIndex( const std::vector<double>& r1, const std::vector<double>& p1,
                                const std::vector<double>& r2, const std::vector<double>& p2 )
      throw()
   {
                     // dummy map for testing purposes
      freqIndex[RinexSatID("R01")] =  1;
      freqIndex[RinexSatID("R02")] = -4;
      freqIndex[RinexSatID("R03")] =  5;
      freqIndex[RinexSatID("R04")] =  6;
      freqIndex[RinexSatID("R05")] =  1;
      freqIndex[RinexSatID("R06")] = -4;
      freqIndex[RinexSatID("R07")] =  5;
      freqIndex[RinexSatID("R08")] =  6;
      freqIndex[RinexSatID("R09")] = -2;
      freqIndex[RinexSatID("R10")] = -7;
      freqIndex[RinexSatID("R11")] =  0;
//      freqIndex[RinexSatID("R12")] = ;
      freqIndex[RinexSatID("R13")] = -2;
      freqIndex[RinexSatID("R14")] = -7;
      freqIndex[RinexSatID("R15")] =  0;
//      freqIndex[RinexSatID("R16")] = ;
      freqIndex[RinexSatID("R17")] =  4;
      freqIndex[RinexSatID("R18")] = -3;
      freqIndex[RinexSatID("R19")] =  3;
      freqIndex[RinexSatID("R20")] =  2;
      freqIndex[RinexSatID("R21")] =  4;
      freqIndex[RinexSatID("R22")] = -3;
      freqIndex[RinexSatID("R23")] =  3;
      freqIndex[RinexSatID("R24")] =  2;
   }

   int GloFreqIndex::getGloIndex( const RinexSatID& id )
      throw()
   {
      std::map< RinexSatID, int >::const_iterator iter;
      iter = freqIndex.find(id);

      if (iter != freqIndex.end())
         return iter->second;
      else
         return -100; // No entry for the given SatID.
   }

   double GloFreqIndex::getGloFreq( const RinexSatID& id, const int& band, int& error )
      throw()
   {
      int index = getGloIndex(id);

      if (index < -10)
      {
         error = 1; // No entry for the given SatID.
         return 0.;
      }
      else
      {
         error = 0;
         GloFreq *inst;
         inst = inst->instance(); // singleton reference
         if (band == 1)
            return inst->L1map[index];
         else if (band == 2)
            return inst->L2map[index];
         else
         {
            error = 2; // Invalid frequency band.
            return 0.;
         }
      }
   }

   void GloFreqIndex::dump(ostream& s) const
   {
      s << "---------------------------------- REQUIRED ----------------------------------";

   } // end of dump

} // namespace
