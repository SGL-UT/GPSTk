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

/// @file WindowFilter.hpp

///    This class implements a statistical filter that uses 'windowed' averages. There
/// are several statistical filters implemented as classes. These classes are
/// templates; the template parameter should be a float (probably double);
/// it is used to construct gpstk::Stats<T>, gpstk::TwoSampleStats<T> and
/// gpstk::SeqStats<T>, which are fundamental to these algorithms.
///    All the filters look for outliers and discontinuities (slips) in a timeseries.
/// The first difference filter analyses the simple first difference of the data.
/// The window filter uses a 2-pane sliding window centered on the data point in
/// question; statistics on the data in each to the 2 panes are computed and used
/// in the analysis.
///    The window filter uses 1- and 2-sample statistics in Stats.hpp, along with a
/// wrapper class (StatsFilterBase, this module) that provides a single interface for
/// the two statistics, allowing WindowFilter::filter() to use either type of filter
/// interchangably. Two-sample stats are used when an xdata array ("time") is given
/// along with the data array; this is appropriate for data that has systematic
/// "time" dependence. One-sample stats are used when the data is ~constant; in this
/// case the xdata can be given as well but will be used only in dump().
///    All the filters have a getStats(FilterHit) function that computes statistics on
/// the filter quantities (NOT the data) over the interval covered by the event, and
/// stores them in the FilterHit. These stats are slightly different for the two
/// filters; WindowFilter::getStats computes min, max, median and mad of
/// sigma = rms(sig of future and past), but not including points within one width of
/// the endpoints (avoids the bump in sigma due to slip(s) at the segment boundaries).
///
///    The structure of these filters allows the caller to call filters repeatedly,
/// and to call different filters, on the same dataset, because none of the filters
/// modify the data array(s) in any way. The arrays are passed as constant references
/// to the constructor.  The xdata reference must be provided, but it may be empty
/// (except for the window filter if two sample statistics are to be used).
/// If xdata is not empty, values of xdata are included in the dump() output.
///    Similarly, an integer vector of flags is also passed to the constructor (also
/// as a const reference), and it may be empty. If it is not empty, flag[i] != 0
/// causes the data at index i to be ignored by the filters.
///    The arrays data, xdata and flags must always be parallel, and xdata and flags
/// cannot be shorter than data unless they are empty (when they are ignored).
///    The filter() function has optional input of the starting index and the number
/// of points to process, so that segments of the data (and xdata and flags) array(s)
/// can be processed in the filters.
///    These features allow the user to, for example, call a filter, mark data in the
/// flags array (e.g. outliers) and then filter again. If a slip is found, the caller
/// can then filter the data again but starting at the slip, or filter only the
/// segment of data before the slip [using filter(index, npts)].
///    NB the caller must construct a new filter at each call - if you declare a
/// Filter object, run filter(), then use the results to modify flags[] and try to
/// call filter() again, it does not see the changes to flags[]. Instead you need to
/// call the constructor again.

#include <vector>
#include <deque>
#include "Stats.hpp"
#include "RobustStats.hpp"
//#include "StringUtils.hpp"       // TEMP
//#include "logstream.hpp"         // TEMP

#include "StatsFilterHit.hpp"

/// A special subset of class FilterHit used for "almost slips" in WindowFilter
template <class T> class FilterNearMiss
{
public:
   /// empty and only constructor
   FilterNearMiss() : index(-1), step(T(0)), score(0), msg(std::string()) { }

   int index;           ///< index in the data array(s) at which this event occurs
   int score;           ///< weight of slip, 0 < score <= 100
   T step;              ///< an estimate of the step in the data
   T sigma;             ///< for a slip, RSS future and past sigma on the data
   std::string msg;     ///< message from analyze()

}; // end class FilterNearMiss

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/// Build a wrapper for these Stats classes that allows interchangeability in
/// a statistical 'filter' designed for a times series of (x,y) data.
/// The pure virtual class StatsFilterBase will define the interface.
template <class T> class StatsFilterBase
{
public:
   /// constructor
   StatsFilterBase() {}

   /// reset, i.e. ignore earlier data and restart sampling
   virtual void Reset(void) = 0;

   /// return the sample size
   virtual unsigned int N(void) const = 0;

   /// Add data to the statistics; in 1-sample stats the x is ignored
   virtual void Add(const T& x, const T& y) = 0;

   /// Subtract data from the statistics; in 1-sample stats the x is ignored
   virtual void Subtract(const T& x, const T& y) = 0;

   /// return computed standard deviation, in 2-sample stats this is SigmaYX()
   virtual T StdDev(void) const = 0;

   /// return computed variance, in 2-sample stats this is VarianceYX()
   virtual T Variance(void) const = 0;

   /// return the average; in 2-sample stats this is AverageY()
   virtual T Average(void) const = 0;

   /// return the predicted Y at the given X; in 1-sample stats this is Average()
   virtual T Evaluate(T x) const = 0;

   /// return the slope of the best-fit line Y=slope*X+intercept;
   /// in 1-sample stats this is 0.0
   virtual T Slope(void) const = 0;

   /// return the intercept of the best-fit line Y=slope*X+intercept;
   /// in 1-sample stats this is Average()
   virtual T Intercept(void) const = 0;

   /// return the stats as a single string
   virtual std::string asString(void) const = 0;

}; // end class StatsFilterBase

/// A StatsFilter class for one-sample statistics that inherits StatsFilterBase.
template <class T> class OneSampleStatsFilter : public StatsFilterBase<T>
{
public:
   /// constructor
   OneSampleStatsFilter() { }

   /// reset, i.e. ignore earlier data and restart sampling
   inline void Reset(void) { S.Reset(); }

   /// return the sample size
   inline unsigned int N(void) const { return S.N(); }

   /// Add data to the statistics; in 1-sample stats the x is ignored
   void Add(const T& x, const T& y) { S.Add(y); }

   /// Subtract data from the statistics; in 1-sample stats the x is ignored
   void Subtract(const T& x, const T& y) { S.Subtract(y); }

   /// return computed standard deviation
   T StdDev(void) const { return S.StdDev(); }

   /// return computed variance
   T Variance(void) const { return S.Variance(); }

   /// return the average
   inline T Average(void) const { return S.Average(); }

   /// return the predicted Y at the given X;
   /// in 1-sample stats this is Average() and x is ignored
   inline T Evaluate(T x) const { return S.Average(); }

   /// return the slope of the best-fit line Y=slope*X+intercept;
   /// in 1-sample stats this is 0.0
   inline T Slope(void) const { return T(); }

   /// return the intercept of the best-fit line Y=slope*X+intercept;
   /// in 1-sample stats this is Average()
   inline T Intercept(void) const { return S.Average(); }

   /// return the stats as a single string
   std::string asString(void) const { return S.asString(); }

private:
   gpstk::Stats<T> S;

}; // end class OneSampleStatsFilter

