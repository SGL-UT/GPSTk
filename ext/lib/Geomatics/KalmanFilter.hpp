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

/// @file KalmanFilter.hpp
/// Kalman filter implementation using the SRIFilter class.

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
/// A base class for implementing Kalman filter using SRIFilter. Define a class with
/// this class as base, implement some or all the virtual functions to define problem.
///
/// The derived class MUST overload:
///   virtual int defineInitial() to define initial time and apriori state and cov.
///   virtual int defineMeasurements() to define Partials, Data and MCov.
///   virtual void defineTimestep() to define Phi, G, Rw and Control
///
/// The derived class MAY overload this function, which is called between updates,
///   virtual int defineInterim() to output, change the state or anything else.
///      This routine is called at four times:
///        before the measurement update (MU),
///        between the MU and the time update (TU),
///        after the TU,
///        between smoother updates (SU).
///  If it returns >0 output() is called.
///
/// The derived class may want to overload the output() routine.
/// The Update routines may be overloaded if necessary (but it shouldn't be).
///
/// Note that a "time" is used by the Kalman to index the data and determine how to
/// time update (TU); this need not be a real time, however; it is ONLY required that
/// 0. defineInitial(T0 ...) defines the starting value of T (NB also timeReverse)
/// 1. ForwardFilter(finalT,dt) defines ending value of T and nominalDT = dt
/// 2. defineMeasurements(T...) on input, T tells which data (epoch) to get
/// 3. defineMeasurements(T...) on output, T contains the time of the NEXT data epoch
/// The actual value of T is used only a) it is printed in output(), and
///   b) some implementations will use DT in the TU to compute process noise(s).
/// Otherwise, T could be completely fictional.
///
/// How to use the derived class:
/// 1. Define all the 'define' functions; this constitutes the filter design.
/// 2. create the filter, using either empty c'tor and Reset(Namelist), or c'tor(NL)
/// 3. initialize the filter by calling initializeFilter() (calls defineInitial())
/// 4. call ForwardFilter(finalTime,dt); which increments time by dt and NTU (from 0),
///    until time reaches finalTime.
///       This routine is a loop over time; the loop consists of:
///       KalmanInterim(), which calls defineInterim(1,...)
///       KalmanMeasurementUpdate(), which calls
///          defineMeasurements() to get current time, data, mcov, and partials
///          NB. defineMeasurements() controls the time steps.
///       KalmanInterim(), calling defineInterim(2,...)
///       KalmanTimeUpdate(), which calls
///          defineTimestep() to get Phi,Rw,G,Control
///       KalmanInterim(), calling defineInterim(3,...)
///  5. call BackwardFilter(M); this will smooth, starting at the current time down to
///     NTU = M, it will decrement both time and NTU.
///     This calls KalmanInterim(), calling defineInterim(4,...)
///     NB. Smoothing knows nothing about time, but times stored during the
///        forward filter are restored here for output purposes.
///
/// Several switches may be used to control the filter. For each there is also a 'set'
/// routine.
/// 1. setDoOutput(false) turns off the output routine.
/// 2. setDoInvert(false) stops inversions (compute State and Cov from the SRI) and
///       output during the forward filter. NB. cf. setSRISU for the backward filter.
/// 3. setSmoother(true) must be called before ForwardFilter() if BackwardFilter() is
///       to be called. This causes the smoothing information to be stored during TUs.
/// 4. setSRISU(true) causes BackwardFilter() to use the SRIF form of the smoothing
///       algorithm (which requires inversions); otherwise the DM form is used.
/// 5. setTimeReverse(true) to run in reverse time order.
///
class KalmanFilter {
public:

      // enum to define the current filter operation, mostly for output()
   typedef enum FilterStageEnum {
      Unknown=0,
      Init,
      IB1, IB2, IB3,             // for "in between" meaning Interim
      TU,
      MU,
      SU,
      StageCount
   } FilterStage;

