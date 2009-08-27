#pragma ident "$Id$"

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
 * @file Synchronization.cpp
 * Interpolate the phase data to correct for the clock offset, synchronizing the
 * data at different stations; part of program DDBase.
 */

//------------------------------------------------------------------------------------
// TD Synchronization.cpp make number of phase points in fit an input parameter
// TD Synchronization.cpp make MaxGap=10; an input parameter and
// TD Synchronization.cpp use this in EditRawDataBuffers() to remove single points
// TD Synchronization.cpp   that have gaps larger than this on each side of them.

//------------------------------------------------------------------------------------
// includes
// system
#include <deque>

// GPSTk
#include "geometry.hpp"             // DEG_TO_RAD
#include "PolyFit.hpp"
#include "EphemerisRange.hpp"

// DDBase
#include "DDBase.hpp"
#include "PhaseWindup.hpp"
#include "SunEarthSatGeometry.hpp"
#include "index.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- this module only -- called by Synchronization()
void FitPhaseAndMoveData(GSatID& sat, string site, Station& st, RawData& rd, int freq)
   throw(Exception);

//------------------------------------------------------------------------------------
int Synchronization(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN Synchronization()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;

   GSatID sat;
   map<string,Station>::iterator it;
   map<GSatID,RawData>::iterator jt;

      // loop over stations
   for(it=Stations.begin(); it != Stations.end(); it++) {
      //string label = it->first;
      Station& st=it->second;
         // loop over satellites
      for(jt=st.RawDataBuffers.begin(); jt != st.RawDataBuffers.end(); jt++) {
         sat = jt->first;
         RawData& rawdat=jt->second;

         if(rawdat.count.size() == 0) continue;

            // Loop over all points in the buffers, using a sliding window.
            // For each window, fit a polynomial to the phase data.
            // At each point, evaluate the polynomial at the true receive time.
         if(CI.Frequency != 2)
            FitPhaseAndMoveData(sat,it->first,st,rawdat,1);
         if(CI.Frequency != 1)
            FitPhaseAndMoveData(sat,it->first,st,rawdat,2);

      }  // loop over sats

   }  // loop over stations
   
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end Synchronization()

//------------------------------------------------------------------------------------
// Process using a sliding window:
// loop over all points in the buffers of RawData, using a sliding window of fixed
// length which is centered (as much as possible) about the buffer point of interest.
// Process each buffer point using the data in the sliding window.
void FitPhaseAndMoveData(GSatID& sat, string site, Station& statn, RawData& rawdat,
      int freq) throw(Exception)
{
try {
   const int N=11;   // size of the window // best odd  // TD make input
   const int D=3;    // degree of polynomial to be fit   // TD make input
   bool change;      // mark a change in the deques --> fit a new polynomial
   int nc;           // index into the buffer at the current point
   int nbeg;         // index into the buffer at the start of the window
   int nend;         // index into the buffer at the end of the window
   int nhalf=N/2;    // half the window size
   int len;          // length of the buffers
   int ngap;         // number of counts between the end pt (nend) and the next
   int nsize;        // size of the sliding window (deques)
   int i,j;
   double x,x0,d0,dx,dph;
   PolyFit<double> PF;// fit polynomials to phase
   deque<int> dc;     // the sliding window : time -- keep the deques 
   deque<double> dp;  // the sliding window : data -- parallel

   //if(CI.Verbose) oflog << "BEGIN FitPhasesAndMoveData() for site " << site
   //   << " and sat " << sat << " at total time " << fixed << setprecision(3)
   //   << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
   //   << endl;

      // starting: nend is before the current point (0)
   nbeg = 0;
   nend = -1;
   change = true;
   len = int(rawdat.count.size());  // length of the buffers

      // Loop over count (epochs). At each count, fill a 'sliding window' deque
      // (one for each of count, L1 and L2) with up to N points, including
      // containing the current count. The points run from index nbeg to nend.
   for(nc=0; nc<len; nc++) {
         // -------------------------------------------------------------
         // the only way this could be true is if the current point is the
         // first point past a big (>=MaxGap) gap
      if(nc > nend) {
            // clear window and start again
         dc.clear();
         dp.clear();
         nbeg = nend = nc;
         ngap = rawdat.count[nend+1]-rawdat.count[nend];
         if(ngap >= CI.MaxGap) continue;        // skip this point if there's a gap
         dc.push_back(rawdat.count[nend]);      // time / DataInterval
         dp.push_back(freq == 1 ? rawdat.L1[nend] :
                                  rawdat.L2[nend]);         // cycles
         change = true;
      }

         // -------------------------------------------------------------
         // advance the end of the window (nend) when all these are true:
      while(   (nend < len-1)       // point is not beyond the end of the buffer
            && (nend-nbeg+1 < N)    // & the window is not full
                                    // & there is not a big gap
            && ((ngap = rawdat.count[nend+1]-rawdat.count[nend]) < CI.MaxGap)
            && (nc >= nbeg)         // & the current point will stay in window
            ) {
            // expand the window one point into the future
         nend++;
         dc.push_back(rawdat.count[nend]);      // keep the deques parallel
         dp.push_back(freq == 1 ? rawdat.L1[nend] :
                                  rawdat.L2[nend]);
         change = true;
      };

         // -------------------------------------------------------------
         // is this an isolated point?
      //if(ngap >= CI.MaxGap) {
      //   nc = nend+1;
      //   continue;
      //}

         // -------------------------------------------------------------
         // Process the deques when a change has been made
      if(change) {
            // size of the sliding window (deques)
         nsize = dc.size();

            // must not have isolated points
            // EditRawBuffers should have removed these
         if(nsize < 2) {
            Exception e( (nsize == 0 ?
               string("ERROR - empty window") : string("ERROR - isolated point"))
               + string(" for station ") + site + string(" and satellite ")
               + sat.toString()
               + string(" at count ") + StringUtils::asString(rawdat.count[nc])
               + string(" = time ")
               + (FirstEpoch + rawdat.count[nc]*CI.DataInterval).printf(
                  "%Y/%m/%d %H:%02M:%6.3f = %F/%10.3g"));
            GPSTK_THROW(e);
         }

         // fit a polynomial of degree D to the points in deques
         PF.Reset(D<nsize ? D : nsize);

            // debias using the first point
         x0 = double(dc[0]);
         d0 = dp[0];

            // use all the data in the sliding window
         for(i=0; i<nsize; i++) {
               // x is nominal receive time in units of count (DataInterval)
            x = double(dc[i]);
               // find the same count in the station buffers
            j = index(statn.CountBuffer,dc[i]);
            //if(j == -1) ?? TD
            x -= statn.RxTimeOffset[j]/CI.DataInterval;
            PF.Add(dp[i]-d0,x-x0);
         }

         change = false;

         //if(CI.Debug)
         //   for(i=0; i<nsize; i++) {
         //      x = double(dc[i]);         // count
         //      j = index(statn.CountBuffer,dc[i]);
         //      x -= statn.RxTimeOffset[j]/CI.DataInterval;
         //      //PF.Add(dp[i]-d0,x-x0);
         //      oflog << "FIT " << site << " " << sat
         //         << " " << nc << " " << rawdat.count[nc]
         //         << " " << (D<nsize?D:nsize) << " " << nsize
         //         << fixed << setprecision(6)
         //         << " " << nbeg+i << " " << dc[i] << " " << rawdat.count[nbeg+i]
         //         << " " << x-x0 << " " << dp[i]-d0
         //         << " " << PF.Evaluate(x-x0)
         //         << " " << dp[i]-d0 - PF.Evaluate(x-x0)
         //         << endl;
         //}

      }  // end if change

         // -------------------------------------------------------------
         // Process each point in the window/buffer
         // correct L1,L2,P1,P2 using the polynomials and dt=RxTTOffset+clk/c
         // statn.ClockBuffer contains raw PRS clock solution
         // statn.RxTimeOffset contains SolutionEpoch - Rx timetag
         //
         // nominal time for point nc
      x = double(rawdat.count[nc]);
         // find the index of the same count in the station buffers
      j = index(statn.CountBuffer,rawdat.count[nc]);
         // time difference due to receiver clock, in units of count
      dx =  statn.RxTimeOffset[j]/CI.DataInterval
         + (statn.ClockBuffer[j]/C_GPS_M)/CI.DataInterval;
         // change in phase between nominal and true time
      dph = PF.Evaluate(x-x0) - PF.Evaluate(x-dx-x0);
      if(freq == 1) {
         rawdat.L1[nc] += dph;
         rawdat.P1[nc] += dph * wl1;
      }
      else {
         rawdat.L2[nc] += dph;
         rawdat.P2[nc] += dph * wl2;
      }

      //if(CI.Debug) oflog << "FIT " << site << " " << sat
      //   << " " << nc << " " << rawdat.count[nc]
      //   << fixed << setprecision(6)
      //   << " " << x-x0 << " " << dx
      //   << " " << statn.RxTimeOffset[nc]
      //   << " " << statn.ClockBuffer[nc]/C_GPS_M
      //   << " " << dph << " eval" << endl;

         // -------------------------------------------------------------
         // remove old point(s) from the deques
      while(   (nend < len-1)      // a new end point would not go beyond buffer
            && (ngap < CI.MaxGap)  // & there would not be a big gap
            && (nend-nbeg+1 > N-1) // & window is full
            && (nc >= nbeg+nhalf)  // & current point is at mid-window or later
            ) {
         dc.pop_front();      // keep the deques parallel
         dp.pop_front();
         nbeg++;
         change = true;
      };

   }  // loop over counts

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
} // end FitPhaseAndMoveData

//------------------------------------------------------------------------------------
int RecomputeFromEphemeris(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN RecomputeFromEphemeris()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;

   int nc;
   double angle,pwu,shadow;
   double prevwindup = 0.0;
   DayTime tt;
   GSatID sat;
   Position SV;
   Position West,North,Rx2Tx;
   CorrectedEphemerisRange CER;  // TD PreciseRange?
   map<string,Station>::iterator it;
   map<GSatID,RawData>::iterator jt;

      // loop over stations
   for(it=Stations.begin(); it != Stations.end(); it++) {
      //string label = it->first;
      Station& statn=it->second;

      //if(CI.Verbose) oflog << " Station " << it->first
      //   << " with " << statn.RawDataBuffers.size() << " raw buffers." << endl;

         // compute W and N unit vectors at this station,
         // rotated by antenna azimuth angle
      angle = statn.ant_azimuth * DEG_TO_RAD;
      if(fabs(angle) > 0.0001) {    // also below..
         Matrix<double> Rot;
         Rot = SingleAxisRotation(angle,1) * UpEastNorth(statn.pos);
         West = Position(-Rot(1,0),-Rot(1,1),-Rot(1,2));
         North = Position(Rot(2,0),Rot(2,1),Rot(2,2));
      }

         // loop over satellites
      for(jt=statn.RawDataBuffers.begin(); jt!=statn.RawDataBuffers.end(); jt++) {
         sat = jt->first;
         RawData& rawdat=jt->second;

         //if(CI.Verbose) oflog << "   Satellite " << sat
         //   << " with buffer size " << rawdat.count.size() << endl;

         if(rawdat.count.size() == 0) continue;

            // Loop over count (epochs). At each count, recompute the ephemeris
            // range and correct the phase for phase windup.
         for(nc=0; nc<rawdat.count.size(); nc++) {

               // nominal time is now the actual receive time of the data
            tt = FirstEpoch + rawdat.count[nc] * CI.DataInterval;

               // try to get the ephemeris info
            try {
                  // update ephemeris range and elevation
               rawdat.ER[nc] =
                  CER.ComputeAtReceiveTime(tt, statn.pos, sat, *pEph);
               rawdat.elev[nc] = CER.elevation;
               rawdat.az[nc] = CER.azimuth;

                  // correct for phase windup
               if(fabs(angle) > 0.0001) {    // also above..
                     // get the receiver-to-transmitter unit vector
                     // and the satellite position
                  Rx2Tx = Position(CER.cosines);
                  SV = Position(CER.svPosVel.x[0],
                                CER.svPosVel.x[1],
                                CER.svPosVel.x[2]);

                     // compute phase windup
                  pwu = PhaseWindup(prevwindup,tt,SV,Rx2Tx,West,North,shadow);
                  prevwindup = pwu;

                  // TD eclipse alert
                  //if(shadow > 0.0) { ... }

                     // correct the phase
                  rawdat.L1[nc] += pwu * wl1;
                  rawdat.L2[nc] += pwu * wl2;
               }
            }
            catch(InvalidRequest& e) {
               // these should have been caught and removed before...
               oflog << "Warning - No ephemeris found for sat " << sat
                     << " at time "
                     << tt.printf("%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g")
                     << " in RecomputeFromEphemeris()" << endl;
               rawdat.ER[nc] = 0.0;
               rawdat.elev[nc] = -90.0;
               rawdat.az[nc] = 0.0;
            }
         }  // end loop over counts
      }  // loop over sats

   }  // loop over stations

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
