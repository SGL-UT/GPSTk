//============================================================================
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

/// @file Stats.hpp
/// Conventional, sequential and weighted one-sample, and two-sample statistics

#ifndef INCLUDE_GPSTK_STATS_INCLUDE
#define INCLUDE_GPSTK_STATS_INCLUDE

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Exception.hpp"
#include "MiscMath.hpp"
#include "Vector.hpp"

namespace gpstk
{
   /** @addtogroup math */
   //@{

   /// Compute the median of a gpstk::Vector
   template <class T> inline T median(const Vector<T>& v)
   {
      const unsigned int n(v.size());
      if(n==0) return T();
      if(n==1) return v(0);
      if(n==2) return (v(0)+v(1))/T(2);
      // insert sort
      int i,j;
      T x;
      Vector<T> w(v);
      for(i=0; i<n; i++) {
         x = w[i] = v(i);
         j = i-1;
         while(j>=0 && x<w[j]) {
            w[j+1] = w[j];
            j--;
         }
         w[j+1] = x;
      }

      if(n % 2) return (w[(n+1)/2-1]);
      return ((w[n/2-1]+w[n/2])/T(2));

   }  // end median(Vector)

   /// median absolute deviation of a gpstk::Vector
   template <class T> inline T mad(const gpstk::Vector<T>& v)
   {
      if (v.size() < 2) return T();
      
      double med = gpstk::median(v);
      gpstk::Vector<T> w(v);
      for(size_t i=0; i < w.size(); i++)
         w[i] = std::abs(w[i]- med);

      return gpstk::median(w);
   }  // end mad(Vector)

   /// Compute the median of a std::vector
   template <class T> inline T median(const std::vector<T>& v)
   {
      const unsigned int n(v.size());
      if(n==0) return T();

      std::vector<T> w(v);
      std::sort(w.begin(), w.end());

      if (n % 2) return w[(n+1)/2-1];
      return ((w[n/2-1]+w[n/2])/T(2));
   }  // end median(vector)

   /// median absolute deviation of a std::vector
   template <class T> inline T mad(const std::vector<T>& v)
   {
      if (v.size() < 2) return T();
      
      double med = gpstk::median(v);
      std::vector<T> w(v);
      for(size_t i=0; i < w.size(); i++)
         w[i] = std::abs(w[i]- med);

      return gpstk::median(w);
   }  // end mad(vector)

   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   // forward declaration
   template <class T> class TwoSampleStats;

