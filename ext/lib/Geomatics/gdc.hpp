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

/// @file gdc.hpp GPSTk Discontinuity Corrector
/// GPS phase discontinuity correction. Given a SatPass object
/// containing dual-frequency pseudorange and phase for an entire satellite pass,
/// detect discontinuities in the phase and, if possible, estimate their size.
/// Output is a list of Rinex editing commands (see EditRinex or class RinexEditor).

#ifndef GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE
#define GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "Exception.hpp"
#include "Epoch.hpp"
#include "RinexSatID.hpp"
#include "SatPass.hpp"
#include "FirstDiffFilter.hpp"
#include "FDiffFilter.hpp"
#include "WindowFilter.hpp"

namespace gpstk {

//---------------------------------------------------------------------------
/// Class Arc encapsulates information for both break points in the data, due to
/// gaps, slips and the initial point, and also for the segment of the data from
/// this break to the next break. It includes start index, number of good and
/// total number of points, stats and a mark giving type of segment and history
/// ("GFslip", "outlier", "bad", "WLslipfixed", etc).
/// There will always be an Arc with index==0 and type BEG to begin the data.
/// Note that the only thing connecting the Arc to the data is the index; npts and
/// ngood are computed using the map<index,Arc> Arcs and recomputeArcs()
class Arc {
private:
   /// helper routine to initialize markStr map
   static std::map<unsigned, std::string> create_mark_string_map(void) {
      std::map<unsigned, std::string> m;
      m[BEG]    = std::string("Begin");
      m[WLSLIP] = std::string("WLslip");
      m[GFSLIP] = std::string("GFslip");
      m[WLFIX]  = std::string("WLfix");
      m[GFFIX]  = std::string("GFfix");
      m[WLMARK] = std::string("WLmark");
      m[GFMARK] = std::string("GFmark");
      m[REJ]    = std::string("Reject");
      return m;
   }

public:
   // NB cannot use markStr[...] since its const -- use iterators
   /// strings describing values used for Arc::mark
   static const std::map<unsigned, std::string> markStr;

   /// mark bitmap values for characterizing Arc: BEG, slips, fixes, etc; see init()
   static const unsigned BEG;    ///<   1 beginning of data (beg-of-pass or after gap)
   static const unsigned WLSLIP; ///<   2 slip found on WL
   static const unsigned GFSLIP; ///<   4 slip found on GF
   static const unsigned WLFIX;  ///<   8 slip fixed on WL
   static const unsigned GFFIX;  ///<  16 slip fixed on GF
   static const unsigned GFMARK; ///<  32 slip suspected but not found on GF
   static const unsigned WLMARK; ///<  64 slip suspected but not found on WL
   static const unsigned REJ;    ///< 128 segment of bad data - returnMessage() only

   // -------------------------------------------------------------------
   /// little class used to store information about one data array within this Arc
   class Arcinfo {
   public:
      /// constructor
      Arcinfo() : Nslip(0), step(0.0), n(0), ave(0.0), sig(0.0) { }

      // member data
      double step;   ///< for slips, estimate of slip magnitude (data units).
                     ///< does not accumulate across the data - for this Acc only
                     ///< is only the latest filter estimate (gross OR fine)
      double sigma;  ///< for slips, RSS future and past sigma on the data
      // TD half-cycles?
      int Nslip;     ///< net slip in wavelengths = accumulated integerized step
                     ///< Thus Nslip always is the total slip, while step has only
                     ///< the latest estimate, and is used to fix the data (with int).
      int n;         ///< number of points in stats(ave,sig) - may be > npts in Arc
      double ave;    ///< average value of the data in the Arc (data units)
      double sig;    ///< std dev or other sig of all the data in the Arc (data units)
   }; // end class Arcinfo

   // -------------------------------------------------------------------
   // member data

   // these are used more for "break" information
   unsigned mark;    ///< bitmap identifying actions that created or modified Arc
   int index;        ///< index in data arrays of beginning ("break")
   unsigned npts;    ///< number of points in the segment -> last index
   unsigned ngood;   ///< number of good data points in the segment