/// A StatsFilter class for two-sample statistics that inherits StatsFilterBase.
template <class T> class TwoSampleStatsFilter : public StatsFilterBase<T>
{
public:
   /// constructor
   TwoSampleStatsFilter() { }

   /// reset, i.e. ignore earlier data and restart sampling
   inline void Reset(void) { TSS.Reset(); }

   /// return the sample size
   inline unsigned int N(void) const { return TSS.N(); }

   /// Add data to the statistics
   void Add(const T& x, const T& y) { TSS.Add(x,y); }

   /// Subtract data from the statistics
   void Subtract(const T& x, const T& y) { TSS.Subtract(x,y); }

   /// return computed standard deviation; in 2-sample stats this is SigmaYX()
   T StdDev(void) const {
      if(TSS.N() < 3) return TSS.StdDevY();      // cheat a little
      return TSS.SigmaYX();
   }

   /// return computed variance; in 2-sample stats this is VarianceYX()
   T Variance(void) const {
      if(TSS.N() < 3) return TSS.VarianceY();      // cheat a little
      return TSS.VarianceYX();
   }

   /// return the average; in 2-sample stats this is AverageY()
   inline T Average(void) const { return TSS.AverageY(); }

   /// return the predicted Y at the given X;
   inline T Evaluate(T x) const { return TSS.Evaluate(x); }

   /// return the slope of the best-fit line Y=slope*X+intercept;
   inline T Slope(void) const { return TSS.Slope(); }

   /// return the intercept of the best-fit line Y=slope*X+intercept;
   inline T Intercept(void) const { return TSS.Intercept(); }

   /// return the stats as a single string
   std::string asString(void) const { return TSS.asString(); }

private:
   gpstk::TwoSampleStats<T> TSS;

}; // end class TwoSampleStatsFilter