      // enum to define the return values for defineMeasurements()
   typedef enum KalmanMUReturnValuesEnum {
      Process=0,
      ProcessThenQuit,  // 1
      SkipThisEpoch,    // 2
      SkipThenQuit,     // 3
      QuitImmediately,  // 4
      ReturnCount       // 5
   } KalmanReturn;

      // -------------------------------------------------------------------------------
      // data
protected:
      /// if true, output at each stage using output() routine. NB used inside output()
   bool doOutput;
      /// if true, invert the SRIF to get State and Covariance whenever SRIF changes.
      /// In general it is wise to set this false, then reset to true only when the
      /// State is to be used. For example if you need the State Vector in the MU, then
      /// set doInversions=true in defineIntermin(1), then set to false in defineM.
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
      /// if true then independent variable "time" decreases
   bool timeReversed;
      /// if true, do a "dry run" calling all user-defined func, but none of the SRIF
   bool dryRun;

   int NTU;                         ///< count of time updates: ++ in TU, -- in SU
   int NMU;                         ///< count of measurement updates
   int NSU;                         ///< count of smoother updates
   int Nstate;                      ///< number of state elements
   int Nnoise;                      ///< Nnoise is there only for the user

   FilterStage stage;               ///< current stage of the filter - see enum
   double time;                     ///< seconds since start
   double nominalDT;                ///< change in time for one TU seconds
   double big,small;                ///< condition number at last inversion = b/s
   std::string KFtag;               ///< optional tag to put in output (2nd field)

   gpstk::Vector<double> State;     ///< filter state
   gpstk::Matrix<double> Cov;       ///< filter covariance
   gpstk::SRIFilter srif;           ///< SRIF
      // MU
   gpstk::Vector<double> PFResid;   ///< post-fit residuals - valid after MU
   gpstk::Matrix<double> Partials;  ///< matrix defined by defineM() and used in MU
   gpstk::Vector<double> Data;      ///< vector defined by defineM() and used in MU
   gpstk::Matrix<double> MCov;      ///< measurement covariance (defineM() for MU)
      // TU
   gpstk::Vector<double> Zw;        ///< SRIF vector used internally
   gpstk::Vector<double> Control;   ///< SRIF vector used internally
   gpstk::Matrix<double> PhiInv;    ///< SRIF matrix used internally - inv state trans
   gpstk::Matrix<double> G;         ///< SRIF matrix used internally - noise
   gpstk::Matrix<double> Rw;        ///< SRIF matrix used internally
      // SU
   gpstk::Vector<double> SMResid;   ///< post-smoother residuals - value after SU

      /// Storage for smoothing algorithm; stored by forward filter, used by SU
   typedef struct Smoother_storage_record {
      gpstk::Matrix<double> Rw;
      gpstk::Matrix<double> Rwx;
      gpstk::Matrix<double> PhiInv;
      gpstk::Matrix<double> G;
      gpstk::Vector<double> Zw;
      gpstk::Vector<double> Control;
      double Time;
   } SmootherStoreRec;
   std::map<int, SmootherStoreRec> SmootherStore;

public:
      // functions
      // -------------------------------------------------------------------------------
      // 1. Define all the 'define' functions; this constitutes the filter design; these
      // get information from the user and MUST be implemented in the derived class.
      // -------------------------------------------------------------------------------

      /** Pure virtual function, to be overloaded and provided by the
       * caller, providing complete apriori information, including
       * initial time T0, and either {state vector X, and covariance
       * Cov} (return 1) or the inverse {inverse covariance*state X,
       * and inverse covariance Cov} (return -1) or no (zero)
       * information (return 0). If non-0 is returned, the matrix must
       * be non-singular. Note that the SRIF was zero-ed by either the
       * constructor or Reset() just before this call, so if no
       * information is added, doInversions should be false.
       * @param T0 initial time
       * @param X initial state/inv(cov)*state/ignored as return 1/-1/0
       * @param Cov initial covariance/inv(cov)/ignored as return 1/-1/0
       * @param return 1 for state/cov, -1 for information, 0 for
       *   nothing provided
       * @throw Exception
       */
   virtual int defineInitial(double& T0,
                             gpstk::Vector<double>& X,
                             gpstk::Matrix<double>& Cov) = 0;

