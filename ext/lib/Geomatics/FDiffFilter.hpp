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

/// @file FDiffFilter.hpp

///    This class implements a statistical filter that uses first-differences, like
/// FirstDiffFilter, but this is simpler, more effective and more robust, at least
/// for single differenced (across satellites) phase data.
/// Also includes here is a class, IterativeFDiffFilter, that applies the FDiffFilter
/// more than once; this is the most effective way to use this filter.
/// 
/// There are several statistical filters implemented as classes. These classes are
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

#ifndef FDIFF_FILTER_INCLUDE
#define FDIFF_FILTER_INCLUDE

#include "Stats.hpp"
#include "StatsFilterHit.hpp"
#include "RobustStats.hpp"
#include "StringUtils.hpp"
//#include "stl_helpers.hpp"

#include <vector>

//------------------------------------------------------------------------------------
// TODO
// ?make first result BOD?

//------------------------------------------------------------------------------------
// forward declaration
template <class T> class IterativeFDiffFilter;

//------------------------------------------------------------------------------------
/// This class computes the first difference of the input data array at each point,
/// as well as stats on both the first difference and the data for the Nwind points
/// ending at the current point. The difference, sigmaYX of the difference statistics,
/// and the slope from the data statistics, are stored in an "analysis vector" Avec.
/// The Avec is used to find outliers, gaps and slips in the data; the analysis()
/// method returns a vector of "result" objects FilterHit<T>.
/// The caller should filter and analyze iteratively, since a single call is often
/// not sufficient to find and fix all the problems.
/// NB Flags and xdata are required and critical. Outliers must be flagged or
/// removed between iterations, and xdata is needed for gaps and to correct slip
/// magnitudes for the slope of the data.
/// Probably the major weakness of this filter is it tends to find false slips
/// after large gaps.
template <class T> class FDiffFilter
{
private:
   friend class IterativeFDiffFilter<T>;

   // embedded class -----------------------------------------
   /// class used to store analysis arrays filled by the first difference filter
   class Analysis
   {
   public:
      /// constructor
      Analysis() { index=0; sloInd=0; diff = sigN = T(0); }
      // member data
      unsigned int index;  ///< index in original arrays to which this info applies.
      T diff;              ///< first difference = data[index]-data[index-1]
      T sigN;              ///< sigmaYX for N pts of first diff ending at index
      T sloN;              ///< 2-sample slope of data for N pts ending at index
      int sloInd;          ///< index in Avec of slope used in current calc. of diff
   }; // end class Analysis

   /// vector of Analysis objects, holding first differences and indexes,
   /// generated by filter(), used by analyze() and included in dump() output.
   std::vector<Analysis> Avec;

   /// vector of results of analysis()
   std::vector< FilterHit<T> > results;

   // member data ---------------------------------------
   int osw,osp;                  ///< width and precision for dump(), (default 8,3)
   bool noxdata;                 ///< true when xdata is not given
   bool noflags;                 ///< true when flags array is not given

   const std::vector<T>& xdata;  ///< reference to xdata - 'time'; if empty use count
   const std::vector<T>& data;   ///< reference to data to be filtered
   const std::vector<int>& flags;///< reference to flags, parallel to data, 0 == good
   int ilimit;                   ///< largest allowed index in data[] is ilimit-1

   T fdlim;                      ///< |first diff| must be > this to be outlier/slip
   T siglim;                     ///< sigma > this indicates possible outlier/slip
   T Esiglim;                    ///< compute siglim (= robust outlier limit)
   unsigned int Nwind;           ///< size of sliding window
   unsigned int Nsig;            ///< filter()'s count of high sigma - analysis needed
   bool doSmall;                 ///< if true, include small slips (<fdlim) in results
   bool keepSigIndex;            ///< if true, keep vector of HighSigmaIndex
   std::vector<unsigned int> sigIndexes; ///< saved indexes of Nsig high sigma pts

   T medSlope, madSlope;         ///< robust stats on slope

public:
   // member functions ---------------------------------------
   /// constructor with two arrays - x is used only in dump(); x and f must exist
   /// but may be empty
   /// @param x const vector<T> of 'times' values, NB (x,d,f) all parallel
   /// @param d const vector<T> of data values
   /// @param f const vector<int> of flags, 0 means good. this array may be empty.
   FDiffFilter(const std::vector<T>& x,
               const std::vector<T>& d,
               const std::vector<int>& f) : data(d), xdata(x), flags(f)
   {
      fdlim = T(0.8);
      siglim = T(0.3);
      Esiglim = T(0);
      noxdata = (xdata.size() == 0);
      noflags = (flags.size() == 0);
      osw = 8; osp = 3;
      Nsig = 0;
      doSmall = true;
      keepSigIndex = false;
   }

   /// get and set
   inline void setWidth(unsigned int w) { Nwind = w; }
   inline unsigned int getWidth(void) { return Nwind; }
   inline void setLimit(T val) { fdlim = val; }
   inline T getLimit(void) { return fdlim; }
   inline void setSigma(T val) { siglim = val; }
   inline T getSigma(void) { return siglim; }
   inline bool doSmallSlips(const bool& doit) { doSmall = doit; return doit; }
   inline bool doSmallSlips(void) { return doSmall; }
   inline bool indexHighSigmas(const bool& doit) { keepSigIndex = doit; return doit; }
   inline bool indexingHighSigmas(void) { return keepSigIndex; }
   /// get and set for dump
   inline void setw(int w) { osw=w; }
   inline void setprecision(int p) { osp=p; }
   /// get results vector of FilterHit
   inline std::vector< FilterHit<T> > getResults(void) { return results; }

   /// get the number of high-sigma points :
   /// after filter() can tell if possible outliers/slips present
   inline unsigned int getNhighSigma(void) { return Nsig; }

   /// get the indexes of high-sigma points (only if indexHighSigmas(true))
   inline std::vector<unsigned int> getHighSigmaIndex(void) { return sigIndexes; }

   /// filter routine that computes the first difference, and uses siglim to mark
   /// areas of possible outlier/slips.
   /// @param index in data arrays at which to start, defaults to 0
   /// @param npts  number of data to process, defaults to -1, meaning to end of data.
   /// @return > 0 number of points in the analysis vector == number good data
   ///          -1 the data array is too short (< 2)
   ///          -3 if the flags array is given but shorter than data array
   /// This routine clears the analysis vector.
   int filter(const size_t index=0, int npts=-1);

   /// after filter(), and before analyze(), compute robust stats on the sigma of
   /// first differences, to get a suggested siglim
   /// @param N int output: computed number of sigmas > new limit, -1 not enough data
   /// @param new_siglim T output: computed limit (high outlier) on sigma for analyze
   void ComputeRobustSigmaLimit(int& N, T& new_siglim);

   /// analyze the output of the filter(), filling the results array with outliers
   /// @param return vector of FilterHit
   /// @return the number of outliers
   int analysis(void);

   /// dump the data and analysis; optionally include a tag at the start of each line,
   /// and configure width and precision.
   void dump(std::ostream& s, std::string tag=std::string());

}; // end class FDiffFilter