   /// Arcinfo for each datatype
   Arcinfo WLinfo, GFinfo;       // others? array? IF?

   std::string message;  ///< readable description of whatever

   // -------------------------------------------------------------------
   // member functions

   /// empty c'tor
   Arc(void) { init(); }

   /// c'tor with minimum info
   Arc(int ind, unsigned n, unsigned ng, unsigned m)
   {
      init();
      mark = m;
      index = ind;
      npts = n;
      ngood = ng;
   }

   /// initialize (clear or empty) an Arc
   void init(void)
   {
      mark = 0;
      index = -1;
      npts = ngood = 0;
      WLinfo = Arcinfo();
      GFinfo = Arcinfo();
      message = std::string();
   }

   /// create an Arc from a FilterHit (filters returns a vector of FilterHit)
   Arc(const FilterHit<double>& seg) {
      init();
      index = seg.index;
      if(seg.type == FilterHit<double>::BOD) mark = BEG; // NB BEG != FilterHit::BOD
      else mark = 0;
      npts = seg.npts;
      ngood = seg.ngood;
   }

   /// copy constructor
   Arc(const Arc& right) {
      init();
      *this = right;
   }

   /// operator=
   Arc& operator=(const Arc& right) {
      index = right.index;
      npts = right.npts;
      ngood = right.ngood;
      mark = right.mark;
      message = right.message;
      WLinfo = right.WLinfo;
      GFinfo = right.GFinfo;
      return *this;
   }

   /// convert one mark into a readable string
   static std::string markString(const unsigned mark)
   {
      std::map<unsigned, std::string>::const_iterator it;
      it = markStr.find(mark);
      if(it != markStr.end()) return it->second;
      else return std::string("Unknown");
   }

   /// convert all marks in Arc.mark into a readable string
   /// e.g. Beg   OR  WLslip/GFslip/WLfix/GFfix
   std::string markString(void) const
   {
      std::string msg;
      std::map<unsigned, std::string>::const_iterator it;
      //  NB don't use markStr[it->first]; b/c its const
      for(it = markStr.begin(); it != markStr.end(); ++it)
         if(mark & it->first) msg += std::string(msg.empty() ? "":"/") + it->second;
      return msg;
   }

   /// string giving brief summary of break info
   /// e.g. WLslip/GFslip/WLfix 445 pts 438 good
   std::string brkString(void) const
   {
      std::ostringstream os;
      os << markString() << " good/tot=" << ngood << "/" << npts << "="
            << std::fixed << std::setprecision(2) << 100.*ngood/double(npts) << "%";
      return os.str();
   }

   /// generate a string giving more extensive summary of the Arc
   /// e.g. WLslip/GFslip/WLfix/GFfix good/tot=438/445=98.43%
   ///                   dWL=46.5436wl NoAn dGF=-46.3682wl NoAn
   std::string asString(const int prec=3) const
   {
      std::ostringstream os;
      os << std::fixed << std::setprecision(prec);
      os << brkString();

      if(markString() != "BEG" && markString() != "REJ")
         os << " NWL=" << WLinfo.Nslip << " dWL=" << WLinfo.step << "wl";
      else
         os << " WL";
      if(WLinfo.n > 0)
         os << " " << WLinfo.n << " " << WLinfo.ave << " +- " << WLinfo.sig;
      else
         os << " NoAn";

      if(markString() != "BEG" && markString() != "REJ")
         os << " NGF=" << GFinfo.Nslip << " dGF=" << GFinfo.step << "wl";
      else
         os << " GF";
      if(GFinfo.n > 0)
         os << " " << GFinfo.n << " " << GFinfo.ave << " +- " << GFinfo.sig;
      else
         os << " NoAn";

      return os.str();
   }

}; // end class Arc

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/// Class gdc encapsulates configuration and processing for cycle slip detection
/// and fixing using class SatPass.
class gdc {
public:
   /// constructor; this sets a full default set of parameters.
   gdc(void) { init(); }

   /// destructor
   ~gdc(void) { }

