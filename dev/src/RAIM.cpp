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
 * @file RAIM.cpp
 * Autonomous pseudorange navigation solution, including RAIM algorithm
 * This module is now deprecated; use PRSolution.
 */
 
#include <cstdlib>  // for std::abs()
#include "MathBase.hpp"
#include "RAIMSolution.hpp"
#include "EphemerisRange.hpp"
#include "GPSGeoid.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   /* /deprecated  Use PRSolution
   * 'Tr'          Measured time of reception of the data.
   * 'Satellite'   Vector of satellites; if element i is marked
   *               (meaning satellite[i].prn < 0), ignore this data.
   *               On successful return, satellites not used are marked.
   * 'Pseudorange' Vector of raw pseudoranges (parallel to satellite).
   *               On successful return, contains residual of fit
   *               (for unmarked satellites).
   * 'Eph'         EphemerisStore to be used.
   *
   * Return values:
   *  2  failed to find a good solution (RMS residual or slope exceed limits)
   *  1  solution is suspect (slope is large)
   *  0  ok
   * -1  failed to converge
   * -2  singular problem
   * -3  not enough good data to form a RAIM solution
   *     (the 4 satellite solution might be returned - check isValid())
   * -4  ephemeris not found for one or more satellites
   */
   int RAIMSolution::Compute(const DayTime& Tr, vector<RinexPrn>& Satellite,
      vector<double>& Pseudorange, const EphemerisStore& Eph,
      TropModel *pTropModel) throw(Exception)
      {
      try {
         Valid = false;

         // count how many good satellites we have
         int i,j,N=0;
         for(i=0; i<Satellite.size(); i++) if(Satellite[i].prn > 0) N++;
         if(N < 4) return -3;

         // Save the input solution (for use in rejection when ResidualCriterion
         // is false).
         APrioriSolution = Solution;

         // minimum number of sats needed for algorithm
         int MinSV=5;   // RAIM
                        // not really RAIM || not RAIM (one solution):
         if(!ResidualCriterion || NSatsReject==0) MinSV=4;

         // default is -1, meaning as many as possible
         if(NSatsReject == -1) NSatsReject=N-MinSV;

         // ----------------------------------------------------------------
         // fill SVP
         if((i=PrepareAutonomousSolution(Tr, Satellite, Pseudorange, Eph, SVP)))
            return i;

         // ----------------------------------------------------------------
         // Compute a solution for each set of valid satellites.
         // Do this inside a loop, in which all allowed combinations of
         //    good satellites are used.
         // The combinations of good satellites are determined by
         //    1) there must be at least MinSV good ones, and
         //    2) no more than NSatsReject can be (temporarily) marked bad.
         // Inside the loop, save the 'best' (lowest RMS residual) solution.
         //

               // UseSat is used to mark good sats (true) and those to ignore (false).
         Vector<bool> UseSat(Satellite.size()),UseSave;
            // stage is the number of satellites currently being excluded.
         int stage,stageold,Udim;
            // MarkIndex is a list of indexes into vector Satellite,
            // with -1 marking 'unused'; it is used to temporarily mark the satellites
            // to be excluded from the positioning.
         Vector<int> MarkIndex;
            // Slope will be used by the positioning routine to return slopes for
            // each satellite.
         Vector<double> Slope(Pseudorange.size());
            // Residual will be used by the positioning routine to return the range
            // residuals.
         Vector<double> Residual(Satellite.size(),0.0);
            // Save the 'best' solution.
         int BestNIter;
         double BestRMS,BestSL,BestConv;
         Vector<double> BestSol;
         Vector<bool> BestUse;

         if(NSatsReject > 0) MarkIndex=Vector<int>(NSatsReject,-1);

            // Initialize UseSat based on Satellite
         for(i=0; i<Satellite.size(); i++)
            if(Satellite[i].prn > 0) UseSat[i]=true;
               else UseSat[i]=false;
         UseSave = UseSat;
         Udim = Satellite.size();
         while(Udim > 0 && !UseSat(Udim-1)) Udim--;
         if(Udim == 0) return -3;

         int iret,jret;
         BestRMS = -1.0;      // mark 'unused'

         stage = stageold = 0;
         while(1) {
            // Compute a solution given the data; ignore ranges for marked satellites.
            // Fill Vector Slope with slopes for each unmarked satellite.
            // Return 0  ok
            //       -1  failed to converge
            //       -2  singular problem
            //       -3  not enough good data
            NIterations = MaxNIterations;             // pass limits in
            Convergence = ConvergenceLimit;
            iret = AutonomousPRSolution(Tr, UseSat, SVP, pTropModel, Algebraic,
               NIterations, Convergence, Solution, Covariance, Residual, Slope);

            // Compute RMS residual when "distance from a priori" is the criterion.
            if(!ResidualCriterion) {
               Vector<double> D=Solution-APrioriSolution;
               RMSResidual = RMS(D);
            }
            else {
               RMSResidual = RMS(Residual);
            }

            // find the maximum slope
            MaxSlope = 0.0;
            if(iret == 0)
               for(i=0; i<UseSat.size(); i++)
                  if(UseSat(i) && Slope(i)>MaxSlope) MaxSlope=Slope[i];

            // print solution with diagnostic information
            if(Debug) { //*pDebugStream) 
               *pDebugStream << "RPS " << setw(2) << stage
                  << " " << setw(4) << Tr.GPSfullweek()
                  << " " << fixed << setw(10) << setprecision(3) << Tr.GPSsecond()
                  << " " << setw(2) << N-stage
                  << " " << setw(16) << setprecision(6) << Solution(0)
                  << " " << setw(16) << setprecision(6) << Solution(1)
                  << " " << setw(16) << setprecision(6) << Solution(2)
                  << " " << setw(14) << setprecision(6) << Solution(3)
                  << " " << setw(12) << setprecision(6) << RMSResidual
                  << " " << fixed << setw(5) << setprecision(1) << MaxSlope
                  << " " << NIterations
                  << " " << scientific << setw(8) << setprecision(2) << Convergence;
                  // print the RinexPrn for good sats
               for(i=0; i<UseSat.size(); i++) {
                  if(UseSat(i)) *pDebugStream << " " << setw(3) << Satellite[i].prn;
                  else *pDebugStream << " " << setw(3) << -::abs(Satellite[i].prn);
               }
               *pDebugStream << " (" << iret << ")" << std::endl;
            }

            if(iret) {     // failure for this combination
               RMSResidual = 0.0;
               Solution = 0.0;
            }
            else {         // success
               if((stage==0 || ReturnAtOnce) && RMSResidual<RMSLimit) {
                  Nsvs = Satellite.size();
                  Valid=true;
                  return 0;
               }
                  // save 'best' solution
               if(BestRMS < 0.0 || RMSResidual < BestRMS) {
                  BestRMS = RMSResidual;
                  BestSol = Solution;
                  BestUse = UseSat;
                  BestSL = MaxSlope;
                  BestConv = Convergence;
                  BestNIter = NIterations;
               }
            }

            // is there enough data to continue?
            if(N < MinSV) { iret=-3; break; }

            // 'increment' the combinations
            UseSat = UseSave;
            int in=0;      // 'in' must be here and must be set to 0 every call
            jret = IncrementMarkedIndexes(MarkIndex,in,UseSat,Udim,stage);
            for(i=0; i<MarkIndex.size(); i++)
               if(MarkIndex(i) > -1) UseSat(MarkIndex(i))=false;

            if(stage != stageold || jret == -2) {
               // did it transition to a new stage?
               if(BestRMS > 0.0 && BestRMS < RMSLimit) { iret=0; break; }
               // are there enough satellites for the algorithm to continue?
               if(jret==-2 || Satellite.size()-stage < int(MinSV)) { iret=2; break; }
               // save the current stage
               stageold = stage;
            }

         }  // end while(1) loop over combinations of good satellites

         // copy out the best solution and return
         Convergence = BestConv;
         NIterations = BestNIter;
         RMSResidual = BestRMS;
         Solution = BestSol;
         MaxSlope = BestSL;
         for(Nsvs=0,i=0; i<BestUse.size(); i++) {
            if(!BestUse(i)) Satellite[i].prn = -std::abs(Satellite[i].prn);
            else Nsvs++;
         }

         if(iret==0 && BestSL > SlopeLimit) iret=1;
         if(iret>=0 && BestRMS >= RMSLimit) iret=2;

         if(iret==0) Valid=true;
         return iret;

      }
      catch(Exception& e) {
         GPSTK_RETHROW(e);
      }
      }  // end RAIMSolution::Compute()

   // used by the RAIM algorithm to form all usable combinations of satellites
   // return -2 when no more combinations exist, else return 0.
   int RAIMSolution::IncrementMarkedIndexes(Vector<int>& MI, int& n,
      Vector<bool>& V, int& Vd, int& st)
      {
         if(n >= MI.size()) return -2; // n is the index currently being incremented
         do { MI(n)++; } while(MI(n) < Vd-n && !V(MI(n)));
         if(MI(n) >= Vd-n) {  // increment n, ie begin incrementing the next index
            n++;
            if(n >= MI.size()) return -2;
            if(IncrementMarkedIndexes(MI, n, V, Vd, st) == -2) return -2;
            n--;
         }
         if(n>0) {
            MI(n-1) = MI(n)+1;
            while(MI(n-1) < Vd-n && !V(MI(n-1))) { MI(n-1)++; }
         }
         if(n+1 > st) st=n+1;
         return 0;
      }

   /* Compute the satellite position / corrected range matrix (SVP); used in
    * AutonomousPRSolution(). SVP is output, dimensioned (N,4) where N is the number
    * of satellites and the length of both Satellite and Pseudorange. Data is ignored
    * whenever Satellite[i].prn is < 0.
    * Return values:
    *  0  ok
    * -4  ephemeris not found for one or more satellites
    */
   int PrepareAutonomousSolution(const DayTime& Tr, vector<RinexPrn>& Satellite,
      vector<double>& Pseudorange, const EphemerisStore& Eph, Matrix<double>& SVP)
      {
         int i,j,N=Satellite.size();
         DayTime tx;                // transmit time
         Xvt PVT;

         if(N <= 0) return 0;
         SVP = Matrix<double>(N,4);
         SVP = 0.0;

         for(i=0; i<N; i++) {
               // skip marked satellites
            if(Satellite[i].prn <= 0) continue;

               // first estimate of transmit time
            tx = Tr;
            tx -= Pseudorange[i]/C_GPS_M;
               // get ephemeris range, etc
            try {
               PVT = Eph.getPrnXvt(short(Satellite[i].prn), tx);
            }
            catch(EphemerisStore::NoEphemerisFound& e) {
               Satellite[i].prn = -std::abs(Satellite[i].prn);
               continue;
            }

               // update transmit time and get ephemeris range again
            tx -= PVT.dtime;     // clk+rel
            try {
               PVT = Eph.getPrnXvt(short(Satellite[i].prn), tx);
            }
            catch(EphemerisStore::NoEphemerisFound& e) {
               Satellite[i].prn *= -std::abs(Satellite[i].prn);
            }

               // SVP = {SV position at transmit time}, raw range + clk + rel
            for(j=0; j<3; j++) SVP(i,j) = PVT.x[j];
            SVP(i,3) = Pseudorange[i] + C_GPS_M * PVT.dtime;
         }

         return 0;
  
      } // end PrepareAutonomousPRSolution


   // internal use only - details of the algebraic solution
   int AlgebraicSolution(Matrix<double>& A, Vector<double>& Q, Vector<double>& X,
      Vector<double>& R)
      {
       try {
         int N=A.rows();
         Matrix<double> AT=transpose(A);
         Matrix<double> B=AT,C(4,4);

         C = AT * A;
         // invert
         try {
            //double big,small;
            //condNum(C,big,small);
            //if(small < 1.e-15 || big/small > 1.e15) return -2;
            C = inverseSVD(C);
         }
         catch(SingularMatrixException& sme) {
            return -2;
         }

         B = C * AT;

         Vector<double> One(N,1.0),V(4),U(4);
         double E,F,G,d,lam;

         U = B * One;
         V = B * Q;
         E = Minkowski(U,U);
         F = Minkowski(U,V) - 1.0;
         G = Minkowski(V,V);
         d = F*F-E*G;
         if(d < 0.0) d=0.0; // avoid imaginary solutions: what does this really mean?
         d = SQRT(d);

            // first solution ...
         lam = (-F+d)/E;
         X = lam*U + V;
         X(3) = -X(3);
            // ... and its residual
         R(0) = A(0,3)-X(3) - RSS(X(0)-A(0,0), X(1)-A(0,1), X(2)-A(0,2));

            // second solution ...
         lam = (-F-d)/E;
         X = lam*U + V;
         X(3) = -X(3);
            // ... and its residual
         R(1) = A(0,3)-X(3) - RSS(X(0)-A(0,0), X(1)-A(0,1), X(2)-A(0,2));

            // pick the right solution
         if(ABS(R(1)) > ABS(R(0))) {
            lam = (-F+d)/E;
            X = lam*U + V;
            X(3) = -X(3);
         }

            // compute the residuals
         for(int i=0; i<N; i++)
            R(i) = A(i,3)-X(3) - RSS(X(0)-A(i,0), X(1)-A(i,1), X(2)-A(i,2));
      
         return 0;

      }
      catch(Exception& e) {
         GPSTK_RETHROW(e);
      }
      }  // end AlgebraicSolution


   /* /deprecated  Use PRSolution
    *  Compute a single autonomous pseudorange solution.
    *   DayTime Tr           data time tag (for use by some trop models)
    *   Vector<bool> Use     of length N, the number of satellites; if value is
    *                           false, do not include it in the computation.
    *   Matrix<double> SVP   of dimension (N,4).  This Matrix must have been
    *                           computed by calling PrepareAutonomousPRSolution().
    *   bool Algebraic       flag indicating algebraic (true) algorithm, or
    *                           linearized least squares (false).
    *   pTropModel           pointer to TropModel for use within the algorithm
    *
    *   Weight matrix TD......
    *
    * Input and output (for least squares only; ignored if Algebraic==true):
    *   int n_iterate         limit on iterations. On output, the number of iterations
    *                            used.
    *   double converge       convergence criterion (RSS change in solution, meters).
    *                            On output, the final value.
    * Output:  (these will be resized within the function)
    *   Vector<double> Sol    solution (ECEF & time; all in meters) length 4
    *   Matrix<double> Cov    covariance matrix (meter*meter) dimension 4x4
    *   Vector<double> Resid  range residuals for each satellite (m),
    *                            length N-(number of satellites with Use=true).
    *   Vector<double> Slope  slope value used in RAIM for each good satellite,
    *                            length N
    * Return values:
    *  0  ok
    * -1  failed to converge
    * -2  singular problem
    * -3  not enough good data to form a solution (at least 4 satellites required)
    * -4  ephemeris not found for one or more satellites
    */
   int AutonomousPRSolution(const DayTime& T, const Vector<bool>& Use,
      const Matrix<double> SVP, TropModel *pTropModel, const bool Algebraic,
      int& n_iterate, double& converge, Vector<double>& Sol, Matrix<double>& Cov,
      Vector<double>& Resid, Vector<double>& Slope)
         throw(Exception)
      {
      try {
         int iret,i,j,n,N;
         double rho,wt,svxyz[3];
         GPSGeoid geoid;               // WGS84?

            // find the number of good satellites
         for(N=0,i=0; i<Use.size(); i++) if(Use(i)) N++;
         if(N < 4) return -3;

            // define for computation
         Vector<double> CRange(N),dX(4);
         Matrix<double> P(N,4),PT,G(4,N),PG(N,N);
         Xvt SV,RX;

         Sol.resize(4);
         Cov.resize(4,4);
         Resid.resize(N);
         Slope.resize(Use.size());

            // define for algebraic solution
         Vector<double> U(4),Q(N);
         Matrix<double> A(P);
            // and for linearized least squares
         int niter_limit = (n_iterate<2 ? 2 : n_iterate);
         double conv_limit = converge;

            // prepare for iteration loop
         Sol = 0.0;                                   // initial guess: center of earth
         n_iterate = 0;
         converge = 0.0;

            // iteration loop
            // do at least twice (even for algebraic solution) so that
            // trop model gets evaluated
         do {
               // current estimate of position solution
            RX.x = ECEF(Sol(0),Sol(1),Sol(2));

               // loop over satellites, computing partials matrix
            for(n=0,i=0; i<Use.size(); i++) {
                  // ignore marked satellites
               if(!Use(i)) continue;

                  // time of flight (sec)
               if(n_iterate == 0)
                  rho = 0.070;             // initial guess: 70ms
               else
                  rho = RSS(SVP(i,0)-Sol(0), SVP(i,1)-Sol(1), SVP(i,2)-Sol(2))
                            / geoid.c();

                  // correct for earth rotation
               wt = geoid.angVelocity()*rho;             // radians
               svxyz[0] =  ::cos(wt)*SVP(i,0) + ::sin(wt)*SVP(i,1);
               svxyz[1] = -::sin(wt)*SVP(i,0) + ::cos(wt)*SVP(i,1);
               svxyz[2] = SVP(i,2);

                  // corrected pseudorange (m)
               CRange(n) = SVP(i,3);

                  // correct for troposphere (but not on the first iteration)
               if(n_iterate > 0) {
                  SV.x = ECEF(svxyz[0],svxyz[1],svxyz[2]);
                  CRange(n) -= pTropModel->correction(RX,SV,T);
               }

                  // geometric range
               rho = RSS(svxyz[0]-Sol(0),svxyz[1]-Sol(1),svxyz[2]-Sol(2));

                  // partials matrix
               P(n,0) = (Sol(0)-svxyz[0])/rho;           // x direction cosine
               P(n,1) = (Sol(1)-svxyz[1])/rho;           // y direction cosine
               P(n,2) = (Sol(2)-svxyz[2])/rho;           // z direction cosine
               P(n,3) = 1.0;

                  // data vector: corrected range residual
               Resid(n) = CRange(n) - rho - Sol(3);

                  // TD: allow weight matrix = measurement covariance
               // P *= MCov;
               // Resid *= MCov;

                  // compute intermediate quantities for algebraic solution
               if(Algebraic) {
                  U(0) = A(n,0) = svxyz[0];
                  U(1) = A(n,1) = svxyz[1];
                  U(2) = A(n,2) = svxyz[2];
                  U(3) = A(n,3) = CRange(n);
                  Q(n) = 0.5 * Minkowski(U,U);
               }

               n++;        // n is number of good satellites - used for Slope
            }  // end loop over satellites


               // compute information matrix = inverse covariance matrix
            PT = transpose(P);
            Cov = PT * P;

               // invert using SVD
            //double big,small;
            //condNum(PT*P,big,small);
            //if(small < 1.e-15 || big/small > 1.e15) return -2;
            try { Cov = inverseSVD(Cov); }
            catch(SingularMatrixException& sme) {
               return -2;
            }

               // generalized inverse
            G = Cov * PT;
            PG = P * G;                         // used for Slope

            n_iterate++;                        // increment number iterations

               // ----------------- algebraic solution -----------------------
            if(Algebraic) {
               iret = AlgebraicSolution(A,Q,Sol,Resid);
               if(iret) return iret;                     // (singular)
               if(n_iterate > 1) {                       // need 2, for trop
                  iret = 0;
                  break;
               }
            }
               // ----------------- linearized least squares solution --------
            else {
               dX = G * Resid;
               Sol += dX;
                  // test for convergence
               converge = norm(dX);
                  // success: quit
               if(n_iterate > 1 && converge < conv_limit) {
                  iret = 0;
                  break;
               }
                  // failure: quit
               if(n_iterate >= niter_limit || converge > 1.e10) {
                  iret = -1;
                  break;
               }
            }
               

         } while(1);    // end iteration loop

            // compute slopes
         Slope = 0.0;
         if(iret == 0) for(j=0,i=0; i<Use.size(); i++) {
            if(!Use(i)) continue;
            for(int k=0; k<4; k++) Slope(i) += G(k,j)*G(k,j);
            Slope(i) = SQRT(Slope(i)*double(n-4)/(1.0-PG(j,j)));
            j++;
         }

         return iret;

      }
      catch(Exception& e) {
         GPSTK_RETHROW(e);
      }
      } // end AutonomousPRSolution

} // namespace gpstk
