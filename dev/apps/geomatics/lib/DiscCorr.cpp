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

/**
 * @file DiscCorr.cpp
 * GPS phase discontinuity correction. Given a SatPass object
 * containing dual-frequency pseudorange and phase for an entire satellite pass,
 * and a configuration object (as defined herein), detect discontinuities in
 * the phase and, if possible, estimate their size.
 * Output is in the form of Rinex editing commands (see class RinexEditor).
 */

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>

#include "StringUtils.hpp"
#include "Stats.hpp"
#include "PolyFit.hpp"
#include "icd_200_constants.hpp"    // PI,C_GPS_M,OSC_FREQ,L1_MULT,L2_MULT
#include "RobustStats.hpp"

#include "DiscCorr.hpp"

using namespace std;
using namespace gpstk;
using namespace StringUtils;

// only to unclutter the top of this file; not included anywhere else...
#include "DCinternals.hpp"

//------------------------------------------------------------------------------------
// NB. string giving version of GDC (GDCVersion) is found in GDCconfiguration.cpp

// these are used only to associate a unique number in the log file with each pass
int GDCUnique=0;     // unique number for each call
int GDCUniqueFix;    // unique for each (WL,GF) fix

// conveniences only...
#define log *(p_oflog)
#define cfg(a) cfg_func(#a)

//------------------------------------------------------------------------------------
// These from SatPass.cpp
//const unsigned short SatPass::BAD = 0; // used by caller and DC to mark bad data
//const unsigned short SatPass::OK  = 1; // good data, no discontinuity
//const unsigned short SatPass::LL1 = 2; // good data, discontinuity on L1 only
//const unsigned short SatPass::LL2 = 4; // good data, discontinuity on L2 only
//const unsigned short SatPass::LL3 = 6; // good data, discontinuity on L1 and L2
const unsigned short GDCPass::WLDETECT =   2;
const unsigned short GDCPass::GFDETECT =   4;
const unsigned short GDCPass::DETECT   =   6;  // = WLDETECT | GFDETECT
const unsigned short GDCPass::WLFIX    =   8;
const unsigned short GDCPass::GFFIX    =  16;
const unsigned short GDCPass::FIX      =  24;  // = WLFIX | GFFIX

// notes on the use of these flags:
// if(flag & DETECT) is true for EITHER WL or GF or both
// if(flag & FIX)  is true for EITHER WL or GF or both
// if((flag & WLDETECT) && (flag & GFDETECT)) is true only for both WL and GF

// NB typical slip will have flag = DETECT+OK+FIX = 31
//    typical unfixed slip   flag = DETECT+OK     =  7

