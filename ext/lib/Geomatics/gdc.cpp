//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file gdc.cpp GPSTk Discontinuity Corrector
/// GPS phase discontinuity correction. Given a SatPass object containing
/// dual-frequency pseudorange and phase for an entire satellite pass,
/// detect discontinuities in the phase and, if possible, estimate their size and fix.
/// Output is a list of Rinex editing commands (see EditRinex or class RinexEditor).

#include "gdc.hpp"
#include "GNSSconstants.hpp"
#include "stl_helpers.hpp"
#include "logstream.hpp"

using namespace std;

namespace gpstk {

//------------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------------
// class gdc: string giving version of gpstk Discontinuity Corrector
const string gdc::GDCVersion = string("9.0 5/20/17");

// ----------------------- flags and bitmaps
// values for flags[]; NB flags[] is either good (0) or bad (non-zero)
// not to be confused with Arc::marks or SatPass flags  TD bitmap?
const unsigned gdc::OK         = 0;  // good;           NB SatPass::OK == 1
const unsigned gdc::BAD        = 1;  // bad in SatPass; NB SatPass::BAD == 0
const unsigned gdc::WLOUTLIER  = 2;  // called outlier by WL filter
const unsigned gdc::GFOUTLIER  = 3;  // called outlier by GF filter
const unsigned gdc::WLSHORT    = 4;  // data with Arc.ngood < MinPts
const unsigned gdc::GFSHORT    = 5;  // data with Arc.ngood < MinPts
const unsigned gdc::ISOLATED   = 6;  // final check - isolated good points (<MinPts)

// Bitmap values used by Arc::mark - see create_mark_string_map() routine.
const unsigned Arc::BEG    =  1;
const unsigned Arc::WLSLIP =  2;
const unsigned Arc::GFSLIP =  4;
const unsigned Arc::WLFIX  =  8;
const unsigned Arc::GFFIX  = 16;
const unsigned Arc::WLMARK = 32;
const unsigned Arc::GFMARK = 64;
const unsigned Arc::REJ   = 128;
// NB add additional values to the Arc::create_mark_string_map() routine.

// ------------------------ mere conveniences
const unsigned gdc::WL = 0;      // keep these 0,1 for use as array indexes
const unsigned gdc::GF = 1;
const vector<unsigned> gdc::SLIP = gdc::create_vector_SLIP();
const vector<unsigned> gdc::FIX = gdc::create_vector_FIX();
const vector<string> gdc::LAB = gdc::create_vector_LAB();

// static const map giving string descriptors for each Arc::mark value
const map<unsigned, string> Arc::markStr = Arc::create_mark_string_map();

// used to access CFG - also see setcfg(a,b,c) below
#define cfg(a) cfg_func(#a)      // #a 'string-izes' a "a"

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// See doc in .hpp
// what it does
//    Initialize() define globals and wavelengths, check GLOn and compute if nec.
//    FillDataVectors define first Arc(BEG), fill 5 parallel arrays
//           xdata=time-begin, (dataWL, dataGF, dataIF), flag=OK or BAD
//           units cycles(WL,GF,NL), remove biases WLbias, GFbias, IFbias, get N12bias
//           internally data (WL,GF,IF,GR) has units wl(WL,GF,NL,(meters))
//              but dump all in meters.
//    ProcessOneCombo(WL)
//       GrossProcessing
//          filterFirstDiff - filter using first diff
//          mergeFilterResultsIntoArcs
//             outlier or short seg -> set flag for all segment; 
//             slip: if already a slip of this type, "unfix";
//                   else add an Arc and mark "slip"  ** define slip, not Nslip **
//          getArcStats
//          findLargeGaps
//          fixSlips  fix from here out using ** step, ADD to Nslip **
//       FineProcessing
//          filterWindow
//          mergeFilterResultsIntoArcs
//          getArcStats
//          fixSlips
//    end ProcessOneCombo
//           
//    ProcessOneCombo(GF)
//
//    final "double check" -- tbd
//
//    applyFixesToSatPass - only routine to change SatPass - apply all fixes
//       and outputs to SatPass and editing commands.
// NB
//  Arc "floats above" data vectors and does not modify them at all
//  Arcs don't "know" about each other - type BEG,slip,rej -- see top of gdc.cpp
//  flag vector also does not change data vectors, bitmaps -- see top of gdc.cpp
//  flag is either good (0) or not (!0); flags NOT related to SatPass or Arc::marks
//  Arcs handles BEG, SLIP/FIX, and GAP(new BEG); flags handle bad data w/in Arc
//  NB BAD != SatPass::BAD, but ONLY SatPass::BAD on input produces flag = BAD
//
// convert SatPass to data arrays and call DC(arrays)
int gdc::DiscontinuityCorrector(SatPass& SP, string& retMsg, vector<string>& cmds,
      int GLOn)
{
   try {
      sat = SP.getSat();
      isGLO = (sat.system == SatID::systemGlonass);
      GLOchan = GLOn;
      // if GLONASS frequency channel not given, try to find it
      if(isGLO && GLOchan == -99) {
         if(!SP.getGLOchannel(GLOchan, retMsg)) {
            retMsg = " Error - unable to compute GLO channel - fail: " + retMsg;
            return -9;
         }
         LOG(VERBOSE) << "# Compute GLO channel = " << GLOchan << " " << retMsg;
      }

      // get obstypes for this pass
      vector<string> obstypes(SP.getObsTypes());
      string L1("L1"), L2("L2"), P1("P1"), P2("P2");
      // useCA? no, assume caller knows what he is doing and only gave you C || P
      if(vectorindex(obstypes,string("P1")) == -1) P1=string("C1");
      if(vectorindex(obstypes,string("P2")) == -1) P2=string("C2");

      outfmt = SP.getOutputFormat();
      Epoch beg = SP.getFirstTime();

      vector<double> L1_in,L2_in,P1_in,P2_in,dt_in;
      vector<int> flags_in;

      // loop over the pass - MUST keep flags_in, dt_in, arrays all parallel
      for(unsigned int i=0; i<SP.size(); i++) {

         // save the seconds since beg
         dt_in.push_back(SP.time(i) - beg);

         // test for good data
         // must consistently mark bad data in SP with SatPass::BAD
         if(!(SP.spdvector[i].flag & SatPass::OK)
            || SP.data(i,L1) == 0.0 || SP.data(i,L2) == 0.0
            || SP.data(i,P1) == 0.0 || SP.data(i,P2) == 0.0)
         {
            flags_in.push_back(0);           // 0 bad - as in SatPass
            L1_in.push_back(0.0);
            L2_in.push_back(0.0);
            P1_in.push_back(0.0);
            P2_in.push_back(0.0);
            continue;
         }

         // good data
         flags_in.push_back(1);              // 1 good data - as in SatPass
         L1_in.push_back(SP.data(i,L1));
         L2_in.push_back(SP.data(i,L2));
         P1_in.push_back(SP.data(i,P1));
         P2_in.push_back(SP.data(i,P2));
      }

      // save the first GDC output line from SatPass (SPS)
      {
         ostringstream oss;
         oss << "GDC " << setw(3) << unique+1 << " SPS " << SP;
         SPSstr = oss.str();
      }

      int iret = DiscontinuityCorrector(sat, SP.getDT(), beg,
                  L1_in, L2_in, P1_in, P2_in, dt_in, flags_in,
                  retMsg, cmds, GLOchan, outfmt);
      if(iret) return iret;

      // apply fixes to SatPass
      if(cfg(doFix)) applyFixesToSatPass(SP);   // ,breaks,marks);

      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end int gdc::DiscontinuityCorrector(SatPass& SP, string& retMsg, int GLOn)

//------------------------------------------------------------------------------------
// Call to DC without SatPass.
// Flags on input must be either 1(OK) or 0(BAD), as in SatPass
int gdc::DiscontinuityCorrector(
      const RinexSatID& sat_in, const double& nominalDT, const Epoch& beginTime,
      vector<double> dataL1, vector<double> dataL2,
      vector<double> dataP1, vector<double> dataP2,
      vector<double> dt_in, vector<int> flags_in,
      string& retMsg, vector<string>& cmds,
      int GLOn, string outfmt_in)
{
   try {
      int iret;
      unsigned int i;

      // TD check that arrays are all full length

      sat = sat_in;
      dt = nominalDT;
      beginT = beginTime;
      beginT += dt_in[0];
      outfmt = outfmt_in;

      isGLO = (sat.system == SatID::systemGlonass);
      GLOchan = GLOn;
      if(isGLO && GLOchan == -99) {
         //// this is the non-SatPass version, still in SatPass.cpp
         //if(!getGLOchannel(GLOchan,sat,dataL1,dataL2,dataP1,dataP2,flags_in,retMsg))
         //{
         //   retMsg = " Error - unable to compute GLO channel - fail: " + retMsg;
            return -9;
         //}
      }

      // NB wl1,alpha,beta are used only in this routine...
      wl1 = getWavelength(sat.system,1,GLOchan);    // GLOchan ignored by GPS
      wl2 = getWavelength(sat.system,2,GLOchan);
      alpha = getAlpha(sat.system,1,2);
      beta = getBeta(sat.system,1,2);
      wlWL = wl2*(beta+1.0)/alpha;  // wl(WL) = 86cm GPS, depends on GLOchan
      wlGF = wl2 - wl1;             // wl(GF) = wl1-wl2 = 5.376cm GPS, or f(GLOchan)
                                    // wl(NL) = 10.7cm GPS, used for IF

      // fill data vectors from input -------------------------------
      Arc arc(0,0,0,Arc::BEG);
      xdata.clear(); flags.clear(); dataWL.clear(); dataGF.clear();

      // loop over the pass - MUST keep xdata, flags, dataWL and dataGF parallel
      double d,dtlast;
      arc.ngood = 0;
      for(i=0; i<dt_in.size(); i++) {
         // save the seconds since beginT
         xdata.push_back(dt_in[i]);

         // test for good data
         // caller must consistently mark bad data with SatPass::BAD(0)
         if(!(flags_in[i] & SatPass::OK)
                     || dataL1[i] == 0.0 || dataL2[i] == 0.0
                     || dataP1[i] == 0.0 || dataP2[i] == 0.0)
         {
            flags.push_back(BAD);         // bad data from SatPass
            dataWL.push_back(0.0);
            dataGF.push_back(0.0);
            continue;
         }

         // good data
         dtlast = dt_in[i];
         flags.push_back(OK);             // 0 good data
         arc.ngood++;

         // WLC = (WLphase - NLrange) in units of WLwl
         d = ((beta*wl1*dataL1[i] - wl2*dataL2[i]) / (beta-1.0)
            - (beta*dataP1[i] + dataP2[i]) / (beta+1.0)) / wlWL;
         if(arc.ngood == 1) WLbias = d;
         dataWL.push_back(d - WLbias);

         // LGF = wl1*L1 - wl2*L2 in units of GFwl
         d = (wl1*dataL1[i] - wl2*dataL2[i]) / wlGF;
         if(arc.ngood == 1) GFbias = d;
         dataGF.push_back(d - GFbias);

         // initial phase biases - mainly just for output
         if(arc.ngood == 1) {
            N1bias = static_cast<long long>(dataP1[i]/wl1 - dataL1[i]);
            N2bias = static_cast<long long>(dataP2[i]/wl2 - dataL2[i]);
         }
      }

      // fill the Arc as much as possible
      Arcs.clear();
      arc.index = 0;                      // always since we use data[0,size-1]
      arc.npts = xdata.size();
      Arcs[arc.index] = arc;

      // Begin GDC processing ----------------------------------------
      unique++;
      // generate strings for output
      ostringstream oss;
      oss << "GDC " << setw(3) << unique;
      tag = oss.str();
      if(SPSstr.empty()) {
         Epoch endT(beginT);
         endT += dtlast;
         oss.str("");
         oss << tag << " SPS " << setw(4) << arc.npts           // ntot
            << " " << sat << " " << setw(4) << arc.ngood        // sat ngood
            << " 0 "                // status flag
            << printTime(beginT,outfmt) << " " << printTime(endT,outfmt)
            << " " << setprecision(1) << fixed << dt << " L1 L2 P1 P2";
         SPSstr = oss.str();
      }
      LOG(INFO) << SPSstr;
      SPSstr = string();            // clear for next call

      // dump data with tag RAW
      if(cfg(RAW)) dumpData(LOGstrm,tag+" RAW");

      // check that segment is long enough
      if(flags.size() < 2*cfg(width)) {
         for(i=0; i<flags.size(); i++) {           // cf. flagBadData()
            if(flags[i] == OK) flags[i] = BAD;
         }
         LOG(INFO) << tag << " Pass is too short to analyze: "
            << flags.size() << " < 2 * window width = " << 2*cfg(width);
      }

      while(1) {
         // Process WL --------------------------------------
         iret = ProcessOneCombo(WL);
         if(iret < 0) break;

         // Process GF --------------------------------------
         iret = ProcessOneCombo(GF);
         if(iret < 0) break;              // TD not return?

         // Check value of slips found


         break;      // mandatory
      }  // end while(1)

      // final check
      iret = FinalCheck();
      if(iret < 0) { return iret; }    // never?

      // build the return message
      retMsg = returnMessage();
      if(cfg_func("verbose")) DumpArcs("#"+tag+" FIN","");

      // generate editing commands
      if(cfg(doCmds)) generateCmds(cmds);

      //// are there ever going to be breaks?
      //for(i=0; i<breaks.size(); i++)
      //   LOG(WARNING) << " Warning - GDC breaks pass at " << breaks[i];

      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end int gdc::DiscontinuityCorrector(dataL1, dataL2,...)

//------------------------------------------------------------------------------------
// NB return value == nslips is not used
int gdc::ProcessOneCombo(const unsigned which)
{
   try {
      int iret, nslips(0);

      // -------------------------------------------------------------------------
      // first look for gross slips using 1st differences, then compute stats,
      // look for gaps, and fix the slips (WLG GFG)
      iret = GrossProcessing(which);
      if(iret < 0) return iret;
      nslips += iret;

      // -------------------------------------------------------------------------
      // now look for small slips using window filter, then compute stats and
      // fix the slips (WLW GFW)
      iret = FineProcessing(which);
      if(iret < 0) return iret;
      nslips += iret;

      return nslips;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end ProcessOneCombo()

//------------------------------------------------------------------------------------
// process one combo (WL or GF) using 1st differences; called by ProcessOneCombo
// @return return value of filter() if negative, otherwise number of slips found
int gdc::GrossProcessing(const unsigned which)
{
   try {
      int i, nslips(0), iret;
      double limit;
      string label;
      vector< FilterHit<double> > filterResults;

      // filter using first difference, for gross slips and outliers
      // WL1 GF1
      label = LAB[which]+"1";
      limit = cfg_func(LAB[which]+"grossStep");
      iret = filterFirstDiff(which, label, limit, filterResults);
      if(iret < 0) return iret;
      nslips += iret;
   
      // dump filter hits
      if(cfg_func("debug") > -1) DumpHits(filterResults,"#"+tag,label,2);
   
      // merge 1st difference filter results with Arcs; returns number of new arcs
      // NB i unused
      i = mergeFilterResultsIntoArcs(filterResults, which);
      
      // recompute stats in each segment
      // not until window filter - gross slip can use Arc.info.step = FilterHit.step
      getArcStats(which);
   
      // dump Arcs
      if(cfg_func(label)) DumpArcs("#"+tag,label,2);
   
      // look for gaps > MaxGap, end Arc there, add Arc(BEG) where data resumes
      findLargeGaps();
   
      // fix gross slips
      // remove slips that are "size 0" -- do this in vector<FilterHit> ? no
      // NB i unused
      i = fixSlips(which);
   
      // dump data (WLG GFG)
      label = LAB[which]+"G"; 
      if(cfg_func(label)) dumpData(LOGstrm,tag+" "+label);
   
      return nslips;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end GrossProcessing()

//------------------------------------------------------------------------------------
// process one combo (WL or GF) using window filter; called by ProcessOneCombo
// @return return value of filter() if negative, otherwise number of slips found
int gdc::FineProcessing(const unsigned which)
{
   try {
      int i, nslips(0), iret;
      double limit;
      string label;
      map<int, Arc>::const_iterator ait;
      vector< FilterHit<double> > filterResults;

      // filter using the window filter
      label = LAB[which]+"W";                         // WLW or GFW
      limit = cfg_func(LAB[which]+"fineStep");
      iret = filterWindow(which,label,limit,filterResults);
      if(iret < 0) {
         // a segment is too small...
         //DumpArcs("#After FilterWindow ",label,2);
         return iret;
      }
      nslips += iret;         // iret >= 1 -- counts BOD
   
      // dump filter hits
      if(cfg_func("debug") > -1) DumpHits(filterResults,"#"+tag,label,2);
   
      // merge window filter results with Arcs
      i = mergeFilterResultsIntoArcs(filterResults, which);
   
      // Recompute stats in each segment.
      // NB Filters define FilterHit.step using their analysis; that step is then
      // copied over to Arc.xxinfo.step in mergeFilterResultsIntoArcs().
      // The first difference step is used to fix gross slips. The window filter step
      // (same as in the window algorithm) can also be used, or you could try to 
      // re-compute step using more data.
      getArcStats(which);
   
      // fix small slips using stats
      // NB i unused
      i = fixSlips(which);
   
      // dump Arcs
      if(cfg_func(label)) DumpArcs("#"+tag,label,2);
   
      // dump data WLF GFF
      label = LAB[which]+"F";
      if(cfg_func(label)) dumpData(LOGstrm,tag+" "+label);
   
      return nslips;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end FineProcessing()

//------------------------------------------------------------------------------------
// filter using first differences, to find gross slips and outliers
// param which is either WL or GF
// param label string to be passed to dump e.g. "GF1"
// param limit pass to filter.setLimit()
// return return value of filter() if negative (failure),
//                otherwise return number of FilterHit found (>= 1)
int gdc::filterFirstDiff(const unsigned which, const string label,
                           const double& limit, vector< FilterHit<double> >& hits)
{
   try {
      // configure first difference filter
      FirstDiffFilter<double> fdf(xdata, (which==GF ? dataGF:dataWL), flags);
      fdf.setw(cfg(oswidth));
      fdf.setprecision(cfg(osprec));
      fdf.setLimit(limit);

      // run it
      int iret = fdf.filter();
      if(iret < 0) return iret;

      // analyze results
      iret = fdf.analyze();

      // compute stats on each segment, then get results to return
      for(int i=0; i<fdf.results.size(); i++)
         fdf.getStats(fdf.results[i]);

      // NB must do this after getStats()
      hits = fdf.getResults();

      // dump filter results - will use stats from getStats() WL1 GF1
      // fdf.setDumpNoAnal(cfg(debug)>-1);
      if(cfg_func(label)) fdf.dump(LOGstrm, tag + " " + label);

      return iret;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end gdc::filterFirstDiff()

//------------------------------------------------------------------------------------
// filter using window filter
// param which is either WL or GF
// param label string to be passed to dump e.g. "GFG"
// param limit pass to filter.setLimit()
// param return vector< FilterHit<T> > hits containing all outliers and slips
// return return value of filter() if negative, otherwise number of slips found
int gdc::filterWindow(const unsigned which, const string label,
                      const double& limit, vector< FilterHit<double> >& hits)
{
   try {
      unsigned int i;

      // configure window filter
      WindowFilter<double> wf(xdata, (which==GF ? dataGF : dataWL), flags);
      wf.setWidth(cfg(width));
      wf.setw(cfg(oswidth));
      wf.setprecision(cfg(osprec));
      wf.setMinStep(limit);
      wf.setTwoSample(which==GF);
      //wf.setDebug(true);            // TEMP

      // run it
      int iret = wf.filter();
      if(iret == -2) {
         LOG(ERROR) << " Call to GF window filter without time data!";
         GPSTK_THROW(Exception("Call to GF window filter without time data"));
      }
      else if(iret == -1 || iret == -3) {    // segment is too small
         return iret;
      }

      // analyze results
      iret = wf.analyze();

      // compute stats on each segment, then get results to return
      for(int i=0; i<wf.results.size(); i++)
         wf.getStats(wf.results[i]);

      // NB this must be after getStats()
      hits = wf.getResults();

      // dump filter results - will use stats from getStats()
      wf.setDumpAnalMsg(cfg(debug)>-1 || cfg(verbose)!=0);
      if(cfg_func(label)) wf.dump(LOGstrm, tag + " " + label);

      //LOG(INFO) << " There are " << wf.maybes.size() << " maybes";
      for(i=0; i<wf.maybes.size(); i++) {
         //if(wf.maybes[i].score < 66) continue;
         if(wf.maybes[i].score < 85) continue;
         LOG(INFO) << "#" << tag << " " << sat
            << " poss(" << wf.maybes[i].score << "%)"
            << " " << LAB[which] << " slip: step "
            << fixed << setprecision(2) << setw(6) << wf.maybes[i].step << " wl"
            //<< " indx " << wf.maybes[i].index
            << " " << printTime(xtime(wf.maybes[i].index),outfmt)
            << (cfg(debug)>-1 ? wf.maybes[i].msg : "");
      }

      return iret;
   }
   catch(std::exception& e) {
      LOG(ERROR) << "std exception " << e.what();
      GPSTK_THROW(Exception(string("std exception")+e.what()));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end int gdc::filterWindow()

//------------------------------------------------------------------------------------
// merge filter results (vector<FilterHit>) into the Arcs list, and set flags[].
// The merge will mark outliers, add new Arc's where there are slips, and call
// fixUpArcs(), if necessary.
// Test with cases where there is huge data rejection in GF, after WL slips, etc.
// param hits vector<FilterHit> which is results of filter
// param which is either WL or GF
// return the number of new Arcs in Arcs
int gdc::mergeFilterResultsIntoArcs(vector< FilterHit<double> >& hits,
                                    const unsigned which)
{
   try {
      // is this necessary? ever used?
      if(Arcs.size() == 0) GPSTK_THROW(Exception("No Arcs found"));
      if(hits.size() == 0) GPSTK_THROW(Exception("No Filter results found"));

      bool fixup(false);
      int i, narcs(0);
      unsigned int minpts(cfg(MinPts));
      double lostslip(0.0);

      // flag data BAD for new outliers and small segments
      for(i=0; i<hits.size(); i++) {
         LOG(DEBUG) << "#" << tag << " merge " << LAB[which]
            << " hit into Arc[" << hits[i].index << "] " << hits[i].asString();

         // hits[i].type can be BOD slip outlier other(never used)
         if(hits[i].type == FilterHit<double>::BOD) {
            ; // nothing to do
         }

         else if(hits[i].type == FilterHit<double>::outlier) {
            unsigned int flag = (which == WL ? WLOUTLIER : GFOUTLIER);
            // mark all the data in this hit
            flagBadData(hits[i], flag);
            fixup = true;
         }

         //else if(hits[i].type == FilterHit<double>::other) {
         // never used
         //}

         else if(hits[i].type == FilterHit<double>::slip) {       // slip
            // if too short, mark it and don't make an Arc; however
            // accumulate the slip magnitude for following slips
            if(hits[i].ngood < minpts) {                          // too short
               // mark all the data in this hit
               flagBadData(hits[i], (which==WL ? WLSHORT : GFSHORT));
               fixup = true;

               // save the slip, to add to later slips
               lostslip += hits[i].step;

               continue;
            }

            // find the Arc in which this hit lies
            map<int,Arc>::iterator ait = Arcs.begin();
            findArc(hits[i].index, ait);

            // is there already an Arc here?
            if(hits[i].index == ait->second.index) {
               // already an Arc at this index
               if(ait->second.mark & SLIP[which]) {   // already a slip(which) here
                  if(ait->second.mark & FIX[which]) { // and its been fixed
                     // happens when gross and fine slip
                     // just remove the fix mark...
                     ait->second.mark ^= FIX[which];
                  }
                  else GPSTK_THROW(                   // marked SLIP but not fixed
                     Exception("Already marked but unfixed should not happen"));
               }
               else                                   // no slip(which) here
                  ait->second.mark |= SLIP[which];    // so mark it SLIP

               // no need for fixup
            }

            // no Arc at this point - add one
            else {
               if(addArc(hits[i].index, SLIP[which])) {
                  narcs++;
                  fixup = true;
               }
            }

            // copy hits[i].step into the Arc; this will be used in fixSlips()
            (which == WL ? Arcs[hits[i].index].WLinfo.step :
                           Arcs[hits[i].index].GFinfo.step)
                           = lostslip + hits[i].step;
            (which == WL ? Arcs[hits[i].index].WLinfo.sigma :
                           Arcs[hits[i].index].GFinfo.sigma)
                           = lostslip + hits[i].sigma;
            lostslip = 0.0;

         }  // end if slip

         // NB note there is a continue stmt above

      }  // end loop over hits

      if(fixup) fixUpArcs();     // recompute points for all Arcs

      return narcs;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end int gdc::mergeFilterResultsIntoArcs()

//------------------------------------------------------------------------------------
// Flag bad data in the flags[] array, using a filter hit object. Don't alter Arcs
// NB FixUpArcs() must be called after this routine to recompute ngood.
// Note that flags[] is changed ONLY if flags[] currently OK....TD use bitmap?
// @param hit        the FilterHit, which describes a segment to be marked
// @param flagvalue  the value to which flags[] is set for each outlier.
void gdc::flagBadData(const FilterHit<double>& hit, const unsigned flagvalue)
{
   // loop over all the data in this segment (hit)
   for(unsigned int i=hit.index; i<hit.index+hit.npts; i++) {
      if(flags[i] == OK) {
         flags[i] = flagvalue;            // TD let flags be a bitmap too?
         // don't modify Arc.ngood b/c fixUpArcs() will have to be called anyway
      }
   }
}  // end void gdc::flagBadData()

//------------------------------------------------------------------------------------
// add a new Arc to Arcs at index, using the given value for mark. If there is
// already an Arc at index, instead just assign the mark using &= (its a bitmap).
// param index the index into data[] at which to add the Arc
// param mark the value to assign to the new Arc's mark
// return true if new Arc created -> fixUpArcs() should be called.
bool gdc::addArc(const int index, const unsigned mark)
{
   // find the Arc containing the given index
   map<int,Arc>::iterator ait = Arcs.begin();
   findArc(index, ait);

   // create a new segment
   Arc B;
   B.mark = mark;
   B.index = index;
   B.npts = ait->second.npts - (index-ait->second.index);
   B.ngood = 0;

   // add it
   Arcs[B.index] = B;
   //if(cfg(debug)>-1) LOG(INFO) << "# Add new Arc " << B.asString();

   // modify the existing segment
   ait->second.npts = index - ait->second.index;
   ait->second.ngood = 0;

   // replace it
   B = ait->second;        // B is now the original Arc
   Arcs.erase(ait);
   Arcs[B.index] = B;

   return true;            // have to recompute ngood's in fixUpArcs()

}  // end bool gdc::addArc()

//------------------------------------------------------------------------------------
// modify Arcs: recompute npts and ngood, remove empty Arcs
// TD also remove Arcs with slip == 0
void gdc::fixUpArcs(void)
{
   try {
      map<int,Arc>::iterator ait;
      vector<int> tmpArcs;

      // recompute all the Arcs npts and ngood
      recomputeArcs();

      // ensure that Arcs[a.index] = a;
      map<int,Arc> oldArcs(Arcs);
      Arcs.clear();
      for(ait = oldArcs.begin(); ait != oldArcs.end(); ++ait)
         Arcs[ait->second.index] = ait->second;

   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end void gdc::fixUpArcs()

//------------------------------------------------------------------------------------
// recompute the npts and ngood for each Arc using the indexes in the map: Arcs
void gdc::recomputeArcs(void)
{
   try {
      map<int,Arc>::iterator ait, cit;

      // loop over Arcs, recomputing npts
      cit = ait = Arcs.begin();
      while(++cit != Arcs.end()) {
         ait->second.npts = cit->second.index - ait->second.index;
         ++ait;
      }
      ait->second.npts = xdata.size() - ait->second.index;  // last one

      // loop over Arcs, recomputing ngood
      for(ait = Arcs.begin(); ait != Arcs.end(); ++ait) {
         computeNgood(ait->second);                         // ignore return value
         //LOG(INFO) << "GDC recomputeArcs " << ait->second.asString();
      }
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// compute stats for 'which' data (WL or GF but not both) for the given Arc.
// NB this is sneaky and goes across fixed slips....
// NB do not confuse this with *Filter::getStats()
// NB there is a getArcStats(which) which loops over all Arcs calling this for each.
// param iterator pointing to element of Arcs that contains the Arc
// param which is either WL or GF
void gdc::getArcStats(map<int,Arc>::iterator& ait, const unsigned which)
{
   try {
      bool isWL(which == WL);
      int i,index,npts;
      map<int,Arc>::iterator cit(ait);
         
      StatsFilterBase<double> *ptrStats;
      if(isWL) ptrStats = new OneSampleStatsFilter<double>();
      else     ptrStats = new TwoSampleStatsFilter<double>();
      i = index = cit->second.index;
      npts = cit->second.npts;
      
      // loop over continuous data in the arc
      while(cit != Arcs.end() && i < xdata.size()) {
         // add to stats (xdata is ignored in OneSampleStats)
         // don't include bad data, unless this is a REJ arc...
         if(flags[i] == OK || cit->second.mark == Arc::REJ)
            ptrStats->Add(xdata[i], (isWL ? dataWL[i] : dataGF[i]));
         i++;
         if(i == index+npts) {                     // reached end of Arc
            cit++;                                 // go to the next one..
            if(cit == Arcs.end())                  // ..unless there isn't one
               break;
            if(!(cit->second.mark & SLIP[which]))  // ..or its not a slip
               break;
            if(!(cit->second.mark & FIX[which]))   // ..or its not been fixed
               break;
            index = cit->second.index;
            npts = cit->second.npts;
         }
      }

      // store results (N,ave/aveY,sig/sigYX) in the original Arc
      (isWL ? ait->second.WLinfo : ait->second.GFinfo).n = ptrStats->N();
      (isWL ? ait->second.WLinfo : ait->second.GFinfo).ave = ptrStats->Average();
      (isWL ? ait->second.WLinfo : ait->second.GFinfo).sig = ptrStats->StdDev();

   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end void gdc::getArcStats()

//------------------------------------------------------------------------------------
// find gaps within Arc in Arcs; if gap is larger than MaxGap, break the Arc into
// two, adding a BEG Arc (unless its at the very end of the data).
// param which is either WL or GF
void gdc::findLargeGaps(void)
{
   try {
      int limit(cfg(MaxGap));
      map<int,int> allgaps, gaps;
      map<int,int>::const_iterator git;
      map<int,Arc>::iterator ait;

      // loop over Arcs
      for(ait = Arcs.begin(); ait != Arcs.end(); ++ait) {
         // find all the gaps
         gaps = findGaps(ait->second);
         if(gaps.size() == 0) continue;

         allgaps.insert(gaps.begin(),gaps.end());
      }

      // process the gaps
      // must do it this way (not within Arcs loop), in case one Arc gets split twice
      bool fixup(false);
      for(git=gaps.begin(); git!=gaps.end(); ++git) {
         if(git->second <= limit)
            continue;                     // skip small gaps

         // find the Arc it belongs to
         ait = Arcs.begin();
         findArc(git->first, ait);

         if(git->first == ait->second.index)
            continue;                     // skip 'gap' at beginning of Arc
         if(git->first+git->second == ait->second.index+ait->second.npts)
            continue;                     // skip 'gap' at end of Arc

         // Arc at ait must be split      // we don't need fixUp
         addArc(git->first+git->second, Arc::BEG);

         // must recompute ngood, but only for one Arc .. oh well
         ait = Arcs.begin();              // iterator is corrupted by addArc
         findArc(git->first+git->second, ait);
         computeNgood(ait->second);
      }
      if(fixup) fixUpArcs();              // recompute points for all Arcs

   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end void gdc::findLargeGaps()

//------------------------------------------------------------------------------------
// find gaps within the given Arc, including those at the very beginning
// (index==Arc.index) and at the very end (index+nptsgap==Arc.index+Arc.npts).
// return map with key=index of beginning of gap, value=number of points in gap.
map<int,int> gdc::findGaps(const Arc& arc)
{
   try {
      unsigned int i,count,index;// count consecutive bad pts, starting at index
      map<int,int> gaps;
      for(count=0,i=arc.index; i < arc.index+arc.npts; ++i) {
         if(flags[i] == OK) {    // good
            if(count > 0) {      // is there a gap ending here?
               gaps[index] = count;
               count = 0;
            }
         }
         else {                  // bad - add to count
            if(count == 0) index = i;
            count++;
         }
      }
      if(count > 0) {            // is there a gap at the very end?
         gaps[index] = count;
      }

      return gaps;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end map<int,int> gdc::findGaps(const Arc& arc) throw(Exception)

//------------------------------------------------------------------------------------
// fix slips between Arcs, using info.step (NOT info.Nslip), which is defined by the
// filter in results(FilterHit). Compute an integer from step and ADD it to Nslip.
// Thus Nslip has the total slip, while step has only the last estimate, used to fix.
// In the case of the FirstDifferenceFilter this step
// is only an approximate fix; for the WindowFilter the step is defined by stats
// on the two segments (one-sample for WL and two-sample for GF).
// param which is either WL or GF
// return the number of slips fixed.
int gdc::fixSlips(const unsigned which)
{
   try {
      int i,nslips(0);
      long N;
      double step,istep;
      static const double GFfactor(wl2/wlGF);
      static const double IFfactor(isGLO ? 3.5 : 3.52941176470588);// TD what is this?

      // loop over Arcs using iterator ait //, with dummy copy cit 
      map<int,Arc>::iterator ait;   //, cit;
      for(ait = Arcs.begin(); ait != Arcs.end(); ++ait) {
         if((ait->second.mark & SLIP[which]) == 0)             // its not a slip
            continue;
         if((ait->second.mark & FIX[which]) != 0)              // its been fixed
            continue;

         LOG(DEBUG) << "#" << tag << " fix slip for Arc[" << ait->first << "] "
            << ait->second.asString();

         // first get the step in wavelengths for this slip (not the total)
         step = (which==WL ? ait->second.WLinfo.step : ait->second.GFinfo.step);

         // get the integer-wavelengths step, and use this for fixing
         N = long(step + (step > 0.0 ? 0.5 : -0.5));
         istep = static_cast<double>(N);

         // accumulate Nslip, which is the total slip
         // NB b/c gross and fine both contribute, and want the total slip at the end
         (which==WL ? ait->second.WLinfo.Nslip : ait->second.GFinfo.Nslip) += N;

         // mark it fixed, TD but if N==0 shouldn't you remove the SLIP?
         ait->second.mark |= FIX[which];

         // if it's a non-zero step, modify the data from here all the way out
         if(N) {
            nslips++;                                       // count it
            if(which == WL) {
               for(i=ait->second.index; i<xdata.size(); i++) {
                  dataWL[i] -= istep;
                  dataGF[i] -= GFfactor*istep;
               }
            }
            else {
               for(i=ait->second.index; i<xdata.size(); i++) {
                  dataGF[i] -= istep;
               }
            }

            // save 'learn' message here, with time tag, which, step, etc

         }  // end if N
      }

      return nslips;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end int gdc::fixSlips(const unsigned which) throw(Exception)

// do a final check on the pass. Look for isolated good points (< MinPts good
// points surrounded by N(?) bad points on each side.
int gdc::FinalCheck(void)
{
   try {
      bool fixup(false);
      unsigned int i,k;
      int iret(0),j,currstate,ngood,nbad;
      vector<int> gbs;
      
      // look for segments of N good (+) or bad (-) points
      currstate = ngood = nbad = 0;
      for(i=0; i<xdata.size(); i++) {
         if(flags[i] == 0) {
            if(currstate == -1) {         // end of bad segment
               gbs.push_back(-nbad);
               nbad = ngood = 0;
            }
            currstate = 1;
            ngood++;
         }
         else {
            if(currstate == 1) {          // end of good segment
               gbs.push_back(ngood);
               nbad = ngood = 0;
            }
            currstate = -1;
            nbad++;
         }
      }

      // look for isolated good segments
      int min=cfg(MinPts);
      if(min > 10) min=10;
      int gap=cfg(MaxGap);
      if(gap > 10) gap=10;

      // only do for >3 segments
      // k is current index, used for marking
      if(gbs.size() > 3) for(k=0,i=0; i<gbs.size(); i++) {
         if(gbs[i] > 0 && gbs[i] < min) {    // current segment is good and short
            // can now assume gbs[i-1] and gbs[i+1] are < 0 (bad)

            // prev segment is first or last
            bool prev1(i==1);
            // next segment is last
            bool next1(i==gbs.size()-2);
            // prev segment is big and bad, or not there
            bool prevbb(i==0 || -gbs[i-1] > gap);
            // next segment is big and bad, or not there
            bool nextbb(i==gbs.size()-1 || -gbs[i+1] > gap);
            // prev is 2nd or 3rd, and 1st or 2nd is small
            bool prev23((i==2 && gbs[0] < gap) || (i==3 && gbs[1] < gap));
            // next is 2nd or 3rd from end, and 1st or 2nd is small
            bool next23((i==gbs.size()-1 && -gbs[i-1] < gap) ||
                        (i==gbs.size()-2 && -gbs[i+1] < gap) ||
                        (i==gbs.size()-3 && -gbs[i+1] < gap));
            bool ibeg(i==0 && -gbs[1] > gap);
            bool iend(i==gbs.size()-1 && -gbs[i-1] > gap);

            if((prevbb && nextbb) || prev1 || next1 ||
                           prev23 || next23 || ibeg || iend)
            {
               fixup = true;
               //oss2 << " Mark:";
               for(j=0; j<gbs[i]; j++) {
                  flags[k+j] = ISOLATED;
               //oss2 << " " << k+j;
               }
            }
         }  // end if long and bad

         // keep k pointed to start of this segment
         k += ::abs(gbs[i]);
      }

      if(fixup) fixUpArcs();     // recompute points for all Arcs

      if(cfg_func("FIN")) dumpData(LOGstrm,tag+" FIN");

      return iret;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// end of processing routines

//------------------------------------------------------------------------------------
void gdc::dumpData(ostream& os, const string msg)
{
   size_t i,k;
   map<int, Arc>::const_iterator kt = Arcs.begin();

   for(k=0,i=0; i<xdata.size(); i++) {
      string arcmsg;
      if(kt != Arcs.end() && kt->second.index == i) {
         arcmsg = string(" ") + kt->second.asString();
         kt++;
      }

      os << msg << " " << sat << " " << printTime(xtime(i),outfmt)
         << fixed << setprecision(3)
         << " " << setw(9) << (xtime(i) - beginT)     //  xdata[i]
         << " " << setw(2) << flags[i] << fixed << setprecision(4)
         << " " << setw(9) << dataWL[i] * wlWL
         << " " << setw(9) << dataGF[i] * wlGF
         << arcmsg << endl;
   }
}  // end void gdc::dumpData(ostream& os, const string msg)

//------------------------------------------------------------------------------------
void gdc::DumpHits(const vector< FilterHit<double> >& filterResults,
                     const string& tag, const string& label, int prec) throw()
{
   if(prec == -1) prec = cfg(osprec);
   for(int i=0; i<filterResults.size(); i++) {
      LOG(INFO) << tag << " " << label
         << " Hit" << i+1 << "[" << filterResults[i].index << "] "
         << filterResults[i].asStatsString(prec);
   }
}

//------------------------------------------------------------------------------------
void gdc::DumpArcs(const string& tag, const string& label, int prec) throw()
{
   if(prec == -1) prec = cfg(osprec);
   int i(1);
   map<int, Arc>::const_iterator ait;
   for(ait = Arcs.begin(); ait != Arcs.end(); ++ait) {
      LOG(INFO) << tag << (label.size() ? " "+label : "")
               << " Arc" << i++ << "[" << ait->first << "] "
               << ait->second.asString(prec);
   }
}  // end void gdc::DumpArcs()

//------------------------------------------------------------------------------------
// build the string that is returned by the discontinuity corrector
string gdc::returnMessage(int prec, int wid) throw()
{
   int minpts(cfg(MinPts));
   string retmsg;
   ostringstream oss,oss2;
   map<int, Arc>::iterator ait;

   if(prec == -1) prec = cfg(osprec);
   if(wid == -1) wid = cfg(oswidth);
   oss << fixed << setprecision(prec);
   oss2 << fixed << setprecision(prec);

   // Find segs (>MinPts) w/ ngood=0, call "REJ" Arcs; recompute stats
   // each Arc can potentially be broken into three: REJ,Arc,REJ
   map<int,Arc> newArcs;
   for(ait = Arcs.begin(); ait != Arcs.end(); ++ait) {
      int n,i,ib(ait->second.index),ie(ait->second.index+ait->second.npts);

      // first part of Arc
      for(n=0,i=ib; i<ie; i++)
         if(flags[i] == 0) break; else n++;

      // if the entire Arc is bad data, just relabel it REJ
      if(n == ait->second.npts) {
         ait->second.mark = Arc::REJ;
         continue;
      }

      // if first part of Arc is >minpts of bad data, call it a REJ Arc
      if(n > minpts) {
         Arc A;
         A.mark = Arc::REJ;
         A.index = 0;
         A.npts = n; A.ngood = 0;
         newArcs[A.index] = A;
         ait->second.index = ib+n;
      }

      // last part of Arc
      for(n=0,i=ie-1; i>=ib; i--)
         if(flags[i] == 0) break; else n++;
      if(n > minpts) {
         Arc A;
         A.mark = Arc::REJ;
         A.index = ie-n;
         A.npts = n; A.ngood = 0;
         newArcs[A.index] = A;
      }
   }

   if(newArcs.size()) {
      // add existing ones to newArcs, changing the key to (possibly new) index
      for(ait = Arcs.begin(); ait != Arcs.end(); ++ait)
         newArcs[ait->second.index] = ait->second;
      // now remake Arcs
      Arcs.clear();
      for(ait = newArcs.begin(); ait != newArcs.end(); ++ait) {
         Arcs[ait->second.index] = ait->second;
      }
      recomputeArcs();
   }
   // recompute stats for all arcs // TD better to do it before this
   getArcStats(WL);
   getArcStats(GF);

   // TD add a SUM line giving number of slips, fixes, etc.

   // loop over the Arcs, converting each to a line of the message
   for(ait = Arcs.begin(); ait != Arcs.end(); ++ait) {
      const Arc A(ait->second);
      const unsigned mark(A.mark);

      oss.str(""); oss2.str("");
      if(mark & Arc::BEG) {
         oss << "BEG";
      }
      else if((mark & Arc::WLSLIP) || (mark & Arc::GFSLIP)) {
         oss << "FIX";
         oss2 << " n(WL,GF) " << A.WLinfo.Nslip << "," << A.GFinfo.Nslip;
      }
      else if(mark & Arc::REJ) {
         oss << "REJ";
         // NB REJ'd arcs can still hold slips
         if(A.WLinfo.Nslip != 0 || A.GFinfo.Nslip != 0)
            oss2 << " n(WL,GF) " << A.WLinfo.Nslip << "," << A.GFinfo.Nslip;
      }
      oss << " " << setw(4) << A.index
         << " " << printTime(xtime(A.index),outfmt)
         << " " << setw(4) << A.npts << " " << setw(4) << A.ngood;
      if(A.WLinfo.n > 0) oss << " WL " << setw(4) << A.WLinfo.n
         << " " << setw(wid) << A.WLinfo.ave
         << " +- " << setw(wid) << A.WLinfo.sig;
      if(A.GFinfo.n > 0) oss << " GF " << setw(4) << A.GFinfo.n
         << " " << setw(wid) << A.GFinfo.ave
         << " +- " << setw(wid) << A.GFinfo.sig;
      oss << oss2.str();

      retmsg += oss.str() + "\n";
   }
   StringUtils::stripTrailing(retmsg,'\n');

   return retmsg;
}

//------------------------------------------------------------------------------------
// apply the results to fix the input SatPass cf. cfg(doFix)
// param SP       SatPass object containing the input data.
// param breaks  vector of indexes where SatPass SP must be broken into two
// param marks   vector of indexes in SatPass SP where breaks are suspected
void gdc::applyFixesToSatPass(SatPass& SP)
                           //, vector<int>& breaks, vector<int>& marks)
{
try {
   unsigned int i,j;
   long long nGF, nWL, nL1, nL2;
   Epoch ttag,tbeg,tend;
   map<int, Arc>::iterator ait;

   double dL1, dL2;                       // just double versions of nL1, nL2
   const string L1("L1"), L2("L2");

   nL1 = nL2 = 0;                         // running total slip
   ait = Arcs.begin();  
   for(i=0; i<SP.size(); i++) {
      if(ait != Arcs.end() && i == ait->first) {      // at new arc
         Arc& arc(ait->second);

         LOG(DEBUG) << "#" << tag << " applyFix with Arc[" << ait->first << "] "
                                 << ait->second.asString();

         if((arc.mark & (Arc::WLSLIP | Arc::GFSLIP)) || (arc.mark & Arc::REJ)) {
            // redefine biases nL1 nL2
            nGF = arc.GFinfo.Nslip;
            nWL = arc.WLinfo.Nslip;
            // real slips do accumulate here
            nL1 -= nGF;                // b/c Ngf(corrected) = -N1
            nL2 -= (nGF+nWL);          // b/c Nwl = N1-N2
            dL1 = static_cast<double>(nL1);
            dL2 = static_cast<double>(nL2);
         }

         //if((arc.mark & Arc::WLMARK) || (arc.mark & Arc::GFMARK)) {
         //   marks.push_back(i);
         //   NO continue;
         //}

         if(arc.mark & Arc::REJ) {
            // reject all the data in this Arc
            for(j=0; j<arc.npts; j++) {
               SP.setFlag(i+j,SatPass::BAD);
               if(cfg(UserFlag)) SP.setUserFlag(i+j,cfg(UserFlag));
            }
         }

         if(arc.mark & Arc::BEG) {
            if(i != 0) LOG(WARNING) << " Warning - GDC breaks pass at index " << i;
         }

         // increment ait, prep for next arc
         ++ait;
      }

      if(flags[i] == BAD)              // nothing to do - SatPass set before call
         continue;

      if(flags[i] != OK) {
         SP.setFlag(i,SatPass::BAD);
         if(cfg(UserFlag)) SP.setUserFlag(i,cfg(UserFlag));
      }
      else {
         if(nL1) SP.data(i,L1) -= dL1;
         if(nL2) SP.data(i,L2) -= dL2;
      }
   }  // end loop over data in SP
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// apply the results to generate editing commands; cfg(doCmds)
// Use tk-RinEdit form for commands (--IF name, etc) since EditRinex also takes.
// @param cmds     vector of strings giving editing commands for RINEX editor.
void gdc::generateCmds(vector<string>& cmds)
{
try {
   unsigned int i,j,k;
   long long nGF, nWL, nL1, nL2;
   Epoch ttag,tbeg,tend;
   map<int, Arc>::iterator ait;
   static const string L1(cfg(doRINEX3) ? "L1C":"L1"), L2(cfg(doRINEX3) ? "L2W":"L2");

   // generate commands
   ostringstream oss;
   oss << "--BD+ " << sat << "," << L1 << ","
         << printTime(beginT,"%Y,%m,%d,%H,%M,%S,")
         << N1bias << printTime(beginT," # initial L1 bias at %F,%.3g");
   cmds.push_back(oss.str()); oss.str("");
   oss << "--BD+ " << sat << "," << L2 << ","
         << printTime(beginT,"%Y,%m,%d,%H,%M,%S,")
         << N2bias << printTime(beginT," # initial L2 bias at %F,%.3g");
   cmds.push_back(oss.str()); oss.str("");

   for(ait = Arcs.begin(); ait != Arcs.end(); ++ait) {
      Arc& arc(ait->second);

      // apply slips -- REJ can store a slip - see karr0880.10o pass 7
      if((arc.mark & (Arc::WLSLIP | Arc::GFSLIP)) || (arc.mark & Arc::REJ)) {
         nGF = arc.GFinfo.Nslip;
         nWL = arc.WLinfo.Nslip;
         // slips don't accumulate here, but editing commands do
         nL1 = -nGF;                // b/c Ngf(corrected) = -N1
         nL2 = -nGF-nWL;            // b/c Nwl = N1-N2
         ttag = xtime(arc.index);
         if(nL1) {
            oss << "--BD+ " << sat << "," << L1 << ","
                << printTime(ttag,"%Y,%m,%d,%H,%M,%S,") << -nL1
                << printTime(ttag," # L1 slip at %F,%.3g");
            cmds.push_back(oss.str()); oss.str("");
         }
         if(nL2) {
            oss << "--BD+ " << sat << "," << L2 << ","
                << printTime(ttag,"%Y,%m,%d,%H,%M,%S,") << -nL2
                << printTime(ttag," # L2 slip at %F,%.3g");
            cmds.push_back(oss.str()); oss.str("");
         }
      }

      // delete entire segment
      if(arc.mark & Arc::REJ) {
         tbeg = xtime(arc.index);
         tend = xtime(arc.index+arc.npts-1);
         tend += dt;                   // NB DD- means stop here, don't do this one
         if(arc.npts == 1) {
            oss << "--DD " << sat << "," << L1 << ","
            << printTime(tbeg,"%Y,%m,%d,%H,%M,%S # delete outlier at %F,%.3g");
            cmds.push_back(oss.str()); oss.str("");
            oss << "--DD " << sat << "," << L2 << ","
            << printTime(tbeg,"%Y,%m,%d,%H,%M,%S # delete outlier at %F,%.3g");
            cmds.push_back(oss.str()); oss.str("");
         }
         else {
            oss << "--DD+ " << sat << "," << L1
               << "," << printTime(tbeg,"%Y,%m,%d,%H,%M,%S # from %F,%.3g")
               << " - delete entire segment = " << arc.npts << " epochs";
            cmds.push_back(oss.str()); oss.str("");
            oss << "--DD- " << sat
            << "," << L1 << ","<< printTime(tend,"%Y,%m,%d,%H,%M,%S # to %F,%.3g");
            cmds.push_back(oss.str()); oss.str("");
            oss << "--DD+ " << sat << "," << L2
            << "," << printTime(tbeg,"%Y,%m,%d,%H,%M,%S # from %F,%.3g");
            cmds.push_back(oss.str()); oss.str("");
            oss << "--DD- " << sat
            << "," << L2 << ","<< printTime(tend,"%Y,%m,%d,%H,%M,%S # to %F,%.3g");
            cmds.push_back(oss.str()); oss.str("");
         }

         continue;
      }

      // if there are no outliers, done
      if(arc.ngood == arc.npts) continue;

      // now run over the data in this Arc looking for outliers
      bool bad=false; j=0;
      k = arc.index+arc.npts;
      for(i=arc.index; i<k; i++) {
         if(!bad) {
            if(flags[i] == OK) continue;
            j = i;
            bad = true;
         }

         if(bad && (flags[i] == OK || i == k-1)) {
            if((flags[i]==OK && i == j+1) || (i==k-1 && i==j)) {
                                                   // isolated outlier
               ttag = xtime(j);
               oss << "--DD " << sat << "," << L1 << ","
               << printTime(ttag,"%Y,%m,%d,%H,%M,%S # delete outlier at %F,%.3g");
               cmds.push_back(oss.str()); oss.str("");
               oss << "--DD " << sat << "," << L2 << ","
               << printTime(ttag,"%Y,%m,%d,%H,%M,%S # delete outlier at %F,%.3g");
               cmds.push_back(oss.str()); oss.str("");
            }
            else {                                 // more than one outlier
               ttag = xtime(j);
               oss << "--DD+ " << sat << "," << L1 << "," << printTime(ttag,
                  "%Y,%m,%d,%H,%M,%S # delete outliers starting at %F,%.3g");
               cmds.push_back(oss.str()); oss.str("");
               oss << "--DD+ " << sat << "," << L2 << "," << printTime(ttag,
                  "%Y,%m,%d,%H,%M,%S # delete outliers starting at %F,%.3g");
               cmds.push_back(oss.str()); oss.str("");

               ttag = xtime(i==k-1 ? i : i-1);
               ttag += dt;
               oss << "--DD- " << sat << "," << L1 << "," << printTime(ttag,
                  "%Y,%m,%d,%H,%M,%S # end deleting outliers at %F,%.3g");
               cmds.push_back(oss.str()); oss.str("");
               oss << "--DD- " << sat << "," << L2 << "," << printTime(ttag,
                  "%Y,%m,%d,%H,%M,%S # end deleting outliers at %F,%.3g");
               cmds.push_back(oss.str()); oss.str("");
            }
            bad = false;
         }
      }

   }  // end loop over Arcs

}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// configuration
//------------------------------------------------------------------------------------
// Set a parameter in the configuration; the input string 'cmd' is of the form
// '[--DC]<id><s><value>' : separator s is one of ':=,'; leading --DC is optional.
bool gdc::setParameter(string cmd)
{
   try {
      if(cmd.empty()) return false;
         // remove leading --DC
      while(cmd[0] == '-') cmd.erase(0,1);
      if(cmd.substr(0,2) == "DC") cmd.erase(0,2);

      string label, value;
      string::size_type pos=cmd.find_first_of(",=:");
      if(pos == string::npos) {
         label = cmd;
      }
      else {
         label = cmd.substr(0,pos);
         value = cmd;
         value.erase(0,pos+1);
      }

      return setParameter(label, StringUtils::asDouble(value));
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end bool gdc::setParameter(string cmd) throw(Exception)

//------------------------------------------------------------------------------------
// Set a parameter in the configuration using the label and the value,
// for booleans use (T,F)=(non-zero,zero).
bool gdc::setParameter(string label, double value)
{
   if(CFG.find(label) == CFG.end())
      return false; // GPSTK_THROW(Exception("Unknown configuration label " + label));

   CFG[label] = value;

   // if debug is turned on, turn on some/all of output as well
   if(CFG["debug"] > -1) {
      LOG(DEBUG) << "Set GDC " << label << " to " << value;
   }

   // Turn output on/off if debug is being set
   // NB this does NOT set LOG(DEBUG)
   if(label == string("debug")) {
      // first return to default
      CFG["RAW"] = CFG["WLF"] = CFG["GFF"] = CFG["WL1"] = CFG["WLG"] = CFG["WLW"] = 
      CFG["WLF"] = CFG["GF1"] = CFG["GFG"] = CFG["GFW"] = CFG["GFF"] = CFG["FIN"] = 0;
      if(value > -1) {
         CFG["verbose"] = 1;              // debug implies verbose
         CFG["WLF"] = 1;                  // WL after fixing
         CFG["GFF"] = 1;                  // GF after fixing
         CFG["FIN"] = 1;                  // after final check
         LOG(INFO) << "GDC:debug sets GDC to output fixed data WLF GFF";
         if(CFG["debug"] > 0) {
            CFG["RAW"] = 1;               // data (WL,GF) before any processing
            LOG(INFO) << "GDC:debug sets GDC to output RAW data";
         }
         if(CFG["debug"] > 1) {
            CFG["WL1"] = 1;               // results of 1st diff filter on WL
            CFG["WLG"] = 1;               // WL after fixing gross slips (after fdif)
            CFG["GF1"] = 1;               // results of 1st diff filter on GF
            CFG["GFG"] = 1;               // GF after fixing gross slips (after fdif)
            LOG(INFO) << "GDC:debug sets GDC to output 1st diff fixes WL1, GF1";
            LOG(INFO) << "GDC:debug sets GDC to output gross fixes WLG, GFG";
         }
         if(CFG["debug"] > 2) {
            CFG["WLW"] = 1;               // results of window filter on WL
            CFG["GFW"] = 1;               // results of window filter on GF
            LOG(INFO) << "GDC:debug sets GDC to output window filters WLW, GFW";
         }
      }
   }
   if(label == string("verbose")) {
      if(value != 0) CFG["verbose"] = 1;
      else           CFG["verbose"] = 0;
   }

   return true;
}  // end bool gdc::setParameter(string label, double value)

//------------------------------------------------------------------------------------
// Print help page, including descriptions and current values of all
// the parameters, to the ostream.
void gdc::DisplayParameterUsage(ostream& os, string tag, bool advanced)
{
   try {
      static const unsigned name_val_width(18),adv_name_val_width(18);
      os << tag << "GPSTk Discontinuity Corrector (GDC) v." << GDCVersion
         << " configuration:" << endl;

      string name;
      //map<string,double>::const_iterator it;
      map<int,string>::const_iterator it;
      for(it=CFGlist.begin(); it != CFGlist.end(); it++) {
         name = it->second;
         if(CFGdesc[name][0] == '*')      // advanced options
            continue;  
         ostringstream stst;
         stst << name                     // label
            << "=" << CFG[name];          // value
         os << tag << " " << StringUtils::leftJustify(stst.str(),name_val_width)
            << " : " << CFGdesc[name]     // description
            << endl;
      }
      if(advanced) {
         os << tag << "  Advanced options  :\n";
         for(it=CFGlist.begin(); it != CFGlist.end(); it++) {
            name = it->second;
            if(CFGdesc[name][0] != '*')   // ordinary options
               continue;  
            ostringstream stst;
            stst << name                  // label
               << "=" << CFG[name];       // value
            os << tag << " "
               << StringUtils::leftJustify(stst.str(),adv_name_val_width)
               << " : " << CFGdesc[name].substr(2)  // description
               << endl;
         }
      }
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end void gdc::DisplayParameterUsage(ostream& os, bool advanced)

//------------------------------------------------------------------------------------
// use to define configuration members with value and descriptive string
// NB #a expands to string("a"); note that the parameters are just strings == keys.
#define setcfg(a,b,c) { CFG[#a]=b; CFGdesc[#a]=c; CFGlist[CFGindex]=#a; CFGindex++; }

// initialize with default values - only place setcfg is used
// Notes:
// Don't make grossStep too small, or noise will -> many 'slips' in tiny segments
// Note that units of step limits are wavelengths: internally wl, output meters.
void gdc::init(void)
{
   try {
      unique = 0;       // unique number for each call of DiscCorr(), for output
      CFGindex = 0;     // just a count of configuration members

      //     name, value, "description"    NB "* description" makes it 'advanced'
      setcfg(MaxGap, 10, "maximum allowed gap within a segment (points)");
      setcfg(MinPts, 10, "minimum number of good points in phase segment (points)");
      setcfg(width, 20, "* sliding window width (points)");
      // WL
      setcfg(WLgrossStep, 6.0, "WL gross slip detection threshold (WLwl)");
      setcfg(WLfineStep, 0.7, "WL fine slip detection threshold (WLwl)");
      // GF
      setcfg(GFgrossStep, 6.0, "GF gross slip detection threshold (GFwl)");
      setcfg(GFfineStep, 0.7, "GF fine slip detection threshold (GFwl)");
      // I/O
      setcfg(oswidth, 7, "output stream width (chars)");
      setcfg(osprec, 3, "output stream precision (chars)");
      setcfg(debug, -1, "level of diagnostic output, from -1(none) to 3(all)");
      setcfg(verbose, 0, "output analysis message in window filter");

      // types of labeled output
      setcfg(RAW, 0, "* output data (WL,GF) before any processing (m) [0=don't]");

      setcfg(WL1, 0, "* output results of 1st diff filter on WL (wl) [0=don't]");
      setcfg(WLG, 0, "* output WL after fixing gross slips (m) [0=don't]");
      setcfg(WLW, 0, "* output results of window filter on WL (wl) [0=don't]");
      setcfg(WLF, 0, "* output WL after fixing (m) [0=don't]");

      setcfg(GF1, 0, "* output results of 1st diff filter on GF (wl) [0=don't]");
      setcfg(GFG, 0, "* output GF after fixing gross slips (m) [0=don't]");
      setcfg(GFW, 0, "* output results of window filter on GF (wl) [0=don't]");
      setcfg(GFF, 0, "* output GF after fixing (m) [0=don't]");
      setcfg(FIN, 0, "* output WL/GF after final check [0=don't]");

      // options to fix input SatPass, and/or generate editcmds
      setcfg(doFix, 0, "apply fixes to input L1 and L2 SatPass arrays");
      setcfg(doCmds, 0, "generate editing commands");
      setcfg(doRINEX3, 1, "* editing commands use L1C L2W instead of L1 L2");

      // when rejecting data, set SatPass UserFlag using this value
      setcfg(UserFlag, 0, "* call SatPass::setUserFlag(value) for rejects");
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end void gdc::init(void)

//------------------------------------------------------------------------------------
#undef setcfg
#undef cfg

}  // end namespace gpstk
