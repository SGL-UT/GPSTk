#pragma ident "$Id$"
#ifndef INCLUDE_GPSTK_STATS_HPP
#define INCLUDE_GPSTK_STATS_HPP
#include <sstream>
#include "MiscMath.hpp"
#include "Vector.hpp"
#include "Exception.hpp"

namespace gpstk
{
   template <class T>
   class Stats
   {
   public:
      explicit Stats() { n=0; weighted=false; }

      Stats(Vector<T>& X, Vector<T>& W)
      {
         n = 0;
         weighted = false;
         Add(X,W);
      }

      inline void Reset(void) { n=0; weighted=false; W=T(); }

      inline unsigned int N(void) const { return n; }

      inline T Minimum(void) const { if(n) return min; else return T(); }

      inline T Maximum(void) const { if(n) return max; else return T(); }

      inline T Average(void) const
      {
         if(n == 0) return T();
         return ave;
      }

      inline T Variance(void) const
      {
         if(n <= 1) return T();
         return (T(n)*var/T(n-1));
      }

      inline T StdDev(void) const
      {
         if(n <= 1) return T();
         return SQRT(Variance());
      }

      inline T Normalization(void) const { return W; }

      inline bool Weighted(void) const { return weighted; }

      void Add(const T& x, const T& wt_in=T())
      {
         T wt(ABS(wt_in));
         if(wt != T()) { weighted=true; }

         if(n == 0) {
            min = max = ave = x;
            var = T();
            W = T();
         }
         else {
            if(x < min) min=x;
            if(x > max) max=x;
         }

         if(weighted) {
            if(W+wt > T(1.e-10))     // if W+wt=0, nothing yet has had non-zero weight
               ave += (x-ave)*(wt/(W+wt));
            if(n > 0 && W > 1.e-10)
               var = (W/(W+wt))*var + (x-ave)*(x-ave)*(wt/W);
            W += wt;
         }
         else {
            ave += (x-ave)/T(n+1);
            if(n > 0)
               var = T(n)*var/T(n+1) + (x-ave)*(x-ave)/T(n);
         }

         n++;
      }

      inline void Add(Vector<T>& X, Vector<T> w = Vector<T>())
      {
         if(w.size() > 0 && w.size() < X.size()) {
            Exception e("Inconsistent input: weight vector short");
            GPSTK_THROW(e);
         }

         size_t i;
         if(w.size() > 0)
            for(i=0; i<X.size(); i++) Add(X(i),w(i));
         else
            for(i=0; i<X.size(); i++) Add(X(i));
      }

      void Subtract(const T x, const T wt_in=T())
      {
         if(n == 0) return;
         if(weighted) {
            if(W > T(1.e-10)) {
               T wt(ABS(wt_in));
               if(W-wt > T(1.e-10))
                  var = (var - (wt/(W-wt))*(x-ave)*(x-ave)) * (W/(W-wt));
               else
                  var = T();
               ave = (ave - wt*x/W)*W/(W-wt);
               W -= wt;
            }
            else { ave = var = W = T(); }
         }
         else {
            if(n > 1)
               var = (var - (x-ave)*(x-ave)/T(n-1))*T(n)/(T(n)-T(1));
            else
               var = T();
            ave = T(n)*(ave - x/T(n))/(T(n)-T(1));
         }
         n--;
      }

      inline void Subtract(Vector<T>& X)
      {
         for(size_t i=0; i<X.size(); i++) Subtract(X(i));
      }

      void Load(unsigned int in_n, T in_min, T in_max, T in_ave, T in_var,
                  bool wtd=false, T norm=1.0)
      {
         n = in_n;
         min = in_min;
         max = in_max;
         var = in_var;
         ave = in_ave;
         weighted = wtd;
         W = norm;
      }

      Stats<T>& operator+=(const Stats<T>& S)
      {
         if(S.n == 0) return *this;
         if(n==0) { *this = S; return *this; }
         if((weighted && !S.weighted) || (!weighted && S.weighted)) {
            Exception e("Stats::operator+= : operands have inconsistent weighting");
            GPSTK_THROW(e);
         }

         if(S.min < min) min=S.min;
         if(S.max > max) max=S.max;

         T newave, newvar;
         if(weighted) {
            if(W + S.W > T(1.e-10)) {
               newave = W*ave + S.W*S.ave;
               newvar = W*var + S.W*S.var + W*ave*ave + S.W*S.ave*S.ave;
               W += S.W;
               ave = newave/W;
               //var = (newvar-W*ave*ave)/W;
               var = newvar/W -ave*ave;
            }
         }
         else {
            newave = T(n)*ave + T(S.n)*S.ave;
            newvar = T(n)*var + T(S.n)*S.var + T(n)*ave*ave + T(S.n)*S.ave*S.ave;
            ave = newave/T(n+S.n);
            //var = (newvar-T(n+S.n)*ave*ave)/T(n+S.n);
            var = newvar/T(n+S.n) - ave*ave;
         }
         n += S.n;

         return *this;

      }

