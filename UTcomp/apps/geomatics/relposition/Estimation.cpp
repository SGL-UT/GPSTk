#pragma ident "$Id: $"

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
 * @file Estimation.cpp
 * Solve estimation problem using linearized least squares, part of program DDBase.
 */

//------------------------------------------------------------------------------------
// TD Estimation.cpp SRIF convergence parameters -> input parameters
// TD Estimation.cpp For L3 : average DD WL range-phase to solve for widelane bias,
// TD Estimation.cpp  use this to solve for biases, then allow fixing of biases.
// TD Estimation.cpp Need to account for signs in single diff
// TD Estimation.cpp Singular problems in Solve

//------------------------------------------------------------------------------------
// system includes

// GPSTk
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Namelist.hpp"
#include "SRIFilter.hpp"
#include "PreciseRange.hpp"
#include "Stats.hpp"
#include "RobustStats.hpp"
#include "geometry.hpp"

// DDBase
#include "DDBase.hpp"
#include "index.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- this module only
   // called by ConfigureEstimation(), which is Configure(3)
void DefineStateVector(void);
void DefineLSProblem(void);
string ComposeName(const string& site1, const string& site2,
                   const GSatID& sat1, const GSatID& sat2);
string ComposeName(const DDid& ddid);
void DecomposeName(const string& label, string& site1, string& site2,
                    GSatID& sat1, GSatID& sat2);
   // called by Estimation() -- inside the loop
int EditDDdata(int n);
int ModifyState(int n);
int InitializeEstimator(void);
int aPrioriConstraints(void);
int FillDataVector(int count);
void StochasticModel(int count, Namelist& DNL, Matrix<double>& MCov);
void EvaluateLSEquation(Vector<double>& X,Vector<double>& f,Matrix<double>& P);
int MeasurementUpdate(Matrix<double>& P, Vector<double>& f, Matrix<double>& MC);
int Solve(void);
int UpdateNominalState(void);
void OutputIterationResults(bool final);
int IterationControl(int iter_n);
void OutputFinalResults(int iret);
double RMSResidualOfFit(int N, Vector<double>& dX, bool final=false);

//------------------------------------------------------------------------------------
// local data
static int N,M;                    // lengths of state and data
static Namelist StateNL;           // state vector namelist
static Vector<double> State;       // state vector
static Vector<double> dX;          // update to state vector
static Matrix<double> Cov;         // covariance matrix
static Namelist DataNL;            // data vector namelist
static Vector<double> Data;        // data vector
static Matrix<double> MeasCov;     // measurement covariance matrix
static Matrix<double> Partials;    // partials matrix
static bool Biasfix;               // if true, fix estimated biases and solve for
                                   // position states only -- NB used widely!
static SRIFilter srif;             // square root information filter for least squares
static double small,big;           // condition number in inversion = big/small
static int NEp,nDD;                // counters used in LS problem
static int Mmax;                   // largest M (data size) encountered
static int NState;                 // true length of the state vector
static Vector<double> BiasState;   // save the solution for biases, before bias fixing
static Matrix<double> BiasCov;     // save covariance for biases, before bias fixing
static Vector<double> NominalState;// save the nominal state to output with solution

