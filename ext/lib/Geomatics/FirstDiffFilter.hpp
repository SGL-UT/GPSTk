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

/// @file FirstDiffFilter.hpp

///    This class implements a statistical filter that uses first-differences. There
/// are several statistical filters implemented as classes. These classes are
/// templates; the template parameter should be a float (probably double);
/// it is used to construct gpstk::Stats<T>, gpstk::TwoSampleStats<T> and
/// gpstk::SeqStats<T>, which are fundamental to these algorithms.
///    All the filters look for outliers and discontinuities (slips) in a timeseries.
/// The first difference filter analyses the simple first difference of the data.
/// The window filter uses a 2-pane sliding window centered on the data point in
/// question; statistics on the data in each to the 2 panes are computed and used
/// in the analysis.
///    All filters' analysis() function computes a vector of FilterHit objects named
/// 'results'that give the caller the results of the filtering.
///    All filters have a getStats(FilterHit) function that computes statistics on
/// the filter quantities (NOT the data) over the interval covered by the event, and
/// stores them in the FilterHit. These stats are slightly different for the two
/// filters; FirstDiffFilter::getStats computes min, max, median and mad of the first
/// differences.
///
///    The structure of these filters allows the caller to call filters repeatedly,
/// and to call different filters on the same dataset, because none of the filters
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

#ifndef FIRST_DIFF_FILTER_INCLUDE
#define FIRST_DIFF_FILTER_INCLUDE

#include <vector>
#include "Stats.hpp"
#include "RobustStats.hpp"
//#include "StringUtils.hpp"       // TEMP
//#include "logstream.hpp"         // TEMP