//------------------------------------------------------------------------------------
template<class T> int FDiffFilter<T>::filter(const size_t i0, int dsize)
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

   // generate the analysis vector
   Avec.clear();

   // compute stats on sigmas and data in a sliding window of width Nwind
   gpstk::TwoSampleStats<T> fstats;       // stats on the first diffs in window
   gpstk::TwoSampleStats<T> dstats;       // stats on the data in window
   std::vector<T> slopes;                 // store slopes, for robust stats

   // loop over all data, computing first difference and stats in sliding window
   Nsig = 0;
   int iprev(-1),j,islope(0);
   // start at the first good point
   n = 0; i = i0; if(!noflags) while(i<ilimit && flags[i]) i++;
   while(i<ilimit) {
      Analysis A;
      A.index = i;
      n++;              // count data points, just for approx slope when n==2

      // add to stats on data (for slope)
      dstats.Add(xdata[i],data[i]);

      // compute first difference, accounting for slope of data
      if(iprev == -1)
         A.diff = T(0);
      else {
         // get approx slope at first pt (often slope==0 here => OUT; index=0)
         if(n==2) Avec[islope].sloN =     // first slope = 2nd slope = d(data)/dx
                  (data[i]-data[iprev])/(xdata[i]-xdata[iprev]);
         // index of latest good slope
         A.sloInd = islope;
         // compute the difference = change in data - correction for slope
         A.diff = data[i]-data[iprev]
                  - Avec[islope].sloN * (xdata[i]-xdata[iprev]);
         // add diff to stats
         fstats.Add(xdata[i], A.diff);
      }

      // remove old data from stats buffers if full
      j = Avec.size()-Nwind;              // index of earliest of the Nwind points
      if(fstats.N() > Nwind)
         fstats.Subtract(xdata[Avec[j].index], Avec[j].diff);
      if(dstats.N() > Nwind)
         dstats.Subtract(xdata[Avec[j].index], data[Avec[j].index]);

      //NO A.sigN = fstats.SigmaYX();     // sigma first diff, given slope in fdiffs
      A.sigN = fstats.StdDevY();          // sigma first diff
      A.sloN = dstats.Slope();            // slope of data
      if(A.sigN > siglim) {
         Nsig++;                          // count it if sigma is high
         if(keepSigIndex) sigIndexes.push_back(i);
      }
      else {
         islope = Avec.size();          // keep this, the most recent good slope
         // keep slopes for roubst stats
         slopes.push_back(A.sloN);
      }

      Avec.push_back(A);
      iprev = i;
      i++; if(!noflags) while(i<ilimit && flags[i]) i++;
   }

   // compute robust stats on slopes
   madSlope = gpstk::Robust::MedianAbsoluteDeviation(&slopes[0], slopes.size(),
                                                      medSlope, false);

   return Avec.size();

}  // end FDiffFilter::filter()

//------------------------------------------------------------------------------------
// after filter(), and before analysis(), compute robust stats on the sigma of
// first differences, to get a suggested siglim
// param new_siglim T computed limit on sigma for analysis == high outlier limit
// return -1 if not enough data, else number of data outside new sigma limit
template<class T> void FDiffFilter<T>::ComputeRobustSigmaLimit(int& N, T& new_siglim)
{
   new_siglim = siglim;
   if(Avec.size() < 2) { N=-1; return; }

   // compute high-outlier limit of sigmas using robust stats
   // compute quartiles - must sort first
   unsigned int i;
   std::vector<T> sd;                                    // put sigmas in temp vector
   for(i=0; i<Avec.size(); i++)
      sd.push_back(Avec[i].sigN);

   T Q1,Q3;
   gpstk::QSort(&sd[0],sd.size());                       // sort
   gpstk::Robust::Quartiles(&sd[0],sd.size(),Q1,Q3);     // get Quartiles

   // compute new sigma limit ; outlier limit (high) 2.5Q3-1.5Q1
   new_siglim = 2.5*Q3 - 1.5*Q1;
   if(new_siglim <= siglim) { N = Nsig; return; }

   // count up hi-sigma points
   for(N=0,i=0; i<Avec.size(); i++)
      if(Avec[i].sigN > new_siglim) N++;
}