   /// Set a parameter in the configuration; the input string 'cmd'
   /// is of the form '[--DC]<id><s><value>' where the separator s is
   /// one of (:=,) and leading '-','--', or '--DC' are optional.
   /// @param cmd string version of assigning a parameter, e.g. 'debug=0'
   /// @return true if successful, otherwise label not valid, or parsing error
   /// @throw Exception
   bool setParameter(std::string cmd);

   /// Set a parameter in the configuration using the label and the value,
   /// for booleans use (T,F)=(non-zero,zero).
   /// @param label parameter label - cf. DisplayParameterUsage() for list of labels
   /// @param value parameter value to be assigned (0 for false)
   /// @return true if successful, otherwise the label is not a valid parameter
   /// @throw Exception
   bool setParameter(std::string label, double value);

   /// Get the parameter in the configuration corresponding to label
   /// @param label parameter label for which value is returned
   /// @return value of the 'label' parameter
   double getParameter(std::string label) throw() { return CFG[label]; }

   /// Print help page, including descriptions and current values of all
   /// the parameters, to the ostream.
   /// @param os ostream to which to write
   /// @param tag string to put at the beginning of each line of output
   /// @param advanced if true, also print advanced parameters.
   /// @throw Exception
   void DisplayParameterUsage(std::ostream& os,
                              std::string tag=std::string(),
                              bool advanced=false);

   /// define this function so that invalid labels will throw, because
   /// this fails silently #define cfg(a) CFG[#a]     // stringize
   /// @throw Exception
   double cfg_func(std::string a)
   {
      if(CFGdesc[a] == std::string()) {
         Exception e("cfg(UNKNOWN LABEL) : " + a);
         GPSTK_THROW(e);
      }
      return CFG[a];
   }

   /// get the unique number; note that it is incremented at start of Disc..Corr()
   int getUniqueNumber(void) throw() { return unique; }

   /// Allow caller to force unique number, before call to DiscontinuityCorrector(),
   /// which will immediately increment it.
   /// @param n integer force unique number in output to be this+1
   void ForceUniqueNumber(int n) throw() { unique = n; }

   //---------------------------------------------------------------------------
   /// GPSTK Discontinuity Corrector (GDC).
   /// Find, and fix if possible, discontinuities in the GPS or GLONASS carrier phase
   /// data, given dual-frequency pseudorange and phase data for a satellite pass.
   ///
   /// There are two versions, one accepts the data in parallel vectors, the other
   /// uses class SatPass, converts the data to vectors and calls the first version.
   ///
   /// Input is the SatPass object, or a set of vectors, that hold the data.
   /// Part of the input is a flags array, which is defined in SatPass (ok=1, bad=0);
   /// a similarly defined array is required in the array-input version.
   ///
   /// Glonass satellites require a frequency channel integer; the caller may pass
   ///  this in, or let the GDC compute it from the data - if it fails it returns -9.
   ///
   /// This call assumes that the GDC has already been configured.
   /// See DisplayParameterUsage() for list of cfg.
   ///
   /// The results of the process can be found in the flags array, with its values
   /// defined by static constants OK, SLIP, etc.
   /// NB on output OK=0, the OPPOSITE of input/SatPass where OK/BAD = 1/0.
   /// 
   /// Other output depends on the two configuration parameters doFix and doCmds.
   /// If doFix is on (non-zero) then the SatPass is modified for slips, outliers and
   /// bad data. Note that the GDC never splits a SatPass in two, even if rejecting
   /// data has created a large gap; this is a function only the caller can apply and
   /// so must be done after this call.
   ///
   /// If doCmds is on, the call returns a vector of strings which are editing
   /// commands for EditRinex or tk-RinEdit; they are the "--IF name" form which is
   /// accepted by both (tk-RinEdit only takes this form).
   ///
   /// Also, the L1 and L2 arrays in the input SatPass are corrected.
   /// The routine will flag bad points in the input data using the values defined in
   /// this class, including OK, BAD, WLOUTLIER, GFOUTLIER, Arc::WLSLIP, Arc::GFSLIP
   /// Glonass satellites require a frequency channel integer; the caller may pass
   /// this in, or let the GDC compute it from the data - if this fails it returns -9.
   ///
   /// Output data       filter output    : use setParameter(WLF,1)
   ///        RAW                data as read from SatPass in m
   ///                   WL1     results of first difference filter of WL in wl
   ///                   WLW     results of window filter of WL in wl
   ///        WLF                data with WL slips fixed and outliers removed in m
   ///                   GF1     results of first difference filter of GF in wl
   ///                   GFW     results of window filter of GF in wl
   ///        GFF                data with GF slips fixed and outliers removed in m
   ///        FIN                data after final check
   ///
   /// @param SP       SatPass object containing the input data.
   /// @param retMsg   string summary of results: see 'GDC' in output
   /// @param cmds     vector of strings giving editing commands for RINEX editor.
   /// @param GLOn     GLONASS frequency channel (-7<=n<7), -99 (default) : UNKNOWN
   ///                 if -99, try to compute from SatPass.
   /// @return 0 for success, otherwise return an Error code; defined as follows.
   ///        -4 insufficient input data, or all data is bad   // TD this true?
   ///        -9 GLONASS frequency channel could not be computed.
   /// @throw Exception on the following:
   ///    input data does not have the required obs types
   ///    DT is not set, or a memory problem was encountered
   ///    failed to find the Glonass frequency channel
   int DiscontinuityCorrector(SatPass& SP, std::string& retMsg,
                              std::vector<std::string>& cmds, int GLOn=-99);