#include "StatsFilterHit.hpp"

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/// This class computes the first difference of the input data array. It will read the
/// data, compute the first difference and store it, then analyze the result,
/// returning a vector of simple results giving outliers and statistics on the data
/// between outliers.
template <class T> class FirstDiffFilter
{
public:
   /// class used to store analysis arrays filled by the first difference filter
   class Analysis
   {
   public:
      /// constructor
      Analysis() { }
      // member data
      unsigned int index;  ///< index in original arrays to which this info applies.
      T diff;              ///< first difference = data[index]-data[index-1]
      T aveN,sigN;         ///< stats from N pts ending at index (fstats at index)
                              // pstats for index+N+1
   }; // end class Analysis

   /// constructor with two arrays - x is used only in dump(); x and f must exist
   /// but may be empty
   /// @param x const vector<T> of 'times' values, NB (x,d,f) all parallel
   /// @param d const vector<T> of data values
   /// @param f const vector<int> of flags, 0 means good. this array may be empty.
   FirstDiffFilter(const std::vector<T>& x,
                   const std::vector<T>& d,
                   const std::vector<int>& f) : data(d), xdata(x), flags(f)
   {
      fdlimit = T(8.0);
      noxdata = (xdata.size() == 0);
      noflags = (flags.size() == 0);
      dumpNA = true;
      osw = 8; osp = 3;
   }

   /// get and set
   inline void setLimit(T val) { fdlimit = val; }
   inline T getLimit(void) { return fdlimit; }
   /// get and set for dump
   inline void setw(int w) { osw=w; }
   inline void setprecision(int p) { osp=p; }
   inline void setDumpNoAnal(bool b) { dumpNA = b; }
   inline bool willDumpNoAnal(void) { return dumpNA; }
   /// return a copy of the filter hit results
   std::vector< FilterHit<T> > getResults(void) { return results; }

   /// filter routine that computes the first difference
   /// @param index in data arrays at which to start, defaults to 0
   /// @param npts  number of data to process, defaults to -1, meaning to end of data.
   /// @return > 0 number of points in the analysis vector == number good data
   ///          -1 the data array is too short (< 2)
   ///          -3 if the flags array is given but shorter than data array
   /// This routine clears the analysis vector.
   int filter(const size_t index=0, int npts=-1);

   /// analyze the output of the filter(), filling the analysis array 'analvec'
   /// also fills the 'results' vector of one or more FilterHit.
   /// @return the number of slips, outliers, etc (size of results vector<FilterHit>)
   int analyze(void);

   /// analyze the output of the filter(), filling the analysis array 'analvec'
   /// also fills the 'results' vector of one or more FilterHit.
   /// Uses a different algorithm - sliding window ave/sigma of fdiff on either side
   /// of, vs fdiff at, point of interest.
   /// @param ratlim limit on |ratio fdiff to <sigmas>| (also fdiff > fdlimit for hit)
   /// @return the number of slips (size of results vector<FilterHit> - 1)
   int analyze2(const double ratlim, const double siglim)
      { std::string msg; return analyze2(ratlim, siglim, false, msg); }

   /// Overloaded version with analysis dump to string reference; cf. other versions
   /// @param ratlim limit on |ratio fdiff to <sigmas>| (also fdiff > fdlimit for hit)
   /// @param dumpmsg string for analysis dump when bool dump is true
   int analyze2(const double ratlim, const double siglim, std::string& dumpmsg)
      { return analyze2(ratlim, siglim, true, dumpmsg); }

   /// fix some potential problems in the results vector, namely:
   /// if the first point(s) are outliers, analyze() makes the first FilterHit BOD
   /// with only 1 point, then either outliers or a slip; fix this by making first
   /// FilterHit outliers. Called at end of analyze()
   void fixUpResults(void);

   /// dump the data and analysis; optionally include a tag at the start of each line,
   /// and configure width and precision.
   void dump(std::ostream& s, std::string tag=std::string());

   // NB there should be another routine (outside this class) to get stats on the
   // data within a segment
   /// compute stats on the first differences within the given FilterHit, store in Seg
   /// NB this must be called on filter.results[i] in order to show stats in dump()
   void getStats(FilterHit<T>& fe);

   // member data
   int osw,osp;                  ///< width and precision for dump(), (default 8,3)
   bool noxdata;                 ///< true when xdata is not given
   bool noflags;                 ///< true when flags array is not given
   bool dumpNA;                  ///< if false, don't dump() data with no analysis (T)

   T fdlimit;                    ///< |first diff| must be > this to be an outlier
   unsigned int Npts;            ///< size of sliding window
   const std::vector<T>& xdata;  ///< reference to xdata - used only in dump
   const std::vector<T>& data;   ///< reference to data to be filtered
   const std::vector<int>& flags;///< reference to flags, parallel to data, 0 == good
   int ilimit;                   ///< largest allowed index in data[] is ilimit-1

   /// vector of Analysis objects, holding first differences and indexes,
   /// generated by filter(), used by analyze() and included in dump() output.
   std::vector<Analysis> analvec;

   /// vector of FilterHit, generated by analyze(), also for use in dump()
   std::vector< FilterHit<T> > results;

private:
   /// private version of analyze2 with all arguments, cf. overloaded versions' doc.
   /// @param ratlim limit on |ratio fdiff to <sigmas>| (also fdiff > fdlimit for hit)
   /// @param dump bool if true dump analysis output to dumpmsg
   /// @param dumpmsg string for analysis dump when bool dump is true
   int analyze2(const double ratlim, const double siglim,
                  bool dump, std::string& dumpmsg);

}; // end class FirstDiffFilter