//------------------------------------------------------------------------------------
template<class T> int FDiffFilter<T>::analysis(void)
{
   //const bool debug(true);

   // don't clear results - may be a case where caller wants to keep old ones

   // loop over analysis vector
   // outliers have >= Nwind big sigmas
   //      unless its the first point, then there are Nwind-2 bad sigmas
   // slips have exactly Nwind big sigmas
   bool isBad(false);
   int nbad(0),nout,n1st,Nw(Nwind),k;        // DO NOT use unsigned
   unsigned int i,j,bad0;
   for(i=0; i<Avec.size(); i++) {
      //if(debug)
      //   std::cout << "Loop " << i << " " << xdata[Avec[i].index] << std::endl;

      if(Avec[i].sigN > siglim) {
         if(!isBad) { bad0=i; isBad=true;}
         nbad++;
      }

      else if(isBad) {
         nout = int(bad0)+nbad-Nw;
         n1st = int(i)-Nw;
         //if(debug) std::cout << "FDFa: isBad at i " << i
         //      << " index " << Avec[i].index << " x " << xdata[Avec[i].index]
         //      << " bad0 " << bad0 << " nbad " << nbad << " nout " << nout
         //      << " Nwind " << Nw << " 2Nw-2 " << 2*Nw-2 << std::endl;

         if(nout>0 && nbad > Nw) {                  // outliers - more than a slip
            //if(debug) for(j=bad0; j<nout; j++)
            //   std::cout << "FDFa: OUT " << Avec[j].index
            //            << " " << xdata[Avec[j].index] << std::endl;
            j = bad0;
            FilterHit<T> fdfr;
            fdfr.type = FilterHit<T>::outlier;
            fdfr.index = Avec[j].index;
            fdfr.npts = nbad-Nw;
            fdfr.dx = xdata[Avec[int(j)+nbad-Nw].index] - xdata[Avec[j].index];
            results.push_back(fdfr);
         }

         else if(nbad == Nw) {                   // slip
            j = bad0;
            //if(debug) std::cout << "FDFa: SLIP " << Avec[j].index << std::fixed
            //   << " " << xdata[Avec[j].index] << std::setprecision(osp)
            //   << " " << Avec[j].diff << std::endl;
            FilterHit<double> fdfr;
            fdfr.type = FilterHit<T>::slip;
            fdfr.index = Avec[j].index;
            fdfr.sigma = Avec[j].sigN;

            // find number of missing pts before the slip
            k = Avec[j].index-1;                      // j is an Avec[] index
            if(noflags && k>0) k--;                   // NB k is int
            else while(k>0 && flags[k] != 0) k--;     // k is the data[] index
            fdfr.dx = xdata[Avec[j].index]-xdata[k];
            fdfr.npts = Avec[j].index - k;

            // get the step = first difference of data across slip
            fdfr.step = Avec[j].diff;
            // score here is just step/fdlim as a percentage (max 100)
            fdfr.score = (unsigned int)(0.5 + 100.*::fabs(fdfr.step)/fdlim);
            if(fdfr.score > 100) fdfr.score = 100;

            // BUT if step is of order few*MADslope*dx, then probably a false positive
            if(fdfr.score == 100 && ::fabs(fdfr.step) < 3*madSlope*fdfr.dx) {
               //if(debug)
               //std::cout << "FDFa: F+ slip (|diff| < 3*madSlope*dx) at index "
               //<< Avec[j].index << std::fixed << std::setprecision(osp)
               //<< ": " << Avec[j].diff << " < 3 * " << madSlope
               //<< " * " << fdfr.dx << " = " << 3*madSlope*fdfr.dx << std::endl;
               fdfr.score = -100;
            }

            // save the hit
            if(doSmall || fdfr.score == 100) results.push_back(fdfr);
         }

         else if(int(i) <= 2*Nw-2 && n1st >= 0) {   // first <Nwind pts outliers
            //if(debug) std::cout << "FDFa: isBad at small i " << i << std::endl;
            //if(debug) for(int n=0; n<n1st; n++)
            //   std::cout << "FDFa OUT " << Avec[n].index << " "
            //            << xdata[Avec[n].index] << " small i " << i << std::endl;
            FilterHit<double> fdfr;
            fdfr.type = FilterHit<T>::outlier;
            fdfr.index = Avec[0].index;
            fdfr.npts = n1st;
            fdfr.dx = xdata[Avec[n1st].index] - xdata[Avec[0].index];
            results.push_back(fdfr);
         }

         isBad = false;
         nbad = 0;
      }
   }

   // catch outliers at the very end
   if(isBad) {                            // bad at the very end
      //if(debug) for(j=bad0; j<Avec.size(); j++)
      //   std::cout << "FDFa: OUT " << Avec[j].index
      //            << " " << xdata[Avec[j].index] << std::endl;
      j = bad0;
      FilterHit<double> fdfr;
      fdfr.type = FilterHit<T>::outlier;
      fdfr.index = Avec[j].index;
      fdfr.npts = Avec.size()-j;
      fdfr.dx = xdata[Avec[Avec.size()-1].index] - xdata[Avec[j].index];
      results.push_back(fdfr);
   }

   return results.size();

}  // end FDiffFilter::analysis()