   //---------------------------------------------------------------------------
   /// Overloaded version that accepts input data in parallel arrays.
   /// NB phases are in cycles, ranges in meters.
   /// See the doc in the SatPass version.
   /// This is where the work is done; SatPass version creates arrays and calls this.
   /// Flags on input must be either 1(OK) or 0(BAD) (as in SatPass), however on
   /// output they are defined by static consts OK, BAD, etc
   /// @throw Exception
   int DiscontinuityCorrector(const RinexSatID& sat,
                              const double& nominalDT,
                              const Epoch& beginTime,
                              std::vector<double> dataL1,
                              std::vector<double> dataL2,
                              std::vector<double> dataP1,
                              std::vector<double> dataP2,
                              std::vector<double> dt,
                              std::vector<int> flags,
                              std::string& retMsg, std::vector<std::string>& cmds,
                              int GLOn=-99,
                              std::string outfmt=std::string("%4F %10.3g"));

private:
   /// helper routine to initialize vectors
   static std::vector<unsigned> create_vector_SLIP(void) {
      std::vector<unsigned> v;
      v.push_back(Arc::WLSLIP);
      v.push_back(Arc::GFSLIP);
      return v;
   }
   /// helper routine to initialize vectors
   static std::vector<unsigned> create_vector_FIX(void) {
      std::vector<unsigned> v;
      v.push_back(Arc::WLFIX);
      v.push_back(Arc::GFFIX);
      return v;
   }
   /// helper routine to initialize vectors
   static std::vector<std::string> create_vector_LAB(void) {
      std::vector<std::string> v;
      v.push_back("WL");
      v.push_back("GF");
      return v;
   }

protected:
   /// Version string
   static const std::string GDCVersion;

   // NB flags[] is either good (0) or not (!0);
   // flags have NOTHING to do with either SatPass or Arc::marks
   // Arcs handles BEG, SLIP/FIX, and GAP(new BEG); flags handle bad data w/in Arc
   // NB BAD != SatPass::BAD but ONLY SatPass::BAD on input => BAD
   /// Values for flags[] = bit maps
   static const unsigned OK;         ///< flag for good data = 0 NB SatPass::OK = 1
   static const unsigned BAD;        ///< flag for bad data  = 1 NB SatPass::BAD = 0
   static const unsigned WLOUTLIER;  ///< flag for data called outlier by WL filter
   static const unsigned GFOUTLIER;  ///< flag for data called outlier by GF filter
   static const unsigned WLSHORT;    ///< flag for data with Arc.ngood < MinPts
   static const unsigned GFSHORT;    ///< flag for data with Arc.ngood < MinPts
   static const unsigned ISOLATED;   ///< flag for isolated good data (< MinPts)
   //others?

