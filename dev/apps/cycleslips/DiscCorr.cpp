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
 * @file DiscCorr.cpp
 * Discontinuity correction of GPS carrier phase data.
 */

//------------------------------------------------------------------------------------
// The algorithm used in this code is based on the paper by Geoffrey Blewitt:
// "An Automatic Editing Algorithm for GPS Data", Geophysical Research Letters,
// Vol. 17, No. 3, pp 199-202, 1990.
//
// The algorithm requires input of dual frequency pseudorange and carrier phase
// data (assumed expressed in units of meters and cycles, respectively),
// usually for an entire satellite pass; these are called P1,P2,L1 & L2.
// (The input - see class SVPass - consists of 4 parallel double arrays named
// P1,P2,L1,L2, plus a parallel integer array Flag.)
//
// Obvious outliers and bad points are marked, and then linear combinations of
// these four data are formed, as follows.
// L1 -> L1
// L2 -> L1-L2 = geometry-free phase or GFP.
//             GFP = [(f1/f2)^2-1]*I + wl1*N1 - wl2*N2
// GFP is also called the ionospheric phase, b/c it ~= ionosphere + bias
//
// Also,
//             Let WLP = (f1*L1-f2*L2)/(f1-f2) (the wide lane phase)
//             and NLR = (f1*P1+f2*P2)/(f1+f2) (the narrow lane range), then
// P1 -> WLP-NLR = the Wide-Lane Phase minus the Narrow-Lane Range, sometimes
//             called (sic) 'wide-lane range minus phase' or
//             the Melbourne-Wubbena combination;
//             Here it is 'wide-lane bias' or WLB because WLP-NLR = wlwl*NWL,
//             where NWL = N1 - N2;
// i.e. it behaves as if it were merely a carrier of a different frequency.
//
// P2 -> P2-P1 = the negative of the geometry-free range or -GFR
//             -GFR = [(f1/f2)^2-1]*I
// Note that GFP-GFR does NOT have the 'new carrier' property that WLB does,
// since GFP-GFR = wl1*N1-wl2*N2, and this combo cannot be set equal to wlX*NX.
// 
// The following definitions are used above and in the code:
// I = ionospheric delay (meters) on L1.
// f0 = GPS fundamental frequency = 10.23MHz
// f1 = frequency multiplier for L1 = 154
// f2 = frequency multiplier for L2 = 120
// c = speed of light = 299792458.0 m/s
// wl1 = wavelength of L1 = f1*f0/c = 19.0cm
// wl2 = wavelength of L2 = f2*f0/c = 24.4cm
// wlwl= wavelength of WL = 86.2cm
// wl21= wl2-wl1 = 5.4cm
// N1 and N2 are the biases on the phases L1 and L2;
//    i.e. ideally P1 = wl1*(L1+N1) and P2 = wl2*(L2+N2).
// NWL = N1-N2, and note that GFP+GFR = wl1*N1-wl2*N2 = wl2*NWL-wl21*N1
//
// [In practice here N1, N2 and NWL will refer to CHANGES, or 'slips', in these
// biases, rather than the biases themselves.]
//       
//
// The algorithm takes the approach of resolving the wide lane bias, then
// correcting the GFP for NWL and then solving for N1 using the GFP. This is
// done in four steps.
//
// 1. Detect slips (changes in bias) in the wide-lane bias.
//    This is done by computing statistics on the WL bias, using two sliding
//    windows of fixed width, one including the future of the point and the
//    point itself, the other containing the past of the point. The difference
//    in averages in these two windows is compared with the total variance.
//    By carefully examining the test and the standard deviations of the two
//    windows, we can mark outliers and identify slips.
//    After detecting large slips this way, the dual-sliding-window-statistics
//    process is repeated with a much larger window width, in order to find
//    small slips.
//    At the end of this part, the WL data is divided into continuous segments,
//    each with statistics (average and std dev) computed.
//
// 2. Estimate ('fix') the wide-lane bias, and correct the GFP for WL slips.
//    The estimate is simply the difference of the average WLbias in the two
//    adjoining segments on either side of the slip. If successful, the first
//    point in the future of the slip is marked with a flag 'WL fixed', and
//    the estimated NWL slip is used to correct the GFP.
//
// 3. Detect slips in the GFP (i.e. changes in N1, with effective wl=5.4cm).
//    GFP now == [(f1/f2)^2-1]*I - wl21*N1, where wl21=5.4cm. To detect slips
//    in the GFP, a polynomial is fit to the GF range and added (note signs)
//    to the GFP. This has the effect of removing a smoothed estimate of the
//    ionospheric term I that is common to both GFP and GFR. The degree of the
//    polynomial fit is determined by the number of points in the segment and
//    the size of the gap between segments; it can also be increased by user
//    input (parameter GFPolynomMaxDeg). The GF residual [=GFP+Fit(GFR)] is 
//      scanned for discontinuities; outliers may be rejected and new slips may
//    be found. New (GF only) slips are marked with a 'GF detected' flag, and
//    old (WL) slips are also tested and marked when a GF slip is found.
//    At the end of this step, the data is cleaned (bad points are marked)
//    and divided into segments, with the beginning of each segment (a good pt)
//    marked with either 'WL detected' or 'WL fixed' and/or 'GF detected'.
//
//    Note that slips in the WL only, or in the GF only, are the same as slips
//    in both WL and GF, they simply happen to have particular values of N1
//    and/or N2. Specifically, if N1==N2 then NWL=0 and there is no slip in the
//    WLbias; and if N1==0, NWL = -N2 and there is no GF slip.
//
// 4. Fix slips in the GFP.
//    Slips in the GFP are estimated by fitting a polynomial to the GFP data on
//    each side of the detected slip. The RMS residual of fit on both sides of
//    the slip are used to judge whether the estimation is acceptable. If it is
//    not, the number of data points used in the fit is first halved, then
//    doubled, and the fix is attempted again.
//
//------------------------------------------------------------------------------------
// A note about Debug. Debug is an int, set using the Debug,<n> DC command. The
// value of <n> determines how much output there is, as follows.
//  n   output: this plus everything at smaller n
// ---- ----------------------------------------------------
//  0   nothing
//  1   Summary of input
//  2   High level actions, change in GDC config, results summary, timing
//  3   Actual results (Rinex Editor commands), stats on SVPass smoothing
//  4   All SVPass information - defined, filled, processed, done, etc.
//  5   List of GDC segments, dump data: before,linear combo, WL, GF, after
//      processing.
//  6   Dump WL statistical, and GF range fit, data
//  7   Editing actions, outliers, GFR fit and slip fixing stats and decisions.
//
// Debug > 2 is pretty expensive (in time) because the iostreams library is slow.
//------------------------------------------------------------------------------------
// TD
// GFSlipFix should use n *good* points on either side of slip, not just
//  n values of the index - there are cases where you get too few points. Could
//  this be solved by marking as bad isolated points near the slip? - gap size?
// ** Real data shows trends in the WLB, enough so that the avereage of a large
//  segment can be influenced adversely, giving the wrong WL slip. Consider
//  using a smaller portion of large segments to compute the avereage in WLSFix.
//  Or perhaps look at sigma and decrease the window width when sig is large -
//   no, this seems to be too insensitive - perhaps fit a slope.
// Gap size -- it could be much larger, especially at low latitudes.
// Are the DumpSegment data for WLL WLS and WLF the same?
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#include "MathBase.hpp"
#include "StringUtils.hpp"
#include "PolyFit.hpp"
#include "DiscCorr.hpp"
#include "Stats.hpp"
#include "icd_200_constants.hpp"    // PI,C_GPS_M,OSC_FREQ,L1_MULT,L2_MULT

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
//string GDCVersion("4.0 12/12/2003");   // Version
string GDCVersion("4.1 08/16/2005");   // Version
int GDCUnique=0;                       // unique number for each call
int GDCNFail,GDCNFix,GFSj;             // for output
int ibad[10];                          // TD
double dbad[10];                       // TD
#define LGF *(GDC.oflog)

//------------------------------------------------------------------------------------
// Segment - used internally only.
// Define an object to hold information about Segments = periods of continuous
// phase; keep a linked list of these objects, and subdivide whenever a
// discontinuity is detected.
class Segment {
public:
      // member data
   int nbeg,nend;          // array indexes of the first and last good points
   int npts;               // number of good points in this Segment
   double bias1,bias2;     // biases for this Segment: WL,GF, then L1,L2
   long NWL,N1;            // slip fixes for WL (N1-N2) and GF (=N1)
   Stats<double> WLStat;   // includes N,min,max,ave,sig
   PolyFit<double> PF;     // for fit to GF range
   double RMSROF;          // RMS residual of fit of polynomial (PF) to GFR
      // member functions
   Segment(void)
   { WLStat.Reset(); nend=nbeg=npts=0; bias1=bias2=0; N1=NWL=0; }
   Segment(const Segment& s)
   { npts=s.npts; nbeg=s.nbeg; nend=s.nend; NWL=s.NWL; N1=s.N1;
      bias1=s.bias1; bias2=s.bias2; WLStat=s.WLStat; PF=s.PF; }
   ~Segment(void) {}
   Segment& operator=(const Segment& s)
   { if(this==&s) return *this; nbeg=s.nbeg; nend=s.nend; NWL=s.NWL;
	   N1=s.N1; bias1=s.bias1; bias2=s.bias2;
      WLStat=s.WLStat; PF=s.PF; return *this; }
}; // end class Segment

//------------------------------------------------------------------------------------
// SegList is a list of Segments, always in time order, of segments of
// continuous data within the SVPass.
list<Segment> SegList;

//------------------------------------------------------------------------------------
// internal function prototypes
int Preprocess(SVPass& SVP, GDCConfig& C);
int LinearCombinations(SVPass& SVP, GDCConfig& C);
int WLSlipDetect(SVPass& SVP, GDCConfig& C);
int WLSlipDetectLarge(SVPass& SVP, GDCConfig& C);
int WLSlipDetectSmall(SVPass& SVP, GDCConfig& C);
int WLStatSweep(SVPass& SVP, GDCConfig& C, int nb, int ne, int iwid, int iseg);
void DivideWLSegments(SVPass& SVP, GDCConfig& C);
int WLSlipFix(SVPass& SVP, GDCConfig& C);
int GFSlipDetect(SVPass& SVP, GDCConfig& C);
int GFRangePolynomialFit(SVPass& SVP,GDCConfig& C,list<Segment>::iterator& it,int j);
int GFSlipFix(SVPass& SVP, GDCConfig& C);
int CorrectData(SVPass& SVP, GDCConfig& C);
void CorrectSegments(SVPass& SVP, GDCConfig& C);
void DumpSegments(const string& lab, int N, GDCConfig& C, SVPass& SVP);
int OutputAndQuit(SVPass& SVP, GDCConfig& C, vector<string>& EditCmds, int iret);

//------------------------------------------------------------------------------------
// constants used in linear combinations
const double CFF=C_GPS_M/OSC_FREQ;
const double F1=L1_MULT;   // 154.0;
const double F2=L2_MULT;   // 120.0;
const double f12=F1*F1;
const double f22=F2*F2;
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
   // ionosphere-free R and Ph
const double if1r=f12/(f12-f22);
const double if2r=-f22/(f12-f22);
const double if1p=wl1*f12/(f12-f22);
const double if2p=-wl2*f22/(f12-f22);
   // for geometry-free R and Ph