      /** Pure virtual function, to be overloaded and provided by the
       * caller, providing members Partials, Data and MCov
       * (M. Covariance) at next data epoch time T.  The current time
       * T is passed into this routine; it should redefine T to be the
       * time of the next data epoch.
       * Generally, if T(next) > T(curr) + nominalDT, this routine
       * should return Skip(2) (data will not be used) and save the
       * data until T(next) ~<= T(curr) + DT.  if useFlag=false, State
       * and Cov should NOT be used as they may be singular.
       * @param[in] T current time, on output time of NEXT set of data
       * @param[in] X current state
       * @param[in] C current covariance
       * @param[in] useFlag if false, State and Cov are singular - do not use
       * @return Process (0) process the data,
       *         ProcessThenQuit (1) process this data, but then quit
       *         SkipThisEpoch (2) skip both this data and output, but
       *            don't quit
       *         SkipThenQuit (3) skip data and output, and quit
       *         QuitImmediately (4) quit immediately without
       *            processing or output
       * @throw Exception
       */
   virtual KalmanReturn defineMeasurements(double& T,
                                           const gpstk::Vector<double>& X,
                                           const gpstk::Matrix<double>& C,
                                           const bool useFlag) = 0;

      /** Pure virtual function, to be overloaded and provided by the
       * caller, providing members PhiInv,G,Rw,Control, given
       * T,DT,X,Cov at each timestep.  if useFlag=false, State and Cov
       * should NOT be used ... may be singular
       * @param[in] T current time
       * @param[in] DT current timestep
       * @param[in] X current state
       * @param[in] C current covariance
       * @param[in] useFlag if false, State and Cov are singular - do not use
       * @throw Exception
       */
   virtual void defineTimestep(const double T, const double DT,
                               const gpstk::Vector<double>& State,
                               const gpstk::Matrix<double>& Cov,
                               const bool useFlag) = 0;

      /** Pure virtual function, to be overloaded and provided by the
       * caller, providing This routine is called three times within
       * the ForwardFilter loop: before MU, between MU and TU, and
       * after TU, and once during the BackwardFilter between SUs.
       * @param which = 1(before MU), 2(between MU and TU), 3(after TU),
       *   4(after SU)
       * @return -1 if this epoch is to be skipped, otherwise return >= 0.
       * @throw Exception
       */
   virtual int defineInterim(int which, const double Time)
   { return -1; }

      // -------------------------------------------------------------------------------
      // 2. create the filter, using either the empty c'tor followed by Reset(Namelist),
      // or the c'tor(Namelist), giving an initial Namelist for the filter state.

      /** empty constructor; Reset() must be called before
       * initializing or filtering */
   KalmanFilter(void)
         : NTU(0),NMU(0),NSU(0),Nstate(0),stage(Unknown),Nnoise(0),
           extended(false),smoother(false),doSRISU(true),
           doOutput(true),doInversions(true),singular(true),
           timeReversed(false),dryRun(false)
   { }

      /// Constructor given an initial Namelist for the filter state
      /// @param NL Namelist of the filter states (determines Nstate)
   KalmanFilter(const gpstk::Namelist& NL)
   { Reset(NL); }

      /// Reset or recreate filter - use this after the empty constructor
      /// @param NL Namelist of the filter states (determines Nstate)
   void Reset(const gpstk::Namelist& NL) throw()
   { initialize(NL); }

      /// destructor
   virtual ~KalmanFilter(void) {};