// end template <class T> class StatsFilterBase

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/// This class computes statistics in a sliding window with two equal halves, 'past'
/// and 'future', at each point in the input data array(s). The objective is to detect
/// discontinuities or "slips" and compute statistics on the data between slips.
/// This class will read the data, apply the window filter, generating statistics that
/// are stored, then analyze the statistics, returning a vector of simple results,
/// and optionally dump all the data, analysis information and results.
template <class T> class WindowFilter
{
public:
   /// class used to store analysis arrays filled by the window filters.
   class Analysis
   {
   public:
      /// constructor
      Analysis() : pN(0), fN(0) { }

      // member data
      unsigned int index;  ///< index in original arrays to which this info applies.
      T step;              ///< step in average: average(future) - average(past)
      T sigma;             ///< combined standard deviation: RSS(stddev(f),stddev(p))
      // past stats
      unsigned int pN;     ///< number of points in the past buffer
      T pave;              ///< average of <width> points in past
      T psig;              ///< standard deviation of <width> points in past
      // future stats
      unsigned int fN;     ///< number of points in the future buffer
      T fave;              ///< average of <width> points in future
      T fsig;              ///< standard deviation of <width> points in future
      //T pslope;          ///< (twoSample only) slope (ave p,f) in units data/xdata
      //T fslope;          ///< (twoSample only) slope (ave p,f) in units data/xdata
      // results of analysis
      /// net result of analysis: -1,-2,-3,-4 (failure) or a percentage
      /// -1 near end; -2 small step; -3 small ratio; -4 marginal step&ratio;
      int score;
      std::string msg;     ///< readable description of what analysis found

   }; // end class Analysis

   /// constructor
   /// @param x const vector<T> of 'times' values, NB (x,d,f) all parallel
   /// @param d const vector<T> of data values
   /// @param f const vector<int> of flags, 0 means good. this array may be empty.
   WindowFilter(const std::vector<T>& x,
                const std::vector<T>& d,
                const std::vector<int>& f) : xdata(x), data(d), flags(f)
   {
      width = 20;
      twoSample = false;
      minratio = T(2.0);
      minstep = T(0.8);
      minmargin = T(0.5);
      pffrac = T(0.75);
      halfwidth = 3;
      buffsize = 0;
      balanced = false;
      fullwindows = false;
      noxdata = (xdata.size() == 0);
      noflags = (flags.size() == 0);
      dumpNA = true;
      dumpAmsg = false;
      debug = false;
      osw = 8; osp = 3;
   }

   /// get and set filter configuration
   inline void setWidth(int w) { width = w; }
   inline void setBufferSize(int b) { buffsize = b; }
   inline void setTwoSample(bool b) { twoSample=b; }
   inline void setBalanced(bool b) { balanced = b; }
   inline void setFullWindows(bool b) { fullwindows = b; }
   inline int getWidth(void) { return width; }
   inline int getBufferSize(void) { return buffsize; }
   inline bool isTwoSample(void) { return twoSample; }
   inline bool isOneSample(void) { return !twoSample; }
   inline bool isBalanced(void) { return balanced; }
   inline bool isFullWindows(void) { return fullwindows; }
   /// get and set analysis configuration
   inline void setMinRatio(T val) { minratio=val; }
   inline void setMinStep(T val) { minstep=val; }
   inline void setMinMargin(T val) { minmargin=val; }
   inline void setPFFrac(T val) { pffrac=val; }
   inline void setHalfWidth(int hw) { halfwidth=hw; }
   inline T getMinRatio(void) { return minratio; }
   inline T getMinStep(void) { return minstep; }
   inline T getMinMargin(void) { return minmargin; }
   inline T getPFFrac(void) { return pffrac; }
   inline int getHalfWidth(void) { return halfwidth; }
   /// get and set for dump() parameters
   /// in dump(), include the little message at the end of line from analyze()
   inline void setDumpAnalMsg(bool b) { dumpAmsg = b; }
   inline bool willDumpAnalMsg(void) { return dumpAmsg; }
   /// in dump(), don't dump data when there was no analysis
   inline void setDumpNoAnal(bool b) { dumpNA = b; }
   inline bool willDumpNoAnal(void) { return dumpNA; }
   /// debug prints in analysis()
   inline void setDebug(bool b) { debug = b; }
   inline bool getDebug(void) { return debug; }

   inline void setw(int w) { osw=w; }
   inline void setprecision(int p) { osp=p; }

   /// get results vector of FilterHit
   std::vector< FilterHit<T> > getResults(void) { return results; }

   /// reset the analysis information stored internally. This does not change the
   /// data arrays; to do so, instantiate a new filter object.
   void reset(void) { analvec.clear(); }

   /// window filter routine that does the work. NB this routine configures the
   /// analysis data and therefore is critical to anlayse() and dump().
   /// @param index in data arrays at which to start, defaults to 0
   /// @param npts  number of data to process, defaults to -1, meaning to end of data.
   /// @return > 0 number of points in analysis vector, which will be ngood
   ///          -1 the data arrays are too short for the given window width
   ///          -2 if the xdata-array is required (twoSample) yet missing
   ///          -3 if the xdata|flags array is given but shorter than data array
   /// This routine clears the analysis vector, runs filter and generates analysis.
   int filter(const size_t index=0, int npts=-1);

   /// analyze the output of the filter(), filling the analysis array 'analvec'
   /// also fills the 'results' vector of one or more FilterHit.
   /// @return the number of slips, outliers, etc (size of results vector<FilterHit>)
   int analyze(void);

   /// dump the data and analysis, including first and last 2 pts which have no stats.
   /// optionally include a tag at the start of each line, and configure width and
   /// precision.
   void dump(std::ostream& s, std::string tag=std::string());

   // NB there should be another routine (outside this class) to get stats on the
   // data within a segment
   /// compute stats on the filter quanitites within the given FilterHit, defined as
   /// the min, max, median and mad of sigma, the rss(future and past stddev).
   /// if the flag is true (default), exclude data within the filter width of the
   /// end points, to avoid the bump(s) due to slip(s) at the FilterHit boundaries.
   void getStats(FilterHit<T>& fe, bool noedge=true);

private:

   // member data
   bool balanced;                ///< if true, 2 panes of sliding window have = size
   bool fullwindows;             ///< if true, only process with full windows
   bool twoSample;               ///< if true, use two-sample statistics
   unsigned int width;           ///< width or number of points in (1 pane of) window
   int buffsize;                 ///< number of good points ignored btwn past, future
   bool noxdata;                 ///< true when xdata array is not given
   bool noflags;                 ///< true when flags array is not given

   unsigned int halfwidth;       ///< number of points on either side of slip analyzed
   T minratio;                   ///< ratio=|step/sig| < this is not a slip
   T minstep;                    ///< |step|(=fut ave - past ave) < this is not slip
   T minmargin;                  ///< limit on step/minstep+ratio/minratio-2
   T pffrac;                     ///< delta(f,p) sigma < this frac * sigma not a slip

   const std::vector<T>& xdata;  ///< reference to x-data to be filtered (TwoSample)
   const std::vector<T>& data;   ///< reference to data to be filtered
   const std::vector<int>& flags;///< reference to flags, parallel to data, 0 == good

   int osw,osp;                  ///< width and precision for dump(), (default 8,3)
   bool dumpNA;                  ///< if false, don't dump() data with no analysis (T)
   bool dumpAmsg;                ///< if true, add analysis message in dump() (F)
   bool debug;                   ///< if true, print debug messages in analyze() (F)

   /// vector of Analysis objects, holding analysis information, generated by filter()
   /// and used by analyze() and included in dump() output.
   std::vector<Analysis> analvec;

public:

   /// vector of FilterHit, generated and returned by analyze();
   /// keep this copy for use in dump()
   std::vector< FilterHit<T> > results;

   /// vector of FilterNearMiss, generated by analyze()
   std::vector< FilterNearMiss<T> > maybes;

}; // end class WindowFilter