// BAD is used either as flag == BAD (for bad data) or flag != BAD (for good data),
// thus there are two gotcha's
//   - if a point is marked, but is later set BAD, that info is lost
//   - if a BAD point is marked, it becomes 'good'
// To avoid this we have to use OK rather than BAD:
// either !(flag & OK) or (flag ^ OK) for bad data, and (flag & OK) for good data

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// yes you need the gpstk::
int gpstk::DiscontinuityCorrector(SatPass& svp,
                                  GDCconfiguration& gdc,
                                  vector<string>& editCmds)
   throw(Exception)
{
try {
   int iret;
   GDCUnique++;

   // create the GDCPass from the input SatPass and the input GDC configuration
   GDCPass gp(svp,gdc);
   gp.initialize();

   // NB search for 'change the arrays' for places where arrays are re-defined
   // NB search for 'change the data' for places where the data is modified (! biases)
   // NB search for 'change the bias' for places where the bias is changed
   for(;;) {      // a convenience...
      // preparation
      if( (iret = gp.preprocess() )) break;
      if( (iret = gp.linearCombinations() )) break;

      // WL
      if( (iret = gp.detectWLslips() )) break;
      if( (iret = gp.fixAllSlips("WL") )) break;

      // GF
      if( (iret = gp.prepareGFdata() )) break;
      if( (iret = gp.detectGFslips() )) break;
      if( (iret = gp.WLconsistencyCheck() )) break;
      if( (iret = gp.fixAllSlips("GF") )) break;

      break;      // mandatory
   }

   // generate editing commands for deleted (flagged) data and slips,
   // use editing command (slips and deletes) to modify the original SatPass data
   // and print ending summary
   gp.finish(iret, svp, editCmds);

   // clear the temp arrays
   gp.clearTempArrays();

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GDCPass::preprocess(void) throw(Exception)
{
try {
   int i,ilast,Ngood;
   double biasL1,biasL2,dbias;
   list<Segment>::iterator it;

   if(cfg(Debug) >= 2) {
      DayTime CurrentTime;
      CurrentTime.setLocalTime();
      log << "\n======== Beg GPSTK Discontinuity Corrector " << GDCUnique
         << " ================================================\n";
      log << "GPSTK Discontinuity Corrector Ver. " << GDCVersion << " Run "
         << CurrentTime << endl;
   }

      // check input
   if(cfg(DT) <= 0) {
      log << "Error: data time interval is not set...Abort" << endl;
      return FatalProblem;
   }

      // create the first segment
   SegList.clear();
   {
      Segment s;
      s.nseg = 1;
      SegList.push_back(s);
   }
   it = SegList.begin();

      // loop over points in the pass
      // editing obviously bad data and adding segments where necessary
   for(ilast=-1,i=0; i<data.size(); i++) {

      if(!(data[i].flag & OK)) continue;

      // just in case the caller has set it to something else...
      data[i].flag = OK;

         // look for obvious outliers
         // Don't do this - sometimes the pseudoranges get extreme values b/c the
         // clock is allowed to run off for long times - perfectly normal
      //if(data[i].P1 < cfg(MinRange) || data[i].P1 > cfg(MaxRange) ||
      //   data[i].P2 < cfg(MinRange) || data[i].P2 > cfg(MaxRange) )
      //{
      //   data[i].flag = BAD;
      //   learn["points deleted: obvious outlier"]++;
      //   if(cfg(Debug) > 6)
      //      log << "Obvious outlier " << GDCUnique << " " << sat
      //         << " at " << i << " " << time(i).printf(outFormat) << endl;
      //   continue;
      //}

         // note first good point
      if(ilast == -1) ilast = it->nbeg = i;

         // is there a gap here? if yes, create a new segment
         // TD? do this here? why not allow any gap in the WL, and look for gaps
         // TD? only in the GF?
      if(cfg(DT)*(i-ilast) > cfg(MaxGap))
         it = createSegment(it,i,"initial gap");

         // count good points
      it->npts++;
      ilast = i;
   }

      // note last good point
   if(ilast == -1) ilast = it->nbeg;
   it->nend = ilast;

      // 'change the arrays' A1, A2 to be range minus phase for output
      // do the same at the end ("AFT")
      // loop over segments, counting the number of non-trivial ones
   for(Ngood=0,it=SegList.begin(); it != SegList.end(); it++) {
      biasL1 = biasL2 = 0.0;

         // loop over points in this segment
      for(i=it->nbeg; i<=it->nend; i++) {
         if(!(data[i].flag & OK)) continue;

         dbias = fabs(data[i].P1-wl1*data[i].L1-biasL1);
         if(dbias > cfg(RawBiasLimit)) {
            if(cfg(Debug) >= 2) log << "BEFresetL1 " << GDCUnique
               << " " << sat << " " << time(i).printf(outFormat)
               << " " << fixed << setprecision(3) << biasL1
               << " " << data[i].P1 - wl1 * data[i].L1 << endl;
            biasL1 = data[i].P1 - wl1 * data[i].L1;
         }

         dbias = fabs(data[i].P2-wl2*data[i].L2-biasL2);
         if(dbias > cfg(RawBiasLimit)) {
            if(cfg(Debug) >= 2) log << "BEFresetL2 " << GDCUnique
               << " " << sat << " " << time(i).printf(outFormat)
               << " " << fixed << setprecision(3) << biasL2
               << " " << data[i].P2 - wl2 * data[i].L2 << endl;
            biasL2 = data[i].P2 - wl2 * data[i].L2;
         }

         A1[i] = data[i].P1 - wl1 * data[i].L1 - biasL1;
         A2[i] = data[i].P2 - wl2 * data[i].L2 - biasL2;

      }  // end loop over points in the segment

         // delete small segments
      if(it->npts < int(cfg(MinPts)))
         deleteSegment(it,"insufficient data in segment");
      else
         Ngood++;
   }

   if(cfg(Debug) >= 2) dumpSegments("BEF",2,true);

   if(Ngood == 0) return NoData;
   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GDCPass::linearCombinations(void) throw(Exception)
{
try {
   int i;
   double wlr,wlp,wlbias,gfr,gfp;
   list<Segment>::iterator it;

   // iterate over segments
   for(it=SegList.begin(); it != SegList.end(); it++) {
      it->npts = 0;                       // re-compute npts here

      // loop over points in this segment
      for(i=it->nbeg; i<=it->nend; i++) {
         if(!(data[i].flag & OK)) continue;

         wlr = wl1r * data[i].P1 + wl2r * data[i].P2;    // narrow lane range (m)
         wlp = wl1p * data[i].L1 + wl2p * data[i].L2;    // wide lane phase (m)
         gfr =        data[i].P1 -        data[i].P2;    // geometry-free range (m)
         gfp = gf1p * data[i].L1 + gf2p * data[i].L2;    // geometry-free phase (m)
         wlbias = (wlp-wlr)/wlwl;                        // wide lane bias (cycles)

         // change the bias
         if(it->npts == 0) {                             // first good point
            it->bias1 = wlbias;                          // WL bias (NWL)
            it->bias2 = gfp;                             // GFP bias
         }

         // change the arrays
         //data[i].L1 = unused!
         data[i].L2 = gfp;
         data[i].P1 = wlbias;
         data[i].P2 = -gfr;

         it->npts++;
      }
   }

   if(cfg(Debug) >= 2) dumpSegments("LCD");

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// detect slips in the wide lane bias
int GDCPass::detectWLslips(void) throw(Exception)
{
try {
   int iret;
   list<Segment>::iterator it;

   // look for obvious slips. this will break one segment into many
   if( (iret = detectObviousSlips("WL"))) return iret;

   for(it=SegList.begin(); it != SegList.end(); it++) {

      // compute stats and delete segments that are too small
      WLcomputeStats(it);

      // sigma-strip the WL bias, and remove small segments
      if(it->npts > 0) WLsigmaStrip(it);

      // print this before deleting segments with large sigma
      if(cfg(Debug) >= 1 && it->npts >= int(cfg(MinPts)))
         log << "WLSIG " << GDCUnique << " " << sat
            << " " << it->nseg
            << " " << time(it->nbeg).printf(outFormat)
            << fixed << setprecision(3)
            << " " << it->WLStats.StdDev()
            << " " << it->WLStats.Average()
            << " " << it->WLStats.Minimum()
            << " " << it->WLStats.Maximum()
            << " " << it->npts
            << " " << it->nbeg << " - " << it->nend
            << " " << it->bias1
            << " " << it->bias2
            << endl;

      // delete segments if sigma is too high...
      if(it->WLStats.StdDev() > cfg(WLNSigmaDelete)*cfg(WLSigma))
         deleteSegment(it,"WL sigma too big");

      // if there are less than about 2.5*cfg(WLWindowWidth) good points, don't bother
      // to use the sliding window method to look for slips; otherwise
      // compute stats for each segment using the 'two-paned sliding stats window',
      // store results in the temporary arrays
      if(double(it->npts) >= cfg(WLNWindows) * int(cfg(WLWindowWidth))) {
         iret = WLstatSweep(it,int(cfg(WLWindowWidth)));
         if(iret) return iret;
      }

   }  // end loop over segments

   // use the temporary arrays filled by WLstatSweep to detect slips in the WL bias
   // recompute WLstats, and break up the segments where slips are found
   if( (iret = detectWLsmallSlips())) return iret;

   // delete all segments that are too small
   for(it=SegList.begin(); it != SegList.end(); it++) {
      if(it->npts < int(cfg(MinPts)))
         deleteSegment(it,"insufficient data in segment");
   }

   if(cfg(Debug) >= 4) dumpSegments("WLD");

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// detect obvious slips by computing the first difference (of either WL or GFP)
// and looking for outliers. create new segments where there are slips
// which is either 'WL' or 'GF'.
int GDCPass::detectObviousSlips(string which) throw(Exception)
{
try {
      // TD determine from range noise // ~ 2*range noise/wl2
   const double WLobviousNwlLimit=cfg(WLobviousLimit)*cfg(WLSigma);
   const double GFobviousNwlLimit=cfg(GFobviousLimit)*cfg(GFVariation)/wl21;
   bool outlier;
   int i,j,iret,ibad,igood,nok;
   double limit,wlbias;
   list<Segment>::iterator it;

   // compute 1st differences of (WL bias, GFP-GFR) as which is (WL,GF)
   iret = firstDifferences(which);
   if(iret) return iret;

   if(cfg(Debug) >= 5) dumpSegments(string("D")+which,2,true); // DWL DGF

   // scan the first differences, eliminate outliers and
   // break into segments where there are WL slips.
   limit = (which == string("WL") ? WLobviousNwlLimit : GFobviousNwlLimit);
   it = SegList.begin();
   nok = 0;
   igood = -1;
   outlier = false;
   for(i=0; i<data.size(); i++) {
      if(i < it->nbeg) {
         outlier = false;
         continue;
      }
      if(i > it->nend) {                  // change segments
         if(outlier) {
            if(data[ibad].flag & OK) nok--;
            data[ibad].flag = BAD;
            learn[string("points deleted: ") + which + string(" slip outlier")]++;
            outlier = false;
         }
         it->npts = nok;
         // update nbeg and nend
         while(it->nbeg < it->nend
            && it->nbeg < data.size()
            && !(data[it->nbeg].flag & OK) ) it->nbeg++;
         while(it->nend > it->nbeg
            && it->nend > 0
            && !(data[it->nend].flag & OK) ) it->nend--;
         it++;
         if(it == SegList.end())
            return ReturnOK;
         nok = 0;
      }

      if(!(data[i].flag & OK))
         continue;
      nok++;                                 // nok = # good points in segment

      if(igood == -1) igood = i;             // igood is index of last good point

      if(fabs(A1[i]) > limit) {              // found an outlier (1st diff, cycles)
         outlier = true;
         ibad = i;                           // ibad is index of last bad point
      }
      else if(outlier) {                     // this point good, but not past one(s)
         for(j=igood+1; j<ibad; j++) {
            if(data[j].flag & OK)
               nok--;
            if(data[j].flag & DETECT)
               log << "Warning - found an obvious slip, "
                  << "but marking BAD a point already marked with slip "
                  << GDCUnique << " " << sat
                  << " " << time(j).printf(outFormat) << " " << j << endl;
            data[j].flag = BAD;             // mark all points between as bad
            learn[string("points deleted: ") + which + string(" slip outlier")]++;
         }

            // create a new segment, starting at the last outlier
         it->npts = nok-2;
         // WL slip gross  OR  GF slip gross
         it = createSegment(it,ibad,which+string(" slip gross"));

            // mark it
         data[ibad].flag |= (which == string("WL") ? WLDETECT : GFDETECT);

            // change the bias in the new segment
         if(which == "WL") {
            wlbias = data[ibad].P1;
            it->bias1 = long(wlbias+(wlbias > 0 ? 0.5 : -0.5));   // WL bias (NWL)
         }
         if(which == "GF")
            it->bias2 = data[ibad].L2;                            // GFP bias

            // prep for next point
         nok = 2;
         outlier = false;
         igood = ibad;
      }
      else 
         igood = i;
   }
   it->npts = nok;

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// compute first differences of data array(s) for WL and GF gross slip detection.
// for WL difference the WLbias (P1); for GF, the GFP (L2) and the GFR (P2)
// Store results in A1, and for GF put the range difference in A2
int GDCPass::firstDifferences(string which) throw(Exception)
{
try {
   if(A1.size() != data.size()) return FatalProblem;

   int i,iprev = -1;

   for(i=0; i<data.size(); i++) {
      // ignore bad data
      if(!(data[i].flag & OK)) {
         A1[i] = A2[i] = 0.0;
         continue;
      }

      // compute first differences - 'change the arrays' A1 and A2
      if(which == string("WL")) {
         if(iprev == -1)
            A1[i] = 0.0;
         else
            A1[i] = (data[i].P1 - data[iprev].P1) / (data[i].ndt-data[iprev].ndt);
      }
      else if(which == string("GF")) {
         if(iprev == -1)            // first difference not defined at first point
            A1[i] = A2[i] = 0.0;
         else {
            // compute first difference of L1 = raw residual GFP-GFR
            A1[i] = (data[i].L1 - data[iprev].L1) / (data[i].ndt-data[iprev].ndt);
            // compute first difference of GFP = L2
            A2[i] = (data[i].L2 - data[iprev].L2) / (data[i].ndt-data[iprev].ndt);
         }
      }

      // go to next point
      iprev = i;
   }

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// for one segment, compute conventional statistics on the
// WL bias and count the number of good points
void GDCPass::WLcomputeStats(list<Segment>::iterator& it) throw(Exception)
{
try {
   // compute WLStats
   it->WLStats.Reset();
   it->npts = 0;

   // loop over data, adding to Stats, and counting good points
   for(int i=it->nbeg; i<=it->nend; i++) {
      if(!(data[i].flag & OK)) continue;
      it->WLStats.Add(data[i].P1 - it->bias1);
      it->npts++;
   }

   // eliminate segments with too few points
   if(it->npts < int(cfg(MinPts)))
      deleteSegment(it,"insufficient data in segment");
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// for one segment, compute conventional statistics on the
// WL bias, remove small segments, and mark bad points that lie outside N*sigma
void GDCPass::WLsigmaStrip(list<Segment>::iterator& it) throw(Exception)
{
try {
   bool outlier,haveslip;
   unsigned short slip;
   int i,slipindex;
   double wlbias,nsigma,ave;

   // use robust stats on small segments, for big ones stick with conventional
   // 'change the arrays' A1 and A2; they will be used again by WLstatSweep
   if(it->npts < cfg(WLNptsOutlierStats)) {   // robust
      int j;
      double median,mad;
      // put wlbias in A1, but without gaps - let j index good points only from nbeg
      for(j=i=it->nbeg; i<=it->nend; i++) {
         if(!(data[i].flag & OK)) continue;
         wlbias = data[i].P1 - it->bias1;
         A1[j] = wlbias;
         j++;
      }

      mad = Robust::MAD(&(A1[it->nbeg]),j-it->nbeg,median,true);
      nsigma = cfg(WLNSigmaStrip) * mad;
      // change the array : A1 is wlbias, A2 will contain the weights
      ave = Robust::MEstimate(&(A1[it->nbeg]),j-it->nbeg,median,mad,&(A2[it->nbeg]));

      haveslip = false;
      for(j=i=it->nbeg; i<=it->nend; i++) {
         if(!(data[i].flag & OK)) continue;

         wlbias = data[i].P1 - it->bias1;

         // TD ? use weights at all? they remove a lot of points
         // TD add absolute limit?
         if(fabs(wlbias-ave) > nsigma || A2[j] < cfg(WLRobustWeightLimit))
            outlier = true;
         else
            outlier = false;

         // remove points by sigma stripping
         if(outlier) {
            if(data[i].flag & DETECT || i == it->nbeg) {
               haveslip = true;
               slipindex = i;        // mark
               slip = data[i].flag; // save to put on first good point
               //log << "Warning - marking a slip point BAD in WL sigma strip "
               //   << GDCUnique << " " << sat
               //   << " " << time(i).printf(outFormat) << " " << i << endl;
            }
            data[i].flag = BAD;
            learn["points deleted: WL sigma stripping"]++;
            it->npts--;
            it->WLStats.Subtract(wlbias);
         }
         else if(haveslip) {
            data[i].flag = slip;
            haveslip = false;
         }

         if(cfg(Debug) >= 6) {
            log << "DSCWLR " << GDCUnique << " " << sat
            << " " << it->nseg
            << " " << time(i).printf(outFormat)
            << fixed << setprecision(3)
            << " " << setw(3) << data[i].flag
            << " " << setw(13) << A1[j]         // wlbias
            << " " << setw(13) << fabs(wlbias-ave)
            << " " << setw(5) << A2[j]          // 0 <= weight <= 1
            << " " << setw(3) << i
            << (outlier ? " outlier" : "");
            if(i == it->nbeg) log
               << " " << setw(13) << it->bias1
               << " " << setw(13) << it->bias2;
            log << endl;
         }

         j++;
      }

   }
   else {                                             // conventional

      //nsigma = WLsigmaStrippingNsigmaLimit * it->WLStats.StdDev();
      nsigma = cfg(WLNSigmaStrip) * it->WLStats.StdDev();

      haveslip = false;
      ave = it->WLStats.Average();
      for(i=it->nbeg; i<=it->nend; i++) {
         if(!(data[i].flag & OK)) continue;

         wlbias = data[i].P1 - it->bias1;

         // remove points by sigma stripping
         if(fabs(wlbias-ave) > nsigma) { // TD add absolute limit?
            if(data[i].flag & DETECT) {
               haveslip = true;
               slipindex = i;        // mark
               slip = data[i].flag; // save to put on first good point
               //log << "Warning - marking a slip point BAD in WL sigma strip "
               //   << GDCUnique << " " << sat
               //   << " " << time(i).printf(outFormat) << " " << i << endl;
            }
            data[i].flag = BAD;
            learn["points deleted: WL sigma stripping"]++;
            it->npts--;
            it->WLStats.Subtract(wlbias);
         }
         else if(haveslip) {
            data[i].flag = slip;
            haveslip = false;
         }

      }  // loop over points in segment
   }

   // change nbeg, but don't change the bias
   if(haveslip) {
      it->nbeg = slipindex;
      //wlbias = data[slipindex].P1;
      //it->bias1 = long(wlbias+(wlbias > 0 ? 0.5 : -0.5));
   }

   // again
   if(it->npts < int(cfg(MinPts)))
      deleteSegment(it,"WL sigma stripping");
   else {
      // update nbeg and nend // TD add limit 0 data.size()
      while(it->nbeg < it->nend && !(data[it->nbeg].flag & OK)) it->nbeg++;
      while(it->nend > it->nbeg && !(data[it->nend].flag & OK)) it->nend--;
   }

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// in the given segment, compute statistics on the WL bias using a
// 'two-paned sliding window', each pane of width 'width' good points.
// store the results in the parallel (to SatPass::data) arrays A1,A2.
int GDCPass::WLstatSweep(list<Segment>::iterator& it, int width) throw(Exception)
{
try {
   int iminus,i,iplus;
   double wlbias,test,limit;
   Stats<double> pastStats, futureStats;

   // ignore empty segments
   if(it->npts == 0) return ReturnOK;
   it->WLsweep = true;

      // Cartoon of the 'two-pane moving window'
      // windows:  'past window'      'future window'
      // stats  :  --- pastStats----  ----futureStats--
      // data   : (x x x x x x x x x)(x x x x x x x x x) x ...
      //           |               |  |                  |
      // indexes: iminus          i-1 i                 iplus

   // start with the window 'squashed' to one point - the first one
   iminus = i = iplus = it->nbeg;

   // fill up the future window to size 'width', but don't go beyond the segment
   while(futureStats.N() < width && iplus <= it->nend) {
      if(data[iplus].flag & OK) {                // add only good data
         futureStats.Add(data[iplus].P1 - it->bias1);
      }
      iplus++;
   }

   // now loop over all points in the segment
   for(i=it->nbeg; i<= it->nend; i++) {
      if(!(data[i].flag & OK))                      // add only good data
         continue;

      // compute test and limit
      test = 0;
      if(pastStats.N() > 0 && futureStats.N() > 0)
         test = fabs(futureStats.Average()-pastStats.Average());
      limit = sqrt(futureStats.Variance() + pastStats.Variance());
      // 'change the arrays' A1 and A2
      A1[i] = test;
      A2[i] = limit;

      wlbias = data[i].P1 - it->bias1;        // debiased WLbias

      // dump the stats
      if(cfg(Debug) >= 6) log << "WLS " << GDCUnique
         << " " << sat << " " << it->nseg
         << " " << time(i).printf(outFormat)
         << fixed << setprecision(3)
         << " " << setw(3) << pastStats.N()
         << " " << setw(7) << pastStats.Average()
         << " " << setw(7) << pastStats.StdDev()
         << " " << setw(3) << futureStats.N()
         << " " << setw(7) << futureStats.Average()
         << " " << setw(7) << futureStats.StdDev()
         << " " << setw(9) << A1[i]
         << " " << setw(9) << A2[i]
         << " " << setw(9) << wlbias
         << " " << setw(3) << i
         << endl;

      // update stats :
      // move point i from future to past, ...
      futureStats.Subtract(wlbias);
      pastStats.Add(wlbias);
      // ... and move iplus up by one (good) point, ...
      while(futureStats.N() < width && iplus <= it->nend) {
         if(data[iplus].flag & OK) {
            futureStats.Add(data[iplus].P1 - it->bias1);
         }
         iplus++;
      }
      // ... and move iminus up by one good point
      while(pastStats.N() > width && iminus <= it->nend) {// <= nend not really nec.
         if(data[iminus].flag & OK) {
            pastStats.Subtract(data[iminus].P1 - it->bias1);
         }
         iminus++;
      }
   
   }  // end loop over i=all points in segment

   return  ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// look for slips in the WL using the results of WLstatSweep
// if slip is close to either end (< window width), just chop off the small segment
// recompute WLstats; when a slip is found, create a new segment
int GDCPass::detectWLsmallSlips(void) throw(Exception)
{
try {
   int i,k,nok;
   double wlbias;
   list<Segment>::iterator it;

   // find first segment for which WLstatSweep was called
   it = SegList.begin();
   while(!it->WLsweep) {
      it++;
      if(it == SegList.end()) return ReturnOK;
   }
   it->WLStats.Reset();

   // loop over the data arrays - all segments
   i = it->nbeg;
   nok = 0;
   while(i < data.size())
   {
      // must skip segments for which WLstatSweep was not called
      while(i > it->nend || !it->WLsweep) {
         if(i > it->nend) {
            it->npts = nok;
            nok = 0;
         }
         it++;
         if(it == SegList.end()) return ReturnOK;
         i = it->nbeg;
         if(it->WLsweep) {
            it->WLStats.Reset();
         }
      }

      if(data[i].flag & OK) {
         nok++;                                 // nok = # good points in segment

         if(nok == 1) {                         // change the bias, as WLStats reset
            wlbias = data[i].P1;
            it->bias1 = long(wlbias+(wlbias > 0 ? 0.5 : -0.5));
         }

         //  condition 3 - near ends of segment?
         if(nok < 2 || (it->npts - nok) < 2 ) {       // TD input 1/2 width
            // failed test 3 - near ends of segment
            // consider chopping off this end of segment - large limit?
            // TD must do something here ... 
            log << "too near end " << GDCUnique
               << " " << i << " " << nok << " " << it->npts-nok
               << " " << time(i).printf(outFormat)
               << " " << A1[i] << " " << A2[i]
               << endl;



         }
         else if(foundWLsmallSlip(it,i)) { // met condition 3
            // create new segment
            // TD what if nok < MinPts? -- cf detectGFsmallSlips
            k = it->npts;
            it->npts = nok;
            //log << "create new segment at i = " << i << " " << nok << "pts\n";
            it = createSegment(it,i,"WL slip small");

            // mark it
            data[i].flag |= WLDETECT;

            // prep for next segment
            // biases remain the same in the new segment
            it->npts = k - nok;
            nok = 0;
            it->WLStats.Reset();
            wlbias = data[i].P1; // change the bias, as WLStats reset
            it->bias1 = long(wlbias+(wlbias > 0 ? 0.5 : -0.5));
         }

         it->WLStats.Add(data[i].P1 - it->bias1);

      } // end if good data

      i++;

   }  // end loop over points in the pass

   it->npts = nok;

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// determine if a slip has been found at index i, in segment it
// CONDITIONs for a slip to be detected:
// 1. test must be >= ~0.67 wlwl
// 2. limit must be much smaller than test
// 3. slip must be far (>1/2 window) from either end
// 4. test must be at a local maximum within ~ window width
// 5. limit must be at a local minimum (")
// 6. (test-limit)/limit > 1.2
// large limit (esp near end of a pass) means too much noise
bool GDCPass::foundWLsmallSlip(list<Segment>::iterator& it, int i)
   throw(Exception)
{
try {
   const int minMaxWidth=int(cfg(WLSlipEdge)); // test 4,5, = ~~1/2 WLWindowWidth
   int j,jp,jm,pass4,pass5,Pass;

   // A1 = test = fabs(futureStats.Average() - pastStats.Average());
   // A2 = limit = sqrt(futureStats.Variance() + pastStats.Variance());
   // all units WL cycles

   // CONDITION 1     CONDITION 2
   if(A1[i] <= cfg(WLSlipSize) || A1[i]-A2[i] <= cfg(WLSlipExcess)) {
      return false;
   }

   // Debug print
   if(cfg(Debug) >= 6) log << "WLslip " << GDCUnique
      << " " << sat << " " << it->nseg
      << " " << setw(3) << i
      << " " << time(i).printf(outFormat)
      //<< " " << it->npts << "pt"
      << fixed << setprecision(2)
      << " test " << setw(4) << A1[i] << (A1[i]>0.67?">":"<=") << "0.67"
      << ", " << setw(4) << A1[i]-A2[i]
      << (A1[i]-A2[i]>0.1 ? ">" : "<=") << "0.1"
      << ", lim " << setw(4) << A2[i]
      << " (" << (A1[i]-A2[i])/A2[i];
      // no endl

   // CONDITIONs 4 and 5
   // do for 3 points on each side of point - best score is pass=6
   j = pass4 = pass5 = 0;
   jp = jm = i;
   do {
      // find next good point in future
      do { jp++; } while(jp < it->nend && !(data[jp].flag & OK));
         // CONDITION 4: test(A1) is a local maximum
      if(A1[i]-A1[jp] > 0) pass4++;
         // CONDITION 5: limit(A2) is a local maximum
      if(A2[i]-A2[jp] < 0) pass5++;

      // find next good point in past
      do { jm--; } while(jm > it->nbeg && !(data[jm].flag & OK));
         // CONDITION 4: test(A1) is a local maximum
      if(A1[i]-A1[jp] > 0) pass4++;
         // CONDITION 5: limit(A2) is a local maximum
      if(A2[i]-A2[jp] < 0) pass5++;

   } while(++j < minMaxWidth);

   // perfect = 2*minMaxWidth; allow 1 miss...?
   Pass = 0;
   if(pass4 >= 2*minMaxWidth-1) { Pass++; if(cfg(Debug) >= 6) log << " tst_max"; }
   if(pass5 >= 2*minMaxWidth-1) { Pass++; if(cfg(Debug) >= 6) log << " lim_min"; }

   // CONDITION 6
   if( (A1[i]-A2[i])/A2[i] > cfg(WLSlipSeparation) ) {
      Pass++;
      if(cfg(Debug) >= 6) log << " tst_lim_separation";
   }

   if(cfg(Debug) >= 6) log << ")";

   if(Pass == 3) {
		if(cfg(Debug) >= 6) log << " possible WL slip" << endl;
		return true;
	}
   if(cfg(Debug) >= 6) log << endl;

   return false;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// estimate slips and adjust biases appropriately - ie fix slips - for both WL and GF
// merge all data into one segment
int GDCPass::fixAllSlips(string which) throw(Exception)
{
try {
   // find the largest segment and start there, always combine the largest with its
   // largest neighbor
   int i,nmax,ifirst;
   list<Segment>::iterator it, kt;

   // loop over all segments, erasing empty ones
   it = SegList.begin();
   while(it != SegList.end()) {
      if(it->npts == 0)
         it = SegList.erase(it);
      else
         it++;
   }

   if(SegList.empty())
      return NoData;

   // find the largest segment
   for(kt=SegList.end(),nmax=0,it=SegList.begin(); it != SegList.end(); it++) {
      if(it->npts > nmax) {
         nmax = it->npts;
         kt = it;
      }
   }

   // fix all the slips, starting with the largest segment
   // this will merge all segments into one
   GDCUniqueFix = 0;
   while(kt != SegList.end()) {
      fixOneSlip(kt,which);
   }

   // TD here to return should be a separate call...

   // now compute stats for the WL for the (single segment) whole pass
   kt = SegList.begin();
   if(which == string("WL")) {                                    // WL
      WLPassStats.Reset();
      for(i=kt->nbeg; i <= kt->nend; i++) {
         if(!(data[i].flag & OK)) continue;
         WLPassStats.Add(data[i].P1 - kt->bias1);
      }
      // NB Now you have a measure of range noise for the whole pass :
      // sigma(WLbias) ~ sigma(WLrange) = 0.71*sigma(range), so
      // range noise = WLPassStats.StdDev() * wlwl / 0.71;  // meters
      // 0.71 / wlwl = 0.83

      // TD mark the first slip 'fixed' - unmark it - or something
   }
   // change the biases - reset the GFP bias so that it matches the GFR
   // (NB dumpSegments does not remove a bias from L1)
   else {                                                         // GF
      ifirst = -1;
      for(i=kt->nbeg; i <= kt->nend; i++) {
         if(!(data[i].flag & OK)) continue;
         if(ifirst == -1) {
            ifirst = i;
            kt->bias2 = data[ifirst].L2 - data[ifirst].P2;
            kt->bias1 = data[ifirst].P1;
         }
         // change the data - recompute GFR-GFP so it has one consistent bias
         data[i].L1 = data[i].L2 - kt->bias2 - data[i].P2;
      }
   }

   if(cfg(Debug) >= 3) dumpSegments(which + string("F"),2,true);   // WLF GFF

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by fixAllSlips
// assume there are no empty segments in the list
void GDCPass::fixOneSlip(list<Segment>::iterator& kt, string which) throw(Exception)
{
try {
   if(kt->npts == 0) { kt++; return; }

   list<Segment>::iterator left,right,it;

   // kt points to the biggest segment
   // define left and right to be the two segments on each side of the slip to be
   // fixed; assume there are no empty segments in the list
   right = left = kt;

   // choose the next segment on the right of kt
   right++;

   // choose the next segment on the left of kt
   if(kt != SegList.begin())
      left--;
   else
      left = SegList.end();            // nothing on the left

   // no segment left of kt and no segment right of kt - nothing to do
   if(left == SegList.end() && right == SegList.end()) {
      kt++;
      return;
   }

   // Always define kt to == left, as it will be returned and right will be erased.
   if(left == SegList.end()) {         // no segment on left
      left = kt;
   }
   else if(right == SegList.end()      // no segment on right
      || left->npts >= right->npts) {  // or left is the bigger segment
      right = kt;
      kt = left;                       // fix between left and kt
   }
   else {                              // left and right exist, and right is bigger
      left = kt;                       // fix between kt and right
   }

   // fix the slip between left and right, making data in 'right' part of 'left'
   if(which == string("WL"))
      WLslipFix(left,right);
   else
      GFslipFix(left,right);

   left->npts += right->npts;
   left->nend = right->nend;

   // always delete right, otherwise on return kt(==left) will be invalid
   // (ignore return value = iterator to first element after the one erased)
   SegList.erase(right);

   return;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by fixOneSlip
void GDCPass::WLslipFix(list<Segment>::iterator& left,
                        list<Segment>::iterator& right)
throw(Exception)
{
try {
   int i;

   GDCUniqueFix++;

   // full slip
   double dwl = right->bias1 + right->WLStats.Average()
         - (left->bias1 + left->WLStats.Average());
   long nwl = long(dwl + (dwl > 0 ? 0.5 : -0.5));

      // TD ? test gap size?
   //if(cfg(DT)*(right->nbeg - left->nend) > cfg(MaxGap)) break;

      // test that total variance is small
   //if(sqrt(left->WLStats.Variance() + right->WLStats.Variance())
   //   / (left->WLStats.N() + right->WLStats.N()) < cfg(WLFixSigma)) {
   //   log << "Cannot fix WL slip (noisy) at " << right->nbeg
   //      << " " << time(right->nbeg).printf(outFormat)
   //      << endl;
   //   break;
   //}

      // TD ? test fractional part of offset fabs
   //if(fabs(dwl-nwl) > cfg(WLFixFrac)) break;

   if(cfg(Debug) >= 6) log << "Fix " << GDCUnique << " " << sat << " " << GDCUniqueFix
      << " WL " << time(right->nbeg).printf(outFormat)
      << " " << left->nseg << "-" << right->nseg
      << fixed << setprecision(2)
      << " right: " << right->bias1 << " + " << right->WLStats.Average()
      << " - left: " << left->bias1 << " + " << left->WLStats.Average()
      << " = " << dwl << " " << nwl << " "
      << endl;

   // now do the fixing - change the data in the right segment to match left's
   for(i=right->nbeg; i<=right->nend; i++) {
      //if(!(data[i].flag & OK)) continue;
      data[i].P1 -= nwl;                                 // WLbias
      data[i].L2 -= nwl * wl2;                           // GFP
      // add to WLStats
      //if(!(data[i].flag & OK)) continue;
      //left->WLStats.Add(data[i].P1 - left->bias1);
   }

   // fix the slips beyond the 'right' segment.
   // change the data in the GFP, and change the both the data and the bias in the WL.
   // this way, WLStats is still valid, but if we change the GF bias, we will lose
   // that information before the GF slips get fixed.
   list<Segment>::iterator it = right;
   for(it++; it != SegList.end(); it++) {
      // Use real, not int, nwl b/c rounding error in a pass with many slips
      // can build up and produce errors.
      it->bias1 -= dwl;
      for(i=it->nbeg; i<=it->nend; i++) {
         //if(!(data[i].flag & OK)) continue;                 // TD don't?
         data[i].P1 -= nwl;                                 // WLbias
         data[i].L2 -= nwl * wl2;                           // GFP
      }
   }

   // Add to slip list
   Slip newSlip(right->nbeg);
   newSlip.NWL = nwl;
   newSlip.msg = "WL";
   SlipList.push_back(newSlip);

   // mark it
   data[right->nbeg].flag |= WLFIX;

   return;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// fix one slip in the geometry-free phase
// called by fixOneSlip
void GDCPass::GFslipFix(list<Segment>::iterator& left,
                        list<Segment>::iterator& right) throw(Exception)
{
try {
      // use this number of data points on each side of slip
   const int Npts=int(cfg(GFFixNpts));
   int i,nb,ne,nl,nr,ilast;
   long n1,nadj;              // slip magnitude (cycles)
   double dn1,dnGFR;
   Stats<double> Lstats,Rstats;

   GDCUniqueFix++;

   // find Npts points on each side of slip
   nb = left->nend;
   i = 1;
   nl = 0;
   ilast = -1;                               // ilast is last good point before slip
   while(nb > left->nbeg && i < Npts) {
      if(data[nb].flag & OK) {
         if(ilast == -1) ilast = nb;
         i++; nl++;
         Lstats.Add(data[nb].L1);
      }
      nb--;
   }
   ne = right->nbeg;
   i = 1;
   nr = 0;
   while(ne < right->nend && i < Npts) {
      if(data[ne].flag & OK) {
         i++; nr++;
         Rstats.Add(data[ne].L1);
      }
      ne++;
   }

   // first estimate of n1, without biases
   // need to use the GFR-GFP estimate here, and limit |nadj| to be well within
   // sigmas on the stats, b/c when ionosphere is very active, GFP and GFR will both
   // vary sharply, and fitting a polynomial to GFP is a BAD thing to do....
   // ultimately, GFR-GFP is accurate but noisy.
   // rms rof should tell you how much weight to put on rof
   // larger rof -> smaller npts and larger degree
   dn1 = data[right->nbeg].L2 - right->bias2 - (data[ilast].L2 - left->bias2);
   // this screws up most fixes
   //dn1 = Rstats.Average() - right->bias2 - (Lstats.Average() - left->bias2);
   n1 = long(dn1 + (dn1 > 0 ? 0.5 : -0.5));

   // TD worry about too small pieces - nr or nl too small

   // estimate the slip using polynomial fits
   nadj = EstimateGFslipFix(left,right,nb,ne,n1);

   // adjust the adjustment if it is not consistent with Lstats vs Rstats
   // dn1+nadj                       - a. current best estimate
   // Rstats.Averge()-Lstats.Average - b. estimate from stats on GFR-GFP across slip
   // difference should be consistent with R/Lstats.StdDev
   // if not, replace nadj with b. - dn1
   dnGFR = Rstats.Average() - Lstats.Average();
   if(fabs(n1+nadj-dnGFR) > (Rstats.StdDev()+Lstats.StdDev())) {
      if(cfg(Debug) >= 6)
         log << "GFRadjust " << GDCUnique << " " << sat << " " << GDCUniqueFix
         << " GF " << time(right->nbeg).printf(outFormat)
         << fixed << setprecision(2)
         << " dbias(GFR): " << Rstats.Average() - Lstats.Average()
         << " n1+nadj: " << n1+nadj;

      nadj = long(dnGFR+(dnGFR > 0 ? 0.5 : -0.5)) - n1;

      if(cfg(Debug) >= 6)
         log << " new n1+nadj: " << n1+nadj << endl;
   }

   // output result
   if(cfg(Debug) >= 6) {
      log << "Fix " << GDCUnique << " " << sat << " " << GDCUniqueFix
      << " GF " << time(right->nbeg).printf(outFormat)
      << fixed << setprecision(2)
      << " dbias: " << right->bias2 - left->bias2
      << ", dn1: " << dn1 << ", n1: " << n1 << ", adj: " << nadj
      << " indexes " << nb << " " << ne << " " << nl << " " << nr
      << " segs " << left->nseg << " " << right->nseg
      << " GFR-GFP "
      << Lstats.N() << " " << Lstats.Average() << " " << Lstats.StdDev()
      << "    "
      << Rstats.N() << " " << Rstats.Average() << " " << Rstats.StdDev()
      << " tests " << n1+nadj-dnGFR << " " << Rstats.StdDev()+Lstats.StdDev()
      << endl;
   }
   
   // full slip, including biases
   dn1 += right->bias2 - left->bias2;
   n1 = long(dn1 + (dn1 > 0 ? 0.5 : -0.5));
   n1 += nadj;

   // now do the fixing : 'change the data' within right segment
   // and through the end of the pass, to fix the slip
   for(i=right->nbeg; i<data.size(); i++) { //=right->nend; i++) {
      //if(!(data[i].flag & OK)) continue;                 // TD? don't?
      //data[i].P1 -= nwl;                           // no change to WLbias
      data[i].L2 -= n1;                              // GFP
      data[i].L1 -= n1;                              // GFR+GFP
   }
   // 'change the bias'  although right is about to be deleted....
   //right->bias2 = left->bias2;

   // Add to slip list, but if one exists with same time tag, use it instead
   list<Slip>::iterator jt;
   for(jt=SlipList.begin(); jt != SlipList.end(); jt++)
      if(jt->index == right->nbeg) break;

   if(jt == SlipList.end()) {
      Slip newSlip(right->nbeg);
      newSlip.N1 = -n1;
      newSlip.msg = "GF only";
      SlipList.push_back(newSlip);
   }
   else {
      jt->N1 = -n1;
      jt->msg += string(" GF");
   }

   // mark it
   data[right->nbeg].flag |= GFFIX;

   return;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by GFslipFix
// estimate GF slip using polynomial fit to data around it
long GDCPass::EstimateGFslipFix(list<Segment>::iterator& left,
                               list<Segment>::iterator& right,
                               int nb, int ne, long n1)
throw(Exception)
{
try {
   bool quit;
   int i,k,in[3];
   double rof,rmsrof[3];
   PolyFit<double> PF[3];

   // start at zero and limit |nadj| to ...TD
   long nadj = 0;

   // use a little indirect indexing array to avoid having to copy PolyFit objects....
   for(k=0; k<3; k++) {
      in[k]=k;
      PF[in[k]].Reset(int(cfg(GFFixDegree)));
   }

   while(1) {
      // compute 3 polynomial fits to this data, with slips of
      // (nadj-1, nadj and nadj+1) wavelengths added to left segment
      for(k=0; k<3; k++) {
         if(PF[in[k]].N() > 0) continue;

         // add all the data
         for(i=nb; i<=ne; i++) {
            if(!(data[i].flag & OK)) continue;
            PF[in[k]].Add(
            // data    - (either          left bias - poss. slip : right bias)
            data[i].L2 - (i < right->nbeg ? left->bias2-n1-(nadj+k-1) : right->bias2),
            //  use debiased count
            data[i].ndt - data[nb].ndt
            );
         }

         // TD check that it not singular

         // compute RMS residual of fit
         rmsrof[in[k]] = 0.0;
         for(i=nb; i<=ne; i++) {
            if(!(data[i].flag & OK)) continue;
            rof =    // data minus fit
            data[i].L2 - (i < right->nbeg ? left->bias2-n1-(nadj+k-1) : right->bias2)
            - PF[in[k]].Evaluate(data[i].ndt - data[nb].ndt);
            rmsrof[in[k]] += rof*rof;
         }
         rmsrof[in[k]] = sqrt(rmsrof[in[k]]);

      }  // end loop over fits

      // the value of this is questionable, b/c with active ionosphere the real
      // GFP is NOT smooth 
      for(quit=false,k=0; k<3; k++) if(rmsrof[in[k]] > cfg(GFFixMaxRMS)) {
         log << "Warning - large RMS ROF in GF slip fix at in,k = "
            << in[k] << " " << k << " " << rmsrof[in[k]] << " abort.\n";
         quit = true;
      }
      if(quit) break;

      //if(cfg(Debug) >= 6) {
      //   log << "Fix GF slip RMSROF : adj: " << nadj;
      //   for(i=0; i<3; i++) log << " " << rmsrof[in[i]];
      //   // below log << endl;
      //}

      // three cases: (TD - exceptions?) :
      // rmsrof: 0 > 1 < 2   good
      //         0 > 1 > 2   shift 0,1,2 to 1,2,3
      //         0 < 1 < 2   shift 0,1,2 to -1,0,1
      //         0 < 1 > 2   local max! - ??
      if(rmsrof[in[0]] > rmsrof[in[1]]) {
         if(rmsrof[in[1]] < rmsrof[in[2]]) { // local min - done
            //if(cfg(Debug) >= 6) log << " done." << endl;
            break;
         }
         else {                              // shift 0,1,2 to 1,2,3
            k = in[0];
            in[0] = in[1];
            in[1] = in[2];
            in[2] = k;
            PF[in[2]].Reset();
            nadj += 1;
            //if(cfg(Debug) >= 6) log << " shift left" << endl;
         }
      }
      else {
         if(rmsrof[in[1]] < rmsrof[in[2]]) { // shift 0,1,2 to -1,0,1
            k = in[2];
            in[2] = in[1];
            in[1] = in[0];
            in[0] = k;
            PF[in[0]].Reset();
            nadj -= 1;
            //if(cfg(Debug) >= 6) log << " shift right" << endl;
         }
         else {                              // local max
            log << "Warning - local maximum in RMS residuals in EstimateGFslipFix"
               << endl;
            // TD do something
            break;
         }
      }

   }  // end while loop

   // dump the raw data with all the fits
   for(i=nb; i<=ne; i++) {
      if(!(data[i].flag & OK)) continue;
      log << "GFE " << GDCUnique << " " << sat
         << " " << GDCUniqueFix
         << " " << time(i).printf(outFormat)
         << " " << setw(2) << data[i].flag << fixed << setprecision(3);
      for(k=0; k<3; k++) log << " "
         << data[i].L2 - (i < right->nbeg ? left->bias2-n1-(nadj+k-1) : right->bias2)
         << " " << PF[in[k]].Evaluate(data[i].ndt - data[nb].ndt);
      log << " " << setw(3) << data[i].ndt << endl;
   }

   return nadj;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// fit a polynomial to the GF range, and change the units of -gfr(P2) and gfp(L2)
// to cycles of wl21 (=5.4cm)
int GDCPass::prepareGFdata(void) throw(Exception)
{
try {
   bool first;
   int i,nbeg,nend;
   unsigned ndeg;

   // decide on the degree of fit
   nbeg = SegList.begin()->nbeg;
   nend = SegList.begin()->nend;
   ndeg = 2 + int(0.5 + (nend-nbeg+1)*cfg(DT)/3000.0);
   if(ndeg > 6) ndeg = 6;
   //if(ndeg > int(cfg(GFPolyMaxDegree))) ndeg = int(cfg(GFPolyMaxDegree));
   if(ndeg < 2) ndeg = 2;

   // global fit to the gfr
   GFPassFit.Reset(ndeg);

   for(first=true,i=nbeg; i <= nend; i++) {
      if(!(data[i].flag & OK)) continue;

      // 'change the bias' in the GFP by changing units, also
      // slip fixing in the WL may have changed the values of GFP
      if(first) {
         //if(fabs(data[i].L2 - SegList.begin()->bias2)/wl21 > 10.) {
         //   SegList.begin()->bias2 = data[i].L2;
         //}
         SegList.begin()->bias2 /= wl21;
         first = false;
      }

      // 'change the arrays'
      // change units on the GFP and the GFR
      data[i].P2 /= wl21;                    // gfr (cycles of wl21)
      data[i].L2 /= wl21;                    // gfp (cycles of wl21)

      // compute polynomial fit
      GFPassFit.Add(data[i].P2,data[i].ndt);

      // 'change the data'
      // save in L1                          // gfp+gfr residual (cycles of wl21)
      // ?? data[i].L1 = data[i].L2 - data[i].P2 - SegList.begin()->bias2;
      data[i].L1 = data[i].L2 - data[i].P2;
   }

   if(GFPassFit.isSingular()) {
      log << "Polynomial fit to GF range is singular! .. abort." << endl;
      return Singular;
   }

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// detect slips in the geometry-free phase
int GDCPass::detectGFslips(void) throw(Exception)
{
try {
   int i,iret;
   double bias;
   list<Segment>::iterator it;

   // places first difference of GF in A1 - 'change the arrays' A1
   if( (iret = detectObviousSlips("GF"))) return iret;

   GFPassStats.Reset();
   for(it=SegList.begin(); it != SegList.end(); it++) {
      // save for debiasing below
      // TD what if this segment deleted?
      if(it == SegList.begin()) bias = it->bias2;

      // compute stats on dGF/dt
      for(i=it->nbeg; i <= it->nend; i++) {
         if(!(data[i].flag & OK)) continue;

         // compute first-diff stats in meters
         // skip the first point in a segment - it is an obvious GF slip
         if(i > it->nbeg) GFPassStats.Add(A1[i]*wl21);

         // if a gross GF slip was found, must remove bias in L1=GF(R-P)
         // in all subsequent segments
         if(it != SegList.begin()) data[i].L1 += bias - it->bias2;
      }

      // delete segments if sigma too high?

      // check number of good points
      if(it->npts < int(cfg(MinPts))) {
         deleteSegment(it,"insufficient data in segment");
         continue;
      }

      // fit polynomial to GFR in each segment
      // compute (1stD of) fit residual GFP-fit(GFR) -> A1 - 'change the arrays' A1
      // delete segment if polynomial is singular - probably due to too little data
      if( (iret = GFphaseResiduals(it))) {
         //return iret;
         deleteSegment(it,"polynomial fit to GF residual failed");
         continue;
      }
   }

   // 'change the arrays'
   // at this point:
   // L1 = GFP+GFR in cycles, by prepareGFdata()
   // L2 = GFP in cycles, by prepareGFdata()
   // P1 = wlbias
   // P2 = GFR in cycles, by prepareGFdata() 
   // A1 = GFP-(local fit) OR its 1stD, by GFphaseResiduals()
   //      (was 1stD of GFP+GFR (in L1), by firstDifferences())
   // A2 = 1stD of GFP (in L2), by firstDifferences()
   if( (iret = detectGFsmallSlips())) return iret;

   // delete all segments that are too small
   for(it=SegList.begin(); it != SegList.end(); it++) {
      if(it->npts < int(cfg(MinPts)))
         deleteSegment(it,"insufficient data in segment");
   }

   if(cfg(Debug) >= 4) dumpSegments("GFD",2,true);

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// for each segment, fit a polynomial to the gfr, then compute and store the
// residual of fit
int GDCPass::GFphaseResiduals(list<Segment>::iterator& it) throw(Exception)
{
try {
   int i,j,ndeg,nprev;
   double fit,rbias,prev,tmp;
   Stats<double> rofStats;

   // decide on the degree of fit
   ndeg = 2 + int(0.5 + (it->nend-it->nbeg+1)*cfg(DT)/3000.0);
   //if(ndeg > int(cfg(GFPolyMaxDegree))) ndeg = int(cfg(GFPolyMaxDegree));
   if(ndeg > 6) ndeg = 6;
   if(ndeg < 2) ndeg = 2;

   it->PF.Reset(ndeg);     // for fit to GF range

   for(i=it->nbeg; i <= it->nend; i++) {
      if(!(data[i].flag & OK)) continue;
      it->PF.Add(data[i].P2,data[i].ndt);
   }

   if(it->PF.isSingular()) {     // this should never happen
      log << "Polynomial fit to GF range is singular in segment " << it->nseg
         << "! .. abort." << endl;
      return Singular;
   }

   // now compute the residual of fit
   rbias = prev = 0.0;
   rofStats.Reset();
   for(i=it->nbeg; i <= it->nend; i++) {
      if(!(data[i].flag & OK)) continue;
      
      // TD? Use whole pass for small segments?
      //fit = GFPassFit.Evaluate(data[i].ndt);     // use fit to gfr for whole pass
      fit = it->PF.Evaluate(data[i].ndt);

      // all (fit, resid, gfr and gfp) are in cycles of wl21 (5.4cm)

      // compute gfp-(fit to gfr), store in A1 - 'change the arrays' A1 and A2
      // OR let's try first difference of residual of fit
      A1[i] = data[i].L2 - it->bias2 - fit; // residual: phase - fit to range
      if(rbias == 0.0) { rbias = A1[i]; nprev = data[i].ndt - 1; }
      A1[i] -= rbias;                       // debias residual for plots

         // compute stats on residual of fit
      rofStats.Add(A1[i]);

      if(1) { // 1stD of residual - remember A1 has just been debiased
         tmp = A1[i];
         A1[i] -= prev;       // diff with previous epoch's
         A1[i] /= (data[i].ndt - nprev);
         prev = tmp;          // store residual for next point
         nprev = data[i].ndt;
      }
      
      // store fit in A2
      //A2[i] = fit;                         // fit to gfr (cycles of wl21)
      // store raw residual GFP-GFR (cycles of wl21) in A2
      //A2[i] = data[i].L2 - it->bias2 - data[i].P2;
   }

   // TD? need this? use this?
   //log << "GFDsum " << GDCUnique << " " << sat << " " << it->nseg << " " << ndeg
   //   << " " << it->nbeg << " " << it->npts << " " << it->nend
   //   << " " << rofStats.N() << fixed << setprecision(3)
   //   << " " << rofStats.Minimum()
   //   << " " << rofStats.Maximum()
   //   << " " << rofStats.Average()
   //   << " " << rofStats.StdDev() << endl;

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// detect small slips in the geometry-free phase
// TD outliers at the beginning or end of the segment....
int GDCPass::detectGFsmallSlips(void) throw(Exception)
{
try {
   const int width=int(cfg(GFSlipWidth));
   int i,j,iplus,inew,ifirst,nok;
   list<Segment>::iterator it;
   Stats<double> pastStats,futureStats;

   // loop over segments
   for(it=SegList.begin(); it != SegList.end(); it++) {

      if(it->npts < 2*width+1) continue;

      // Cartoon of the GF 'two-pane moving window'
      //          point of interest:|
      // windows:     'past window' | 'future window'
      // stats  :        pastStats  | futureStats  (5 pts in each window)
      // data   : ... x (x x x x x) x (x x x x x) x ...
      //                 |          |          |
      // indexes:        j          i        iplus

      deque<int> pastIndex, futureIndex;
      pastStats.Reset();
      futureStats.Reset();
      i = inew = ifirst = -1;
      nok = 0;                          // recount

      // loop over points in the segment
      for(iplus=it->nbeg; iplus<=it->nend+width; iplus++) {

         // ignore bad points
         if(iplus <= it->nend && !(data[iplus].flag & OK)) continue;
         if(ifirst == -1) ifirst = iplus;

         // pop the new i from the future
         if(futureIndex.size() == width || iplus > it->nend) {
            inew = futureIndex.front();
            futureIndex.pop_front();
            futureStats.Subtract(A1[inew]);
            nok++;
         }

         // put iplus into the future deque
         if(iplus <= it->nend) {
            futureIndex.push_back(iplus);
            futureStats.Add(A1[iplus]);
         }
         else
            futureIndex.push_back(-1);

         // check for outliers
         // we now have:
         //                (  past   )     ( future  )
         // data   : ... x (x x x x x) x x (x x x x x) x ...
         //                            | |          |
         // indexes:                   i inew     iplus
         // outlier if: (i,inew) = opposite signs but ~= large magnitude
         // if found, mark i bad and replace A1(inew) = A1(inew)+A1(i)
         if(foundGFoutlier(i,inew,pastStats,futureStats)) {
            // check that i was not marked a slip in the last iteration
            // if so, let inew be the slip and i the outlier
            if(data[i].flag & DETECT) {
               //log << "Warning - marking a slip point BAD in GF detect small "
               //   << GDCUnique << " " << sat
               //   << " " << time(i).printf(outFormat) << " " << i << endl;
               data[inew].flag = data[i].flag;
               it->nbeg = inew;
            }
            data[i].flag = BAD;
            A1[inew] += A1[i];
            learn["points deleted: GF outlier"]++;
            i = inew;
            nok--;
         }

         // pop last from past
         if(pastIndex.size() == width) {
            j = pastIndex.front();
            pastIndex.pop_front();
            pastStats.Subtract(A1[j]);
         }

         // move i into the past
         if(i > -1) {
            pastIndex.push_back(i);
            pastStats.Add(A1[i]);
         }

         // return to original state
         i = inew;

         // test for slip .. foundGF...prints to log
         if(foundGFsmallSlip(i,it->nseg,it->nend,it->nbeg,
            pastIndex,futureIndex,pastStats,futureStats)) {

            // create a new segment
            it->npts = nok-1;
            it = createSegment(it,i,"GF slip small");
            nok = 1;

            // mark it
            data[i].flag |= GFDETECT;

            // TD print the "possible GF slip" and timetag here - see WLS
         }

      }  // end loop over points in the pass
      it->npts = nok;

   }  // end loop over segments

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
bool GDCPass::foundGFoutlier(int i, int inew, 
   Stats<double>& pastSt, Stats<double>& futureSt)
   throw(Exception)
{
try {
   if(i < 0 || inew < 0) return false;
   double pmag = A1[i]; // -pastSt.Average();
   double fmag = A1[inew]; // -futureSt.Average();
   double var = sqrt(pastSt.Variance() + futureSt.Variance());

   if(cfg(Debug) >= 7) log << "GFoutlier " << GDCUnique
      << " " << sat << " " << setw(3) << inew
      << " " << time(inew).printf(outFormat)
      << fixed << setprecision(3)
      << " mags: " << pmag << " ~=? " << -fmag
      //<< "; dmag/mag: " << 2*fabs((pmag+fmag)/(pmag-fmag)) << " <? " << 0.3
      << "; mag/noise: " << fabs(pmag)/var <<" & "<< fabs(fmag)/var << " >? " << 5;

   if(pmag * fmag >= 0)                               // opposite signs
      { if(cfg(Debug) >= 7) log << endl; return false; }

   //if(fabs(pmag+fmag) > 0.15*fabs(pmag-fmag))         // approx equal magnitude
      //{ if(cfg(Debug) >= 7) log << endl; return false; }

   if(fabs(pmag) < cfg(GFSlipOutlier)*var ||
      fabs(fmag) < cfg(GFSlipOutlier)*var)   // and large
   {
      if(cfg(Debug) >= 7) log << endl;
      return false;
   }

   if(cfg(Debug) >= 7) log << " possible GF outlier" << endl;

   return true;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Better to find too many small ones than to miss them, since the fixing algorithm
// will most likely refuse to act on the questionable ones.
bool GDCPass::foundGFsmallSlip(int i,int nseg,int iend,int ibeg,
   deque<int>& pastIn,deque<int>& futureIn,
   Stats<double>& pastSt, Stats<double>& futureSt)
   throw(Exception)
{
try {
   if(i < 0) return false;

   int j,k;
   double mag,pmag,fmag,pvar,fvar;

   pmag = fmag = pvar = fvar = 0.0;
   // note when past.N == 1, this is first good point, which has 1stD==0
   // TD be very careful when N is small
   if(pastSt.N() > 0) pmag = A1[i]-pastSt.Average();
   if(futureSt.N() > 0) fmag = A1[i]-futureSt.Average();
   if(pastSt.N() > 1) pvar = pastSt.Variance();
   if(futureSt.N() > 1) fvar = futureSt.Variance();
   mag = (pmag + fmag) / 2.0;

   if(cfg(Debug) >= 6) log << "GFS " << GDCUnique
      << " " << sat << " " << nseg
      << " " << time(i).printf(outFormat)
      //<< " P( " << setw(3) << pastIn[0]            // don't print this...
      //<< " " << setw(3) << pastIn[1]
      //<< " " << setw(3) << pastIn[2]
      //<< " " << setw(3) << pastIn[3]
      //<< " " << setw(3) << pastIn[4]
      //<< ") " << setw(3) << i
      //<< " F( " << setw(3) << futureIn[0]
      //<< " " << setw(3) << futureIn[1]
      //<< " " << setw(3) << futureIn[2]
      //<< " " << setw(3) << futureIn[3]
      //<< " " << setw(3) << futureIn[4] << ")"       // ...to here
      << fixed << setprecision(3)
      << " " << setw(3) << pastSt.N()
      << " " << setw(7) << pastSt.Average()
      << " " << setw(7) << pastSt.StdDev()
      << " " << setw(3) << futureSt.N()
      << " " << setw(7) << futureSt.Average()
      << " " << setw(7) << futureSt.StdDev()
      << " " << setw(7) << mag
      << " " << setw(7) << sqrt(pvar+fvar)
      << " " << setw(9) << A1[i]
      << " " << setw(7) << pmag
      << " " << setw(7) << pvar
      << " " << setw(7) << fmag
      << " " << setw(7) << fvar
      << " " << setw(3) << i
      << endl;

   //                    x                    -- mag
   //
   //    x   x   x   x                         - step
   //                       x    x   x   x   ---
   const double minMag=cfg(GFSlipSize);     // minimum slip magnitude
   const double STN=cfg(GFSlipStepToNoise); // step (past->future) to noise ratio
   const double MTS=cfg(GFSlipToStep);      // magnitude to step ratio
   const double MTN=cfg(GFSlipToNoise);     // magnitude to noise ratio
   const int Edge=int(cfg(GFSlipEdge));     // number of points before edge
   const double RangeCheckLimit = 2*cfg(WLSigma)/(0.83*wl21);
                                                // 2 * range noise in units of wl21
   // if WL slip here - ...? 
   
   // 1. slip must be non-trivial
   if(fabs(mag) <= minMag) return false; 

   // 2. change in average is small compared to noise
   if(fabs(pmag-fmag) >= STN*sqrt(pvar+fvar)) return false;

   if(cfg(Debug) >= 7) log << "GFslip " << GDCUnique
      << " " << sat << " " << nseg
      << " " << setw(3) << i
      << " " << time(i).printf(outFormat)
      << " mag: " << mag << " > " << minMag
      << "; step/noise: " << fabs(pmag-fmag)/sqrt(pvar+fvar) << " < " << STN;

   // 3. slip is large compared to change in average
   if(fabs(mag) <= MTS*fabs(pmag-fmag)) {
      if(cfg(Debug) >= 7) log << endl;
      return false;
   }

   if(cfg(Debug) >= 7)
      log << "; mag/step: " << fabs(mag/(pmag-fmag)) << " > " << MTS;

   // 4. magnitude is large compared to noise: a 3-sigma slip
   if(fabs(mag) <= MTN*sqrt(pvar+fvar)) {
      if(cfg(Debug) >= 7) log << endl;
      return false;
   }

   if(cfg(Debug) >= 7)
      log << "; mag/noise: " << fabs(mag)/sqrt(pvar+fvar) << " > " << MTN;

   // if very close to edge, declare it an outlier
   if(pastSt.N() < Edge || futureSt.N() < Edge+1) {
      if(cfg(Debug) >= 7) log << "; too close to edge"; // << endl;
      //return false;      // TD
   }

   // TD? if slip is within a few epochs of WL slip - skip it


   // large slips (compared to range noise): check the GFR-GFP for consistency
   if(fabs(mag) > RangeCheckLimit) {
      double magGFR,mtnGFR;
      Stats<double> pGFRmPh,fGFRmPh;
      for(j=0; j<pastIn.size(); j++) {
         if(pastIn[j] > -1) pGFRmPh.Add(data[pastIn[j]].L1);
         if(futureIn[j] > -1) fGFRmPh.Add(data[futureIn[j]].L1);
      }
      magGFR = data[i].L1 - (pGFRmPh.Average()+fGFRmPh.Average())/2.0;
      mtnGFR = fabs(magGFR)/sqrt(pGFRmPh.Variance()+fGFRmPh.Variance());
         
      if(cfg(Debug) >= 7)
         log << "; GFR-GFP has mag: " << magGFR
             << ", |dmag|: " << fabs(mag-magGFR)
             << " and mag/noise " << mtnGFR;

      // TD test - mag must ~= magGFR if magGFR/noiseGFR >> 1
      // test - metz 54,56,57,58
      if(fabs(mag-magGFR) > fabs(magGFR)) {
         if(cfg(Debug) >= 7) log << " - does not match GFR "
            << fabs(mag-magGFR) << endl;
         return false;
      }
      if(mtnGFR < 3) {
         if(cfg(Debug) >= 7) log << " - cannot see in GFR " << mtnGFR << endl;
         return false;
      }
   }

   // small slips (compared to variations in dGF): extra careful
   // TD beware of small slips in the presence of noise >~ 1
   else { //if(fabs(mag) <= RangeCheckLimit)
      double magFD;
      Stats<double> fdStats;
      j = i-1; k=0;
      while(j >= ibeg && k < 15) {
         if(data[j].flag & OK) { fdStats.Add(A2[j]); k++; }
         j--;
      }
      j = i+1; k=0;
      while(j <= iend && k < 15) {
         if(data[j].flag & OK) { fdStats.Add(A2[j]); k++; }
         j++;
      }
      magFD = A2[i] - fdStats.Average();

      if(cfg(Debug) >= 7)
         log << "; 1stD(GFP) has mag: " << magFD
            << ", noise " << fdStats.StdDev()
            << " and mag/noise " << fabs(magFD)/fdStats.StdDev()
            << "; min/max are " << fdStats.Minimum() << "/" << fdStats.Maximum();
   }
   
   if(cfg(Debug) >= 7) log << " possible GF slip" << endl;

   return true;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// check the consistency of WL slips where a GF slip, but not a WL slip, was detected.
int GDCPass::WLconsistencyCheck(void) throw(Exception)
{
try {
   int i,k;
   const int N=2*int(cfg(WLWindowWidth));
   double mag,absmag,factor=wl2/wl21;

   // loop over the data and look for points with GFDETECT but not WLDETECT or WLFIX
   for(i=0; i<data.size(); i++) {

      if(!(data[i].flag & OK)) continue;        // bad
      if(!(data[i].flag & DETECT)) continue;    // no slips
      if(data[i].flag & WLDETECT) continue;     // WL was detected

      // GF only slip - compute WL stats on both sides
      Stats<double> futureStats,pastStats;
      k = i;
      // fill future
      while(k < data.size() && futureStats.N() < N) {
         if(data[k].flag & OK)                  // data is good
            futureStats.Add(data[k].P1);        // wlbias
         k++;
      }
      // fill past
      k = i-1;
      while(k >= 0 && pastStats.N() < N) {
         if(data[k].flag & OK)                  // data is good
            pastStats.Add(data[k].P1);          // wlbias
         k--;
      }

      // is there a WL slip here?
      // 1. magnitude of slip > 0.75
      // 2. magnitude is > stddev on both sides
      // 3. N() > 10 on both sides TD??
      mag = futureStats.Average()-pastStats.Average();
      absmag = fabs(mag);

      if(absmag > cfg(WLSlipSize) &&      // 0.75 &&
         absmag > pastStats.StdDev() &&
         absmag > futureStats.StdDev()) {

         long nwl;
         nwl = long(mag + (mag > 0 ? 0.5 : -0.5));

         if(nwl == 0) continue;

         // now do the fixing - change the data to the future of the slip
         for(k=i; k<data.size(); k++) {
            //if(!(data[i].flag & OK)) continue;
            data[k].P1 -= nwl;                                 // WLbias
            data[k].L2 -= nwl * factor;                        // GFP
         }
         
         // Add to slip list
         Slip newSlip(i);
         newSlip.NWL = nwl;
         newSlip.msg = "WL";
         SlipList.push_back(newSlip);

         // mark it
         data[i].flag |= (WLDETECT + WLFIX);

         if(cfg(Debug) >= 6) log << "CHECK " << GDCUnique << " " << sat
            << " " << i
            << " " << time(i).printf(outFormat)
            << fixed << setprecision(3)
            << "  " << pastStats.N()
            //<< " " << pastStats.Average()
            << " " << pastStats.StdDev()
            << "  " << futureStats.N()
            //<< " " << futureStats.Average()
            << " " << futureStats.StdDev()
            << "  " << futureStats.Average() - pastStats.Average()
            << " " << nwl
            << endl;

      }
   }

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// last call before returning:
// generate editing commands for deleted (flagged) data,
// use editing command (slips and deletes) to modify the original SatPass data
// and print ending summary
void GDCPass::finish(int iret, SatPass& svp, vector<string>& editCmds)
   throw(Exception)
{
try {
   bool ok;
   int i,ifirst,ilast,npts;
   long N1,N2,prevN1,prevN2;
   double slipL1,slipL2,biasL1,biasL2;
   SatPassData spd;
   list<Slip>::iterator jt;

   // ---------------------------------------------------------
   // sort the slips in time
   SlipList.sort();

   // ---------------------------------------------------------
   // merge *this GDCPass and the input SatPass...
   // use this->flag to generate edit commands for data marked bad
   // use the SlipList to fix slips
   // 'change the arrays' A1 and A2 - fill with range minus phase for output
   npts = 0;
   ilast = -1;                         // ilast is the index of the last good point
   ifirst = -1;                        // ifirst is the index of the first good point
   biasL1 = biasL2 = slipL1 = slipL2 = 0.0;
   prevN1 = prevN2 = 0L;
   jt = SlipList.begin();
   for(i=0; i<data.size(); i++) {

      // is this point bad?
      if(!(data[i].flag & OK)) {       // data is bad
         ok = false;
         if(i == data.size() - 1) {    // but this is the last point 
            i++;
            ok = true;
         }
      }
      else ok = true;                  // data is good

      if(ok) {
         if(ifirst == -1) ifirst = i;

         // generate edit commands: delete from ilast+1 to i-1
         if(i-ilast > 2 && cfg(OutputDeletes) != 0) {
            // delete 2, or a range of, points
            // -DS+<sat>,<time>
            ostringstream stst1;
            stst1 << "-DS";
            if(i-ilast > 3) stst1 << "+";
            stst1 << sat << ",";
            if(cfg(OutputGPSTime))
               stst1 << time(ilast+1).printf("%F,%.3g");
            else
               stst1 << time(ilast+1).printf("%Y,%m,%d,%H,%M,%f");
            if(i-ilast > 3) stst1 << " # begin delete of "
                  << asString(i+1-ilast) << " points";
            editCmds.push_back(stst1.str());

            // -DS-<sat>,<time>
            ostringstream stst2;
            stst2 << "-DS";
            if(i-ilast > 3) stst2 << "-";
            stst2 << sat << ",";
            if(cfg(OutputGPSTime))
               stst2 << time(i-1).printf("%F,%.3g");
            else
               stst2 << time(i-1).printf("%Y,%m,%d,%H,%M,%f");
            if(i-ilast > 3) stst2 << " # end delete of "
               << asString(i+1-ilast) << " points";
            editCmds.push_back(stst2.str());
         }
         else if(i-ilast > 1 && cfg(OutputDeletes) != 0) {
            // delete a single isolated point
            ostringstream stst;
            stst << "-DS" << sat << ",";
            if(cfg(OutputGPSTime))
               stst << time(i-1).printf("%F,%.3g");
            else
               stst << time(i-1).printf("%Y,%m,%d,%H,%M,%f");
            editCmds.push_back(stst.str());
         }

         ilast = i;
         npts++;
      }

      // keep track of net slip fix
      if(jt != SlipList.end() && i == jt->index) {          // there is a slip here
         // fix the slip by changing the bias added to phase
         N1 = jt->N1;
         N2 = jt->N1 - jt->NWL;
         slipL1 += double(N1);
         slipL2 += double(N2);

         // generate edit commands
         {
            ostringstream stst;
            stst << "-BD+" << sat << ",L1,";
            if(cfg(OutputGPSTime))
               stst << time(jt->index).printf("%F,%.3g");
            else
               stst << time(jt->index).printf("%Y,%m,%d,%H,%M,%f");
            stst << "," << N1-prevN1;
            if(!jt->msg.empty()) stst << " # " << jt->msg;
            //stst << " # WL: " << jt->NWL << " N1: " << jt->N1; //temp
            editCmds.push_back(stst.str());
         }
         {
            ostringstream stst;
            stst << "-BD+" << sat << ",L2,";
            if(cfg(OutputGPSTime))
               stst << time(jt->index).printf("%F,%.3g");
            else
               stst << time(jt->index).printf("%Y,%m,%d,%H,%M,%f");
            stst << "," << N2-prevN2;
            if(!jt->msg.empty()) stst << " # " << jt->msg;
            editCmds.push_back(stst.str());
         }

         prevN1 = N1;
         prevN2 = N2;
         jt++;
      }

      if(i >= data.size()) break;

      // get the untouched data from the input SatPass
      spd = svp.getData(i);

      // 'change the data' for the last time
      data[i].L1 = spd.L1 - slipL1;
      data[i].L2 = spd.L2 - slipL2;
      data[i].P1 = spd.P1;
      data[i].P2 = spd.P2;

      // compute range minus phase for output
      // do the same at the beginning ("BEG")

      // compute WL and GFP
      double wlr = wl1r * data[i].P1 + wl2r * data[i].P2;  // narrow lane range (m)
      double wlp = wl1p * data[i].L1 + wl2p * data[i].L2;  // wide lane phase (m)
      double gfr = gf1r * data[i].P1 + gf2r * data[i].P2;  // geo-free range (m)
      double gfp = gf1p * data[i].L1 + gf2p * data[i].L2;  // geo-free phase (m)
      if(i == ifirst) {
         biasL1 = (wlp-wlr)/wlwl;
         //biasL2 = gfp;
      }
      A1[i] = (wlp-wlr)/wlwl - biasL1;    // wide lane bias (cyc)
      //A2[i] = gfp - biasL2;               // geo-free phase (m)
      A2[i] = gfr - gfp;                  // geo-free range - phase (m)

   }

   // first fix the segment for dump - TD? is this necessary?
   if(SegList.begin() != SegList.end()) {
      SegList.begin()->bias1 = SegList.begin()->bias2 = 0;     // not necessary..
      SegList.begin()->nbeg = 0;
      SegList.begin()->nend = data.size()-1;
      SegList.begin()->npts = npts;
   }
   // dump the corrected data
   if(cfg(Debug) >= 2) dumpSegments("AFT",2,true);

   // dump the edit commands to log
   for(i=0; i<editCmds.size(); i++)
      log << "EditCmd: " << GDCUnique << " " << editCmds[i] << endl;

	//if(fixOutput) {
		for(i=0; i<data.size(); i++) {
		// change the flag for use by SatPass
      //const unsigned short SatPass::OK  = 1; good data
      //const unsigned short SatPass::BAD = 0; used by caller and DC to mark bad data
		//const unsigned short SatPass::LL1 = 2; discontinuity on L1 only
		//const unsigned short SatPass::LL2 = 4; discontinuity on L2 only
		//const unsigned short SatPass::LL3 = 6; discontinuity on L1 and L2
			if(data[i].flag & OK) {
				if(((data[i].flag & DETECT)==0 && (data[i].flag & FIX)!=0) || i==ifirst)
					data[i].flag = LL3 + OK;
				else
					data[i].flag = OK;
			}
			else
				data[i].flag = BAD;
      }
   	// svp is the original SatPass
   	svp = (SatPass)(*this);
	//}

   // ---------------------------------------------------------
   // print stuff at the end
   if(cfg(Debug) >= 1) dumpSegments("GDC",1);

   // print WL stats for whole pass
   if(WLPassStats.N() > 2) {
      log << "GDC " << GDCUnique << " " << sat
         << " " << fixed << setprecision(3) << WLPassStats.StdDev()
         << " WL sigma in cycles"
         << " N=" << WLPassStats.N()
         << " Min=" << WLPassStats.Minimum()
         << " Max=" << WLPassStats.Maximum()
         << " Ave=" << WLPassStats.Average();
      if(WLPassStats.StdDev() > cfg(WLSigma))
         log << " Warning - WL sigma > input (" << cfg(WLSigma) << ")";
      log << endl;
   }

   if(GFPassStats.N() > 2) {
      log << "GDC " << GDCUnique << " " << sat
         << " " << fixed << setprecision(3) << GFPassStats.StdDev()
         << " sigma GF variation in meters/DT"
         << " N=" << GFPassStats.N()
         << " Min=" << GFPassStats.Minimum()
         << " Max=" << GFPassStats.Maximum()
         << " Ave=" << GFPassStats.Average()
         << endl;
      log << "GDC " << GDCUnique << " " << sat
         << " " << fixed << setprecision(3)
         << (fabs(GFPassStats.Minimum()) > fabs(GFPassStats.Maximum()) ?
            fabs(GFPassStats.Minimum()) : fabs(GFPassStats.Maximum()))
         << " maximum GF variation in meters/DT"
         << " N=" << GFPassStats.N()
         << " Ave=" << GFPassStats.Average()
         << " Std=" << GFPassStats.StdDev()
         << endl;
   }

   // print 'learn' summary
   map<string,int>::const_iterator kt;
   for(kt=learn.begin(); kt != learn.end(); kt++)
      log << "GDC " << GDCUnique << " " << sat
         << " " << setw(3) << kt->second << " " << kt->first << endl;

   int n = int((lastTime-firstTime)/cfg(DT)) + 1;
   double percent = 100*ngood/n;
   log << "GDC# " << setw(2) << GDCUnique << ", SAT " << sat
      << ", Pts: " << setw(4) << n << " total " << setw(4) << ngood
      << " good " << setprecision(1) << setw(5) << percent << "%"
      << ", start " << firstTime.printf(outFormat)
      << endl;

   if(iret) {
      log << "GDC is returning with error code: "
         << (iret == NoData ? "insufficient data" :
            (iret == Singular ? "singularity" :
            (iret == FatalProblem ? "fatal problem" : "unknown problem")
            //(iret == PrematureEnd ? "premature end" : "unknown problem")
            ))
         << endl;
   }

   log << "======== End GPSTK Discontinuity Corrector " << GDCUnique
      << " ================================================\n";
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// create, delete and dump Segments
//------------------------------------------------------------------------------------
// create a new segment from the given one, starting at index ibeg,
// and insert it after the given iterator.
// Return an iterator pointing to the new segment. String msg is for debug output
list<Segment>::iterator GDCPass::createSegment(list<Segment>::iterator sit,
                                               int ibeg, string msg) throw(Exception)
{
try {
   Segment s;
   s = *sit;
   s.nbeg = ibeg;
   s.nend = sit->nend;
   sit->nend = ibeg-1;

   // 'trim' beg and end indexes
   while(s.nend > s.nbeg && !(data[s.nend].flag & OK)) s.nend--;
   while(sit->nend > sit->nbeg && !(data[sit->nend].flag & OK)) sit->nend--;

   // get the segment number right
   s.nseg++;
   list<Segment>::iterator skt=sit;
   for(skt++; skt != SegList.end(); skt++) skt->nseg++;

   if(cfg(Debug) >= 6)
      log << "SEG " << GDCUnique << " " << sat
         << " " << msg
         << " " << time(ibeg).printf(outFormat)
         << " " << s.nbeg << " - " << s.nend
         << " biases " << fixed << setprecision(3) << s.bias1 << " " << s.bias2
         << endl;
 
   learn["breaks found: " + msg]++;

   return SegList.insert(++sit,s); // insert puts s before ++sit
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// dump a list of the segments
// level=0 one line summary (number of segments)
// level=1 one per line list of segments
// level=2 dump all data, including (if extra) temporary arrays
void GDCPass::dumpSegments(string label, int level, bool extra) throw(Exception)
{
try {
   int i,ifirst,ilast;
   list<Segment>::iterator it;

      // summary of SegList
   log << label << " " << GDCUnique
      << " list of Segments (" << SegList.size() << "):"
      << endl;

   if(level < 1) return;

      // one line per segment
   ilast = -1;                               // last good point
   for(it=SegList.begin(); it != SegList.end(); it++) {
      //if(it->npts > 0) {
      //   biaswl = data[it->nbeg].P1;
      //   biasgf = data[it->nbeg].L2;
      //}
      //else biaswl = biasgf = 0.0;

      i = (it->nend - it->nbeg + 1);         // total number of points

      log << label << " " << GDCUnique << " " << sat
         << " #" << setw(2) << it->nseg << ": "
         << setw(4) << it->npts << "/" << setw(4) << i << " pts, # "
         << setw(4) << it->nbeg << "-" << setw(4) << it->nend
         << " (" << time(it->nbeg).printf(outFormat)
         << " - " << time(it->nend).printf(outFormat)
         << ")";

      if(it->npts > 0) {
         log << fixed << setprecision(3)
            << " bias(wl)=" << setw(13) << it->bias1 //biaswl
            << " bias(gf)=" << setw(13) << it->bias2; //biasgf;
         if(ilast > -1) {
            ifirst = it->nbeg;
            while(ifirst <= it->nend && !(data[ifirst].flag & OK)) ifirst++;
            i = data[ifirst].ndt - data[ilast].ndt;
            log << " Gap " << setprecision(1) << setw(5)
               << cfg(DT)*i << " s = " << i << " pts.";
         }
         ilast = it->nend;
         while(ilast >= it->nbeg && !(data[ilast].flag & OK)) ilast--;
      }

      log << endl;
   }

   if(level < 2) return;

      // dump the data
   for(it=SegList.begin(); it != SegList.end(); it++) {
      for(i=it->nbeg; i<=it->nend; i++) {
         //if(!(data[i].flag & OK)) continue;  //dfplot ignores bad data

         log << "DSC" << label << " " << GDCUnique << " " << sat << " " << it->nseg
            << " " << time(i).printf(outFormat)
            << " " << setw(3) << data[i].flag
            << fixed << setprecision(3)
            << " " << setw(13) << data[i].L1
            << " " << setw(13) << data[i].L2 - it->bias2 //biasgf
            << " " << setw(13) << data[i].P1 - it->bias1 //biaswl
            << " " << setw(13) << data[i].P2;
         if(extra) log
            << " " << setw(13) << A1[i]
            << " " << setw(13) << A2[i];
         log << " " << setw(4) << i;          // TD? make this data[i].ndt?
         if(i == it->nbeg) log
            << " " << setw(13) << it->bias1 //biaswl
            << " " << setw(13) << it->bias2; //biasgf;
         log << endl;
      }
   }
   
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void GDCPass::deleteSegment(list<Segment>::iterator& it, string msg) throw(Exception)
{
try {
   int i;

   if(cfg(Debug) >= 6) log << "Delete segment "
      << GDCUnique << " " << sat << " " << it->nseg
      << " pts " << it->npts
      << " indexes " << it->nbeg << " - " << it->nend
      << " start " << firstTime.printf(outFormat)
      << " : " << msg
      << endl;

   it->npts = 0;
   for(i=it->nbeg; i<=it->nend; i++) if(data[i].flag & OK) {
      // count these : learn
      learn["points deleted: " + msg]++;
      data[i].flag = BAD;
   }

   learn["segments deleted: " + msg]++;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