      // -------------------------------------------------------------------------------
      /** 3. initialize the filter; this calls defineInitial() to get
       * the apriori state and covariance (or information)
       * @throw Exception
       */
   virtual void initializeFilter(void)
   {
      try {
         double T;

         int isInfo;
         gpstk::Vector<double> initX;
         gpstk::Matrix<double> initCov;       // may be info or cov

            // call derived class to get initial time, apriori state
            // and covariance
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
               Invert(std::string("Invert after adding a priori info"));
                  //// this assumes srif was 0 before addAPriori....
                  //Cov = initCov;
                  //State = initX;
                  //inverted = true;
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

         if(inverted) output(NTU);
      }
      catch(gpstk::Exception& e) { e.addText("initializeFilter"); GPSTK_RETHROW(e); }
   }

      // -------------------------------------------------------------
      /** 4. forward filter(finalTime,nominalDT).  This is main
       * routine; it runs the filter forward to finalTime, using
       * timesteps nominalDT (NB the defineMeasurements() routine
       * controls actual timesteps).  This calls defineTimestep() to
       * get propagation matricies at each timestep.  It calls
       * defineMeasurements() to get time of the next data, data,
       * mcov, and partials at each timestep. Timing is controlled by
       * defineMeasurements().  It calls defineInterim 3 times, before
       * MU, between MU and TU, and after TU.
       * @param finalT time at which to stop the filter
       * @param DT nominal timestep
       * @throw Exception
       */
   virtual void ForwardFilter(const double finalT, const double DT)
   {
      int iret;
      try {
            // don't allow a non-positive timestep
         if((!timeReversed && DT <= 0.0) || (timeReversed && DT >= 0.0)) {
            gpstk::Exception e(std::string("Filter time step must be ")
                               + (timeReversed ? std::string("< 0") : std::string("> 0")));
            GPSTK_THROW(e);
         }

            // save filter timestep, which is the timestep of one TU,
            // and ~timestep between consecutive defineMeasurements()
         nominalDT = DT;

            // to avoid round-off problems, make time comparisons only to within tol
         const double tol(nominalDT/10.0);

            // forward filter: loop over time
         double tmax(finalT+nominalDT);
         while((timeReversed ? (time-tmax >= tol) : (time-tmax <= -tol))) {

               // ------------------------------------------------------------
               // interim #1
            iret = KalmanInterim(1,time);

            if(iret) {
               stage = IB1;
               Invert(std::string("Invert after interim 1"));
               output(NTU);
            }

               // ------------------------------------------------------------
               // MU: returns 0: Process         : ok, process normally
               //             1: ProcessThenQuit : quit after this data,
               //             2: SkipThisEpoch   : don't process this data, but proceed
               //             3: SkipThenQuit    : skip this data, and then quit
               //             4: QuitImmediately : stop now
               // This defines nexttime as the next available data epoch.
            double nexttime = time;
            KalmanReturn kfret = KalmanMeasurementUpdate(nexttime);
            if(kfret == QuitImmediately || kfret == SkipThenQuit)
               break;
               // else SkipThisEpoch
            else if(kfret == Process || kfret == ProcessThenQuit) {
               stage = MU;

               if(doInversions) {
                  Invert(std::string("Invert after MU"));
                  output(NMU);
               }
            }
               // TD would you ever want several TUs before the first good MU?
            else if(kfret == SkipThisEpoch && NTU == 0) {
               time = nexttime;
               continue;
            }

               // ------------------------------------------------------------
               // interim #2
            iret = KalmanInterim(2,time);

            if(iret) {
               stage = IB2;
               Invert(std::string("Invert after interim 2"));
               output(NTU);
            }

               // ------------------------------------------------------------
               // compute next timestep
            double deltaT = nexttime - time;
               // why the 1.5? why not? it must be >1 and <=2
            if(::fabs(deltaT) > 1.5*::fabs(nominalDT)) deltaT=nominalDT;

               // TU. this will update time by deltaT
            KalmanTimeUpdate(time,deltaT);
            stage = TU;

            if(doInversions) {
               Invert(std::string("Invert after TU"));
               output(NTU);
            }

               // ------------------------------------------------------------
               // interim #3
            iret = KalmanInterim(3,time);

            if(iret) {
               stage = IB3;
               Invert(std::string("Invert after interim 3"));
               output(NTU);
            }

            if(kfret == ProcessThenQuit || kfret == SkipThenQuit) break;

         }  // end loop over forward filter
      }
      catch(gpstk::Exception& e) { e.addText("ForwardFilter"); GPSTK_RETHROW(e); }
   }