   //---------------------------------------------------------------------------
   /// Conventional statistics for one sample, with scaling to improve numerical
   /// error in cases of very large numbers. Constructor does the same as Reset();
   /// use it when starting a new series of input samples.
   /// Results are available at any time by calling N(), Minimum(), Maximum(),
   /// Average(), Variance() and StdDev(). Also the scale is available in Scale().
   /// NB. Variance is normalized with 1/(N-1) and StdDev is sqrt(Variance).
   /// NB. This class is not intended to be used with non-floating types,
   ///     for which it may yield incorrect results.
   template <class T> class Stats
   {
   public:
      friend class TwoSampleStats<T>;

      /// constructor
      Stats() { Reset(); }

      /// reset, i.e. ignore earlier data and restart sampling
      inline void Reset(void)
      {
         n = 0;
         setScale = false;
         scale = T(1);
         sum = sum2 = min = max = T();
      }

      // add and subtract ---------------------------------------------------

      /// add a single sample to the computation of statistics
      /// NB this is the fundamental Add routine; all other Add's call this
      void Add(const T& x)          // Stats
      {
         if(n == 0) {
            sum = sum2 = T();
            min = max = x;
            scale = T(1);
            setScale = false;
         }
         if(!setScale && x!=T()) { scale=::fabs(x); setScale = true; }
         sum += x/scale;
         sum2 += (x/scale)*(x/scale);
         if(x < min) min=x;
         if(x > max) max=x;
         n++;
      }

      /// remove a sample from the computation of statistics.
      /// NB this is the fundamental Subtract routine; all others call this
      /// NB. Assumes that this sample was previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      void Subtract(const T& x)           // Stats
      {
         if(n < 1) return;       // TD throw
         if(n == 1) { n = 0; return; }
         T sx(x/scale);
         sum -= sx;
         sum2 -= sx*sx;
         n--;
      }

      /// Add gpstk::Vector<T> of data to the statistics
      void Add(const Vector<T>& X)                 // Stats
      {
         for(size_t i=0; i < X.size(); i++) Add(X(i));
      }

      /// add a std::vector<T> of samples to the computation of statistics
      inline void Add(std::vector<T>& X)           // Stats
      {
         for(size_t i=0; i<X.size(); i++) Add(X[i]);
      }

      /// Subtract gpstk::Vector<T> of data to the statistics
      /// NB. Assumes that these samples were previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      void Subtract(const Vector<T>& X)            // Stats
      {
         for(size_t i=0; i < X.size(); i++) Subtract(X(i));
      }

      /// remove a std::vector<T> of samples to the computation of statistics
      /// NB. Assumes that these samples were previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      inline void Subtract(std::vector<T>& X)       // Stats
      {
         for(size_t i=0; i<X.size(); i++) Subtract(X[i]);
      }

      // combine two Stats objects -------------------------------------

      /// combine two Stats (assumed taken from the same or equivalent ensembles)
      Stats<T>& operator+=(const Stats<T>& S)
      {
         if(S.n == 0)
            return *this;
         if(!setScale) { setScale=true; scale = S.scale; }
         // TD what if both have !setScale?
         if((n == 0) || (S.min < min)) min=S.min;
         if((n == 0) || (S.max > max)) max=S.max;
         sum += S.scale*S.sum/scale;
         sum2 += (S.scale/scale)*(S.scale/scale)*S.sum2;
         n += S.n;
         return *this;
      }

      /// remove one Stats from another, assumed to be taken from the same or
      /// equivalent ensembles.
      /// NB. Assumes that these samples were previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      Stats<T>& operator-=(const Stats<T>& S)
      {
         if(n <= S.n) { Reset(); return *this; }
         sum -= S.scale*S.sum/scale;
         sum2 -= (S.scale/scale)*(S.scale/scale)*S.sum2;
         n -= S.n;
         return *this;
      }

      // dump and reload -----------------------------------------------

      /// Dump Stats private members directly;
      /// useful in saving an object (e.g. to a file); reload with Load().
      /// @param vuint output vector<unsigned int> of length 2, input of Load()
      /// @param vT output vector<T> of length 5, input of Load()
      void Dump(std::vector<unsigned int>& vuint, std::vector<T>& vT)
      {
         vuint.clear();
         vuint.push_back(n); vuint.push_back(setScale ? 1:0);

         vT.clear();
         vT.push_back(setScale ? scale : T(0));
         vT.push_back(min); vT.push_back(max);
         vT.push_back(sum); vT.push_back(sum2);
      }

      /// Define Stats private members directly; useful in continuing
      /// with an object that was earlier saved (e.g. to a file) using Dump().
      /// @param vuint input vector<unsigned int> of length 2, output of Dump()
      /// @param vT input vector<T> of length 5, output of Dump()
      /// NB no checking at all - caller has burden of validity
      /// NB zero-fill rather than throwing
      void Load(const std::vector<unsigned int>& vuint, const std::vector<T>& vT)
      {
         if(vuint.size() < 2 || vT.size() < 4) {
            n = 0;
            setScale = false;
            scale = T(0);
            min = T(0); max = T(0);
            sum = T(0); sum2 = T(0);
         }
         else {
            n = vuint[0];
            setScale = (vuint[1] != 0);
            scale = vT[0];
            min = vT[1]; max = vT[2];
            sum = vT[3]; sum2 = vT[4];
         }
      }

      // output ---------------------------------------------------

      /// Write Stats to a single-line string
      std::string asString(std::string msg=std::string(), int w=7, int p=4) const
      {
         std::ostringstream oss;
         //static const int p=4, w=7;
         oss << "stats(con):" << (msg.empty() ? "" : " "+msg)
            << " N " << std::setw(w) << N() << std::fixed << std::setprecision(p)
            << "  Ave " << std::setw(w) << Average()
            << "  Std " << std::setw(w) << StdDev()
            << "  Var " << std::setw(w) << Variance()
            << "  Min " << std::setw(w) << Minimum()
            << "  Max " << std::setw(w) << Maximum()
            << "  P2P " << std::setw(w) << Maximum()-Minimum();
         return oss.str();
      }

      /// Write Stats N,ave,sig to a short single-line string
      std::string asShortString(std::string msg=std::string(), int w=0, int p=3) const
      {
         std::ostringstream oss;
         oss << msg << std::fixed << std::setprecision(p)
            << " N " << std::setw(w) << N()
            << "  Ave " << std::setw(w) << Average()
            << "  Std " << std::setw(w) << StdDev();
         return oss.str();
      }

      // the rest are just accessors --------------------------------------
      /// return the sample size
      inline unsigned int N(void) const { return n; }

      /// return minimum value
      inline T Minimum(void) const
         { if(n) return min; else return T(); }

      /// return maximum value
      inline T Maximum(void) const
         { if(n) return max; else return T(); }

      /// return the average
      inline T Average(void) const           // Stats
      {
         if(n>0)
            return (scale*sum/T(n));
         else
            return T();
      }

      /// return computed variance
      inline T Variance(void) const          // Stats
      {
         if(n>1)
            return scale*scale*(sum2-sum*sum/T(n))/T(n-1);
         else
            return T();
      }

      /// return computed standard deviation
      inline T StdDev(void) const
         { if(n <= 1) return T(); return SQRT(Variance()); }

      /// return the scale
      inline T Scale(void) const
         { if(n) return scale; else return T(); }

   protected:
      unsigned int n;   ///< number of samples added to the statistics
      bool setScale;    ///< scale has been set to non-zero
      T scale;          ///< scale
      T min;            ///< Minimum value
      T max;            ///< Maximum value
      T sum;            ///< sum of x
      T sum2;           ///< sum of x squared

   }; // end class Stats

