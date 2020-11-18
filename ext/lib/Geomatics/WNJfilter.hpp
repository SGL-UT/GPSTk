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

/// @file WNJfilter.cpp White noise jerk (and higher dimension) Kalman filter.

#ifndef WHITE_NOISE_JERK_KALMAN_FILTER
#define WHITE_NOISE_JERK_KALMAN_FILTER

// std
#include <string>
#include <vector>
#include <sstream>
// gpstk
#include "Exception.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Namelist.hpp"
#include "StringUtils.hpp"
#include "KalmanFilter.hpp"
// geomatics
#include "logstream.hpp"

//------------------------------------------------------------------------------------
class WNJfilter : public KalmanFilter {
public:
   // member data is accessible by caller, but must be set before initializeFilter().
   bool filterOutput;               // output usual KMU,KTU,KSU,etc only if true
   // initial
   gpstk::Vector<double> apState;   // apriori state, of length Nstate
   gpstk::Vector<double> apNoise;   // apriori noise, of length Nstate

   // TU
   int count;                       // index in data,msig of next point for MU

   // MU - all these parallel, time order, no gaps
   std::vector<double> ttag;        // time since first epoch (not needed by filter)
   std::vector<double> data;        // measurement data(ttag)
   std::vector<double> msig;        // measurement sigma(ttag)
   std::vector<double> psig;        // process noise sigma(ttag)
   // pointers to output state
   std::vector<double> *ptrx,*ptrv,*ptra;    // position, velocity, accel
   std::vector<double> *ptrs;                // sigma on position

   // output
   unsigned int prec,width;         // precision and width

   // member functions

   // empty c'tor - required but don't use it
   WNJfilter(void) throw()
      : filterOutput(true), ptrx(NULL), ptrv(NULL), ptra(NULL), ptrs(NULL),
            prec(2), width(9) { }

   // explicit c'tor
   WNJfilter(int dim) throw() { Reset(dim); }

   void Reset(int dim) throw()
   {
      // dim = NL.size() = Nstate is number of states : X, V, A, J, S, C, P
      gpstk::Namelist NL;
      NL += std::string("X"); NL += std::string("V"); NL += std::string("A");
      if(dim > 3) NL += std::string("J");
      if(dim > 4) NL += std::string("S");
      if(dim > 5) NL += std::string("C");
      if(dim > 6) NL += std::string("P");
      apState = gpstk::Vector<double>(dim,0.0);  // apriori state, of length Nstate
      apNoise = gpstk::Vector<double>(dim,0.0);  // apriori noise, of length Nstate
      ttag.clear();
      data.clear();
      msig.clear();
      psig.clear();
      if(ptrx) ptrx->clear();
      if(ptrv) ptrv->clear();
      if(ptra) ptra->clear();
      if(ptrs) ptrs->clear();

      KalmanFilter::Reset(NL);                   // dim's SRIF, sets Nstate=NL.size()
   }

      /** Get apriori state and covariance from user.
       * @return 1 if state std::vector X, and covariance Cov are defined,
       *   -1 if inverse covariance*state X, and inverse covariance Cov,
       *   0 if no information is returned
       * @throw Exception
       */
   int defineInitial(double& T0, gpstk::Vector<double>& State,
                                 gpstk::Matrix<double>& Cov)
   {
      count = 0;              // index into data arrays
      T0 = ttag[0];           // initial time
      Nnoise = 1;             // number of noises

      if(apState.size() != Nstate || apNoise.size() != Nstate) {
         gpstk::Exception e(std::string("Must define apState and apNoise, and they ")
            + std::string("must be of length Nstate = ")
            + gpstk::StringUtils::asString(Nstate)
            + std::string(" before calling initializeFilter"));
         GPSTK_THROW(e);
      }

      State = apState;
      Cov = gpstk::Matrix<double>(Nstate,Nstate,0.0);
      for(int i=0; i<Nstate; i++) Cov(i,i) = apNoise(i)*apNoise(i);
      LOG(DEBUG) << "defineI state " << State;
      LOG(DEBUG) << "defineI cov " << Cov;

      if(filterOutput)
         LOG(INFO) << "#K[MTS]U N   time  X     V     A    "
                  << "sigX   sigV   sigA  data  SOLresid  (M)PFresid";

      return 1;      // since Cov is covariance
   }

      /**
       * @throw Exception
       */
   void defineTimestep(const double T, const double DT,
                       const gpstk::Vector<double>& State,
                       const gpstk::Matrix<double>& Cov,
                       const bool useFlag)
   {
      if(!useFlag) {
         LOG(INFO) << "Filter is singular in defineT";
         //gpstk::Exception e("defineTimestep called with singular filter");
         //GPSTK_THROW(e);
      }

      LOG(DEBUG) << "defineT with Nstate " << Nstate << " and Nnoise " << Nnoise;
      G = gpstk::Matrix<double>(Nstate,Nnoise,0.0);
      Rw = gpstk::Matrix<double>(Nnoise,Nnoise,0.0);
      PhiInv = gpstk::Matrix<double>(Nstate,Nstate,0.0);

      // build G and Rw
      G(Nstate-1,0) = 1.0;
      Rw(0,0) = 1.0/psig[count];
      LOG(DEBUG) << "defineT makes G " << G;
      LOG(DEBUG) << "defineT makes Rw " << Rw;

      // build PhiInv, the inverse state transition matrix
      // 1 -DT DT^2/2 -DT^3/6  DT^4/24 ...
      // 0  1  -DT     DT^2/2 -DT^3/6  ...
      // 0  0   1     -DT      DT^2/2  ...
      // 0  0   0      1      -DT      ...
      // 0  0   0      0       1       ...
      // ....
      ident(PhiInv);
      for(int i=0; i<Nstate; i++) {       // rows
         double elem(-DT);
         for(int j=i+1; j<Nstate; j++) {  // cols
            PhiInv(i,j) = elem;
            elem *= -DT/double(j+1);
         }
      }
      LOG(DEBUG) << "defineT makes PhiInv\n" << PhiInv;
   }

