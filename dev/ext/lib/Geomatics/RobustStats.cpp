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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

/**
 * @file RobustStats.cpp
 * Namespace Robust includes basic robust statistical computations, including median,
 * median average deviation, quartiles and m-estimate, as well as implementation of
 * of stem-and-leaf plots, quantile plots and robust least squares estimation of a
 * polynomial.
 * Reference: Mason, Gunst and Hess, "Statistical Design and
 *            Analysis of Experiments," Wiley, New York, 1989.
 */
 
//------------------------------------------------------------------------------------
// GPSTk includes
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "Matrix.hpp"
#include "SRI.hpp"
#include "RobustStats.hpp"

//------------------------------------------------------------------------------------
// moved to RobustStats.hpp as macros
//const double gpstk::Robust::TuningT=1.5;      // or 1.345;       // or 1.5
//const double gpstk::Robust::TuningA=0.778;    // or 0.67;        // or 0.778
//const double gpstk::Robust::TuningE=0.6745;

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
inline long Stem(double x, double& scale) { return (long(x/scale)); }

//------------------------------------------------------------------------------------
void Robust::StemLeafPlot(ostream& os, double *xd, long nd, string msg)
   throw(Exception)
{
   long stem, l, nout=0, s=0, sM, sQ1, sQ3, sOH, sOL;
   int i, sign, pos, k, leaf;
   unsigned len=0, kk;
   char c;
   double x,scale;
   string buf;

   if(!xd || nd < 2) GPSTK_THROW(Exception("Invalid input"));

      // find range 
   double range = xd[nd-1] - xd[0];                   // max - min
   if(range < 0.0)
      GPSTK_THROW(Exception("Array is not sorted"));
   if(range == 0.0)
      range = xd[0]; //GPSTK_THROW(Exception("Array has zero range"));

      // find scale
   scale = 0.0;
   short nscale=0;       // scale = 1*10^nscale
   if(range >= 10.0)
      sign = 1;
   else if(range < 1.0)
      sign = -1;
   else
      scale = 1.0;

   if(!scale) do {
      nscale += sign;
   } while(range*::pow(10.0,-nscale) < 1.0 || range*::pow(10.0,-nscale) >= 10.0);

   scale = ::pow(10.0,nscale);

   double M=Robust::Median(xd,nd);
   double Q1,Q3;
   Robust::Quartiles(xd,nd,Q1,Q3);
      // outlier limits
   double OH=2.5*Q3-1.5*Q1;         // outlier high limit
   double OL=2.5*Q1-1.5*Q3;         // outlier low limit ('oh L' not 'zero L')

      // starting stem=stem(min=xd[0]), and stem step==scale
   i = 1+short((range/scale)+0.5);              // number of stems
   if(xd[0]*xd[nd-1] < 0.0) i++;                // add one stem for zero
   if(nd > 8 && i < 8 && xd[nd-1] != xd[0]) {   // fudge so #stems is big enough
      scale /= 10.0;
      nscale--;
   }

      // find length of stem for printing
   buf = asString<long>(::abs(Stem(xd[0],scale)));
   len = buf.size();
   buf = asString<long>(::abs(Stem(xd[nd-1],scale)));
   if(len < buf.size()) len=buf.size();
   buf = asString<long>(::abs(Stem(M,scale)));
   if(len < buf.size()) len=buf.size();

      // loop through data, adding stems and leaves to plot
   bool start=true;
   if(xd[0] < 0.0) pos=-1; else pos=1;
   sM = Stem(M,scale);
   sQ1 = Stem(Q1,scale);
   sQ3 = Stem(Q3,scale);
   sOH = Stem(OH,scale);
   sOL = Stem(OL,scale);
   for(l=0; l<nd; l++) {
         // current: stem=s,pos; data=stem,sign(xd[l])
      if(xd[l]>OH || xd[l]<OL) nout++;                   // count outliers
      sign = 1;
      if(xd[l] < 0.0) sign=-1;
      stem = Stem(fabs(xd[l]),scale);
      x = 10*fabs(xd[l]/scale-sign*stem);
      leaf = short(x + 0.5);
      if(leaf == 10) {
         stem++;
         leaf=0;
      }
      stem = sign*stem;

      // print it
      if(start || s!=stem || (s==0 && pos*sign<0.0)) {
         // Change of stem -> print
         if(start) {                                        // first time through
            os << "Stem and Leaf Plot (scale ";             // print scale
            i=nscale;
            if(nscale < 0) {
               os << "0.";
               i++;
               k = 1;
            }
            else {
               os << "1";
               k = -1;
            }
            while(i != 0) {
               os << "0";
               i += k;
            }
            if(nscale < 0)
               os << "1";
            else
               os << ".0";
            os << ", " << nd << "pts) : ";                  // print npts
            if(msg.size() > 0) os << msg;                   // and message
            s = stem - 1;                                   // save for later
            start = false;
         }

         while(s < stem || (s==0 && pos*sign<0)) { // also print stems without leaves
            if(s != 0) s++;
            else if(pos < 0) pos = 1;
            else s++;

               // print the new line with stem s
            os << "\n";
            buf = asString<long>(s < 0 ? -s : s); // abs(stem)

            //for(kk=buf.size(); kk<len; kk++) os << " ";
            //if(s<0) c='-'; else if(s>0) c='+'; else if(pos>0)c='+'; else c='-';
            //os << c;
            //os << buf << " ";

            if(s<0) c='-';                                     // sign of stem
            else if(s>0) c='+';
            else if(pos>0)c='+';
            else c='-';
            os << c;

            for(kk=buf.size(); kk<len; kk++) os << " ";        // pad out to length
            os << buf << " ";                                  // stem/axis space

               // now print either |, M (median), Q (quartiles), or >< (outliers)
            k=0;

               // if s==sM
            if(s==sM && (s!=0 || pos*M>0.0)) {
               os << "M";                       // marks the median
               k++;
            }

            if((s == sQ3 && (s != 0 || pos*Q3 < 0.0)) ||
                  (s == sQ1 && (s != 0 || pos*Q1 < 0.0) )) {
               os << "Q";                       // marks a quartile
               k++;
            }

            if((s < sOL || (s == 0 && sOL == 0 && (pos == -1 && OL > 0.0))) ||
               (s == sOL && (s != 0 || pos*OL > 0.0))) {
               os << "<";                       // marks an outlier (small)
               k++;
            }
            else if((s > sOH || (s == 0 && sOH == 0 && (pos == 1 && OH < 0.0))) ||
               (s == sOH && (s != 0 || pos*OH > 0.0))) {
               os << ">";                       // marks an outlier (big)
               k++;
            }

            if(k == 0) {
               os << "|";                       // marks a regular point
               k++;
            }

            while(k < 3) {
               os << " ";
               k++;
            }
         }
      }     // end change stem

         // print leaf
      os << leaf;
   }

   os << "\nEND Stem and Leaf Plot (there are " << nout << " outliers.)\n";

}  // end StemLeafPlot