const double gf1r=-1;
const double gf2r=1;
const double gf1p=wl1;
const double gf2p=-wl2;

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// This is the main entry point.
// Return values (int) (used by all routines within this module):
const int FatalProblem=-3;
const int PrematureEnd=-2;
const int Singular=-1;
const int ReturnOK=0;
//------------------------------------------------------------------------------------
int gpstk::GPSTKDiscontinuityCorrector(SVPass& SVP, GDCConfig& GDC, vector<string>& EditCmds)
{
try {
   int iret;
   DayTime CurrentTime;
   CurrentTime.setLocalTime();

   GDCUnique++;
   GDCNFail = GDCNFix = 0;

   if(GDC.Debug>4) {
      LGF << "\n========Beg GPSTK Discontinuity Corrector " << GDCUnique
         << "================================================\n";
      LGF << "GPSTK Discontinuity Corrector Ver. " << GDCVersion << " Run "
         << CurrentTime << endl;
      LGF << "  SV:" << SVP.SV << " Npts: " << SVP.Npts << " Len:" << SVP.Length
         << " Times:(" << SVP.BegTime << ")-(" << SVP.EndTime << ")\n";
   }

      // check input
   if(GDC.DT <= 0) {
      LGF << "Error: DT is not set in configuration\n";
      if(GDC.Debug>1) LGF << "GDC " << setw(2) << GDCUnique << ", PRN " << SVP.SV
         << ", Pts " << setw(4) << SVP.Npts << ", Slip 0, Fix 0, Fail 0\n";
      if(GDC.Debug>4) LGF << "========End GPSTK Discontinuity Corrector "
         << "==================================================\n";
      return FatalProblem;
   }

      // MaxGap should be the smaller of WL and GF MaxGap
   GDC.MaxGap = GDC.WLFixMaxGap;
   if(GDC.MaxGap > GDC.GFFixMaxGap) GDC.MaxGap=GDC.GFFixMaxGap;

      // --------------------------------------------------------------------
      // Create the first Segment (entire SVPass), scan for gaps -> new Segments
      // Preprocess the data: Look for outliers and obvious bad points, and mark
   iret = Preprocess(SVP, GDC);
   if(iret) goto quit;

      // --------------------------------------------------------------------
      // Compute linear combinations: L2->GF phase, P1->WLbias, P2->-GF Range
      // Define a bias for WLbias and GF phase for each segment.
   iret = LinearCombinations(SVP, GDC);
   if(iret) goto quit;

      // --------------------------------------------------------------------
      // Look for slips and outliers in the widelane range minus phase
      // Divide into more than one Segment when a discontinuity is detected
   iret = WLSlipDetect(SVP, GDC);
   if(iret) goto quit;

      // --------------------------------------------------------------------
      // Estimate the WL slip, using statistics in each segment, and set the
      // value of NWL.
   iret = WLSlipFix(SVP, GDC);
   if(iret) goto quit;

      // --------------------------------------------------------------------
      // Loop over Segments: compute the WL slip, and correct the GF phase.
      // Fit a polynomial to the GF range, and replace P2(-GFR) with
      //   residual (GFPh + fit(GFR)) divided by wl21 [=N1];
      // Further divide into Segments when the change in the residual is large.
   iret = GFSlipDetect(SVP, GDC);
   if(iret) goto quit;

      // --------------------------------------------------------------------
      // Loop over Segments: Fit polynomials to either side of the GF slips;
      // Fix the GF slips.
   iret = GFSlipFix(SVP, GDC);
   if(iret) goto quit;
      
      // --------------------------------------------------------------------
      // Loop over the data, applying the corrections to the L1 and L2 arrays
      // Editing commands are generated in OutputAndQuit
   iret = CorrectData(SVP, GDC);

quit:
   return OutputAndQuit(SVP,GDC,EditCmds,iret);
}
catch(gpstk::Exception& e) {
   LGF << "GPSTK_Discontinuity_Corrector threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end GPSTKDiscontinuityCorrector(SVPass& SVP, GDCConfig& GDC, vector<string>& EditCmds)

//------------------------------------------------------------------------------------
// Preprocess the data: Look for outliers and obvious bad points, and mark them.
// Create the first Segment and look for gaps => new Segments
// After calling this routine the arrays are L1 = L1 phase (cycles)
//                                           L2 = L2 phase (cycles)
//                                           P1 = L1 range (meters)
//                                           P2 = L2 range (meters)
int Preprocess(SVPass& SVP, GDCConfig& GDC)
{
try {
   int i,ilast;
   list<Segment>::iterator Sit;

   if(GDC.Debug > 5) LGF << "PP..............................................."
      << ".................................\n";

   if(SVP.Npts <= 0) {
      if(GDC.Debug>4) LGF << "Abort GDC: No points in SV pass\n";
      return PrematureEnd;
   }
   
   // create the first segment
   SegList.clear();
   {
      Segment S;
      S.nbeg = S.npts = 0;
      S.nend = SVP.Length-1;
      // add it to the list of segments within this SVPass
      SegList.push_back(S);
      // Sit will always point to the current segment
      Sit = SegList.begin();
   }

   // -------------------------------------------
   // loop over points in the pass
   ilast = -1;
   for(i=0; i<SVP.Length; i++) {
      // edit the data
      if(SVP.Flag[i] == SVPass::OK) {                    // data is not marked
         // look for obvious outliers
         if(SVP.P1[i] < GDC.MinRange || SVP.P1[i] > GDC.MaxRange ||
            SVP.P2[i] < GDC.MinRange || SVP.P2[i] > GDC.MaxRange ||
            SVP.P1[i]==0 || SVP.P2[i]==0 || SVP.L1[i]==0 || SVP.L2[i]==0)
         {
            SVP.Flag[i] = SVPass::SETBAD;                // mark it bad
            // (CorrectData will set it BAD)

            if(GDC.Debug>6) {
               DayTime ttag(SVP.BegTime);
               ttag += double(i)*GDC.DT;
               LGF << "Obvious outlier " << GDCUnique << " " << SVP.SV
                  << " at # " << i << " " << ttag << " (";
               if(SVP.L1[i]==0) LGF << " 0L1";
               if(SVP.L2[i]==0) LGF << " 0L2";
               if(SVP.P1[i]==0) LGF << " 0P1"; else
               if(SVP.P1[i] < GDC.MinRange) LGF << " -P1"; else
               if(SVP.P1[i] > GDC.MaxRange) LGF << " +P1";
               if(SVP.P2[i]==0) LGF << " 0P2"; else
               if(SVP.P2[i] < GDC.MinRange) LGF << " -P2"; else
               if(SVP.P2[i] > GDC.MaxRange) LGF << " +P2";
               LGF << endl;
            }
         }  // end if obvious outlier
      }  // end if data is not marked

      // look for gaps in data => new Segments
      if(SVP.Flag[i] == SVPass::OK) {                    // data is good
         if(ilast == -1)                     // the first good point
            Sit->nbeg = ilast = i;

         // is there a gap?
         if(GDC.DT*(i-ilast) > GDC.MaxGap) { // create a new Segment
            Segment Snew;
            Snew.nbeg = i;
            Snew.nend = Sit->nend;
            Sit->nend = ilast;
            Snew.npts = 0;
            SegList.push_back(Snew);
            // re-set current Segment to Snew == last one in list
            Sit = SegList.end();
            Sit--;

            if(GDC.Debug>6) LGF << "Gap: new Segment at # " << i << endl;
         }  // end create a new Segment

         // count only
         Sit->npts++;
         ilast = i;
      }  // end if good data

   }  // end loop over points in the pass

   if(ilast == -1) ilast=Sit->nbeg;
   Sit->nend = ilast;

   if(GDC.Debug>4) {
      if(GDC.Debug>5) LGF << "Initial list of Segments (" << SegList.size() << ")\n";
      list<Segment>::iterator it;
      for(it=SegList.begin(); it != SegList.end(); it++)
         LGF << "INIList " << GDCUnique << " " << SVP.SV << " #"
            << setw(2) << i+1 << ": "
            << setw(3) << it->nbeg << "-" << setw(4) << it->nend << endl;
   }

   CorrectSegments(SVP,GDC);

   if(GDC.Debug > 4) DumpSegments("BEF",GDCUnique,GDC,SVP);

   if(SegList.size() <= 0) {
      if(GDC.Debug>4) LGF << "Abort GDC: Not enough good data in Preprocess.\n";
      return PrematureEnd;
   }

   return ReturnOK;

}
catch(gpstk::Exception& e) {
   LGF << "Preprocess threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end Preprocess(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// After calling this routine the arrays are L1 = L1 phase (cycles)
//                                           L2 = GFP (m) - bias2
//                                           P1 = WLB (cycles) - bias1
//                                           P2 = -GFR (m)
//                                  where bias1 = initial wlb, each Segment
//                                    and bias2 = initial gfp, each Segment
int LinearCombinations(SVPass& SVP, GDCConfig& GDC)
{
try {
   if(GDC.Debug > 5) LGF << "LC............................................."
      << "...................................\n";

   int i,j,k,ilast,nb,ne;
   double wlr,wlp,wlbias,gfr,gfp;
   DayTime ttag;

   // consider each Segment in turn
   list<Segment>::iterator it;
   for(j=0,it=SegList.begin(); it!=SegList.end(); it++) {
      if(j==0) nb=it->nbeg;
      ne = it->nend;
      it->npts = 0;                   // npts will now be determined
      ttag = SVP.BegTime;
      ttag += it->nbeg * GDC.DT;

      // loop over the points in Segment k
      ilast = -1;       // index of the most recent good point
      for(i=it->nbeg; i<=it->nend; i++) {

         if(SVP.Flag[i] >= SVPass::OK) {                       // data is good
            wlr = wl1r*SVP.P1[i] + wl2r*SVP.P2[i];    // 'NL' range (meters)
            wlp = wl1p*SVP.L1[i] + wl2p*SVP.L2[i];    // Wide lane phase (meters)
            gfr =      SVP.P1[i] -      SVP.P2[i];    // Geo-free range (meters)
            gfp = gf1p*SVP.L1[i] + gf2p*SVP.L2[i];    // Geo-free phase (meters)
            wlbias = (wlp-wlr)/wlwl;                  // Wide lane bias (cycles)

            // define biases for this Segment
            if(ilast == -1) {
               it->bias1 = long(wlbias+(wlbias>0?0.5:-0.5));  // WL bias (NWL)
               it->bias2 = gfp;                               // GFP bias
            }
            // debias (GFR has bias 0)
            wlbias -= it->bias1;
            gfp    -= it->bias2;

            SVP.L2[i] = gfp;                       // Geo-free phase (meters)
            SVP.P1[i] = wlbias;                    // Wide lane bias (cycles)
            SVP.P2[i] = -gfr;                      // -Geo-free range (meters)
            ilast = i;
            it->npts++;

         }  // end if data is good

         ttag += GDC.DT;

      }  // end loop over points in Segment k

   }  // end loop over Segments

   if(GDC.Debug > 4) DumpSegments("LCD",GDCUnique,GDC,SVP);

   return ReturnOK;

}
catch(gpstk::Exception& e) {
   LGF << "LinearCombinations threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end LinearCombinations(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// Look for slips and outliers in the widelane bias.
// Divide into more than one Segment when a discontinuity is detected.
// After calling this routine the arrays are L1 = L1 phase (cycles)
//                                           L2 = GFP (m) - bias2
//                                           P1 = WLB (cy) - bias1
//                                           P2 = -GFR (m)
//                                  where bias1 = initial wlb, each Segment
//                                    and bias2 = initial gfp, each Segment
// (DivideSegments will change the biases on the new Segments)
int WLSlipDetect(SVPass& SVP, GDCConfig& GDC)
{
try {
   if(GDC.Debug > 5) LGF << "WD............................................"
      << "....................................\n";

   SVP.A1 = new double[SVP.Length];
   if(!SVP.A1) { OutOfMemory e("A1"); GPSTK_THROW(e); }
   SVP.A2 = new double[SVP.Length];
   if(!SVP.A2) { OutOfMemory e("A2"); GPSTK_THROW(e); }

   int iret;
   iret = WLSlipDetectLarge(SVP,GDC);
   if(iret) goto done;

   iret = WLSlipDetectSmall(SVP,GDC);
   if(iret) goto done;

done:
   if(SVP.A1) delete[] SVP.A1;
   if(SVP.A2) delete[] SVP.A2;
   SVP.A1 = SVP.A2 = NULL;

   return iret;
}
catch(gpstk::Exception& e) {
   LGF << "WLSlipDetect threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end WLSlipDetect(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// Look for LARGE slips and outliers in the widelane bias.
// Divide into more than one Segment when a discontinuity is detected.
int WLSlipDetectLarge(SVPass& SVP, GDCConfig& GDC)
{
try {
   bool Prnt=(GDC.Debug > 6);
   int i,j,k,nb,ne,iret;
   list<Segment>::iterator it;

   // Compute WL statistics with sliding window
   for(i=1,it=SegList.begin(); it!=SegList.end(); i++,it++) {
      nb = it->nbeg;
      ne = it->nend;
      iret = WLStatSweep(SVP, GDC, nb, ne, 0, i);
      if(iret) return iret;
   }

   int ilast,itest,inext;
   DayTime ttag;

   // interpret the results
   for(k=0,it=SegList.begin(); it!=SegList.end(); k++,it++) {
      nb = it->nbeg;
      ne = it->nend;

      ttag = SVP.BegTime;
      ttag += double(nb)*GDC.DT;
      ilast = nb;

      // search auxiliary arrays for slips
      for(itest=nb; itest<=ne; itest++) {

         if(Prnt && SVP.Flag[itest] != SVPass::BAD)
            LGF << "WLLD " << GDCUnique << " " << SVP.SV << " " << k+1
               << ttag.printf(" %13.6Q ") << setw(2) << SVP.Flag[itest]
               << " " << fixed << setw(13) << setprecision(3) << SVP.L1[itest]
               << " " << fixed << setw(13) << setprecision(3) << SVP.L2[itest]
               << " " << fixed << setw(13) << setprecision(3) << SVP.P1[itest]
               << " " << fixed << setw(13) << setprecision(3) << SVP.P2[itest]
               << " " << fixed << setw(13) << setprecision(3) << SVP.A1[itest]
               << " " << fixed << setw(13) << setprecision(3) << SVP.A2[itest]
               << " " << itest;
            // NB no endl here
         
         // test good data only
         if(SVP.Flag[itest] >= SVPass::OK) {
            // find the next good point
            inext = itest+1;
            while(inext<=ne && SVP.Flag[inext]<SVPass::OK) inext++;

            // is there potentially a slip here?
            if(SVP.A1[itest] > 10.0) {                 // test > limit+10
               if(Prnt) LGF << " D:T>L";
               if(SVP.A2[itest] >= -1.0) {             // A2 ok or -1
                  if(SVP.A2[inext] >= -1) {               // 1,3,4 - slip at itest
                     SVP.Flag[itest] |= SVPass::SLIPWL;
                     if(Prnt) LGF << " clSLIP";
                  }
                  else {                                  // 5, next == -3,-2
                     if(SVP.A1[inext] > 0.0) {             //    next T>L
                        SVP.Flag[itest] = SVPass::SETBAD;
                        if(Prnt) LGF << " 5SETBAD, next T>L";
                     }
                     else {                                //    next not T>L
                        SVP.Flag[itest] |= SVPass::SLIPWL;
                        if(Prnt) LGF << " 5SLIP SETBAD" << inext << "-";
                        i = inext;
                        while(i<=ne && (SVP.A2[i] < -1 || SVP.Flag[i] < SVPass::OK))
                        {
                           if(SVP.A1[i] > 0.0) break;
                           if(SVP.A2[i] < -1) SVP.Flag[i]=SVPass::SETBAD;
                           i++;
                        }
                        inext = i;
                        if(Prnt) LGF << i-1;
                     }
                  }
               }                                                  // end A2 ok or -1
               else if(SVP.A2[itest] < -1) {                      // 2 set bad
                  if(Prnt) LGF << " 2SETBAD";
                  SVP.Flag[itest] = SVPass::SETBAD;
                  i = inext;
                  while(i<=ne && (SVP.A2[i]<-1 || SVP.Flag[i]<SVPass::OK)) {
                     if(SVP.Flag[i] >= SVPass::OK)
                        SVP.Flag[i] = SVPass::SETBAD;
                     i++;
                  }
                  if(Prnt) LGF << itest << "-" << i-1;
                  if(SVP.A1[i] <= 0.0) {
                     if(Prnt) LGF << " SLIP" << i;
                     SVP.Flag[i] |= SVPass::SLIPWL;
                  }
                  inext = i;
               }                                              // end 2 set bad
               else {
                  if(Prnt) LGF << " OOP 2";                   // should never happen
               }
            }
               // --------------------------------------------------------------
            else if(SVP.A2[itest] == -2) {                     // -2
               if(inext > ne) {                                   // -2 w/ no next
                  if(Prnt) LGF << " D:-2end";
               }
               else if((SVP.A2[inext]==-1 || SVP.A2[inext]==-3)) {// -2 => -3,-1
                  if(Prnt) LGF << " D:-2=>-1-3";
                  i = itest;
                  while(i<=ne && (SVP.A2[i]<=-2 || SVP.Flag[i]<SVPass::OK)) {
                     if(SVP.Flag[i] >= SVPass::OK) SVP.Flag[i] = SVPass::SETBAD;
                     i++;
                  }
                  if(i!=itest && Prnt) LGF << " SETBAD" << itest << "-" << i-1;
                  inext = i;

                  if(SVP.A2[i] == -1) {
                     SVP.Flag[i] |= SVPass::SLIPWL;
                     if(Prnt) LGF << " SLIP " << i;
                  }
                  else if(SVP.A1[i] > 0.0) {
                     if(Prnt) LGF << " T>L";
                  }
                  else {
                     if(Prnt) LGF << " OOP 3(no -1)";
                  }
               }
            }
            else {
               //if(Prnt) LGF << " ok";
            }

            if(Prnt) LGF << " (" << inext << ")\n";

         }  // end if good data

         else {                                    // skip bad data
            if(Prnt && SVP.Flag[itest] != SVPass::BAD) LGF << " BAD\n";
         }

         ilast = itest;
         ttag += GDC.DT;
      }  // end loop over auxiliary arrays

   }  // end loop over Segments

      // create new Segments and compute WLStat
   DivideWLSegments(SVP,GDC);
   CorrectSegments(SVP,GDC);

   if(GDC.Debug > 4) DumpSegments("WLL",GDCUnique,GDC,SVP);

   if(SegList.size() <= 0) {
      if(GDC.Debug>4) LGF << "Abort GDC: No good data in WLSlipDetectLarge.\n";
      return PrematureEnd;
   }

   return ReturnOK;
}
catch(gpstk::Exception& e) {
   LGF << "WLSlipDetectLarge threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end WLSlipDetectLarge(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// Look for SMALL slips and outliers in the widelane bias.
// Divide into more than one Segment when a discontinuity is detected.
int WLSlipDetectSmall(SVPass& SVP, GDCConfig& GDC)
{
try {
   bool Prnt=(GDC.Debug>6),slip;
   int i,j,nb,ne,ib,ie,imax,imin;
   int width0=int(GDC.MinSeg/GDC.DT);
   double Amin,Amax,flag;
   DayTime ttag;
   list<Segment>::iterator it;

      // Small slips
      // for each Segment, compute WL statistics with sliding window
   // Compute WL statistics with sliding window
   for(j=1,it=SegList.begin(); it!=SegList.end(); j++,it++) {
      nb = it->nbeg;
      ne = it->nend;
      i = WLStatSweep(SVP, GDC, nb, ne, GDC.WLSSWindowWidth*width0, j);
      if(i) return i;
   }

      // look for slips using A1=|dAve| and A2=Wt'd variance
   flag = GDC.WLSSDetectMin-1.0;
   for(j=1,it=SegList.begin(); it!=SegList.end(); j++,it++) {
      nb = it->nbeg;
      ne = it->nend;
         // just in case
      while(nb < ne && SVP.Flag[nb] < SVPass::OK) nb++;
      while(ne > nb && SVP.Flag[ne] < SVPass::OK) ne--;

         // loop through first, flagging data that fails the tests
         // and replacing A1 with A1-A2
      ttag = SVP.BegTime;
      ttag += double(nb)*GDC.DT;
      for(i=nb; i<=ne; i++) {
         if(SVP.Flag[i] >= SVPass::OK) {
            if(SVP.A1[i] <= GDC.WLSSTestMin
                  || SVP.A1[i]-SVP.A2[i] <= GDC.WLSSDetectMin)
               SVP.A1[i] = SVP.A2[i] = flag;
            else SVP.A1[i] -= SVP.A2[i];

               // Debug plot - see the test
            if(Prnt) {
               LGF << "WLST " << GDCUnique << " " << SVP.SV << " " << j
                  << ttag.printf(" %13.6Q ") << setw(2) << SVP.Flag[i]
                  << " " << fixed << setw(13) << setprecision(3) << SVP.L1[i]
                  << " " << fixed << setw(13) << setprecision(3) << SVP.L2[i]
                  << " " << fixed << setw(13) << setprecision(3) << SVP.P1[i]
                  << " " << fixed << setw(13) << setprecision(3) << SVP.P2[i];
               if(SVP.A1 && SVP.A2)
                  LGF << " " << fixed << setw(13) << setprecision(3) << SVP.A1[i]
                        << " " << setw(9) << setprecision(4) << SVP.A2[i];
               LGF << " " << i << endl;
            }
         }
         ttag += GDC.DT;
      }  // end first loop over points within this Segment

         // search the non-flagged data for max in A1 and min in A2
         // first find continuous pieces of non-flagged data
      ib = nb;
      do {
         while(ib<=ne && (SVP.Flag[ib]<SVPass::OK || SVP.A1[ib]==flag)) ib++;
         i = ie = ib;
         while(i<=ne && (SVP.Flag[i]<SVPass::OK || SVP.A1[i]!=flag)) {
            if(SVP.Flag[i] < SVPass::OK) SVP.A1[i]=flag; // mark missing data
            ie = i++;
         }
         if(ib < ie) {     // find min in A2 and max in A1
            Amin = SVP.A2[imin=ib];
            Amax = SVP.A1[imax=ib];
            for(i=ib; i<=ie; i++) {
               if(SVP.A1[i] != flag && SVP.A1[i] > Amax) {
                  imax = i;
                  Amax = SVP.A1[i];
               }
               if(SVP.A1[i] != flag && SVP.A2[i] < Amin) {
                  imin = i;
                  Amin = SVP.A2[i];
               }
            }

               // (complete after slip test)
            if(Prnt) LGF << "WLSTest " << GDCUnique << " " << SVP.SV << " " << j
               << ttag.printf(" %13.6Q Seg:") << nb << "-" << ne
               << ", Piece:" << ib << "-" << ie << " Max:" << imax << " Min:" << imin;
               
               // test for slip
            slip = true;
               // min and max don't coincide
            if(abs(imax-imin) >= 2) {
               slip=false;
               if(Prnt) LGF << " (" << imax << "!=" << imin << ")";
            }
               // too close to edge of segment
            if(abs(imax-nb) < 5 || abs(imax-ne) < 5) {
               slip=false;
               if(Prnt) LGF << " (edges " << imax-nb << " " << ne-imax << ")";
            }
               // too narrow
            if(ie-ib+1 < width0) {
               slip=false;
               if(Prnt) LGF << " (narrow " << ie-ib+1 << " < " << width0 << ")";
            }

            if(Prnt) LGF << (slip?" SLIP\n":" FAIL\n");

            if(slip) {
               SVP.Flag[imax] |= SVPass::SLIPWL;
               if(Prnt) {
                  ttag = SVP.BegTime;
                  ttag += double(imax)*GDC.DT;
                  LGF << "WL Slip " << GDCUnique << " " << SVP.SV << " " << j
                     << ttag.printf(" %13.6Q")
                     << " " << fixed << setw(7) << setprecision(3) << Amin
                     << " " << fixed << setw(7) << setprecision(3) << Amax
                     << " " << imax << " (small)\n";
               }
            }
         }
         ib = ie+1;
      } while(ib <= ne);

   }  // end loop over Segments

      // create new Segments and compute WLStat
   DivideWLSegments(SVP,GDC);
   CorrectSegments(SVP,GDC);

   if(GDC.Debug > 4) DumpSegments("WLS",GDCUnique,GDC,SVP);

   if(SegList.size() <= 0) {
      if(GDC.Debug>4) LGF << "Abort GDC: No good data in WLSlipDetectSmall.\n";
      return PrematureEnd;
   }

   return ReturnOK;
}
catch(gpstk::Exception& e) {
   LGF << "WLSlipDetectSmall threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end WLSlipDetectSmall(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// WLStatSweep() : Wide lane statistics sweep.
// Average WL bias over 'two-pane moving window' of width iwidth points,
// over the data from index nb to ne. Compute 'step' = |change in averages|, and
// 'limit' = rss stddev's, (past and future)
// The minimum iwidth is == (min segment timespan / DT), and
// the maximum iwidth is == (ne-nb+1); iwidth is changed if necessary.
// Ignore bad data; this requires that indexes be allowed to vary within windows.
// DO NOT change Flag[] within this routine; this could lead to disaster.
// 
// Auxiliary arrays are filled, depending on iwidth:
// If iwidth==0 on input,
//    iwidth=min, test=|dAve|^2, limit=(WLNSigma*RSSsig)^2,
//    A1=(test-limit);
//    A2=flag: -1 if past StdDev large, -2 if future StdDev large, -3 both
//       otherwise A2 = limit
// Otherwise, A1 = 'step' and A2 = 'limit'.
//
// Return FatalProblem if input doesn't make sense
//
//------------------------------------------------------------------------------------
// 7 cases: time increase downward. check A1 and A2 as defined by WLSweep with
//          minimum window width.
//------------------------------------------------------------------------------------
// A: test is > limit
//       A1 (t-l)      A2 (-1: past sigma large, -2: future, -3: both, >=0: ok)
//     -------------   --
// 1.                  -2
//     test > limit    ok   Normal slip
//                     -1
//------------------------------------------------------------------------------------
// 2.                  -2 or >0
//     test > limit    -2/-3                 <---- this pt bad
//                     -2/-3 (any number)    <---- these bad
//      maybe t>l OR   -1                    <---- slip here, if t>l do nothing
//------------------------------------------------------------------------------------
// 3.                  -1                    <---- there must be gap after this
//     test > limit    ok                    <---- slip here
//                     ok
//------------------------------------------------------------------------------------
// 4.                  ok
//     test > limit    ok                    <---- slip here; gaps both sides
//                     ok
//------------------------------------------------------------------------------------
// 5.                  -2
//     test > limit    ok                    <---- slip (or bad)
//                     -2/-3                 <---- 0 or more; if 0 t>l pt is bad
//             t>l     -2
//------------------------------------------------------------------------------------
// B: -2 followed by -3 or -1, no t>l
// 6.                  -2                    <---- any number of these: ok
//                     -2                    <---- this one bad
//                     -3                    <---- any number of these: bad
//                     -1
//------------------------------------------------------------------------------------
// 7.                  -2                    <---- any number of these: ok
//                     -2                    <---- this one bad
//                     -1                    <---- slip here
//------------------------------------------------------------------------------------
//Notes on interpreting output of Sweep:
//if (test > limit) {
// if(A2>0 and A2 previous == -2 and A2 next == -1)
//    this is a normal slip
// if(A2==-3 or -2 AND A2prev == -2 or >0)
//    search downward until you find A2==-1 OR test>limit again with A2>0
//       this is new slip
// if(A2>0 && A2pre==-1 and A2next>0)
//    slip here (there must be gap btwn here and prev)
// if(A2, A2prev, A2next all > 0)
//    this is a slip with gaps on both sides
// if(A2>0, A2prev==-2, A2next may == -3 or (test>limit w/ A2==-2) )
//    this is a slip, unless A2next==-3, then its bad
//}
//if (A2==-2 and A2next==-3 or -1) {
// if(A2next == -3)
//    this and all down to the next A2==-1 are bad, the -1 is a slip
// if(A2next == -1)
//    this is bad and the next is a slip
// [check npts in the future stats - you may have too few for a slip - mark bad]
//}
//
int WLStatSweep(SVPass& SVP, GDCConfig& GDC, int nb, int ne, int iwidth, int iseg)
{
try {
   bool MinRun=false;
   int width0=int(GDC.MinSeg/GDC.DT);
   int i,iplus,iminus,ilast;
   double wlbias,bias,nsig2,test,limit;
   DayTime ttag;
   Stats<double> SWLf,SWLp;              // stats in future and past windows
   list<Segment>::iterator it,itplus,itminus;

   if(iwidth==0) MinRun=true;

   if(MinRun) {
      // if large slips, width = width0, nsig2 = configuration
      iwidth = width0;
      nsig2 = GDC.WLNSigma*GDC.WLNSigma;
   }
   else {
      // if checking for small slips, width = 10*("), nsig2 = 1
      nsig2 = 1.0;
      //iwidth = 15*width0;//TEMP      // don't go to max = ne-nb+1;
   }

      //
      // Cartoon of the 'two-pane moving window'
      // windows:  'past window'      'future window'
      // stats  :    SWLp(N,A,S)        SWLp(N,A,S)
      // data   : (x x x x x x x x x)(x x x x x x x x x) x ...
      //           |               |  |                  |
      // indexes: iminus          i-1 i                 iplus
      // Seg ind: kminus              k                 kplus
      //

      // determine window width (number of points)
   if(iwidth == 0) iwidth=width0;
   if(iwidth > ne-nb+1) iwidth=ne-nb+1;   // max possible width

      // find Segment of first point
   for(it=SegList.begin(); it!=SegList.end(); it++) {
      if(nb <= it->nend) break;
   }
   if(it == SegList.end()) return FatalProblem;
   itplus = itminus = it;

      // initialize
   bias = it->bias1;               // overall bias == first bias1
   iplus = iminus = ilast = nb;           // ilast is most recent good point
   ttag = SVP.BegTime;                    // timetag of i
   ttag += double(nb) * GDC.DT;
   SWLf.Reset();                          // future stats
   SWLp.Reset();                          // past stats

      // 'prime the pump' by setting up future stats
      // add iplus to future stats, and then move iplus up
   while(SWLf.N() < unsigned(iwidth) && iplus <= ne) {
      wlbias = SVP.P1[iplus] - itplus->bias1 + bias;
      SWLf.Add(wlbias);
         // now update iplus by finding the next good point
      do { iplus++; } while(iplus <= ne && SVP.Flag[iplus] < SVPass::OK);
         // update kplus
      if(iplus <= ne && iplus > itplus->nend) {
         for(; itplus!=SegList.end(); itplus++) {
            if(iplus <= itplus->nend) break;
         }
         if(itplus == SegList.end()) return FatalProblem;   // should never happen
      }
   }

      // loop over points in the Segment
   for(i=nb; i<=ne; i++) {
         // update Segment pointers; k = Segment containing i
      if(i > it->nend) {
         for(; it != SegList.end(); it++) {
            if(i <= it->nend) break;
         }
         if(it == SegList.end()) return FatalProblem;
      }

      SVP.A1[i] = SVP.A2[i] = 0.0;

      if(SVP.Flag[i] >= SVPass::OK) {
            // current value
         wlbias = SVP.P1[i] - it->bias1 + bias;

            // test is (change in Ave's)^2 < N^2*(Sig(p)^2+Sig(f)^2)
         test = SWLf.Average() - SWLp.Average();
         test = test*test;

            // LARGE slip detection
            // -----------------------------------------------------
         if(MinRun) {                              // width==min:
               // weighted average, wt N()
            limit = SWLf.N()*SWLf.Variance() + SWLp.N()*SWLp.Variance();
            limit *= nsig2/(SWLf.N()+SWLp.N());
            if(limit < 1.0) limit=1.0;                   // minimum cycle^2

            //if(test > limit)
            if(test>limit) SVP.A1[i]=test-limit;
            if(SVP.A1[i] > 999999.) SVP.A1[i]=999999.000;// just for convenience

               // catch gaps near slips
            if(SVP.A1[i]>=0.0 && SWLp.N()==1 && fabs(wlbias-SWLp.Average()) > 10.0)
            {
               SVP.A1[i] = 999999.000;
               // LGF << "CATCH " << GDCUnique << " " << SVP.SV << " " iseg
               // << " " << i << endl;
            }
            //SVP.A1[i] += 0.1*SWLp.N()+0.01*SWLf.N();

               // set indicators of large sigma past and future
            if(SWLp.StdDev() > 10.0) SVP.A2[i] -= 1;       // 10 cycles: default
            if(SWLf.StdDev() > 10.0) SVP.A2[i] -= 2;       // 'small' slips
            if(SVP.A2[i] >= 0.0) SVP.A2[i] = limit;
         }
            // -----------------------------------------------------
            // SMALL slip detection
         else {                                    // width > min
            limit = nsig2*(SWLf.Variance()+SWLp.Variance());
            //if(limit < 0.7) limit=0.7;                    // minimum cyc^2
            SVP.A1[i] = sqrt(test);
            SVP.A2[i] = sqrt(limit);
         }
         
            // output stats now
            // u prn seg mjd (N A S)p (N A S)f A1 A2 test limit i
         if(GDC.Debug > 5) LGF << "WL" << (MinRun?"L":"S") << "S " << GDCUnique
            << " " << SVP.SV << " " << iseg << ttag.printf(" %13.6Q ")
            << " " << setw(3) << SWLp.N()
            << " " << fixed << setw(7) << setprecision(3) << SWLp.Average()
            << " " << fixed << setw(7) << setprecision(3) << SWLp.StdDev()
            << " " << setw(3) << SWLf.N()
            << " " << fixed << setw(7) << setprecision(3) << SWLf.Average()
            << " " << fixed << setw(7) << setprecision(3) << SWLf.StdDev()
            << " " << fixed << setw(9) << setprecision(3) << SVP.A1[i]
            << " " << fixed << setw(9) << setprecision(3) << SVP.A2[i]
            << " " << fixed << setw(7) << setprecision(3) << sqrt(test)
            << " " << fixed << setw(7) << setprecision(3) << sqrt(limit)
            << " " << fixed << setw(9) << setprecision(3) << wlbias
            << " " << i << endl;

            // update for next step
         SWLf.Subtract(wlbias);           // remove i from future statistics
         SWLp.Add(wlbias);                // add i to past statistics
         ilast = i;
      }
      ttag += GDC.DT;

         // now must update future and past statistics
         // add iplus to future stats, and then move iplus up
      while(SWLf.N() < unsigned(iwidth) && iplus <= ne) {
         wlbias = SVP.P1[iplus] - itplus->bias1 + bias;
         SWLf.Add(wlbias);
            // now update iplus by finding the next good point
         do { iplus++; } while(iplus <= ne && SVP.Flag[iplus] < SVPass::OK);
            // update kplus
         if(iplus <= ne && iplus > itplus->nend) {
            for(; itplus != SegList.end(); itplus++) {
               if(iplus <= itplus->nend) break;
            }
            if(itplus == SegList.end()) return FatalProblem;   // should never happen
         }
      }
         // remove iminus from past stats, then move iminus up
      while(SWLp.N() > unsigned(iwidth) && iminus >= nb) {
         wlbias = SVP.P1[iminus]-itminus->bias1+bias;
         SWLp.Subtract(wlbias);
            // now update iminus by finding the next good point
            // (references to <=ne are unnecessary)
         do { iminus++; } while(iminus <= ne && SVP.Flag[iminus] < SVPass::OK);
            // update kminus
         if(iminus <= ne && iminus > itminus->nend) {
            for(; itminus != SegList.end(); itminus++) {
               if(iminus <= itminus->nend) break;
            }
            if(itminus == SegList.end()) return FatalProblem;   // should never happen
         }
      }

   }  // end loop over all points in Segment

   return ReturnOK;
}
catch(gpstk::Exception& e) {
   LGF << "WLStatSweep threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end WLStatSweep(SVPass& SVP, GDCConfig& GDC, int nb, int ne, int iwid, int iseg)

//------------------------------------------------------------------------------------
// Create new Segments where SLIPs have been marked.
// Modify the bias, and compute WLStat's
void DivideWLSegments(SVPass& SVP, GDCConfig& GDC)
{
try {
   bool old;         // true if current segment was not created by this routine
   int i,j,ilast,nb,ne;
   double bias1,bias2,bias10,bias20;
   list<Segment>::iterator it;

      // loop over segments, changing start and stop, counting good points,
      // and making new segments where flagged
   j = 0;
   old = true;
   bias1 = bias2 = 0;
   for(it=SegList.begin(); it!=SegList.end(); it++) {
         // limits of Segment (good data)
      nb = it->nbeg;
      ne = it->nend;
         // change start point?
      while(nb <= ne && SVP.Flag[nb] < SVPass::OK) nb++;
      it->nbeg = nb;
         // biases
      if(old) {
         bias10 = it->bias1;
         bias20 = it->bias2;
      }

         // look for new Segments
      ilast = nb;
      it->npts = 0;            // count good points
      it->WLStat.Reset();      // compute WLStats along the way
      for(i=nb; i<=ne; i++) {
         if(SVP.Flag[i] >= SVPass::OK) {              // good point
               // slip is marked mid-segment
            if(i>nb && SVP.Flag[i]>SVPass::OK && (SVP.Flag[i]&SVPass::SLIPWL)){
                  // create new Segment
               Segment Snew;
               Snew.nbeg = i;
               Snew.nend = ne;
               it->nend = ilast;
                  // biases
               bias1 = int(SVP.P1[i]+(SVP.P1[i]<0?-0.5:0.5));
               bias2 = SVP.L2[i];
               Snew.bias1 = bias10 + bias1;
               Snew.bias2 = bias20 + bias2;
               old = false;
                  // add it after the current segment:
                  // insert(iter,s) puts s before iter, then returns iterator for s
               it++;
               it = SegList.insert(it,Snew);
               it--;             // (loop will increment it again)
               break;   // ..the for loop - goto next (new) segment
            }

               // good point - debias and count
            SVP.P1[i] -= bias1;
            SVP.L2[i] -= bias2;
            it->WLStat.Add(SVP.P1[i]);
            it->npts++;
            it->nend = ilast = i;       // this corrects nend if wrong

         }  // end if good point

         if(i==ne) {
            old = true;
            bias1 = bias2 = 0;
         }
      }  // end for loop over points in the Segment

   }  // end loop over Segments
}
catch(gpstk::Exception& e) {
   LGF << "DivideWLSegments threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
} // end DivideWLSegments(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// Estimate the WL slip, using statistics in each segment, and set NWL to the
// appropriate value. Where a WL slip can be fixed, combine the Segments in the
// sense that, in the Segment in the future of the slip, correct the WLbias (P1)
// and the GFP (L2) for the slip, redefine bias1, and let WLStat in both
// Segments apply to all the data in both.
//
// Initially we have, for example, the following, where each Segment has its own
// WLStats and biases.
//
//SVP.Flag:    0    S      S      S                S       S    S    S
//            ( 0 )( 1 )  (  2  )(      3        )(  4   )( 5 )( 6 )( 7 )
//NWL:         0    0      0      0                0       0    0    0
//
// Start with the largest Segment (3), and first move to the right (future),
// then move to the left (past).  Combine Segments when the WL slip can be
// fixed. 'Combined' means make equal WLStat and bias1, and correct the
// WLB and GFP data in all the Segments on the future side for NWL.
//
// So after this routine we may have, for example:
//
//    0    F      S      F                F       S    F    F
//   ( 0 )( 1 )  (  2  )(      3        )(  4   )( 5 )( 6 )( 7 )
//    0    N1     0      N3               N4      0    N6   N7
//    |______|    |____________________________|  |___________|
//      (0)                    (2)                    (5)
//
// where Stats, bias1, for Segments 2,3,4 apply to all data in (2), etc
// but the NWL's are left alone (this is so we can write out Edit Cmds later);
// leave everything else alone as well (nbeg, nend, npts). The GFP is left alone
// except for correction of the WL slip (bias2 -> bias2 + wl2*NWL).
//
// After this routine arrays are L1 = L1 phase (cycles)
//                               L2 = GFP (m) - (wl1-wl2)*N1 - bias2
//                               P1 = WLB (cy) - bias1
//                               P2 = -GFR (m)
//        where bias1 = initial wlb, each Super segment
//          and bias2 = initial gfp, each segment
//            and NWL = slip each Segment
//
int WLSlipFix(SVPass& SVP, GDCConfig& GDC)
{
try {
   if(GDC.Debug > 5) LGF << "LF..........................................."
      << ".....................................\n";

   if(SegList.size() <= 0) return PrematureEnd;
   if(SegList.size() == 1) return ReturnOK;

   bool fix,MoveForward;
   int i,j,k,nfe,jbig;
   unsigned int nbig;
   long nwl;
   double dwl,test,dbias1;
   DayTime ttag;
   list<Segment>::iterator it,kt,itpast,itfuture,itbig;

      // find the largest Segment
   for(j=1,jbig=0,nbig=0,it=SegList.begin(); it != SegList.end(); j++,it++) {
      if(it->WLStat.N() > nbig) {
         itbig = it;
         jbig = j;
         nbig = it->WLStat.N();
      }
   }

   if(GDC.Debug > 6) {
      LGF << "\nWLSF: SegList:\n";
      for(j=1,it=SegList.begin(); it != SegList.end(); j++,it++)
         LGF << "WLSF: " << j << " nb=" << it->nbeg
            << " bias1=" << fixed << setprecision(3) << it->bias1
            << " bias2=" << setprecision(3) << it->bias2 << endl;
   }

      // ------------------------------------------------------
      // loop starting at largest Segment
   MoveForward = true;                       // move to future first
   for(j=jbig,it=itbig; ; ) {
         // identify the two Segments of interest
      if(MoveForward) {          // moving toward future
         itfuture = itpast = it;
         itfuture++;
         if(itfuture == SegList.end()) { // no more, go back to itbig and move to past
            MoveForward = false;
            it = itbig;
            j = jbig;
            continue;
         }
      }
      else {                     // moving toward past
         itfuture = itpast = it;
         if(itpast == SegList.begin()) break;      // all done
         itpast--;
      }

      if(GDC.Debug > 6) {
         LGF << "\nWLSF: Attempt with Segs starting at " << itpast->nbeg
               << " & " << itfuture->nbeg << "\n";
         LGF << "WLSF:  Past  : bias1=" << fixed
             << setw(13) << setprecision(3) << itpast->bias1
             << " bias2=" << fixed << setw(13) << setprecision(3) << itpast->bias2
             << " N=" << itpast->WLStat.N() << " A=" << setprecision(3)
             << itpast->WLStat.Average()
             << " SD=" << setprecision(3) << itpast->WLStat.StdDev() << endl;
         LGF << "WLSF:  Future: bias1=" << fixed
             << setw(13) << setprecision(3) << itfuture->bias1
             << " bias2=" << fixed << setw(13) << setprecision(3) << itfuture->bias2
             << " N=" << itfuture->WLStat.N() << " A=" << setprecision(3)
             << itfuture->WLStat.Average()
             << " SD=" << setprecision(3) << itfuture->WLStat.StdDev() << endl;
      }

         // ------------------------------------------------------
         // first decide if a slip has been found here
         // perhaps we don't need this ...?
      if(SVP.Flag[itfuture->nbeg] & SVPass::SLIPWL) {

            // get the time tag
         ttag = SVP.BegTime;
         ttag += double(itfuture->nbeg)*GDC.DT;

            // ------------------------------------------------------
            // compute the slip
         dwl = itfuture->bias1 + itfuture->WLStat.Average()
               - (itpast->bias1 + itpast->WLStat.Average());

         if(GDC.Debug > 6) {
            LGF << "WLSF: dwl = f->bias1 + f->ave - (p->bias1 + p->ave)\n";
            LGF << "WLSF: " << fixed << setw(13) << setprecision(3) << dwl
               << " = " << setw(13) << setprecision(3) << itfuture->bias1
               << " + " << setw(13) << setprecision(3) << itfuture->WLStat.Average()
               << " - (" << setw(13) << setprecision(3) << itpast->bias1
               << " + " << setw(13) << setprecision(3) << itpast->WLStat.Average()
               << ")\n";
         }

            // and round it
         nwl = long(dwl + (dwl>0?0.5:-0.5));
            // dwl is now the fractional part
         dwl -= double(nwl);

            // ------------------------------------------------------
            // can it be fixed? Here are the tests
         if(GDC.Debug > 6) LGF << "WL Slip " << GDCUnique << " " << SVP.SV
            << " " << j << " at #" << setw(4) << itfuture->nbeg << ttag << " = "
            << nwl << " + " << setw(6) << setprecision(3) << dwl << endl;

         fix = true;
            // check the gap - if too large, must fail
            // ?? TD why? Gap should have no effect on WLB -- kill this
         test = (itfuture->nbeg - itpast->nend)*GDC.DT;
         if(test > GDC.WLFixMaxGap) {
            if(GDC.Debug>6) LGF << "WL Slip " << GDCUnique << " " << SVP.SV
               << " fix FAILS (Gap too large " << setw(5) << setprecision(3) << test
               << " > " << setw(5) << setprecision(3) << GDC.WLFixMaxGap << endl;
            fix = false;
         }
            // minimum # pts in both segments
         if(itpast->WLStat.N() <= GDC.WLFixNpts
               || itfuture->WLStat.N() <= GDC.WLFixNpts) {
            if(GDC.Debug>6) LGF << "WL Slip " << GDCUnique << " " << SVP.SV
               << " fix FAILS (Not enough points " << itpast->WLStat.N()
               << " " << itfuture->WLStat.N() << " <= " << GDC.WLFixNpts << ")\n";
            fix = false;
         }
            // total error (stddev/sqrt(n), RMS for 2 sides) is small
         test = sqrt(itpast->WLStat.Variance() + itfuture->WLStat.Variance()) / 
            double(itpast->WLStat.N() + itfuture->WLStat.N());
         if(test >= GDC.WLFixSigma) {
            if(GDC.Debug>6) LGF << "WL Slip " << GDCUnique << " " << SVP.SV
               << " fix FAILS (Sigma too large " << fixed
               << setw(7) << setprecision(3)
               << sqrt(itpast->WLStat.Variance() + itfuture->WLStat.Variance())
               << " >= " << setw(7) << setprecision(3) << GDC.WLFixSigma << ")\n";
            fix = false;
         }
            // fractional part of offset is small
            // ?? TD ? Effective?
         //if(fabs(dwl) > C.WLFixFrac) {
         // if(C.fplog) fprintf(C.fplog,"WL Slip %d %2.2d fix FAILS "
         //    "(Fraction large %6.4f > %6.4f)\n",GDCUnique,SVP.SV.Prn(),
         //    fabs(dwl),C.WLFixFrac);
         // fix = false;
         //}

            // ------------------------------------------------------
            // fix the slip
         if(fix) {
               // define the slip
            if(SVP.Flag[itfuture->nbeg] & SVPass::SLIPWL) // don't want to set it
                     SVP.Flag[itfuture->nbeg] ^= SVPass::SLIPWL;
            SVP.Flag[itfuture->nbeg] |= SVPass::FIXWL;
            itfuture->NWL = nwl;

            if(GDC.Debug > 6) {
               LGF << "WLSF: nwl at nbeg\n";
               LGF << "WLSF: " << nwl << " at " << itfuture->nbeg << endl;
            }

               // Join the segments
               // fix bias1, NWL and WLStats in future (super-)Segment
            nfe = itfuture->nend;          // index at end of future Segment(s)
            kt = it;
            if(MoveForward) kt++;          // it == past
            //else                         // it == future
            while(kt != SegList.end()) {
                  // always include the first segment, at least
               if(MoveForward || (SVP.Flag[kt->nbeg] & SVPass::FIXWL))
                  nfe = kt->nend;
                  // but quit when a slip failure encountered//v3.1,5/13/03
               if(!(SVP.Flag[kt->nbeg] & SVPass::FIXWL)) break;
               kt++;
            }
            dbias1 = itfuture->bias1 - itpast->bias1;    // ~ itfuture->NWL

            if(GDC.Debug > 6) {
               LGF << "WLSF: correct nb to nfe: dbias1 NWL\n";
               LGF << "WLSF: " << itfuture->nbeg << " to " << nfe << " " << fixed
                  << setw(13) << setprecision(3) << dbias1
                  << " " << itfuture->NWL << endl;
            }

               // -----------------------------------------------
               // loop over all data (all Segments) in future of slip
               // correct the WLB data for the WL slip
            for(i=itfuture->nbeg; i<=nfe; i++) {
               if(SVP.Flag[i] >= SVPass::OK) {
                  SVP.P1[i] += dbias1 - itfuture->NWL;    // ~ 1
                  itpast->WLStat.Add(SVP.P1[i]);       // they have same bias..
               }
            }

               // -----------------------------------------------
               // loop over all Segments in future
               // Redefine biases (correct GFP for NWL using bias2)
               // and set all Stats in future (super-)Segment to past stats
            kt = it;
            if(MoveForward) kt++;
            while(kt != SegList.end()) {
               if(MoveForward || (SVP.Flag[kt->nbeg] & SVPass::FIXWL)){
                  kt->bias1 -= dbias1;               // WLB
                  kt->bias2 -= wl2*itfuture->NWL;    // GFP
                  if(!MoveForward || kt==it)
                     kt->WLStat = itpast->WLStat;
               }
                  //v3.1,5/13/03
               else if(!(SVP.Flag[kt->nbeg] & SVPass::FIXWL)) break;
               kt++;
            }
            // (leave NWL, N1, nbeg, nend, npts alone!)

            if(GDC.Debug > 6) LGF << "WLSF:  Joined: bias1="
                  << fixed << setw(13) << setprecision(3) << itfuture->bias1
                  << " N=" << itfuture->WLStat.N()
                  << " A=" << setprecision(3) << itfuture->WLStat.Average()
                  << " SD=" << setprecision(3) << itfuture->WLStat.StdDev()
                  << endl << endl;

         }  // end if fix
         //else GDCNFail++;

      }  // end if WL slip exists here

         // move to next Segment
      if(MoveForward) { j++; it++; }
      else            { j--; it--; }

   }  // end loop over Segments

   if(GDC.Debug > 5) DumpSegments("WLF",GDCUnique,GDC,SVP);
   
   return ReturnOK;
}
catch(gpstk::Exception& e) {
   LGF << "WLSlipFix threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end WLSlipFix(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// Loop over Segments:
// Fit a polynomial to the GF range, and replace -GF Range (P2) with
//   residual GFPh+Fit(GFR) divided by wl21.
// Also scale the GFPh by 1/wl21, ie convert to cycles - only (for GFP)
// Either reject outliers, or further divide into Segments, when the change in
// the residual is large.
//
// After this routine arrays are L1 = L1 phase (cycles)
//                               L2 = GFP(cycles) - bias2/wl21
//                               P1 = WLB (cy) - bias1
//                               P2 = GFP-Fit(-GFR)(cycles)
//                         and bias2 is in units of cycles (of wl21)
//
int GFSlipDetect(SVPass& SVP, GDCConfig& GDC)
{
try {
   if(GDC.Debug > 5) LGF << "GD..............................................."
      << ".................................\n";

   if(SegList.size() <= 0) return PrematureEnd;

   bool first;
   int i,j,k,nb,ne,dn,ilast,nout,newbeg;
   double t,data,fit,bias;
   DayTime ttag;
   list<Segment>::iterator it;

      // ------------------------------------------------------------------
      // loop over the Segments
   for(j=0,it=SegList.begin(); it != SegList.end(); j++,it++) {
      nb = it->nbeg;              // beginning of super-Segment
      ne = it->nend;              // end of Segment
      dn = ne-nb;                 // last - first : used in polynomial fit

         // ---------------------------------------------------
         // Do the polynomial fit
      i = GFRangePolynomialFit(SVP,GDC,it,j);
      if(i == PrematureEnd) continue;           // segment was too small
      if(i < ReturnOK) return i;

         // ------------------------------------------------------------
         // Loop over the Segment:
         //    GF Phase (L2) = (f1^2/f2^2 - 1)I + WL2*NWL - (WL2-WL1)*N1
         // (but the NWL term has been removed at this point)
         // Replace the -GFR(P2) with residual GFP - Polynomial(-GFR),
         //    divided by WL2-WL1; also divide bias2 by wl21.
         // Look for gaps and jumps, if necessary creating a new segment

      ilast = -1;          // index of last good point
      nout = 0;            // number of outliers
      first = true;        // first good point in the super-Segment
      bias = 0.0;          // net bias change
      ttag = SVP.BegTime;  // time tag of i
      ttag += double(nb) * GDC.DT;
      it->npts = 0;                           // count points in Segment
      it->bias2 /= wl21;                      // data will be rescaled below
      if(j>0 && ((SVP.Flag[nb]&SVPass::SLIPWL) || (SVP.Flag[nb]&SVPass::FIXWL)))
         SVP.Flag[nb] |= SVPass::SLIPGF;  // mark all WL slips GF slips

         // loop over points
      for(i=nb; i<=ne; i++) {
            // only consider good data
         if(SVP.Flag[i] >= SVPass::OK) {
               // evaluate the polynomial and compute GF Ph+Fit(Range)
            t = 2*double(i-nb)/dn - 1.0;
            data = (SVP.L2[i] - it->PF.Evaluate(t))/wl21; // (temp use data=P2)

               // change units of GFPhase (WL corrected) to cycles of wl21
            fit = SVP.L2[i] / wl21;                       // (temp use of fit=L2)
               
               // --------------------------------------------------
               // look for anomalous point in the GFP+Fit(GFR)
               // (no need for bias2 here: within one segment, bias is irrelevant)
            if(ilast > -1 && fabs(data-SVP.P2[ilast]) > GDC.GFOnlySlipTol) {

                  // if nout >= GDC.GFNOutliers, check that sigma of these
                  // nout 'outliers' is small -- ie they agree; if so, slip;
                  // else mark all outliers (ie set nout=0)
               if(nout >= GDC.GFNOutliers) {
                     // Compute stats(1st-all others) and stats(2nd-(3,4,...))
                     // 1st is earliest: is 1st an outlier? or slip?
                     // Ought to also compare with ilast -  is slip big?
                     //  esp. in high ionosphere, where false slips are prob.
                  double d1,d2;
                  Stats<double> St1,St2;
                  ibad[nout] = i;
                  dbad[nout] = data;
                  for(k=1; k<=nout; k++) {
                     d1 = dbad[k]-dbad[0];
                     St1.Add(d1);
                     d2 = dbad[k]-dbad[1];
                     if(k>1) St2.Add(d2);
                     //if(GDC.Debug>1)
                     //fprintf(C.fplog,"GF Out/GF Slip %d %2.2d %d test: %4d "
                     //"%7.3f %7.3f\n",GDCUnique,SVP.SV.Prn(),j+1,ibad[k],
                     //d1,d2);
                  }
                  if(GDC.Debug>6) LGF << "GF Out/GF Slip " << GDCUnique
                     << " " << SVP.SV
                     << " " << j+1 << " test:"
                     << " Stat1(" << St1.N() << ")="
                     << fixed << setw(7) << setprecision(3)
                     << St1.Average() << "+-" << setprecision(3)
                     << St1.StdDev()
                     << " Stat2(" << St2.N() << ")="
                     << fixed << setw(7) << setprecision(3)
                     << St2.Average() << "+-" << setprecision(3)
                     << St2.StdDev();

                     // now the test: if sigmas big OR averages differ, reject
                  if(St1.StdDev() > GDC.GFDetectMaxSigma ||
                     St2.StdDev() > GDC.GFDetectMaxSigma ||
                     fabs(St1.Average()-St2.Average()) > GDC.GFDetectMaxSigma) {
                     for(k=1; k<nout; k++) {
                        ibad[k-1]=ibad[k];
                        dbad[k-1]=dbad[k];
                     }
                     nout--;
                     if(GDC.Debug>6) LGF << " :O\n";
                  }
                  else if(GDC.Debug>6) LGF << " :S\n";

               } // end if more than min anomalous points

               // --------------------------------------------------
               if(nout < GDC.GFNOutliers) {                         // outlier
                     // 0,1,2,4,8 -> -16,-15,-14,-12,-8
                  SVP.Flag[i] += SVPass::GFBAD;
                  ibad[nout] = i;
                  dbad[nout] = data;
                  nout++;
                  if(GDC.Debug>6) LGF << "GF Out " << GDCUnique << " " << SVP.SV
                     << " at # " << i << " " << ttag << " ("
                     << fixed << setw(7) << setprecision(4) << fabs(data-SVP.P2[ilast])
                     << " > " << setw(7) << setprecision(4) << GDC.GFOnlySlipTol
                     << ")\n";
               }
               // --------------------------------------------------
               else {                                             // slip
                     // undo 'outlier' status of prev nout points
                     // find first good point of segment and mark
                  newbeg = -1;
                  for(k=0; k<nout; k++) {
                     if(SVP.Flag[ibad[k]] < SVPass::SETBAD) {
                           // -16,-15,-14,-12,-8 -> 0,1,2,4,8
                        SVP.Flag[ibad[k]] -= SVPass::GFBAD;
                        if(newbeg == -1) {
                           newbeg = ibad[k];
                           SVP.Flag[ibad[k]] |= SVPass::SLIPGF;
                           ttag = SVP.BegTime;
                           ttag += double(ibad[k]) * GDC.DT;
                        }
                     }
                  }

                     // create a new Segment
                  Segment Snew(*it);
                  Snew.nbeg = newbeg;
                  Snew.nend = it->nend;
                  it->nend = ilast;
                  Snew.npts = 0;
                  Snew.bias2 *= wl21;        // undone later
                  Snew.NWL = 0;              // this is GFonly slip

                     // if Stats WLStat needed in both segments,
                     // have to recompute here!

                     // add new Segment after it and redefine it (j is unchanged)
                  it++;
                  it = SegList.insert(it,Snew);

                  if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique
                     << " " << SVP.SV << " " << j+1
                     << " (w/o WL): new Segment at # " << Snew.nbeg << " " << ttag
                     << " " << fixed << setw(13) << setprecision(3)
                     << data-SVP.P2[ilast] << endl;

                  break;      // ...the for loop - go to next (new) Segment

               } // end if slip

            }  // end if anomalous point

            // --------------------------------------------------
            else {                           // it is not anomalous point
               ilast = i;
               nout = 0;
               it->npts++;                   // count good points
               SVP.P2[i] = data;             // = (L2-fit)/wl21
               SVP.L2[i] = fit;              // = L2/wl21

            } // end if not anomalous point

         }  // end if good point

         ttag += GDC.DT;

      }  // end for loop over points within the segment

   }  // end loop over segments

   CorrectSegments(SVP,GDC);

   if(GDC.Debug > 4) DumpSegments("GFD",GDCUnique,GDC,SVP);
   
   if(SegList.size() <= 0) {
      if(GDC.Debug>4) LGF << "Abort GDC: No good data in GFSlipDetect.\n";
      return PrematureEnd;
   }

   return ReturnOK;
}
catch(gpstk::Exception& e) {
   LGF << "GFSlipDetect threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end GFSlipDetect(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// fit a polynomial to the geometry-free range in the super-Segment
int GFRangePolynomialFit(SVPass& SVP,GDCConfig& GDC,list<Segment>::iterator& it,int j)
{
try {
   int i,nb,ne,dn,npts,ndeg;
   double t,data,fit;
   DayTime ttag;

   nb = it->nbeg;              // beginning of super-Segment
   ne = it->nend;              // end of Segment
   dn = ne-nb;
      // count the number of good points in super-Segment
   npts = 0;
   for(i=nb; i<=ne; i++) {
      if(SVP.Flag[i] >= SVPass::OK) npts++;
   }

      // determine the degree of the polynomial fit to GFRange
      // ndeg = MIN( 10, MIN( (2+2*minutes_in_seg/100), (npts-1) )
   ndeg = 2+long((dn+1)*GDC.DT/3000.0 + 0.5);
   if(npts-1 < ndeg) ndeg=npts-1;
   if(ndeg > GDC.GFPolyMaxDegree) ndeg=GDC.GFPolyMaxDegree;
   if(ndeg < 2) {
      //if(GDC.Debug)
         //LGF << "GF Slip Detect polynomial degree is too small: " << ndeg << endl;
      return PrematureEnd;
   }

      // do the polynomial fit
   it->PF.Reset(unsigned(ndeg));
   for(i=nb; i<=ne; i++) {
      if(SVP.Flag[i] >= SVPass::OK) {
            // time is rescaled to [-1 to +1]
         t = 2*double(i-nb)/dn - 1.0;
         it->PF.Add(SVP.P2[i],t);
      }
   }

      // evaluate the RMS residual of fit - do we need to change ndeg?
   if(it->PF.isSingular()) {
      LGF << "GDC: GF Slip Detect Segment " << j+1
         << ": Polynomial fit is singular!\n";
      return FatalProblem;
   }

   it->RMSROF = 0.0;
   ttag = SVP.BegTime;
   ttag += double(nb) * GDC.DT;
   for(i=nb; i<=ne; i++) {
      if(SVP.Flag[i] >= SVPass::OK) {
         t = 2*double(i-nb)/dn - 1.0;
         data = SVP.P2[i];
         fit = it->PF.Evaluate(t);
         it->RMSROF = RSS(it->RMSROF,data-fit);
            // print ttag,GFP(m),t,GFR(m),fit(m),res(m)
         if(GDC.Debug>5) LGF << "GFRF " << GDCUnique << " " << SVP.SV << " " << j+1
            << ttag.printf(" %13.6Q ") << fixed
            << " " << setw(7) << setprecision(4) << SVP.L2[i]
            << " " << setw(8) << setprecision(3) << t
            << " " << setw(8) << setprecision(3) << data
            << " " << setw(8) << setprecision(3) << fit
            << " " << setw(8) << setprecision(3) << data-fit
            << " " << i << endl;
      }
      ttag += GDC.DT;
   }

      // RMS residual of fit
   it->RMSROF /= sqrt(double(npts));
   if(GDC.Debug>6) LGF << "GFProc Segment " << j+1 << " RMSROF ("
      << npts << " pts, " << ndeg << " deg) "
      << fixed << setw(8) << setprecision(3) << it->RMSROF << endl;

      // test RMS residual of fit (ROF) - nothing is done with this
   //if(it->RMSROF > GDC.GFRMaxRMSFit) ??

   return ReturnOK;
}
catch(gpstk::Exception& e) {
   LGF << "GFRangePolynomialFit threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end GFRangePolynomialFit()

//------------------------------------------------------------------------------------
// Called by GFSlipFix(). Given two consecutive Segments, with a GF slip between
// them, compute the degree of polynomial, and then fit polynomials to the GF
// Phase, compute RMS residuals of fit, the number of points used in the fit,
// and the estimated discontinuity.
// Note on signs. Note that L2 = GFP-N1-bias2. Thus, if D = fut-past operator,
//                    then DL2 = DGFP - DN1 - Dbias2, but DGFP=0 (no slips)
//                  and so DN1 = -DL2-Dbias2
//
// Return Singular or ReturnOK
int GFSlipEstimate(SVPass& SVP, GDCConfig& GDC, list<Segment>::iterator& itpast,
   list<Segment>::iterator& itfuture, int NFit, double *ROF, int *npts,
   double& discontinuity)
{
try {
   int i,k,nb[2],ne[2];
   double t,d,data,fit,ti,bias;
   DayTime ttag;
   PolyFit<double> SPF[2];

      // unique number for debug purposes
   GFSj++;

      // range of indexes for data in fit
   ne[0] = itpast->nend;
   nb[0] = ne[0]-NFit;
   if(nb[0] < itpast->nbeg) nb[0]=itpast->nbeg;
   nb[1] = itfuture->nbeg;
   ne[1] = nb[1]+NFit;
   if(ne[1] > itfuture->nend) ne[1]=itfuture->nend;

      //--------------------------------------------------------
      // now do the fits and compute the residuals
   for(k=0; k<2; k++) {                      // loop over past(0), future(1)
         // time tag of slip
      ttag = SVP.BegTime;
      ttag += double(itfuture->nbeg) * GDC.DT;

         // fit polynominals
      SPF[k].Reset(3);                    // degree 3 (quadratic) ?DCparam?

         // loop over points in the fit
      npts[k] = 0;
      d = 2.0/double(ne[k]-nb[k]);
      for(i=nb[k]; i<=ne[k]; i++) {
         if(SVP.Flag[i] >= SVPass::OK) {
               // time is rescaled to [-1 to +1]
            t = d*double(i-nb[k]) - 1.0;
            data = SVP.L2[i];
            SPF[k].Add(data,t);
            npts[k]++;
         }
      }

         // error exit
      if(npts[k] < 4) {
         ROF[k] = 0;
         return PrematureEnd;
      }
      if(SPF[k].isSingular()) {
         LGF << "GDC: GF Slip Fix Polynomial fit " << k << " at " << ttag
            << "is singular!\n";
         ROF[k] = 0;                // this flags bad solution
         return Singular;
      }

         // compute residuals of fit
      ROF[k] = 0.0;
      ttag = SVP.BegTime;
      ttag += double(nb[k]) * GDC.DT;
      d = 2.0/double(ne[k]-nb[k]);
         // bias is for output purposes only - if 0, slip shows in plots
      bias = 0.0;
      if(k==1) bias = SVP.L2[itfuture->nbeg] - SVP.L2[itpast->nend];

         // loop over data computing ROF
      for(i=nb[k]; i<=ne[k]; i++) {
         if(SVP.Flag[i] >= SVPass::OK) {
               // 'time' t is scaled to [-1,1]
            t = d*double(i-nb[k]) - 1.0;
            data = SVP.L2[i];
            fit = SPF[k].Evaluate(t);
            ROF[k] = RSS(ROF[k],data-fit);
               // print ttag,GFP(m),t,GFR(m),fit(m),res(m)
            if(GDC.Debug>5) LGF << "GFS " << GDCUnique << " " << SVP.SV << " " << GFSj
               << ttag.printf(" %13.6Q ") << fixed
               << " " << setw(7) << setprecision(3) << t
               << " " << setw(8) << setprecision(3) << data-bias
               << " " << setw(8) << setprecision(3) << fit-bias
               << " " << setw(8) << setprecision(3) << data-fit
               << endl;
         }
         ttag += GDC.DT;
      }  // end loop over data

      ROF[k] /= sqrt(double(npts[k]));

   }  // end loop over past,future fits

      // estimate the discontinuity
      // 'index' of the mid-pt of gap
   ti = itpast->nend + 0.5*(itfuture->nbeg-itpast->nend);
      // time tag
   ttag = SVP.BegTime;
   ttag += double(ti) * GDC.DT;

      // evaluate on the 'future' side
   t = 2*double(ti-nb[1])/double(ne[1]-nb[1]) - 1.0;
   discontinuity = -SPF[1].Evaluate(t);

      // output result of fit
   if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " " << SVP.SV << " " << GFSj
      << " est future at " << ttag << fixed
      << " t=" << setw(7) << setprecision(3) << t
      << " value=" << setw(8) << setprecision(4) << discontinuity << endl;

      // evaluate on the 'past' side
   t = 2*double(ti-nb[0])/double(ne[0]-nb[0]) - 1.0;
   fit = SPF[0].Evaluate(t);
   discontinuity += fit;

      // output result of fit
   if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " " << SVP.SV << " " << GFSj
      << " est past   at " << ttag << fixed
      << " t=" << setw(7) << setprecision(3) << t
      << " value=" << setw(8) << setprecision(4) << fit << endl;

      // include biases
   discontinuity -= itfuture->bias2 - itpast->bias2;

   if(GDC.Debug>1) {
      long n1=long(discontinuity+(discontinuity>0?0.5:-0.5));
      double disc=discontinuity-double(n1);
      if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " " << SVP.SV << " " << GFSj
         << " at " << ttag << " = " << n1 << " + " << fixed
         << setw(6) << setprecision(3) << disc << endl;
      if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " " << SVP.SV << " " << GFSj
         << " fit of future, " << npts[1] << ", RMSROF " << fixed
         << setw(8) << setprecision(3) << ROF[1] << endl;
      if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " " << SVP.SV << " " << GFSj
         << " fit of past,   " << npts[0] << ", RMSROF " << fixed
         << setw(8) << setprecision(3) << ROF[0] << endl;
   }

   return ReturnOK;
}
catch(gpstk::Exception& e) {
   LGF << "GFSlipEstimate threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end GFSlipEstimate(SVP,GDC,it,itprev,nfit,ROF,npts,disc)

//------------------------------------------------------------------------------------
// Loop over Segments: Fit polynomials to either side of the GF slips;
// Fix the GF slips.
//
// After this routine arrays are L1 = L1 phase (cycles)
//                               L2 = GFP(cycles) - bias2
//                               P1 = WLB (cy) - bias1
//                               P2 = GFP-Fit(-GFR)(cycles)
//       i.e. no slips, and bias2 is in units of cycles (of wl21)
//
int GFSlipFix(SVPass& SVP, GDCConfig& GDC)
{
try {
   if(GDC.Debug > 5) LGF << "GF.............................................."
      << "..................................\n";

   if(SegList.size() <= 1) return ReturnOK;

   bool fix[5];
   int i,j,k,ngap,nfit,nfit0,npts[2];
   int NpreEnd,NfutBeg,NpreEm1,NfutBm1,FfutBeg,FfutBm1;
   long n1[5];
   double disc[5],ROF[10],bias,test;
   list<Segment>::iterator it,itprev;

   GFSj = 0;
      // bias2 and L2 are in units of cycles of wl21
      // start with the second segment
   it = SegList.begin();
   for(j=2,it++; it != SegList.end(); j++,it++) {
      if(SVP.Flag[it->nbeg]>0 && (SVP.Flag[it->nbeg] & SVPass::SLIPGF)) {
            // found a GF slip
            // Segment in the past of the slip (it is in future of slip)
         itprev = it;
         itprev--;
            // nominal slip - for output purposes only
         bias = SVP.L2[it->nbeg] - SVP.L2[itprev->nend];

            // compute number of points to fit
         nfit = GDC.GFFitNMin;
         ngap = it->nbeg - itprev->nend - 1;
         nfit = int(GDC.GFFitTime/GDC.DT) + 2*ngap + 2;
         if(nfit < GDC.GFFitNMin) nfit=GDC.GFFitNMin;
   
            // do the estimation up to 5 times: first regular, then
            // remove 2 points closest to slips, then 2 more, then
            // half as many points, then twice as many
         k = 0;
         do {
               // ---------------------------------------------------------
               // fit polynomials, compute stats, estimate discontinuity
               // TD figure out how many good points there are before calling
               // TD do all the different fits, then choose the best.
            i = GFSlipEstimate(SVP,GDC,itprev,it,nfit,&ROF[2*k],npts,disc[k]);
            if(i < ReturnOK) return i;
            // set ROF=0 to flag bad solution if(i > ReturnOK) fix=false;
            else {   // increment k and try again
               k++;
               if(k==1) {              // first re-try: remove two points
                  NpreEnd = itprev->nend;
                  NfutBeg = it->nbeg;
                  FfutBeg = SVP.Flag[it->nbeg];
                  SVP.Flag[itprev->nend] = SVP.Flag[it->nbeg] = -99;
                  while(SVP.Flag[itprev->nend] < SVPass::OK) itprev->nend--;
                  while(SVP.Flag[it->nbeg] < SVPass::OK) it->nbeg++;
               }
               else if(k==2) {         // second re-try: remove two more
                  NpreEm1 = itprev->nend;
                  NfutBm1 = it->nbeg;
                  FfutBm1 = SVP.Flag[it->nbeg];
                  SVP.Flag[itprev->nend] = SVP.Flag[it->nbeg] = -99;
                  while(SVP.Flag[itprev->nend] < SVPass::OK) itprev->nend--;
                  while(SVP.Flag[it->nbeg] < SVPass::OK) it->nbeg++;
               }
               else if(k==3) {         // third re-try: halve npts
                  SVP.Flag[NpreEnd] = SVP.Flag[NpreEm1] = SVPass::OK;
                  SVP.Flag[NfutBeg] = FfutBeg;
                  SVP.Flag[NfutBm1] = FfutBm1;
                  itprev->nend = NpreEnd;
                  it->nbeg = NfutBeg;
                  nfit0 = nfit;
                  nfit = nfit0/2;
                  if(nfit < 8) nfit=8; // absolute minimum ?DCparam?
                  // ? increase degree of polynomial?
               }
               else if(k==4) {         // fourth re-try: double npts
                  nfit = 2*nfit0;
               }
               else break;             // give up
            }  // end if successful fit
         } while(1);

            // choose the best result
         int kmin=-1;
         for(k=0; k<5; k++) {
            if(ROF[2*k]==0 || ROF[2*k+1]==0) { fix[k]=false; continue; }

               // divide into integer and fractional parts
               // GFP+GFR = wl2*NWL - wl21*N1 so n1 = -N1
            n1[k] = long(disc[k] + (disc[k]>0?0.5:-0.5));
            disc[k] -= double(n1[k]);

               // ----------------------------------
               // Here is the test: can it be fixed?
            fix[k] = true;

               // (1) check the gap - if too large, must fail
            test = (it->nbeg-itprev->nend)*GDC.DT;
            if(test > GDC.GFFixMaxGap) {
               if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " " << SVP.SV
                  << " " << j << "." << GFSj << " fix FAILS (Gap too large " << fixed
                  << setw(5) << setprecision(3) << test << " > "
                  << setw(5) << setprecision(3) << GDC.GFFixMaxGap << ")\n";
               fix[k] = false;
            }
               // (2) RMS residual of fit is small, on both sides
            if(ROF[2*k] >= GDC.GFFixSigma || ROF[2*k+1] >= GDC.GFFixSigma) {
               if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " " << SVP.SV
                  << " " << j << "." << GFSj << " fix FAILS (Sigma too large "
                  << fixed
                  << setw(7) << setprecision(3) << test << " | " << ROF[2*k]
                  << setw(7) << setprecision(3) << test << " >= " << ROF[2*k+1]
                  << setw(7) << setprecision(3) << GDC.GFFixSigma << ")\n";
               fix[k] = false;
            }
               // (3) fractional part of offset is small
            //if(fabs(disc[k]) > C.GFFixFrac) {
            //   if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " " << SVP.SV
            //      << " " << j << "." << GFSj
            //      << " fix FAILS (Fractional part large " << fixed
            //      << setw(6) << setprecision(4) << fabs(disc[k]) << " > "
            //      << setw(6) << setprecision(4) << GDC.GFFixFrac << ")\n";
            // fix[k] = false;
            //}

            if(kmin == -1 ||
               (ROF[2*k]+ROF[2*k+1] < ROF[2*kmin]+ROF[2*kmin+1])) kmin=k;

         }  // end loop over 5 possible fixes

            // ------------------------------------------------------
            // fix the slip
         if(kmin > -1 && fix[kmin]) {                 // success
            SVP.Flag[it->nbeg] |= SVPass::FIXGF;
            SVP.Flag[it->nbeg] ^= SVPass::SLIPGF;
            it->N1 = n1[kmin];
            if(GDC.Debug > 0) {
               DayTime ttag;
               ttag = SVP.BegTime;
               ttag += double(it->nbeg) * GDC.DT;
               if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " "
                  << SVP.SV << " " << GFSj
                  << " FIX at " << ttag << " = " << n1[kmin] << fixed
                  << " " << setw(6) << setprecision(3) << disc[kmin] << endl;
               if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " "
                  << SVP.SV << " " << GFSj
                  << " fit of past,   RMSROF " << fixed
                  << setw(8) << setprecision(3) << ROF[2*kmin] << endl;
               if(GDC.Debug>6) LGF << "GF Slip " << GDCUnique << " "
                  << SVP.SV << " " << GFSj
                  << " fit of future, RMSROF " << fixed
                  << setw(8) << setprecision(3) << ROF[2*kmin+1] << endl;
            }
         }
         else if(kmin > -1) it->N1=n1[kmin];        // failure but have N1
         else it->N1 = 0;                           // failure

            // Make NWL -> N2
         it->NWL = it->N1 - it->NWL;   // N1-(N1-N2)=N2
            
      }  // end if GF slip was previously found

      else {                          // no GF slip -> N1==0
         it->N1 = 0;                  // may not be necessary...
         it->NWL = -it->NWL;          // NWL -> N2 = 0-NWL
      }

   }  // end loop over segments

   return ReturnOK;
}
catch(gpstk::Exception& e) {
   LGF << "GFSlipFix threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end GFSlipFix(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// Loop over the data and correct the L1 and L2 data arrays. First correct for
// N1 in both L1 and GFP, then solve for L1 and L2 and convert to cycles.
int CorrectData(SVPass& SVP, GDCConfig& GDC)
{
try {
   int i,j;
   long N1;
   double bias1,bias2;
   DayTime ttag;
   list<Segment>::iterator it;

      // loop over all the data in the SVPass,
      // keeping pointer to Segment j at same time
   j = 0;
   N1 = 0;
   it = SegList.begin();
   bias1 = 0;
   bias2 = it->bias2*wl21;        // overall GFP bias in meters
   ttag = SVP.BegTime;

   for(i=0; i<SVP.Length; i++) {
         // check for beginning of Segment
         // N1 must accumulate
      if(i == it->nbeg) {
         if(SVP.Flag[i] & SVPass::FIXGF) N1 += it->N1;
         else N1 = 0;
      }

         // modify the data
      if(i >= it->nbeg && (SVP.Flag[i]>=SVPass::OK || SVP.Flag[i]==SVPass::SETBAD)) {
         SVP.L1[i] -= N1;                       // correct L1 for GF slips
         SVP.L2[i] += N1;                       // correct GFP for GF slips
         SVP.L2[i] += it->bias2;                // restore bias on GFP
         SVP.L2[i] *= wl21;                     // convert to GF (m)
         SVP.L2[i] = wl1*SVP.L1[i]-SVP.L2[i];   // convert to L2 (m)
         SVP.L2[i] /= wl2;                      // convert to L2 cycles

            // output WL range minus phase and GF phase
         //if(dormp && C.fplog && C.Debug>1) {
         // double gfp = gf1p*SVP.L1[i]+gf2p*SVP.L2[i];  // GFP(meters)
         // gfp -= bias2;
         // fprintf(C.fplog,"%s %d %2.2d %13.6f %13.3f %13.3f\n",
         //    (SVP.Flag[i]>=SVPass::OK ? "RMP ":"RMPX"),
         //    GDCUnique,SVP.SV.Prn(),t.MJD(),SVP.P1[i]-bias1,gfp);
         //}
      }

         // check for end of Segment - need it for N1, bias2
      if(i == it->nend) {
         bias1 += it->bias1;         // output purposes only
         j++;
         it++;
         if(it==SegList.end()) break;
         bias1 -= it->bias1;         // output purposes only
      }

      ttag += GDC.DT;
   }  // end for loop over all data

   if(GDC.Debug > 4) DumpSegments("AFT",GDCUnique,GDC,SVP);

   return ReturnOK;
}
catch(gpstk::Exception& e) {
   LGF << "CorrectData threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end CorrectData(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
// Modify nbeg or nend for bad points at beginning and end, and create new
// Segments where SLIPs have been marked. Modify the bias.
// Then delete Segments smaller than the limit...this is only place where
// Segments are deleted.
void CorrectSegments(SVPass& SVP, GDCConfig& GDC)
{
try {
   int i,j,nb,ne;
   list<Segment>::iterator it,jt;

      // loop over segments, changing start and stop
   for(it=SegList.begin(); it != SegList.end(); it++) {
         // change start point
      nb = it->nbeg;
      ne = it->nend;
      while(nb <= ne && SVP.Flag[nb] < SVPass::OK) nb++;
      it->nbeg = nb;
      while(ne >= nb && SVP.Flag[ne] < SVPass::OK) ne--;
      it->nend = ne;
   }

      // now delete small Segments
   for(j=0,it=SegList.begin(); it != SegList.end(); j++,it++) {
      jt = it;
      jt++;            // jt points to the next Segment
      if(it->npts < GDC.MinPts || GDC.DT*(it->nend-it->nbeg+1) < GDC.MinSeg) {
         if(GDC.Debug>4) LGF << "DEL " << GDCUnique << " " << SVP.SV << " Seg #"
            << setw(2) << j+1 << ": " << setw(3) << it->npts << " pts, # "
            << setw(3) << it->nbeg << "-" << setw(4) << it->nend << endl;

            // remove from list
         if(SVP.Flag[it->nbeg] > SVPass::SLIP) {    // it has been fixed
            // if deleting a Segment that has been fixed, give flags to next Segment
            if(jt != SegList.end()) {
               if(SVP.Flag[it->nbeg] & SVPass::FIXWL)
                  SVP.Flag[jt->nbeg] |= SVPass::FIXWL;
               if(SVP.Flag[it->nbeg] & SVPass::FIXGF)
                  SVP.Flag[jt->nbeg] |= SVPass::FIXGF;
            }
            //else GDCNFix--;
         }
            // mark all the points bad
         for(i=it->nbeg; i<=it->nend; i++)
            if(SVP.Flag[i] >= SVPass::OK) SVP.Flag[i]=SVPass::SETBAD;

            // remove the segment from the list
         SegList.erase(it);
         it = jt;
      }
   }

      // if the first Segment gets deleted, it leaves a SLIP flag on SL[0]
   if(SegList.size() <= 0) return;
   SVP.Flag[SegList.begin()->nbeg] = SVPass::OK;
}
catch(gpstk::Exception& e) {
   LGF << "CorrectSegments threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
}  // end CorrectSegments(SVPass& SVP, GDCConfig& GDC)

//------------------------------------------------------------------------------------
void DumpSegments(const string& lab, int N, GDCConfig& GDC, SVPass& SVP)
{
try {
   if(GDC.Debug<5) return;

   int i,j;
   DayTime ttag;
   list<Segment>::iterator it,itprev;

      // first print a list of Segments and stats
   LGF << lab << " list of Segments (" << SegList.size()
      << "): (" << GDCNFix << " fixed, " << GDCNFail << " failed)\n";
   for(i=1,it=SegList.begin(); it!=SegList.end(); i++,it++) {
      LGF << lab << "List " << GDCUnique << " " << SVP.SV << " #" << i << ": ";
      if(SVP.Flag[it->nbeg] == SVPass::OK) LGF << " 000";
      if(SVP.Flag[it->nbeg] & SVPass::SLIPWL) LGF << " SWL";
      if(SVP.Flag[it->nbeg] & SVPass::FIXWL) LGF << " FWL";
      if(SVP.Flag[it->nbeg] & SVPass::SLIPGF) LGF << " SGF";
      if(SVP.Flag[it->nbeg] & SVPass::FIXGF) LGF << " FGF";
      LGF << setw(4) << it->npts << " pts, # "
         << setw(4) << it->nbeg << "-" << setw(4) << it->nend;
      ttag = SVP.BegTime;
      ttag += it->nbeg * GDC.DT;
      LGF << " (" << ttag;
      ttag = SVP.BegTime;
      ttag += it->nend * GDC.DT;
      LGF << ttag;
      LGF << ") bias1=" << fixed << setw(13) << setprecision(3) << it->bias1
           << " bias2=" << fixed << setw(13) << setprecision(3) << it->bias2;
      if(it != SegList.begin()) LGF << " Gap " << setw(5) << setprecision(1) <<
         GDC.DT*(it->nbeg-itprev->nend) << " s = " << 
            it->nbeg-itprev->nend << " pts";
      LGF << endl;
      itprev = it;
   }

   if(GDC.Debug < 6) return;

   for(i=1,it=SegList.begin(); it!=SegList.end(); i++,it++) {
      ttag = SVP.BegTime;
      ttag += it->nbeg*GDC.DT;
         // don't print when BAD b/c data may be corrupt due to absence of satellite
      for(j=it->nbeg; j<=it->nend; j++) {
         if(SVP.Flag[j] != SVPass::BAD) {
            LGF << "DSC" << lab << " " << N << " " << SVP.SV << " " << i
               << " " << ttag.printf("%13.6Q") << setw(3) << SVP.Flag[j]
               << " " << fixed << setw(13) << setprecision(3) << SVP.L1[j]
               << " " << fixed << setw(13) << setprecision(3) << SVP.L2[j]
               << " " << fixed << setw(13) << setprecision(3) << SVP.P1[j]
               << " " << fixed << setw(13) << setprecision(3) << SVP.P2[j];
            if(SVP.A1 && SVP.A2) LGF
               << " " << fixed << setw(13) << setprecision(3) << SVP.A1[j]
               << " " << fixed << setw(13) << setprecision(3) << SVP.A2[j];
            LGF << " " << setw(4) << j;
            if(j==it->nbeg) LGF
               << " " << fixed << setw(13) << setprecision(3) << it->bias1
               << " " << fixed << setw(13) << setprecision(3) << it->bias2
               << " " << it->N1 << " " << it->NWL;
            LGF << endl;
         }
         ttag += GDC.DT;
      }
   }
}
catch(gpstk::Exception& e) {
   LGF << "DumpSegments threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
}  // end DumpSegments(char *lab, int N, GDCConfig& GDC, SVPass& SVP)   // ? string?

//------------------------------------------------------------------------------------
// Set all SETBAD points to BAD.
// Also mark the beginning of every segment as slip, unless it was fixed.
// This is the only place where editing commands are generated.
int OutputAndQuit(SVPass& SVP, GDCConfig& GDC, vector<string>& EditCmds, int iret)
{
try {
   int i,j,inbad,nbad;
   DayTime ttag;
   string cmd;

   EditCmds.clear();

      // loop over points in the entire pass, looking for data set bad by DC
   for(nbad=0,i=0; i<SVP.Length; i++) {
      if(SVP.Flag[i] == SVPass::SETBAD) {       // data has been set bad
         if(nbad == 0) inbad = i;
         nbad++;
      }
      if(nbad > 0 && GDC.FixOutput
            && (SVP.Flag[i] != SVPass::SETBAD || i==SVP.Length-1)) {
            // write out delete commands for bad data
         ttag = SVP.BegTime;
         ttag += double(inbad) * GDC.DT;
         if(nbad == 1) {         // delete a single point
            if(GDC.Debug) {
               ostringstream stst;
               stst << "# delete a single point";
               EditCmds.push_back(stst.str());
            }
            {
               ostringstream stst;
               stst << "-DS" << SVP.SV << ",";
               if(GDC.OutputGPSTime) stst << ttag.printf("%F,%g");
               else stst << ttag.printf("%Y,%m,%d,%H,%M,%f");
               EditCmds.push_back(stst.str());
            }
         }
         else {                  // delete a range
            if(GDC.Debug) {
               ostringstream stst;
               stst << "# delete " << nbad << " points";
               EditCmds.push_back(stst.str());
            }
            {
               ostringstream stst;
               stst << "-DS+" << SVP.SV << ",";
               if(GDC.OutputGPSTime) stst << ttag.printf("%F,%g");
               else stst << ttag.printf("%Y,%m,%d,%H,%M,%f");
               EditCmds.push_back(stst.str());
            }
            {
               ostringstream stst;
               ttag += double(nbad) * GDC.DT;
               stst << "-DS-" << SVP.SV << ",";
               if(GDC.OutputGPSTime) stst << ttag.printf("%F,%g");
               else stst << ttag.printf("%Y,%m,%d,%H,%M,%f");
               EditCmds.push_back(stst.str());
            }
         }
         nbad = 0;
      }

      if(SVP.Flag[i] < SVPass::OK) {
         // set all bad data to BAD, the only 'bad' flag the caller has
         SVP.Flag[i]=SVPass::BAD;
      }
   }

      // loop over Segments looking for slips
   list<Segment>::iterator it;
   for(it=SegList.begin(); it != SegList.end(); it++) {
      j = it->nbeg;
      ttag = SVP.BegTime;
      ttag += double(j) * GDC.DT;

      if(SVP.Flag[j] & SVPass::SLIPWL ||
         SVP.Flag[j] & SVPass::SLIPGF ||
         SVP.Flag[j] & SVPass::FIX) {     // slip was found
   
         if(GDC.Debug>3) LGF << "DSCSLP " << GDCUnique << " " << SVP.SV
            << " " << setw(2) << i+1
            << ttag.printf(" %13.6Q ") << setw(2) << SVP.Flag[j] << fixed
            << " " << setw(13) << setprecision(3) << SVP.L1[j]
            << " " << setw(13) << setprecision(3) << SVP.L2[j]
            << " " << setw(13) << setprecision(3) << SVP.P1[j]
            << " " << setw(13) << setprecision(3) << SVP.P2[j] << endl;

            // L1
            // fix slip, even if fix failed....
         {
            ostringstream stst;
            stst << "-BD+" << SVP.SV << ",L1,";
            if(GDC.OutputGPSTime) stst << ttag.printf("%F,%g");
            else stst << ttag.printf("%Y,%m,%d,%H,%M,%f");
            stst << "," << -(it->N1) << (SVP.Flag[j]&SVPass::FIX?"":"#fix failed");
            EditCmds.push_back(stst.str());
         }
            // if fix failed, set loss of lock flag
         if(SVP.Flag[j] & SVPass::SLIPWL || SVP.Flag[j] & SVPass::SLIPGF) {
            ostringstream stst;
            stst << "-SL" << SVP.SV << ",L1,";
            if(GDC.OutputGPSTime) stst << ttag.printf("%F,%g");
            else stst << ttag.printf("%Y,%m,%d,%H,%M,%f");
            stst << ",1";
            EditCmds.push_back(stst.str());
         }

            // L2
            // fix slip, even if fix failed... N2 = it->NWL now
         {
            ostringstream stst;
            stst << "-BD+" << SVP.SV << ",L2,";
            if(GDC.OutputGPSTime) stst << ttag.printf("%F,%g");
            else stst << ttag.printf("%Y,%m,%d,%H,%M,%f");
            stst << "," << -(it->NWL) << (SVP.Flag[j]&SVPass::FIX?"":"#fix failed");
            EditCmds.push_back(stst.str());
         }
            // if fix failed, set loss of lock flag
         if(SVP.Flag[j] & SVPass::SLIPWL || SVP.Flag[j] & SVPass::SLIPGF) {
            ostringstream stst;
            stst << "-SL" << SVP.SV << ",L2,";
            if(GDC.OutputGPSTime) stst << ttag.printf("%F,%g");
            else stst << ttag.printf("%Y,%m,%d,%H,%M,%f");
            stst << ",1";
            EditCmds.push_back(stst.str());
            GDCNFail++;
         }
         else GDCNFix++;
      }
      else {         // mark loss of lock on any segment without slip
         {                 // set L1 LLI flag to 1
            ostringstream stst;
            stst << "-SL" << SVP.SV << ",L1,";
            if(GDC.OutputGPSTime) stst << ttag.printf("%F,%g");
            else stst << ttag.printf("%Y,%m,%d,%H,%M,%f");
            stst << ",1";
            EditCmds.push_back(stst.str());
         }
         {                 // set L2 LLI flag to 1
            ostringstream stst;
            stst << "-SL" << SVP.SV << ",L2,";
            if(GDC.OutputGPSTime) stst << ttag.printf("%F,%g");
            else stst << ttag.printf("%Y,%m,%d,%H,%M,%f");
            stst << ",1";
            EditCmds.push_back(stst.str());
         }
            // reset slip count
            // mark beginning of every segment as slip, unless fixed
         SVP.Flag[j] |= SVPass::SLIP;
      }
   }

      // dump the edit commands to the output
   if(GDC.Debug>2 && EditCmds.size()>0) {
      LGF << "Results (RinexEdit commands):\n";
      for(i=0; i<EditCmds.size(); i++) LGF << EditCmds[i] << endl;
   }

   if(iret && GDC.Debug>1) {
      LGF << "GPSTK Discontinuity Corrector returning (" << iret << "): ";
      if(iret==FatalProblem) LGF << "Fatal problem";
      else if(iret==PrematureEnd) LGF << "Too few points; pass rejected";
      else if(iret==Singular) LGF << "Singular GFR polynomial fit";
      LGF << endl;
   }

   if(GDC.Debug > 4) {
      LGF << "GDC " << setw(2) << GDCUnique << ", PRN " << SVP.SV
         << ", Pts " << setw(4)
         << SVP.Npts << ", " << SVP.BegTime << " Slip " << GDCNFix+GDCNFail
         << ", Fix " << GDCNFix << ", Fail " << GDCNFail << endl;
      LGF << "========End GPSTK Discontinuity Corrector " << GDCUnique << " ("
         << iret << ") ============================================\n\n";
   }

      // destroy the Segment list
   SegList.clear();

   return iret;
}
catch(gpstk::Exception& e) {
   LGF << "OutputAndQuit threw an exception\n" << e << endl;
   GPSTK_RETHROW(e);
}
   return FatalProblem;
}  // end OutputAndQuit(SVPass& SVP, GDCConfig& C, vector<string>& EditCmds, int iret)

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// GDCConfig routines
//------------------------------------------------------------------------------------
// Set defaults
GDCConfig::GDCConfig(void)
{
//Misc
   oflog = &cout;
   help = false;
   Debug = 0;
   DT = 0;
//Editing
   MinRange = 1.e4;
   MaxRange = 1.e16;
//Segment
   MaxGap = 180;
   MinSeg = 200;
   MinPts = 13;
//WL R-Ph processing
   WLNSigma = 4;
   WLSSWindowWidth = 10;
   WLSSTestMin = 0.7;
   WLSSDetectMin = -0.25;
//WL slip estimation
   WLFixMaxGap=2500;
   WLFixNpts = 4;
   WLFixSigma = 0.15;
//GF Ph & Fit(R) processing
   GFPolyMaxDegree = 10;
   GFRMaxRMSFit = 0.0;      // not implemented yet
   GFDetectMaxSigma = 10.0;
   GFOnlySlipTol = 10.;
   GFSlipTol = 0.9;
   GFNOutliers = 3;
//GF slip estimation
   GFFixMaxGap=250;
   GFFitTime = 300;
   GFFitNMin = 8;
   GFFixSigma = 0.40;
// Output
   OutputGPSTime = true;
   FixOutput = true;
}

//------------------------------------------------------------------------------------
// Set a parameter in the configuration;
// cmd is of the form '<id>,<value>' or '<id>=<value>' or '<id>:<value>' ;
// to see a list of possible choices, call with cmd="help".
// optional leading '--DC' is permitted
void GDCConfig::SetParameter(string cmd)
{
try {
      // ignore leading '-'s
   while(cmd.size() && (cmd[0]=='-' || (cmd[0]==' '||cmd[0]=='\t'))) cmd.erase(0,1);
   if(cmd[0]=='D' && cmd[1]=='C') cmd.erase(0,2);

   string id,value;
   string::size_type pos=cmd.find_first_of(",=:");
   if(pos==string::npos) {
      id=cmd;
      value="";
   }
   else {
      id=cmd.substr(0,pos);
      value=cmd;
      value.erase(0,pos+1);
   }
   double dval=StringUtils::asDouble(value);
   if(Debug>1) *oflog << "   GDC::SetParameter with " << id << " = "
      << value << "(" << dval << ")\n";
   //&& (*oflog).good() //
   if(id == string("help")) help=true;
   else if(id == string("NoFixOutput")) FixOutput=false;
   else if(id == string("OutputGPSTime")) OutputGPSTime=!OutputGPSTime;
   else if(id == string("Debug")) Debug=int(dval);
   else if(id == string("DT")) DT=dval;
   else if(id == string("MaxRange")) MaxRange=dval;
   else if(id == string("MinRange")) MinRange=dval;
   else if(id == string("MinPts")) MinPts=int(fabs(dval)+0.5);
   else if(id == string("MinSeg")) MinSeg=dval;
   else if(id == string("WLNSigma")) WLNSigma=dval;
   else if(id == string("WLSSWindowWidth")) WLSSWindowWidth=int(dval);
   else if(id == string("WLSSTestMin")) WLSSTestMin=dval;
   else if(id == string("WLSSDetectMin")) WLSSDetectMin=dval;
   else if(id == string("WLFixMaxGap")) WLFixMaxGap=fabs(dval);
   else if(id == string("WLFixNpts")) WLFixNpts=int(fabs(dval)+0.5);
   else if(id == string("WLFixSigma")) WLFixSigma=fabs(dval);
   else if(id == string("GFPolynomMaxDeg")) GFPolyMaxDegree=int(dval);
   else if(id == string("GFRMaxRMSFit")) GFRMaxRMSFit=dval;
   else if(id == string("GFDetectMaxSigma")) GFDetectMaxSigma=dval;
   else if(id == string("GFOnlySlipTol")) GFOnlySlipTol=fabs(dval);
   else if(id == string("GFSlipTol")) GFSlipTol=fabs(dval);
   else if(id == string("GFNOutliers")) {
      GFNOutliers = int(fabs(dval)+0.5);
      if(GFNOutliers < 3) GFNOutliers=3;
      if(GFNOutliers > 9) GFNOutliers=9;
   }
   else if(id == string("GFFitTime")) GFFitTime=int(fabs(dval)+0.5);
   else if(id == string("GFFitNMin")) GFFitNMin=int(fabs(dval)+0.5);
   else if(id == string("GFFixMaxGap")) GFFixMaxGap=fabs(dval);
   else if(id == string("GFFixSigma")) GFFixSigma=fabs(dval);
   else {
      //if((*oflog).good())
      *oflog << "  GDC::SetParam Unknown command: " << id << " = "
         << value << "(" << dval << ")\n";
      return;
   }
   return;
}
catch(gpstk::Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// Print help page
void GDCConfig::DisplayParameterUsage(ostream& os)
{
try {
   os << "\nGPSTK Discontinuity Corrector (Version " << GDCVersion
      << ") parameter commands:\n";
   os << "      (each has the form 'label,value' OR 'label=value' "
      << "with no whitespace)\n";
   os << "  Debug=<x>    x: level of debugging output: 0(silent)-7(debugging)\n";
   os << "  DT=<x>       x: nominal timestep in data (seconds) - "
      << "(required : no default!)\n";
   //os << "  MaxGap=<x>   x: maximum allowed gap in data (seconds)\n";
   os << "     Data Editing:\n";
   os << "  MinRange=<x> x: minimum allowed pseudorange (meters)\n";
   os << "  MaxRange=<x> x: maximum allowed pseudorange (meters)\n";
   os << "  MinPts=<x>   x: minimum number of good points in phase segment ()\n";
   os << "  MinSeg=<x>   x: minimum acceptable length of phase segment (seconds)\n";
   os << "     Wide-lane slip detection:\n";
   os << "  WLNSigma=<x>        x: n-sigma WL slip detection ()\n";
   os << "  WLSSWindowWidth=<x> x: sliding window width (xMinWidth)\n";
   os << "  WLSSTestMin=<x>     x: minimum delta averages to start test "
      << "(WL cycles)\n";
   os << "  WLSSDetectMin=<x>   x: minimum del-averages: detect WL small slip "
      << "(WL cy)\n";
   os << "     Wide-lane slip estimation:\n";
   os << "  WLFixMaxGap=<x>     x: Maximum allowed gap between segments (sec)\n";
   os << "  WLFixNpts=<x>       x: Minimum number points (each segment) req'd to fix "
      << "()\n";
   os << "  WLFixSigma=<x>      x: Maximum total error on WL R-Ph to fix "
      << "(WL cycles)\n";
   os << "     Geometry-free slip detection:\n";
   os << "  GFPolynomMaxDeg=<x> x: Maximum degree of polynomial fit to GF Range ()\n";
   os << "  GFRMaxRMSFit=<x>    x: Maximum allowed RMS residual of fit to GFRange "
      << "(m)\n";
   os << "  GFDetectMaxSigma=<x>x: Maximum allowed GF res. error: detect slip "
      << "(cyc)\n";
   os << "  GFOnlySlipTol=<x>   x: Minimum slip on GF (no WL slip) "
      << "(cycles WL2-WL1)\n";
   os << "  GFSlipTol=<x>       x: Minimum slip on GF (with WL slip) "
      << "(cycles WL2-WL1)\n";
   os << "  GFNOutliers=<x>     x: Maximum # consecutive GF outliers w/o slip "
      << "(>=3,<10)\n";
   os << "     Geometry-free slip estimation:\n";
   os << "  GFFixMaxGap=<x>     x: Maximum allowed gap between segments (sec)\n";
   os << "  GFFitTime=<x>       x: Time span in fitting GF Phase for slip fix "
      << "(sec)\n";
   os << "  GFFitNMin=<x>       x: Min number of data points in fit of GF Ph ()\n";
   os << "  GFFixSigma=<x>      x: Maximum RMS resid. of fit on GF Ph to fix "
      << "(GF cycles)\n";
   os << "     Output:\n";
   os << "  OutputGPSTime       Toggle GPS/YMDHMS time format in editing "
      << "command output\n";
   os << "  NoFixOutput         Turn off putting delete commands in output "
      << "cmd list\n";
   os << "\n";
   os << "  help                Print this message.\n";
   os << " For example, 'DT=30' or 'MinSeg:180' are valid commands\n";

      // now print current values
   os << "\nCurrent values:\n";
   PrintParameters(os);
}
catch(gpstk::Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// Print current values of the parameters
void GDCConfig::PrintParameters(ostream& os)
{
   os << "  DT     = " << fixed << setprecision(3) << DT << " (seconds)\n";
   //os << "  MaxGap = " << fixed << setprecision(3) << MaxGap << " (seconds)\n";
   os << "     Data Editing:\n";
   if(fabs(MinRange) > 99999999.)
   os << "  MinRange    = " << scientific << setprecision(3)
      << MinRange << " (meters)\n";
   else
   os << "  MinRange    = " << fixed << setprecision(3)
      << MinRange << " (meters)\n";
   if(fabs(MaxRange) > 99999999.)
   os << "  MaxRange    = " << scientific << setprecision(3)
      << MaxRange << " (meters)\n";
   else
   os << "  MaxRange    = " << fixed << setprecision(3) << MaxRange << " (meters)\n";
   os << "  MinPts      = " << MinPts << " (points)\n";
   os << "  MinSeg      = " << fixed << setprecision(3) << MinSeg << " (seconds)\n";
   os << "     Wide-lane slip detection:\n";
   os << "  WLNSigma        = " << fixed << setprecision(3) << WLNSigma << " ()\n";
   os << "  WLSSWindowWidth = " << WLSSWindowWidth << " (times min width)\n";
   os << "  WLSSTestMin     = " << fixed << setprecision(3)
      << WLSSTestMin << " (WL cycles)\n";
   os << "  WLSSDetectMin   = " << fixed << setprecision(3)
      << WLSSDetectMin << " (WL cycles)\n";
   os << "     Wide-lane slip estimation:\n";
   os << "  WLFixMaxGap = " << fixed << setprecision(3)
      << WLFixMaxGap << " (seconds)\n";
   os << "  WLFixNpts   = " << WLFixNpts << " (points)\n";
   os << "  WLFixSigma  = " << fixed << setprecision(3)
      << WLFixSigma << " (WL cycles)\n";
   os << "     Geometry-free slip detection:\n";
   os << "  GFPolynomMaxDeg  = " << GFPolyMaxDegree << " ()\n";
   os << "  GFRMaxRMSFit     = " << fixed << setprecision(3)
      << GFRMaxRMSFit << " (m)\n";
   os << "  GFDetectMaxSigma = " << fixed << setprecision(3)
      << GFDetectMaxSigma << " (5.4cm cycles)\n";
   os << "  GFOnlySlipTol    = " << fixed << setprecision(3)
      << GFOnlySlipTol << " (5.4cm cycles)\n";
   os << "  GFSlipTol        = " << fixed << setprecision(3)
      << GFSlipTol << " (5.4cm cycles)\n";
   os << "  GFNOutliers      = " << GFNOutliers << " (points)\n";
   os << "     Geometry-free slip estimation:\n";
   os << "  GFFixMaxGap = " << fixed << setprecision(3)
      << GFFixMaxGap << " (seconds)\n";
   os << "  GFFitTime   = " << GFFitTime << " (seconds)\n";
   os << "  GFFitNMin   = " << GFFitNMin << " (points)\n";
   os << "  GFFixSigma  = " << fixed << setprecision(3)
      << GFFixSigma << " (GF cycles)\n";
   os << "     Output:\n";
   os << "  Debug = " << Debug << endl;
   os << "  OutputGPSTime = Output time in "
      << (OutputGPSTime?"GPS":"Conventional") << " format\n";
   os << "  NoFixOutput   = " << (FixOutput?"DO":"Do NOT")
      << " delete outliers in output commands.\n";
}

//------------------------------------------------------------------------------------
// SVPass routines
//------------------------------------------------------------------------------------
const int SVPass::OK      =   0;      // good data, no discontinuity
const int SVPass::BAD     =  -1;      // used by caller and GDC to mark bad data
const int SVPass::SETBAD  =  -2;      // used within GDC only
const int SVPass::GFBAD   = -16;      // used within GDC only
// good data with discontinuity may have any or all of these:
const int SVPass::SLIPWL  =   1;
const int SVPass::SLIPGF  =   2;
const int SVPass::FIXWL   =   4;
const int SVPass::FIXGF   =   8;

const int SVPass::SLIP    =   3;      // 1+2 both slips found, neither fixed
const int SVPass::FIX     =  12;      // 4+8 both slips found and fixed

//------------------------------------------------------------------------------------
// empty (and only) constructor
SVPass::SVPass(void)
{
   L1 = L2 = P1 = P2 = NULL;
   A1 = A2 = NULL;
   E1 = E2 = NULL;
   Flag = NULL;
   Npts = Length = 0;
   Extra = false;
   BegTime=DayTime(DayTime::BEGINNING_OF_TIME);
   EndTime=DayTime(DayTime::END_OF_TIME);
}

//------------------------------------------------------------------------------------
SVPass::~SVPass(void)
{
   if(Length==0) return;
   delete[] L1;
   delete[] L2;
   delete[] P1;
   delete[] P2;
   delete[] A1;
   delete[] A2;
   delete[] E1;
   delete[] E2;
   delete[] Flag;
   Length = 0;
}

//------------------------------------------------------------------------------------
void SVPass::Resize(int n)
{
try {
   (*this).~SVPass();
   if(n <= 0) return;

   L1 = new double[n]; if(!L1) { OutOfMemory e("L1"); GPSTK_THROW(e); }
   L2 = new double[n]; if(!L2) { OutOfMemory e("L2"); GPSTK_THROW(e); }
   P1 = new double[n]; if(!P1) { OutOfMemory e("P1"); GPSTK_THROW(e); }
   P2 = new double[n]; if(!P2) { OutOfMemory e("P2"); GPSTK_THROW(e); }
   // do this in WLSlipDetect
   //A1 = new double[n]; if(!A1) { OutOfMemory e("A1"); GPSTK_THROW(e); }
   //A2 = new double[n]; if(!A2) { OutOfMemory e("A2"); GPSTK_THROW(e); }
   if(Extra) {
      E1 = new double[n]; if(!E1) { OutOfMemory e("E1"); GPSTK_THROW(e); }
      E2 = new double[n]; if(!E2) { OutOfMemory e("E2"); GPSTK_THROW(e); }
   }
   Flag = new int[n]; if(!Flag) { OutOfMemory e("Flag"); GPSTK_THROW(e); }
   for(int i=0; i<n; i++) Flag[i]=SVPass::BAD;
   Length = n;
   Npts = 0;
   return;
}
catch(gpstk::Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