   /// conveniences
   static const unsigned WL;        ///< used internally to denote the WL combo
   static const unsigned GF;        ///< used internally to denote the GF combo
   static const std::vector<unsigned> SLIP,FIX; ///< slips, fixes
   static const std::vector<std::string> LAB;   ///< vector of labels: WL, GF
   static std::vector<double> wl;   ///< vector of wavelengths

   //---------------------------------------------------------------------------
   // configuration
   /// map containing configuration labels and their values
   std::map <std::string,double> CFG;

   /// map containing configuration labels and their descriptions
   std::map <std::string,std::string> CFGdesc;

   /// map used to keep them in order
   int CFGindex;
   std::map <int, std::string> CFGlist;

   //---------------------------------------------------------------------------
   /// unique number, counting passes or calls
   int unique;
   std::string tag;     ///< begin each output line with 'GDC <unique>' ( <label>)
   std::string SPSstr;  ///< SPS output of SatPass, or generated equivalent

   /// member data used internally
   RinexSatID sat;      ///< satellite id, from SatPass
   bool isGLO;          ///< true if this is a GLONASS satellite
   double dt;           ///< data time spacing, from SatPass, in sec
   Epoch beginT;        ///< begin time from SatPass

   std::string outfmt;  ///< output time format, from SatPass
   int oswidth;         ///< output stream width
   int osprec;          ///< output stream precision

   int GLOchan;         ///< GLONASS frequency channel (-99 by default in GDC())
   // just to keep numerical range down
   double WLbias;       ///< bias determined by initial value of WL, in wl
   double GFbias;       ///< bias determined by initial value of GF, in wl
   // approximate; for output of editing commands only
   long long N1bias;    ///< bias in L1 at initial point (from P1=wl1*(N1bias+L1))
   long long N2bias;    ///< bias in L2 at initial point (from P2=wl2*(N2bias+L2))

   double wl1;          ///< L1 wavelength, from sat, in meters
   double wl2;          ///< L2 wavelength, from sat, in meters
   double alpha;        ///< alpha, from sat
   double beta;         ///< beta, from sat
   double wlWL;         ///< WL wavelength ~86cm, in meters
   double wlGF;         ///< GF wavelength = wl2-wl1 = 5.376cm, in meters
   double wlNL;         ///< NL wavelength ~10.7cm, in meters

   /// vectors used in processing - these MUST always remain parallel and equal length
   /// vector of data e.g. WLC and GFP, in wavelengths
   std::vector<double> dataWL, dataGF;

   /// vector of dt*ndt = number of steps of dt from begin point * dt; from spdvector
   std::vector<double> xdata;

   // NB flags must be int, not unsigned, for StatsFilter processing
   /// vector of flags from SatPass OR GDC processing; non-zero == ignore data
   std::vector<int> flags;

   /// The list (map) of arcs used to store beginnings (BEG=first Arc and after gap),
   /// slips, number of points, number of good points and stats. The map has
   /// key=index into data arrays at which the Arc begins, and value = Arc. Note that
   /// Arcs[A.index] = A, always. Since Arcs is a map with key=index, it is sorted
   /// on index i.e. in the same order as data[]. The first value in this map
   /// (Arcs.begin(), at index 0) is always marked BEG and has index=0 = first
   /// data index, and the last Arc in Arcs has index+npts = last data index + 1.
   /// New Arcs are created whenever there is a slip, or a gap larger than MaxGap.
   /// Arcs hang around, but slips get fixed, e.g. Arc.mark=WLSLIP&GFSLIP&WLFIX&GFFIX.
   /// When a slip is fixed, it is marked with xxFIX and the data modified from
   /// Arc.index all the way to the end.
   std::map<int, Arc> Arcs;
 
   // member functions
   //---------------------------------------------------------------------------

   /// Initializer used in c'tor to define default configuration
   void init(void);

   /// process one combo (WL or GF) all the way through 1st diff and window filters,
   /// flagging outliers, marking and fixing slips, and dumping.
   /// @return return value of filter() if negative, otherwise number of slips found
   /// @throw Exception
   int ProcessOneCombo(const unsigned which);

