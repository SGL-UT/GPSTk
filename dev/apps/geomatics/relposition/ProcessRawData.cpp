#pragma ident "$Id: $"

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
 * @file ProcessRawData.cpp
 * Process raw data, including editing, buffering and computation of a pseudorange
 * solution using RAIM algorithm, part of program DDBase.
 */

//------------------------------------------------------------------------------------
// TD ProcessRawData put back EOP mean of date
// TD ProcessRawData user input pseudorange limits in EditRawData(ObsFile& obsfile)

//------------------------------------------------------------------------------------
// system includes

// GPSTk
#include "EphemerisRange.hpp"

// DDBase
//#include "PreciseRange.hpp"
#include "DDBase.hpp"
#include "index.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// local data
static vector<SatID> Sats;     // used by RAIM, bad ones come back marked (id < 0)

//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// prototypes -- this module only
   // ComputeRAIMSolution.cpp
int ComputeRAIMSolution(ObsFile& of, DayTime& tt, vector<SatID>& Sats);
void RAIMedit(ObsFile& of, vector<SatID>& Sats);
   // here
void FillRawData(ObsFile& of);
void GetEphemerisRange(ObsFile& obsfile, DayTime& timetag);
void EditRawData(ObsFile& of);
int BufferRawData(ObsFile& of);

//------------------------------------------------------------------------------------
int ProcessRawData(ObsFile& obsfile, DayTime& timetag)
{
try {
   int iret;

      // fill RawDataMap for Station
   FillRawData(obsfile);

      // compute nominal elevation and ephemeris range; RecomputeFromEphemeris
      // will re-do after synchronization and before differencing
   GetEphemerisRange(obsfile,timetag);

      // Edit raw data for this station
   EditRawData(obsfile);

      // fill RawDataMap for Station, and compute pseudorange solution
      // return Sats, with bad satellites marked with (id < 0)
   iret = ComputeRAIMSolution(obsfile,timetag,Sats);
   if(iret) {
      if(CI.Verbose) oflog
         << " Warning - ProcessRawData for station " << obsfile.label
         << ", at time "
         << timetag.printf("%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g,")
         << " failed with code " << iret
         << (iret ==  2 ? " (large RMS residual)" :
            (iret ==  1 ? " (large slope)" :
            (iret == -1 ? " (no convergence)" :
            (iret == -2 ? " (singular)" :
            (iret == -3 ? " (not enough satellites)" :
            (iret == -4 ? " (no ephemeris)" :
            (iret == -5 ? " (invalid solution)" :
            " (unknown)")))))))
         << endl;
      // TD change this -- or user input ?
      //if(iret > 0)   iret = 0;      // suspect solution
      if(iret) {
         Stations[obsfile.label].PRS.Valid = false;   // remove data in RAIMedit
      }
   }

      // save statistics on PR solution
   Station& st=Stations[obsfile.label];
   if(st.PRS.Valid) {
      st.PRSXstats.Add(st.PRS.Solution(0));
      st.PRSYstats.Add(st.PRS.Solution(1));
      st.PRSZstats.Add(st.PRS.Solution(2));
   }

      // if user wants PRSolution as a priori, update it here so that the
      // elevation can be computed - this serves to eliminate the low-elevation
      // data from the raw data buffers and simplifies processing.
      // it does not seem to affect the final estimation processing at all...
   if(st.usePRS && st.PRSXstats.N() >= 10) {
      Position prs;
      prs.setECEF(st.PRSXstats.Average(),
                  st.PRSYstats.Average(),
                  st.PRSZstats.Average());
      st.pos = prs;

      if(CI.Debug) oflog << "Update apriori=PR solution for " << obsfile.label
         << " at " << timetag.printf("%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g")
         << fixed << setprecision(5)
         << " " << setw(15) << st.PRSXstats.Average()
         << " " << setw(15) << st.PRSYstats.Average()
         << " " << setw(15) << st.PRSZstats.Average()
         << endl;
   }

      // edit based on RAIM, using Sats
   RAIMedit(obsfile,Sats);

      // buffer raw data, including ER(==0), EL and clock
   iret = BufferRawData(obsfile);
   if(iret) return iret;                  // always returns 0

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void FillRawData(ObsFile& of)
{
try {
   //int nsvs;
   double C1;
   GSatID sat;
   RinexObsData::RinexSatMap::const_iterator it;
   RinexObsData::RinexObsTypeMap otmap;
   RinexObsData::RinexObsTypeMap::const_iterator jt;
   Station& st=Stations[of.label];
   st.RawDataMap.clear();              // assumes one file per site at each epoch

      // loop over sat=it->first, ObsTypeMap=it->second
      // fill DataStruct
   //nsvs = 0;
   for(it=of.Robs.obs.begin(); it != of.Robs.obs.end(); ++it) {
      sat = it->first;
      otmap = it->second;

         // ignore non-GPS satellites
      if(sat.system != SatID::systemGPS) continue;

         // is the satellite excluded?
      if(index(CI.ExSV,sat) != -1) continue;

         // pull out the data
      DataStruct D;
      D.P1 = D.P2 = D.L1 = D.L2 = D.D1 = D.D2 = D.S1 = D.S2 = 0;
      if(of.inP1>-1 && (jt=otmap.find(of.Rhead.obsTypeList[of.inP1])) != otmap.end())
         D.P1 = jt->second.data;
      if(of.inP2>-1 && (jt=otmap.find(of.Rhead.obsTypeList[of.inP2])) != otmap.end())
         D.P2 = jt->second.data;
      if(of.inL1>-1 && (jt=otmap.find(of.Rhead.obsTypeList[of.inL1])) != otmap.end())
         D.L1 = jt->second.data;
      if(of.inL2>-1 && (jt=otmap.find(of.Rhead.obsTypeList[of.inL2])) != otmap.end())
         D.L2 = jt->second.data;
      if(of.inD1>-1 && (jt=otmap.find(of.Rhead.obsTypeList[of.inD1])) != otmap.end())
         D.D1 = jt->second.data;
      if(of.inD2>-1 && (jt=otmap.find(of.Rhead.obsTypeList[of.inD2])) != otmap.end())
         D.D2 = jt->second.data;
      if(of.inS1>-1 && (jt=otmap.find(of.Rhead.obsTypeList[of.inS1])) != otmap.end())
         D.S1 = jt->second.data;
      if(of.inS2>-1 && (jt=otmap.find(of.Rhead.obsTypeList[of.inS2])) != otmap.end())
         D.S2 = jt->second.data;
      if(of.inC1>-1 && (jt=otmap.find(of.Rhead.obsTypeList[of.inC1])) != otmap.end())
         C1 = jt->second.data;

      // if P1 is not available, but C1 is, use C1 in place of P1
      if((of.inP1 == -1 || D.P1 == 0) && of.inC1 > -1 &&
         (jt=otmap.find(of.Rhead.obsTypeList[of.inC1])) != otmap.end() )
            D.P1 = jt->second.data;

      st.RawDataMap[sat] = D;
      st.time = SolutionEpoch;
      //nsvs++;

   }  // end loop over sats
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end FillRawData()

//------------------------------------------------------------------------------------
void GetEphemerisRange(ObsFile& obsfile, DayTime& timetag)
{
try {
   CorrectedEphemerisRange CER;        // temp
   //PreciseRange CER;

   Station& st=Stations[obsfile.label];

   map<GSatID,DataStruct>::iterator it;
   for(it=st.RawDataMap.begin(); it != st.RawDataMap.end(); it++) {

      // ER cannot be used until the a priori positions are computed --
      // because user may want the PRSolution as the a priori, we must wait.
      // This will be updated in RecomputeFromEphemeris(), after Synchronization()
      it->second.ER = 0.0;

      // this will happen when user has chosen to use the PRSolution as the a priori
      // and the st.pos has not yet been updated
      if(st.pos.getCoordinateSystem() == Position::Unknown) {
         it->second.elev = 90.0;       // include it in the PRS
         it->second.az = 0.0;
         continue;
      }

      // TD why did PreciseRange not throw here?
      // catch NoEphemerisFound and set elevation -90 --> edited out later
      try {
         //it->second.ER =
            CER.ComputeAtReceiveTime(timetag, st.pos, it->first, *pEph);
         it->second.elev = CER.elevation;  // this will be compared to PRS elev Limit
         it->second.az = CER.azimuth;
      }
      catch(EphemerisStore::NoEphemerisFound& e) {
         if(CI.Verbose)
            oflog << "No ephemeris found for sat " << it->first << " at time "
                  << timetag.printf("%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
         //it->second.ER = 0.0;
         it->second.elev = -90.0;         // do not include it in the PRS
         it->second.az = 0.0;
      }

   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void EditRawData(ObsFile& obsfile)
{
try {
   int i;

   Station& st=Stations[obsfile.label];

   vector<GSatID> BadSVs;
   map<GSatID,DataStruct>::iterator it;
   for(it=st.RawDataMap.begin(); it != st.RawDataMap.end(); it++) {
      if(
         // DON'T DO THIS - clock may get large and negative, leading to negative PR
         // bad pseudorange
         //   (CI.Frequency != 2 && it->second.P1 < 1000.0) ||   // TD
         //   (CI.Frequency != 1 && it->second.P2 < 1000.0) ||
            // below elevation cutoff (for PRS)
         (it->second.elev < CI.PRSMinElevation)
         )//end if
      {
         BadSVs.push_back(it->first);
      }
   }

      // delete the bad satellites
   for(i=0; i<BadSVs.size(); i++) {
      st.RawDataMap.erase(BadSVs[i]);
   }

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end EditRawData()

//------------------------------------------------------------------------------------
// add good raw data in RawDataMap to RawDataBuffers for the appropriate station
// and satellite. Also buffer the clock solution and sigma.
// NB these buffers must remain parallel.
int BufferRawData(ObsFile& obsfile)
{
try {
   int n;

   Station& st=Stations[obsfile.label];

   map<GSatID,DataStruct>::iterator it;
   map<GSatID,RawData>::iterator jt;
      // loop over satellites
   for(n=0,it=st.RawDataMap.begin(); it != st.RawDataMap.end(); it++) {

      // find iterator for this sat in Buffers map
      jt = st.RawDataBuffers.find(it->first);
      if(jt == st.RawDataBuffers.end()) {
         RawData rd;
         st.RawDataBuffers[it->first] = rd;
         jt = st.RawDataBuffers.find(it->first);
      }

      // buffer the data -- keep parallel with count and clock
      jt->second.count.push_back(Count);
      jt->second.L1.push_back(it->second.L1);
      jt->second.L2.push_back(it->second.L2);
      jt->second.P1.push_back(it->second.P1);
      jt->second.P2.push_back(it->second.P2);
      jt->second.ER.push_back(it->second.ER);
      jt->second.elev.push_back(it->second.elev);
      jt->second.az.push_back(it->second.az);
      n++;
   }

      // now buffer the clock solution and the timetag offset
      // and buffer the (Station) count if there is some data
      // NB these are NOT necessarily parallel to raw data buffers
   if(st.PRS.isValid()) {
      st.ClockBuffer.push_back(st.PRS.Solution(3));
      st.ClkSigBuffer.push_back(st.PRS.Covariance(3,3));
      st.RxTimeOffset.push_back(SolutionEpoch - obsfile.Robs.time);
   }
   else {
      st.ClockBuffer.push_back(0.0);
      st.ClkSigBuffer.push_back(0.0);
      st.RxTimeOffset.push_back(0.0);
   }
   st.CountBuffer.push_back(Count);

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
