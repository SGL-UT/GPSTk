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

/**
 * @file EditDDs.cpp
 * Edit buffered double differences for outliers, cycle slips and isolated
 * points for program DDBase.
 */

//------------------------------------------------------------------------------------
// TD EditDDs.cpp make various numbers input parameters

//------------------------------------------------------------------------------------
// includes
// system
#include <vector>

// GPSTk
#include "Matrix.hpp"
#include "Stats.hpp"
#include "RobustStats.hpp"
//#include "SRIFilter.hpp"

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
static int ngood,nbad;           // number good data, number of data marked bad
static vector<int> mark;         // parallel to count and data vectors, mark bad data
static ofstream tddofs;          // output stream for OutputTDDFile

//------------------------------------------------------------------------------------
// prototypes -- this module only
int EditDDResets(const DDid& ddid, DDData& dddata) throw(Exception);
int EditDDIsolatedPoints(const DDid& ddid, DDData& dddata) throw(Exception);
int EditDDSlips(const DDid& ddid, DDData& dddata, int frequency) throw(Exception);
int EditDDOutliers(const DDid& ddid, DDData& dddata, int frequency) throw(Exception);
//void LSPolyFunc(Vector<double>& X, Vector<double>& f, Matrix<double>& P)
//   throw(Exception);
// prototypes -- DataOutput.cpp
int OutputRawDData(const DDid& ddid, const DDData& dddata, const vector<int>& mark)
   throw(Exception);
int OutputDDData(void) throw(Exception);

