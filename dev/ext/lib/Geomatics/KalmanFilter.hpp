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

/**
 * @file KalmanFilter.hpp
 * Kalman filter implementation using the SRIFilter class.
 */

#ifndef KALMAN_FILTER_INCLUDE
#define KALMAN_FILTER_INCLUDE

#include <map>
#include <string>
#include <sstream>
#include "Exception.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Namelist.hpp"
#include "SRIFilter.hpp"
#include "logstream.hpp"

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// A base class for implementing Kalman filter using SRIFilter. Define a class with
// this class as base, implement some or all the virtual functions to define problem.
//
// The derived class MUST overload:
//   virtual int defineInitial() to define initial time and apriori state and cov.
//   virtual int defineMeasurements() to define Partials, data and meas.covariance.
//   virtual void defineTimestep() to define Phi, G, Rw and Control
//
// The derived class MAY overload these, if the state will be modified on the fly:
//   virtual int defineAddsDrops() to define an initial SRI for states to be added
//      and to define a Namelist of states to be removed. This routine is called
//      three times, before the MU, between the MU and the TU, and after the TU.
//      If it returns -1, it does nothing (this allows the caller to skip an epoch).
//
// The derived class may want to overload the output() routine.
// The Update routines may be overloaded if necessary (but it shouldn't be).
//
// How to use the derived class:
// 1. Define all the 'define' functions; this constitutes the filter design.
//
// 2. create the filter, using either empty c'tor and Reset(Namelist), or c'tor(NL)
//
// 3. initialize the filter by calling initializeFilter() (calls defineInitial())
//
// 4. call ForwardFilter(finalTime); which increment time and NTU (from 0),
//    until time reaches finalTime.
//       This routine is a loop over time; the loop consists of:
//
//       KalmanAddsDrops(), which calls
//          defineAddsDrops(1,...) to get any new states (an SRI) to be added
//          or states to be dropped (a Namelist).
//
//       KalmanMeasurementUpdate(), which calls
//          defineMeasurements() to get current time, data, mcov, and partials
//          NB. defineMeasurements() controls the time steps.
//
//       KalmanAddsDrops(), calling defineAddsDrops(2,...)
//
//       KalmanTimeUpdate(), which calls
//          defineTimestep() to get Phi,Rw,G,Control
//
//       KalmanAddsDrops(), calling defineAddsDrops(3,...)
//
// Several switches may be used to control the filter. For each there is also a 'set'
// routine.
// 1. setDoOutput(false) turns off the output routine.
// 2. setDoInvert(false) stops inversions (compute State and Cov from the SRI) and
//       output during the forward filter. NB. cf. setSRISU for the backward filter.
//
class KalmanFilter {
public:

   // enum to define the current filter operation, mostly for output()
   typedef enum FilterStageEnum {
      Unknown=0,
      Init,
      AD1, AD2, AD3,
      TU,
      MU,
      SU,
      StageCount
   } FilterStage;

   // enum to define the return values for defineMeasurements()
   typedef enum KalmanMUReturnValuesEnum {
      Process=0,
      ProcessThenQuit,
      SkipThisEpoch,
      SkipThenQuit,
      QuitImmediately,
      ReturnCount
   } KalmanReturn;

   // -------------------------------------------------------------------------------
   // 1. Define all the 'define' functions; this constitutes the filter design; these
   // get information from the user and MUST be implemented in the derived class.
   // -------------------------------------------------------------------------------
   //
   /// Get complete apriori information from user, including initial time T0, and
   /// either {state vector X, and covariance Cov} (return 1) or the inverse
   /// {inverse covariance*state X, and inverse covariance Cov} (return -1) or no
   /// (zero) information (return 0). If non-0 is returned, the matrix must be
   /// non-singular. Note that the SRIF was zero-ed by either the constructor or
   /// Reset() just before this call, so if no information is added, doInversions
   /// should be false.
   virtual int defineInitial(double& T0, gpstk::Vector<double>& X,
                              gpstk::Matrix<double>& Cov)
      throw(gpstk::Exception) = 0;