void Robust::QuantilePlot(double *yd, long nd, double *xd)
   throw(Exception)
{
   if(!xd || nd<2 || !yd) {
      Exception e("Invalid input");
      GPSTK_THROW(e);
   }

   double f;
   for(int i=0; i<nd; i++) {
      f = double(8*i+5)/double(8*nd+2);         // f(i) = i-3/8 / n+1/4, i=1,n
      xd[i] = 4.91*(::pow(f,0.14) - ::pow(1-f,0.14));
   }

}  // end QuantilePlot


#define SEQUENTIAL 1     // don't see much difference in timing...
int Robust::RobustPolyFit(double *xd, const double *td, int nd,
                          int N, double *c, double *w)
   throw(Exception)
{
   try {
      if(!xd || !td || !c || nd < 2) {
         Exception e("Invalid input");
         GPSTK_THROW(e);
      }

      const int maxiter=50;
      const double conv_limit=::sqrt(double(nd))*1.e-3;
      int i,j,niter;
      double x0=xd[0],t0=td[0],mad,median,conv;
#ifdef SEQUENTIAL
      double fit,dt;
      Matrix<double> R,A(1,N+1),invR;
      Vector<double> Z;
#else
      Matrix<double> PT,P(nd,N,1.0);
      Vector<double> D(nd);
#endif
      Matrix<double> Cov;
      Vector<double> Wts(nd,1.0), Coeff(N,0.0), Res(nd), ResCopy(nd);

#ifndef SEQUENTIAL
      // build the data vector and the (constant) partials matrix
      for(i=0; i<nd; i++) {
         D(i) = xd[i]-x0;
         for(j=1; j<N; j++)
            P(i,j) = P(i,j-1)*(td[i]-t0);
      }
#endif

      // iterate until weights don't change
      niter = 0;
      while(1) {
#ifdef SEQUENTIAL
         R = Matrix<double>(N,N,0.0);
         Z = Vector<double>(N,0.0);
         // loop over the data
         for(i=0; i<nd; i++) {
            //if(Wts(i) < 1.e-8) continue;             // ignore if weight is very small
            A(0,N) = (xd[i]-x0)*Wts(i);              // data
            dt = td[i]-t0;       
            A(0,0) = 1.0*Wts(i);                     // partials
            for(j=1; j<N; j++) A(0,j) = A(0,j-1)*dt;
            SrifMU<double>(R,Z,A,1);
         }
#else
         // compute partials transpose multiplied by 'weight matrix'=diag(squared wts)
         PT = transpose(P);
         for(i=0; i<N; i++)
            for(j=0; j<nd; j++)
               PT(i,j) *= Wts(j)*Wts(j);
         Cov = PT * P;        // information matrix
#endif

         // solve
         try {
#ifdef SEQUENTIAL
         invR = inverseUT(R,&mad,&median);
         Cov = UTtimesTranspose(invR);
         Coeff = invR * Z;
#else
         Cov = inverse(Cov);
         Coeff = Cov * PT * D;
#endif
         }
         catch(Exception& e) { return -1; }

         // compute residuals
#ifdef SEQUENTIAL
         // loop over the data
         for(i=0; i<nd; i++) {
            fit = Coeff(N-1);                      // fit = partials * coeff
            dt = td[i]-t0;                         // delta time
            for(j=N-2; j>=0; j--)
               fit = fit*dt + Coeff(j);
            ResCopy(i) = Res(i) = xd[i]-x0 - fit;  // data - fit
         }
#else
         ResCopy = Res = D - P*Coeff;
#endif

         // compute median and MAD. NB Median() will sort the vector...
         mad = MedianAbsoluteDeviation(&(ResCopy[0]),ResCopy.size(),median);

         // recompute weights
         Vector<double> OldWts(Wts);
         for(i=0; i<nd; i++) {
            if(Res(i) < -RobustTuningT*mad)
               Wts(i) = -RobustTuningT*mad/Res(i);
            else if(Res(i) > RobustTuningT*mad)
               Wts(i) = RobustTuningT*mad/Res(i);
            else
               Wts(i) = 1.0;
         }

         // test for convergence
         niter++;
         conv = RMS(OldWts - Wts);
         if(niter > maxiter) break; // return -2;
         if(conv > 1.) break; // return -3;
         if(niter > 2 && conv < conv_limit) break;
      }

      // copy out weights, residuals and solution
      for(i=0; i<N; i++) c[i] = Coeff(i);
      //c[0] += x0;
      for(i=0; i<nd; i++) {
         xd[i] = Res(i);
         if(w) w[i] = Wts(i);
      }
      if(niter > maxiter) return -2;
      if(conv > 1.) return -3;

#undef SEQUENTIAL
      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e) {
      Exception E("std except: "+string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

   // Anderson-Darling test statistic, which is a variant of the Kolmogorov-Smirnoff
   // test, comparing the distribution of data with mean m and standard deviation s
   // to the normal distribution.
   // @param xd         array of data.
   // @param nd         length of array xd.
   // @param mean       mean of the data.
   // @param stddev     standard deviation of the data.
   // @param save_flag  if true (default) array xd will NOT be changed, otherwise
   //                      it will be sorted.
double gpstk::ADtest(double *xd, const int nd,
                     double mean, double stddev, bool save_flag)
      throw(Exception)
{
   if(!xd || nd < 2) {
      Exception e("Invalid input");
      GPSTK_THROW(e);
   }

   try {
      int i;
      double *save;

      if(save_flag) {
         save = new double[nd];
         if(!save) {
            Exception e("Could not allocate temporary array");
            GPSTK_THROW(e);
         }
         for(i=0; i<nd; i++) save[i]=xd[i];
      }
      QSort(xd,nd);

      double tn = double(nd);
      double AD = -tn, cdf;
      for(i=0; i<nd; i++) {
         cdf = normalCDF<double>(mean, stddev, xd[i]);
         //cout << "CDF " << setw(4) << i
         //     << " " << fixed << setprecision(6) << setw(11) << xd[i]
         //     << " " << setw(11) << cdf << endl;
         AD -= ((2.0*double(i)-1.0) * ::log(cdf)
               + (2.0*(tn-double(i))+1.0) * ::log(1.0-cdf))/tn;
      }

      AD *= 1.0 + (0.75+2.25/tn)/tn;

      if(save_flag) {
         for(i=0; i<nd; i++) xd[i]=save[i];
         delete[] save;
      }

      return AD;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e) {
      Exception E("std except: "+string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