      /** Input T,X,Cov - the current state. Output T=time of next MU
       * Fill and return the data quantities Partials,Data,MCov.
       * @return Process=0,
       *   ProcessThenQuit, quit after this data
       *   SkipThisEpoch, skip this data and output
       *   SkipThenQuit, skip this data and output, then quit
       *   QuitImmediately, quit now
       * @throw Exception
       */
   KalmanReturn defineMeasurements(double& T,
                                   const gpstk::Vector<double>& X,
                                   const gpstk::Matrix<double>& Cov,
                                   const bool useFlag)
    {
      if(!useFlag) {
         LOG(INFO) << "Filter is singular in defineM";
         //gpstk::Exception e("defineMeasurement called with singular filter");
         //GPSTK_THROW(e);
      }

      // TD make Partials, etc members of KalmanFilter, then don't have to resize
      Partials = gpstk::Matrix<double>(1,Nstate,0.0);
      Partials(0,0) = 1.0;
      Data = gpstk::Vector<double>(1);
      Data(0) = data[count];
      MCov = gpstk::Matrix<double>(1,1);
      MCov(0,0) = msig[count];

      LOG(DEBUG) << "MU at T " << T << " Data: " << Data;
      LOG(DEBUG) << "MU at T " << T << " Partials: " << Partials;
      LOG(DEBUG) << "MU at T " << T << " MCov: " << MCov;

      // next point
      count++;
      if(count == data.size()) {
         count--;
         T = ttag[count]+nominalDT; // nominalDT is stored in KalmanFilter by FF()
         return ProcessThenQuit;
      }
      T = ttag[count];
      return Process;
    }

   // output at each stage ... the user may override
   // if singular is true, State and Cov may or may not be good
   virtual void output(int N) throw()
   {
      int i;
      std::ostringstream oss;

      if(stage == Unknown) {
         LOG(ERROR) << "Kalman stage not defined in output().";
         return;
      }
      LOG(DEBUG) << "Enter KalmanFilter::output(" << N << ")";

      // define the output arrays
      if(stage == MU) {
         if(ptrx) ptrx->push_back(State(0));
         if(ptrv) ptrv->push_back(State(1));
         if(ptra) ptra->push_back(State(2));
         if(ptrs) ptrs->push_back(singular ? 0.0 : sqrt(Cov(0,0)));
      }
      if(stage == SU) {          // NB count was decremented just above
         if(ptrx) (*ptrx)[count] = State(0);
         if(ptrv) (*ptrv)[count] = State(1);
         if(ptra) (*ptra)[count] = State(2);
         if(ptrs) (*ptrs)[count] = (singular ? 0.0 : sqrt(Cov(0,0)));
      }

      if(!filterOutput) { if(stage == SU) count--; return; }

      // if MU or SU, output the namelist first
      // TD make verbose
      //if(stage == Init || stage == MU || stage == SU) {
      //   oss << ((stage==MU || stage==Init) ? "KNL" : "KSL") << KFtag << " "
      //      << std::fixed << N << " " << std::setprecision(3) << time;
      //   gpstk::Namelist NL = srif.getNames();
      //   for(i=0; i<NL.size(); i++)
      //      oss << " " << std::setw(9) << NL.getName(i);
      //   for(i=0; i<NL.size(); i++)
      //      oss << " " << std::setw(9) << std::string("sig")+NL.getName(i);
      //   oss << " " << std::setw(9) << std::string("sol.res");
      //   if(stage == MU)
      //      oss << " " << std::setw(9) << std::string("pfit-res");

      //   LOG(INFO) << oss.str();
      //   oss.str("");
      //}

      // output a label
      switch(stage) {
         case    Init: oss << "KIN"; break;
         case     IB1:
         case     IB2:
         case     IB3: oss << "KAD"; break;
         case      TU: oss << "KTU"; break;
         case      MU: oss << "KMU"; break;
         case      SU: oss << "KSU"; break;
         default: case Unknown: return; break;
      }
      oss << KFtag << " ";

      // output the time and raw data
      oss << std::fixed << N << " " << std::setprecision(3) << time;
      
      // output the state
      for(i=0; i<State.size(); i++)
         oss << " " << std::fixed << std::setprecision(prec) << std::setw(width)
                  << State(i);

      // output sqrt of diagonal covariance elements
      oss << std::scientific << std::setprecision(prec);
      for(i=0; i<State.size(); i++)
         oss << " " << std::setw(width) << (singular ? 0.0 : sqrt(Cov(i,i)));

      // if MU, also output data, sol residual and PF residual
      if(stage == MU)
         oss << std::scientific << std::setprecision(prec)
            << " " << std::setw(width) << data[count-1]
            << " " << std::setw(width) << data[count-1] - State(0)
            << " " << std::setw(width) << PFResid(0);
      // if SU, also output data, sol residual
      if(stage == SU) {
         oss << std::scientific << std::setprecision(prec)
            << " " << std::setw(width) << data[count]
            << " " << std::setw(width) << data[count] - State(0);
         count--;
      }

      LOG(INFO) << oss.str();
   }

}; // end class WNJfilter

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#endif // WHITE_NOISE_JERK_KALMAN_FILTER