   /// Derived class must provide Partials, Data and Measurement Covariance at next
   /// data epoch time T.
   /// The current time T is passed into this routine; it should redefine T to be
   /// the time of the next data epoch.
   /// Generally, if T(next) > T(curr) + filterDT, this routine should return 2
   /// (data will not be used) and save the data until T(next) ~<= T(curr) + DT.
   /// if useFlag=false, State and Cov should NOT be used as they may be singular.
   /// Return 0 process the data,
   ///        1 process this data, but then quit
   ///        2 skip both this data and output, but don't quit
   ///        3 quit immediately without processing or output
   virtual KalmanReturn defineMeasurements(double& T,
                                           const gpstk::Vector<double>& X,
                                           const gpstk::Matrix<double>& Cov,
                                           const bool useFlag,
                                           gpstk::Matrix<double>& Partials,
                                           gpstk::Vector<double>& Data,
                                           gpstk::Matrix<double>& MCov)
       throw(gpstk::Exception) = 0;

   /// User must provide PhiInv,G,Rw,Control, given T,DT,X,Cov at each timestep.
   /// if useFlag=false, State and Cov should NOT be used ... may be singular
   virtual void defineTimestep(const double T, const double DT,
                               const gpstk::Vector<double>& State,
                               const gpstk::Matrix<double>& Cov,
                               const bool useFlag,
                               gpstk::Matrix<double>& PhiInv,
                               gpstk::Matrix<double>& G,
                               gpstk::Matrix<double>& Rw,
                               gpstk::Vector<double>& Control)
       throw(gpstk::Exception) = 0;

   /// The derived class MAY provide additions to or deletions from the state.
   /// This routine is called three times within the ForwardFilter loop. If there are
   /// no adds (drops), the SRI (Namelist) in the argument should be cleared.
   /// If there are adds, the SRI is defined by the user and gives an apriori SRI for
   /// the states to be added, including state names (must be unique) and apriori
   /// state Z and information R. If there are drops, the dNL Namelist is the list of
   /// states (currently in the SRIF) that are to be dropped.
   /// Return -1 if this epoch is to be skipped, otherwise return >= 0.
   virtual int defineAddsDrops(int which, const double T,
                                gpstk::SRI& aSRI,
                                gpstk::Namelist& dNL)
      throw(gpstk::Exception)
      { aSRI = gpstk::SRI(); dNL.clear(); return -1; }

   // -------------------------------------------------------------------------------
   // 2. create the filter, using either the empty c'tor followed by Reset(Namelist),
   // or the c'tor(Namelist), giving an initial Namelist for the filter state.

   /// empty constructor; Reset() must be called before initializing or filtering
   KalmanFilter(void)
         : NTU(0),NMU(-1),Nstate(0),stage(Unknown),Nnoise(0),
           extended(false),smoother(false),doSRISU(false),
           doOutput(true),doInversions(true),singular(true)
      { }

   /// the constructor with an initial namelist for the filter state
   KalmanFilter(const gpstk::Namelist& NL)
      { initialize(NL); }

   /// reset or recreate filter - use this after the empty constructor
   void Reset(const gpstk::Namelist& NL)
      throw()
      { initialize(NL); }

   // destructor
   virtual ~KalmanFilter(void) {};

   // -------------------------------------------------------------------------------
   /// 3. initialize the filter; this calls defineInitial to get the apriori state/cov
   virtual void initializeFilter(void) throw(gpstk::Exception)
   {
      try {
         double T;

         int isInfo;
         gpstk::Vector<double> initX;
         gpstk::Matrix<double> initCov;       // may be info or cov

         isInfo = defineInitial(T, initX, initCov);
         time = T;
         stage = Init;

         try {
            if(isInfo == -1) {
               srif.addAPrioriInformation(initCov,initX);
               Invert(std::string("Invert after adding a priori info"));
            }
            else if(isInfo == 1) {
               srif.addAPriori(initCov,initX);
               // this assumes srif was 0 before addAPriori....
               Cov = initCov;
               State = initX;
               inverted = true;
            }
            else { // returned zero
               State = initX;
               Cov = initCov;
               inverted = false;
            }
         }
         catch(gpstk::Exception& e) {
            e.addText("Failed to add apriori");
            GPSTK_RETHROW(e);
         }

         if(inverted && doOutput) output(NTU);
      }
      catch(gpstk::Exception& e) {
         e.addText("initializeFilter");
         GPSTK_RETHROW(e);
      }
   }

