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
 * @file SRIleastSquares.cpp
 * Include file defining class SRIleastSquares, which inherits class SRI and
 * implements a general least squares algorithm that includes linear or linearized
 * problems, weighting, robust estimation, and sequential estimation.
 */

//------------------------------------------------------------------------------------
// GPSTk includes
#include "SRIleastSquares.hpp"
#include "RobustStats.hpp"
#include "StringUtils.hpp"

//------------------------------------------------------------------------------------
using namespace std;

namespace gpstk {
using namespace StringUtils;

//------------------------------------------------------------------------------------
// empty constructor
SRIleastSquares::SRIleastSquares(void) throw()
{ defaults(); }

//------------------------------------------------------------------------------------
// constructor given the dimension N.
SRIleastSquares::SRIleastSquares(const unsigned int N)
   throw()
{
   defaults();
   R = Matrix<double>(N,N,0.0);
   Z = Vector<double>(N,0.0);
   names = Namelist(N);
}

//------------------------------------------------------------------------------------
// constructor given a Namelist, its dimension determines the SRI dimension.
SRIleastSquares::SRIleastSquares(const Namelist& NL)
   throw()
{
   defaults();
   if(NL.size() <= 0) return;
   R = Matrix<double>(NL.size(),NL.size(),0.0);
   Z = Vector<double>(NL.size(),0.0);
   names = NL;
}

//------------------------------------------------------------------------------------
// explicit constructor - throw if the dimensions are inconsistent.
SRIleastSquares::SRIleastSquares(const Matrix<double>& Rin,
                     const Vector<double>& Zin,
                     const Namelist& NLin)
   throw(MatrixException)
{
   defaults();
   if(Rin.rows() != Rin.cols() ||
      Rin.rows() != Zin.size() ||
      Rin.rows() != NLin.size()) {
      MatrixException me("Invalid input dimensions: R is "
         + asString<int>(Rin.rows()) + "x"
         + asString<int>(Rin.cols()) + ", Z has length "
         + asString<int>(Zin.size()) + ", and NL has length "
         + asString<int>(NLin.size())
         );
      GPSTK_THROW(me);
   }
   R = Rin;
   Z = Zin;
   names = NLin;
}

//------------------------------------------------------------------------------------
// operator=
SRIleastSquares& SRIleastSquares::operator=(const SRIleastSquares& right)
   throw()
{
   R = right.R;
   Z = right.Z;
   names = right.names;
   iterationsLimit = right.iterationsLimit;
   convergenceLimit = right.convergenceLimit;
   divergenceLimit = right.divergenceLimit;
   doWeight = right.doWeight;
   doRobust = right.doRobust;
   doLinearize = right.doLinearize;
   doSequential = right.doSequential;
   doVerbose = right.doVerbose;
   valid = right.valid;
   number_iterations = right.number_iterations;
   number_batches = right.number_batches;
   rms_convergence = right.rms_convergence;
   condition_number = right.condition_number;
   Xsave = right.Xsave;
   return *this;
}

//------------------------------------------------------------------------------------
// SRI least squares update (not the Kalman measurement update).
// Given data and measurement covariance, compute a solution and
// covariance using the appropriate least squares algorithm.
// @param D   Data vector, length M
//               Input:  raw data
//               Output: post-fit residuals
// @param X   Solution vector, length N
//               Input:  nominal solution X0 (zero when doLinearized is false)
//               Output: final solution
// @param Cov Covariance matrix, dimension (N,N)
//               Input:  (If doWeight is true) inverse measurement covariance
//                       or weight matrix(M,M)
//               Output: Solution covariance matrix (N,N)
// @param LSF Pointer to a function which is used to define the equation to be solved.
// LSF arguments are:
//            X  Nominal solution (input)
//            f  Values of the equation f(X) (length M) (output)
//            P  Partials matrix df/dX evaluated at X (dimension M,N) (output)
//        When doLinearize is false, LSF should ignore X and return the (constant)
//        partials matrix in P and zero in f.
// @return 0 ok
//               -1 Problem is underdetermined (M<N) // TD -- naturalized sol?
//               -2 Problem is singular
//               -3 Algorithm failed to converge
//               -4 Algorithm diverged
//
// Reference for robust least squares: Mason, Gunst and Hess,
// "Statistical Design and Analysis of Experiments," Wiley, New York, 1989, pg 593.
//
// Notes on the algorithm:
// Least squares, including linearized (iterative) and sequential processing.
// This class will solve the equation f(X) = D, a vector equation in which the
// solution vector X is of length N, and the data vector D is of length M.
// The function f(X) may be linear, in which case it is of the form
// P*X=D where P is a constant matrix,
// or non-linear, in which case it will be linearized by expanding about a given
// nominal solution X0:
//          df |
//          -- |     * dX = D - f(X0),
//          dX |X=X0
// where dX is defined as (X-X0), the new solution is X, and the partials matrix is
// P=(df/dX)|X=X0. Dimensions are P(M,N)*dX(N) = D(M) - f(X0)(M).
// Linearized problems are iterated until the solution converges (stops changing). 
// 
// The solution may be weighted by a measurement covariance matrix MCov,
// or weight matrix W (in which case MCov = inverse(W)). MCov must be non-singular.
// 
// Options are to make the algorithm linearized (via the boolean input variable
// doLinearize) and/or sequential (doSequential).
// 
//    - linearized. When doLinearize is true, the algorithm solves the linearized
//    version of the measurement equation (see above), rather than the simple
//    linear version P*X=D. Also when doLinearize is true, the code will iterate
//    (repeat until convergence) the linearized algorithm; if you don't want to
//    iterate, set the limit on the number of iterations to zero.
//    NB In this case, a solution must be found for each nominal solution
//    (i.e. the information matrix must be non-singular); otherwise there can be
//    no iteration.
// 
//    - sequential. When doSequential is true, the class will save the accumulated
//    information from all the calls to this routine since the last reset()
//    within the class. This means the resulting solution is determined by ALL the
//    data fed to the class since the last reset(). In this case the data is fed
//    to the algorithm in 'batches', which may be of any size.
// 
//    NB When doLinearize is true, the information stored in the class has a
//    different interpretation than it does in the linear case.
//    Calling Solve(X,Cov) will NOT give the solution vector X, but rather the
//    latest update (X-X0) = (X-Xsave).
// 
//    NB In the linear case, the result you get from sequentially processing
//    a large dataset in many small batches is identical to what you would get
//    by processing all the data in one big batch. This is NOT true in the
//    linearized case, because the information at each batch is dependent on the
//    nominal state. See the next comment.
// 
//    NB Sequential, linearized LS really makes sense only when the state is
//    changing. It is difficult to get a good solution in this case with small
//    batches, because the stored information is dependent on the (final) state
//    solution at each batch. Start with a good nominal state, or with a large
//    batch of data that will produce one.
// 
// The general Least Squares algorithm is:
//  0. set i=0.
//  1. If non-sequential, or if this is the first call, set R=z=0
//     (However doing this prevents you from adding apriori/constraint information)
//  2. Let X = X0 (X0 = initial nominal solution - input). if linear, X0==0.
//  3. Save SRIsave=SRI and X0save=X0                       (SRI is the pair R,z)
//  4. start iteration i here.
//  5. increment the number of iterations i
//  6. Compute partials matrix P and f(X0) by calling LSF(X0,f,P).
//        if linear, LSF returns the constant P and f(X0)=0.
//  7. Set R = SRIsave.R + P(T)*inverse(MCov)*P                 (T means transpose)
//  8. Set z = SRIsave.z + P(T)*inverse(MCov)*(D-f(X0))
//  9. [The measurement equation is now P*DX=d-F(X0)
//        where DX=(X-X0save); in the linear case it is PX = d and DX = X ]
// 10. Solve z = Rx to get
//          Cov = inverse(R)
//       and DX = inverse(R)*z OR
// 11. Set X = X0save + DX
//     [or in the linear case X = DX
// 12. Compute RMS change in X: rms = ||X-X0||/N    (not X-X0save)
// 13. if linear goto quit [else linearized]
// 14. If rms > divergence limit, goto quit(failure).
// 15. If i > 1 and rms < convergence limit, goto quit(success)
// 16. If i (number of iterations) >= iteration limit, goto quit(failure)
// 17. Set X0 = X
// 18. Return to step 5.
// 19. quit: if(sequential and failed) set SRI=SRIsave.
// 
// From the code:
//  1a. Save SRI (i.e. R, Z) in Rapriori, Zapriori
//  2a. If non-sequential, or if this is the first call, set R=z=0 -- DON'T
//  3a. If sequential and not the first call, X = Xsave
//  4a. if linear, X0=0; else X0 is input. Let NominalX = X0
//  5a. set number_iterations = 0
//  6a. start iteration
//  7a. increment number_iterations
//  8a. get partials and f from LSfunc using NominalX
//  9a. if robust, compute weight matrix
// 10a. if number_iterations > 1, restore (R,Z) = (Rapriori,Zapriori)
// 11a. MU : R,Z,Partials,D-f(NominalX),MeasCov(if weighted)
// 12a. Invert to get Xsol  [ Xsol = X-NominalX or, if linear = X]
// 13a. if linearized, add NominalX to Xsol; Xsol now == X = new estimate
// 14a. if linear and not robust, quit here
// 15a. if linearized, compute rms_convergence = RMS(Xsol - NominalX)
// 16a. if robust, recompute weights and define rms_convergence = RMS(old-new wts)
// 17a. failed? if so, and sequential, restore (R,Z) = (Rapriori,Zapriori); quit
// 18a. success? quit
// 19a. if linearized NominalX = Xsol;  if robust NominalX = X
// 20a. iterate - return to 6a.
// 21a. set X = Xsol for return value
// 22a. save X for next time : Xsave = X
//
int SRIleastSquares::dataUpdate(Vector<double>& D,
                                Vector<double>& X,
                                Matrix<double>& Cov,
                                void (LSF)(Vector<double>& X,
                                           Vector<double>& f,
                                           Matrix<double>& P)) throw(MatrixException)
{
   const int M = D.size();
   const int N = R.rows();
   if(doVerbose) cout << "\nSRIleastSquares::leastSquaresUpdate : M,N are "
      << M << "," << N << endl;

   // errors
   if(N == 0) {
      MatrixException me("Called with zero-sized SRIleastSquares");
      GPSTK_THROW(me);
   }
   if(doLinearize && M < N) {
      MatrixException me(
            string("When linearizing, problem must not be underdetermined:\n")
            + string("   data dimension is ") + asString(M)
            + string(" while state dimension is ") + asString(N));
      GPSTK_THROW(me);
   }
   if(doSequential && R.rows() != X.size()) {
      MatrixException me("Sequential problem has inconsistent dimensions:\n  SRI is "
         + asString<int>(R.rows()) + "x"
         + asString<int>(R.cols()) + " while X has length "
         + asString<int>(X.size()));
      GPSTK_THROW(me);
   }
   if(doWeight && doRobust) {
      MatrixException me("Cannot have doWeight and doRobust both true.");
      GPSTK_THROW(me);
   }
   // TD disallow Robust and Linearized ? why?
   // TD disallow Robust and Sequential ? why?

try {
   int i,j,iret;
   double big,small;
   Vector<double> f(M),Xsol(N),NominalX,Res(M),Wts(M,1.0),OldWts(M,1.0);
   Matrix<double> Partials(M,N),MeasCov(M,M);
   const Matrix<double> Rapriori(R);
   const Vector<double> Zapriori(Z);

   // save measurement covariance matrix
   if(doWeight) MeasCov=Cov;

   // NO ... this prevents you from giving it apriori information...
   // if the first time, clear the stored information
   //if(!doSequential || number_batches==0)
   //   zeroAll();

   // if sequential and not the first call, NominalX must be the last solution
   if(doSequential && number_batches != 0) X = Xsave;

   // nominal solution
   if(!doLinearize) {
      if(X.size() != N) X=Vector<double>(N);
      X = 0.0;
   }
   NominalX = X;

   valid = false;
   condition_number = 0.0;
   rms_convergence = 0.0;
   number_iterations = 0;
   iret = 0;

   // iteration loop
   do {
      number_iterations++;

      // call LSF to get f(NominalX) and Partials(NominalX)
      LSF(NominalX,f,Partials);

      // Res will be both pre- and post-fit data residuals
      Res = D-f;
      if(doVerbose) {
         cout << "\nSRIleastSquares::leastSquaresUpdate :";
         if(doLinearize || doRobust)
            cout << " Iteration " << number_iterations;
         cout << endl;
         LabelledVector LNX(names,NominalX);
         LNX.message(" Nominal X:");
         cout << LNX << endl;
         cout << " Pre-fit data residuals:  "
            << fixed << setprecision(6) << Res << endl;
      }

      // build measurement covariance matrix for robust LS
      if(doRobust) {
         MeasCov = 0.0;
         for(i=0; i<M; i++) MeasCov(i,i) = 1.0 / (Wts(i)*Wts(i));
      }

      // restore apriori information
      if(number_iterations > 1) {
         R = Rapriori;
         Z = Zapriori;
      }

      // update information with simple MU
      if(doVerbose) {
         cout << " Meas Cov:";
         for(i=0; i<M; i++) cout << " " << MeasCov(i,i);
         cout << endl;
         cout << " Partials:\n" << Partials << endl;
      }
      //if(doRobust || doWeight)
      //   measurementUpdate(Partials,Res,MeasCov);
      //else
      //   measurementUpdate(Partials,Res);
      {
         Matrix<double> P(Partials);
         Cholesky<double> Ch;
         if(doRobust || doWeight) {
            Ch(MeasCov);
            Matrix<double> L = inverse(Ch.L);
            P = L * P;
            Res = L * Res;
         }

         // update with whitened information
         SrifMU(R, Z, P, Res);

         // un-whiten the residuals
         if(doRobust || doWeight)
            Res = Ch.L * Res;
      }

      if(doVerbose) {
         cout << " Updated information matrix\n" << LabelledMatrix(names,R) << endl;
         cout << " Updated information vector\n" << LabelledVector(names,Z) << endl;
      }

      // invert
      try { getStateAndCovariance(Xsol,Cov,&small,&big); }
      catch(SingularMatrixException& sme) {
         iret = -2;
         break;
      }
      condition_number = big/small;
      if(doVerbose) {
         cout << " Condition number: " << scientific << condition_number
            << fixed << endl;
         cout << " Post-fit data residuals:  "
            << fixed << setprecision(6) << Res << endl;
      }

      // update X: when linearized, solution = dX
      if(doLinearize) {
         Xsol += NominalX;
      }
      if(doVerbose) {
         LabelledVector LXsol(names,Xsol);
         LXsol.message(" Updated X:");
         cout << LXsol << endl;
      }

      // linear non-robust is done..
      if(!doLinearize && !doRobust) break;

      // test for convergence of linearization
      if(doLinearize) {
         rms_convergence = RMS(Xsol - NominalX);
         if(doVerbose) {
            cout << " RMS convergence : "
               << scientific << rms_convergence << fixed << endl;
         }
      }

      // test for convergence of robust weighting, and compute new weights
      if(doRobust) {
         // must de-weight post-fit residuals
         LSF(Xsol,f,Partials);
         Res = D-f;

         // compute a new set of weights
         double mad,median;
         //for(mad=0.0,i=0; i<M; i++)
         //   mad += Wts(i)*Res(i)*Res(i);
         //mad = sqrt(mad)/sqrt(Robust::TuningA*(M-1));
         mad = Robust::MedianAbsoluteDeviation(&(Res[0]),Res.size(),median);

         OldWts = Wts;
         for(i=0; i<M; i++) {
            if(Res(i) < -RobustTuningT*mad)
               Wts(i) = -RobustTuningT*mad/Res(i);
            else if(Res(i) > RobustTuningT*mad)
               Wts(i) = RobustTuningT*mad/Res(i);
            else
               Wts(i) = 1.0;
         }

         // test for convergence
         rms_convergence = RMS(OldWts - Wts);
         if(doVerbose) cout << " Convergence: "
            << scientific << setprecision(3) << rms_convergence << endl;
      }

      // failures
      if(rms_convergence > divergenceLimit) iret=-4;
      if(number_iterations >= iterationsLimit) iret=-3;
      if(iret) {
         if(doSequential) {
            R = Rapriori;
            Z = Zapriori;
         }
         break;
      }

      // success
      if(number_iterations > 1 && rms_convergence < convergenceLimit) break;

      // prepare for another iteration
      if(doLinearize)
         NominalX = Xsol;
      if(doRobust)
         NominalX = X;

   } while(1); // end iteration loop

   number_batches++;
   if(doVerbose) cout << "Return from SRIleastSquares::leastSquaresUpdate\n\n";

   if(iret) return iret;
   valid = true;

   // output the solution
   Xsave = X = Xsol;

   // put residuals of fit into data vector, or weights if Robust
   if(doRobust) D = OldWts;
   else         D = Res;

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// output operator
ostream& operator<<(ostream& os, const SRIleastSquares& srif)
{
   Namelist NL(srif.names);
   NL += string("State");
   Matrix<double> A;
   A = srif.R || srif.Z;
   LabelledMatrix LM(NL,A);
   LM.setw(os.width());
   LM.setprecision(os.precision());
   os << LM;
   return os;
}

//------------------------------------------------------------------------------------
// reset the computation, i.e. remove all stored information
void SRIleastSquares::zeroAll(void)
{
   SRI::zeroAll();
   Xsave = 0.0;
   number_batches = 0;
}

//------------------------------------------------------------------------------------
// reset the computation, i.e. remove all stored information, and
// optionally change the dimension. If N is not input, the
// dimension is not changed.
// @param N new SRIleastSquares dimension (optional).
void SRIleastSquares::Reset(const int N) throw(Exception)
{
   try {
      if(N > 0 && N != R.rows()) {
         R.resize(N,N,0.0);
         Z.resize(N,0.0);
      }
      else
         SRI::zeroAll(N);
      if(N > 0) Xsave.resize(N);
      Xsave = 0.0;
      number_batches = 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
} // end namespace gpstk