   private:
      unsigned int n;
      T min;
      T max;
      T ave;
      T var;
      T W;
      bool weighted;

   };

   // template <class T>
   // std::ostream& operator<<(std::ostream& s, const Stats<T>& ST)
   // {
   //    std::ofstream savefmt;
   //    savefmt.copyfmt(s);
   //    s << " N       = " << ST.N() << (ST.Weighted() ? " ":" not") << " weighted\n";
   //    s << " Minimum = "; s.copyfmt(savefmt); s << ST.Minimum();
   //    s << " Maximum = "; s.copyfmt(savefmt); s << ST.Maximum() << "\n";
   //    s << " Average = "; s.copyfmt(savefmt); s << ST.Average();
   //    s << " Std Dev = "; s.copyfmt(savefmt); s << ST.StdDev();
   //    s << " Variance = "; s.copyfmt(savefmt); s << ST.Variance(); // temp
   //    return s;
   // }

   template <class T>
   class TwoSampleStats
   {
   public:
      TwoSampleStats() { n=0; }

      TwoSampleStats(Vector<T>& X, Vector<T>& Y)
      {
         n = 0;
         Add(X,Y);
      }

      void Add(const T& X, const T& Y)
      {
         if(n == 0) {
            sumx = sumy = sumx2 = sumy2 = sumxy = T();
            xmin = xmax = X;
            ymin = ymax = Y;
            scalex = scaley = T(1);
         }
         if(scalex==T(1) && X!=T()) scalex=ABS(X);
         if(scaley==T(1) && Y!=T()) scaley=ABS(Y);
         sumx += X/scalex;
         sumy += Y/scaley;
         sumx2 += (X/scalex)*(X/scalex);
         sumy2 += (Y/scaley)*(Y/scaley);
         sumxy += (X/scalex)*(Y/scaley);
         if(X < xmin) xmin=X;
         if(X > xmax) xmax=X;
         if(Y < ymin) ymin=Y;
         if(Y > ymax) ymax=Y;
         n++;
      }

      void Add(const Vector<T>& X, const Vector<T>& Y)
      {
         size_t m = (X.size() < Y.size() ? X.size() : Y.size());
         if(m==0) return;
         for(size_t i=0; i<m; i++) Add(X(i),Y(i));
      }

      void Subtract(const T& X, const T& Y)
      {
         if(n == 1) {
            sumx = sumy = sumx2 = sumy2 = sumxy = T();
            xmin = xmax = T();
            ymin = ymax = T();
            scalex = scaley = T(1);
            return;
         }

         sumx -= X/scalex;
         sumy -= Y/scaley;
         sumx2 -= (X/scalex)*(X/scalex);
         sumy2 -= (Y/scaley)*(Y/scaley);
         sumxy -= (X/scalex)*(Y/scaley);
         n--;
      }

      void Subtract(const Vector<T>& X, const Vector<T>& Y)
      {
         size_t m=(X.size()<Y.size()?X.size():Y.size());
         if(m==0) return;
         for(size_t i=0; i<m; i++) Subtract(X(i),Y(i));
      }

      inline void Reset(void) { n=0; }

      inline unsigned int N(void) const { return n; }
      inline T MinimumX(void) const { if(n) return xmin; else return T(); }

      inline T MaximumX(void) const { if(n) return xmax; else return T(); }

      inline T MinimumY(void) const { if(n) return ymin; else return T(); }

      inline T MaximumY(void) const { if(n) return ymax; else return T(); }

      inline T AverageX(void) const
         { if(n>0) return (scalex*sumx/T(n)); else return T(); }

      inline T AverageY(void) const
         { if(n>0) return (scaley*sumy/T(n)); else return T(); }

      inline T VarianceX(void) const
      {
         if(n>1) return scalex*scalex*(sumx2-sumx*sumx/T(n))/T(n-1);
         else return T();
      }