      // -------------------------------------------------------------
      // prevent common confusion - Forward(t,dt) but Backward(firstNTU usually 0)
      /// Backward filter (smoother) with double argument - this is a trick to prevent
      /// the user from calling BackwardFilter(time).
   void BackwardFilter(double M)
   {
      GPSTK_THROW(gpstk::Exception("BackwardFilter must be called with integer NTU"));
   }

      // -------------------------------------------------------------
      /** Backward filter or smoother. Smooth down to
       * NTU==M. Decrements time and NTU Calls defineInterim(4,time)
       * after each smoother update
       * @param M value of NTU at which to stop the smoother (usually 0)
       * @throw Exception
       */
   virtual void BackwardFilter(int M)
   {
      try {
         if(!isSmoother()) {
            gpstk::Exception e("Use setSmoother(true) to turn on smoothing");
            GPSTK_THROW(e);
         }
         if(singular) {
            gpstk::Exception e("Cannot smooth singular filter");
            GPSTK_THROW(e);
         }

         stage = SU;
         if(M < 0) M=0;

         while(NTU > M) {

               // Do the SU. Decrements time by timestep, and decrements NTU (first)
            KalmanSmootherUpdate();

               // get state after SU -- only if using SRISU, not SRIS_DM
            if(doSRISU) {
               Invert(std::string("Invert after SRISU"));
            }

               // ------------------------------------------------------------
               // interim #4
               // Calls defineInterim(4,time);     NB ignore return value
            KalmanInterim(4,time);

               // output - do it here so names agree forward/backward
            output(NTU);

         }  // end loop
      }
      catch(gpstk::Exception& e) { e.addText("BackwardFilter"); GPSTK_RETHROW(e); }
   }