   /// Output operator for Stats class
   template <class T> std::ostream& operator<<(std::ostream& s, const Stats<T>& ST) 
   {
      std::ofstream savefmt;
      savefmt.copyfmt(s);
      s << "       N = " << ST.N() << " (1-sample statistics)\n";
      s << " Minimum = "; s.copyfmt(savefmt); s << ST.Minimum();
      s << " Maximum = "; s.copyfmt(savefmt); s << ST.Maximum() << "\n";
      s << " Average = "; s.copyfmt(savefmt); s << ST.Average();
      s << " Std Dev = "; s.copyfmt(savefmt); s << ST.StdDev();
      s << " Variance = "; s.copyfmt(savefmt); s << ST.Variance(); // temp
      return s;
   }

   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   /// Sequential conventional statistics for one sample; gives results identical
   /// to class Stats except there is no scaling. This class maintains a current
   /// average and variance with each Add(); thus it is more efficient when results
   /// at each step are accessed - use this class when stats are to be accessed often,
   /// e.g. after each Add().
   /// Class Stats holds sum(x) and sum(x squared) and computes ave, sig etc on call.
   /// NB. class WtdStats (weighted stats) derives from this class.
   template <class T> class SeqStats
   {
   public:
      /// constructor
      SeqStats() : n(0) { }

      /// reset, i.e. ignore earlier data and restart sampling
      inline void Reset(void) { n=0; }

      /// constructor given a gpstk::Vector<T> of data
      SeqStats(Vector<T>& X) { n = 0; Add(X); }

      // add and subtract ---------------------------------------------------

      /// add a single sample to the computation of statistics)
      /// NB this is the fundamental Add routine; all other Add's call this
      void Add(const T& x)                // SeqStats
      {
         if(n == 0) {
            min = max = ave = x;
            var = T();
         }
         else {
            if(x < min) min=x;
            if(x > max) max=x;
         }

         ave += (x-ave)/T(n + 1);
         if(n > 0) var = T(n)*var/T(n + 1) + (x-ave)*(x-ave)/T(n);

         n++;
      }

      /// remove a sample from the computation of statistics
      /// NB this is the fundamental Subtract routine; all others call this
      /// NB. Assumes that this sample was previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      void Subtract(const T x)                  // SeqStats
      {
         if(n == 0) return;
         if(n > 1)
            var = (var - (x-ave)*(x-ave)/T(n-1))*T(n)/(T(n)-T(1));
         else
            var = T();
         ave = T(n)*(ave - x/T(n))/(T(n)-T(1));
         n--;
      }

      /// add a gpstk::Vector<T> of samples to the computation of statistics
      inline void Add(Vector<T>& X)                // SeqStats
      {
         for(size_t i=0; i<X.size(); i++) Add(X(i));
      }

      /// add a std::vector<T> of samples to the computation of statistics
      inline void Add(std::vector<T>& X)                 // SeqStats
      {
         for(size_t i=0; i<X.size(); i++) Add(X[i]);
      }

      /// remove a gpstk::Vector<T> of samples to the computation of statistics
      /// NB. Assumes that these samples were previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      inline void Subtract(Vector<T>& X)                 // SeqStats
      {
         for(size_t i=0; i<X.size(); i++) Subtract(X(i));
      }

      /// remove a std::vector<T> of samples to the computation of statistics
      /// NB. Assumes that these samples were previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      inline void Subtract(std::vector<T>& X)                  // SeqStats
      {
         for(size_t i=0; i<X.size(); i++) Subtract(X[i]);
      }

      // combine two Stats objects -------------------------------------

      /// combine two SeqStats (assumed taken from the same or equivalent ensembles);
      SeqStats<T>& operator+=(const SeqStats<T>& S)
      {
         if(S.n == 0) return *this;
         if(n==0) { *this = S; return *this; }

         if(S.min < min) min=S.min;
         if(S.max > max) max=S.max;

         T newave = T(n)*ave + T(S.n)*S.ave;
         T newvar = T(n)*var + T(S.n)*S.var + T(n)*ave*ave + T(S.n)*S.ave*S.ave;
         ave = newave/T(n+S.n);
         var = newvar/T(n+S.n) - ave*ave;
         n += S.n;

         return *this;
      }  // end SeqStats operator+=

      /// remove one SeqStats from another, assumed to be taken from the same or
      /// equivalent ensembles.
      /// NB. Assumes that these samples were previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      SeqStats<T>& operator-=(const SeqStats<T>& S)
      {
         if(n <= S.n) { n=0; return *this; }

         T newave = T(n)*ave - T(S.n)*S.ave;
         T newvar = T(n)*var - T(S.n)*S.var + T(n)*ave*ave - T(S.n)*S.ave*S.ave;
         ave = newave/T(n-S.n);
         var = newvar/T(n-S.n) - ave*ave;
         n -= S.n;

         return *this;
      }  // end SeqStats operator-=

      // dump and reload -----------------------------------------------

      /// Dump SeqStats private members directly;
      /// useful in saving an object (e.g. to a file); reload with Load().
      /// @param vuint output vector<unsigned int> of length 1, input of Load()
      /// @param vT output vector<T> of length 4, input of Load()
      void Dump(std::vector<unsigned int>& vuint, std::vector<T>& vT)
      {
         vuint.clear();
         vuint.push_back(n);

         vT.clear();
         vT.push_back(min); vT.push_back(max);
         vT.push_back(ave); vT.push_back(var);
      }

      /// Define SeqStats private members directly; useful in continuing
      /// with an object that was earlier saved (e.g. to a file) using Dump().
      /// @param vuint input vector<unsigned int> of length 1, output of Dump()
      /// @param vT input vector<T> of length 4, output of Dump()
      /// NB no checking at all - caller has burden of validity
      /// NB zero-fill rather than throwing
      void Load(const std::vector<unsigned int>& vuint, const std::vector<T>& vT)
      {
         if(vuint.size() < 1 || vT.size() < 4) {
            n = 0;
            min = T(0); max = T(0);
            ave = T(0); var = T(0);
         }
         else {
            n = vuint[0];
            min = vT[0]; max = vT[1];
            ave = vT[2]; var = vT[3];
         }
      }

      // output ---------------------------------------------------

      /// Write SeqStats to a single-line string
      std::string asString(std::string msg=std::string(), int w=7, int p=4) const
      {
         std::ostringstream oss;
         //static const int p=4, w=7;
         oss << "stats(seq):" << (msg.empty() ? "" : " "+msg)
            << " N " << std::setw(w) << N() << std::fixed << std::setprecision(p)
            << "  Ave " << std::setw(w) << Average()
            << "  Std " << std::setw(w) << StdDev()
            << "  Var " << std::setw(w) << Variance()
            << "  Min " << std::setw(w) << Minimum()
            << "  Max " << std::setw(w) << Maximum()
            << "  P2P " << std::setw(w) << Maximum()-Minimum();
         return oss.str();
      }

      /// Write SeqStats N,ave,sig to a short single-line string
      std::string asShortString(std::string msg=std::string(), int w=0, int p=3) const
      {
         std::ostringstream oss;
         oss << msg << std::fixed << std::setprecision(p)
            << " N " << std::setw(w) << N()
            << "  Ave " << std::setw(w) << Average()
            << "  Std " << std::setw(w) << StdDev();
         return oss.str();
      }

      // accessors -------------------------------------------------------
      /// return the sample size
      inline unsigned int N(void) const { return n; }

      /// return minimum value
      inline T Minimum(void) const { if(n) return min; else return T(); }

      /// return maximum value
      inline T Maximum(void) const { if(n) return max; else return T(); }

      /// return computed average
      inline T Average(void) const { if(n == 0) return T(); return ave; }

      /// return computed variance
      inline T Variance(void) const       // SeqStats
      {
         if(n <= 1) return T();
         return (T(n)*var/T(n-1));
      }

      /// return computed standard deviation
      inline T StdDev(void) const         // SeqStats
      {
         if(n <= 1) return T();
         return SQRT(Variance());
      }

   protected:
      unsigned int n;   ///< number of samples added to the statistics
      T min;            ///< Minimum value
      T max;            ///< Maximum value
      T ave;            ///< Average value
      T var;            ///< Variance (square of the standard deviation)

   }; // end class SeqStats