//------------------------------------------------------------------------------------
template<class T> void FDiffFilter<T>::dump(std::ostream& os, std::string tag)
{
   size_t i,j,k,iprev;
   int w(osw>5?osw+1:5);
   os << "#" << tag << " FDiffFilter::dump() with limit "
      << std::fixed << std::setprecision(osp) << fdlim << " sigma limit " << siglim
      << std::setprecision(osp+2) << " med,mad slope " << medSlope << " " << madSlope
      << std::setprecision(osp) << (noxdata ? " (xdata is index)" : "")
      << "\n#" << tag << " " << std::setw(2) << "i"
      << " " << std::setw(w) << "xd" << " "
      << std::setw(3) << "flg" << " " << std::setw(w) << "data" << " "
      << std::setw(w) << "fdif" << " " << std::setw(w) << "sig" << " "
      << std::setw(w) << "slope" << " " << std::setw(w) << "slp_u" << " "
      << std::setw(w) << "sl*dx" << " " << std::setw(w) << "slu*dx" << " "
      << std::setw(5) << "dx"
      //<< " " << std::setw(w) << "sigslope*dx"
      << std::endl;

   T dt(0);
   std::string sdif,ssig,sldx,slop,slou,sludx;//,sigslo;
   const size_t N(Avec.size());
   for(i=0,j=0,k=0,iprev=-1; i<ilimit; i++) {
      // find j where Avec[j].index == i
      while(j<N && Avec[j].index < i) j++;
      bool haveAvec(Avec[j].index == i);
      if(haveAvec) {
         sdif = gpstk::StringUtils::asString(Avec[j].diff,osp);
         ssig = gpstk::StringUtils::asString(Avec[j].sigN,osp);
         if(iprev > -1) dt = (noxdata ? T(i-iprev) : xdata[i]-xdata[iprev]);
         slop = gpstk::StringUtils::asString(Avec[j].sloN,osp);
         slou = gpstk::StringUtils::asString(Avec[Avec[j].sloInd].sloN,osp);
         sldx = gpstk::StringUtils::asString(Avec[j].sloN*dt,osp);
         sludx = gpstk::StringUtils::asString(Avec[Avec[j].sloInd].sloN*dt,osp);
         //sigslo = gpstk::StringUtils::asString(madSlope*dt,osp);
      }
      else {
         sdif = ssig = sldx = sludx = slou = "?"; //sigslo = "?";
      }
      os << tag << std::fixed << std::setprecision(osp)
         << " " << std::setw(3) << i
         << " " << std::setw(w) << (noxdata ? T(i) : xdata[i])
         << " " << std::setw(3) << (noflags ? 0 : flags[i])
         << " " << std::setw(w) << data[i]
         << " " << std::setw(w) << sdif
         << " " << std::setw(w) << ssig
         << " " << std::setw(w) << slop
         << " " << std::setw(w) << slou
         << " " << std::setw(w) << sldx
         << " " << std::setw(w) << sludx
         << " " << std::setprecision(2) << std::setw(5) << dt
         //<< " " << std::setw(w) << sigslo
         << (haveAvec && Avec[j].sigN > siglim ? " SIG":"")
         << (haveAvec ? "":" NA");
      if(k < results.size() && haveAvec && i == results[k].index) {
         os << " " << results[k].asString()
            << (results[k].type == FilterHit<T>::slip &&
               results[k].score < 100 ? " SMALL":"");
         k++;
      }
      os << std::endl;
      if(haveAvec) iprev = Avec[j].index;
   }
}  // end FDiffFilter<T>::dump()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/// Just an iteration loop that applies FDiffFilters to the data, rejecting outliers
/// and fixing slips in each iteration.
template <class T> class IterativeFDiffFilter
{
private:
   /// vector of results of the iterative analysis
   std::vector< FilterHit<T> > results;

   // member data ---------------------------------------
   // first those passes to FDiffFilter
   int osw,osp;                  ///< width and precision for dump(), (default 8,3)

   const std::vector<T>& xdata;  ///< reference to xdata - 'time'; if empty use count
   const std::vector<T>& data;   ///< reference to data to be filtered
   const std::vector<int>& flags;///< reference to flags, parallel to data, 0 == good

   T fdlim;                      ///< |first diff| must be > this to be outlier/slip
   T siglim;                     ///< sigma > this indicates possible outlier/slip
   unsigned int Nwind;           ///< size of sliding window
   unsigned int Nsig;            ///< filter()'s count of high sigma - analysis needed
   bool doSmall;                 ///< if true, include small slips (<fdlim) in results

   // and those unique to this class
   unsigned int itermax;         ///< maximum number of itertions (3)
   T Esiglim;                    ///< Estimated sigma limit computed from robust stats
   std::ostream& logstrm;        ///< write to output stream
   bool resetSigma;              ///< if true, set siglim to Esiglim in each iteration
   T siguse;                     ///< sigma limit used in last iteration
   bool keepSigIndex;            ///< if true, keep vector of HighSigmaIndex
   std::vector<unsigned int> sigIndexes; ///< saved indexes of Nsig high sigma pts
   bool verbose;                 ///< output comments and dump FDiffFilters
   std::string label;            ///< put on output lines when verbose

public:
   // member functions ---------------------------------------
   /// Constructor with three parallel arrays, xdata (~time), data, flags (0=good).
   /// Flags f may be empty (size 0), but on output of analysis() it will be filled.
   /// @param x const vector<T> of 'times' values, NB (x,d,f) all parallel
   /// @param d const vector<T> of data values
   /// @param f const vector<int> of flags, 0 means good. this array may be empty.
   IterativeFDiffFilter(const std::vector<T>& x,
                        const std::vector<T>& d,
                        const std::vector<int>& f,
                        std::ostream& os=std::cout)
      : data(d), xdata(x), flags(f), logstrm(os)
   {
      keepSigIndex = resetSigma = verbose = false;
      doSmall = true;
      itermax = 3;
      label = std::string();
      // rest are defaults of FDiffFilter
      FDiffFilter<T> fdf(x,d,f);
      osw = fdf.osw;
      osp = fdf.osp;
   }

   /// get and set
   inline void setWidth(unsigned int w) { Nwind = w; }
   inline unsigned int getWidth(void) { return Nwind; }
   inline void setLimit(T val) { fdlim = val; }
   inline T getLimit(void) { return fdlim; }
   inline void setSigma(T val) { siglim = val; }
   inline T getSigma(void) { return siglim; }
   inline bool doSmallSlips(const bool& doit) { doSmall = doit; return doit; }
   inline bool doSmallSlips(void) { return doSmall; }
   inline bool doResetSigma(const bool& doit) { resetSigma = doit; return doit; }
   inline bool indexHighSigmas(const bool& doit) { keepSigIndex = doit; return doit; }
   inline bool indexingHighSigmas(void) { return keepSigIndex; }
   inline bool doVerbose(const bool& doit) { verbose = doit; return doit; }
   inline void setLabel(const std::string doit) { label = doit; }
   inline std::string getLabel(void) { return label; }

   /// get and set for dump
   inline void setw(int w) { osw=w; }
   inline void setprecision(int p) { osp=p; }

   /// get computed sigma limit (= robust outlier limit) after analysis()
   inline T getEstimatedSigmaLimit(void) { return Esiglim; }

   /// get sigma limit used in last iteration
   inline T getUsedSigmaLimit(void) { return siguse; }

   /// get results vector of FilterHit
   std::vector< FilterHit<T> > getResults(void) { return results; }

   /// get the number of remaining high-sigma points after analysis()
   inline unsigned int getNhighSigma(void) { return Nsig; }

   /// get the indexes of high-sigma points (only if indexHighSigmas(true))
   inline std::vector<unsigned int> getHighSigmaIndex(void) { return sigIndexes; }

   /// Analyze the data using FDiffFilters, optionally computing new sigma outlier
   /// limit, in an interative loop. Best for single differenced phase.
   /// If doResetSigma(true), sigma limit is reduced, if possible, to outlier limit.
   /// Flags array may be empty (size 0), but on output it will be filled.
   /// analyze the output of the filter(), filling the results array with outliers
   /// Return results in Nsig, Esiglim, and vector<FilterHit> results.
   /// @return the number of outliers, or -1 if not enough good data to analyze.
   int analysis(void);

   /// Edit the data for the caller using results created by analysis().
   /// NB data arrays are NOT edited by filter.
   /// NB must pass SAME arrays used in c'tor, after calling analysis(), (not const).
   /// @param data vector<T> of data values
   /// @param flags vector<int> of flags, 0 means good. this array may be empty.
   /// @param doInt if true, integerize the slips before fixing (T)
   /// @param badFlag set flags[] to this int when marking it bad (1)
   /// @return the number of edits (slips + pts rejected)
   int editArrays(std::vector<T>& data, std::vector<int>& flags,
                  const bool doInt=true, const int badFlag=1);

}; // end class IterativeFDiffFilter