//------------------------------------------------------------------------------------
// return number of analyzed points, else -1 too little data, -2 two sample w/o x,
//    -3 too little x or flag data.
template<class T> int WindowFilter<T>::filter(const size_t i0, int dsize)
{
   // number of points to filter
   if(dsize == -1)   // this bc can't use data.size() as a default arg
      dsize = data.size()-i0-buffsize;

   // largest index is ilimit-1
   const int ilimit(dsize+i0);

   // validate input -----------------------------------------------------------
   // is there enough data to apply the filter? if not, return -1
   // if there are flags, count the good points
   int i(i0),j;
   unsigned int n(0);
   if(!noflags) {
      while(i<ilimit && n < 2*width+buffsize) { if(flags[i]==0) n++; i++; }
      if(i==ilimit) return -1;
   }
   else if(dsize < int(2*width+buffsize)) return -1;

   // is xdata given? can't two-sample without x...
   if(twoSample && noxdata) return -2;

   // if xdata or flags is there, make sure its big enough
   if(!noxdata && xdata.size()-i0-buffsize < dsize) return -3;
   if(!noflags && flags.size()-i0-buffsize < dsize) return -3;

   // create stats for "past" and "future" sliding windows ---------------------
   StatsFilterBase<T> *ptrPast, *ptrFuture;

   if(twoSample) {
      ptrPast = new TwoSampleStatsFilter<T>();
      ptrFuture = new TwoSampleStatsFilter<T>();
   }
   else {
      ptrPast = new OneSampleStatsFilter<T>();
      ptrFuture = new OneSampleStatsFilter<T>();
   }

   // stick a little buffer, length buffsize, holding indexes, between past and future
   std::deque<int> buff;

   // --------------------------------------------------------------------------
   // Cartoon of the 'two-pane moving window'
   // windows:  'past window'      'future window'
   // stats  :  ----pastStats----  ----futureStats--
   // data   : (x x x x x x x x x)(x x x x x x x x x) x ...
   //           |               |  |                  |
   // indexes: iminus          i-1 i                 iplus
   // at each step, move i from F to P, add iplus to F, subtract iminus from P
   // --------------------------------------------------------------------------
   // if balanced=F, at the begin(end), only the past(future) window will shrink.
   // stats  :  -pastSt-  ----futureStats--
   // data   : (x x x x)(x x x x x x x x x) x ...
   //           |        |   |              |
   // indexes: iminus    i-1 i            iplus
   // So, at each step, move i from F to P;
   //                   if(iplus < size-1)   add 1 to F
   //                   if(past.N()>=width)  sub 1 from P
   //                   else                 add 1 to F, sub 1 from P
   // --------------------------------------------------------------------------
   // if balanced=T, force the past and future windows to stay the same size
   // start:
   //        i=1|  |ip=2
   // data: (x)(x) x ...
   //        |im=0            now move i from F to P, add 2 to F
   //             |i=2 |ip=4
   // data: (x x)(x x) x ...
   //          |im=1          now move i from F to P, add 2 to F
   //               |i=3   |ip=6
   // data: (x x x)(x x x) x ...
   //            |im=2        now move i from F to P, add 2 to F
   // do this until P.N() = width
   // --------------------------------------------------------------------------
   // stop:
   // data: (x x x x x x x x x)(x x x x x x x x x)
   //        |im                |i                |ip==size() now mv i F->P, P-2
   // data:  x x(x x x x x x x x)(x x x x x x x x)
   //            |im              |i              |ip==size() now mv i F->P, P-2
   // data:  x x x x(x x x x x x x)(x x x x x x x)
   //                |im            |i            |ip==size() now mv i F->P, P-2
   // data:  x x x x x x(x x x x x x)(x x x x x x)
   //                    |im          |i          |ip==size() now mv i F->P, P-2
   // data:  x x x x x x x x(x x x x x)(x x x x x)
   //                        |im        |i        |ip==size() now mv i F->P, P-2
   // So, at each step, move i from F to P;
   //                   if(P.N()<width+1)    add 2 to F
   //                   else if(ip==size())  sub 2 from P
   //                   else                 add 1 to F, sub 1 from P
   // NB when balanced, pts are add(sub)ed TWO at a time - does this affect result?
   // --------------------------------------------------------------------------

   // clear the analysis vector
   analvec.clear();

   // try to make index handling clean and neat
   #define dvec(i) (data[i])
   // use a dummy if xdata is not there
   #define xvec(i) (noxdata ? T(i) : xdata[i])
   // increment indexes - leave off last ;
   #define inc(i) i++; if(!noflags) while(i<ilimit && flags[i]) i++

   // find the first good point, but don't necessarily increment
   i = i0; if(!noflags) while(i<ilimit && flags[i]) i++;

   //TEMPtestTSS// keep deques containing the indexes i in the past and future stats
   //TEMPtestTSS std::deque<int> pindex,findex;

   // start with two points in past, and up to width pts in future
   // 'return -1' code above implies this will not overrun arrays
   int iplus,iminus(i),isecond;
   ptrPast->Add(xvec(i),dvec(i));      // put first point in past
   //TEMPtestTSS pindex.push_back(i);
   T xprev(xvec(i)), xmid;             // save prev x for two-sample slips
   inc(i);                             // second good point
   ptrPast->Add(xvec(i),dvec(i));      // put second point in past
   //TEMPtestTSS pindex.push_back(i);

   // fill the buffer
   while(buff.size() < buffsize) { inc(i); buff.push_back(i); }

   // continue filling windows
   if(fullwindows) {                   // fill up past and future (x x...x)(x x...x)
      while(ptrPast->N() < width) {    // assumes dsize > 2*width+buffsize
         inc(i);
         buff.push_back(i);
         // always true here if(buff.size() > buffsize)
         j = buff[0];
         buff.pop_front();
         ptrPast->Add(xvec(j),dvec(j));
         //TEMPtestTSS pindex.push_back(j);
      }
      isecond = iplus = i;
      while(ptrFuture->N() < width) {  // assumes dsize > 2*width+buffsize
         // NB this fails: ptrFuture->Add(xvec(iplus),dvec(iplus++)); don't use it.
         inc(iplus);
         ptrFuture->Add(xvec(iplus),dvec(iplus));
         //TEMPtestTSS findex.push_back(iplus);
      }
      inc(iplus);
   }
   else if(balanced) {                 // start at (x x x)(x x x)
      inc(i); ptrPast->Add(xvec(i),dvec(i));       // put third point in past
      //TEMPtestTSS pindex.push_back(i);
      isecond = i;
      xprev = xvec(i);
      inc(i); ptrFuture->Add(xvec(i),dvec(i));     // put 3 into future
      //TEMPtestTSS findex.push_back(i);
      inc(i); ptrFuture->Add(xvec(i),dvec(i));
      //TEMPtestTSS findex.push_back(i);
      inc(i); ptrFuture->Add(xvec(i),dvec(i));
      //TEMPtestTSS findex.push_back(i);
      inc(i);
      iplus = i;
   }
   else {                              // fill up the future (x x)(x x x ... x) x
      isecond = iplus = i;
      while(ptrFuture->N() < width) {  // assumes dsize > 2*width+buffsize
         // NB this fails: ptrFuture->Add(xvec(iplus),dvec(iplus++)); don't use it.
         inc(iplus);
         ptrFuture->Add(xvec(iplus),dvec(iplus));
         //TEMPtestTSS findex.push_back(iplus);
      }
      inc(iplus);
   }

   // need the equivalent of i0+dsize-2
   // or if(fullwindows) i0+dsize-width
   int limm2(ilimit);
   for(i=0; i<3; i++) { limm2--; if(!noflags) while(flags[limm2]) limm2--; }

   // loop over all points
   // NB no i++ in this for-loop (inc() instead), and no continues in loop
   for(i=isecond; i<limm2;  ) {
      xprev = xvec(i);
      inc(i);                       // instead of i++ in for(), do it here

      // save results in this, add to vector 'analvec'
      Analysis A;
      A.index = i;
      A.pN = ptrPast->N();
      A.fN = ptrFuture->N();
      // assume slip happens at midpt of interval (this can matter with gaps)
      xmid = xprev + 0.5*(xvec(i)-xprev);
      A.pave = ptrPast->Evaluate(xmid); //xvec(i));
      A.fave = ptrFuture->Evaluate(xmid); //xvec(i));

      //Dump the TSS, build the TSS manually, and print that
      //TEMPtestTSS {
      //TEMPtestTSS gpstk::TwoSampleStats<T> pTSS,fTSS;
      //TEMPtestTSS unsigned int k;
      //TEMPtestTSS std::string str;

      //TEMPtestTSS std::cout << "At i=" << i << " Past_indexes(" << pindex.size()
      //TEMPtestTSS           << "):";
      //TEMPtestTSS for(k=0; k<pindex.size(); k++) {
         //TEMPtestTSS std::cout << " " << pindex[k];
         //TEMPtestTSS pTSS.Add(xvec(pindex[k]),dvec(pindex[k]));
      //TEMPtestTSS }
      //TEMPtestTSS std::cout << std::endl;

      //TEMPtestTSS str = pTSS.asString();
      //TEMPtestTSS gpstk::StringUtils::change(str,"\n"," ");
      //TEMPtestTSS std::cout << std::fixed << std::setprecision(3) << xvec(i)
      //TEMPtestTSS           << " pman " << str << std::endl;

      //TEMPtestTSS str = ptrPast->asString();
      //TEMPtestTSS gpstk::StringUtils::change(str,"\n"," ");
      //TEMPtestTSS std::cout << std::fixed << std::setprecision(3) << xvec(i)
      //TEMPtestTSS           << " past " << str << " eval " << A.pave << std::endl;

      //TEMPtestTSS // ---------

      //TEMPtestTSS std::cout << "At i=" << i << " Futu_indexes(" << findex.size()
      //TEMPtestTSS           << "):";
      //TEMPtestTSS for(k=0; k<findex.size(); k++) {
         //TEMPtestTSS std::cout << " " << findex[k];
         //TEMPtestTSS fTSS.Add(xvec(findex[k]),dvec(findex[k]));
      //TEMPtestTSS }
      //TEMPtestTSS std::cout << std::endl;

      //TEMPtestTSS str = fTSS.asString();
      //TEMPtestTSS gpstk::StringUtils::change(str,"\n"," ");
      //TEMPtestTSS std::cout << std::fixed << std::setprecision(3) << xvec(i)
      //TEMPtestTSS           << " fman " << str << std::endl;

      //TEMPtestTSS str = ptrFuture->asString();
      //TEMPtestTSS gpstk::StringUtils::change(str,"\n"," ");
      //TEMPtestTSS std::cout << std::fixed << std::setprecision(3) << xvec(i)
      //TEMPtestTSS           << " futu " << str << " eval " << A.fave
      //TEMPtestTSS           << std::endl << std::endl;

      //TEMPtestTSS }

      // compute a "step" = difference in future and past averages
      // must evaluate at the same x-point
      // NB for two-sample, this accounts for slope - see Evaluate() in each filter
      A.step = A.fave - A.pave;

      // get sigmas
      // test variance - sometimes with large range in data, variance at small N < 0
      A.psig = ptrPast->Variance();
      A.fsig = ptrFuture->Variance();
      if(A.psig <= T(0) && A.fsig <= T(0))
         A.psig = A.fsig = T(1);                   // TD better?
      else if(A.psig <= T(0))
         A.psig = A.fsig = ::sqrt(A.fsig);
      else if(A.fsig <= T(0))
         A.psig = A.fsig = ::sqrt(A.psig);
      else {
         A.psig = ::sqrt(A.psig);
         A.fsig = ::sqrt(A.fsig);
      }

      // compute a "sigma" = RSS of future and past stats
      // TD add a sqrt(1/2) here? Technically the sum is
      //   sqrt(((Nf-1)*Varf+(Np-1)*Varp)/(Nf+Np-1))
      // because "slip" is assumed removed, ave.s are same => above applies exactly
      A.sigma = ::sqrt((ptrFuture->Variance()+ptrPast->Variance())/T(2.0));

      if(debug) std::cout << "WF:FIL" << std::fixed << std::setprecision(osp)
         << " " << std::setw(osw) << xvec(i)
         << " " << std::setw(osw) << dvec(i)
         << " " << std::setw(osw) << A.step
         << " " << std::setw(osw) << A.sigma
         << " " << std::setw(3) << A.pN
         << " " << std::setw(osw) << A.pave
         << " " << std::setw(osw) << A.psig
         << " " << std::setw(3) << A.fN
         << " " << std::setw(osw) << A.fave
         << " " << std::setw(osw) << A.fsig
         << " " << std::setw(osw) << ::fabs(A.step/A.sigma) << std::endl;

      // save in analvec
      analvec.push_back(A);

      // update stats ----------------------------------------------
      // at each step, move i from F to P;
      //               if(P.N()<width+1)    add 2 to F
      //               else if(ip==size())  sub 2 from P
      //               else                 add 1 to F, sub 1 from P
      // move i from future to past
      ptrFuture->Subtract(xvec(i),dvec(i));
      //TEMPtestTSS findex.pop_front();
      buff.push_back(i);
      j = buff[0];
      buff.pop_front();
      ptrPast->Add(xvec(j),dvec(j));
      //TEMPtestTSS pindex.push_back(j);

      // if fullwindows, quit when future meets limit
      // ?? won't it just stop b/c loop reaches end?
      // NB fullwindows overrides balanced
      if(fullwindows && iplus >= i0+dsize-1) break;

      // if balanced and future has met the end-of-data, remove two from past
      if(balanced && iplus == i0+dsize) {         // assumes data.size() > 2*width
         //TEMPtestTSS std::cout << " balanced,fateod\n";
         ptrPast->Subtract(xvec(iminus),dvec(iminus));
         //TEMPtestTSS pindex.pop_front();
         inc(iminus);
         ptrPast->Subtract(xvec(iminus),dvec(iminus));
         //TEMPtestTSS pindex.pop_front();
         inc(iminus);
      }

      // else if balanced and past not full, move two into the future
      else if(balanced && ptrPast->N() < width+1) {           // same assumption
         //TEMPtestTSS std::cout << " balanced,pastnotfull\n";
         ptrFuture->Add(xvec(iplus),dvec(iplus));
         //TEMPtestTSS findex.push_back(iplus);
         inc(iplus);
         ptrFuture->Add(xvec(iplus),dvec(iplus));
         //TEMPtestTSS findex.push_back(iplus);
         inc(iplus);
      }

      // else not near either end
      else {
         // move iplus up by one
         //TEMPtestTSS std::cout << " normal\n";
         if(balanced || iplus < i0+dsize-1) {
            ptrFuture->Add(xvec(iplus),dvec(iplus));
            //TEMPtestTSS findex.push_back(iplus);
            inc(iplus);
         }
         // and move iminus up by one
         if(balanced || ptrPast->N() > width) {
            ptrPast->Subtract(xvec(iminus),dvec(iminus));
            //TEMPtestTSS pindex.pop_front();
            inc(iminus);
         }
      }

   }  // end loop over all data

   delete ptrPast;
   delete ptrFuture;

   #undef xvec
   #undef dvec
   #undef inc

   return analvec.size();

}  // end WindowFilter::filter()