   /// Output operator for SeqStats class
   template <class T> std::ostream& operator<<(std::ostream& s, const SeqStats<T>& ST)
   {
      std::ofstream savefmt;
      savefmt.copyfmt(s);
      s << "       N = " << ST.N() << " (1-sample stats, seq.impl.)\n";
      s << " Minimum = "; s.copyfmt(savefmt); s << ST.Minimum();
      s << " Maximum = "; s.copyfmt(savefmt); s << ST.Maximum() << "\n";
      s << " Average = "; s.copyfmt(savefmt); s << ST.Average();
      s << " Std Dev = "; s.copyfmt(savefmt); s << ST.StdDev();
      s << " Variance = "; s.copyfmt(savefmt); s << ST.Variance(); // temp
      return s;
   }

   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   /// Weighted conventional statistics for one sample. Derived from SeqStats<T>
   /// Weights must not be zero; zero weight causes the sample to be ignored.
   template <class T> class WtdStats : public SeqStats<T>
   {
   public:
      /// constructor
      WtdStats() { }

      /// constructor given a gpstk::Vector<T> of data, with weights
      WtdStats(Vector<T>& X, Vector<T>& W)
      {
         SeqStats<T>::n = 0;
         Add(X,W);
      }

      // add and subtract -------------------------------------

      /// add a single sample to the computation of statistics
      /// NB this is the fundamental Add routine; all other Add's call this
      /// NB input of zero weight causes the sample x to be ignored.
      void Add(const T& x, const T& wt_in)      // WtdStats
      {
         T wt(::fabs(wt_in));
         if(wt == T()) return;      // Don't add with zero weight

         if(SeqStats<T>::n == 0) {
            SeqStats<T>::min = SeqStats<T>::max = SeqStats<T>::ave = x;
            SeqStats<T>::var = T();
            WtNorm = T();
         }
         else {
            if(x < SeqStats<T>::min) SeqStats<T>::min=x;
            if(x > SeqStats<T>::max) SeqStats<T>::max=x;
         }

         SeqStats<T>::ave += (x-SeqStats<T>::ave)*(wt/(WtNorm+wt));
         if(SeqStats<T>::n > 0)
            SeqStats<T>::var = (WtNorm/(WtNorm+wt))*SeqStats<T>::var
                           + (x-SeqStats<T>::ave)*(x-SeqStats<T>::ave)*(wt/WtNorm);
         WtNorm += wt;

         SeqStats<T>::n++;
      }

      /// remove a sample from the computation of statistics
      /// NB input of zero weight causes the sample x to be ignored.
      /// NB this is the fundamental Subtract routine; all others call this
      /// NB. Assumes that this sample was previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      void Subtract(const T& x, const T& wt_in)    // WtdStats
      {
         if(SeqStats<T>::n == 0) return;
         T wt(::fabs(wt_in));
         if(wt == T()) return;      // Don't add with zero weight

         T newave = WtNorm*SeqStats<T>::ave - wt*x;
         T newvar = WtNorm*SeqStats<T>::var
                        + WtNorm * SeqStats<T>::ave * SeqStats<T>::ave - wt*x*x;
         WtNorm -= wt;
         SeqStats<T>::ave = newave/WtNorm;
         SeqStats<T>::var = newvar/WtNorm - SeqStats<T>::ave * SeqStats<T>::ave;
         SeqStats<T>::n--;
      }

      /// add a gpstk::Vector<T> of samples, with weights
      inline void Add(Vector<T>& X, Vector<T>& W)     // WtdStats
      {
         for(size_t i=0; i<(X.size()>W.size() ? W.size():X.size()); i++)
            Add(X(i),W(i));
      }

      /// add a std::vector<T> of samples, with weights
      inline void Add(std::vector<T>& X, std::vector<T>& W)    // WtdStats
      {
         for(size_t i=0; i<(X.size()>W.size() ? W.size():X.size()); i++)
            Add(X(i),W(i));
      }

      /// remove a gpstk::Vector<T> of samples, with weights
      /// NB input of zero weight causes the sample x to be ignored.
      /// NB. Assumes that this sample was previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      inline void Subtract(Vector<T>& X, Vector<T>& W)      // WtdStats
      {
         size_t i, nn(X.size() > W.size() ? W.size() : X.size());
         for(i=0; i<nn; i++) Subtract(X(i),W(i));
      }

      /// remove a std::vector<T> of samples, with weights
      /// NB input of zero weight causes the sample x to be ignored.
      /// NB. Assumes that this sample was previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      inline void Subtract(std::vector<T>& X, std::vector<T>& W)     // WtdStats
      {
         size_t i, nn(X.size() > W.size() ? W.size() : X.size());
         for(i=0; i<nn; i++) Subtract(X(i),W(i));
      }

      // combine two objects -------------------------------------------

      /// combine two WtdStats (assumed taken from the same or equivalent ensembles);
      WtdStats<T>& operator+=(const WtdStats<T>& S)
      {
         if(S.n == 0)
            return *this;
         if(SeqStats<T>::n == 0)
            { *this = S; return *this; }

         //if(S.min < SeqStats<T>::min) SeqStats<T>::min = S.min; // gcc why!?!?
         if(SeqStats<T>::min > S.min) SeqStats<T>::min = S.min;
         if(S.max > SeqStats<T>::max) SeqStats<T>::max = S.max;

         T newave = WtNorm*SeqStats<T>::ave + S.WtNorm*S.ave;
         T newvar = WtNorm*SeqStats<T>::var + S.WtNorm*S.var
                    + WtNorm*SeqStats<T>::ave*SeqStats<T>::ave + S.WtNorm*S.ave*S.ave;
         WtNorm += S.WtNorm;
         SeqStats<T>::ave = newave/WtNorm;
         SeqStats<T>::var = newvar/WtNorm - SeqStats<T>::ave*SeqStats<T>::ave;
         SeqStats<T>::n += S.n;

         return *this;
      }

      /// remove one WtdStats from another, assumed to be taken from the same or
      /// equivalent ensembles.
      /// NB. Assumes that this sample was previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      WtdStats<T>& operator-=(const WtdStats<T>& S)
      {
         if(SeqStats<T>::n <= S.n) { SeqStats<T>::n=0; return *this; }

         T newave = WtNorm*SeqStats<T>::ave - S.WtNorm*S.ave;
         T newvar = WtNorm*SeqStats<T>::var-S.WtNorm*S.var
                    + WtNorm*SeqStats<T>::ave*SeqStats<T>::ave - S.WtNorm*S.ave*S.ave;
         WtNorm -= S.WtNorm;
         SeqStats<T>::ave = newave/WtNorm;
         SeqStats<T>::var = newvar/WtNorm - SeqStats<T>::ave*SeqStats<T>::ave;
         SeqStats<T>::n -= S.n;

         return *this;

      }  // end WtdStats operator-=

      // dump and reload -----------------------------------------------

      /// Dump WtdStats private members directly;
      /// useful in saving an object (e.g. to a file); reload with Load().
      /// @param vuint output vector<unsigned int> of length 1, input of Load()
      /// @param vT output vector<T> of length 5, input of Load()
      void Dump(std::vector<unsigned int>& vuint, std::vector<T>& vT)      // WtdStats
      {
         SeqStats<T>::Dump(vuint,vT);
         vT.push_back(WtNorm);
      }

      /// Define WtdStats private members directly; useful in continuing
      /// with an object that was earlier saved (e.g. to a file) using Dump().
      /// @param vuint input vector<unsigned int> of length 1, output of Dump()
      /// @param vT input vector<T> of length 5, output of Dump()
      /// NB no checking at all - caller has burden of validity
      /// NB zero-fill rather than throwing
      void Load(std::vector<unsigned int>& vuint, std::vector<T>& vT)      // WtdStats
      {
         if(vuint.size() < 1 || vT.size() < 5) {
            vuint.clear();
            vuint.push_back(0);
            vT.clear();
            for(size_t i=0; i<5; i++) vT.push_back(T(0));
         }
         SeqStats<T>::Load(vuint,vT);
         WtNorm = vT[4];
      }

      // output ----------------------------------------------------------

      /// Write WtdStats to a single-line string
      std::string asString(std::string msg=std::string(), int w=7, int p=4) const
      {
         std::ostringstream oss;
         oss << "stats(wtd):" << (msg.empty() ? "" : " "+msg)
            << " N " << std::setw(w) << SeqStats<T>::N()
            << std::fixed << std::setprecision(p)
            << "  Ave " << std::setw(w) << SeqStats<T>::Average()
            << "  Std " << std::setw(w) << SeqStats<T>::StdDev()
            << "  Var " << std::setw(w) << SeqStats<T>::Variance()
            << "  Min " << std::setw(w) << SeqStats<T>::Minimum()
            << "  Max " << std::setw(w) << SeqStats<T>::Maximum()
            << "  P2P " << std::setw(w)
            << SeqStats<T>::Maximum()-SeqStats<T>::Minimum()
            << "  Wts " << std::setw(w) << WtsSum();
         return oss.str();
      }

      /// Write WtdStats N,ave,sig to a short single-line string
      std::string asShortString(std::string msg=std::string(), int w=0, int p=3) const
      {
         std::ostringstream oss;
         oss << msg << std::fixed << std::setprecision(p)
            << " N " << std::setw(w) << SeqStats<T>::N()
            << "  Ave " << std::setw(w) << SeqStats<T>::Average()
            << "  Std " << std::setw(w) << SeqStats<T>::StdDev();
         return oss.str();
      }

      /// return normalization = sum of weights
      inline T WtsSum(void) const { return WtNorm; }     // WtdStats

   protected:
      T WtNorm;               ///< Normalization constant = sum weights

   }; // end class WtdStats