//------------------------------------------------------------------------------------
/// Analyze the data using FDiffFilters, optionally computing new sigma outlier limit,
/// in an interative loop. Best for single differenced phase.
template<class T> int IterativeFDiffFilter<T>::analysis(void)
{
   // make sure vectors are sized
   const unsigned int size(xdata.size() < data.size() ? xdata.size() : data.size());
   if(size <= 2) return -1;

   // save all results in each iteration; use results during each iteration
   std::vector< std::vector< FilterHit<T> > > Results;

   // handle input data ---------------------------------
   // use a copy of the data within the iteration loop
   std::vector<T> Tdata(data);
   // copy flags, create if needed
   std::vector<int> Tflags;
   if(flags.size() < size) Tflags = std::vector<int>(size,0);
   else Tflags = std::vector<int>(flags);


   // analysis ------------------------------------------
   unsigned int iter,i,j,k,nr(0);
   siguse = siglim;

   // iterate over (filter / compute new sigma limit / analysis)
   iter = 1;
   while(iter <= itermax) {
      // must redefine filter each time since arrays (const in fdf) change
      FDiffFilter<T> fdf(xdata, Tdata, Tflags);
      fdf.setWidth(Nwind);          // fdf.Nwind
      fdf.setLimit(fdlim);          // fdf.fdlim
      fdf.setSigma(siguse);         // fdf.siglim
      fdf.setprecision(osp);        // fdf.osp
      fdf.setw(osw);                // fdf.osw
      fdf.doSmallSlips(doSmall);    // fdf.doSmall
      fdf.indexHighSigmas(iter==itermax && keepSigIndex);

      // filter the data -----------
      i = fdf.filter();
      //if(verbose) logstrm << "# " << label << " filter returned " << i << std::endl;
      if(i <= 2) { logstrm << "Not enough data, abort.\n"; return -1; }

      if(iter==itermax && keepSigIndex)
         sigIndexes = fdf.getHighSigmaIndex();

      // compute outlier limit from robust stats, and count outliers
      int N;
      fdf.ComputeRobustSigmaLimit(N, Esiglim);
      if(verbose) logstrm << "# " << label << " Estimated sigma limit "
                  << std::fixed << std::setprecision(osp) << Esiglim
                  << " and used sigma limit " << siguse
                  << " (input was " << siglim << ") with "
                  << N << " hi-sigma points " << std::endl;

      // reset sigma limit in filter, but not if its too large
      // Esiglim/siglim < 3.0            // not too big
      // Esiglim/siglim > 0.1            // not too small
      if(resetSigma) {
         if(Esiglim > siglim) {
            if(Esiglim/siglim < 3.0) siguse = Esiglim;
            else                     siguse = 3*siglim;
         }
         else if(Esiglim < siglim) {
            if(Esiglim/siglim > 0.1) siguse = Esiglim;
            else                     siguse = 0.1*siglim;
         }
         fdf.setSigma(siguse);      // use the new sigma limit
      }

      // analysis ------------------
      fdf.analysis();               // get the outliers and slips

      // consider results ----------
      results = fdf.getResults();
      std::vector<unsigned int> eraseIndex;       // index in results[] to be erased

      // loop over results: mark duplicates to be erased, mark outliers, fix slips
      for(i=0; i<results.size(); i++) {
         if(verbose) logstrm << "# " << label << " Result " << ++nr
               << std::fixed << std::setprecision(osp)
               << " " << xdata[results[i].index]
               << " " << results[i].asString(osp) << std::endl;

         // mark outliers
         if(results[i].type == FilterHit<T>::outlier) {
            k = results[i].index;
            for(j=0; j<results[i].npts; j++) Tflags[k+j] = 1;
         }

         // slips: handle duplicates, and edit data
         if(results[i].type == FilterHit<T>::slip) {

            // search previous results, if they exist, for duplicate slips
            bool skip(false);
            for(k=0; k<Results.size(); k++) {

               for(j=0; j<Results[k].size(); j++) {         // loop over prev results
                  FilterHit<T>& oldres(Results[k][j]);

                  if(oldres.type != FilterHit<T>::slip)     // not a slip
                     continue;
                  if(results[i].index != oldres.index)      // different index
                     continue;

                  // if both slips are non-small: add new to previous and delete new
                  if(results[i].score == 100 && oldres.score == 100)
                  {
                     //logstrm << " Combine slips " << results[i].asString(osp)
                     // << " and " << oldres.asString(osp) << std::endl;
                     T step = oldres.step + results[i].step;
                     oldres.step = step;
                     // keep the later sigma - outliers have been removed
                     oldres.sigma = results[i].sigma;
                     oldres.score = (unsigned int)(0.5+100.*::fabs(step)/fdlim);
                     if(oldres.score > 100) oldres.score = 100;
                     //logstrm << " New combined slip is "
                     // << oldres.asString(osp) << std::endl;
                  }

                  // if both slips are small, they should be identical => delete new
                  // ...so, if only one is small, go on
                  else if(results[i].score == 100 || oldres.score == 100)
                     continue;

                  // else both are small: delete new result
                  skip = true;
                  break;
               }
            }

            // save the index, to be deleted later
            if(skip) { eraseIndex.push_back(i); continue; }

            // if slip is too small to fix, go on
            if(::fabs(results[i].step) < fdlim) {
               //if(verbose) logstrm << "# " << label << " Slip too small: "
               // << std::fixed << std::setprecision(osp) << results[i].step
               //<< " < " << fdlim << std::endl;
               continue;
            }

            // fix the slip in the Tdata
            int islip(results[i].step + (results[i].step >= 0.0 ? 0.5:-0.5));
            if(islip) {
               if(verbose) logstrm << "# " << label << " Fix slip " << islip
                     << " " << results[i].asString(osp) << std::endl;

               // NB if slips in Tdata not fixed, its going to affect later
               // runs of the analysis
               for(j=results[i].index; j<Tdata.size(); j++)
                  Tdata[j] -= islip;
            }
         }  // end if slip

      }  // end loop over results

      // erase marked slips
      if(eraseIndex.size() > 0) {
         //sort(eraseIndex.begin(),eraseIndex.end());
         for(i=eraseIndex.size()-1; i>=0; i--) {
            //logstrm << " Erase " << results[eraseIndex[i]].asString(osp)
            //<< std::endl;
            results.erase(results.begin()+eraseIndex[i]);    // erase vector
            if(i == 0) break;    // i is unsigned
         }
      }

      // save to Results
      Nsig = fdf.getNhighSigma();
      Results.push_back(results);
      results.clear();

      // dump this analysis
      if(verbose) fdf.dump(logstrm,"FIX"+gpstk::StringUtils::asString(iter-1)+label);

      ++iter;                    // next iteration

   }  // end iteration loop

   // copy all Results into results
   results.clear();
   for(k=0; k<Results.size(); k++)
      for(j=0; j<Results[k].size(); j++)
         results.push_back(Results[k][j]);

   // scan for remaining small slips (result of combination of two large)
   if(!doSmall) {
      std::vector<unsigned int> eraseIndex;       // index in results[] to be erased
      for(i=0; i<results.size(); i++) {
         if(results[i].type != FilterHit<T>::slip) continue;
         //logstrm << " Final check " << results[i].asString(osp) << std::endl;
         if(results[i].score == 100) continue;
         eraseIndex.push_back(i);
      }  // end loop over results
      if(eraseIndex.size() > 0) {
         for(i=eraseIndex.size()-1; i>=0; i--) {
            //logstrm << " Final Erase " << results[eraseIndex[i]].asString(osp)
            //<< std::endl;
            results.erase(results.begin()+eraseIndex[i]);    // erase vector
            if(i == 0) break;    // i is unsigned
         }
      }
      //logstrm << " End Final check" << std::endl;
   }


   return results.size();
}  // end IterativeFDiffFilter::analysis()