      // -------------------------------------------------------------
      /// Output at each stage ... the user may override
      /// if singular is true, State and Cov may or may not be good
      /// @param N user-defined counter that is included on each line after the tag.
   virtual void output(int N) throw()
   {
      if(!doOutput) return;

      unsigned int i;
      std::ostringstream oss;

      if(stage == Unknown) {
         LOG(ERROR) << "Kalman stage not defined in output().";
         return;
      }
      LOG(DEBUG) << "Enter KalmanFilter::output(" << N << ")";

         // if MU or SU, output the namelist first
         // TD make verbose
      if(stage == Init || stage == MU || stage == SU) {
         oss << ((stage==MU || stage==Init) ? "KNL" : "KSL") << KFtag << " "
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
         case     IB1:
         case     IB2:
         case     IB3: oss << "KIB"; break;
         case      TU: oss << "KTU"; break;
         case      MU: oss << "KMU"; break;
         case      SU: oss << "KSU"; break;
         default:
         case Unknown:
            LOG(INFO) << "Kalman stage not defined." << std::endl;
            return;
      }
      oss << KFtag << " ";

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
      /** Interim processing.
       * @return defineInterim(), if >0 output() is called, ignored after SU
       * @throw Exception
       */
   virtual int KalmanInterim(int which, double Time)
   {
      try {
         int iret = defineInterim(which, Time);
         if(iret < 0) return Process;
         return iret;
      }
      catch(gpstk::Exception& e) { e.addText("KINT"); GPSTK_RETHROW(e); }
   }

      // -------------------------------------------------------------
      /** Perform the measurement update;
       * @param[in,out] T current time(input), time of NEXT data(output)
       * @return Process=0,
       *         ProcessThenQuit, quit after this data
       *         SkipThisEpoch, skip this data and output
       *         SkipThenQuit, skip this data and output, then quit
       *         QuitImmediately, quit now
       * @throw Exception
       */
   virtual KalmanReturn KalmanMeasurementUpdate(double& T)
   {
      try {
            // Pass in T=current, return T=next data epoch;
            // if next > curr+nominalDT, should return SkipThisEpoch so TU will catch up
         KalmanReturn ret = defineMeasurements(T, State, Cov, !singular && inverted);
         PFResid = gpstk::Vector<double>(0);
         if(ret == Process || ret == ProcessThenQuit) {
            if(extended)
               srif.zeroState();
               // NB. derived class must update reference trajectory

            if(!dryRun) {
                  // this func whitens before update, then unwhitens resid (PFResid)
               PFResid = Data;   // MU will replace with post-fit residuals
               srif.measurementUpdate(Partials, PFResid, MCov);
            }

            inverted = false;
            NMU++;
         }

         return ret;
      }
      catch(gpstk::Exception& e) { e.addText("KMU"); GPSTK_RETHROW(e); }
   }

      // -------------------------------------------------------------
      /** the Kalman time update
       * @param[in] T current time
       * @param[in] DT current timestep
       * @throw Exception
       */
   virtual void KalmanTimeUpdate(double T, double DT)
   {
      try {
            //LOG(INFO) << "KTU with NTU NMU " << NTU << " " << NMU;

         double timesave(time);

         time += DT;
         defineTimestep(time, DT, State, Cov, !singular && inverted);

         Nnoise = Rw.rows();                 // Nnoise is member, but temporary
         Zw = gpstk::Vector<double>(Nnoise);

            // control
         if(Control.size() > 0) {
            srif.shift(-PhiInv*Control);        // not tested
         }

            // create a new smoother storage record
         if(isSmoother()) {                     // save for smoother
            SmootherStore[NTU] = SmootherStoreRec();
            SmootherStoreRec& rec = SmootherStore[NTU];
               // timeUpdate will trash these
            rec.PhiInv = PhiInv;
            rec.G = G;
            if(Control.size() > 0) rec.Control = Control;
         }

         Zw = 0.0;         // yes this is necessary
         gpstk::Matrix<double> Rwx(Nnoise,Nstate,0.0);
         if(!dryRun) srif.timeUpdate(PhiInv, Rw, G, Zw, Rwx);
         inverted = false;

         if(isSmoother()) {                     // save for smoother
            SmootherStoreRec& rec = SmootherStore[NTU];
               // indexing is 0...NTU-1
            rec.Rw = Rw;
            rec.Rwx = Rwx;
            rec.Zw = Zw;
            rec.Time = timesave;
         }

         NTU++;
      }
      catch(gpstk::Exception& e) { e.addText("KTU"); GPSTK_RETHROW(e); }
   }

      // -------------------------------------------------------------
      /** the smoother update
       * @throw Exception
       */
   virtual void KalmanSmootherUpdate(void)
   {
      try {
         NTU--;
         NSU++;

            //LOG(DEBUG) << " SU at " << NTU << " with state " << srif.getNames();
         SmootherStoreRec& rec(SmootherStore[NTU]);
         gpstk::Matrix<double> Rw = rec.Rw;
         gpstk::Matrix<double> Rwx = rec.Rwx;
         gpstk::Matrix<double> PhiInv = rec.PhiInv;
         gpstk::Matrix<double> G = rec.G;
         gpstk::Vector<double> Zw = rec.Zw;
            // SU knows nothing about time; this is just for output purposes
         time = rec.Time;

            // TD should Control vector correction be here???

         if(!dryRun) {
            if(doSRISU) {
               gpstk::Matrix<double> Phi;
               Phi = inverse(PhiInv);
               srif.smootherUpdate(Phi,Rw,G,Zw,Rwx);
               inverted = false;
            }
            else
               srif.DMsmootherUpdate(Cov,State,PhiInv,Rw,G,Zw,Rwx);
         }

            // correct for Control vector
         if(rec.Control.size() > 0) {
            gpstk::Vector<double> Control = rec.Control;
            if(doSRISU)
               srif.shift(PhiInv*Control);
            else
               State -= PhiInv * Control;
         }
      }
      catch(gpstk::Exception& e) { e.addText("KSU"); GPSTK_RETHROW(e); }
   }

      // -------------------------------------------------------------------------------
      // Utilities
      /// if doInversions, SRIF is inverted at each step, defining State and Cov
   bool getDoInvert(void) { return doInversions; }
   void setDoInvert(bool on) { doInversions=on; }

      /// if doOutput, output() is called at each step
   bool getDoOutput(void) { return doOutput; }
   void setDoOutput(bool on) { doOutput=on; }

      /// if extended, use an extended Kalman (not implemented)
   bool isExtended(void) { return extended; }
   void setExtended(bool ext) { extended=ext; }

      /// if smoother, save info during forward filter for use by backward filter
   void setSmoother(bool ext) { smoother=ext; }
   bool isSmoother(void) { return smoother; }

      /// if doSRISU use SRIF form of smoother, else DM smoother
   void setSRISU(bool ext) { doSRISU=ext; }
   bool isSRISU(void) { return doSRISU; }

      /// true when filter is singular
   bool isSingular(void) { return singular; }

      /// if timeReversed, time T decreases during the forward filter
   void setTimeReverse(bool tr=true) { timeReversed=tr; }
   bool isTimeReversed(void) { return timeReversed; }

      /// if dryRun, do not operate the filter, just print
   void setDryRun(bool t=true) { dryRun=t; }
   bool isDryRun(void) { return dryRun; }

      /// KF tag is a user-defined string output on each line
   std::string getTag(void) { return KFtag; }
   void setTag(std::string tag) { KFtag = tag; }

      /// get the filter SRI
   void setSRI(gpstk::SRI& sri) { srif = static_cast<gpstk::SRIFilter&>(sri); }
   gpstk::SRI getSRI(void) { return static_cast<gpstk::SRI>(srif); }

      /// get the state namelist
   gpstk::Namelist getNames(void) { return srif.getNames(); }
      /// get the state (must be non-singular)
   gpstk::Vector<double> getState(void) { return State; }
      /// get the covariance (must be non-singular)
   gpstk::Matrix<double> getCovariance(void) { return Cov; }

      /// get number of measurements processed
   int getNMU(void) { return NMU; }

private:
      // -------------------------------------------------------------------------------
      /// for internal use in constructors and by Reset. Create SRIF and initialize
      /// counters and stores
   void initialize(const gpstk::Namelist& NL)
   {
      Nstate = NL.size();
         //Nnoise = Ns;    // Nnoise is for the user only
      NTU = NMU = NSU = 0;

      stage = Unknown;

         // initialize the SRIF
      srif = gpstk::SRIFilter(NL);
      inverted = false;

      State = gpstk::Vector<double>(Nstate,0.0);
      Cov = gpstk::Matrix<double>(Nstate,Nstate,0.0);

         // clear smoother store
      SmootherStore.clear();
   }


      /** For internal use to invert the SRIF to get State and Covariance
       * @throw Exception
       */
   void Invert(const std::string& msg=std::string())
   {
      if(dryRun) {
         LOG(INFO) << "Dry invert" << (msg.empty() ? "" : " "+msg);
         return;
      }
      if(!doInversions) {
         LOG(DEBUG) << msg << " (doInversions false)";
         return;
      }

         // get state and covariance
      try {
         srif.getStateAndCovariance(State,Cov,&small,&big);
         singular = false;
         inverted = true;
         Nstate = srif.size();
         LOG(DEBUG) << msg << " (non-singular)";
      }
      catch(gpstk::Exception& e) {
         singular = true;
         inverted = false;
         LOG(DEBUG) << msg << " (singular)";
         e.addText(msg);
         GPSTK_RETHROW(e);
      }
      catch(std::exception& e) {
         gpstk::Exception E(std::string("std exception: ") + e.what());
         GPSTK_THROW(E);
      }
   }

}; // end class KalmanFilter

#endif