//------------------------------------------------------------------------------------
int EditDDs(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN EditDDs()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;

   if(!CI.OutputTDDFile.empty()) {
      tddofs.open(CI.OutputTDDFile.c_str(),ios::out);
      if(tddofs) {
         oflog << "Opened file " << CI.OutputTDDFile
               << " for triple difference and cycle slip output." << endl;
         tddofs << "# " << Title << endl;
         tddofs << "TDS  site site  sat sat freq iter cnt week sow       "
                << "dcnt     TD(m)       slip(cy) frac\n";
         tddofs << "SED  site site  sat sat freq iter cnt week sow           "
                << "DDres(m)    TDres(m)\n";
      }
      else {
         oflog << "Warning - Failed to open file " << CI.OutputTDDFile << endl;
      }
   }
   
   if(CI.Verbose) {
      oflog << " TUR site site sat sat iter  N Average  StdDev   SigYX"
         << "   Median   M-est     MAD\n";
      oflog << " SUR site site sat sat iter  N Average  StdDev   SigYX"
         << "   Median   M-est     MAD\n";
   }

   int i,j,k;
   map<DDid,DDData>::iterator it;

      // -------------------------------------------------------------------
      // delete DD buffers that are too small, or that user wants to exclude
      // also compute maxCount, the largest value of Count seen in all baselines
   maxCount = 0;
   vector<DDid> DDdelete;
   for(it = DDDataMap.begin(); it != DDDataMap.end(); it++) {

         // is it too small?
      if(it->second.count.size() < CI.MinDDSeg) {
         DDdelete.push_back(it->first);
         continue;
      }

         // prepare 'mark' vector
      mark.assign(it->second.count.size(),1);
      ngood = mark.size();
      nbad = 0;

         // remove points where bias had to be reset multiple times
      k = EditDDResets(it->first, it->second);
      if(k || ngood < CI.MinDDSeg) {
         DDdelete.push_back(it->first);
         continue;
      }

         // remove isolated points
      k = EditDDIsolatedPoints(it->first, it->second);
      if(k || ngood < CI.MinDDSeg) {
         DDdelete.push_back(it->first);
         continue;
      }

         // find and remove slips
      if(CI.Frequency != 2) {                // L1
         k = EditDDSlips(it->first, it->second,1);
         if(k || ngood < CI.MinDDSeg) {
            DDdelete.push_back(it->first);
            continue;
         }
      }
      if(CI.Frequency != 1) {                // L2
         k = EditDDSlips(it->first, it->second,2);
         if(k || ngood < CI.MinDDSeg) {
            DDdelete.push_back(it->first);
            continue;
         }
      }

         // find and remove outliers
      if(CI.Frequency != 2) {                // L1
         k = EditDDOutliers(it->first, it->second,1);
         if(k || ngood < CI.MinDDSeg) {
            DDdelete.push_back(it->first);
            continue;
         }
      }
      if(CI.Frequency != 1) {                // L2
         k = EditDDOutliers(it->first, it->second,2);
         if(k || ngood < CI.MinDDSeg) {
            DDdelete.push_back(it->first);
            continue;
         }
      }

         // output raw data with mark
      OutputRawDData(it->first, it->second, mark);

         // use vector 'mark' to delete data
      if(nbad > 0) {
         vector<double> nDDL1,nDDL2,nDDP1,nDDP2,nDDER;
         vector<int> ncount;
         for(i=0; i<it->second.count.size(); i++) {
            if(mark[i] == 1) {
               nDDL1.push_back(it->second.DDL1[i]);
               nDDL2.push_back(it->second.DDL2[i]);
               nDDP1.push_back(it->second.DDP1[i]);
               nDDP2.push_back(it->second.DDP2[i]);
               nDDER.push_back(it->second.DDER[i]);
               ncount.push_back(it->second.count[i]);
            }
         }
         it->second.DDL1 = nDDL1;
         it->second.DDL2 = nDDL2;
         it->second.DDP1 = nDDP1;
         it->second.DDP2 = nDDP2;
         it->second.DDER = nDDER;
         it->second.count = ncount;
         // ignore resets from now on...
      }

         // find the max count
      if(it->second.count[it->second.count.size()-1] > maxCount)
         maxCount = it->second.count[it->second.count.size()-1];
   }

      // close the output file
   tddofs.close();
   mark.clear();

      // now delete the ones that were marked
   for(i=0; i<DDdelete.size(); i++) {
      if(CI.Verbose) oflog << setw(2) << DDdelete[i]
         << " total = " << setw(5) << DDDataMap[DDdelete[i]].count.size()
         << ", count = " << setw(5) << DDDataMap[DDdelete[i]].count[0]
         << " - " << setw(5)
         << DDDataMap[DDdelete[i]].count[DDDataMap[DDdelete[i]].count.size()-1]
         << " -- Delete this DD dataset."
         << endl;
      DDDataMap.erase(DDdelete[i]);
   }
   DDdelete.clear();

      // -------------------------------------------------------------------
      // output DD summary
   {
      // use this loop to compute the number of DDs
      long nDDs;
      ostringstream oss;
      oss << "Double differences summary:" << endl;
      for(k=1,nDDs=0,it=DDDataMap.begin(); it!=DDDataMap.end(); it++,k++) {
            // output
         oss << " " << setw(2) << k << " " << it->first
            << " " << setw(5) << it->second.count.size()
            << " " << setw(5) << it->second.count[0]
            << " - " << setw(5) << it->second.count[it->second.count.size()-1];
         nDDs += it->second.count.size();
               // gaps - (count : number of pts)
         for(i=0; i<it->second.count.size()-1; i++) {
            j = it->second.count.at(i+1) - it->second.count.at(i);
            if(j > 1) oss << " (" << it->second.count.at(i)+1 << ":" << j-1 << ")";
         }
         oss << endl;
      }
         // output
      if(CI.Verbose) oflog << oss.str();
      if(CI.Screen) cout << oss.str();

         // check that there were some DDs
      if(k <= 1 || nDDs < 500) {              // what is the min number of DDs ?
         oflog << "Too few double differences (" << nDDs
            << ") were found.. Abort." << endl;
         cout << "Too few double differences (" << nDDs
            << ") were found.. Abort." << endl;
         return -3;
      }
   }

      // dump buffers to a file
   OutputDDData();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end EditDDs()

//------------------------------------------------------------------------------------
// There is no provision in DDBase for resetting a bias. This would imply
// solving for different biases (separated in time) for the same DDid.
// Therefore, this routine simply deletes all but the largest unbroken segment
// separated by resets.
int EditDDResets(const DDid& ddid, DDData& dddata) throw(Exception)
{
try {
   int i,j,ibeg,iend;

   // resets[0] will always be the initial count
   if(dddata.resets.size() <= 1) return 0;

   oflog << " Warning - DD " << ddid << " had " << dddata.resets.size()-1
      << " resets between " << dddata.count[1]
      << " and " << dddata.count[dddata.count.size()-1] << " :";
   for(i=1; i<dddata.resets.size(); i++)
      oflog << " " << dddata.count[dddata.resets[i]]
         << "[" << dddata.resets[i] << "]";
   oflog << endl;

   //for(i=1; i<dddata.resets.size(); i++) {
   //   // difference in index
   //   int di = dddata.resets[i] - dddata.resets[i-1];
   //   // difference in counts
   //   int dc = dddata.count[dddata.resets[i]] - dddata.count[dddata.resets[i-1]];
   //   j = dddata.resets[i];
   //   // mark it bad
   //   if(dc < 12 && mark[j]==1) {           // TD make 12 an input parameter
   //      mark[j] = 0;
   //      ngood--;
   //      nbad++;
   //   }
   //}

   // find the largest segment between resets
   // NB this assumes nothing yet 'marked'
   ibeg = 0;
   iend = dddata.resets[1];
   for(i=2; i<=dddata.resets.size(); i++) {
      if(i == dddata.resets.size()) // last point
         j = dddata.count.size();
      else
         j = dddata.resets[i];

      if(j - dddata.resets[i-1] > iend-ibeg) {
         ibeg = dddata.resets[i-1];
         iend = j;
      }
   }

   if(CI.Verbose) oflog << " Delete data due to reset for DD " << ddid
      << " in the range " << ibeg << " to " << iend << endl;

      // mark all points from beginning to just before the 'ibeg' reset
   for(i=0; i<ibeg; i++) if(mark[i]==1) {
      mark[i] = 0;
      ngood--;
      nbad++;
   }
   
      // mark all points from 'iend' reset to the end
   for(i=iend; i<dddata.count.size(); i++) if(mark[i]==1) {
      mark[i] = 0;
      ngood--;
      nbad++;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int EditDDIsolatedPoints(const DDid& ddid, DDData& dddata) throw(Exception)
{
try {
   //if(CI.Verbose) oflog << "BEGIN EditDDIsolatedPoints()"
   //   << " at total time " << fixed << setprecision(3)
   //   << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
   //   << endl;

   int i,j,gappast,gapfuture;

   // loop over all counts
   // i is current (good) point, j is the next good point
   i = 0; while(i<dddata.count.size() && mark[i]==0) i++;     // find first good pt

   gapfuture = CI.MaxGap;
   while(i < dddata.count.size()) {
      gappast = gapfuture;

      // find next good pt
      j = i+1;
      while(j < dddata.count.size() && mark[j]==0) j++;

      if(j < dddata.count.size()) gapfuture = dddata.count[j] - dddata.count[i];
      else                        gapfuture = CI.MaxGap;

      if(gappast >= CI.MaxGap && gapfuture >= CI.MaxGap) {
         if(CI.Verbose) oflog << " Mark isolated " << ddid
            << " " << dddata.count[i] << endl;
         mark[i] = 0;
         ngood--;
         nbad++;
      }

      i = j;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int EditDDSlips(const DDid& ddid, DDData& dddata, int frequency) throw(Exception)
{
try {
   int i,j,k,n,m,tdcount,tddt,ii,iter;
   double slip,fslip,tol;
   vector<int> slipindex;
   vector<double> td,slipsize;
   DayTime tt;
   TwoSampleStats<double> tsstats;

      // -------------------------------------- define td tolerance for slips
   const int iter_limit = 3;       // this allows iter_limit-1 slips to be fixed
   const double tolerance = 0.5;         // cycles
   // 3.4 const double tol = 0.5;        // cycles -- TD make input
   // 3.2 revert tol = 0.9;        // cycles
//#define turnoffslips 1

      // iterate
   for(iter=1; iter<=iter_limit; iter++) {
      if(iter == 1) tol = tolerance;
      if(iter >  1) tol = 0.6 * tolerance;
      td.clear();
      slipsize.clear();
      slipindex.clear();
      tsstats.Reset();

         // -------------------------------------- find slips
         // compute triple differences
         // j is the index of the previous good point
      for(k=0,j=-1,i=0; i<dddata.count.size(); i++) {
         if(mark[i] == 0) {
            //oflog << "Data 1 marked at count " << dddata.count[i] << endl;
            continue;
         }
         if(j == -1) { j = i; continue; }
         //tdcount = dddata.count[i];
         tt = FirstEpoch + CI.DataInterval * dddata.count[i];
         tddt = dddata.count[i]-dddata.count[j];
         td.push_back(
            (frequency == 1 ? dddata.DDL1[i] - dddata.DDL1[j]
                            : dddata.DDL2[i] - dddata.DDL2[j]  )
               - (dddata.DDER[i] - dddata.DDER[j])
         );
         tsstats.Add(dddata.count[i],td[k]);
            // slip in cycles
         slip = td[k]/(frequency == 1 ? wl1 : wl2);
            // fractional part of slip
         fslip = fabs(fmod(slip,1.0));
         if(fslip > 0.5) fslip = 1.0-fslip;
#ifndef turnoffslips
            // look for slips
            // if frac > 0.2, call it a slip anyway and hope it will be combined
         if(fabs(slip) > tol) {  // || fslip > 0.2) 
            oflog << " Warning - DD " << ddid << " L" << frequency << fixed
               << " slip " << setprecision(3) << setw(8) << slip << " cycles, at "
               << tt.printf(" %4F %10.3g = %Y/%02m/%02d %2H:%02M:%6.3f")
               << " = count " << dddata.count[i] << " on iteration " << iter
               << endl;

               // first see if it can be combined with previous slip
            n = slipindex.size();
            if(n>0 && dddata.count[i]-dddata.count[slipindex[n-1]] < CI.MaxGap)  {
                  // combine these slips
               slipsize[n-1] += slip;
                  // mark all points from old slip to pt before this as bad
               for(m=slipindex[n-1]; m<i; m++) {
                  mark[m] = 0;
                  ngood--;
                  nbad++;
               }
               slipindex[n-1] = i;
               oflog << " Warning - DD " << ddid << " L" << frequency << fixed
                     << " last two slips combined (iter " << iter << ")"
                     << endl;
            }
            else {
               slipindex.push_back(i);
               slipsize.push_back(slip);
            }
         }
#endif
         if(tddofs) {
            tddofs << "TDS " << ddid << " L" << frequency << fixed
               << " " << iter
               << " " << setw(4) << dddata.count[i]
               << " " << tt.printf("%4F %10.3g")
               << " " << setw(3) << tddt << setprecision(6)
               << " " << setw(11) << td[k]
               << " " << setw(11) << slip << setprecision(3)
               << " " << setw(8) << fslip
               << endl; }

         k++;
         j = i;
      } // end for loop over dddata to compute TDs

         // if too small, delete the whole pass
      if(td.size() < 10) return -1;

         // print stats to log
      if(CI.Verbose) {
         double median,mad,mest;
         vector<double> weights;
         weights.resize(td.size());
         mad = Robust::MedianAbsoluteDeviation(&td[0], td.size(), median);
         mest = Robust::MEstimate(&td[0], td.size(), median, mad, &weights[0]);

         oflog << " TUR " << ddid << " L" << frequency << fixed << setprecision(3)
            << " " << iter
            << " " << setw(5) << tsstats.N()
            << " " << setw(7) << tsstats.AverageY()
            << " " << setw(7) << tsstats.StdDevY()
            << " " << setw(7) << tsstats.SigmaYX()
            << "  " << setw(7) << median
            << " " << setw(7) << mest
            << " " << setw(7) << mad
            << endl;
      }

         // if no slips found, normal return
      if(slipindex.size() == 0) return 0;
         // if on last iteration, don't bother to fix...
      if(iter == iter_limit) break;
         // TD check for too many slips -> reject the whole pass

      //if(CI.Verbose) for(i=0; i<slipindex.size(); i++)
      //   oflog << "Slip " << " L" << frequency << setprecision(3) << slipsize[i]
      //      << " found at count " << dddata.count[slipindex[i]] << endl;

         // -------------------------------------- remove slips
         // add a dummy..
      slipindex.push_back(99999);

         // ii is slip count, k is current correction in cycles,
         // j is index of previous good point
      for(k=0,j=-1,ii=0,i=0; i<dddata.count.size(); i++) {
         if(mark[i] == 0) {
            //oflog << "Data 2 marked at " << dddata.count[i] << endl;
            continue;
         }
         tt = FirstEpoch + CI.DataInterval * dddata.count[i];
            // fix
         if(i == slipindex[ii]) {     // new slip on this count
            k += int(slipsize[ii] + (slipsize[ii]>0 ? 0.5 : -0.5));
            if(CI.Verbose) oflog << " Fix L" << frequency << " slip at count "
               << dddata.count[i]
               << " " << tt.printf("%4F %10.3g")
               << " total mag " << k << " iteration " << iter
               << endl;
            ii++;
         }
            // fix double differences using accumulated net slip
         if(k != 0) {
            if(frequency == 1) dddata.DDL1[i] -= k * wl1;
            else               dddata.DDL2[i] -= k * wl2;
         }
            // output the slip-edited DDs and TDs
         if(tddofs) {
            tddofs << "SED " << ddid << fixed
               << " L" << frequency
               << " " << iter
               << " " << setw(4) << dddata.count[i]
               << " " << tt.printf("%4F %10.3g")
               << " " << setw(11) << setprecision(6)           // DD in m
               << (frequency == 1 ?  dddata.DDL1[i] : dddata.DDL2[i])
                     - dddata.DDER[i]
               << " " << setw(11)
               << (j == -1 ? 0.0 :                             // TD in m
                     (frequency == 1 ?  dddata.DDL1[i] - dddata.DDL1[j] :
                                        dddata.DDL2[i] - dddata.DDL2[j])
                        - (dddata.DDER[i] - dddata.DDER[j])
                  )
               << endl;

            j = i;

         }  // end output

      }  // end for loop over data to fix slips

   } // end for loop over iterations

      // failed - return non-zero to delete the whole segment
   oflog << " Warning - Delete " << ddid << " L" << frequency
      << ": unable to fix slips" << endl;

   return -1;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// No - use sigma stripping
// Process using robust least squares fit to a polynomial
// TD consider using a straight line if size of the dataset is small.
// Form vector of data = phase residual = raw DD phase minus DD ephemeris range
// Compute a robust LS fit to a polynomial, compute statistics on residuals of fit.
// Use weights and residuals normalized by RMSROF to mark outliers.
// Data used to test this algorithm come from T202:
// ASWA CTRA G11 G14  T202B
// ASWA CTRA G16 G25  T202D
// ASWA CTRA G20 G25  T202D
int EditDDOutliers(const DDid& ddid, DDData& dddata, int frequency) throw(Exception)
{
try {
   int i,j,n,tol;
   int N,M;                            // number of parameters, number of data
   int len = int(dddata.count.size()); // length of the buffers
   double median,mad,mest;
   Vector<double> cnt;
   Vector<double> dat,residuals,weights;
   TwoSampleStats<double> tsstats;

   if(len < 10) return -1;

   int tolsigstrip = 10; // limit on ratio of ddph to MAD  10  1000
   double tolsigyx = 0.02;  // limit on conditional sigma    0.02 0.5

   for(int iter=1; iter<=2; iter++) {
      dat.resize(len);
      cnt.resize(len);

         // pull out the good data, count it and ...
      for(M=0,i=0; i<len; i++) {
         if(mark[i] == 0) continue;             // skip the bad points

         if(frequency == 1)
            dat[M] = dddata.DDL1[i] - dddata.DDER[i];
         else
            dat[M] = dddata.DDL2[i] - dddata.DDER[i];
            // pull out the corresponding counts
         cnt[M] = double(dddata.count[i]);
            // count the number of good points
         M++;
      }
      if(M != len) {
         dat.resize(M);
         cnt.resize(M);      // important -- see LSPolyFunc()
      }

         // fail if too little data
      if(dat.size() < 10) break;

         // ... compute stats on it
      tsstats.Reset();
      tsstats.Add(cnt,dat);
      mad = Robust::MedianAbsoluteDeviation(&dat[0], dat.size(), median);
      mest = Robust::MEstimate(&dat[0], dat.size(), median, mad, &weights[0]);

         // print stats to log
      if(CI.Verbose) {
         oflog << " SUR " << ddid << " L" << frequency << " " << iter
            << fixed << setprecision(3)
            << " " << setw(5) << tsstats.N()
            << " " << setw(7) << tsstats.AverageY()
            << " " << setw(7) << tsstats.StdDevY()
            << " " << setw(7) << tsstats.SigmaYX()
            << "  " << setw(7) << median
            << " " << setw(7) << mest
            << " " << setw(7) << mad
            << endl;
      }

         // only continue if the conditional sigma is high...
      if(tsstats.SigmaYX() <= tolsigyx) return 0; // success

      oflog << " Warning - high sigma (" << iter << ") for "
         << ddid << " L" << frequency << " : " << fixed
         << setprecision(3) << setw(7) << tsstats.SigmaYX() << endl;

         // if this is the second iteration, failure
      if(iter == 2) break;

         // sigma stripping ... robust fit to quadratic is too slow...
      for(n=j=0,i=0; i<len; i++) {
         if(mark[i] == 0) continue;              // skip the bad points

         //oflog << "HIS " << ddid
         //   << " L" << frequency << " " << setw(3) << i
         //   << " " << setw(3) << dddata.count[i]
         //   << fixed << setprecision(3)
         //   << " " << setw(8) << dat[j]
         //   << endl;

         if(fabs(dat[j]) > tolsigstrip*mad) {
            if(CI.Verbose) oflog << " Warning - mark outlier " << ddid
               << " L" << frequency << fixed << setprecision(3)
               << " count " << dddata.count[i]
               << " ddph " << dat[j]
               << " res/sig " << fabs(dat[j])/(tolsigstrip*mad)
               << endl;
            mark[i] = 0;
            ngood--;
            nbad++;
            n++;
         }
         j++;
      }

   }  // end iteration loop

      // failed - return non-zero to delete the whole segment
   oflog << " Warning - Delete " << ddid << " L" << frequency
      << " : unable to sigma strip" << endl;

   return -1;

/* this is too slow...
      // leave LSdata unchanged; SRIFilter will take data input, output weights.
   weights = LSdata;

      // compute a robust least squares fit to a polynomial
   N = 3;    // degree of polynomial .. TD consider 2 if dataset is small
   Vector<double> sol(N);
   Matrix<double> cov(N,N);
   SRIFilter robfit(N);
   //robfit.doVerbose = true;   // temp
   robfit.doRobust = true;
   robfit.doLinearize = false;
   robfit.doWeight = false;
   robfit.doSequential = false;
   robfit.iterationsLimit = 10;
   robfit.convergenceLimit = 1.e-2;
   sol = 0.0;

      // robust LS will return weights in data Vector = weights
   i = robfit.leastSquaresEstimation(weights,sol,cov,&LSPolyFunc);
   if(i) {
      oflog << " Warning - outlier check: robust fit for " << ddid
         << " returned " << i << endl;
      if(i==-1) return i;     // underdetermined
      if(i==-2) return i;     // singular
      if(i==-3) return i;     // failed to converge
      return i;   // ??
   }
   //if(!robfit.isValid())      // probably did not converge

      // compute post-fit residual weighted statistics
   Vector<double> f(M);
   Matrix<double> partials(M,N);
   LSPolyFunc(sol,f,partials);     // so f = solution evaluated at each point

   residuals = LSdata - f;         // residuals of fit

   stats.Reset();
   stats.Add(residuals,weights);   // compute weighted stats

      // Loop over counts (epochs)
   for(j=0,i=0; i<len; i++) {

      if(mark[i] == 0) continue;              // skip the bad points

      double resnorm = fabs(residuals[j]/stats.StdDev());

      if(CI.Verbose) oflog << "FIT " << ddid    // TD debug?
         << " " << setw(3) << i
         << " " << setw(3) << dddata.count[i]
         << fixed << setprecision(3)
         << " " << setw(5) << weights[j]
         << " " << setw(8) << LSdata[j]
         << " " << setw(8) << f[j]
         << " " << setw(8) << residuals[j]
         << " " << setw(8) << resnorm
         << endl;

      if(weights[j] <= 0.25 && resnorm > 4.0) {
         if(CI.Verbose) oflog << " Warning - mark outlier " << ddid
            << fixed << setprecision(3)
            << " count " << dddata.count[i]
            << " weight " << weights[j]
            << " res/sig " << resnorm
            << endl;
         mark[i] = 0;
         ngood--;
         nbad++;
      }

      j++;

   }  // end loop over counts
*/

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

// compute the partials matrix P and the solution at each data point (Vector f),
// given the solution Vector X. Called by SRIFilter::leastSquaresEstimation()
//void LSPolyFunc(Vector<double>& X, Vector<double>& f, Matrix<double>& P)
//   throw(Exception)
//{
//   try {
//      for(int i=0; i<LScount.size(); i++) {
//         double t = LScount[i] - LScount[0];
//         P(i,0) = 1.0;
//         for(int j=1; j<X.size(); j++) P(i,j) = P(i,j-1) * t;
//      }
//      f = P * X;
//   }
//   catch(Exception& e) { GPSTK_RETHROW(e); }
//}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