//------------------------------------------------------------------------------------
template<class T> int FirstDiffFilter<T>::filter(const size_t i0, int dsize)
{
   if(dsize == -1) dsize = data.size()-i0;

   // largest allowed index is ilimit-1
   ilimit = dsize+i0;

   // is there enough data? if not return -1
   int i(i0),n(0);
   if(!noflags) {
      while(i<ilimit && n < 2) { if(flags[i]==0) n++; i++; }
      if(i==ilimit) return -1;
   }
   else if(dsize < 2) return -1;

   // if xdata or flags is there, make sure its big enough
   if(!noflags && flags.size()-i0 < dsize) return -3;

   analvec.clear();

   // find the first good point, but don't necessarily increment
   i = i0; if(!noflags) while(i<ilimit && flags[i]) i++;
   int iprev(-1);
   while(i<ilimit) {
      Analysis A;
      A.index = i;
      A.diff = (iprev == -1 ? T(0) : data[i]-data[iprev]);
      analvec.push_back(A);
      iprev = i;
      i++; if(!noflags) while(i<ilimit && flags[i]) i++;
   }

   return analvec.size();

}  // end FirstDiffFilter::filter()

//------------------------------------------------------------------------------------
/// Analyse the first difference data, looking for slips, outliers and gaps and
/// computing statistics. Return number of elements in results vector (FilterHit)
template<class T> int FirstDiffFilter<T>::analyze(void)
{
   bool prevIsBad(false);
   int i,igood(0),nbad(0);
   T sumbad(0);

   // create first event = BOD
   results.clear();
   {
      FilterHit<T> fe;
      fe.index = analvec[0].index;
      fe.type = FilterHit<T>::BOD;
      fe.ngood = 0;
      results.push_back(fe);
   }
   int curr(0);

   // loop over first differences
   for(i=0; i<analvec.size(); i++) {
      results[curr].ngood++;               // count it; only good gets into analvec

      // NB analvec[0].diff == 0 always
      if(::fabs(analvec[i].diff) > fdlimit) {
         nbad++;
         sumbad += analvec[i].diff;
         prevIsBad = true;
         //LOG(INFO) << "bad index " << analvec[i].index << " nbad " << nbad;
      }
      else if(!prevIsBad) {               // good 1st diff following good 1st diff
         //LOG(INFO) << "good index " << analvec[i].index;
         igood = i;
      }
      else {                              // good 1st diff following bad one(s)
         //LOG(INFO) << "good after bad index "<< analvec[i].index<< " nbad " << nbad;
         results[curr].ngood -= nbad+1;   // finish the current segment

         if(::fabs(sumbad) > fdlimit) {   // its a slip
            //LOG(INFO) << "slip " << sumbad << " > " << fdlimit;
            // if nbad > 1, must report outlier(s) first
            FilterHit<T> fe;
            if(nbad > 1) {
               results[curr].npts = analvec[igood+1].index - results[curr].index;
               fe.index = analvec[igood+1].index;
               fe.type = FilterHit<T>::outlier;
               fe.ngood = 0;
               fe.npts = analvec[i].index - fe.index;
               results.push_back(fe); curr++;
            }
            results[curr].npts = analvec[igood+nbad].index - results[curr].index;
            // start the new segment (slip)
            fe.index = analvec[igood+nbad].index;
            fe.type = FilterHit<T>::slip;
            fe.ngood = 2;                 // bc this is 2nd point into slip
            fe.step = data[analvec[igood+nbad].index]-data[analvec[igood].index];
            results.push_back(fe); curr++;
         }
         else {                           // its just outliers(s)
            //LOG(INFO) << "outliers " << sumbad << " <= " << fdlimit;
            results[curr].npts = analvec[igood+1].index - results[curr].index;
            // report the outliers
            FilterHit<T> fe;
            fe.index = analvec[igood+1].index;
            fe.type = FilterHit<T>::outlier;
            fe.ngood = 0;
            fe.npts = analvec[igood+nbad].index - fe.index;
            fe.step = sumbad;
            results.push_back(fe); curr++;
            // start the new segment (BOD)
            fe.index = analvec[igood+nbad].index;
            fe.type = FilterHit<T>::BOD;
            fe.ngood = 2;
            results.push_back(fe); curr++;
         }

         // prep for next segment
         sumbad = T(0);
         nbad = 0;
         igood = i;
         prevIsBad = false;
      }
   }  // end loop over first differences

   // finish off with outlier(s)
   if(prevIsBad) {
      results[curr].ngood -= nbad;
      FilterHit<T> fe;
      fe.index = analvec[igood+1].index;
      fe.type = FilterHit<T>::outlier;
      fe.ngood = 0;
      results[curr].npts = fe.index - results[curr].index;
      fe.npts = ilimit - fe.index;
      results.push_back(fe); curr++;
   }
   else // define npts for the last segment
      results[curr].npts = ilimit - results[curr].index;

   fixUpResults();

   return (results.size());

}  // end FirstDiffFilter::analyze()

