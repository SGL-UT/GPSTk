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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file DiscCorr.cpp GPS phase discontinuity correction. Given a SatPass object
/// containing dual-frequency pseudorange and phase for an entire satellite pass,
/// and a configuration object (as defined herein), detect discontinuities in
/// the phase and, if possible, estimate their size.
/// Output is in the form of Rinex editing commands (see class RinexEditor).

//------------------------------------------------------------------------------------
// system
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
// gpstk
#include "StringUtils.hpp"
#include "Stats.hpp"
#include "PolyFit.hpp"
#include "GNSSconstants.hpp"    // PI,C_MPS,OSC_FREQ_GPS,L1_MULT_GPS,L2_MULT_GPS
#include "RobustStats.hpp"
// geomatics
#include "DiscCorr.hpp"

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
// class GDCconfiguration
//------------------------------------------------------------------------------------
// class GDCconfiguration: string giving version of gpstk Discontinuity Corrector
const string GDCconfiguration::GDCVersion = string("6.3 12/15/2015");

// class GDCconfiguration: member functions
//------------------------------------------------------------------------------------
// Set a parameter in the configuration; the input string 'cmd' is of the form
// '[--DC]<id><s><value>' : separator s is one of ':=,' and leading --DC is optional.
void GDCconfiguration::setParameter(string cmd)
{
try {
   if(cmd.empty()) return;
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

   setParameter(label, asDouble(value));
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Set a parameter in the configuration using the label and the value,
// for booleans use (T,F)=(non-zero,zero).
void GDCconfiguration::setParameter(string label, double value)
{
try {
   if(CFG.find(label) == CFG.end())
      ; // throw
   else {
      // log is not defined yet
      if(CFG["Debug"] > 0) *(p_oflog) << "GDCconfiguration::setParameter sets "
         << label << " to " << value << endl;
      CFG[label] = value;
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Print help page, including descriptions and current values of all
// the parameters, to the ostream.
void GDCconfiguration::DisplayParameterUsage(ostream& os, bool advanced)
{
try {
   os << "GPSTk Discontinuity Corrector (GDC) v." << GDCVersion
      << " configuration:"
      //<< "\n  [ pass setParameter() a string '<label><sep><value>';"
      //<< " <sep> is one of ,=: ]"
      << endl;

   map<string,double>::const_iterator it;
   for(it=CFG.begin(); it != CFG.end(); it++) {
      if(CFGdescription[it->first][0] == '*')      // advanced options
         continue;  
      ostringstream stst;
      stst << it->first                            // label
         << "=" << it->second;                     // value
      os << " " << leftJustify(stst.str(),18)
         << " : " << CFGdescription[it->first]     // description
         << endl;
   }
   if(advanced) {
   os << "   Advanced options:\n";
   for(it=CFG.begin(); it != CFG.end(); it++) {
      if(CFGdescription[it->first][0] != '*')      // ordinary options
         continue;  
      ostringstream stst;
      stst << it->first                            // label
         << "=" << it->second;                     // value
      os << " " << leftJustify(stst.str(),25)
         << " : " << CFGdescription[it->first].substr(2)  // description
         << endl;
   }
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}


//------------------------------------------------------------------------------------
#define setcfg(a,b,c) { CFG[#a]=b; CFGdescription[#a]=c; }
// initialize with default values
void GDCconfiguration::initialize(void)
{
try {
   p_oflog = &cout;

   // bookkeeping
   setcfg(ResetUnique, 0, "if non-zero, reset the unique number to zero");

   // use cfg(DT) NOT dt -  dt is part of SatPass...
   setcfg(DT, -1, "nominal timestep of data (seconds) [required - no default!]");
   setcfg(Debug, 0, "level of diagnostic output to log, from 0(none) to 7(extreme)");
   setcfg(useCA1, 0, "use L1 C/A code pseudorange (C1) ()");
   setcfg(useCA2, 0, "use L2 C/A code pseudorange (C2) ()");
   setcfg(MaxGap, 180, "maximum allowed time gap within a segment (seconds)");
   setcfg(MinPts, 13, "minimum number of good points in phase segment ()");
   setcfg(WLSigma, 1.5, "expected WL sigma (WL cycle) [NB = ~0.83*p-range noise(m)]");
   setcfg(GFVariation, 16,                    // about 300 5.4-cm wavelengths
      "expected maximum variation in GF phase in time DT (meters)");
   // output
   setcfg(OutputGPSTime, 0,
      "if 0, output Y,M,D,H,M,S else: W,SoW in edit cmds (log uses SatPass fmt)");
   setcfg(OutputDeletes, 1,
      "if non-zero, include delete commands in the output cmd list");

   // -------------------------------------------------------------------------
   // advanced options - marked with * - ordinary user will most likely NOT change
   setcfg(RawBiasLimit, 100, "* change in raw R-Ph that triggers bias reset (m)");
   // WL editing
   setcfg(WLNSigmaDelete, 2, "* delete segments with sig(WL) > this * WLSigma ()");
   setcfg(WLWindowWidth, 50,
      "* sliding window width for WL slip detection = 10+this/dt) (points)");
   setcfg(WLNWindows, 2.5,
      "* minimum segment size for WL small slip search (WLWindowWidth)");
   setcfg(WLobviousLimit, 3,
      "* minimum delta(WL) that produces an obvious slip (WLSigma)");
   setcfg(WLNSigmaStrip, 3.5, "* delete points with WL > this * computed sigma ()");
   setcfg(WLNptsOutlierStats, 200,
      "* maximum segment size to use robust outlier detection (pts)");
   setcfg(WLRobustWeightLimit, 0.35,
      "* minimum good weight in robust outlier detection (0<wt<=1)");
   // WL small slips
   setcfg(WLSlipEdge, 3,
      "* minimum separating WL slips and end of segment, else edit (pts)");
   setcfg(WLSlipSize, 0.9, "* minimum WL slip size (WL wavelengths)");
   setcfg(WLSlipExcess, 0.1,
      "* minimum amount WL slip must exceed noise (WL wavelengths)");
   setcfg(WLSlipSeparation, 2.5, "* minimum excess/noise ratio of WL slip ()");
   // GF small slips
   setcfg(GFSlipWidth, 5,
      "* minimum segment length for GF small slip detection (pts)");
   setcfg(GFSlipEdge, 3,
      "* minimum separating GF slips and end of segment, else edit (pts)");
   setcfg(GFobviousLimit, 1,
      "* minimum delta(GF) that produces an obvious slip (GFVariation)");
   setcfg(GFSlipOutlier, 5, "* minimum GF outlier magnitude/noise ratio ()");
   setcfg(GFSlipSize, 0.8, "* minimum GF slip size (5.4cm wavelengths)");
   setcfg(GFSlipStepToNoise, 0.1, "* maximum GF slip step/noise ratio ()");
   setcfg(GFSlipToStep, 3, "* minimum GF slip magnitude/step ratio ()");
   setcfg(GFSlipToNoise, 3, "* minimum GF slip magnitude/noise ratio ()");
   // GF fix
   setcfg(GFFixNpts, 15,
      "* maximum number of points on each side to fix GF slips ()");
   setcfg(GFFixDegree, 3, "* degree of polynomial used to fix GF slips ()");
   setcfg(GFFixMaxRMS, 100,
      "* limit on RMS fit residuals to fix GF slips, else delete (5.4cm)");
   setcfg(GFSkipSmall, 1,
      "* if non-zero, skip small GF slips unless there is a WL slip");

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// class Segment - used internally only.
// An object to hold information about segments = periods of continuous phase.
// Keep a linked list of these objects, subdivide whenever a discontinuity is
// detected, and join whenever one is fixed.
class Segment {
public:
      // member data
   unsigned int nbeg,nend; // array indexes of the first and last good points
                           // always maintain these so they point to good data
   unsigned int npts;      // number of good points in this Segment
   int nseg;               // segment number - used for data dumps only
   double bias1;           // bias subtracted from WLbias for WLStats - only
   Stats<double> WLStats;  // includes N,min,max,ave,sig
   double bias2;           // bias subtracted from GFP for polynomial fit - only
   PolyFit<double> PF;     // for fit to GF range
   double RMSROF;          // RMS residual of fit of polynomial (PF) to GFR
   bool WLsweep;           // WLstatSweep(this) was called, used by detectWLsmallSlips

      // member functions
   Segment(void) : nbeg(0),nend(0),npts(0),nseg(0),bias1(0.0),bias2(0.0)
      { WLStats.Reset(); WLsweep=false; }
   Segment(const Segment& s)
      { *this = s; }
   ~Segment(void) { }
   Segment& operator=(const Segment& s) {
      if(this==&s) return *this;
      nbeg = s.nbeg; nend = s.nend; npts = s.npts; nseg = s.nseg;
      bias1 = s.bias1; bias2 = s.bias2;
      WLStats = s.WLStats; PF = s.PF; RMSROF = s.RMSROF; WLsweep = s.WLsweep;
      return *this;
   }

}; // end class Segment

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// class Slip - used internally only
class Slip {
public:
   int index;           // index in arrays where this slip occurs
   long NWL,N1;         // slip fixes for WL (N1-N2) and GF (=N1)
   string msg;          // string to be output after '#' on edit cmds
   explicit Slip(int in) : index(in),NWL(0),N1(0) { }
   bool operator<(const Slip &rhs) const { return index < rhs.index; }
}; // end class Slip

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// class GDCPass - inherit SatPass and GDConfiguration - use internally only
// this object is used to implement the DC algorithm.
class GDCPass : public SatPass, public GDCconfiguration
{
public:
   static const unsigned short DETECT;           // both WL and GF
   static const unsigned short FIX;              // both WL and GF
   static const unsigned short WLDETECT;
   static const unsigned short WLFIX;
   static const unsigned short GFDETECT;
   static const unsigned short GFFIX;

   explicit GDCPass(SatPass& sp, const GDCconfiguration& gdc);

   //~GDCPass(void) { };

   /// edit obvious outliers, divide into segments using MaxGap
   /// @throw Exception
   int preprocess(void);

   /// compute linear combinations and place the result in the data arrays:
   /// L1 -> L1;                     L2 -> GFP(m)
   /// P1 -> WLB(cyc)                P2 -> -GFR(m)
   /// @throw Exception
   int linearCombinations(void);

   /// detect slips in the wide lane bias
   /// @throw Exception
   int detectWLslips(void);

   /// detect obvious slips by computing the first difference (of either WL or GFP)
   /// and looking for outliers. create new segments where there are slips
   /// which is either 'WL' or 'GF'.
   /// @throw Exception
   int detectObviousSlips(string which);

   /// compute first differences of data arrays for WL and GF gross slips detection.
   /// for WL difference the WLbias; for GF, the GFP and the residual GFP-GFR
   /// Store results in temporary array A1 and A2
   /// @throw Exception
   int firstDifferences(string which);

   /// for one segment, compute conventional statistics on the
   /// WL bias and count the number of good points
   /// @throw Exception
   void WLcomputeStats(list<Segment>::iterator& it);

   /// for one segment mark bad points that lie outside N*sigma
   /// delete segments that are too small
   /// @throw Exception
   void WLsigmaStrip(list<Segment>::iterator& it);

   /// for one segment, compute statistics on the WL bias using a
   /// 'two-paned sliding window', each pane of width 'width' good points.
   /// store the results in the parallel (to SatPass::data) arrays A1,A2.
   /// @throw Exception
   int WLstatSweep(list<Segment>::iterator& it, int width);

   /// detect slips in all segments using the results of WLstatSweep()
   /// if close to either end (< window width), just chop off the small segment
   /// when a slip is found, create a new segment
   /// also compute conventional stats for each Segment, store in Segment.WLStats
   /// @throw Exception
   int detectWLsmallSlips(void);

   /// determine if a slip has been found at index i, in segment nseg (0..)
   /// which is associated with it.
   /// conditions for a slip to be detected:
   /// 1. test must be >~ 0.67 wlwl
   /// 2. limit must be much smaller than test
   /// 3. slip must be far (>1/2 window) from either end
   /// 4. test must be at a local maximum (~ 2 window widths)
   /// 5. limit must be at a local minimum (")
   /// also, large limit (esp near end of a pass) means too much noise, and
   /// @throw Exception
   bool foundWLsmallSlip(list<Segment>::iterator& it, int i);

   /// estimate slips in the WL bias and adjust biases appropriately - ie fix WL slips
   /// also compute stats for WL for the whole pass
   /// @throw Exception
   int fixAllSlips(string which);

   /// fix the slip at the beginning of the segment pointed to by kt,
   /// which is the string 'WL' or 'GF'.
   /// @throw Exception
   void fixOneSlip(list<Segment>::iterator& kt, string which);

   /// fix the slip between segments pointed to by left and right
   /// @throw Exception
   void WLslipFix(list<Segment>::iterator& left,
                  list<Segment>::iterator& right);

   /// fit a polynomial to the GF range, and replace P2 (-gfr) with the residual
   /// gfp+fit(gfr); divide both P1(gfp) and P2(residual) by wlgf to convert to cycles
   /// also place the residual gfp+gfr(cycles) in L1
   /// @throw Exception
   int prepareGFdata(void);

   /// detect slips in the geometry-free phase
   /// @throw Exception
   int detectGFslips(void);

   /// for each segment, fit a polynomial to the gfr, then compute and store the
   /// residual of fit; at the same time, compute stats on the first difference of GF
   /// @throw Exception
   int GFphaseResiduals(list<Segment>::iterator& it);

   /// detect small slips in the geometry-free phase using its first difference
   /// compute statistics in two windows of fixed width on either side of the point
   /// of interest and use these to find slips and outliers
   /// @throw Exception
   int detectGFsmallSlips(void);

   /// determine if there is an outlier in the GF phase, using the
   /// GFP first difference and the statistics computed in detectGFsmallSlips().
   /// Criteria:
   /// 1. adjacent first differences have different signs
   /// 2. they have approximately the same magnitude
   /// 3. that magnitude is large compared to the noise in the dGFP
   /// @throw Exception
   bool foundGFoutlier(int i,int inew,Stats<double>& pastSt,Stats<double>& futureSt);

   /// determine if a small GF slip is found, using the first differenced gfp
   /// and statistics computed in detectGFsmallSlips()
   /// Criteria:
   /// 1. slip is non-trivial - at least one wavelength
   /// 2. the change in the average 1stDiff(GFP) across the slip is small
   /// 3. the slip itself is large compared to the average on either side
   /// 4. the slip itself is large compared to the noise
   /// Declare 'slips' that are very near the ends of the segment as outliers
   /// Conservatively, ignore small slips that are near the level of the noise,
   /// unless there was a WL slip at the same epoch.
   /// @throw Exception
   bool foundGFsmallSlip(int i, int nseg, int iend, int ibeg,
      deque<int>& pastIn, deque<int>& futureIn,
                         Stats<double>& pastSt, Stats<double>& futureSt);

   /// fix the slip between segments pointed to by left and right
   /// @throw Exception
   void GFslipFix(list<Segment>::iterator& left,
                  list<Segment>::iterator& right);

   /// estimate the size of the slip between segments left and right,
   /// using points from indexes nb to ne; n1 is the initial estimate of the slip
   /// called by GFslipFix()
   /// @throw Exception
   long EstimateGFslipFix(list<Segment>::iterator& left,
                          list<Segment>::iterator& right,
                          unsigned int nb, unsigned int ne, long n1);

   /// final check on consistency of WL slip fixes with GF slip detection
   /// @throw Exception
   int WLconsistencyCheck(void);

   /// last call before returning: copy edited data back into caller's SatPass,
   /// generate editing commands, and print and return the final summary.
   /// @throw Exception
   string finish(int iret, SatPass& svp, vector<string>& editCmds);

   /// create a new segment from the given one, starting at index ibeg,
   /// and insert it after the given iterator.
   /// Return an iterator pointing to the new segment. String msg is for debug output
   /// @throw Exception
   list<Segment>::iterator createSegment(list<Segment>::iterator sit,
                                         int ibeg,
                                         string msg=string());

   /// dump a list of the segments, detail dependent on level
   /// level=0 one line summary (number of segments)
   /// level=1 one per line list of segments
   /// level=2 dump all data, including (if extra) temporary arrays
   /// return the level 1 output as a string
   /// @throw Exception
   string dumpSegments(string msg, int level=2, bool extra=false);

   /// delete (set all points bad) segment it, optional message
   /// is used in debug print
   /// @throw Exception
   void deleteSegment(list<Segment>::iterator& it, string msg=string());

private:

   /// define this function so that invalid labels will throw, because
   /// this fails silently #define cfg(a) CFG[#a]     // stringize
   /// @throw Exception
   double cfg_func(string a)
   {
      if(CFGdescription[a] == string()) {
         Exception e("cfg(UNKNOWN LABEL) : " + a);
         GPSTK_THROW(e);
      }
      return CFG[a];
   }

   /// list of Segments, always in time order, of segments of
   /// continuous data within the SVPass.
   list<Segment> SegList;

   /// list of Slips found; used to generate the editing commands on output
   list<Slip> SlipList;

   /// temporary storage arrays, parallel to SatPass::data
   //vector<double> A1,A2;

   /// stats on the WL bias after editing for the entire pass
   Stats<double> WLPassStats;

   /// stats on the first difference of GF after detectObviousSlips(GF)
   Stats<double> GFPassStats;

   /// polynomial fit to the geometry-free range for the whole pass
   //07202010 not used PolyFit<double> GFPassFit;

   /// keep count of various results: slips, deletions, etc.; print to log in finish()
   map<string,int> learn;

}; // end class GDCPass

//------------------------------------------------------------------------------------
// conveniences...
#define cfg(a) cfg_func(#a)
#define log *(p_oflog)
static const int L1 = 0;
static const int L2 = 1;
static const int P1 = 2;
static const int P2 = 3;
static const int A1 = 4;
static const int A2 = 5;
vector<string> DCobstypes; // indexes into both data and this vector are L1,L2,etc...

//------------------------------------------------------------------------------------
// Return values (used by all routines within this module):
static const int GLOfailed=-6;
static const int BadInput=-5;
static const int NoData=-4;
static const int FatalProblem=-3;
static const int PrematureEnd=-2;      // NB never used
static const int Singular=-1;
static const int ReturnOK=0;

//------------------------------------------------------------------------------------
// these are used only to associate a unique number in the log file with each pass
static int GDCUnique=0;     // unique number for each call
static int GDCUniqueFix;    // unique for each (WL,GF) fix
static string GDCtag="GDC"; // begin each line of return message

//------------------------------------------------------------------------------------
// wavelength and other frequency-dependent quantities, determined early in DC()
// constants used in linear combinations
int GLOn;
double wl1,wl2,wlwl,wlgf;        // wavelengths: L1,L2,widelane,narrowlane
double wl1r,wl2r,wl1p,wl2p;      // coefficients in widelane linear combinations
double gf1r,gf2r,gf1p,gf2p;      // coefficients in geometry-free linear combinations

//------------------------------------------------------------------------------------
// Flags - constants used to mark slips, etc. using the SatPass flag:
//------------------------------------------------------------------------------------
// These are from SatPass.cpp
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
// The discontinuity corrector function
//------------------------------------------------------------------------------------
// yes you need the gpstk::
int gpstk::DiscontinuityCorrector(SatPass& svp,
                                  GDCconfiguration& gdc,
                                  vector<string>& editCmds,
                                  string& retMessage,
                                  int GLOn_in)
{
try {
   unsigned int i,j;
   int iret;

   if(gdc.getParameter("ResetUnique") != 0)
      { GDCUnique=0; gdc.setParameter("ResetUnique=0"); }
   GDCUnique++;

   //if(!retMessage.empty()) { GDCtag = retMessage; }
   retMessage = "";

   // --------------------------------------------------------------------------------
   // require obstypes L1,L2,C1/P1,C2/P2, and add two auxiliary arrays
   DCobstypes.clear();
   DCobstypes.push_back("L1");
   DCobstypes.push_back("L2");
   DCobstypes.push_back((int(gdc.getParameter("useCA1"))) == 0 ? "P1" : "C1");
   DCobstypes.push_back((int(gdc.getParameter("useCA2"))) == 0 ? "P2" : "C2");
   DCobstypes.push_back("A1");
   DCobstypes.push_back("A2");

   // --------------------------------------------------------------------------------
   // test input for (a) some data and (b) the required obs types L1,L2,C1/P1,P2
   vector<double> newdata(6);
   string found;
   try {
      newdata[L1] = svp.data(0,DCobstypes[L1]); found += " " + DCobstypes[L1];
      newdata[L2] = svp.data(0,DCobstypes[L2]); found += " " + DCobstypes[L2];
      newdata[P1] = svp.data(0,DCobstypes[P1]); found += " " + DCobstypes[P1];
      newdata[P2] = svp.data(0,DCobstypes[P2]); found += " " + DCobstypes[P2];
   }
   catch (Exception& e) { // if obs type is not found in input
      ostringstream oss;
      oss << "   Missing required obs types. Require";
      for(i=0; i<4; i++) oss << " " << DCobstypes[i];
      oss << "; found only" << found;
      retMessage = oss.str();
      return BadInput;
   }

   // --------------------------------------------------------------------------------
   // create a SatPass using DCobstypes, and fill from input
   RinexSatID sat(svp.getSat());
   SatPass nsvp(sat, svp.getDT(), DCobstypes);

   // fill the new SatPass with the input data
   nsvp.status() = svp.status();
   vector<unsigned short> lli(6),ssi(6);
   for(i=0; i<static_cast<int>(svp.size()); i++) {
      for(j=0; j<6; j++) {
         newdata[j] = j < 4 ? svp.data(i,DCobstypes[j]) : 0.0;
         lli[j] = j < 4 ? svp.LLI(i,DCobstypes[j]) : 0;
         ssi[j] = j < 4 ? svp.SSI(i,DCobstypes[j]) : 0;
      }
      // return value must be 0
      nsvp.addData(svp.time(i), DCobstypes, newdata, lli, ssi, svp.getFlag(i));
   }

   // --------------------------------------------------------------------------------
   // create a GDCPass from the input SatPass (modified) and GDC configuration
   GDCPass gp(nsvp,gdc);

   // --------------------------------------------------------------------------------
   // if the satellite is Glonass, compute the frequency channel, if necessary,
   // and define wavelengths and other constants for this satellite
   GLOn = GLOn_in;
   if(sat.system == SatelliteSystem::Glonass) {

      // only compute it if it is out of range
      if(GLOn < -7 || GLOn > 7) {
         string msg;
         // call SatPass::getGLOchannel() to get channel from data
         GLOn = 0;
         if(gp.getGLOchannel(GLOn,msg)) {
            //log << "Computed GLONASS frequency channel = " << GLOn
            //   << "\n   (" << msg << ")" << endl;
         }
         else {
            ostringstream oss;
            oss << GDCtag << " " << setw(3) << GDCUnique << " " << sat
               << " " << printTime(svp.getFirstTime(),svp.outFormat)
               << " is returning with error code: failed to find GLONASS frequency\n"
               << msg << endl;
            retMessage = oss.str();
            return GLOfailed;
         }
      }

      // GLO Frequency(Hz) L1 is 1602.0e6 + n*562.5e3 Hz = 9 * (178 + n*0.0625) MHz
      //                   L2    1246.0e6 + n*437.5e3 Hz = 7 * (178 + n*0.0625) MHz
      // Note that L1/L2 is always 9/7 for freq, 7/9 for wavelength
      static const double GLOfreq0L1=1602.0e6;
      static const double GLOdfreqL1= 562.5e3;
      static const double GLOfreq0L2=1246.0e6;
      static const double GLOdfreqL2= 437.5e3;
      static const double F1oF2 = 9.0/7.0;
      static const double F2oF1 = 7.0/9.0;

      wl1 = C_MPS/(GLOfreq0L1 + GLOn*GLOdfreqL1);
      wl2 = C_MPS/(GLOfreq0L2 + GLOn*GLOdfreqL2);
      wlwl = 1.0 / (1.0/wl1 - 1.0/wl2);
      wlgf = wl2 - wl1;

      wl1r = 1.0/(1.0+F2oF1);
      wl2r = 1.0/(1.0+F1oF2);
      wl1p = wl1/(1.0-F2oF1);
      wl2p = wl2/(1.0-F1oF2);

      gf1r = -1.0;
      gf2r = 1.0;
      gf1p = wl1;
      gf2p = -wl2;
   }
   else {                                                   // GPS satellite
      static const double CFF=C_MPS/OSC_FREQ_GPS;
      static const double wl1_GPS = CFF/L1_MULT_GPS;            // 19.0cm
      static const double wl2_GPS = CFF/L2_MULT_GPS;            // 24.4cm
      static const double wlwl_GPS = CFF/(L1_MULT_GPS-L2_MULT_GPS); // 86.2cm
      static const double wlgf_GPS = wl2_GPS - wl1_GPS;     //  5.4cm
      static const double F1oF2 = L1_MULT_GPS/L2_MULT_GPS;          // 77/60
      static const double F2oF1 = L2_MULT_GPS/L1_MULT_GPS;          // 60/77

      wl1 = wl1_GPS;
      wl2 = wl2_GPS;
      wlwl = wlwl_GPS;
      wlgf = wlgf_GPS;

      wl1r = 1.0/(1.0+F2oF1);
      wl2r = 1.0/(1.0+F1oF2);
      wl1p = wl1/(1.0-F2oF1);
      wl2p = wl2/(1.0-F1oF2);

      gf1r = -1.0;
      gf2r = 1.0;
      gf1p = wl1;
      gf2p = -wl2;
   }

   // --------------------------------------------------------------------------------
   // implement the DC algorithm using the GDCPass
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

   // --------------------------------------------------------------------------------
   // generate editing commands for deleted (flagged) data and slips,
   // use editing command (slips and deletes) to modify the original SatPass data
   // and print ending summary
   retMessage = gp.finish(iret, svp, editCmds);

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// class GDCPass member functions
//------------------------------------------------------------------------------------
GDCPass::GDCPass(SatPass& sp, const GDCconfiguration& gdc)
      : SatPass(sp.getSat(), sp.getDT(), sp.getObsTypes())
{
   int i,j;
   Status = sp.status();
   dt = sp.getDT();
   sat = sp.getSat();
   vector<string> ot = sp.getObsTypes();
   for(i=0; i<static_cast<int>(ot.size()); i++) {
      labelForIndex[i] = ot[i];
      indexForLabel[labelForIndex[i]] = i;
   }

   vector<double> vdata;
   vector<unsigned short> lli,ssi;
   for(i=0; i<static_cast<int>(sp.size()); i++) {
      vdata.clear();
      lli.clear();
      ssi.clear();
      for(j=0; j<static_cast<int>(ot.size()); j++) {
         vdata.push_back(sp.data(i,ot[j]));
         lli.push_back(sp.LLI(i,ot[j]));
         ssi.push_back(sp.SSI(i,ot[j]));
      }
      addData(sp.time(i),ot,vdata,lli,ssi,sp.getFlag(i));
   }

   *((GDCconfiguration*)this) = gdc;

   learn.clear();
}

//------------------------------------------------------------------------------------
int GDCPass::preprocess(void)
{
try {
   int ilast;
   unsigned int i,Ngood;
   double biasL1,biasL2,dbias;
   list<Segment>::iterator it;

   if(cfg(Debug) >= 2) {
      Epoch CurrentTime;
      //CurrentTime.setLocalTime();
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

   // 050109 some parameters should depend on DT
   CFG["WLWindowWidth"] = 10 + int(0.5+CFG["WLWindowWidth"]/CFG["DT"]);

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
   for(ilast=-1,i=0; i<static_cast<int>(size()); i++) {

      // ignore data the caller has marked BAD
      if(!(spdvector[i].flag & OK)) continue;

      // just in case the caller has set it to something else...
      spdvector[i].flag = OK;

         // look for obvious outliers
         // Don't do this - sometimes the pseudoranges get extreme values b/c the
         // clock is allowed to run off for long times - perfectly normal
      //if(spdvector[i].data[P1] < cfg(MinRange) ||
      //   spdvector[i].data[P1] > cfg(MaxRange) ||
      //   spdvector[i].data[P2] < cfg(MinRange) ||
      //   spdvector[i].data[P2] > cfg(MaxRange) )
      //{
      //   spdvector[i].flag = BAD;
      //   learn["points deleted: obvious outlier"]++;
      //   if(cfg(Debug) > 6)
      //      log << "Obvious outlier " << GDCUnique << " " << sat
      //         << " at " << i << " " << printTime(time(i),outFormat) << endl;
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
         if(!(spdvector[i].flag & OK)) continue;

         dbias = fabs(spdvector[i].data[P1]-wl1*spdvector[i].data[L1]-biasL1);
         if(dbias > cfg(RawBiasLimit)) {
            if(cfg(Debug) >= 2) log << "BEFresetL1 " << GDCUnique
               << " " << sat << " " << printTime(time(i),outFormat)
               << " " << fixed << setprecision(3) << biasL1
               << " " << spdvector[i].data[P1] - wl1 * spdvector[i].data[L1] << endl;
            biasL1 = spdvector[i].data[P1] - wl1 * spdvector[i].data[L1];
         }

         dbias = fabs(spdvector[i].data[P2]-wl2*spdvector[i].data[L2]-biasL2);
         if(dbias > cfg(RawBiasLimit)) {
            if(cfg(Debug) >= 2) log << "BEFresetL2 " << GDCUnique
               << " " << sat << " " << printTime(time(i),outFormat)
               << " " << fixed << setprecision(3) << biasL2
               << " " << spdvector[i].data[P2] - wl2 * spdvector[i].data[L2] << endl;
            biasL2 = spdvector[i].data[P2] - wl2 * spdvector[i].data[L2];
         }

         spdvector[i].data[A1] =
            spdvector[i].data[P1] - wl1 * spdvector[i].data[L1] - biasL1;
         spdvector[i].data[A2] =
            spdvector[i].data[P2] - wl2 * spdvector[i].data[L2] - biasL2;

      }  // end loop over points in the segment

         // delete small segments
      if(it->npts < static_cast<unsigned int>(cfg(MinPts)))
         deleteSegment(it,"insufficient data in segment");
      else
         Ngood++;
   }

   if(cfg(Debug) >= 2) dumpSegments("BEF",2,true);

   if(Ngood == 0) return NoData;
   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GDCPass::linearCombinations(void)
{
try {
   unsigned int i;
   double wlr,wlp,wlbias,gfr,gfp;
   list<Segment>::iterator it;

   // iterate over segments
   for(it=SegList.begin(); it != SegList.end(); it++) {
      it->npts = 0;                       // re-compute npts here

      // loop over points in this segment
      for(i=it->nbeg; i<=it->nend; i++) {
         if(!(spdvector[i].flag & OK)) continue;

         // narrow lane range (m)
         wlr = wl1r * spdvector[i].data[P1] + wl2r * spdvector[i].data[P2];
         // wide lane phase (m)
         wlp = wl1p * spdvector[i].data[L1] + wl2p * spdvector[i].data[L2];
         // geometry-free range (m)
         gfr =        spdvector[i].data[P1] -        spdvector[i].data[P2];
         // geometry-free phase (m)
         gfp = gf1p * spdvector[i].data[L1] + gf2p * spdvector[i].data[L2];
         // wide lane bias (cycles)
         wlbias = (wlp-wlr)/wlwl;

         // change the bias
         if(it->npts == 0) {                             // first good point
            it->bias1 = wlbias;                          // WL bias (NWL)
            it->bias2 = gfp;                             // GFP bias
         }

         // change the arrays
         spdvector[i].data[L1] = gfp + gfr;              // only used in GF
         spdvector[i].data[L2] = gfp;
         spdvector[i].data[P1] = wlbias;
         spdvector[i].data[P2] = - gfr;

         it->npts++;
      }
   }

   if(cfg(Debug) >= 2) dumpSegments("LCD");

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// detect slips in the wide lane bias
int GDCPass::detectWLslips(void)
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
      if(cfg(Debug) >= 1 && it->npts >= static_cast<unsigned int>(cfg(MinPts)))
         log << "WLSIG " << GDCUnique << " " << sat
            << " " << it->nseg
            << " " << printTime(time(it->nbeg),outFormat)
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
      if(it->npts < static_cast<unsigned int>(cfg(MinPts)))
         deleteSegment(it,"insufficient data in segment");
   }

   if(cfg(Debug) >= 4) dumpSegments("WLD");

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// detect obvious slips by computing the first difference (of either WL or GFP)
// and looking for outliers. create new segments where there are slips
// which is either 'WL' or 'GF'.
int GDCPass::detectObviousSlips(string which)
{
try {
   // TD determine from range noise // ~ 2*range noise/wl2
   const double WLobviousNWLLimit=cfg(WLobviousLimit)*cfg(WLSigma);
   const double GFobviousNWLLimit=cfg(GFobviousLimit)*cfg(GFVariation)/wlgf;
   bool outlier,nogood;
   unsigned int i,j,ibad,igood,nok;
   int iret;
   double limit,wlbias;
   list<Segment>::iterator it;

   // compute 1st differences of (WL bias, GFP-GFR) as 'which' is (WL,GF)
   iret = firstDifferences(which);
   if(iret) return iret;

   if(cfg(Debug) >= 5) dumpSegments(string("D")+which,2,true); // DWL DGF

   // scan the first differences, eliminate outliers and
   // break into segments where there are WL slips.
   limit = (which == string("WL") ? WLobviousNWLLimit : GFobviousNWLLimit);
   it = SegList.begin();
   nok = 0;
   nogood = true;
   outlier = false;
   for(i=0; i<static_cast<int>(size()); i++) {
      if(i < it->nbeg) {
         outlier = false;
         continue;
      }
      if(i > it->nend) {                  // change segments
         if(outlier) {
            if(spdvector[ibad].flag & OK) nok--;
            spdvector[ibad].flag = BAD;
            learn[string("points deleted: ") + which + string(" slip outlier")]++;
            outlier = false;
         }
         it->npts = nok;
         // update nbeg and nend
         while(it->nbeg < it->nend
            && it->nbeg < static_cast<int>(size())
            && !(spdvector[it->nbeg].flag & OK) ) it->nbeg++;
         while(it->nend > it->nbeg
            && it->nend > 0
            && !(spdvector[it->nend].flag & OK) ) it->nend--;
         it++;
         if(it == SegList.end())
            return ReturnOK;
         nok = 0;
      }

      if(!(spdvector[i].flag & OK))
         continue;
      nok++;                                   // nok = # good points in segment

      if(nogood) { igood = i; nogood=false; }  // igood is index of last good point

      if(fabs(spdvector[i].data[A1]) > limit) {// found an outlier (1st diff, cycles)
         outlier = true;
         ibad = i;                             // ibad is index of last bad point
      }
      else if(outlier) {                       // this point good, but not past one(s)
         for(unsigned int j=igood+1; j<ibad; j++) {
            if(spdvector[j].flag & OK)
               nok--;
            if(spdvector[j].flag & DETECT)
               log << "Warning - found an obvious slip, "
                  << "but marking BAD a point already marked with slip "
                  << GDCUnique << " " << sat
                  << " " << printTime(time(j),outFormat) << " " << j << endl;
            spdvector[j].flag = BAD;             // mark all points between as bad
            learn[string("points deleted: ") + which + string(" slip outlier")]++;
         }

            // create a new segment, starting at the last outlier
         it->npts = nok-2;
         // WL slip gross  OR  GF slip gross
         it = createSegment(it,ibad,which+string(" slip gross"));

            // mark it
         spdvector[ibad].flag |= (which == string("WL") ? WLDETECT : GFDETECT);

            // change the bias in the new segment
         if(which == "WL") {
            wlbias = spdvector[ibad].data[P1];
            it->bias1 = long(wlbias+(wlbias > 0 ? 0.5 : -0.5));   // WL bias (NWL)
         }
         if(which == "GF")
            it->bias2 = spdvector[ibad].data[L2];                 // GFP bias

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
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// compute first differences of data array(s) for WL and GF gross slip detection.
// for WL difference the WLbias (P1); for GF, the GFP (L2) and the GFR (P2)
// Store results in A1, and for GF put the range difference in A2
int GDCPass::firstDifferences(string which)
{
try {
   //if(A1.size() != size()) return FatalProblem;

   int i,iprev=-1;

   for(i=0; i<static_cast<int>(size()); i++) {
      // ignore bad data
      if(!(spdvector[i].flag & OK)) {
         spdvector[i].data[A1] = spdvector[i].data[A2] = 0.0;
         continue;
      }

      // compute first differences - 'change the arrays' A1 and A2
      if(which == string("WL")) {
         if(iprev == -1)
            spdvector[i].data[A1] = 0.0;
         else
            spdvector[i].data[A1] =
               (spdvector[i].data[P1] - spdvector[iprev].data[P1]);
      }
      else if(which == string("GF")) {
         if(iprev == -1)            // first difference not defined at first point
            spdvector[i].data[A1] = spdvector[i].data[A2] = 0.0;
         else {
            // compute first difference of L1 = raw residual GFP-GFR
            spdvector[i].data[A1] =
               (spdvector[i].data[L1] - spdvector[iprev].data[L1]);
            // compute first difference of L2 = GFP
            spdvector[i].data[A2] =
               (spdvector[i].data[L2] - spdvector[iprev].data[L2]);
         }
      }

      // go to next point
      iprev = i;
   }

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// for one segment, compute conventional statistics on the
// WL bias and count the number of good points
void GDCPass::WLcomputeStats(list<Segment>::iterator& it)
{
try {
   // compute WLStats
   it->WLStats.Reset();
   it->npts = 0;

   // loop over data, adding to Stats, and counting good points
   for(unsigned int i=it->nbeg; i<=it->nend; i++) {
      if(!(spdvector[i].flag & OK)) continue;
      it->WLStats.Add(spdvector[i].data[P1] - it->bias1);
      it->npts++;
   }

   // eliminate segments with too few points
   if(it->npts < static_cast<unsigned int>(cfg(MinPts)))
      deleteSegment(it,"insufficient data in segment");
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// for one segment, compute conventional statistics on the
// WL bias, remove small segments, and mark bad points that lie outside N*sigma
void GDCPass::WLsigmaStrip(list<Segment>::iterator& it)
{
try {
   bool outlier,haveslip;
   unsigned short slip;
   int slipindex;
   unsigned int i,j,k;
   double wlbias,nsigma,ave;

   // use robust stats on small segments, for big ones stick with conventional

   if(it->npts < cfg(WLNptsOutlierStats)) {   // robust
      // 'change the arrays' A1 and A2; they will be overwritten by WLstatSweep
      // but then dumped as DSCWLF...
      // use temp vectors so they can be passed to Robust::MAD and Robust::MEstimate
      double median,mad;
      vector<double> vecA1,vecA2;      // use these temp, for Robust:: calls

      // put wlbias in vecA1, but without gaps: let j index good points only from nbeg
      for(j=i=it->nbeg; i<=it->nend; i++) {
         if(!(spdvector[i].flag & OK)) continue;
         wlbias = spdvector[i].data[P1] - it->bias1;
         vecA1.push_back(wlbias);
         vecA2.push_back(0.0);
         j++;
      }

      mad = Robust::MAD(&(vecA1[0]),j-it->nbeg,median,true);
      nsigma = cfg(WLNSigmaStrip) * mad;
      ave = Robust::MEstimate(&(vecA1[0]),j-it->nbeg,median,mad,&(vecA2[0]));

      // change the array : A1 is wlbias, A2 (output) will contain the weights
      // copy temps out into A1 and A2
      for(k=0,i=it->nbeg; i<j; k++,i++) {
         spdvector[i].data[A1] = vecA1[k];
         spdvector[i].data[A2] = vecA2[k];
      }

      haveslip = false;
      for(j=i=it->nbeg; i<=it->nend; i++) {
         if(!(spdvector[i].flag & OK)) continue;

         wlbias = spdvector[i].data[P1] - it->bias1;

         if(fabs(wlbias-ave) > nsigma ||
               spdvector[j].data[A2] < cfg(WLRobustWeightLimit))
            outlier = true;
         else
            outlier = false;

         // remove points by sigma stripping
         if(outlier) {
            if(spdvector[i].flag & DETECT || i == it->nbeg) {
               haveslip = true;
               slipindex = i;        // mark
               slip = spdvector[i].flag; // save to put on first good point
            }
            spdvector[i].flag = BAD;
            learn["points deleted: WL sigma stripping"]++;
            it->npts--;
            it->WLStats.Subtract(wlbias);
         }
         else if(haveslip) {
            spdvector[i].flag = slip;
            haveslip = false;
         }

         if(cfg(Debug) >= 6) {
            log << "DSCWLR " << GDCUnique << " " << sat
            << " " << it->nseg
            << " " << printTime(time(i),outFormat)
            << fixed << setprecision(3)
            << " " << setw(3) << spdvector[i].flag
            << " " << setw(13) << spdvector[j].data[A1] // wlbias
            << " " << setw(13) << fabs(wlbias-ave)
            << " " << setw(5) << spdvector[j].data[A2]  // 0 <= weight <= 1
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
         if(!(spdvector[i].flag & OK)) continue;

         wlbias = spdvector[i].data[P1] - it->bias1;

         // remove points by sigma stripping
         if(fabs(wlbias-ave) > nsigma) { // TD add absolute limit?
            if(spdvector[i].flag & DETECT) {
               haveslip = true;
               slipindex = i;        // mark
               slip = spdvector[i].flag; // save to put on first good point
            }
            spdvector[i].flag = BAD;
            learn["points deleted: WL sigma stripping"]++;
            it->npts--;
            it->WLStats.Subtract(wlbias);
         }
         else if(haveslip) {
            spdvector[i].flag = slip;
            haveslip = false;
         }

      }  // loop over points in segment
   }

   // change nbeg, but don't change the bias
   if(haveslip) {
      it->nbeg = slipindex;
   }

   // again
   if(it->npts < static_cast<unsigned int>(cfg(MinPts)))
      deleteSegment(it,"WL sigma stripping");
   else {
      // update nbeg and nend // TD add limit 0 size()
      while(it->nbeg < it->nend && !(spdvector[it->nbeg].flag & OK)) it->nbeg++;
      while(it->nend > it->nbeg && !(spdvector[it->nend].flag & OK)) it->nend--;
   }

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// in the given segment, compute statistics on the WL bias using a
// 'two-paned sliding window', each pane of width 'width' good points.
// store the results in the parallel (to SatPass::data) arrays A1,A2.
int GDCPass::WLstatSweep(list<Segment>::iterator& it, int width)
{
try {
   unsigned int iminus,i,iplus,uwidth(width);
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
   while(futureStats.N() < uwidth && iplus <= it->nend) {
      if(spdvector[iplus].flag & OK) {                // add only good data
         futureStats.Add(spdvector[iplus].data[P1] - it->bias1);
      }
      iplus++;
   }

   // now loop over all points in the segment
   for(i=it->nbeg; i<= it->nend; i++) {
      if(!(spdvector[i].flag & OK))                      // add only good data
         continue;

      // compute test and limit
      test = 0;
      if(pastStats.N() > 0 && futureStats.N() > 0)
         test = fabs(futureStats.Average()-pastStats.Average());
      limit = ::sqrt(futureStats.Variance() + pastStats.Variance());
      // 'change the arrays' A1 and A2
      spdvector[i].data[A1] = test;
      spdvector[i].data[A2] = limit;

      wlbias = spdvector[i].data[P1] - it->bias1;        // debiased WLbias

      // dump the stats
      if(cfg(Debug) >= 6) log << "WLS " << GDCUnique
         << " " << sat << " " << it->nseg
         << " " << printTime(time(i),outFormat)
         << fixed << setprecision(3)
         << " " << setw(3) << pastStats.N()
         << " " << setw(7) << pastStats.Average()
         << " " << setw(7) << pastStats.StdDev()
         << " " << setw(3) << futureStats.N()
         << " " << setw(7) << futureStats.Average()
         << " " << setw(7) << futureStats.StdDev()
         << " " << setw(9) << spdvector[i].data[A1]
         << " " << setw(9) << spdvector[i].data[A2]
         << " " << setw(9) << wlbias
         << " " << setw(3) << i
         << endl;

      // update stats :
      // move point i from future to past, ...
      futureStats.Subtract(wlbias);
      pastStats.Add(wlbias);
      // ... and move iplus up by one (good) point, ...
      while(futureStats.N() < uwidth && iplus <= it->nend) {
         if(spdvector[iplus].flag & OK) {
            futureStats.Add(spdvector[iplus].data[P1] - it->bias1);
         }
         iplus++;
      }
      // ... and move iminus up by one good point
      while(static_cast<int>(pastStats.N()) > uwidth && iminus <= it->nend) {
         if(spdvector[iminus].flag & OK) {
            pastStats.Subtract(spdvector[iminus].data[P1] - it->bias1);
         }
         iminus++;
      }
   
   }  // end loop over i=all points in segment

   return  ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// look for slips in the WL using the results of WLstatSweep
// if slip is close to either end (< window width), just chop off the small segment
// recompute WLstats; when a slip is found, create a new segment
int GDCPass::detectWLsmallSlips(void)
{
try {
   unsigned int k,nok;
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
   unsigned int i = it->nbeg;
   nok = 0;
   unsigned int halfwidth = static_cast<unsigned int>(cfg(WLSlipEdge));
   while(i < size())
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

      if(spdvector[i].flag & OK) {
         nok++;                                 // nok = # good points in segment

         if(nok == 1) {                         // change the bias, as WLStats reset
            wlbias = spdvector[i].data[P1];
            it->bias1 = long(wlbias+(wlbias > 0 ? 0.5 : -0.5));
         }

         //  condition 3 - near ends of segment?
         if(nok < halfwidth || (it->npts - nok) < halfwidth ) {
            // failed test 3 - near ends of segment
            // consider chopping off this end of segment - large limit?
            // TD must do something here ... 
            if(cfg(Debug) >= 6) log << "too near end " << GDCUnique
               << " " << i << " " << nok << " " << it->npts-nok
               << " " << printTime(time(i),outFormat)
               << " " << spdvector[i].data[A1] << " " << spdvector[i].data[A2]
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
            spdvector[i].flag |= WLDETECT;

            // prep for next segment
            // biases remain the same in the new segment
            it->npts = k - nok;
            nok = 0;
            it->WLStats.Reset();
            wlbias = spdvector[i].data[P1]; // change the bias, as WLStats reset
            it->bias1 = long(wlbias+(wlbias > 0 ? 0.5 : -0.5));
         }

         it->WLStats.Add(spdvector[i].data[P1] - it->bias1);

      } // end if good data

      i++;

   }  // end loop over points in the pass
   it->npts = nok;

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// determine if a slip has been found at index i, in segment it
// A1 = test = fabs(futureStats.Average()-pastStats.Average()) ~ step in ave WL
// A2 = limit = sqrt(futureStats.Variance()+pastStats.Variance()) ~ noise in WL
// ALL CONDITIONs needed for a slip to be detected:
// 1. test must be > WLSlipSize (cycles)
// 2. test-limit must be > (WLSlipExcess)       // TD keep this?
// 3. slip must be far (>1/2 window) from either end - handled in detectWLsmallSlips
// 4. test must be at a local maximum within ~ window width
// 5. limit must be at a local minimum within ~ window width
// 6. (test-limit)/limit > (WLSlipSeparation = 2.5)         // this is critical test
// large limit (esp near end of a pass) means too much noise
bool GDCPass::foundWLsmallSlip(list<Segment>::iterator& it, int i)
{
try {
   const unsigned int minMaxWidth=int(cfg(WLSlipEdge));
   unsigned int j,jp,jm,pass4,pass5,Pass;
   // A1 = step = fabs(futureStats.Average() - pastStats.Average());
   // A2 = limit = ::sqrt(futureStats.Variance() + pastStats.Variance());
   // all units WL cycles
   double step = spdvector[i].data[A1];
   double lim = spdvector[i].data[A2];

   // 050109 if Debug=6, print only possible slips, if 7 print all
   bool isSlip=false, halfCycle=false;
   ostringstream oss;

   // Debug print - NB '>' is always pass, '<=' is fail....
   if(cfg(Debug) >= 6) oss << "WLslip " << GDCUnique
      << " " << sat << " " << setw(2) << it->nseg
      << " " << setw(3) << i
      << " " << printTime(time(i),outFormat)
      //<< " " << it->npts << "pt"
      << fixed << setprecision(2)
      << " step=" << step << " lim=" << lim
      << " (1)" << spdvector[i].data[A1]
      << (spdvector[i].data[A1] > cfg(WLSlipSize) ? ">" : "<=")
      << cfg(WLSlipSize)
      << " (2)" << spdvector[i].data[A1]-spdvector[i].data[A2]
      << (spdvector[i].data[A1]-spdvector[i].data[A2]>cfg(WLSlipExcess)?">":"<=")
      << cfg(WLSlipExcess); // no endl

   Pass = 0;         // 111312 count all tests passed

   // CONDITION 1
   if(step > cfg(WLSlipSize)) Pass++;
   else if(step > 0.45) halfCycle = true;
   // CONDITION 2 should be step-lim <= fraction of lim ~~ disc > frac * sigma (6!)
   if(step-lim > cfg(WLSlipExcess)) Pass++;

   // CONDITION 6 - 111312 put 6 here, its more important
   double ratio=(step-lim)/lim;
   if(cfg(Debug) >= 6) oss << " (6)" << ratio
      << (ratio > cfg(WLSlipSeparation) ? ">" : "<=") << cfg(WLSlipSeparation);
   if(ratio > cfg(WLSlipSeparation) ) Pass++;

   // CONDITIONs 4 and 5
   //         x
   //        x x
   //       x   x
   //      x     x
   //     x       x
   //    x         x
   // ------------------------
   //      jp=012345     jp==0 is i, the current point
   // do for minMaxWidth pts on each side of point; best score is pass=2*minMaxWidth
   // why is minMaxWidth used here?
   double slope=(step-lim)/(8.0*minMaxWidth);
   j = pass4 = pass5 = 0;
   jp = jm = i;
   do {
      // find next good point in future
      do { jp++; } while(jp < it->nend && !(spdvector[jp].flag & OK));
      if(jp >= it->nend) break;
         // CONDITION 4: test(A1) is a local maximum
      if(spdvector[i].data[A1]-spdvector[jp].data[A1] > j*slope) pass4++;
         // CONDITION 5: limit(A2) is a local minimum
      if(spdvector[i].data[A2]-spdvector[jp].data[A2] < -(j*slope)) pass5++;

      // find next good point in past
      do { jm--; } while(jm > it->nbeg && !(spdvector[jm].flag & OK));
      if(jm <= it->nbeg) break;
         // CONDITION 4: test(A1) is a local maximum
      if(spdvector[i].data[A1]-spdvector[jm].data[A1] > j*slope) pass4++;
         // CONDITION 5: limit(A2) is a local minimum
      if(spdvector[i].data[A2]-spdvector[jm].data[A2] < -(j*slope)) pass5++;

   } while(++j < minMaxWidth);

   // perfect = 2*minMaxWidth; allow 1 miss..
   if(pass4 >= 2*minMaxWidth-1) Pass++;
   if(cfg(Debug) >= 6) oss << " (4)" << pass4
      << (pass4 >= 2*minMaxWidth-1 ? ">" : "<=") << 2*minMaxWidth-2;
   if(pass5 >= 2*minMaxWidth-1) Pass++;
   if(cfg(Debug) >= 6) oss << " (5)" << pass5
      << (pass5 >= 2*minMaxWidth-1 ? ">" : "<=") << 2*minMaxWidth-2;

   if(Pass == 5) {
      if(cfg(Debug) >= 6) oss << " possible WL slip";
      isSlip = true;
   }

   // half-cycles - warning only - TD detect in GF, and fix
   j = 1;
   if(!halfCycle) {                          // look for half-cycle-slip > 1
      j = 2*step - int(2*step+(step > 0.0 ? 0.5 : -0.5));
      slope = ::fabs(2*step-int(2*step));    // slope is a dummy here
      if(j%2 || slope < 3*lim) halfCycle=true;
   }
   if(Pass >= 4 && halfCycle && j != 0) log << "WLslip " << GDCUnique
      << " " << sat << " " << setw(2) << it->nseg << " " << setw(3) << i
      << " " << printTime(time(i),outFormat)
      << " Warning - possible half-cycle slip of " << j << " WL half-cycles\n";

   if(cfg(Debug) >= 6) log << oss.str() << endl;
   return isSlip;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// estimate slips and adjust biases appropriately - ie fix slips - for both WL and GF
// merge all data into one segment
int GDCPass::fixAllSlips(string which)
{
try {
   // find the largest segment and start there, always combine the largest with its
   // largest neighbor
   unsigned int i,nmax;
   list<Segment>::iterator it, kt;

   // loop over all segments, erasing empty ones
   it = SegList.begin();
   while(it != SegList.end()) {
      if(it->npts <= 0)
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
         if(!(spdvector[i].flag & OK)) continue;
         WLPassStats.Add(spdvector[i].data[P1] - kt->bias1);
      }
   }
   // change the biases - reset the GFP bias so that it matches the GFR
   else {                                                         // GF
      //dumpSegments("GFFbefRebias",2,true); //temp
      bool first(true);
      for(i=kt->nbeg; i <= kt->nend; i++) {
         if(!(spdvector[i].flag & OK)) continue;
         if(first) {
            first = false;
            kt->bias2 = spdvector[i].data[L2] + spdvector[i].data[P2];
            kt->bias1 = spdvector[i].data[P1];
         }
         // change the data - recompute GFR-GFP so it has one consistent bias
         spdvector[i].data[L1] = spdvector[i].data[L2] + spdvector[i].data[P2];
      }
   }

   if(cfg(Debug) >= 3) dumpSegments(which + string("F"),2,true);   // DSCWLF DSCGFF

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by fixAllSlips
// assume there are no empty segments in the list
void GDCPass::fixOneSlip(list<Segment>::iterator& kt, string which)
{
try {
   if(kt->npts == 0) { kt++; return; }

   list<Segment>::iterator left,right;

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
      kt = SegList.end();
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
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by fixOneSlip
void GDCPass::WLslipFix(list<Segment>::iterator& left,
                        list<Segment>::iterator& right)
{
try {
   unsigned int i;

   GDCUniqueFix++;

   // full slip
   double dwl = right->bias1 + right->WLStats.Average()
         - (left->bias1 + left->WLStats.Average());
   long nwl = long(dwl + (dwl > 0 ? 0.5 : -0.5));

      // TD ? test gap size?
   //if(cfg(DT)*(right->nbeg - left->nend) > cfg(MaxGap)) break;

      // test that total variance is small
   //if(::sqrt(left->WLStats.Variance() + right->WLStats.Variance())
   //   / (left->WLStats.N() + right->WLStats.N()) < cfg(WLFixSigma)) {
   //   log << "Cannot fix WL slip (noisy) at " << right->nbeg
   //      << " " << printTime(time(right->nbeg),outFormat)
   //      << endl;
   //   break;
   //}

      // TD ? test fractional part of offset fabs
   //if(fabs(dwl-nwl) > cfg(WLFixFrac)) break;

   if(cfg(Debug) >= 6) log << "Fix " << GDCUnique << " " << sat << " " << GDCUniqueFix
      << " WL " << printTime(time(right->nbeg),outFormat)
      << " " << nwl           // put integer fix after time, all 'Fix'
      << " " << left->nseg << "-" << right->nseg
      << fixed << setprecision(2)
      << " right: " << right->bias1 << " + " << right->WLStats.Average()
      << " - left: " << left->bias1 << " + " << left->WLStats.Average()
      << " = " << dwl << " " << nwl
      << endl;

   // now do the fixing - change the data in the right segment to match left's
   for(i=right->nbeg; i<=right->nend; i++) {
      //if(!(spdvector[i].flag & OK)) continue;
      // 'change the data'
      spdvector[i].data[P1] -= nwl;                                 // WLbias
      spdvector[i].data[L2] -= nwl * wl2;                           // GFP
   }

   // fix the slips beyond the 'right' segment.
   // change the data in the GFP, and change both the data and the bias in the WL.
   // this way, WLStats is still valid, but if we change the GF bias, we will lose
   // that information before the GF slips get fixed.
   list<Segment>::iterator it = right;
   for(it++; it != SegList.end(); it++) {
      // Use real, not int, nwl b/c rounding error in a pass with many slips
      // can build up and produce errors.
      it->bias1 -= dwl;
      for(i=it->nbeg; i<=it->nend; i++) {
         spdvector[i].data[P1] -= nwl;                                 // WLbias
         spdvector[i].data[L2] -= nwl * wl2;                           // GFP
      }
   }

   // Add to slip list
   Slip newSlip(right->nbeg);
   newSlip.NWL = nwl;
   newSlip.msg = "WL";
   SlipList.push_back(newSlip);

   // mark it
   spdvector[right->nbeg].flag |= WLFIX;

   return;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// fix one slip in the geometry-free phase
// called by fixOneSlip
void GDCPass::GFslipFix(list<Segment>::iterator& left,
                        list<Segment>::iterator& right)
{
try {
      // use this number of data points on each side of slip
   const unsigned int Npts=static_cast<unsigned int>(cfg(GFFixNpts));
   unsigned int i,nb,ne,nl,nr;
   int ilast;
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
      if(spdvector[nb].flag & OK) {
         if(ilast == -1) ilast = nb;
         i++; nl++;
         Lstats.Add(spdvector[nb].data[L1] - left->bias2);
         //log << "LDATA " << nb << " " << spdvector[nb].data[L1]-left->bias2 << endl;
      }
      nb--;
   }
   ne = right->nbeg;
   i = 1;
   nr = 0;
   while(ne < right->nend && i < Npts) {
      if(spdvector[ne].flag & OK) {
         i++; nr++;
         Rstats.Add(spdvector[ne].data[L1] - right->bias2);
         //log << "RDATA " << ne << " " << spdvector[ne].data[L1]-right->bias2 <<endl;
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
   dn1 = spdvector[right->nbeg].data[L2] - right->bias2
         - (spdvector[ilast].data[L2] - left->bias2);
   n1 = long(dn1 + (dn1 > 0 ? 0.5 : -0.5));

   // estimate the slip using polynomial fits - this prints GFE data
   nadj = EstimateGFslipFix(left,right,nb,ne,n1);

   // adjust the adjustment if it is not consistent with Lstats vs Rstats
   // dn1+nadj                       - a. current best estimate
   // Rstats.Averge()-Lstats.Average - b. estimate from stats on GFR-GFP across slip
   // difference should be consistent with R/Lstats.StdDev
   // if not, replace nadj with b. - dn1
   dnGFR = Rstats.Average() - Lstats.Average();
   if(fabs(n1+nadj-dnGFR) > 10.*(Rstats.StdDev()+Lstats.StdDev())) {
      if(cfg(Debug) >= 6)
         log << "GFRadjust " << GDCUnique << " " << sat << " " << GDCUniqueFix
         << " GF " << printTime(time(right->nbeg),outFormat)
         << fixed << setprecision(2)
         << " dbias(GFR): " << dnGFR
         << " n1+nadj: " << n1+nadj;

      nadj = long(dnGFR+(dnGFR > 0 ? 0.5 : -0.5)) - n1;

      if(cfg(Debug) >= 6)
         log << " new n1+nadj: " << n1+nadj << endl;
   }

   // output result
   if(cfg(Debug) >= 6) {
      log << "Fix " << GDCUnique << " " << sat << " " << GDCUniqueFix
      << " GF " << printTime(time(right->nbeg),outFormat)
      << " " << nadj           // put integer fix after time, all 'Fix'
      << fixed << setprecision(2)
      << " dbias: " << right->bias2 - left->bias2
      << ", dn1: " << dn1 << ", n1: " << n1 << ", adj: " << nadj
      << " indexes " << nb << " " << ne << " " << nl << " " << nr
      << " segs " << left->nseg << " " << right->nseg
      << " GFR-GFP:L: "
      << Lstats.N() << " " << Lstats.Average() << " " << Lstats.StdDev()
      << "    R: "
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
   for(i=right->nbeg; i<static_cast<int>(size()); i++) {
      spdvector[i].data[L2] -= n1;                              // GFP
      spdvector[i].data[L1] -= n1;                              // GFR+GFP
   }

   // 'change the bias' for all segments in the future (although right to be deleted)
   list<Segment>::iterator kt;
   for(kt=right; kt != SegList.end(); kt++)
      kt->bias2 -= n1;

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
   spdvector[right->nbeg].flag |= GFFIX;

   return;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by GFslipFix
// estimate GF slip using polynomial fit to data around it
long GDCPass::EstimateGFslipFix(list<Segment>::iterator& left,
                               list<Segment>::iterator& right,
                               unsigned int nb, unsigned int ne, long n1)
{
try {
   bool quit;
   unsigned int i,k,in[3];
   double rof,rmsrof[3];
   PolyFit<double> PF[3];

   // start at zero and limit |nadj| to ...TD
   long nadj(0);

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
            if(!(spdvector[i].flag & OK)) continue;
            PF[in[k]].Add(
               // data
               spdvector[i].data[L2]
               // - (either               left bias - poss. slip : right bias)
                  - (i < right->nbeg ? left->bias2-n1-(nadj+k-1) : right->bias2),
               //  use a debiased count
               spdvector[i].ndt - spdvector[nb].ndt
            );
         }

         // TD check that it not singular

         // compute RMS residual of fit
         rmsrof[in[k]] = 0.0;
         for(i=nb; i<=ne; i++) {
            if(!(spdvector[i].flag & OK)) continue;
            rof =    // data minus fit
               spdvector[i].data[L2]
                  - (i < right->nbeg ? left->bias2-n1-(nadj+k-1) : right->bias2)
               - PF[in[k]].Evaluate(spdvector[i].ndt - spdvector[nb].ndt);
            rmsrof[in[k]] += rof*rof;
         }
         rmsrof[in[k]] = ::sqrt(rmsrof[in[k]]);

      }  // end loop over fits

      // the value of this is questionable, b/c with active ionosphere the real
      // GFP is NOT smooth 
      for(quit=false,k=0; k<3; k++) if(rmsrof[in[k]] > cfg(GFFixMaxRMS)) {
         log << "Warning - large RMS ROF in GF slip fix at in,k = "
            << in[k] << " " << k << " " << rmsrof[in[k]] << " abort.\n";
         quit = true;
      }
      if(quit) break;

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
   if(cfg(Debug) >= 4) {
      log << "EstimateGFslipFix dump " << endl;
      for(i=nb; i<=ne; i++) {
         if(!(spdvector[i].flag & OK)) continue;
         log << "GFE " << GDCUnique << " " << sat
            << " " << GDCUniqueFix
            << " " << printTime(time(i),outFormat)
            << " " << setw(2) << spdvector[i].flag << fixed << setprecision(3);
         for(k=0; k<3; k++) log << " " << spdvector[i].data[L2]
               - (i < right->nbeg ? left->bias2-n1-(nadj+k-1) : right->bias2)
            << " " << PF[in[k]].Evaluate(spdvector[i].ndt - spdvector[nb].ndt);
         log << " " << setw(3) << spdvector[i].ndt << endl;
      }
   }

   return nadj;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// 07202010 not used fit a polynomial to the GF range,
// change the units of -gfr(P2) and gfp(L2) to cycles of wlgf (=5.4cm)
int GDCPass::prepareGFdata(void)
{
try {
   bool first;
   int i,nbeg,nend;

   // decide on the degree of fit
   nbeg = SegList.begin()->nbeg;
   nend = SegList.begin()->nend;

   for(first=true,i=nbeg; i <= nend; i++) {
      if(!(spdvector[i].flag & OK)) continue;

      // 'change the bias' (initial bias only) in the GFP by changing units, also
      // slip fixing in the WL may have changed the values of GFP
      if(first) {
         SegList.begin()->bias2 /= wlgf;
         first = false;
      }

      // 'change the arrays'
      // change units on the GFP and the GFR
      spdvector[i].data[P2] /= wlgf;                    // -gfr (cycles of wlgf)
      spdvector[i].data[L2] /= wlgf;                    // gfp (cycles of wlgf)

      // 'change the data'
      // save in L1                          // gfp+gfr residual (cycles of wlgf)
      spdvector[i].data[L1] = spdvector[i].data[L2] - spdvector[i].data[P2];
   }

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// detect slips in the geometry-free phase
int GDCPass::detectGFslips(void)
{
try {
   unsigned int i;
   int iret;
   list<Segment>::iterator it;

   // places first difference of GF in A1 - 'change the arrays' A1
   if( (iret = detectObviousSlips("GF"))) return iret;

   GFPassStats.Reset();
   for(it=SegList.begin(); it != SegList.end(); it++) {
      // compute stats on dGF/dt
      for(i=it->nbeg; i <= it->nend; i++) {
         if(!(spdvector[i].flag & OK)) continue;

         // compute first-diff stats in meters
         // skip the first point in a segment - it is an obvious GF slip
         if(i > it->nbeg) GFPassStats.Add(spdvector[i].data[A1]*wlgf);

      }  // end loop over data in segment it

      // check number of good points
      if(it->npts < static_cast<unsigned int>(cfg(MinPts))) {
         deleteSegment(it,"insufficient data in segment");
         continue;
      }

      // fit polynomial to GFR in each segment
      // compute (1stD of) fit residual GFP-fit(GFR) -> A1 - 'change the arrays' A1
      // delete segment if polynomial is singular - probably due to too little data
      if( (iret = GFphaseResiduals(it))) {
         deleteSegment(it,"polynomial fit to GF residual failed");
         continue;
      }
   }

   // 'change the arrays'
   // at this point:
   // L1 = GFP+GFR in cycles, by prepareGFdata()
   // L2 = GFP in cycles, by prepareGFdata()
   // P1 = wlbias
   // P2 = -GFR in cycles, by prepareGFdata() 
   // A1 = GFP-(local fit) OR its 1stD, by GFphaseResiduals()
   //      (was 1stD of GFP+GFR (in L1), by firstDifferences())
   // A2 = 1stD of GFP (in L2), by firstDifferences()
   if( (iret = detectGFsmallSlips())) return iret;

   // delete all segments that are too small
   for(it=SegList.begin(); it != SegList.end(); it++) {
      if(it->npts < static_cast<unsigned int>(cfg(MinPts)))
         deleteSegment(it,"insufficient data in segment");
   }

   if(cfg(Debug) >= 4) dumpSegments("GFD",2,true);

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// for each segment, fit a polynomial to the gfr, then compute and store the
// residual of fit
int GDCPass::GFphaseResiduals(list<Segment>::iterator& it)
{
try {
   unsigned int i;
   int ndeg,nprev;
   double fit,rbias,prev,tmp;
   Stats<double> rofStats;

   // decide on the degree of fit
   ndeg = 2 + int(0.5 + (it->nend-it->nbeg+1)*cfg(DT)/3000.0);
   //if(ndeg > int(cfg(GFPolyMaxDegree))) ndeg = int(cfg(GFPolyMaxDegree));
   if(ndeg > 6) ndeg = 6;
   if(ndeg < 2) ndeg = 2;

   it->PF.Reset(ndeg);     // for fit to GF range

   for(i=it->nbeg; i <= it->nend; i++) {
      if(!(spdvector[i].flag & OK)) continue;
      it->PF.Add(spdvector[i].data[P2],spdvector[i].ndt);
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
      // skip bad data
      if(!(spdvector[i].flag & OK)) continue;
      
      fit = it->PF.Evaluate(spdvector[i].ndt);

      // all (fit, resid, gfr and gfp) are in cycles of wlgf (5.4cm)

      // compute gfp-(fit to gfr), store in A1 - 'change the arrays' A1 and A2
      // OR let's try first difference of residual of fit
      //           residual =  phase                            - fit to range
      spdvector[i].data[A1] = spdvector[i].data[L2] - it->bias2 - fit;
      if(rbias == 0.0) {
         rbias = spdvector[i].data[A1];
         nprev = spdvector[i].ndt - 1;
      }
      spdvector[i].data[A1] -= rbias;                    // debias residual for plots

         // compute stats on residual of fit
      rofStats.Add(spdvector[i].data[A1]);

      if(1) { // 1stD of residual - remember A1 has just been debiased
         tmp = spdvector[i].data[A1];
         spdvector[i].data[A1] -= prev;       // diff with previous epoch's
         // 040809 should this be divided by delta n?
         // spdvector[i].data[A1] /= (spdvector[i].ndt - nprev);
         prev = tmp;          // store residual for next point
         nprev = spdvector[i].ndt;
      }
      
   }

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// detect small slips in the geometry-free phase
// TD outliers at the beginning or end of the segment....
int GDCPass::detectGFsmallSlips(void)
{
try {
   const unsigned int width=static_cast<unsigned int>(cfg(GFSlipWidth));
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
      for(iplus=static_cast<int>(it->nbeg);
         iplus<=static_cast<int>(it->nend+width);
            iplus++)
      {
         // ignore bad points
         if(iplus <= static_cast<int>(it->nend) && !(spdvector[iplus].flag & OK))
            continue;
         if(ifirst == -1) ifirst = iplus;

         // pop the new i from the future
         if(static_cast<int>(futureIndex.size()) == width
                  || iplus > static_cast<int>(it->nend))
         {
            inew = futureIndex.front();
            futureIndex.pop_front();
            futureStats.Subtract(spdvector[inew].data[A1]);
            nok++;
         }

         // put iplus into the future deque
         if(iplus <= static_cast<int>(it->nend)) {
            futureIndex.push_back(iplus);
            futureStats.Add(spdvector[iplus].data[A1]);
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
            if(spdvector[i].flag & DETECT) {
               //log << "Warning - marking a slip point BAD in GF detect small "
               //   << GDCUnique << " " << sat
               //   << " " << printTime(time(i),outFormat) << " " << i << endl;
               spdvector[inew].flag = spdvector[i].flag;
               it->nbeg = inew;
            }
            spdvector[i].flag = BAD;
            spdvector[inew].data[A1] += spdvector[i].data[A1];
            learn["points deleted: GF outlier"]++;
            i = inew;
            nok--;
         }

         // pop last from past
         if(static_cast<int>(pastIndex.size()) == width) {
            j = pastIndex.front();
            pastIndex.pop_front();
            pastStats.Subtract(spdvector[j].data[A1]);
         }

         // move i into the past
         if(i > -1) {
            pastIndex.push_back(i);
            pastStats.Add(spdvector[i].data[A1]);
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
            spdvector[i].flag |= GFDETECT;
         }

      }  // end loop over points in the pass
      it->npts = nok;

   }  // end loop over segments

   return ReturnOK;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
bool GDCPass::foundGFoutlier(int i, int inew, 
   Stats<double>& pastSt, Stats<double>& futureSt)
{
try {
   if(i < 0 || inew < 0) return false;
   bool ok;
   double pmag = spdvector[i].data[A1]; // -pastSt.Average();
   double fmag = spdvector[inew].data[A1]; // -futureSt.Average();
   double var = ::sqrt(pastSt.Variance() + futureSt.Variance());

   ostringstream oss;
   if(cfg(Debug) >= 6) oss << "GFoutlier " << GDCUnique
      << " " << sat << " " << setw(3) << inew
      << " " << printTime(time(inew),outFormat)
      << fixed << setprecision(3)
      << " p,fave=" << fabs(pmag) << "," << fabs(fmag)
      << " var=" << var
      << " snr=" << fabs(pmag)/var <<","<< fabs(fmag)/var;

   bool isOut=true;
   for(;;) {

      // 1. signs must be opposite
      if(pmag * fmag >= 0) isOut=false;
      if(cfg(Debug) >= 6) oss << " (1)" << (isOut?"ok":"no");
      if(!isOut) break;

      // 2. magnitudes must be large compared to noise
      double noise=cfg(GFSlipOutlier)*var;
      if(fabs(pmag) < noise || fabs(fmag) < noise) isOut=false;
      if(cfg(Debug) >= 6) oss << " (2)" << fabs(pmag)/var << "or" << fabs(fmag)/var
         << (isOut?">=":"<") << cfg(GFSlipOutlier);
      if(!isOut) break;

      if(cfg(Debug) >= 6) oss << " possible GF outlier";

      break;
   }  // end for(;;)

   if(cfg(Debug) >= 6) log << oss.str() << endl;

   return isOut;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Better to find too many small ones than to miss them, since the fixing algorithm
// will most likely refuse to act on the questionable ones.
bool GDCPass::foundGFsmallSlip(int i,int nseg,int iend,int ibeg,
   deque<int>& pastIn,deque<int>& futureIn,
   Stats<double>& pastSt, Stats<double>& futureSt)
{
try {
   if(i < 0) return false;

   int j,k;
   double mag,pmag,fmag,pvar,fvar;

   pmag = fmag = pvar = fvar = 0.0;
   // note when past.N == 1, this is first good point, which has 1stD==0
   // TD be very careful when N is small
   if(pastSt.N() > 0) pmag = spdvector[i].data[A1]-pastSt.Average();
   if(futureSt.N() > 0) fmag = spdvector[i].data[A1]-futureSt.Average();
   if(pastSt.N() > 1) pvar = pastSt.Variance();
   if(futureSt.N() > 1) fvar = futureSt.Variance();
   mag = (pmag + fmag) / 2.0;

   if(cfg(Debug) >= 6) log << "GFS " << GDCUnique
      << " " << sat << " " << nseg
      << " " << printTime(time(i),outFormat)
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
      << " " << setw(7) << ::sqrt(pvar+fvar)
      << " " << setw(9) << spdvector[i].data[A1]
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
   const double RangeCheckLimit = 2*cfg(WLSigma)/(0.83*wlgf);
                                            // 2 * range noise in units of wlgf
   const double snr=fabs(pmag-fmag)/::sqrt(pvar+fvar);
                                            // slip to noise ratio

   // 050109 if Debug=6, print only possible slips, if 7 print all
   bool isSlip=true;
   ostringstream oss;

   for(;;) {
      // NB last printed test is a failure unless it says possible GF slip
      if(cfg(Debug) >= 6) oss << "GFslip " << GDCUnique
         << " " << sat << " " << nseg
         << " " << setw(3) << i
         << " " << printTime(time(i),outFormat) << fixed << setprecision(3)
         << " mag=" << mag << " snr=" << snr;      // no endl

      // 0. if WL slip here - ...? 
   
      // 1. slip must be non-trivial
      if(fabs(mag) <= minMag) isSlip=false;
      if(cfg(Debug) >= 6) oss << " (1)|" << mag << (isSlip?"|>":"|<=") << minMag;
      if(!isSlip) break;

      // 2. change in average is larger than noise
      if(snr <= STN) isSlip=false;
      if(cfg(Debug) >= 6) oss << " (2)" << snr << (isSlip?">":"<=") << STN;
      if(!isSlip) break;

      // 3. slip is large compared to change in average
      if(fabs(mag) <= MTS*fabs(pmag-fmag)) isSlip=false;
      if(cfg(Debug) >= 6) oss << " (3)" << fabs(mag/(pmag-fmag))
         << (isSlip?">":"<=") << MTS;
      if(!isSlip) break;

      // 4. magnitude is large compared to noise: a 3-sigma slip
      if(fabs(mag) <= MTN*::sqrt(pvar+fvar)) isSlip=false;
      if(cfg(Debug) >= 6) oss << " (4)" << fabs(mag)/::sqrt(pvar+fvar)
         << (isSlip?">":"<=") << MTN;
      if(!isSlip) break;

      // 5. if very close to edge, declare it an outlier
      if(static_cast<int>(pastSt.N()) < Edge
         || static_cast<int>(futureSt.N()) < Edge+1) isSlip=false;
      if(cfg(Debug) >= 6) oss << " (5)" << pastSt.N() << "," << futureSt.N()
         << (isSlip?">":"<") << Edge;
      if(!isSlip) break;

      // 6. large slips (compared to range noise): check the GFR-GFP for consistency
      if(fabs(mag) > RangeCheckLimit) {
         double magGFR,mtnGFR;
         Stats<double> pGFRmPh,fGFRmPh;
         for(j=0; j<static_cast<int>(pastIn.size()); j++) {
            if(pastIn[j] > -1) pGFRmPh.Add(spdvector[pastIn[j]].data[L1]);
            if(futureIn[j] > -1) fGFRmPh.Add(spdvector[futureIn[j]].data[L1]);
         }
         magGFR = fGFRmPh.Average() - pGFRmPh.Average();
         mtnGFR = fabs(magGFR)/::sqrt(pGFRmPh.Variance()+fGFRmPh.Variance());
         
         if(cfg(Debug) >= 6)
            oss << "; GFR-GFP has mag: " << magGFR
               << ", |dmag|: " << fabs(mag-magGFR)
               << " and mag/noise " << mtnGFR;

         if(fabs(mag-magGFR) > fabs(magGFR)) isSlip=false;
         if(cfg(Debug) >= 6) oss << " (6a)" << fabs(mag-magGFR)
            << (isSlip ? "<=" : ">") << fabs(magGFR);
         if(!isSlip) break;

         if(mtnGFR < 3) isSlip=false;
         if(cfg(Debug) >= 6) oss << " (6b)"
            << mtnGFR << "><3:can" << (isSlip ? "" : "not") << "_see_in_GFR";
         if(!isSlip) break;
      }

      // 7. small slips (compared to variations in dGF): extra careful
      // TD beware of small slips in the presence of noise >~ 1
      else { //if(fabs(mag) <= RangeCheckLimit)
         double magFD;
         Stats<double> fdStats;
         j = i-1; k=0;
         while(j >= ibeg && k < 15) {
            if(spdvector[j].flag & OK) { fdStats.Add(spdvector[j].data[A2]); k++; }
            j--;
         }
         j = i+1; k=0;
         while(j <= iend && k < 15) {
            if(spdvector[j].flag & OK) { fdStats.Add(spdvector[j].data[A2]); k++; }
            j++;
         }
         magFD = spdvector[i].data[A2] - fdStats.Average();

         if(cfg(Debug) >= 6)
            oss << " (7)1stD(GFP)mag=" << magFD
               << ",noise=" << fdStats.StdDev()
               << ",snr=" << fabs(magFD)/fdStats.StdDev()
               << ",maxima=" << fdStats.Minimum() << "," << fdStats.Maximum();
      }

      // 8. if switch is on and there is no WL slip here - skip
      if(cfg(GFSkipSmall) && !(spdvector[i].flag & WLDETECT)) {
         if(cfg(Debug) >= 6) oss << " (8)skipGFsmall";
         isSlip = false;
      }
   
      break;
   }  // end for(;;)

   if(isSlip) oss << " possible GF slip"; else oss << " not a GF slip";
   if(cfg(Debug) >= 6) log << oss.str() << endl;

   return isSlip;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// check the consistency of WL slips where a GF slip, but not a WL slip, was detected.
int GDCPass::WLconsistencyCheck(void)
{
try {
   int i,k;
   const int N=2*int(cfg(WLWindowWidth));
   double mag,absmag,factor=wl2/wlgf;

   // loop over the data and look for points with GFDETECT but not WLDETECT or WLFIX
   for(i=0; i<static_cast<int>(size()); i++) {

      if(!(spdvector[i].flag & OK)) continue;        // bad
      if(!(spdvector[i].flag & DETECT)) continue;    // no slips
      if(spdvector[i].flag & WLDETECT) continue;     // WL was detected

      // GF only slip - compute WL stats on both sides
      Stats<double> futureStats,pastStats;
      k = i;
      // fill future
      while(k < static_cast<int>(size()) && static_cast<int>(futureStats.N()) < N) {
         if(spdvector[k].flag & OK)                  // data is good
            futureStats.Add(spdvector[k].data[P1]);        // wlbias
         k++;
      }
      // fill past
      k = i-1;
      while(k >= 0 && static_cast<int>(pastStats.N()) < N) {
         if(spdvector[k].flag & OK)                  // data is good
            pastStats.Add(spdvector[k].data[P1]);          // wlbias
         k--;
      }

      // is there a WL slip here?
      // 1. magnitude of slip > 0.75
      // 2. magnitude is > stddev on both sides
      // 3. N() > 10 on both sides TD??
      mag = futureStats.Average()-pastStats.Average();
      absmag = fabs(mag);

      if(absmag > cfg(WLSlipSize) &&
         absmag > pastStats.StdDev() &&
         absmag > futureStats.StdDev()) {

         long nwl;
         nwl = long(mag + (mag > 0 ? 0.5 : -0.5));

         if(nwl == 0) continue;

         // now do the fixing - change the data to the future of the slip
         for(k=i; k<static_cast<int>(size()); k++) {
            //if(!(spdvector[i].flag & OK)) continue;
            // 'change the data'
            spdvector[k].data[P1] -= nwl;                                 // WLbias
            spdvector[k].data[L2] -= nwl * factor;                        // GFP
         }
         
         // Add to slip list
         Slip newSlip(i);
         newSlip.NWL = nwl;
         newSlip.msg = "WL";
         SlipList.push_back(newSlip);

         // mark it
         spdvector[i].flag |= (WLDETECT + WLFIX);

         if(cfg(Debug) >= 7) log << "CHECK " << GDCUnique << " " << sat
            << " " << i
            << " " << printTime(time(i),outFormat)
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
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// last call before returning:
// generate editing commands for deleted (flagged) data,
// use editing command (slips and deletes) to modify the original SatPass data
// print ending summary, and also return it as a string
string GDCPass::finish(int iret, SatPass& svp, vector<string>& editCmds)
{
try {
   bool ok;
   int i,ifirst,ilast,npts;
   long N1,N2,prevN1,prevN2;
   double slipL1,slipL2,WLbias,GFbias;
   //SatPassData spd;
   list<Slip>::iterator jt;
   string retMessage;

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
   WLbias = GFbias = slipL1 = slipL2 = 0.0;
   prevN1 = prevN2 = 0L;
   jt = SlipList.begin();
   for(i=0; i<static_cast<int>(size()); i++) {

      // is this point bad?
      if(!(spdvector[i].flag & OK)) {  // data is bad
         ok = false;
         if(i == static_cast<int>(size()) - 1) {         // but this is the last point 
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
               stst1 << printTime(time(ilast+1),"%F,%.3g");
            else
               stst1 << printTime(time(ilast+1),"%Y,%m,%d,%H,%M,%f");
            if(i-ilast > 3) stst1 << " # begin delete of "
                  << asString(i+1-ilast) << " points";
            editCmds.push_back(stst1.str());

            // -DS-<sat>,<time>
            ostringstream stst2;
            stst2 << "-DS";
            if(i-ilast > 3) stst2 << "-";
            stst2 << sat << ",";
            if(cfg(OutputGPSTime))
               stst2 << printTime(time(i-1),"%F,%.3g");
            else
               stst2 << printTime(time(i-1),"%Y,%m,%d,%H,%M,%f");
            if(i-ilast > 3) stst2 << " # end delete of "
               << asString(i+1-ilast) << " points";
            editCmds.push_back(stst2.str());
         }
         else if(i-ilast > 1 && cfg(OutputDeletes) != 0) {
            // delete a single isolated point
            ostringstream stst;
            stst << "-DS" << sat << ",";
            if(cfg(OutputGPSTime))
               stst << printTime(time(i-1),"%F,%.3g");
            else
               stst << printTime(time(i-1),"%Y,%m,%d,%H,%M,%f");
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

         // generate edit commands for slips
         if(N1-prevN1 != 0) {
            ostringstream stst;
            stst << "-BD+" << sat << ",L1,";
            if(cfg(OutputGPSTime))
               stst << printTime(time(jt->index),"%F,%.3g");
            else
               stst << printTime(time(jt->index),"%Y,%m,%d,%H,%M,%f");
            stst << "," << N1-prevN1;
            if(!jt->msg.empty()) stst << " # " << jt->msg;
            //stst << " # WL: " << jt->NWL << " N1: " << jt->N1; //temp
            editCmds.push_back(stst.str());
         }
         if(N2-prevN2 != 0) {
            ostringstream stst;
            stst << "-BD+" << sat << ",L2,";
            if(cfg(OutputGPSTime))
               stst << printTime(time(jt->index),"%F,%.3g");
            else
               stst << printTime(time(jt->index),"%Y,%m,%d,%H,%M,%f");
            stst << "," << N2-prevN2;
            if(!jt->msg.empty()) stst << " # " << jt->msg;
            editCmds.push_back(stst.str());
         }

         prevN1 = N1;
         prevN2 = N2;
         jt++;
      }

      if(i >= static_cast<int>(size())) break;

      // 'change the data' for the last time
      spdvector[i].data[L1] = svp.data(i,DCobstypes[L1]) - slipL1;
      spdvector[i].data[L2] = svp.data(i,DCobstypes[L2]) - slipL2;
      spdvector[i].data[P1] = svp.data(i,DCobstypes[P1]);
      spdvector[i].data[P2] = svp.data(i,DCobstypes[P2]);

      // compute range minus phase for output
      // do the same at the beginning ("BEG")

      // compute WL and GFP
         // narrow lane range (m)
      double wlr = wl1r * spdvector[i].data[P1] + wl2r * spdvector[i].data[P2];
         // wide lane phase (m)
      double wlp = wl1p * spdvector[i].data[L1] + wl2p * spdvector[i].data[L2];
         // geo-free range (m)
      double gfr = gf1r * spdvector[i].data[P1] + gf2r * spdvector[i].data[P2];
         // geo-free phase (m)
      double gfp = gf1p * spdvector[i].data[L1] + gf2p * spdvector[i].data[L2];
      if(i == ifirst) {
         WLbias = (wlp-wlr)/wlwl;
         GFbias = gfp;
      }
      spdvector[i].data[A1] = (wlp-wlr)/wlwl - WLbias; // wide lane bias (cyc)
      spdvector[i].data[A2] = gfp - GFbias;            // geo-free phase (m)
      //spdvector[i].data[A2] = gfr - gfp;             // geo-free range - phase (m)

   } // end loop over all data

   // first fix the segment for dump - TD? is this necessary?
   if(SegList.begin() != SegList.end()) {
      SegList.begin()->bias1 = SegList.begin()->bias2 = 0;     // not necessary..
      SegList.begin()->nbeg = 0;
      SegList.begin()->nend = static_cast<int>(size())-1;
      SegList.begin()->npts = npts;
   }
   // dump the corrected data
   if(cfg(Debug) >= 2) dumpSegments("AFT",2,true);

   // dump the edit commands to log
   for(i=0; i<static_cast<int>(editCmds.size()); i++)
      if(cfg(Debug) >= 2)
         log << "EditCmd: " << GDCUnique << " " << editCmds[i] << endl;

   // ---------------------------------------------------------
   // copy corrected data into original SatPass, without disturbing other obs types
   for(i=0; i<static_cast<int>(size()); i++) {
      svp.data(i,DCobstypes[L1]) = spdvector[i].data[L1];
      svp.data(i,DCobstypes[L2]) = spdvector[i].data[L2];
      svp.data(i,DCobstypes[P1]) = spdvector[i].data[P1];
      svp.data(i,DCobstypes[P2]) = spdvector[i].data[P2];

      // change the flag for use by SatPass
      //const unsigned short SatPass::OK  = 1; good data
      //const unsigned short SatPass::BAD = 0; used by caller and DC to mark bad data
      //const unsigned short SatPass::LL1 = 2; discontinuity on L1 only
      //const unsigned short SatPass::LL2 = 4; discontinuity on L2 only
      //const unsigned short SatPass::LL3 = 6; discontinuity on L1 and L2
      //const unsigned short GDCPass::DETECT   =   6;  // = WLDETECT | GFDETECT
      //const unsigned short GDCPass::FIX      =  24;  // = WLFIX | GFFIX
      if(spdvector[i].flag & OK) {
         if(((spdvector[i].flag & DETECT)==0 && (spdvector[i].flag & FIX)!=0)
            || i == ifirst)
            spdvector[i].flag = LL3 + OK;
         else
            spdvector[i].flag = OK;
      }
      else
         spdvector[i].flag = BAD;

      svp.LLI(i,DCobstypes[L1]) = (spdvector[i].flag & LL1) ? 1 : 0;
      svp.LLI(i,DCobstypes[L2]) = (spdvector[i].flag & LL2) ? 1 : 0;
      svp.setFlag(i,spdvector[i].flag);
   }

   // ---------------------------------------------------------
   // make up string to return
   ilast = -1;                               // last good point
   ostringstream oss;
   for(list<Segment>::iterator it=SegList.begin(); it != SegList.end(); it++) {
      i = (it->nend - it->nbeg + 1);         // total number of points
      oss << GDCtag << " " << GDCUnique << " " << sat
         << " #" << setw(2) << it->nseg << ": "
         << setw(4) << it->npts << "/" << setw(4) << i << " pts, # "
         << setw(4) << it->nbeg << "-" << setw(4) << it->nend
         << " (" << printTime(time(it->nbeg),outFormat)
         << " - " << printTime(time(it->nend),outFormat)
         << ")";
      if(it->npts > 0) {
         oss << fixed << setprecision(3)
            << " bias(wl)=" << setw(13) << it->bias1 //biaswl
            << " bias(gf)=" << setw(13) << it->bias2; //biasgf;
         if(ilast > -1) {
            ifirst = static_cast<int>(it->nbeg);
            while(ifirst <= static_cast<int>(it->nend)
                  && !(spdvector[ifirst].flag & OK)) ifirst++;
            i = spdvector[ifirst].ndt - spdvector[ilast].ndt;
            oss << " gap_segs " << setprecision(1) << setw(5)
               << cfg(DT)*i << " s = " << i << " pts.";
         }
         ilast = static_cast<int>(it->nend);
         while(ilast >= static_cast<int>(it->nbeg) && !(spdvector[ilast].flag & OK))
            ilast--;
      }
      oss << endl;
   }

   // print the channel number (GLO) and wavelengths in cm
   oss << GDCtag << " " << GDCUnique << " " << sat << fixed << setprecision(2)
      << " DT " << fixed << setprecision(2) << cfg(DT)
      << " wavelengths " << wl1*100.0 << " " << wl2*100.0
      << " " << wlwl*100.0 << " " << wlgf*100.0;
   if(sat.system == SatelliteSystem::Glonass) oss << " GLOn " << GLOn;
   oss << endl;

   // print WL & GF stats for whole pass
   if(WLPassStats.N() > 2) {
      oss << GDCtag << " " << GDCUnique << " " << sat
         << " " << fixed << setprecision(3) << WLPassStats.StdDev()
         << " WL sigma in cycles"
         << " N=" << WLPassStats.N()
         << " Min=" << WLPassStats.Minimum()
         << " Max=" << WLPassStats.Maximum()
         << " Ave=" << WLPassStats.Average();
      if(WLPassStats.StdDev() > cfg(WLSigma))
         oss << " Warning - WL sigma > input (" << cfg(WLSigma) << ")";
      oss << endl;
   }

   if(GFPassStats.N() > 2) {
      oss << GDCtag << " " << GDCUnique << " " << sat
         << " " << fixed << setprecision(3) << GFPassStats.StdDev()
         << " sigma GF variation in meters per DT"
         << " N=" << GFPassStats.N()
         << " Min=" << GFPassStats.Minimum()
         << " Max=" << GFPassStats.Maximum()
         << " Ave=" << GFPassStats.Average()
         << endl;
      oss << GDCtag << " " << GDCUnique << " " << sat
         << " " << fixed << setprecision(3)
         << (fabs(GFPassStats.Minimum()) > fabs(GFPassStats.Maximum()) ?
            fabs(GFPassStats.Minimum()) : fabs(GFPassStats.Maximum()))
         << " maximum GF variation in meters per DT"
         << " N=" << GFPassStats.N()
         << " Ave=" << GFPassStats.Average()
         << " Std=" << GFPassStats.StdDev()
         << endl;
   }

   // print 'learn' summary
   map<string,int>::const_iterator kt;
   for(kt=learn.begin(); kt != learn.end(); kt++)
      oss << GDCtag << " " << GDCUnique << " " << sat
         << " " << setw(3) << kt->second << " " << kt->first << endl;
   //if(sat.system == SatelliteSystem::Glonass)
   //   oss << GDCtag << " " << GDCUnique << " " << sat
   //      << "  " << GLOn << string(" GLONASS frequency channel") << endl;

   int n = int((lastTime-firstTime)/cfg(DT)) + 1;
   double percent = 100.0*double(ngood)/n;
   if(cfg(Debug) > 0) oss << GDCtag << "# " << setw(3) << GDCUnique << ", SAT " << sat
      << ", Pts: " << setw(4) << n << " total " << setw(4) << ngood
      << " good " << fixed << setprecision(1) << setw(5) << percent << "%"
      << ", start " << printTime(firstTime,outFormat)
      << endl;

   if(iret) {
      oss << GDCtag << " " << setw(3) << GDCUnique << " " << sat
         << " " << printTime(firstTime,outFormat)
         << " is returning with error code: "
         << (iret == NoData ? "insufficient data" :
            (iret == BadInput ? "required obs types L1,L2,P1/C1,P2 not found" :
            (iret == Singular ? "singularity in polynomial fit" :
            (iret == FatalProblem ? "time interval DT was not set" :
            (iret == PrematureEnd ? "premature end" : "unknown problem")))))
         << endl;
   }

   retMessage = oss.str();

   if(cfg(Debug) >= 2) log << "======== End GPSTK Discontinuity Corrector "
      << GDCUnique << " ================================================\n";

   stripTrailing(retMessage,'\n');
   return retMessage;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// create, delete and dump Segments
//------------------------------------------------------------------------------------
// create a new segment from the given one, starting at index ibeg,
// and insert it after the given iterator.
// Return an iterator pointing to the new segment. String msg is for debug output
list<Segment>::iterator GDCPass::createSegment(list<Segment>::iterator sit,
                                               int ibeg, string msg)
{
try {
   Segment s;
   s = *sit;
   s.nbeg = ibeg;
   s.nend = sit->nend;
   sit->nend = ibeg-1;

   // 'trim' beg and end indexes
   while(s.nend > s.nbeg && !(spdvector[s.nend].flag & OK)) s.nend--;
   while(sit->nend > sit->nbeg && !(spdvector[sit->nend].flag & OK)) sit->nend--;

   // recompute npts // TD is this done somewhere else?
   unsigned int i;
   s.npts = sit->npts = 0;
   for(i=s.nbeg; i<=s.nend; i++)
      if(spdvector[i].flag & OK) s.npts++;
   for(i=sit->nbeg; i<=sit->nend; i++)
      if(spdvector[i].flag & OK) sit->npts++;

   // get the segment number right
   s.nseg++;
   list<Segment>::iterator skt=sit;
   for(skt++; skt != SegList.end(); skt++) skt->nseg++;

   if(cfg(Debug) >= 6)
      log << "SEG " << GDCUnique << " " << sat
         << " " << msg
         << " " << printTime(time(ibeg),outFormat)
         << " " << s.nbeg << " - " << s.nend
         << " biases " << fixed << setprecision(3) << s.bias1 << " " << s.bias2
         << endl;
 
   learn["breaks found: " + msg]++;

   return SegList.insert(++sit,s); // insert puts s before ++sit
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// dump a list of the segments
// level=0 one line summary (number of segments)
// level=1 one per line list of segments
// level=2 dump all data, including (if extra) temporary arrays
// return level 1 output as string
string GDCPass::dumpSegments(string label, int level, bool extra)
{
try {
   unsigned int i,ifirst;
   int ilast;
   list<Segment>::iterator it;
   string msg;
   ostringstream oss;

      // summary of SegList
   oss << label << " " << GDCUnique
      << " list of Segments (" << SegList.size() << "):"
      << endl;

   if(level < 1) { msg = oss.str(); log << msg; return msg; }

      // one line per segment
   ilast = -1;                               // last good point
   for(it=SegList.begin(); it != SegList.end(); it++) {
      i = (it->nend - it->nbeg + 1);         // total number of points

      oss << label << " " << GDCUnique << " " << sat
         << " #" << setw(2) << it->nseg << ": "
         << setw(4) << it->npts << "/" << setw(4) << i << " pts, # "
         << setw(4) << it->nbeg << "-" << setw(4) << it->nend
         << " (" << printTime(time(it->nbeg),outFormat)
         << " - " << printTime(time(it->nend),outFormat)
         << ")";

      if(it->npts > 0) {
         oss << fixed << setprecision(3)
            << " bias(wl)=" << setw(13) << it->bias1 //biaswl
            << " bias(gf)=" << setw(13) << it->bias2; //biasgf;
         if(ilast > -1) {
            ifirst = it->nbeg;
            while(ifirst <= it->nend && !(spdvector[ifirst].flag & OK)) ifirst++;
            i = spdvector[ifirst].ndt - spdvector[ilast].ndt;
            oss << " Gap " << setprecision(1) << setw(5)
               << cfg(DT)*i << " s = " << i << " pts.";
         }
         ilast = it->nend;
         while(ilast >= static_cast<int>(it->nbeg) && !(spdvector[ilast].flag & OK))
            ilast--;
      }

      oss << endl;
   }

   if(level < 2) { msg = oss.str(); log << msg; return msg; }

      // dump the data
   for(it=SegList.begin(); it != SegList.end(); it++) {
      for(i=it->nbeg; i<=it->nend; i++) {

         oss << "DSC" << label << " " << GDCUnique << " " << sat << " " << it->nseg
            << " " << printTime(time(i),outFormat)
            << " " << setw(3) << spdvector[i].flag
            << fixed << setprecision(3)
            << " " << setw(13) << spdvector[i].data[L1] - it->bias2 //biasgf  //temp
            << " " << setw(13) << spdvector[i].data[L2] - it->bias2 //biasgf
            << " " << setw(13) << spdvector[i].data[P1] - it->bias1 //biaswl
            << " " << setw(13) << spdvector[i].data[P2];
         if(extra) oss
            << " " << setw(13) << spdvector[i].data[A1]
            << " " << setw(13) << spdvector[i].data[A2];
         oss << " " << setw(4) << i;
         if(i == it->nbeg) oss
            << " " << setw(13) << it->bias1 //biaswl
            << " " << setw(13) << it->bias2; //biasgf;
         oss << endl;
      }
   }

   msg = oss.str();
   log << msg;
   return msg;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void GDCPass::deleteSegment(list<Segment>::iterator& it, string msg)
{
try {
   unsigned int i;

   if(cfg(Debug) >= 6) log << "Delete segment "
      << GDCUnique << " " << sat << " " << it->nseg
      << " pts " << it->npts
      << " indexes " << it->nbeg << " - " << it->nend
      << " start " << printTime(firstTime,outFormat)
      << " : " << msg
      << endl;

   it->npts = 0;
   for(i=it->nbeg; i<=it->nend; i++) if(spdvector[i].flag & OK) {
      // count these : learn
      learn["points deleted: " + msg]++;
      spdvector[i].flag = BAD;
   }

   learn["segments deleted: " + msg]++;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) {
   Exception E("std except: "+string(e.what())); GPSTK_THROW(E);
}
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