      inline T VarianceY(void) const
      {
         if(n>1) return scaley*scaley*(sumy2-sumy*sumy/T(n))/T(n-1);
         else return T();
      }

      inline T StdDevX(void) const { return SQRT(VarianceX()); }

      inline T StdDevY(void) const { return SQRT(VarianceY()); }

      inline T Slope(void) const
      {
         if(n>0)
            return ((scaley/scalex)*(sumxy-sumx*sumy/T(n))/(sumx2-sumx*sumx/T(n)));
         else
            return T();
      }

      inline T Intercept(void) const
      {
         if(n>0)
            return (AverageY()-Slope()*AverageX());
         else
            return T();
      }

      inline T SigmaSlope(void) const
      {
         if(n>2)
            return (SigmaYX()/(StdDevX()*SQRT(T(n-1))));
         else
            return T();
      }

      inline T Correlation(void) const
      {
         if(n>1)
         {
            return ( scalex * scaley * (sumxy-sumx*sumy/T(n))
               / (StdDevX()*StdDevY()*T(n-1)) );
         }
         else
            return T();
      }

      inline T SigmaYX(void) const
      {
         if(n>2)
         {
            return (StdDevY() * SQRT(T(n-1)/T(n-2))
                  * SQRT(T(1)-Correlation()*Correlation()) );
         }
         else return T();
      }

      TwoSampleStats<T>& operator+=(TwoSampleStats<T>& S)
      {
         if(n + S.n == 0) return *this;
         if(S.xmin < xmin) xmin=S.xmin;
         if(S.xmax > xmax) xmax=S.xmax;
         if(S.ymin < ymin) ymin=S.ymin;
         if(S.ymax > ymax) ymax=S.ymax;
         sumx += S.scalex*S.sumx/scalex;
         sumy += S.scaley*S.sumy/scaley;
         sumx2 += (S.scalex/scalex)*(S.scalex/scalex)*S.sumx2;
         sumy2 += (S.scaley/scaley)*(S.scaley/scaley)*S.sumy2;
         sumxy += (S.scalex/scalex)*(S.scaley/scaley)*S.sumxy;
         n += S.n;
         return *this;
      }

   private:
      unsigned int n;
      T xmin, xmax, ymin, ymax, scalex, scaley;
      T sumx, sumy, sumx2, sumy2, sumxy;

   };

   // template <class T>
   // std::ostream& operator<<(std::ostream& s, const TwoSampleStats<T>& TSS)
   // {
   //    std::ofstream savefmt;
   //    savefmt.copyfmt(s);
   //    s << " N           = " << TSS.N() << "\n";
   //    s << " Minimum:  X = "; s.copyfmt(savefmt); s << TSS.MinimumX();
   //    s << "  Y = "; s.copyfmt(savefmt); s << TSS.MinimumY() << "\n";
   //    s << " Maximum:  X = "; s.copyfmt(savefmt); s << TSS.MaximumX();
   //    s << "  Y = "; s.copyfmt(savefmt); s << TSS.MaximumY() << "\n";
   //    s << " Average:  X = "; s.copyfmt(savefmt); s << TSS.AverageX();
   //    s << "  Y = "; s.copyfmt(savefmt); s << TSS.AverageY() << "\n";
   //    s << " Std Dev:  X = "; s.copyfmt(savefmt); s << TSS.StdDevX();
   //    s << "  Y = "; s.copyfmt(savefmt); s << TSS.StdDevY() << "\n";
   //    s << " Variance: X = "; s.copyfmt(savefmt); s << TSS.VarianceX();
   //    s << "  Y = "; s.copyfmt(savefmt); s << TSS.VarianceY() << "\n";
   //    s << " Intercept = "; s.copyfmt(savefmt); s << TSS.Intercept();
   //    s << "  Slope = "; s.copyfmt(savefmt); s << TSS.Slope();
   //    s << " with uncertainty = "; s.copyfmt(savefmt); s << TSS.SigmaSlope() << "\n";
   //    s << " Conditional uncertainty (sigma y given x) = ";
   //    s.copyfmt(savefmt); s << TSS.SigmaYX();
   //    s << "  Correlation = "; s.copyfmt(savefmt); s << TSS.Correlation();
   //    return s;
   // }