//------------------------------------------------------------------------------------
// fix some potential problems in the results vector, namely:
// if the first point(s) are outliers, analyze() makes the first FilterHit BOD
// with only 1 point (because the first point is good *by definition*),
// then either outliers or a slip; fix this by making first
// FilterHit outliers.
template<class T> void FirstDiffFilter<T>::fixUpResults(void)
{
   if(results[0].npts > 1) return;

   // change the first FilterHit to outliers
   results[0].type = FilterHit<T>::outlier;

   while(results.size() > 1) {
      if(results[1].type == FilterHit<T>::slip) {
         results[1].type = FilterHit<T>::BOD;
         break;
      }
      else if(results[1].type == FilterHit<T>::outlier) {
         results[0].npts += results[1].npts;
         results[0].ngood = 0;
         results.erase(results.begin()+1);
      }
      else break;
   }
}

//------------------------------------------------------------------------------------
template<class T> int FirstDiffFilter<T>::analyze2(
                                    const double ratlim, const double siglim,
                                    bool dump, std::string& dumpmsg)
{
   const unsigned int N(4);
   unsigned int i,j;
   std::ostringstream oss;
   gpstk::Stats<double> pstats,fstats;       // TD? TwoSampleStats

   if(dump) oss << "FirstDiff analyze2" << std::fixed << std::setprecision(3)
         << " fdlimit=" << fdlimit << " siglim=" << siglim << " ratlim=" << ratlim
         << "\n# index xdata data diff step5  ave sig7 rat8  "
            << "pave psig prat  fave fsig frat [SLIP] [gap]"
         << std::endl;

   // create first event = BOD
   results.clear();
   {
      FilterHit<T> fe;
      fe.index = analvec[0].index;
      fe.type = FilterHit<T>::BOD;
      fe.ngood = 0;
      results.push_back(fe);
   }
   int curr(0), i0(1);
   double tstep,tstepmin,tstepmax(0.0);
   tstepmin = (noxdata ? 0 : xdata[analvec[1].index] - xdata[analvec[0].index]);
   const unsigned int size(analvec.size());

   //// TEST
   //std::cout << "TST size is " << size << std::endl;
   //try {
   //for(i=i0; i<size+N-1; i++) {                   // i=index=1,...  j=count=0,...
   //   j=i-i0+1;                                 // NB i0 is changed when hit found
   //   if(i<size) fstats.Add(analvec[i].diff);
   //   if(j>N) fstats.Subtract(analvec[i-N].diff);
   //   if(j>N+1) pstats.Add(analvec[i-N-1].diff);
   //   if(j>2*N+1) pstats.Subtract(analvec[i-2*N-1].diff);

   //   std::cout << "TST " << std::setw(4) << i << std::fixed<< std::setprecision(3);
   //   if(j>N+2) std::cout
   //      << " P " << std::setw(3) << pstats.N()
   //      << " " << std::setw(8) << pstats.Average()
   //      << " " << std::setw(8) << pstats.StdDev();
   //   else std::cout << " P NA        NA       NA";
   //   if(j>N) std::cout << " T " << std::setw(8) << analvec[i-N].diff;
   //   else std::cout << " T " << std::setw(8) << "NA";
   //   if(j>1 && fstats.N()>2) std::cout
   //      << " F " << std::setw(3) << fstats.N()
   //      << " " << std::setw(8) << fstats.Average()
   //      << " " << std::setw(8) << fstats.StdDev();
   //   else std::cout << " F NA        NA       NA";
   //   //if(i<size) std::cout << " D " << std::setw(8) << analvec[i].diff;
   //   //else std::cout << " D " << std::setw(8) << "NA";
   //   std::cout << std::endl;
   //}
   //} catch(gpstk::Exception& e) { std::cout << "TST " << e.what() << std::endl; }
   //fstats.Reset();
   //pstats.Reset();
   //// END TEST

   // loop over first differences
   // keep stats on the N points before, and N points after, the current point
   // NB skip the first pt b/c diff==0 and it means nothing
   for(j=0,i=i0; i<size+N-1; i++) {       // NB i0 changes when hit
      results[curr].ngood++;              // count it; only good gets into analvec

      //std::cout << "DAT " << i << std::fixed << std::setprecision(3)
      //         << " " << xdata[analvec[i].index]
      //         << " " << data[analvec[i].index]
      //         << " " << analvec[i].diff
      //         << std::endl;

      j++;                                // count points added
      if(i<size) {
         fstats.Add(analvec[i].diff);
         analvec[i].aveN = fstats.Average();
         analvec[i].sigN = fstats.StdDev();
      }
      if(j>N) fstats.Subtract(analvec[i-N].diff);
      if(j>N+1) pstats.Add(analvec[i-N-1].diff);
      if(j>2*N+1) pstats.Subtract(analvec[i-2*N-1].diff);

      if(j>N+2 && pstats.N()>2) {
         double diff(analvec[i-N].diff);
         double pa(pstats.Average()), fa(fstats.Average());
         double ps(pstats.StdDev()), fs(fstats.StdDev());
         double avefd((pa+fa)/2.0);                // fdiff ave of past and future
         double step(diff-avefd);                  // fd minus average fd
         if(pstats.N()<2) { pa=0.0; ps=0.0; }
         double sig(::sqrt(pstats.N()*ps*ps+fstats.N()*fs*fs));
         double pr(::fabs(diff-pa)/ps), fr(::fabs(diff-fa)/fs);// ratios step/sigma
         double rat((pr+fr)/2.0);
         //double rat(step/sig);

         // criteria for finding a slip
         bool hitslip(::fabs(step) > fdlimit    // step is big
                         && sig < siglim        // noise is not large
                         && rat > ratlim);      // ratio step/sig large
         bool hisig(sig > fdlimit);             // sigma is high - bigger than steplim

         tstep = xdata[analvec[i-N-1].index] - xdata[analvec[i-N-2].index];
         if(tstep > tstepmax) tstepmax = tstep;
         if(tstep < tstepmin) tstepmin = tstep;

         if(dump) {
   //<< "\n# index  xdata  data  diff  step5 ave  sig7 rat8 pave psig prat fave fsig frat [SLIP] [gap]"
            oss << i << std::fixed << std::setprecision(3)
               << " " << (noxdata?T(analvec[i-N-1].index):xdata[analvec[i-N-1].index])
               << " " << data[analvec[i-N-1].index]
               << " " << diff << " " << step
               << "  " << avefd << " " << sig << " " << rat
               << "  " << pa << " " << ps << " " << pr
               << "  " << fa << " " << fs << " " << fr
               << (hitslip ? " SLIP":"")
               << (hisig ? " SIG":"");
            if(!noxdata && tstep/tstepmin - 1 > 5)
               oss << " gap(" << tstepmin << "<=" << tstep << "<=" << tstepmax << ")";
            oss << (::fabs(step) > fdlimit ? " step":"")
                << (sig < siglim ? " sig":"")
                << (rat > ratlim ? " rat":"")
                << (pr > ratlim ? " prat":"")
                << (fr > ratlim ? " frat":"")
                ;
            oss << std::endl;
         }

         if(hitslip) {
            results[curr].ngood--;
            results[curr].npts = analvec[i].index - analvec[i0].index;
            results[curr].sigma = ps;
            i0 = i;
            FilterHit<T> fe;
            fe.index = analvec[i-N-1].index;
            fe.type = FilterHit<T>::slip;
            fe.ngood = 1;
            fe.step = diff-(pa+fa)/2.0;       // ave of past and future
            fe.sigma = fs;
            results.push_back(fe); curr++;
         }
      }
   }  // end loop over first differences

   results[curr].npts = analvec[size-1].index - analvec[i0].index + 1;

   if(dump) dumpmsg = oss.str();

   return int(results.size()-1);       // always first result = BOD
}