//------------------------------------------------------------------------------------
// Edit the data for the caller using results created by analysis().
// NB data arrays are NOT edited by filter.
// NB must pass SAME arrays used in c'tor, after calling analysis(), but not const.
// param data vector<T> of data values
// param flags vector<int> of flags, 0 means good. this array may be empty.
// param doInt if true, integerize the slips before fixing
// param badFlag set flags[] to this int when marking it bad (1)
// return the number of edits (slips + pts rejected)
template <class T> int IterativeFDiffFilter<T>::editArrays(
                     std::vector<T>& data, std::vector<int>& flags,
                     const bool doInt, const int badFlag)
{
   int nedit(0);
   unsigned int i,j,n;

   for(i=0; i<results.size(); i++) {
      if(results[i].type==FilterHit<T>::BOD || results[i].type==FilterHit<T>::other)
         continue;

      else if(results[i].type==FilterHit<T>::outlier) {
         j = results[i].index;
         // TD should this loop account for flags[j] already set?
         for(n=0; n<results[i].npts; nedit++,n++,j++)
            flags[j] = badFlag;
      }

      else if(results[i].type==FilterHit<T>::slip) {
         double slip(results[i].step);
         if(doInt) {
            int islip(slip + (slip >= 0.0 ? 0.5:-0.5));
            if(islip == 0) continue;
            slip = double(islip);
         }
         for(j=results[i].index; j<data.size(); j++)
            data[j] -= slip;
         nedit++;
      }

   }

   return nedit;
}  // end int IterativeFDiffFilter::editArrays()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#endif // #define FDIFF_FILTER_INCLUDE