//------------------------------------------------------------------------------------
// analysis, with debug print
// test 1a. ratio must be > minratio (2)
// test 1b. step must be > minstep (0.8)
// test 1c. step/minstep + ratio/minratio - 2 must be > minmargin (0.5)
// look in neighborhood of i - is ratio a local max and sigma a local min?
// test 2. ratio is a local max, n2=# intervals with right sign, n2 <= 2*halfwidth
// test 3. sigma is a local min, n3=# intervals with right sign, n3 <= 2*halfwidth
// test 4. fsig > psig before and psig > fsig after (count points that don't satisfy)
//    allow one miss in count if |slip| is > 1
// return the number of FilterHit in results.
template<class T> int WindowFilter<T>::analyze(void)
{
   // create first event = BOD; define npts later
   results.clear();
   {
      FilterHit<T> fe;
      fe.index = analvec[0].index;
      fe.ngood = 0;
      fe.type = FilterHit<T>::BOD;
      results.push_back(fe);
   }
   int curr(0);
   size_t i,j;
   double tmp = 0.0;

   // ratio(step/sigma), its 1st diff, sigma, its 1st diff, future minus past sigma
   std::deque<double> rat,rat1d,sig,sig1d,fminusp;
   // messages added to analvec[i].msg;
   std::string ratmsg,sigmsg,fmpmsg,wtmsg;

   if(debug) std::cout << "WF:ANL size is " << analvec.size() << std::endl;
   for(i=0; i<analvec.size(); i++) {    // loop over arrays

      // 'prime the pump' for the deques
      if(i==0) {
         for(j=0; j<halfwidth; j++) {
            rat.push_back(0.0); sig.push_back(0.0); fminusp.push_back(0.0);
         }
         for(j=0; (j<=halfwidth && j<analvec.size()); j++) {
            rat.push_back(::fabs(analvec[j].step/analvec[j].sigma)); // ratio
            sig.push_back(analvec[j].sigma);                         // sigma
            fminusp.push_back(analvec[j].fsig - analvec[j].psig);    // fsig - psig
         }
         for(j=0; j<2*halfwidth; j++) { rat1d.push_back(0.0); sig1d.push_back(0.0); }
      }

      // update the deques
      else if(i+halfwidth < analvec.size()) {
         // fill deques
         tmp = rat.back();                      // ratio
         rat.push_back(::fabs(analvec[i+halfwidth].step/analvec[i+halfwidth].sigma));
         rat1d.push_back(rat.back()-tmp);       // and its first diff
         tmp = sig.back();                      // sigma
         sig.push_back(analvec[i+halfwidth].sigma);
         sig1d.push_back(sig.back()-tmp);       // and sig first diff
         // fsig - psig .. no first diff
         fminusp.push_back(analvec[i+halfwidth].fsig - analvec[i+halfwidth].psig);
      }

      // keep deques size 2*half+1
      while(rat.size() > 2*halfwidth+1) {
         rat.pop_front(); sig.pop_front(); fminusp.pop_front();
      }
      // keep 1st difference deques size 2*half
      while(rat1d.size() > 2*halfwidth) { rat1d.pop_front(); sig1d.pop_front(); }

      // test min/max in ratio, sig and fmp of the form +,+,+,any,-,-,-
      bool rmax=true, smin=true, fmp=true;
      int fmpcount(2*halfwidth);
      double fmp0(fminusp[halfwidth]);
      double rat0(::fabs(rat[halfwidth]));
      for(j=0; j<halfwidth; j++) {
         // test is that ratio is at maximum - so 1st dif is +,+,+,-,-,-
         //                                              j=  0 1 2 h h+1 h+2
         if(j == halfwidth-1) {
            if(rat1d[j] < 0.0) rmax=false;
            if(rat1d[j+halfwidth] > 0.0) rmax=false;
         }
         else {
            if(rat1d[j] < -rat0/10.0) rmax=false;
            if(rat1d[j+halfwidth] > rat0/10.0) rmax=false;
         }

         //if(rat1d[j] > 0.0) rmin=false; else
         //if(rat1d[j+halfwidth] < 0.0) rmin=false; else

         if(fminusp[j]-fmp0 < 0.0)             { fmp=false; fmpcount--; }
         if(fminusp[j+halfwidth+1]-fmp0 > 0.0) { fmp=false; fmpcount--; }
      }

      // if(twoSample) same as 1-samp when there's no gap, but with gap its different
      //       - see toy.gf.gap - looks like 2 limp clotheslines on big poles
      //       +small,+verysmall,-big,(slim),+big,-verysmall,-small
      //  sig1d[] 0     1         h-1         h      h+1      h+2
      double slim(0.04*analvec[i].sigma);    // 5/16, was 0.02   // why 0.04?
      if(twoSample) {
         smin = true;
         if(-sig1d[halfwidth-1]/slim < 2.0) smin=false;
         else if(sig1d[halfwidth]/slim < 2.0) smin=false;
         else for(j=0; j<halfwidth-1; j++) {
            if(::fabs(sig1d[j]/sig1d[halfwidth-1]) > 0.5) smin=false;
            if(::fabs(sig1d[halfwidth+1+j]/sig1d[halfwidth]) > 0.5) smin=false;
         }
      }
      else {
         for(j=0; j<halfwidth; j++) {
            // for 1-sample, test is sigma is at minimum - so 1st dif is -,-,-,*,+,+,+
            if(sig1d[j] > slim) smin=false;
            if(sig1d[j+halfwidth] < -slim) smin=false;
         }
      }

      // make this configurable?
      if(2*halfwidth-fmpcount <= halfwidth/3) fmp=true;

      // define a weight [0,1], used in score but only if it passes first tests
      double weight=(rmax ? 0.25:0)+(smin ? 0.25:0)+0.5*fmpcount/double(2*halfwidth);

      // dump all the deque to a string, for debug and dumpAnalMsg (verbose) output
      {
         std::ostringstream oss;
         oss << " F-P" << std::fixed << std::setprecision(3);
         for(j=0; j<fminusp.size(); j++) oss << "," << fminusp[j]-fmp0;
         oss << ",cnt=" << fmpcount << "/" << 2*halfwidth;
         fmpmsg = oss.str();
         oss.str("");
         oss << " RAT1d" << std::fixed << std::setprecision(3);
         for(j=0; j<rat1d.size(); j++) oss << "," << rat1d[j];
         ratmsg = oss.str();
         oss.str("");
         oss << " SIG1d" << std::scientific << std::setprecision(1);
         for(j=0; j<sig1d.size(); j++) oss << "," << sig1d[j];
         oss << ",(" << slim << ")";
         sigmsg = oss.str();
         oss.str("");
         if(weight > 0) {
            if(tmp)
               oss << " changeF-P " << std::scientific << std::setprecision(2) << tmp;
            oss << " wt=" << std::fixed << std::setprecision(3) << weight;
         }
         wtmsg = oss.str();
      }

      // count it; only good data gets into analvec
      results[curr].ngood++;

      // debug print - also see single line below near end of routine
      if(debug) std::cout << "WF:ANL"
         << " " << std::setw(3) << i << " " << std::setw(3) << analvec[i].index
         << std::fixed << std::setprecision(osp) << std::setw(osw)
            << " " << (noxdata ? T(analvec[i].index) : xdata[analvec[i].index])
         << " " << std::setw(osw) << data[analvec[i].index]
         << " " << std::setw(osw) << analvec[i].step
         << " " << std::setw(osw) << analvec[i].sigma
         << " " << std::setw(3) << analvec[i].pN
         << " " << std::setw(osw) << analvec[i].pave
         << " " << std::setw(osw) << analvec[i].psig
         << " " << std::setw(3) << analvec[i].fN
         << " " << std::setw(osw) << analvec[i].fave
         << " " << std::setw(osw) << analvec[i].fsig
         << " " << std::setw(osw) << ::fabs(analvec[i].step/analvec[i].sigma)
         << ratmsg << sigmsg << fmpmsg << wtmsg
         << std::flush;

      // ---------------------- do the tests ----------------------
      // test 1a. ratio must be > minratio(2)
      if(::fabs(analvec[i].step/analvec[i].sigma) <= minratio) {
         if(debug) std::cout << " small ratio" << std::flush;
         analvec[i].score = -3;                                   // failure
         analvec[i].msg = " small_ratio";
      }

      // test 1b. step must be > 0.8
      else if(::fabs(analvec[i].step) < minstep) {
         if(debug) std::cout << " small step" << std::flush;
         analvec[i].score = -2;                                   // failure
         analvec[i].msg = " small_step";
      }
      
      // its too early - before we can compute score
      // usually ratio|step will be small, so not reach here
      else if(i==0) {
         if(debug) std::cout << " begin" << std::flush;
         analvec[i].score = -1;                                   // failure
         analvec[i].msg = " i=0_no_tests";
      }

      // approaching the end
      else if(i==analvec.size()-1) {
         if(debug) std::cout << " end" << std::flush;
         analvec[i].score = -1;                                   // failure
         analvec[i].msg = " i=end_no_tests";
      }

      // test 1c. exclude case where step AND ratio are very close to limit
      else if(::fabs(analvec[i].step/analvec[i].sigma) / minratio
         + ::fabs(analvec[i].step) / minstep - 2. < minmargin) {
         if(debug) std::cout << " marginal" << std::flush;
         analvec[i].score = -4;                                   // failure
         analvec[i].msg = " marginal_step+ratio";
      }

      // test 2. ratio is a local max
      // test 3. sigma is a local min
      // test 4. fsig > psig before and psig > fsig after
      else if(!rmax || !smin || !fmp) {                           // maybe a slip
         std::string msg;
         if(!rmax) {
            msg = "; no-ratio-max";
            analvec[i].msg += msg + ratmsg;
            if(debug) std::cout << msg;
         }
         if(!smin) {
            msg = "; no-sig-min";
            analvec[i].msg += msg + sigmsg;
            if(debug) std::cout << msg;
         }
         if(!fmp) {
            msg = "; no-f-p";
            analvec[i].msg += msg + fmpmsg;
            if(debug) std::cout << msg;
         }
         analvec[i].score = int(100.*weight+0.5);
         analvec[i].msg += wtmsg;
         if(debug) std::cout << std::flush;
      }

      else {                                                      // its a slip
         analvec[i].msg = ";" + ratmsg + ";" + sigmsg + ";" + fmpmsg + wtmsg;
         analvec[i].score = int(100.*weight+0.5);
         results[curr].ngood--;
         results[curr].npts = analvec[i].index - results[curr].index;
         FilterHit<T> fe;
         fe.type = FilterHit<T>::slip;
         fe.index = analvec[i].index;
         fe.ngood = 1;
         fe.step = analvec[i].step;
         fe.sigma = analvec[i].sigma;
         fe.score = analvec[i].score;
         fe.msg = analvec[i].msg;
         results.push_back(fe); curr++;
      }

      if(!rmax || !smin || !fmp || analvec[i].score == -4) {   // maybe a slip
         // save the "almost slip" - TD add flag to turn this on
         FilterNearMiss<T> fnm;
         fnm.index = analvec[i].index;
         fnm.step = analvec[i].step;
         fnm.sigma = analvec[i].sigma;
         fnm.score = analvec[i].score;
         fnm.msg = analvec[i].msg;
         maybes.push_back(fnm);
      }

      // also see several lines above
      if(debug) std::cout << " " << analvec[i].msg << std::endl;

   }  // end loop over analvec array

   // define npts for the last segment
   results[curr].npts = analvec[analvec.size()-1].index - results[curr].index + 1;

   return (results.size());

}  // end WindowFilter::analyze()