   /// process one combo (WL or GF) using 1st differences; called by ProcessOneCombo
   /// @return return value of filter() if negative, otherwise number of slips found
   /// @throw Exception
   int GrossProcessing(const unsigned which);

   /// process one combo (WL or GF) using window filter; called by ProcessOneCombo
   /// @return return value of filter() if negative, otherwise number of slips found
   /// @throw Exception
   int FineProcessing(const unsigned which);

   /// filter using first differences
   /// @param which is either WL or GF
   /// @param label string to be passed to dump e.g. "GFG"
   /// @param limit pass to filter.setLimit()
   /// @param return vector<FilterHit> hits containing all outliers and slips
   /// @return return value of filter() if negative, otherwise number of slips found
   /// @throw Exception
   int filterFirstDiff(const unsigned which, const std::string label,
      const double& limit, std::vector< FilterHit<double> >& hits);

   /// filter using window filter
   /// @param which is either WL or GF
   /// @param label string to be passed to dump e.g. "GFG"
   /// @param limit pass to filter.setLimit()
   /// @param return vector< FilterHit<T> > hits containing all outliers and slips
   /// @return return value of filter() if negative, otherwise number of slips found
   /// @throw Exception
   int filterWindow(const unsigned which, const std::string label,
      const double& limit, std::vector< FilterHit<double> >& hits);

   /// merge filter results (vector<FilterHit>) into the Arcs list, and set flags[].
   /// The merge will mark outliers, add new Arc's where there are slips, and call
   /// fixUpArcs(), if necessary.
   /// Test with cases where there is huge data rejection in GF, after WL slips, etc.
   /// @param hits vector<FilterHit> which is results of filter
   /// @param which is either WL or GF
   /// @return the number of new Arcs in Arcs
   /// @throw Exception
   int mergeFilterResultsIntoArcs(std::vector< FilterHit<double> >& hits,
         const unsigned which);

   // the next three are called by mergeFilterResultsIntoArcs()

   /// Flag bad data in the flags[] array, using a filter hit object. Don't alter Arcs
   /// NB FixUpArcs() must be called after this routine to recompute ngood.
   /// Note that flags[] is changed ONLY if flags[] currently OK....TD use bitmap?
   /// @param hit        the FilterHit, which describes a segment to be marked
   /// @param flagvalue  the value to which flags[] is set for each outlier.
   /// @throw Exception
   void flagBadData(const FilterHit<double>& hit, const unsigned flagvalue);

   /// add a new Arc to Arcs at index, using the given value for mark.
   /// @param index the index into data[] at which to add the Arc
   /// @param mark the value to assign to the new Arc's mark
   /// @return true if new Arc created -> fixUpArcs() should be called.
   /// @throw Exception
   bool addArc(const int index, const unsigned mark);

      /// modify Arcs: recompute npts and ngood, remove empty Arcs
      /// @throw Exception
   void fixUpArcs(void);

      /// recompute the npts and ngood for each Arc using the indexes in the map: Arcs
      /// @throw Exception
   void recomputeArcs(void);

   /// recompute the number of good points in an Arc
   inline unsigned computeNgood(Arc& arc)
   {
      arc.ngood = 0;
      for(unsigned int i=arc.index; i < arc.index+arc.npts; ++i) {
         if(flags[i] == OK) arc.ngood++;
      }
      return arc.ngood;
   }