//------------------------------------------------------------------------------------
template<class T> void FirstDiffFilter<T>::dump(std::ostream& os, std::string tag)
{
   size_t i,j,k;
   os << "#" << tag << " FirstDiffFilter::dump() with limit "
      << std::fixed << std::setprecision(osp) << fdlimit
      << (noxdata ? " (xdata is index)" : "")
      << "\n#" << tag << "  i    xdata   data    1stdiff" << std::endl;

   const size_t N(analvec.size());
   for(i=0,j=0,k=0; i<ilimit; i++) {
      if(j >= N || i != analvec[j].index) {
         if(dumpNA) os << tag << std::fixed << std::setprecision(osp)
            << " " << std::setw(3) << i
            << " " << std::setw(osw) << (noxdata ? T(i) : xdata[i])
            //<< " " << std::setw(3) << (noflags ? 0 : flags[i])
            << " " << std::setw(osw) << data[i]
            << " " << std::setw(osw) << 0.0 << "  NA" << std::endl;
      }
      else {
         os << tag << std::fixed << std::setprecision(osp)
            << " " << std::setw(3) << i
            << " " << std::setw(osw) << (noxdata ? T(i) : xdata[i])
            //<< " " << std::setw(3) << (noflags ? 0 : flags[i])
            << " " << std::setw(osw) << data[i]
            << " " << std::setw(osw) << (j >= N ? 0.0 : analvec[j].diff);
         if(k < results.size() && i == results[k].index) {
            os << "  " << (results[k].mad != T(0) ?   // has getStats been called?
                           results[k].asStatsString(osp) : results[k].asString());
            k++;
         }
         os << std::endl;
         j++;
      }
   }
}  // end FirstDiffFilter<T>::dump()