   // template <class T>
   // inline T median(const Vector<T>& v)
   // {
   //    if(v.size()==0) return T();
   //    if(v.size()==1) return v(0);
   //    if(v.size()==2) return (v(0)+v(1))/T(2);
   //    // insert sort
   //    int i,j;
   //    T x;
   //    Vector<T> w(v);
   //    for(i=0; i<v.size(); i++) {
   //       x = w[i] = v(i);
   //       j = i-1;
   //       while(j>=0 && x<w[j]) {
   //          w[j+1] = w[j];
   //          j--;
   //       }
   //       w[j+1] = x;
   //    }
   //    if(v.size() % 2)
   //       x=w[(v.size()+1)/2-1];
   //    else
   //       x=(w[v.size()/2-1]+w[v.size()/2])/T(2);

   //    return x;
   // }

   // template <class T>
   // inline T median(const std::vector<T>& v)
   // {
   //    if(v.size()==0) return T();
   //    if(v.size()==1) return v[0];
   //    if(v.size()==2) return (v[0]+v[1])/T(2);
   //    // insert sort
   //    int i,j;
   //    T x;
   //    std::vector<T> w(v);
   //    for(i=0; i<v.size(); i++) {
   //       x = w[i] = v[i];
   //       j = i-1;
   //       while(j>=0 && x<w[j]) {
   //          w[j+1] = w[j];
   //          j--;
   //       }
   //       w[j+1] = x;
   //    }
   //    if(v.size() % 2)
   //       x=w[(v.size()+1)/2-1];
   //    else
   //       x=(w[v.size()/2-1]+w[v.size()/2])/T(2);

   //    return x;
   // }

}

#endif

%extend gpstk::Stats {
   std::string __str__()
   {
 	   std::stringstream s;
      std::ofstream savefmt;
      savefmt.copyfmt(s);
      s << " N       = "; s << (*($self)).N() << ((*($self)).Weighted() ? " ":" not") << " weighted\n";
      s << " Minimum = "; s.copyfmt(savefmt); s << (*($self)).Minimum();
      s << " Maximum = "; s.copyfmt(savefmt); s << (*($self)).Maximum() << "\n";
      s << " Average = "; s.copyfmt(savefmt); s << (*($self)).Average();
      s << " Std Dev = "; s.copyfmt(savefmt); s << (*($self)).StdDev();
      s << " Variance = "; s.copyfmt(savefmt); s << (*($self)).Variance();
      return s.str();
   }
}

%extend gpstk::TwoSampleStats {
   std::string __str__()
   {
 	   std::stringstream s;
      std::ofstream savefmt;
      savefmt.copyfmt(s);
      s << " N           = " << (*($self)).N() << "\n";
      s << " Minimum:  X = "; s.copyfmt(savefmt); s << (*($self)).MinimumX();
      s << "  Y = "; s.copyfmt(savefmt); s << (*($self)).MinimumY() << "\n";
      s << " Maximum:  X = "; s.copyfmt(savefmt); s << (*($self)).MaximumX();
      s << "  Y = "; s.copyfmt(savefmt); s << (*($self)).MaximumY() << "\n";
      s << " Average:  X = "; s.copyfmt(savefmt); s << (*($self)).AverageX();
      s << "  Y = "; s.copyfmt(savefmt); s << (*($self)).AverageY() << "\n";
      s << " Std Dev:  X = "; s.copyfmt(savefmt); s << (*($self)).StdDevX();
      s << "  Y = "; s.copyfmt(savefmt); s << (*($self)).StdDevY() << "\n";
      s << " Variance: X = "; s.copyfmt(savefmt); s << (*($self)).VarianceX();
      s << "  Y = "; s.copyfmt(savefmt); s << (*($self)).VarianceY() << "\n";
      s << " Intercept = "; s.copyfmt(savefmt); s << (*($self)).Intercept();
      s << "  Slope = "; s.copyfmt(savefmt); s << (*($self)).Slope();
      s << " with uncertainty = "; s.copyfmt(savefmt); s << (*($self)).SigmaSlope() << "\n";
      s << " Conditional uncertainty (sigma y given x) = ";
      s.copyfmt(savefmt); s << (*($self)).SigmaYX();
      s << "  Correlation = "; s.copyfmt(savefmt); s << (*($self)).Correlation();
      return s.str();
   }
}

%template(Stats_double) gpstk::Stats<double>;
%template(TwoSampleStats_double) gpstk::TwoSampleStats<double>;