   /// Output operator for WtdStats class
   template <class T> std::ostream& operator<<(std::ostream& s, const WtdStats<T>& ST)
   {
      std::ofstream savefmt;
      savefmt.copyfmt(s);
      s << "       N = " << ST.N() << " (weighted 1-sample stats)\n";
      s << " Minimum = "; s.copyfmt(savefmt); s << ST.Minimum();
      s << " Maximum = "; s.copyfmt(savefmt); s << ST.Maximum() << "\n";
      s << " Average = "; s.copyfmt(savefmt); s << ST.Average();
      s << " Std Dev = "; s.copyfmt(savefmt); s << ST.StdDev();
      s << " Variance = "; s.copyfmt(savefmt); s << ST.Variance() << "\n";
      s << " SumWts  = "; s.copyfmt(savefmt); s << ST.WtsSum();
      return s;
   }

   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------
   /// Conventional statistics for two samples. Also uses a pair of Stats<T> for
   /// the each of the two samples.
   template <class T> class TwoSampleStats
   {
   public:
      /// constructor
      TwoSampleStats() : n(0), sumxy(T(0)) {}

      /// reset, i.e. ignore earlier data and restart sampling
      inline void Reset(void) { n=0; SX.Reset(); SY.Reset(); sumxy = T(0); }

      /// constructor given two gpstk::Vector<T>s of data - must be parallel
      TwoSampleStats(Vector<T>& X, Vector<T>& Y)
      {
         Reset();
         Add(X,Y);
      }

      /// Add data to the statistics
      /// NB this is the fundamental Add routine; all other Add's call this
      void Add(const T& x, const T& y)    // TwoSampleStats
      {
         SX.Add(x);
         SY.Add(y);
         sumxy += (x/SX.scale)*(y/SY.scale);
         n++;
      }

      /// Subtract data from the statistics
      /// NB this is the fundamental Subtract routine; all others call this
      /// NB. Assumes that these samples were previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      void Subtract(const T& x, const T& y)     // TwoSampleStats
      {
         if(n < 1) return;       // TD throw
         if(n == 1) { Reset(); return; }
         SX.Subtract(x);
         SY.Subtract(y);
         sumxy -= (x/SX.scale)*(y/SY.scale);
         n--;
      }

      /// Add two gpstk::Vector<T>s of data to the statistics
      void Add(const Vector<T>& X, const Vector<T>& Y)      // TwoSampleStats
      {
         for(size_t i=0; i<(X.size()<Y.size() ? X.size():Y.size()); i++)
            Add(X(i),Y(i));
      }

      /// Add two std::vectors of data to the statistics
      void Add(const std::vector<T>& X, const std::vector<T>& Y)     // TwoSampleStats
      {
         for(size_t i=0; i<(X.size()<Y.size() ? X.size():Y.size()); i++)
            Add(X[i],Y[i]);
      }

      /// Subtract two gpstk::Vector<T>s of data from the statistics
      void Subtract(const Vector<T>& X, const Vector<T>& Y)    // TwoSampleStats
      {
         for(size_t i=0; i<(X.size()<Y.size() ? X.size():Y.size()); i++)
            Subtract(X(i),Y(i));
      }

      /// Subtract two std::vectors of data from the statistics
      void Subtract(const std::vector<T>& X, const std::vector<T>& Y)
      {
         for(size_t i=0; i<(X.size()<Y.size() ? X.size():Y.size()); i++)
            Subtract(X[i],Y[i]);
      }

      // combine two objects ---------------------------------------------

      /// combine two TwoSampleStats (assumed to be taken from the same or
      /// equivalent ensembles)
      TwoSampleStats<T>& operator+=(TwoSampleStats<T>& TSS)
      {
         if(n + TSS.n == 0) return *this;
         SX += TSS.SX;
         SY += TSS.SY;
         sumxy += (TSS.SX.scale/SX.scale)*(TSS.SY.scale/SY.scale)*TSS.sumxy;
         n += TSS.n;
         return *this;
      }  // end TwoSampleStats operator+=

      /// remove one WtdStats from another, assumed to be taken from the same or
      /// equivalent ensembles.
      /// NB. Assumes that these samples were previously added.
      /// NB. Minimum() and Maximum() may no longer be valid.
      TwoSampleStats<T>& operator-=(TwoSampleStats<T>& TSS)
      {
         if(n <= TSS.n) { Reset(); return *this; }
         SX -= TSS.SX;
         SY -= TSS.SY;
         sumxy -= (TSS.SX.scale/SX.scale)*(TSS.SY.scale/SY.scale)*TSS.sumxy;
         n -= TSS.n;
         return *this;
      }  // end TwoSampleStats operator-=

      // dump and reload --------------------------------------------

      /// Dump TwoSampleStats private members directly;
      /// useful in saving an object (e.g. to a file); reload with Load().
      /// @param vuint output vector<unsigned int> of length 5, input of Load()
      /// @param vT output vector<T> of length 11, input of Load()
      void Dump(std::vector<unsigned int>& vuint, std::vector<T>& vT)   // TSS
      {
         size_t i;
         std::vector<unsigned int> vi;
         std::vector<T> vt;

         vuint.clear(); vT.clear();

         SX.Dump(vi,vt);
         for(i=0; i<2; i++) vuint.push_back(vi[i]);
         for(i=0; i<5; i++) vT.push_back(vt[i]);

         SY.Dump(vi,vt);                              // this will clear vi,vt first
         for(i=0; i<2; i++) vuint.push_back(vi[i]);
         for(i=0; i<5; i++) vT.push_back(vt[i]);

         vuint.push_back(n);
         vT.push_back(sumxy);
      }

      /// Define TwoSampleStats private members directly; useful in continuing
      /// with an object that was earlier saved (e.g. to a file) using Dump().
      /// @param vuint input vector<unsigned int> of length 5, output of Dump()
      /// @param vT input vector<T> of length 11, output of Dump()
      /// NB no checking at all - caller has burden of validity
      /// NB zero-fill rather than throwing
      void Load(std::vector<unsigned int>& vuint, std::vector<T>& vT)   // TSS
      {
         size_t i;
         std::vector<unsigned int> vi2;
         std::vector<double> vt2;
         if(vuint.size() < 5 || vT.size() < 11) {
            vuint.clear();
            for(i=0; i<5; i++) vT.push_back(T(0));
            vT.clear();
            for(i=0; i<11; i++) vT.push_back(T(0));
         }
         SX.Load(vuint,vT);

         for(i=0; i<2; i++) vi2.push_back(vuint[i+2]);
         for(i=0; i<5; i++) vt2.push_back(vT[i+5]);
         SY.Load(vi2,vt2);

         n = vuint[4];
         sumxy = vT[10];
      }

      // output -----------------------------------------------------------

      /// Write TwoSampleStats to a three-line string
      std::string asString(std::string msg=std::string(), int w=7, int p=4) const
      {
         std::ostringstream oss;
         //static const int p=4, w=7;
         oss << SX.asString(msg,w,p) << " (X)" << std::endl
            << SY.asString(msg,w,p) << " (Y)" << std::endl
            << "stats(tss):" << (msg.empty() ? "" : " "+msg)
            << " N " << std::setw(w) << N() << std::fixed << std::setprecision(p)
            << "  Int " << std::setw(w) << Intercept()
            << "  Slp " << std::setw(w) << Slope()
            << " +- " << std::setw(w) << SigmaSlope()
            << "  CSig " << std::setw(w) << SigmaYX()
            << "  Corr " << std::setw(w) << Correlation();
         return oss.str();
      }

      /// Write TwoSampleStats as a short 1-line string
      std::string asShortString(std::string msg=std::string(), int w=0, int p=3) const
      {
         std::ostringstream oss;
         oss << SX.asShortString(msg,w,p) << " (X);"
             << SY.asShortString(msg,w,p) << " (Y);"
             << msg << std::fixed << std::setprecision(p)
             << std::fixed << std::setprecision(p)
             << "  Int " << std::setw(w) << Intercept()
             << "  Slp " << std::setw(w) << Slope()
             << " +- " << std::setw(w) << SigmaSlope()
             << "  CSig " << std::setw(w) << SigmaYX()
             << "  Corr " << std::setw(w) << Correlation();
         return oss.str();
      }

      // accessors -------------------------------------------------------

      /// access the sample size
      /// NB. n should match SX.N() and SY.N() at all times
      inline unsigned int N(void) const { return n; }
      /// return minimum X value
      inline T MinimumX(void) const { return SX.Minimum(); }
      /// return maximum X value
      inline T MaximumX(void) const { return SX.Maximum(); }
      /// return minimum Y value
      inline T MinimumY(void) const { return SY.Minimum(); }
      /// return maximum Y value
      inline T MaximumY(void) const { return SY.Maximum(); }
      /// return computed X average
      inline T AverageX(void) const { return SX.Average(); }
      /// return computed Y average
      inline T AverageY(void) const { return SY.Average(); }
      /// return computed X variance
      inline T VarianceX(void) const { return SX.Variance(); }
      /// return computed Y variance
      inline T VarianceY(void) const { return SY.Variance(); }
      /// return computed X standard deviation
      inline T StdDevX(void) const { return SX.StdDev(); }
      /// return computed Y standard deviation
      inline T StdDevY(void) const { return SY.StdDev(); }

      /// return slope of best-fit line Y=slope*X + intercept
      inline T Slope(void) const
      {
         if(n > 0) {
            T den(SX.sum2-SX.sum*SX.sum/T(n));
            if(den == T()) return T();                   // throw?
            return ( (SY.scale/SX.scale) * (sumxy-SX.sum*SY.sum/T(n)) / den );
         }
         else
            return T();
      }

      /// return intercept of best-fit line Y=slope*X + intercept
      inline T Intercept(void) const
      {
         if(n > 0)
            return (AverageY()-Slope()*AverageX());
         else
            return T();
      }

      /// return uncertainty in slope
      inline T SigmaSlope(void) const
      {
         if(n > 2) {
            T den(StdDevX()*SQRT(T(n-1)));
            if(den == T()) return T();                   // throw?
            return (SigmaYX()/den);
         }
         else
            return T();
      }

      /// return correlation
      inline T Correlation(void) const
      {
         if(n>1) {
            T den(StdDevX()*StdDevY()*T(n-1));
            if(den == T()) return T();                   // throw?
            return ( SX.scale*SY.scale*(sumxy-SX.sum*SY.sum/T(n)) / den);
         }
         else
            return T();
      }

      /// return conditional uncertainty = uncertainty y given x
      inline T SigmaYX(void) const
      {
         return SQRT(VarianceYX());
      }

      /// return conditional variance = (uncertainty y given x)^2
      inline T VarianceYX(void) const
      {
         if(n > 2) {
            return (VarianceY() * (T(n-1)/T(n-2))
                       * (T(1)-Correlation()*Correlation()));
         }
         else return T();
      }

      /// return the predicted Y at the given X, using Slope and Intercept
      inline T Evaluate(T x) const
      {
         return (Slope()*x + Intercept());
      }

   protected:
      Stats<T> SX;      ///< conventional 1-sample stats for first sample x
      Stats<T> SY;      ///< conventional 1-sample stats for second sample y
      unsigned int n;   ///< number of samples added to the statistics
      T sumxy;          ///< sum of x*y

   }; // end class TwoSampleStats