//------------------------------------------------------------------------------------
// compute stats on the segment of first differences given by the input FilterHit
template<class T>
void FirstDiffFilter<T>::getStats(FilterHit<T>& fe)
{
   int j(-1);
   unsigned int i,k;
   fe.min = fe.max = fe.med = fe.mad = T(0);
   for(i=0; i<analvec.size(); i++)
      if(analvec[i].index == fe.index) { j=i; break; }
   if(j == -1) return;
   k = fe.index + fe.npts;                // last index in this seg is k-1

   // don't include the step in stats for a segment that starts with a slip
   int i0(0); if(fe.type == FilterHit<T>::slip) i0=1;
   // stats on first difference
   bool first(true);
   T fd;
   std::vector<T> fdv;
   for(i=i0; i<fe.npts; i++) {
      if((unsigned int)(j)+i >= analvec.size() || analvec[j+i].index >= k)
         break;  // no more good data
      fd = analvec[j+i].diff;
      if(first) {
         fe.min=fe.max=fe.med=fd;
         fe.mad=T(0);
         first=false;
      }
      else {
         if(fd < fe.min) fe.min=fd;
         if(fd > fe.max) fe.max=fd;
      }
      fdv.push_back(fd);
   }

   if(fdv.size() < 2) return;       // else MAD throws

   fe.mad=gpstk::Robust::MedianAbsoluteDeviation<T>(&fdv[0],fdv.size(),fe.med,false);
   fe.haveStats = true;
}

// end template <class T> class FirstDiffFilter

//------------------------------------------------------------------------------------
#endif // define FIRST_DIFF_FILTER_INCLUDE
