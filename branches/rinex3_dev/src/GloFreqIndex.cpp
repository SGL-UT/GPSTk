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
#include "Stats.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   // Fills map with known SV info.  This is for testing purposes only.

   void GloFreqIndex::knownIndex()
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
//      freqIndex[RinexSatID("R12")] = ; // not currently in orbit
      freqIndex[RinexSatID("R13")] = -2;
      freqIndex[RinexSatID("R14")] = -7;
      freqIndex[RinexSatID("R15")] =  0;
//      freqIndex[RinexSatID("R16")] = ; // not currently in orbit
      freqIndex[RinexSatID("R17")] =  4;
      freqIndex[RinexSatID("R18")] = -3;
      freqIndex[RinexSatID("R19")] =  3;
      freqIndex[RinexSatID("R20")] =  2;
      freqIndex[RinexSatID("R21")] =  4;
      freqIndex[RinexSatID("R22")] = -3;
      freqIndex[RinexSatID("R23")] =  3;
      freqIndex[RinexSatID("R24")] =  2;
   }


   // Calculates a GLONASS SV's frequency channel index from Obs data.
   /*
     STEPS:
     1. Compute y(i) = R(i) - lambda0*phi(i).
     2. Compute the first differences del-y and del-phi.
     3. Find slope of del-y v. lambda0*delphi (degree one).
     4. Compute double precision index and round to integer.
     5. Store results in struct, including standard error from fit.
     6. Propagate slope error to del-n.
     7. Implement scheme to compute overall final result & fill int map.
   */

   int GloFreqIndex::addPass( const RinexSatID& id, const CommonTime& tt,
                              const std::vector<double>& r1, const std::vector<double>& p1,
                              const std::vector<double>& r2, const std::vector<double>& p2 )
      throw()
   {
      vector<double> y1, y2, dy1, dy2, dp1, dp2;

      if (r1.size() != p1.size()) return 1;
      if (r2.size() != p2.size()) return 2;

      // Compute y(i) = R(i) - lambda0*phi(i) for G1.
      for (int i=0; i < r1.size(); i++)
      {
         double y = r1[i] - L1_FREQ_GLO*p1[i];
         y1.push_back(y);
      }

      // Compute y(i) = R(i) - lambda0*phi(i) for G2.
      for (int i=0; i < r2.size(); i++)
      {
         double y = r2[i] - L2_FREQ_GLO*p2[i];
         y2.push_back(y);
      }

      // TwoSampleStats (vector pairs) of y v. lambda0*phi.

      TwoSampleStats<double> line1, line2;

      // Compute del-y and del-phi for G1.

      for (int i=1; i < y1.size(); i++)
      {
         double dy = y1[i] - y1[i-1];
         double dp = p1[i] - p1[i-1];
         line1.Add(dy,dp);
//         dy1.push_back(dy);
//         dp1.push_back(dp);
      }

      // Compute del-y and del-phi for G2.

      for (int i=1; i < y2.size(); i++)
      {
         double dy = y2[i] - y2[i-1];
         double dp = p2[i] - p2[i-1];
         line2.Add(dy,dp);
//         dy2.push_back(dy);
//         dp2.push_back(dp);
      }

      // Compute best-fit slopes of lines and their uncertainties.

      double  m1 = line1.Slope();
      double  m2 = line2.Slope();
      double dm1 = line1.SigmaSlope();
      double dm2 = line2.SigmaSlope();

      // Compute float values of index from slopes.

      double n1 = -(L1_FREQ_GLO/L1_FREQ_STEP_GLO)*m1/(m1+1.0);
      double n2 = -(L2_FREQ_GLO/L2_FREQ_STEP_GLO)*m2/(m2+1.0);

      // Compute uncertainties on the float index values.

      double dn1 = (L1_FREQ_GLO/L1_FREQ_STEP_GLO)*dm1/std::pow(m1+1.0,2);
      double dn2 = (L2_FREQ_GLO/L2_FREQ_STEP_GLO)*dm2/std::pow(m2+1.0,2);

      // Cast float index results to nearest integer.

      int index1 = static_cast<int>(n1+0.5);
      int index2 = static_cast<int>(n2+0.5);

      // Added data to struct, append to vector in map by SatID.

      if ( n1 != n2 ) return 3; // Error: G1 & G2 results disagree.
      if ( dn1 > 1  ) return 4; // Error: nG1 uncertainty too large.
      if ( dn2 > 1  ) return 5; // Error: nG2 uncertainty too large.

      IndexData tempData;
      tempData.tt  = tt;
      tempData.pG1 = y1.size();
      tempData.pG2 = y2.size();
      tempData.fG1 = n1;
      tempData.fG2 = n2;
      tempData.dG1 = dn1;
      tempData.dG2 = dn2;
      tempData.nG1 = index1;
      tempData.nG2 = index2;

      dataMap[id].push_back(tempData);
   }


   // Method to return the channel index value for a given SV ID.

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


   // Method to return the channel frequency from icd_glo_constants for a given SV ID.

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


   // Dumps data in a nice format.

   void GloFreqIndex::dump(ostream& s) const
   {
      s << "---------------------------------- REQUIRED ----------------------------------";

   } // end of dump()


} // namespace
