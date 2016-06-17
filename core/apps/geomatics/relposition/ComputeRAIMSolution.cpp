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

/**
 * @file ComputeRAIMSolution.cpp
 * Compute a pseudorange solution using a RAIM algorithm, and edit data based on
 * the result, for program DDBase.
 */

//------------------------------------------------------------------------------------
// includes
// system
#include <fstream>
#include "TimeString.hpp"

// GPSTk

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// called by ProcessRawData
int ComputeRAIMSolution(ObsFile& of, CommonTime& tt, vector<SatID>& Sats, ofstream *pofs)
   throw(Exception)
{
try {
   int nsvs,iret;
   size_t i;
   double PR;
   vector<double> Ranges;
   format f166(16,6),f62(6,2),f51(5,1),f82s(8,2,true);

   Sats.clear();

   if(CI.noRAIM) return 0;    // this option is commented out in CommandInput
   if(CI.Debug) oflog << "CRS for file " << of.name << ", site " << of.label << endl;

      // station associated with ObsFile
   Station& st=Stations[of.label];

      // pull data out of raw data map
   map<GSatID,DataStruct>::iterator it;
   for(nsvs=0,it=st.RawDataMap.begin(); it != st.RawDataMap.end(); it++) {
         // use dual frequency if you have it
      if(it->second.P1 != 0 && it->second.P2 != 0)
         PR = if1r * it->second.P1 + if2r * it->second.P2;
      else if(it->second.P1 == 0 && it->second.P2 == 0) PR = 0.0;
      else if(it->second.P1 == 0)
         PR = it->second.P2;
      else
         PR = it->second.P1;

      if(PR != 0.0) {
         Sats.push_back(SatID(it->first));
         Ranges.push_back(PR);
         nsvs++;
      }
   }

   if(CI.Debug) {
      oflog << "Satellites and Ranges before Compute:\n";
      for(i=0; i<Ranges.size(); i++)
         oflog << " " << setw(2) << GSatID(Sats[i]) << fixed
            << " " << setw(13) << setprecision(3) << Ranges[i] << endl;
   }

      // compute a RAIM solution, hence need more than 4 satellites
   if(nsvs <= 4) {
      if(CI.Verbose) oflog << "Not enough data to compute RAIM solution for file "
         << of.name << " at time "
         << printTime(tt,"%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
      return -2;
   }

   iret = st.PRS.RAIMCompute(tt, Sats, Ranges, *pEph, CI.pTropModel);

   if(iret < 0) {
      if(iret == -4)
         oflog << "RAIM Solution failed to find ephemeris";
      if(iret == -3)
         oflog << "Not enough data for a RAIM solution";
      if(iret == -2)
         oflog << "Singular RAIM problem";
      oflog << " for file " << of.name << " at time "
         << printTime(tt,"%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
      return iret;
   }

   if(iret > 0) {
      //oflog << "RAIM solution is suspect (" << iret << ")" << endl;
      return iret;
   }
   if(!st.PRS.isValid()) return -5;
   for(nsvs=0,i=0; i<Sats.size(); i++) if(Sats[i].id > 0) nsvs++;

   if(iret < 0 || nsvs <= 4) {                // did not compute a solution
      if(CI.Verbose) oflog << "At " << SolutionEpoch
         << " RAIM returned " << iret << endl;
      st.PRS.Valid = false;
      if(iret >= 0) return -3;
      return iret;
   }

   // output to OutputPRSFile, opened in ReadAndProcessRawData()
   if(pofs) {
      *pofs << "PRS " << of.label << " " << setw(2) << nsvs
         << printTime(tt," %4F %10.3g");

      // if a Position is defined for this Station, output residuals
      if(st.pos.getSystemName() != string("Unknown"))
         *pofs
         << " " << f166 << st.PRS.Solution(0)-st.pos.X()
         << " " << f166 << st.PRS.Solution(1)-st.pos.Y()
         << " " << f166 << st.PRS.Solution(2)-st.pos.Z();
      else
         *pofs
         << " " << f166 << st.PRS.Solution(0)
         << " " << f166 << st.PRS.Solution(1)
         << " " << f166 << st.PRS.Solution(2);

      *pofs
         << " " << f166 << st.PRS.Solution(3)
         << " " << f62 << st.PRS.RMSResidual
         << " " << f51 << st.PRS.MaxSlope
         << " " << st.PRS.NIterations
         << " " << f82s << st.PRS.Convergence;
      for(i=0; i<Sats.size(); i++) *pofs << " " << setw(3) << Sats[i].id;
      *pofs << " (" << iret << ")" << (st.PRS.isValid() ? " V" : " NV");
      *pofs << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end ComputeRAIMSolution()

//------------------------------------------------------------------------------------
void RAIMedit(ObsFile& of, vector<SatID>& Sats) throw(Exception)
{
try {
   size_t i;
   Station& st=Stations[of.label];

   if(!st.PRS.Valid) {
      st.RawDataMap.clear();
      return;
   }

      // delete satellites that were marked by RAIM
   for(i=0; i<Sats.size(); i++) {
      if(Sats[i].id > 0) continue;
      Sats[i].id *= -1;
      st.RawDataMap.erase(GSatID(Sats[i]));
   }

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