//------------------------------------------------------------------------------------
// currently the estimation problem is designed like this:
// start with state of length np
// loop over data epochs
//    fill data vector for this epoch, length nd
//    fill measurement covariance matrix, nd x nd
//    compute partials and nominal data vector, P(nd x np), f(nd)
//    update srif with P(nd x np), data - f (nd), mc(nd x nd)
// end loop over data epochs
// invert to get solution
//
// inupt batch size : number of epochs / batch (nepb)
// 
// start with state of length np, nepb
// loop over data epochs
//    fill a data vector for this epoch, length nd
//    fill a measurement covariance matrix for this epoch, nd x nd
//    compute a partials and a nominal data vector, P(nd x np), f(nd)
//    add to current totals:  PP = PP && P   ff = ff && f
//     PP =  (P1)  ff = (f1)  MC = (mc1)  0    0
//           (P2)       (f2)         0  (mc2)  0
//           (P3)       (f3)         0    0  (mc3)
//    (PP grows only in rows, MC grows in rows and columns)
//    also fill in correlation (off-diagonal) parts of MC
//    if(its the end || nepb has been reached)
//        update srif with PP, Data-ff and MC
//        if(its the end) break
//        optional - invert to get solution
//        clear out PP, ff, MC
//    endif
// end loop over data epochs
// invert to get solution
int Estimation(void)
{
try {
   if(CI.Verbose) oflog << "BEGIN Estimation()" << endl;
   if(CI.noEstimate) {
      oflog << "Option --noEstimate was chosen .. terminate.\n";
      return 0;
   }
   if(CI.Screen) cout << "BEGIN Estimation..." << endl;

   bool final=false;
   int iret,n,curr;
   Vector<double> NomData,RHS;

      // iterative loop for linearized least squares estimation
   for(n=0; ; n++) {

      if(CI.Verbose) oflog << "BEGIN LLS Iteration #" << n+1
         << "------------------------------------------------------------------\n";
      if(CI.Screen) cout << "BEGIN LLS Iteration #" << n+1
         << "------------------------------------------------------------------\n";

         // edit DD data
      if((iret = EditDDdata(n))) break;

         // modify state - e.g. fix biases:
         // if user has chosen to fix biases, Biasfix is set true on last iteration
      if((iret = ModifyState(n))) break;

         //
      if((iret = InitializeEstimator())) break;

         //
      if((iret = aPrioriConstraints())) break;

         // ------------------ loop over epochs in the DD buffers
         // build the data vector and Namelist
         // build the measurement covariance matrix
         // update the SRI filter
      curr = -1;           // current value of count
      NEp = nDD = 0;
      while(1) {
         curr++;
         if(curr > maxCount) break;

            // this needed by EvaluateLSEquation, and is used in output
         SolutionEpoch = FirstEpoch + curr*CI.DataInterval;

            // get the data and the data namelist
         M = FillDataVector(curr);
            // no data -- but don't assume this implies the last epoch
         if(M == 0) continue;
         nDD += M;

            // compute the measurement covariance matrix
         StochasticModel(curr,DataNL,MeasCov);

            // get nominal data = NomData(nominal state) and partials
            // NB position components of state not used in here..
         EvaluateLSEquation(State,NomData,Partials);

         if(CI.Debug)
            oflog << "EvaluateLSEquation returns vector\n" << fixed
            << setw(8) << setprecision(3) << NomData
            << "\n diff with data " << setw(8) << setprecision(3) << (Data-NomData)
            << "\n partials matrix\n" << setw(8) << setprecision(3) << Partials
            << "\n State\n" << setw(8) << setprecision(3) << State << endl;

         RHS = Data - NomData;              // RHS of linearized LS equation

            // update the SRI filter
         if((iret = MeasurementUpdate(Partials,RHS,MeasCov))) break;

         NEp++;

      }  // end while loop over data epochs
      if(iret) break;

      if((iret = Solve())) break;

      if((iret = UpdateNominalState())) break;

      // return -1  quit now
      //         0   go on
      //         1   reached convergence and don't fix biases
      //         2   reached last iteration and don't fix biases
      //         4   1 and/or 2 and fix biases, i.e. fix the biases then quit
      iret = IterationControl(n+1);

      oflog << endl;

      //if(iret == -1) {        // biases have been fixed
      //   iret = 0;
      //   break;
      //}
      //else if(iret == 4)      // one more, with biases fixed
      //   final = true;
      //else if(iret) {           // quit now
      //   iret = 0;
      //   break;
      //}
      if(iret && iret != 4) final = true;

      OutputIterationResults(final);

      if(iret && iret != 4) {
         iret = 0;
         break;
      }

   }  // end iterative loop

   // iret is -2 (singular) or 0

   OutputFinalResults(iret);

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end Estimation()

//------------------------------------------------------------------------------------
// called by Configure(3)
int ConfigureEstimation(void)
{
try {
   if(CI.Verbose) oflog << "BEGIN ConfigureEstimation()" << endl;

      // find the mean time, get Earth orientation parameters
   MedianEpoch = FirstEpoch;
   MedianEpoch += (LastEpoch-FirstEpoch)/2.;
   eorient = EOPList.getEOP(MedianEpoch);
   if(CI.Verbose) {
      oflog << "Earth orientation parameters at median time " << MedianEpoch << " :"
            << endl << "  xp, yp, UT1mUTC*Wearth (all radians) =" << fixed
            << " " << setprecision(9) << eorient.xp*DEG_TO_RAD/3600.0
            << ", " << setprecision(9) << eorient.yp*DEG_TO_RAD/3600.0
            << ", " << setprecision(9) << eorient.UT1mUTC * 7.2921151467e-5 << endl;
   }

      // define the initial State vector
   DefineStateVector();

      // Configure the SRIF for the estimation
   DefineLSProblem();

      // initial value
   Biasfix = false;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by ConfigureEstimation()
void DefineStateVector(void)
{
try {
      // set up the names of the state vector
      // set up the initial value of the nominal state
      // State = offset from nominal position, stored in Stations[].pos
      // plus offset from nominal biases
      // NB biases MUST be last, after all other states. This b/c inside
      // LSIterationLoop(), dX will be State - bias states when Biasfix == true

   int i;

   // add position states for all the non-fixed stations
   // add residual zenith delay states (per site)
   map<string,Station>::const_iterator it;
   for(it=Stations.begin(); it != Stations.end(); it++) {
      if(!(it->second.fixed)) {
         StateNL += it->first + string("-X");
         StateNL += it->first + string("-Y");
         StateNL += it->first + string("-Z");
      }
      if(CI.NRZDintervals > 0) {
         for(i=0; i<CI.NRZDintervals; i++) {
            StateNL += it->first + string("-RZD") + StringUtils::asString(i);
         }
      }
   }

   // add bias states
   map<DDid,DDData>::iterator jt;
   for(jt=DDDataMap.begin(); jt != DDDataMap.end(); jt++) {
      // += adds it only if it is unique..
      StateNL += ComposeName(jt->first);
   }

   // temp - sanity check
   for(int i=0; i<StateNL.size(); i++) {
      string site1,site2;
      GSatID sat1,sat2;
      DecomposeName(StateNL.getName(i), site1, site2, sat1, sat2);
      oflog << "State name (" << setw(2) << i << ") decomposes as "
         << site1 << " " << site2 << " " << sat1 << " " << sat2;

         // interpret it
      oflog << " [ " << site1;
      if(site2 == string("X") || site2 == string("Y") || site2 == string("Z")) {
         oflog << " : " << site2 << "-component position";
      }
      else if(site2.substr(0,3) == "RZD") {
         oflog << " : trop delay #" << site2.substr(3,site2.size()-3);
      }
      else if(Stations.find(site2) != Stations.end() &&
              sat1.id != -1 &&
              sat2.id != -1) {
         oflog << " " << site2 << " " << sat1 << " " << sat2 << " : bias";
      }
      else
         oflog << " : unknown!";

      oflog << " ]" << endl;
   }

   // dimensions
   // state N, data M, NState=N but if biases are fixed N=non-bias states only
   // State and StateNL always has length NState
   // actual state may shrink to N when biases fixed,
   // but then LSIterationLoop() uses temporaries
   NState = StateNL.size();
   State = Vector<double>(NState,0.0);
   Mmax = DDDataMap.size();            // the largest M (Data.size()) could be

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by ConfigureEstimation()
void DefineLSProblem(void)
{
try {
      // define the least squares processor
   srif.iterationsLimit = CI.nIter;
   srif.convergenceLimit = CI.convergence;
   srif.divergenceLimit = 1.e10;    // TD input parameter
   srif.doWeight = false;
   srif.doRobust = false;
   srif.doLinearize = false;
   srif.doSequential = true;

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// called by Estimation() - inside the iteration loop
// currently, n is not used...
// currently does nothing but print stats on the residuals
int EditDDdata(int n)
{
try {
   int i,k;
   double res,median,mad,mest;
   map<DDid,DDData>::iterator it;

   oflog << "    Estimation data summary  "
         << "N   M-est    MAD     Ave     Std    SigYX  Slop_um SigSl_um" << endl;

      // loop over the data
   for(k=1, it = DDDataMap.begin(); it != DDDataMap.end(); it++) {
      vector<double> ddres,weights;
      TwoSampleStats<double> tsstats;

      for(i=0; i<it->second.count.size(); i++) {
         res = (CI.Frequency == 1 ? it->second.DDL1[i] - it->second.DDER[i] :
               (CI.Frequency == 2 ? it->second.DDL2[i] - it->second.DDER[i] :
                                    if1p*it->second.DDL1[i]+if2p*it->second.DDL2[i]
                                       - it->second.DDER[i]));
         tsstats.Add(it->second.count[i],res);
         ddres.push_back(res);
      }

      weights.resize(ddres.size());
      mad = Robust::MedianAbsoluteDeviation(&ddres[0], ddres.size(), median);
      mest = Robust::MEstimate(&ddres[0], ddres.size(), median, mad, &weights[0]);

         // output
      oflog << "EDS " << setw(2) << k << " " << it->first
         << " " << setw(5) << it->second.count.size()
         << fixed << setprecision(3)
         << " " << setw(7) << mest
         << " " << setw(7) << mad
         << " " << setw(7) << tsstats.AverageY()
         << " " << setw(7) << tsstats.StdDevY()
         << " " << setw(7) << tsstats.SigmaYX()
         << " " << setw(7) << tsstats.Slope()*1000000.0
         << " " << setw(7) << tsstats.SigmaSlope()*1000000.0
         << " " << setw(7) << tsstats.Slope()*1000.0*it->second.count.size()
         << endl;

      k++;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the iteration loop
int ModifyState(int niter)
{
try {
   int i,j,k;

      // set the State elements to zero for next iteration
   map<string,Station>::const_iterator it;
   for(it=Stations.begin(); it != Stations.end(); it++) {

      // ignore fixed sites
      if(it->second.fixed) continue;

      // find the position states
      i = StateNL.index(it->first+string("-X"));
      j = StateNL.index(it->first+string("-Y"));
      k = StateNL.index(it->first+string("-Z"));
      if(i == -1 || j == -1 || k == -1) {
         Exception e("Position states confused: unable to find for " + it->first);
         GPSTK_THROW(e);
      }

      State(i) = State(j) = State(k) = 0.0;
   }

      // ----------------- fix biases?
   if(Biasfix) {
      if(CI.Verbose) oflog << "Fix the biases:\n";
         // State must have the (fixed) biases still in it
      for(i=0; i<State.size(); i++) {
         string site1,site2;
         GSatID sat1,sat2;
         DecomposeName(StateNL.getName(i), site1, site2, sat1, sat2);
         if(site2 == string("X") || site2 == string("Y") || site2 == string("Z"))
            continue;
         if(site2 == string("rzd"))
            continue;
         if(Stations.find(site2) == Stations.end())
            continue;
         if(sat1.id == -1 || sat2.id == -2)
            continue;

         long bias=long(State[i]/wave + (State[i]/wave > 0 ? 0.5 : -0.5));
         if(CI.Verbose) oflog << "  fix " << StateNL.getName(i)
                              << " to " << bias << " cycles" << endl;
         State(i) = wave*double(bias);
      }
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the iteration loop
// actually fixes the biases
int InitializeEstimator(void)
{
try {
   int i;
   Namelist NL;

      // ----------------- initialize this iteration
      // determine length of state, reset the LS
      // Use N and NL rather than NState and StateNL
   N = NState;
   NL = StateNL;
   if(Biasfix) {
      // State will still be full length = NState
      // StateNL will also stay full length, but N and NL will not
      NL.clear();
      for(N=0,i=0; i<NState; i++) {
         string site1,site2;
         GSatID sat1,sat2;
         DecomposeName(StateNL.getName(i), site1, site2, sat1, sat2);
         if(Stations.find(site2) != Stations.end() &&
               sat1.id != -1 &&
               sat2.id != -1)
            break;     // quit when first bias state found
         else {                    // not a bias state
            NL += StateNL.getName(i);
            N++;
         }
      }
      oflog << "Fix biases on this iteration (new State dimension is "
         << N << ")" << endl;
      if(CI.Screen) cout << "Fix biases on this iteration (new State dimension is "
         << N << ")" << endl;
   }
   dX.resize(N);
   srif = SRIFilter(NL);

      // save the nominal state for output with Solution (OutputIterationResults)
   NominalState = State;

      // dump the nominal state (including biases, even if fixed)
   //if(CI.Verbose) {
   //   LabelledVector LabSt(StateNL,State);
   //   LabSt.setw(20).setprecision(6);
   //   oflog << "Nominal State :\n" << LabSt << endl;
   //}

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the iteration loop
int aPrioriConstraints(void)
{
try {
      // add initial constraints
   Matrix<double> apCov(N,N,0.0);
   Vector<double> apState(N,0.0);         // most states have apriori value = 0

   int i,j,k,n;
   double ss;
   Position BL;
   map<string,Station>::const_iterator it;

      // set apCov = unity
   //ident(apCov);

      // loop over baselines - to get the position constraints
   for(n=0; n<Baselines.size(); n++) {
      string one=StringUtils::word(Baselines[n],0,'-');
      string two=StringUtils::word(Baselines[n],1,'-');
      BL = Stations[one].pos - Stations[two].pos;

         // find the position states
      i = StateNL.index(two+string("-X"));
      j = StateNL.index(two+string("-Y"));
      k = StateNL.index(two+string("-Z"));
         // you may have a baseline where both sites are fixed.
      if(i == -1 || j == -1 || k == -1) continue;

      if(Biasfix) {     // 10ppm of baseline
         ss = CI.TightConstraint * fabs(BL.X());
         apCov(i,i) = (ss*ss);
         ss = CI.TightConstraint * fabs(BL.Y());
         apCov(j,j) = (ss*ss);
         ss = CI.TightConstraint * fabs(BL.Z());
         apCov(k,k) = (ss*ss);
         // 1 mm v2.6
         //ss = 1.e-3 ;
         //apCov(i,i) = (ss*ss);
         //ss = 1.e-3 ;
         //apCov(j,j) = (ss*ss);
         //ss = 1.e-3 ;
         //apCov(k,k) = (ss*ss);
      }
      else {            // loose on position
         ss = CI.LooseConstraint * fabs(BL.X());
         apCov(i,i) = (ss*ss);
         ss = CI.LooseConstraint * fabs(BL.Y());
         apCov(j,j) = (ss*ss);
         ss = CI.LooseConstraint * fabs(BL.Z());
         apCov(k,k) = (ss*ss);
      }

      if(CI.Verbose) {
         // assume i,j,k are in order:
         MatrixSlice<double> Rslice(apCov,i,i,3,3);
         Matrix<double> R(Rslice);
         Namelist NL;
         NL += StateNL.getName(i);
         NL += StateNL.getName(j);
         NL += StateNL.getName(k);
         LabelledMatrix Lapc(NL,R);
         Lapc.setw(20).setprecision(3).scientific();
         Lapc.message("a priori covariance");
         oflog << Lapc << endl;
      }
   }

      // constrain the residual trop delay
   if(CI.NRZDintervals > 0) {
         // RZD in different intervals correlated; first order Gauss-Markov
         // dt = time between intervals in hours; these unused if N==1
      double dt = (LastEpoch-FirstEpoch)/(3600.*CI.NRZDintervals);
      double exn,ex = exp(-dt/CI.RZDtimeconst);

         // do for each site
      for(it=Stations.begin(); it != Stations.end(); it++) {

            // find indexes in state vector of all RZD states for this site
         string stname;
         vector<int> indexes;
         for(n=0; n<CI.NRZDintervals; n++) {
            stname = it->first + string("-RZD") + StringUtils::asString(n);
            i = StateNL.index(stname);
            if(i == -1) {
               Exception e("RZD states confused: unable to find state " + stname);
               GPSTK_THROW(e);
            }
            if(CI.Debug) oflog << "RZD state " << stname << " = index " << i << endl;
            indexes.push_back(i);
         }

            // fill the matrix
         for(n=0; n<indexes.size(); n++) {
               // diagonal element
            i = indexes[n];
            apCov(i,i) = CI.RZDsigma * CI.RZDsigma;
               // off-diagonal elements: rows up and cols to the left
            exn = ex;
            for(k=n-1; k>=0; k--) {
               j = indexes[k];
               apCov(j,i) = apCov(i,j) = CI.RZDsigma * CI.RZDsigma * exn;
               exn *= ex;
            }
         }  // end loop over diagonal matrix elements for this site

            // dump
         if(CI.Verbose) {
            if(CI.NRZDintervals > 1) {
               // assume indexes are in order:
               MatrixSlice<double> Rslice(apCov,indexes[0],indexes[0],
                                          CI.NRZDintervals,CI.NRZDintervals);
               Matrix<double> R(Rslice);
               Namelist NL;
               for(n=0; n<indexes.size(); n++) NL += StateNL.getName(indexes[n]);
               LabelledMatrix Lapc(NL,R);
               Lapc.setw(20).setprecision(3).scientific();
               Lapc.message("a priori covariance");
               oflog << Lapc << endl;
            }
            else
               oflog << "a priori covariance for RZD at " << it->first
                  << ": " << setprecision(3) << scientific << CI.RZDsigma*CI.RZDsigma
                  << endl;
         }

      }  // end loop over sites

   }  // end if there are RZD states..

      // TD need to constrain biases ... what is reasonable?
   if(!Biasfix) {
      ss = 0.25 * wave;
      for(n=0; n<StateNL.size(); n++) {
         string site1,site2;
         GSatID sat1,sat2;
         DecomposeName(StateNL.getName(n), site1, site2, sat1, sat2);
         if(site2 == string("X") || site2 == string("Y") || site2 == string("Z")) {
            continue;   // oflog << " : " << site2 << "-component position";
         }
         else if(site2.substr(0,3) == "RZD") {
            continue;   // oflog << " : trop #" << site2.substr(3,site2.size()-3);
         }
         else if(Stations.find(site2) != Stations.end() &&
               sat1.id != -1 &&
               sat2.id != -1) {
            // bias oflog << " " << site2 << " " << sat1 << " " << sat2 << " : bias";
            apCov(n,n) = ss*ss;
         }
         else
            continue;   // oflog << " : unknown!";
      }
      oflog << "a priori covariance for biases : " << setprecision(3)
         << scientific << ss*ss << endl;
   }

      // dump the whole matrix
   //if(CI.Verbose) {
   //   LabelledMatrix Lapc(StateNL,apCov);
   //   Lapc.setw(20).setprecision(3).scientific();
   //   Lapc.message("a priori covariance");
   //   oflog << Lapc << endl;
   //}

      // add it to srif
   srif.addAPriori(apCov,apState);

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the data loop, inside the iteration loop
int FillDataVector(int count)
{
try {
   int i,j;
   string lab;

   Data = Vector<double>(Mmax,0.0);
   DataNL.clear();
      // loop over the data
   map<DDid,DDData>::iterator it;
   for(i=0,it = DDDataMap.begin(); it != DDDataMap.end(); it++) {
      j = index(it->second.count,count);
      if(j == -1) continue;
      if(CI.Frequency == 1) Data(i) = it->second.DDL1[j];
      if(CI.Frequency == 2) Data(i) = it->second.DDL2[j];
      if(CI.Frequency == 3)      // ionosphere-free phase
         Data(i) = if1p * it->second.DDL1[j] + if2p * it->second.DDL2[j];
      lab = ComposeName(it->first);
      DataNL += lab;
      i++;
   }

   if(i > 0) {
      Data.resize(i);
      if(CI.Debug) {
         LabelledVector LD(DataNL,Data);
         LD.setw(20).setprecision(6);
         oflog << "At count " << count
            << " found time " << SolutionEpoch.printf("%F %10.3g")
            << " and Data\n" << LD << endl;
      }
   }

   return i;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the data loop, inside the iteration loop
// Input is Namelist DNL, the double difference data Namelist (DataNL)
// Output is MCov, the measurement covariance matrix for this data (MeasCov).
// Let:
//  d = vector of one-way data (one site, one satellite)
// sd = vector of single difference data (two sites, one satellite)
// dd = vector of double difference data (two sites, two satellites)
// DD and SD are matricies with elements 0,1,-1 which transform d to sd to dd:
// sd = SD * d
// dd = DD * sd
// dd = DD * SD * d
// The covariance matrix will be MC = (DD*SD)*transpose(DD*SD)
//                                  = DD*SD*transpose(SD)*transpose(DD)
// If the one-way data has a measurement covariance, then fill the vector d with
// them; then MC = DD*SD* d * transpose(SD)*transpose(DD).
// Building DD and SD is just a matter of lists:
// loop through the dd namelist, keeping lists of:
// one-way data (site-satellite pairs) (d)
// single differences (site-site-satellite sets) (sd)
// and you have a list of double differences (DNL)
//
// TD MinElevation here should be a separate parameter, not necessarily the mask angle
void StochasticModel(int count, Namelist& DNL, Matrix<double>& MCov)
{
try {
   unsigned int m=DNL.size();
   if(m==0) return;

   int i,j,in,jn,kn;
   double eps,coselev,d0,sig0;
   string site1,site2;
   GSatID sat1,sat2;
   vector<double> d;    // weights of one-way data
   vector<OWid> ld;     // labels of d
   vector<SDid> sd;

   // eps non-zero avoids blowup at zenith
   //eps = 0.0000001;   seems to have no effect
   eps = 0.001;

   for(i=0; i<DNL.size(); i++) {
      // break the label into site1,site2,sat1,sat2
      DecomposeName(DNL.getName(i), site1, site2, sat1, sat2);
      if(index(ld,OWid(site1,sat1)) == -1) ld.push_back(OWid(site1,sat1));
      if(index(ld,OWid(site1,sat2)) == -1) ld.push_back(OWid(site1,sat2));
      if(index(ld,OWid(site2,sat1)) == -1) ld.push_back(OWid(site2,sat1));
      if(index(ld,OWid(site2,sat2)) == -1) ld.push_back(OWid(site2,sat2));
      if(index(sd,SDid(site1,site2,sat1)) == -1) sd.push_back(SDid(site1,site2,sat1));
      if(index(sd,SDid(site1,site2,sat2)) == -1) sd.push_back(SDid(site1,site2,sat2));
   }

      // fill d with the weights
      // d needs to have units meters and be realistic ~ sigma(phase)
      // =sig0(m) at min elev, smaller at higher elevation
   sig0 = 1.0e-3; // smaller than e-2, else little effect
   coselev = eps + cos(CI.MinElevation * DEG_TO_RAD);          // TD new input param
   d0 = sig0 / (coselev*coselev);   // cosine squared model
   //d0 = sig0 / coselev;            // cosine model
   d = vector<double>(ld.size());
   for(i=0; i<ld.size(); i++) {
      j = index(Stations[ld[i].site].RawDataBuffers[ld[i].sat].count,count);
      if(j == -1) {
         oflog << "Error -- count " << count << " not found in buffer for " << ld[i]
            << endl;
         d[i] = d0;
      }
      else {
         coselev = eps + cos(Stations[ld[i].site].RawDataBuffers[ld[i].sat].elev[j]
                           * DEG_TO_RAD);
         d[i] = d0 * coselev * coselev;   // cosine squared model
         //d[i] = d0 * coselev;            // cosine model
      }
   }

   // temp
   //format f113s(11,3,2);
   //oflog << "DDs are (" << DNL.size() << "):\n" << setw(20) << DNL << endl;
   //oflog << "SDs are: (" << sd.size() << ")" << fixed << endl;
   //for(i=0; i<sd.size(); i++) oflog << " / " << sd[i];
   //oflog << endl;
   //oflog << "OWs are: (" << ld.size() << ")" << endl;
   //for(i=0; i<ld.size(); i++) oflog << " / " << ld[i];
   //oflog << endl;
   //oflog << "OW wts are: (" << d.size() << ")" << endl;
   //for(i=0; i<d.size(); i++) oflog << " " << f113s << d[i];
   //oflog << endl;

   Matrix<double> SD(sd.size(),ld.size(),0.0);
   Matrix<double> DD(m,sd.size(),0.0);
   // TD need to account for signs here ... sd[.] may be site2,site1,sat1 ...
   for(in=0; in<DNL.size(); in++) {
      DecomposeName(DNL.getName(in), site1, site2, sat1, sat2);
      jn = index(sd,SDid(site1,site2,sat1));        // site1-site2, sat1
      DD(in,jn) = 1;
      kn = index(ld,OWid(site1,sat1));              // site1, sat1
      SD(jn,kn) = d[kn];
      kn = index(ld,OWid(site2,sat1));              // site2, sat1
      SD(jn,kn) = -d[kn];

      jn = index(sd,SDid(site1,site2,sat2));        // site1-site2, sat2
      DD(in,jn) = -1;
      kn = index(ld,OWid(site1,sat2));              // site1, sat2
      SD(jn,kn) = d[kn];
      kn = index(ld,OWid(site2,sat2));              // site2, sat2
      SD(jn,kn) = -d[kn];
   }

   //oflog << " SD is\n" << fixed << setw(3) << SD << endl;
   //oflog << " DD is\n" << fixed << setw(3) << DD << endl;

   Matrix<double> T;
   T = DD * SD;
   MCov = T * transpose(T);

   if(CI.Debug) oflog << "Measurement covariance is\n" << scientific
      << setw(8) << setprecision(3) << MeasCov << endl;

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the data loop, inside the iteration loop
// Given a nominal state vector X, compute the function f(X) and the partials matrix
// P at X.
// NB X is not used here ... except that State is used for biases
void EvaluateLSEquation(Vector<double>& X, Vector<double>& f, Matrix<double>& P)
{
try {
   int i,j,k,n,m,ntrop;
   double ER,trop,mapf;
   string site1,site2;
   GSatID sat1,sat2;
   PreciseRange CER;
   //CorrectedEphemerisRange CER;

      //
      // assume Station.pos has been defined outside this routine... in UpdateNom.St.
      //

      // find the trop estimation interval for this epoch
      // trop is a temporary here..
   if(CI.NRZDintervals > 0) {
      ntrop = int( (SolutionEpoch-FirstEpoch) /
                    (((LastEpoch-FirstEpoch)+CI.DataInterval)/CI.NRZDintervals) );
   }

      // loop over the data vector, computing f(X) and filling P
   f = Vector<double>(M,0.0);
   P = Matrix<double>(M,N,0.0);
   for(m=0; m<DataNL.size(); m++) {

         // break name into its parts
      DecomposeName(DataNL.getName(m), site1, site2, sat1, sat2);

      Station& st1=Stations[site1];
      Station& st2=Stations[site2];

         // -----------------------------------------------------------
         // site1 ----------------------------------------------------
      if(!st1.fixed) {
         i = StateNL.index(site1 + string("-X"));
         j = StateNL.index(site1 + string("-Y"));
         k = StateNL.index(site1 + string("-Z"));
         if(i == -1 || j == -1 || k == -1) {
            Exception e("Position states confused: unable to find for " + site1);
            GPSTK_THROW(e);
         }
      }
         // sat 1  CER.SVR is satellite Position
         // pos 1  st1.pos is station Position

         // position states
         // should you use CER.rawrange here?
      ER = CER.ComputeAtReceiveTime(SolutionEpoch,st1.pos,sat1.id,*pEph,eorient);
      trop = st1.pTropModel->correction(st1.pos,CER.SVR,SolutionEpoch);
      f(m) += ER+trop;
      if(!st1.fixed) {
         P(m,i) += CER.cosines[0];
         P(m,j) += CER.cosines[1];
         P(m,k) += CER.cosines[2];
      }

         // trop rzd .. depends on site, sat and trop model
      if(CI.NRZDintervals > 0) {
         n = StateNL.index(site1 + string("-RZD") + StringUtils::asString(ntrop));
         if(n == -1) {
            Exception e("RZD states confused: unable to find state " + 
               site1 + string("-RZD") + StringUtils::asString(ntrop));
            GPSTK_THROW(e);
         }
         mapf = st1.pTropModel->wet_mapping_function(CER.elevation);
         P(m,n) += mapf;
         f(m) += mapf * State(n);
      }

         // sat 2 -----------------------------------------------------
      ER = CER.ComputeAtReceiveTime(SolutionEpoch,st1.pos,sat2.id,*pEph,eorient);
      trop = st1.pTropModel->correction(st1.pos,CER.SVR,SolutionEpoch);
      f(m) -= ER+trop;
      if(!st1.fixed) {
         P(m,i) -= CER.cosines[0];
         P(m,j) -= CER.cosines[1];
         P(m,k) -= CER.cosines[2];
      }

         // trop rzd .. depends on site, sat and trop model
      if(CI.NRZDintervals > 0) {
         mapf = st1.pTropModel->wet_mapping_function(CER.elevation);
         P(m,n) += mapf;
         f(m) += mapf * State(n);
      }

         // -----------------------------------------------------------
         // site 2 ----------------------------------------------------
      if(!st2.fixed) {
         i = StateNL.index(site2 + string("-X"));
         j = StateNL.index(site2 + string("-Y"));
         k = StateNL.index(site2 + string("-Z"));
         if(i == -1 || j == -1 || k == -1) {
            Exception e("Position states confused: unable to find for " + site2);
            GPSTK_THROW(e);
         }
      }
         // sat 1 -----------------------------------------------------
      ER = CER.ComputeAtReceiveTime(SolutionEpoch,st2.pos,sat1.id,*pEph,eorient);
      trop = st2.pTropModel->correction(st2.pos,CER.SVR,SolutionEpoch);
      f(m) -= ER+trop;
      if(!st2.fixed) {
         P(m,i) -= CER.cosines[0];
         P(m,j) -= CER.cosines[1];
         P(m,k) -= CER.cosines[2];
      }

         // trop rzd .. depends on site, sat and trop model
      if(CI.NRZDintervals > 0) {
         n = StateNL.index(site2 + string("-RZD") + StringUtils::asString(ntrop));
         if(n == -1) {
            Exception e("RZD states confused: unable to find state " + 
               site2 + string("-RZD") + StringUtils::asString(ntrop));
            GPSTK_THROW(e);
         }
         mapf = st2.pTropModel->wet_mapping_function(CER.elevation);
         P(m,n) += mapf;
         f(m) += mapf * State(n);
      }

         // sat 2 -----------------------------------------------------
      ER = CER.ComputeAtReceiveTime(SolutionEpoch,st2.pos,sat2.id,*pEph,eorient);
      trop = st2.pTropModel->correction(st2.pos,CER.SVR,SolutionEpoch);
      f(m) += ER+trop;
      if(!st2.fixed) {
         P(m,i) += CER.cosines[0];
         P(m,j) += CER.cosines[1];
         P(m,k) += CER.cosines[2];
      }

         // trop rzd .. depends on site, sat and trop model
      if(CI.NRZDintervals > 0) {
         mapf = st2.pTropModel->wet_mapping_function(CER.elevation);
         P(m,n) += mapf;
         f(m) += mapf * State(n);
      }

         // -----------------------------------------------------------
         // bias ------------------------------------------------------
      j = 1;
      i = StateNL.index(DataNL.getName(m));
      if(i == -1) {
         // but what if the bias is A-B_s-r and the data B-A_r-s?
         // (Decompose was called at top of loop)
         j = -1;
         i = StateNL.index(ComposeName(site1,site2,sat2,sat1));      // most likely
         if(i == -1) {
            i = StateNL.index(ComposeName(site2,site1,sat1,sat2));
            if(i == -1) {
               j = 1;
               i = StateNL.index(ComposeName(site2,site1,sat2,sat1));
            }
         }
      }
      f(m) += j * State(i);
      if(!Biasfix)
         P(m,i) = j;

   }  // end loop over data

   f.resize(M);
   P.resize(M,N);

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the data loop, inside the iteration loop
int MeasurementUpdate(Matrix<double>& P, Vector<double>& f, Matrix<double>& MC)
{
try {

   srif.measurementUpdate(P,f,MC);

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the iteration loop
int Solve(void)
{
try {

   try {
      srif.getStateAndCovariance(dX,Cov,&small,&big);
   }
   catch(SingularMatrixException& sme) {
      oflog << "Problem is singular " << endl;
      return -2;                 // TD handle singular problems in Solve()
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the iteration loop
int UpdateNominalState(void)
{
try {
   int n,i,j,k;

   if(Biasfix) {
      // NB when Biasfix, State has dimension NState buf dX has dimension N>NState
      // EvaluateLSFunction uses State bias elements even when Biasfix
      for(i=0; i<N; i++) {
         State[i] += dX[i];
      }
   }
   else {                  // regular update, save for when Biasfix is set
      State += dX;
      BiasState = State;
      BiasCov = Cov;
   }
      // redefine the nominal position
      // set all floating position states to zero
   map<string,Station>::const_iterator it;
   for(it=Stations.begin(); it != Stations.end(); it++) {
      if(it->second.fixed)       // ignore fixed sites
         continue;

      // find the position states
      i = StateNL.index(it->first+string("-X"));
      j = StateNL.index(it->first+string("-Y"));
      k = StateNL.index(it->first+string("-Z"));
      if(i == -1 || j == -1 || k == -1) {
         Exception e("Position states confused: unable to find for " + it->first);
         GPSTK_THROW(e);
      }

      // update the nominal position in Stations[]
      Stations[it->first].pos.setECEF(
         Stations[it->first].pos.X()+dX(i),
         Stations[it->first].pos.Y()+dX(j),
         Stations[it->first].pos.Z()+dX(k));
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the iteration loop
void OutputIterationResults(bool final)
{
try {
   int i,N=dX.size();
   format f166(16,6),f206(20,6),f82s(8,2,2);

   oflog << "         State label"
         << "    Nominal State"
         << "     State Update"
         << "     New Solution"
         << "            Sigma"
         << endl;
   for(i=0; i<N; i++) {
      oflog << setw(20) << StateNL.getName(i)
            << " " << f166 << NominalState[i]
            << " " << f166 << dX[i]
            << " " << f166 << State[i]
            << " " << f166 << SQRT(Cov(i,i))
            << endl;
   }

   //LabelledVector LSol(StateNL,dX);
   //LSol.setw(20).setprecision(6);
   //oflog << "Solution\n" << LSol << endl;

   ////LabelledMatrix LCov(StateNL,Cov);
   //Vector<double> Sig(N);
   //for(i=0; i<N; i++) Sig(i)=SQRT(Cov(i,i));
   //LabelledVector LCov(StateNL,Sig);
   //LCov.setw(20).setprecision(6);
   ////oflog << "Covariance\n" << LCov << endl;
   //oflog << "Sigma\n" << LCov << endl;

      // output baselines
   Position BL;
   for(i=0; i<CI.OutputBaselines.size(); i++) {
         // dependent on the order given in ComputeSingleDifferences()
      string one=StringUtils::word(CI.OutputBaselines[i],0,'-');
      string two=StringUtils::word(CI.OutputBaselines[i],1,'-');
      BL = Stations[one].pos - Stations[two].pos;
      oflog << "Baseline " << CI.OutputBaselines[i]
         << " " << BL.printf("%16.6x %16.6y %16.6z")
         << " " << f166 << BL.getRadius() << endl;
      if(CI.Screen) cout << "Baseline " << CI.OutputBaselines[i]
         << " " << BL.printf("%16.6x %16.6y %16.6z")
         << " " << f166 << BL.getRadius() << endl;
         // offset - if one was input
      if(CI.OutputBaselineOffsets[i].mag() >= 0.01) {
         oflog << " Offset  " << CI.OutputBaselines[i]
            << " " << f166 << BL.X() - CI.OutputBaselineOffsets[i][0]
            << " " << f166 << BL.Y() - CI.OutputBaselineOffsets[i][1]
            << " " << f166 << BL.Z() - CI.OutputBaselineOffsets[i][2]
            << " " << f166 << BL.getRadius() - CI.OutputBaselineOffsets[i].mag()
            << endl;
         if(CI.Screen) cout << " Offset  " << CI.OutputBaselines[i]
            << " " << f166 << BL.X() - CI.OutputBaselineOffsets[i][0]
            << " " << f166 << BL.Y() - CI.OutputBaselineOffsets[i][1]
            << " " << f166 << BL.Z() - CI.OutputBaselineOffsets[i][2]
            << " " << f166 << BL.getRadius() - CI.OutputBaselineOffsets[i].mag()
            << endl;
      }
   }

      // compute residuals of fit and output
   double rmsrof = RMSResidualOfFit(N,dX,final);
   oflog << "RES " << (final ? "final " : "" ) << "total RMS = "
      << f82s << rmsrof << endl;

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation()
// return -1  quit now
//         0   go on
//         1   reached convergence and don't fix biases
//         2   reached last iteration and don't fix biases
//         4   1 and/or 2 and fix biases, i.e. fix the biases then quit
int IterationControl(int iter_n)
{
try {
   int done=0;
   double converge;

   // has it converged?
   converge = norm(dX);
   if(converge <= CI.convergence) {
      oflog << "DDBase finds convergence: " << iter_n << " iterations"
            << ", convergence criterion = " << scientific << setprecision(3)
            << converge << " m; (" << CI.convergence << " m)" << endl;
      if(CI.Screen)
         cout << "DDBase finds convergence: " << iter_n << " iterations"
            << ", convergence criterion = " << scientific << setprecision(3)
            << converge << " m; (" << CI.convergence << " m)" << endl;
      done += 1;
   }

   // have we reached the last iteration?
   if(iter_n == CI.nIter) {
      oflog << "DDBase finds last iteration: " << iter_n << " iterations"
            << ", convergence criterion = " << scientific << setprecision(3)
            << converge << " m; (" << CI.convergence << " m)" << endl;
      if(CI.Screen)
         cout << "DDBase finds last iteration: " << iter_n << " iterations"
            << ", convergence criterion = " << scientific << setprecision(3)
            << converge << " m; (" << CI.convergence << " m)" << endl;
      done += 2;
   }
   
   if(!done && CI.Verbose) {
      oflog << "DDBase: " << iter_n << " iterations"
            << ", convergence criterion = " << scientific << setprecision(3)
            << converge << " m; (" << CI.convergence << " m)" << endl;
      if(CI.Screen)
         cout << "DDBase: " << iter_n << " iterations"
            << ", convergence criterion = " << scientific << setprecision(3)
            << converge << " m; (" << CI.convergence << " m)" << endl;
   }

   // if the previous iteration fixed the biases, we are done no matter what
   if(Biasfix) return 5;

   if(CI.FixBiases && done)  {
      Biasfix = true;
      return 4;                     // signals one more iteration
   }

   return done;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// Utilities - use consistently throughout! these three routines must change together.
string ComposeName(const string& site1,
                   const string& site2,
                   const GSatID& sat1,
                   const GSatID& sat2)
{
try {
   //RinexSatID rsat1=sat2,rsat2=sat2;   // use RinexSatID to get the fill char == '0'
   return ( site1 + string("-") + site2 + string("_")
      //+ rsat1.toString() + string("-") + rsat2.toString() );
      //+ sat1.toString() + string("-") + sat2.toString() );
      + StringUtils::asString(sat1) + string("-") + StringUtils::asString(sat2) );
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}
//------------------------------------------------------------------------------------
string ComposeName(const DDid& ddid)
{
try {
   //ostringstream oss;
   //if(ddid.ssite > 0) oss << ddid.site1 << "-" << ddid.site2 << "_";
   //else               oss << ddid.site2 << "-" << ddid.site1 << "_";
   //if(ddid.ssat  > 0) oss << ddid.sat1  << "-" << ddid.sat2;
   //else               oss << ddid.sat2  << "-" << ddid.sat1;
   //return(oss.str());
   if(ddid.ssite > 0) {
      if(ddid.ssat > 0) 
         return ComposeName(ddid.site1,ddid.site2,ddid.sat1,ddid.sat2);
      else 
         return ComposeName(ddid.site1,ddid.site2,ddid.sat2,ddid.sat1);
   }
   else {
      if(ddid.ssat > 0) 
         return ComposeName(ddid.site2,ddid.site1,ddid.sat1,ddid.sat2);
      else 
         return ComposeName(ddid.site2,ddid.site1,ddid.sat2,ddid.sat1);
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}
//------------------------------------------------------------------------------------
void DecomposeName(const string& label,
                   string& site1,
                   string& site2,
                   GSatID& sat1,
                   GSatID& sat2)
{
try {
   string copy=label;
   //oflog << "Decompose found " << label << " = ";
   site1 = StringUtils::stripFirstWord(copy,'-');
   //oflog << site1;
   site2 = StringUtils::stripFirstWord(copy,'_');
   //oflog << " " << site2;
   sat1.fromString(StringUtils::stripFirstWord(copy,'-'));
   //oflog << " " << sat1;
   sat2.fromString(copy);
   //oflog << " " << sat2 << endl;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void OutputFinalResults(int iret)
{
try {
   int i,j,len;
   string site1,site2;
   GSatID sat1,sat2;
   format f133(13,3),f166(16,6);

   oflog << "Final Solution:" << endl;

   if(iret != -2) {

      if(CI.NRZDintervals > 0) {
         oflog << "Residual zenith tropospheric delays (m) with sigma" << endl;
         for(i=0; i<NState; i++) {
            DecomposeName(StateNL.getName(i), site1, site2, sat1, sat2);
            if(site2.substr(0,3) != string("RZD")) continue;
            oflog << site1 << " : trop delay (m) #" << site2.substr(3,site2.size()-3)
               << " " << f133 << State(i)
               << " " << f133 << SQRT(Cov(i,i))
               << endl;
         }
         oflog << endl;
      }

      oflog << "Biases (cycles) with sigma" << endl;
      for(i=0; i<NState; i++) {
         DecomposeName(StateNL.getName(i), site1, site2, sat1, sat2);
         if(site2.size() ==0 || sat1.id == -1 || sat2.id == -1) continue;
         oflog << StateNL.getName(i)
            << " " << f133 << BiasState(i)/wl1
            << " " << f133 << SQRT(BiasCov(i,i))/wl1
            << endl;
      }
      oflog << endl;

      // output position and covariance for input to later adjustment
      oflog << "Final covariance and position solutions:\n";
      for(len=0,j=0; j<NState; j++) {
         DecomposeName(StateNL.getName(j), site1, site2, sat1, sat2);
         if(site2 == string("X") || site2 == string("Y") || site2 == string("Z")) {
            if(len==0) {
               len = StateNL.getName(j).size();
               oflog << setw(len) << ' ';
               if(len < 16) len=16;
            }
            oflog << setw(len) << StateNL.getName(j);
         }
      }
      oflog << setw(len) << "Position" << endl;
      for(i=0; i<NState; i++) {
         DecomposeName(StateNL.getName(i), site1, site2, sat1, sat2);
         if(site2!=string("X") && site2!=string("Y") && site2!=string("Z"))
            continue;
         oflog << StateNL.getName(i);
         for(j=0; j<NState; j++) {
            string site22,site11;
            GSatID sat11,sat22;
            DecomposeName(StateNL.getName(j), site11, site22, sat11, sat22);
            if(site22==string("X") || site22==string("Y") || site22==string("Z"))
               oflog << scientific << setw(len) << setprecision(6) << Cov(i,j);
         }
         if(site2 == string("X")) oflog << fixed << setw(len)
            << setprecision(6) << Stations[site1].pos.X();
         if(site2 == string("Y")) oflog << fixed << setw(len)
            << setprecision(6) << Stations[site1].pos.Y();
         if(site2 == string("Z")) oflog << fixed << setw(len)
            << setprecision(6) << Stations[site1].pos.Z();
         oflog << endl;
      }
      oflog << endl;

      // output position and sigmas for all non-fixed positions
      map<string,Station>::const_iterator it;
      for(it=Stations.begin(); it != Stations.end(); it++) {
         oflog << it->first << ": " << (it->second.fixed ? "    Fixed" : "Estimated")
            << " Position "
            << Stations[it->first].pos.printf("%16.6x %16.6y %16.6z") << endl;
         if(!(Stations[it->first].fixed)) {
            oflog << it->first << ": Estimated   Sigmas";
            i = StateNL.index(it->first + string("-X"));
            oflog << " " << f166 << SQRT(Cov(i,i));
            i = StateNL.index(it->first + string("-Y"));
            oflog << " " << f166 << SQRT(Cov(i,i));
            i = StateNL.index(it->first + string("-Z"));
            oflog << " " << f166 << SQRT(Cov(i,i));
            oflog << endl;
         }
      }

      // do for all baselines
      for(i=0; i<CI.OutputBaselines.size(); i++) {
         string one=StringUtils::word(CI.OutputBaselines[i],0,'-');
         string two=StringUtils::word(CI.OutputBaselines[i],1,'-');
         Position BL = Stations[one].pos - Stations[two].pos;
         oflog << "Final Baseline " << CI.OutputBaselines[i]
            << " " << BL.printf("%16.6x %16.6y %16.6z")
            << " " << f166 << BL.getRadius() << endl;
         if(CI.Screen)
            cout << "Final Baseline " << CI.OutputBaselines[i]
            << " " << BL.printf("%16.6x %16.6y %16.6z")
            << " " << f166 << BL.getRadius() << endl;

            // offset - if one was input
         if(CI.OutputBaselineOffsets[i].mag() >= 0.01) {
            oflog << "Final  Offset  " << CI.OutputBaselines[i]
               << " " << f166 << BL.X() - CI.OutputBaselineOffsets[i][0]
               << " " << f166 << BL.Y() - CI.OutputBaselineOffsets[i][1]
               << " " << f166 << BL.Z() - CI.OutputBaselineOffsets[i][2]
               << " " << f166 << BL.getRadius() - CI.OutputBaselineOffsets[i].mag()
               << endl;
            if(CI.Screen)
               cout << "Final  Offset  " << CI.OutputBaselines[i]
               << " " << f166 << BL.X() - CI.OutputBaselineOffsets[i][0]
               << " " << f166 << BL.Y() - CI.OutputBaselineOffsets[i][1]
               << " " << f166 << BL.Z() - CI.OutputBaselineOffsets[i][2]
               << " " << f166 << BL.getRadius() - CI.OutputBaselineOffsets[i].mag()
               << endl;
         }
      }
   }
   oflog << "Data Totals: " << NEp << " epochs, " << nDD << " DDs." << endl;
   if(CI.Screen)
      cout << "Data Totals: " << NEp << " epochs, " << nDD << " DDs." << endl;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
double RMSResidualOfFit(int N, Vector<double>& dX, bool final)
{
try {
   int i,j,nd,cnt;
   double rms;
   string lab;
   Vector<double> f,Res;
   Matrix<double> P;
   map<DDid,DDData>::iterator it;
   format f166(16,6),f133(13,3),f82s(8,2,2);

      // open an output file for post fit DD residuals
   ofstream ddrofs;
   if(final && !CI.OutputDDRFile.empty()) {
      ddrofs.open(CI.OutputDDRFile.c_str(),ios::out);
      if(ddrofs) {
         oflog << "Opened file " << CI.OutputDDRFile
            << " for post fit residuals output." << endl;
         ddrofs << "# " << Title << endl;
         ddrofs << "RES site site sat sat week   sec_wk   count"
               << "            Data         Estimate         Residual" << endl;
      }
      else {
         oflog << "Warning - Failed to open DDR output file " << CI.OutputDDRFile
            << ". Do not output post fit residuals.\n";
      }
   }

      // go all the way back through the data
   nd= 0;
   rms = 0.0;
   cnt = -1;
   while(1) {
      cnt++;
      if(cnt > maxCount) break;
      Data = Vector<double>(Mmax,0.0);
      DataNL.clear();
      for(i=0,it=DDDataMap.begin(); it != DDDataMap.end(); it++) {
         j = index(it->second.count,cnt);
         if(j == -1) continue;
         if(CI.Frequency == 1) Data(i) = it->second.DDL1[j];
         if(CI.Frequency == 2) Data(i) = it->second.DDL2[j];
         if(CI.Frequency == 3)      // ionosphere-free phase
            Data(i) = if1p * it->second.DDL1[j] + if2p * it->second.DDL2[j];
         lab = ComposeName(it->first);
         DataNL += lab;
         i++;
      }
      if(i==0) continue;      // no data -- don't assume this is the end
      M = i;
      Data.resize(M);

      // this needed by EvaluateLSEquation
      SolutionEpoch = FirstEpoch + cnt*CI.DataInterval;
      EvaluateLSEquation(State,f,P);

      Res = Data - f;
      if(rms == 0.0) rms = norm(Res);
      else rms *= sqrt(1.0+norm(Res)/(rms*rms));
      nd += M;

      if(final && ddrofs) {
         string site1,site2;
         GSatID sat1,sat2;
         for(i=0; i<M; i++) {
            DecomposeName(DataNL.getName(i), site1, site2, sat1, sat2);
            ddrofs << "RES " << site1 << " " << site2 << " " << sat1 << " " << sat2
                  << " " << SolutionEpoch.printf("%4F %10.3g")
                  << " " << setw(5) << cnt
                  << " " << f166 << Data[i]
                  << " " << f166 << f[i]
                  << " " << f166 << Res[i]
                  << endl;
         }
      }

   }  // end loop over data

   if(final && ddrofs) ddrofs.close();

   rms /= sqrt(double(nd));

   return rms;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