   // -------------------------------------------------------------
   /// 4. forward filter(finalTime,filterDT).
   /// This is main routine; it runs the filter forward to finalTime, using timesteps
   /// filterDT (NB the defineMeasurements() routine controls actual timesteps).
   /// This calls defineTimestep() to get propagation matricies at each timestep.
   /// It calls defineMeasurements() to get time of the next data, data, mcov, and
   /// partials at each timestep. Timing is controlled by defineMeasurements().
   /// It calls defineAddsDrops 3 times, before MU, between MU and TU, and after TU.
   virtual void ForwardFilter(const double finalT, const double filterDT)
      throw(gpstk::Exception)
   {
      int iret;
      try {
         // forward filter: loop over time
         while(time <= finalT) {

            // ------------------------------------------------------------
            // MOD 1: add new state and drop states from the last iteration
            // Calls defineAddsDrops(1,time,addsSRI,dropsNL,(any drops here?))
            iret = KalmanAddsDrops(1,time);

            if(iret) {
               stage = AD1;
               Invert(std::string("Invert after add/drop states"));
               if(doOutput) output(NTU);
            }

            // ------------------------------------------------------------
            // MU: returns 0: Process         : ok, process normally
            //             1: ProcessThenQuit : quit after this data,
            //             2: SkipThisEpoch   : don't process this data, but proceed
            //             3: SkipThenQuit    : skip this data, and then quit
            //             4: QuitImmediately : stop now
            // This defines nexttime as the next available data epoch.
            double nexttime = time;
            KalmanReturn ret = KalmanMeasurementUpdate(nexttime);
            if(ret == QuitImmediately)
               break;
            // else Skip
            // else SkipThenQuit
            else if(ret == Process || ret == ProcessThenQuit) {
               stage = MU;

               if(doInversions) {
                  Invert(std::string("Invert after MU"));
                  if(doOutput) output(NMU);
               }
            }

            // ------------------------------------------------------------
            // MOD 2: add new state and drop states from the last iteration
            // Calls defineAddsDrops(2,time,addsSRI,dropsNL,(any drops here?))
            iret = KalmanAddsDrops(2,time);

            if(iret) {
               stage = AD2;
               Invert(std::string("Invert after add/drop states"));
               if(doOutput) output(NTU);
            }

            // compute next timestep
            double deltaT = nexttime - time;
            if(deltaT > 1.5*filterDT) deltaT=filterDT;

            // ------------------------------------------------------------
            // TU. this will update time by deltaT
            KalmanTimeUpdate(time,deltaT);
            stage = TU;

            if(doInversions) {
               Invert(std::string("Invert after TU"));
               if(doOutput) output(NTU);
            }

            // ------------------------------------------------------------
            // MOD 3: add new state and drop states from the last iteration
            // Calls defineAddsDrops(3,time,addsSRI,dropsNL,(any drops here?))
            iret = KalmanAddsDrops(3,time);

            if(iret) {
               stage = AD3;
               Invert(std::string("Invert after add/drop states"));
               if(doOutput) output(NTU);
            }

            if(ret == ProcessThenQuit || ret == SkipThenQuit) break;

         }  // end loop over forward filter
      }
      catch(gpstk::Exception& e) {
         e.addText("ForwardFilter");
         GPSTK_RETHROW(e);
      }
   }