   /// compute stats for 'which' data (WL or GF or both: WL&GF) for all Arcs
   /// @param which is either WL or GF
   /// @throw Exception
   void getArcStats(const unsigned which)
   {
      try {
         for(std::map<int,Arc>::iterator ait = Arcs.begin(); ait != Arcs.end(); ++ait)
            getArcStats(ait, which);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// compute stats for 'which' data (WL or GF but not both) for the given Arc.
   /// NB this is sneaky and goes across fixed slips....
   /// NB do not confuse this with *Filter::getStats()
   /// @param iterator pointing to element of Arcs that contains the Arc
   /// @param which is either WL or GF
   /// @throw Exception
   void getArcStats(std::map<int,Arc>::iterator& ait, const unsigned which);

   /// find gaps within Arc in Arcs; if gap is larger than MaxGap, break the Arc into
   /// two, adding a BEG Arc after the gap.
   /// @throw Exception
   void findLargeGaps(void);

   /// Fix slips between Arcs, using info.step (NOT info.Nslip), which is defined
   /// by the filter in results(FilterHit).
   /// Compute an integer from step and ADD it to Nslip.
   /// Thus Nslip always has the total slip, while step has only the latest estimate,
   /// and is used to fix.
   /// In the case of the FirstDifferenceFilter this step is only an approximate fix;
   /// for the WindowFilter the step is defined by stats on the two segments
   /// (one-sample for WL and two-sample for GF).
   /// @param which is either WL or GF
   /// @return the number of slips fixed.
   /// @throw Exception
   int fixSlips(const unsigned which);

   /// find gaps within the given Arc, including those at the very beginning
   /// (index==Arc.index) and at the very end (index+nptsgap==Arc.index+Arc.npts).
   /// @return map with key=index of beginning of gap, value=number of points in gap.
   /// @throw Exception
   std::map<int,int> findGaps(const Arc& arc);

   /// dump the data stored in the data arrays
   void dumpData(std::ostream& os, const std::string msg=std::string());

   /// compute a time tag from array index: beginT + (xdata[i]==ndt)*dt
   Epoch xtime(const int& i)
   {
      if(i >= dataWL.size())
         GPSTK_THROW(Exception("Index out of range in xtime"));
      Epoch tt(beginT);
      tt += xdata[i];
      return tt;
   }

   /// find the Arc that contains a given index, starting with the given iterator
   /// @throw Exception if the index is out of range
   void findArc(const unsigned int ind, std::map<int, Arc>::iterator& ait)
   {
      if(int(ind) < ait->second.index)
         GPSTK_THROW(Exception("index before given Arc"));

      while(ait != Arcs.end() && ind >= ait->second.index+ait->second.npts) ++ait;

      if(ait == Arcs.end())         // index not found
         GPSTK_THROW(Exception("index after end of Arcs"));
   }

   /// dump all Arcs, e.g.
   ///#GDC  1 GFW Arc1[0] BEG good/tot=772/772=100.00% dWL=0.00wl 1210 -0.38 +-0.39
   ///                                                   dGF=0.00wl 1210 -0.38 +-0.34
   ///#GDC  1 GFW Arc2[772] WLslip/GFslip/WLfix/GFfix good/tot=438/445=98.43%
   ///                     dWL=46.54wl 438 -0.71 +-0.32 dGF=-46.37wl 438 -0.71 +-0.32
   void DumpArcs(const std::string& tag, const std::string& label, int prec=-1)
      throw();

   /// dump all FilterHits, e.g.
   ///#GDC  1 GFW Hit1[2] BOD npts=1206 ngood=1206 step 0.00
   ///                                            min 0.00 max 0.00 med 0.07 mad 0.03
   void DumpHits(const std::vector< FilterHit<double> >& filterResults,
                  const std::string& tag, const std::string& label, int prec=-1)
      throw();

   /// build the string that is returned by the discontinuity corrector
   std::string returnMessage(int prec=-1, int wid=-1) throw();

   /// apply the results to fix the input SatPass; cf. cfg(doFix)
   /// @param SP       SatPass object containing the input data.
   // @param breaks  vector of indexes where SatPass SP must be broken into two
   // @param marks   vector of indexes in SatPass SP where breaks are suspected
   /// @throw Exception
   void applyFixesToSatPass(SatPass& SP);

   /// apply the results to generate editing commands; cfg(doCmds)
   /// Use tk-RinEdit form for commands (--IF name, etc) since EditRinex also takes.
   /// @param cmds     vector of strings giving editing commands for RINEX editor.
   /// @throw Exception
   void generateCmds(std::vector<std::string>& cmds);

   /// do a final check on the pass. Look for isolated good points (< MinPts good
   /// points surrounded by N(?) bad points on each side.
   /// @throw Exception
   int FinalCheck(void);

}; // end class gdc

}  // end namespace gpstk

//------------------------------------------------------------------------------------
#endif   // GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE
