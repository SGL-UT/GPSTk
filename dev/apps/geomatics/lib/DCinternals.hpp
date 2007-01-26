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
 * @file DCinternals.hpp
 * Class information used internally in DiscCorr.cpp
 */

//------------------------------------------------------------------------------------
// constants used in linear combinations
const double CFF=C_GPS_M/OSC_FREQ;
const double F1=L1_MULT;   // 154.0;
const double F2=L2_MULT;   // 120.0;
   // wavelengths
const double wl1=CFF/F1;                        // 19.0cm
const double wl2=CFF/F2;                        // 24.4cm
const double wlwl=CFF/(F1-F2);                  // 86.2cm, the widelane wavelength
const double wl21=CFF*(1.0/F2 - 1.0/F1);        // 5.4cm, the 'GF' wavelength
   // for widelane R & Ph
const double wl1r=F1/(F1+F2);
const double wl2r=F2/(F1+F2);
const double wl1p=wl1*F1/(F1-F2);
const double wl2p=-wl2*F2/(F1-F2);
   // for geometry-free R and Ph
const double gf1r=-1;
const double gf2r=1;
const double gf1p=wl1;
const double gf2p=-wl2;

//------------------------------------------------------------------------------------
// Return values (used by all routines within this module):
const int NoData=-4;
const int FatalProblem=-3;
//const int PrematureEnd=-2;
const int Singular=-1;
const int ReturnOK=0;
// NoData:
//    preprocess
//    fixAllSlips       segment list is empty
// FatalProblem:
//    preprocess        DT is not set
//    firstDifferences  A1.size is wrong
// Singular:
//    prepareGFdata     polynomial fit to GF range failed
//    no - delete segment instead GFphaseResiduals  polynomial fit to GF range failed

//------------------------------------------------------------------------------------
// Segment - used internally only.
// An object to hold information about segments = periods of continuous phase.
// Keep a linked list of these objects, subdivide whenever a discontinuity is
// detected, and join whenever one is fixed.
class Segment {
public:
      // member data
   int nbeg,nend;          // array indexes of the first and last good points
                           // always maintain these so they point to good data
   int npts;               // number of good points in this Segment
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
class GDCPass : public SatPass, public GDCconfiguration
{
public:
   static const unsigned short DETECT;           // both WL and GF
   static const unsigned short FIX;              // both WL and GF
   static const unsigned short WLDETECT;
   static const unsigned short WLFIX;
   static const unsigned short GFDETECT;
   static const unsigned short GFFIX;

   explicit GDCPass(const SatPass& sp, const GDCconfiguration& gdc)
      : SatPass(sp.getSat(), sp.getDT())
      { *((SatPass*)this) = sp;  *((GDCconfiguration*)this) = gdc; }

   //~GDCPass(void) { };

   /// initialize the pass - just clear the 'learn' map and create temporary arrays
   void initialize(void) throw()
      { learn.clear(); A1.resize(data.size(),0.0); A2.resize(data.size(),0.0); }

   /// edit obvious outliers, divide into segments using MaxGap
   int preprocess(void) throw(Exception);

   /// compute linear combinations and place the result in the data arrays:
   /// L1 -> L1;                     L2 -> GFP(m)
   /// P1 -> WLB(cyc)                P2 -> -GFR(m)
   int linearCombinations(void) throw(Exception);

   /// detect slips in the wide lane bias
   int detectWLslips(void) throw(Exception);

   /// detect obvious slips by computing the first difference (of either WL or GFP)
   /// and looking for outliers. create new segments where there are slips
   /// which is either 'WL' or 'GF'.
   int detectObviousSlips(string which) throw(Exception);

   /// compute first differences of data arrays for WL and GF gross slips detection.
   /// for WL difference the WLbias; for GF, the GFP and the residual GFP-GFR
   /// Store results in temporary array A1 and A2
   int firstDifferences(string which) throw(Exception);

   /// for one segment, compute conventional statistics on the
   /// WL bias and count the number of good points
   void WLcomputeStats(list<Segment>::iterator& it) throw(Exception);

   /// for one segment mark bad points that lie outside N*sigma
   /// delete segments that are too small
   void WLsigmaStrip(list<Segment>::iterator& it) throw(Exception);

   /// for one segment, compute statistics on the WL bias using a
   /// 'two-paned sliding window', each pane of width 'width' good points.
   /// store the results in the parallel (to SatPass::data) arrays A1,A2.
   int WLstatSweep(list<Segment>::iterator& it, int width) throw(Exception);

   /// detect slips in all segments using the results of WLstatSweep()
   /// if close to either end (< window width), just chop off the small segment
   /// when a slip is found, create a new segment
   /// also compute conventional stats for each Segment, store in Segment.WLStats
   int detectWLsmallSlips(void) throw(Exception);

   /// determine if a slip has been found at index i, in segment nseg (0..)
   /// which is associated with it.
   /// conditions for a slip to be detected:
   /// 1. test must be >~ 0.67 wlwl
   /// 2. limit must be much smaller than test
   /// 3. slip must be far (>1/2 window) from either end
   /// 4. test must be at a local maximum (~ 2 window widths)
   /// 5. limit must be at a local minimum (")
   /// also, large limit (esp near end of a pass) means too much noise, and
   bool foundWLsmallSlip(list<Segment>::iterator& it, int i) throw(Exception);

   /// estimate slips in the WL bias and adjust biases appropriately - ie fix WL slips
   /// also compute stats for WL for the whole pass
   int fixAllSlips(string which) throw(Exception);

   /// fix the slip at the beginning of the segment pointed to by kt,
   /// which is the string 'WL' or 'GF'.
   void fixOneSlip(list<Segment>::iterator& kt, string which) throw(Exception);

   /// fix the slip between segments pointed to by left and right
   void WLslipFix(list<Segment>::iterator& left,
                  list<Segment>::iterator& right)
      throw(Exception);

   /// fit a polynomial to the GF range, and replace P2 (-gfr) with the residual
   /// gfp+fit(gfr); divide both P1(gfp) and P2(residual) by wl21 to convert to cycles
   /// also place the residual gfp+gfr(cycles) in L1
   int prepareGFdata(void) throw(Exception);

   /// detect slips in the geometry-free phase
   int detectGFslips(void) throw(Exception);

   /// for each segment, fit a polynomial to the gfr, then compute and store the
   /// residual of fit; at the same time, compute stats on the first difference of GF
   int GFphaseResiduals(list<Segment>::iterator& it) throw(Exception);

   /// detect small slips in the geometry-free phase using its first difference
   /// compute statistics in two windows of fixed width on either side of the point
   /// of interest and use these to find slips and outliers
   int detectGFsmallSlips(void) throw(Exception);

   /// determine if there is an outlier in the GF phase, using the
   /// GFP first difference and the statistics computed in detectGFsmallSlips().
   /// Criteria:
   /// 1. adjacent first differences have different signs
   /// 2. they have approximately the same magnitude
   /// 3. that magnitude is large compared to the noise in the dGFP
   bool foundGFoutlier(int i,int inew,Stats<double>& pastSt,Stats<double>& futureSt)
      throw(Exception);

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
   bool foundGFsmallSlip(int i, int nseg, int iend, int ibeg,
      deque<int>& pastIn, deque<int>& futureIn,
      Stats<double>& pastSt, Stats<double>& futureSt)
      throw(Exception);

   /// fix the slip between segments pointed to by left and right
   void GFslipFix(list<Segment>::iterator& left,
                  list<Segment>::iterator& right)
      throw(Exception);

   /// estimate the size of the slip between segments left and right,
   /// using points from indexes nb to ne; n1 is the initial estimate of the slip
   /// called by GFslipFix()
   long EstimateGFslipFix(list<Segment>::iterator& left,
                          list<Segment>::iterator& right,
                          int nb, int ne, long n1)
      throw(Exception);

   /// final check on consistency of WL slip fixes with GF slip detection
   int WLconsistencyCheck(void) throw(Exception);

   /// last call before returning: copy edited data back into caller's SatPass,
   /// generate editing commands, and print final summary.
   void finish(int iret, SatPass& svp, vector<string>& editCmds) throw(Exception);

   /// create a new segment from the given one, starting at index ibeg,
   /// and insert it after the given iterator.
   /// Return an iterator pointing to the new segment. String msg is for debug output
   list<Segment>::iterator createSegment(list<Segment>::iterator sit,
                                         int ibeg,
                                         string msg=string())
      throw(Exception);

   /// dump a list of the segments, detail dependent on level
   /// level=0 one line summary (number of segments)
   /// level=1 one per line list of segments
   /// level=2 dump all data, including (if extra) temporary arrays
   void dumpSegments(string msg, int level=2, bool extra=false)
      throw(Exception);

   /// delete (set all points bad) segment it, optional message
   /// is used in debug print
   void deleteSegment(list<Segment>::iterator& it, string msg=string())
      throw(Exception);

   /// resize the temporary arrays to zero
   void clearTempArrays(void) { A1.clear(); A2.clear(); }

private:

   /// define this function so that invalid labels will throw, because
   /// this fails silently #define cfg(a) CFG[#a]     // stringize
	double cfg_func(string a) throw(Exception)
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
   vector<double> A1,A2;

   /// stats on the WL bias after editing for the entire pass
   Stats<double> WLPassStats;

   /// stats on the first difference of GF after detectObviousSlips(GF)
   Stats<double> GFPassStats;

   /// polynomial fit to the geometry-free range for the whole pass
   PolyFit<double> GFPassFit;

   /// keep count of various results: slips, deletions, etc.; print to log in finish()
   map<string,int> learn;

}; // end class GDCPass