   // -------------------------------------------------------------
   /// output at each stage ... the user may override
   /// if singular is true, State and Cov may or may not be good
   virtual void output(int N) throw()
   {
      int i;
      std::ostringstream oss;

      if(stage == Unknown) {
         LOG(ERROR) << "Kalman stage not defined in output().";
         return;
      }

      // if MU or SU, output the namelist first
      // TD make verbose
      if(stage == MU || stage == SU) {
         oss << (stage==MU ? "KNL" : "KSL") << msg << " "
            << std::fixed << N << " " << std::setprecision(3) << time;
         gpstk::Namelist NL = srif.getNames();
         for(i=0; i<NL.size(); i++)
            oss << std::setw(10) << NL.getName(i);

         LOG(INFO) << oss.str();
         oss.str("");
      }

      // output a label
      switch(stage) {
         case    Init: oss << "KIN"; break;
         case      AD1:
         case      AD2:
         case      AD3: oss << "KAD"; break;
         case      TU: oss << "KTU"; break;
         case      MU: oss << "KMU"; break;
         case      SU: oss << "KSU"; break;
         default:
         case Unknown:
            LOG(INFO) << "Kalman stage not defined." << std::endl;
            return;
      }
      oss << msg << " ";

      // output the time
      oss << std::fixed << N << " " << std::setprecision(3) << time;

      // output the state
      for(i=0; i<State.size(); i++)
         oss << " " << std::setw(9) << State(i);

      // output sqrt of diagonal covariance elements
      oss << std::scientific << std::setprecision(2);
      for(i=0; i<State.size(); i++)
         oss << " " << std::setw(10) << (singular ? 0.0 : sqrt(Cov(i,i)));

      LOG(INFO) << oss.str();
   }

   // -------------------------------------------------------------------------------
   // The support routines
   // -------------------------------------------------------------------------------
   /// Add states to the filter, called just before the MU
   /// Return the number of states added and dropped.
   virtual int KalmanAddsDrops(int which, double T)
      throw(gpstk::Exception)
   {
      try {
         gpstk::SRI addsSRI, dropSRI;
         gpstk::Namelist addsNL;

         // get new adds and drops from the caller
         int iret = defineAddsDrops(which, T, addsSRI, dropsNL);
         if(iret < 0) return 0;           // skip this epoch
         iret = 0;


         // drop states that were found in the last iteration
         if(dropsNL.size() > 0) {
            LOG(DEBUG) << "KalmanAddsDrops(" << which << ") drops " << dropsNL;

            gpstk::Namelist saveNL(srif.getNames());

            // get state without inverting information matrix
            int i,k,nsingular;
            gpstk::Vector<double> X;
            try {
               srif.getState(X, &nsingular);
            }
            catch(gpstk::MatrixException& me) {
               for(k=0; k<dropsNL.size(); k++) {
                  i = saveNL.index(dropsNL.getName(k));
                  if(nsingular == -1 || i < nsingular) {
                     gpstk::Exception e("Cannot drop states; problem is singular");
                     GPSTK_THROW(e);
                  }
               }
            }

            //LOG(INFO) << "SRIF before drops:\n" << std::fixed
            //   << std::setprecision(5) << std::setw(10) << srif;
            //LOG(INFO) << "State before drops:\n" << std::fixed
            //   << std::setprecision(5) << std::setw(10) << X;

            // fill a vector of drop states
            gpstk::Vector<double> dropStates(dropsNL.size());
            for(k=0; k<dropsNL.size(); k++) {
               i = saveNL.index(dropsNL.getName(k));
               dropStates(k) = X(i);
            }

            //LOG(DEBUG) << "Drops states are " << std::fixed << std::setprecision(5)
            //   << std::setw(10) << dropStates;

            srif.stateFix(dropsNL, dropStates);

            //LOG(DEBUG) << "srif after drops:\n" << std::scientific
            //   << std::setprecision(5) << std::setw(10) << srif;

            iret += dropsNL.size();
         }

         if(addsSRI.size() > 0) {
            LOG(DEBUG) << "KalmanAddsDrops(" << which << ") adds "
               << addsSRI.getNames();

            // check that addsNL and srif have no states in common
            if((addsNL & srif.getNames()).size() > 0) {
               gpstk::Exception e("Adding a state that is already present");
               GPSTK_THROW(e);
            }

            //LOG(DEBUG) << "srif before adds:\n" << std::fixed
            //   << std::setprecision(5) << std::setw(10) << srif;

            // append addsSRI onto end
            srif.append(addsSRI);

            iret += addsSRI.size();
            
            //LOG(DEBUG) << "srif after adds:\n" << std::fixed
            //   << std::setprecision(5) << std::setw(10) << srif;
         }

         return iret;
      }
      catch(gpstk::Exception& e) {
         e.addText("KADD");
         GPSTK_RETHROW(e);
      }
   }