//------------------------------------------------------------------------------------
template<class T> void WindowFilter<T>::dump(std::ostream& os, std::string tag)
{
   size_t i,j,k;
   std::string msg(tag), slip, res;

   // TD print slope?
   os << "#" << msg << " WindowFilter::dump() with " << (twoSample ? "two":"one")
      << "-sample stats, minStep "
      << std::fixed << std::setprecision(osp) << getMinStep() << " minRatio "
      << getMinRatio() << " width " << width << " btwn-buff " << buffsize
      << (noxdata ? " (xdata is index)" : "")
      << std::endl;
   os << "#" << msg
      //<< " i xdata data  step sigma  pN pave psig  fN fave fsig  ratio slope ("
      << " i xdata data  step sigma  pN pave psig  fN fave fsig  ratio ("
      << (balanced?"":"not ") << "balanced, "
      << (twoSample?"two":"one") << "-sample stats)"
      << std::endl;
   
   // loop
   for(i=0,j=0,k=0; i<data.size(); i++) {
      if(j >= analvec.size() || i != analvec[j].index) {
         if(dumpNA) {
            os << msg << std::fixed << std::setprecision(osp)
               << " " << std::setw(3) << i
               << " " << std::setw(osw) << (noxdata ? T(i) : xdata[i])
               //<< " " << std::setw(3) << (noflags ? 0 : flags[i])
               << " " << std::setw(osw) << data[i]
               << " " << std::setw(osw) << "--"
               << " " << std::setw(osw) << "--"
               << " " << std::setw(3) << 0
               << " " << std::setw(osw) << "--"
               << " " << std::setw(osw) << "--"
               << " " << std::setw(3) << 0
               << " " << std::setw(osw) << "--"
               << " " << std::setw(osw) << "--"
               << " " << std::setw(osw) << "--";
            if(dumpAmsg) os << " no analysis";
            os << std::endl;
         }
      }
      else {
         slip = res = "";
         if(analvec[j].score > 0) {
            if(analvec[j].score == 100) slip = "";
            else if(dumpAmsg) slip = " maybe";
            if(dumpAmsg) {
               std::stringstream ss;
               ss << " score:" << analvec[j].score;
               slip += ss.str();
            }
         }

         if(k < results.size() && i == results[k].index) {
            res = " " + (results[k].haveStats ?
                           results[k].asStatsString(osp) : results[k].asString());
            k++;
         }

         os << msg << std::fixed << std::setprecision(osp)
            << " " << std::setw(3) << i
            << " " << std::setw(osw) << (noxdata ? T(i) : xdata[i])
            //<< " " << std::setw(3) << (noflags ? 0 : flags[i])
            << " " << std::setw(osw) << data[i]
            << " " << std::setw(osw) << analvec[j].step
            << " " << std::setw(osw) << analvec[j].sigma
            << " " << std::setw(3) << analvec[j].pN         // past
            << " " << std::setw(osw) << analvec[j].pave
            << " " << std::setw(osw) << analvec[j].psig
            << " " << std::setw(3) << analvec[j].fN         // future
            << " " << std::setw(osw) << analvec[j].fave
            << " " << std::setw(osw) << analvec[j].fsig
                                                            // ratio:
            << " " << std::setw(osw) << ::fabs(analvec[j].step/analvec[j].sigma)

            // results(stats) string, slip string, analysis message
            << res << slip << (dumpAmsg ? analvec[j].msg : "") << std::endl;
         j++;
      }
   }
}  // end WindowFilter::dump()