   /// Output operator for TwoSampleStats class
   template <class T>
   std::ostream& operator<<(std::ostream& s, const TwoSampleStats<T>& TSS) 
   {
      std::ofstream savefmt;
      savefmt.copyfmt(s);
      s << " N       = " << TSS.N() << " (two-sample-statistics)\n";
      s << " Minimum:  X = "; s.copyfmt(savefmt); s << TSS.MinimumX();
      s << "  Y = "; s.copyfmt(savefmt); s << TSS.MinimumY() << "\n";
      s << " Maximum:  X = "; s.copyfmt(savefmt); s << TSS.MaximumX();
      s << "  Y = "; s.copyfmt(savefmt); s << TSS.MaximumY() << "\n";
      s << " Average:  X = "; s.copyfmt(savefmt); s << TSS.AverageX();
      s << "  Y = "; s.copyfmt(savefmt); s << TSS.AverageY() << "\n";
      s << " Std Dev:  X = "; s.copyfmt(savefmt); s << TSS.StdDevX();
      s << "  Y = "; s.copyfmt(savefmt); s << TSS.StdDevY() << "\n";
      s << " Variance: X = "; s.copyfmt(savefmt); s << TSS.VarianceX();
      s << "  Y = "; s.copyfmt(savefmt); s << TSS.VarianceY() << "\n";

      bool bad(TSS.VarianceYX() == T());
      std::string badmsg("undef");
      s << " Intercept = "; s.copyfmt(savefmt);
      if(bad) s << badmsg; else s << TSS.Intercept();
      s << "  Slope = "; s.copyfmt(savefmt);
      if(bad) s << badmsg; else s << TSS.Slope();
      s << " with uncertainty = "; s.copyfmt(savefmt);
      if(bad) s << badmsg; else s << TSS.SigmaSlope();
      s << "\n Conditional uncertainty (sigma Y given X) = "; s.copyfmt(savefmt);
      if(bad) s << badmsg; else s << TSS.SigmaYX();
      s << "  Correlation = "; s.copyfmt(savefmt);
      if(bad) s << badmsg; else s << TSS.Correlation();

      return s;
   }

   //@}

}  // namespace

#endif   // INCLUDE_GPSTK_STATS_INCLUDE