   // -------------------------------------------------------------
   /// do the measurement update;
   /// returns 0: ok, 1: quit after this data, 2: skip, 3: quit now
   virtual KalmanReturn KalmanMeasurementUpdate(double& T)
      throw(gpstk::Exception)
   {
      try {
         gpstk::Matrix<double> Partials, MCov;
         gpstk::Vector<double> Data;

         // 0: ok, 1: quit after this data, 2: skip, 3: quit now
         // TD replace with enum
         // Pass in T=current, return T=next data epoch;
         // if next > curr + filterDT, this should return 2 so TU will catch up
         KalmanReturn ret = defineMeasurements(T, State, Cov, !singular && inverted,
                                                  Partials, Data, MCov);

         PFResid = gpstk::Vector<double>(0);
         if(ret == Process || ret == ProcessThenQuit) {
            if(extended)
               srif.zeroState();
               // NB. derived class must update reference trajectory

            srif.measurementUpdate(Partials, Data, MCov);
            PFResid = Data;
            inverted = false;
            NMU++;
         }


         return ret;
      }
      catch(gpstk::Exception& e) {
         e.addText("KMU");
         GPSTK_RETHROW(e);
      }
   }

   // -------------------------------------------------------------
   /// the Kalman time update
   virtual void KalmanTimeUpdate(double T, double DT) throw(gpstk::Exception)
   {
      try {
         gpstk::Vector<double> Control, Zw;
         gpstk::Matrix<double> PhiInv, G, Rw;

         time += DT;
         defineTimestep(time, DT, State, Cov, !singular && inverted,
                              PhiInv, G, Rw, Control);

         Nnoise = Rw.rows();                    // Nnoise is member, but temporary
         Zw = gpstk::Vector<double>(Nnoise);

         // control
         if(Control.size() > 0) {
            srif.shift(-PhiInv*Control);
         }

         if(isSmoother()) {                     // save for smoother
            // timeUpdate will trash these
            PhiInvStore[NTU] = PhiInv;
            GStore[NTU] = G;
            if(Control.size() > 0) ControlStore[NTU] = Control;
         }

         Zw = 0.0;         // yes this is necessary
         gpstk::Matrix<double> Rwx(Nnoise,Nstate,0.0);
         srif.timeUpdate(PhiInv, Rw, G, Zw, Rwx);
         inverted = false;

         if(isSmoother()) {                     // save for smoother
            // indexing is 0...NTU-1
            RwStore[NTU] = Rw;
            RwxStore[NTU] = Rwx;
            ZwStore[NTU] = Zw;
            TimeStore[NTU] = time;
         }

         NTU++;
      }
      catch(gpstk::Exception& e) {
         e.addText("KTU");
         GPSTK_RETHROW(e);
      }
   }

   // -------------------------------------------------------------
   /// the smoother update
   virtual void KalmanSmootherUpdate(void) throw(gpstk::Exception)
   {
      try {
         NTU--;

         //LOG(DEBUG) << " SU at " << NTU << " with state " << srif.getNames();
         gpstk::Matrix<double> Rw = RwStore[NTU];
         gpstk::Matrix<double> Rwx = RwxStore[NTU];
         gpstk::Matrix<double> PhiInv = PhiInvStore[NTU];
         gpstk::Matrix<double> G = GStore[NTU];
         gpstk::Vector<double> Zw = ZwStore[NTU];
         // SU knows nothing about time; this is just for output purposes
         time = TimeStore[NTU];

         // should Control vector correction be here???

         if(doSRISU) {
            gpstk::Matrix<double> Phi;
            Phi = inverse(PhiInv);
            srif.smootherUpdate(Phi,Rw,G,Zw,Rwx);
            inverted = false;
         }
         else {
            srif.DMsmootherUpdate(Cov,State,PhiInv,Rw,G,Zw,Rwx);
         }

         // correct for Control vector
         if(ControlStore.size() > 0) {
            gpstk::Vector<double> Control = ControlStore[NTU];
            if(doSRISU) {
               srif.shift(PhiInv*Control);
            }
            else {
               State -= PhiInv * Control;
            }
         }
      }
      catch(gpstk::Exception& e) {
         e.addText("KSU");
         GPSTK_RETHROW(e);
      }
   }

   // -------------------------------------------------------------------------------
   // Utilities
   bool getDoInvert(void) { return doInversions; }
   void setDoInvert(bool on) { doInversions=on; }