//------------------------------------------------------------------------------------
// compute stats on the filter quanitites within the given FilterHit.
// return the min, max, median and mad of sigma, the rss(future and past stddev).
// if skip is true (default), exclude data within the filter width of the
// end points, to avoid the bump(s) due to slip(s) at the FilterHit boundaries.
template<class T>
void WindowFilter<T>::getStats(FilterHit<T>& sg, bool skip)
{
   unsigned int i;
   sg.min = sg.max = sg.med = sg.mad = T(0);

   int j(-1);
   for(i=0; i<analvec.size(); i++)
      if(analvec[i].index == sg.index) { j=i; break; }
   if(j == -1) return;

   // stats on sigma       // TD would like the same for step....how to implement
   bool first(true);
   T sd;
   std::vector<T> sdv;
   for(i=0; i<sg.npts; i++) {
      if(skip) {
         if(i < width && sg.type != FilterHit<T>::outlier) continue;
         if(i > sg.npts-width) continue;
      }
      sd = analvec[j+i].sigma;                  // TD version for step?
      if(first) { sg.min = sg.max = sg.med = sd; first = false; }
      else { if(sd < sg.min) sg.min=sd; if(sd > sg.max) sg.max=sd; }
      sdv.push_back(sd);
   }

   i = sdv.size();
   if(i < 2) return;       // else MAD throws
   sg.mad = gpstk::Robust::MedianAbsoluteDeviation<T>(&sdv[0], i, sg.med, false);
   sg.min = sg.max = 0.0;        // TD get min and max
   sg.haveStats = true;
}

// end template <class T> class WindowFilter