   bool getDoOutput(void) { return doOutput; }
   void setDoOutput(bool on) { doOutput=on; }

   bool isExtended(void) { return extended; }
   void setExtended(bool ext) { extended=ext; }

   void setSmoother(bool ext) { smoother=ext; }
   bool isSmoother(void) { return smoother; }

   void setSRISU(bool ext) { doSRISU=ext; }
   bool isSRISU(void) { return doSRISU; }

   bool isSingular(void) { return singular; }

   std::string getTag(void) { return msg; }
   void setTag(std::string tag) { msg = tag; }

   gpstk::Namelist getNames(void) { return srif.getNames(); }
   gpstk::Vector<double> getState(void) { return State; }
   gpstk::Matrix<double> getCovariance(void) { return Cov; }

   // -------------------------------------------------------------------------------
   // data
protected:
   /// if true, output at each stage using the output() routine
   bool doOutput;
   /// if true, invert the SRIF to get State and Covariance whenever SRIF changes.
   /// In general it is wise to set this false, then reset to true only when the
   /// State is to be used. For example if you need the State Vector in the MU, then
   /// set doInversions=true in defineAddsDrops, then set it back to false in defineM.
   /// Inversions are called between all the define...() calls.
   bool doInversions;
   /// if true then the SRIF is currently singular (not a problem unless doInversions)
   bool singular;
   /// if true use the SRIF form of the smoother update in the backward filter
   bool doSRISU;
   /// if true the filter is considered extended; this will zero the state before MU
   bool extended;
   /// if true the forward filter will save the data needed by the backward filter
   bool smoother;
   /// if true then the SRI has been inverted and State and Cov are valid
   bool inverted;

   int NTU,NMU,Nstate,Nnoise;       // Nnoise is there only for the user
                                    // NTU increases with TU, decreases with SU
                                    // NMU just counts MUs, mainly for user
   FilterStage stage;
   double time;
   double big,small;                // condition number at last inversion
   std::string msg;                 // optional tag to put in output (2nd field)
   gpstk::Vector<double> State;
   gpstk::Matrix<double> Cov;
   gpstk::SRIFilter srif;
   gpstk::Vector<double> PFResid;   // valid after MU
   // store by forward filter, used by backward filter
   std::map<int, gpstk::Matrix<double> > RwStore, RwxStore, PhiInvStore, GStore;
   std::map<int, gpstk::Vector<double> > ZwStore, ControlStore;
   std::map<int, double> TimeStore;
   // these are different in that there is not a value at every NTU
   std::map<int, gpstk::Namelist> AddsStore, NamesStore;
   std::map<int, gpstk::SRI> DropsStore;
   // when there are adds and drops on the same epoch, the dropped SRI put in Store
   // must have adds removed from it.
   gpstk::Namelist dropsNL;         // save drops until after TU

   // -------------------------------------------------------------------------------
private:
   // for internal use in constructors and by Reset
   void initialize(const gpstk::Namelist& NL)
   {
      Nstate = NL.size();
      //Nnoise = Ns;    // Nnoise is for the user only
      NTU = 0;
      NMU = -1;

      stage = Unknown;

      // initialize the SRIF
      srif = gpstk::SRIFilter(NL);
      inverted = false;

      State = gpstk::Vector<double>(Nstate,0.0);
      Cov = gpstk::Matrix<double>(Nstate,Nstate,0.0);

      // clear stores
      RwStore.clear(); RwxStore.clear(); PhiInvStore.clear(); GStore.clear();
      ZwStore.clear(); ControlStore.clear();
      TimeStore.clear(); AddsStore.clear(), NamesStore.clear(); DropsStore.clear();

      dropsNL.clear();
   }

   // use to invert the SRIF to get State and Covariance
   void Invert(const std::string& msg=std::string()) throw(gpstk::Exception)
   {
      LOG(DEBUG) << msg;
      if(!doInversions) return;

      // get state and covariance
      try {
         srif.getStateAndCovariance(State,Cov,&small,&big);
         singular = false;
         inverted = true;
         Nstate = srif.size();
      }
      catch(gpstk::Exception& e) {
         singular = true;
         e.addText(msg);
         GPSTK_RETHROW(e);
      }
   }

}; // end class KalmanFilter

#endif
