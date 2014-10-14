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

//------------------------------------------------------------------------------------
// tkalm.cpp  Test Kalman filtering and smoothing
//------------------------------------------------------------------------------------
// This program takes dual frequency range and phase data for a complete satellite
// pass and computes the ionospheric delay and the phase biases. The data has been
// edited of any bad points and there are no cycleslips.
//
// This model from the smoothing algorithm does not work well for numerical reasons;
// the range of the data is too large, i.e. mag(data) too large compared to state
// [ P1 ]   [ 1      1     0  0 ] [ R  ]         (alpha = f1^2/f2^2 - 1)
// [ P2 ] = [ 1   alpha+1  0  0 ] [ I  ]
// [ L1 ]   [ 1     -1     1  0 ] [ B1 ]
// [ L2 ]   [ 1  -alpha-1  0  1 ] [ B2 ]
//
// instead use this model, which removes the large numbers from the problem
// Data      = Partials            * State
// [ L1-L2 ]   [ alpha       1 -1 ] [ I  ]
// [ L1-P1 ] = [ -2          1  0 ] [ B1 ]
// [ L2-P2 ]   [ -2(alpha+1) 0  1 ] [ B2 ]
//
// this partials matrix has inverse
// [  I ]   1 [   -1        1     -1  ] [ L1-L2 ]
// [ B1 ] = - [   -2     (a+2)    -2  ] [ L1-P1 ]
// [ B2 ]   a [ (-2a-2) (2a+2) (-a-2) ] [ L2-P2 ]
//
// the partials matrix implies that the meas.cov. matrix is Cov(M) = H*Cov(L1,etc)*H^T
// [ L1^2+L2^2  L1^2      -L2^2      ]
// [ L1^2       L1^2+P1^2  0         ]
// [ -L2^2      0          L2^2+P2^2 ]
//
// note that once you have the biases, you can construct the debiased phase
// Ldb = L - B and smoothed range  Psm = D*Ldb = Iono-free phase = Iono-free range
// = [(alpha+1)P1-P2]/alpha = [(alpha+1)L1-L2]/alpha

//------------------------------------------------------------------------------------
// system
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
// gpstk
#include "GNSSconstants.hpp"
#include "StringUtils.hpp"
#include "Matrix.hpp"
// geomatics
#include "SRIFilter.hpp"
#include "logstream.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   int i;

   ConfigureLOG::ReportingLevel() = ConfigureLOG::Level("DEBUG");
   //ConfigureLOG::ReportLevels() = true;
   //ConfigureLOG::ReportTimeTags() = true;

   // here is the data
   #include "tkalm.hpp"
   // const int M=734; double data[5*M] = { };
   //double sow[M],L1[M],L2[M],P1[M],P2[M];
   //for(i=0; i<M; i++) {
      //sow[i] = data[5*i];
      //L1[i] = data[5*i+1];
      //L2[i] = data[5*i+2];
      //P1[i] = data[5*i+3];
      //P2[i] = data[5*i+4];
      //cout << " " << setw(3) << i << fixed << setprecision(3)
      //     << " " << setw(6) << data[5*i]
      //     << " L1=" << setw(13) << data[5*i+1]
      //     << " L2=" << setw(13) << data[5*i+2]
      //     << " P1=" << setw(13) << data[5*i+3]
      //     << " P2=" << setw(13) << data[5*i+4]
      //     << endl;
   //}

   const double wl1=L1_WAVELENGTH_GPS;
   const double wl2=L2_WAVELENGTH_GPS;
   const double F1=L1_MULT_GPS;
   const double F2=L2_MULT_GPS;
   const double F1F2=(F1/F2)*(F1/F2);
   const double alpha=(F1F2 - 1.0);
   double big,small;

   // these are arbitrary biases added to L1 and L2, for testing
   const double bias1=0.0,bias2=0.0;

   const double sigP1=3.0,sigP2=3.0,sigL1=0.01,sigL2=0.01;
   Matrix<double> H(3,3,0.0),CM(3,3),PhiInv(3,3),Rw(3,3,0.0),G(3,3),Rwx(3,3),Cov(3,3);
   Vector<double> D(3),Dsave(3),Zw(3),X(3);
   Namelist NL;
   map<int, Matrix<double> > RwStore, RwxStore;
   map<int, Vector<double> > ZwStore, DStore;

   // build the state namelist
   NL += "IONO"; NL += "BIAS_L1"; NL += "BIAS_L2";
   SRIFilter SRIF(NL);

   // partials matrix (const) : state is [I, B1, B2], data is [L1-L2, L1-P1, L2-P2]
   H(0,0)=alpha;         H(0,1)=1;  H(0,2)=-1;
   H(1,0)=-2;            H(1,1)=1;  H(1,2)=0;
   H(2,0)=-2*(alpha+1);  H(2,1)=0;  H(2,2)=1;

   // just for fun
   Matrix<double> Hinv(3,3);
   Hinv(0,0) = -1.0/alpha; Hinv(0,1) = 1.0/alpha; Hinv(0,2) = -1.0/alpha;
   Hinv(1,0) = -2.0/alpha; Hinv(1,1) = (alpha+2.0)/alpha; Hinv(1,2) = -2.0/alpha;
   Hinv(2,0) = -2.0*(alpha+1)/alpha; Hinv(2,1) = -Hinv(2,0);
   Hinv(2,2) = -(alpha+2)/alpha;
   LOG(INFO) << " H times Hinv\n" << H * Hinv;

   // measurement covariance - ought to multiply elevation angle dependent factor
   CM(0,0) = sigL1*sigL1+sigL2*sigL2;
   CM(0,1) = CM(1,0)=sigL1*sigL1;
   CM(0,2) = CM(2,0)=-sigL2*sigL2;
   CM(1,1) = sigP1*sigP1+sigL1*sigL1;
   CM(2,2) = sigP2*sigP2+sigL2*sigL2;
   CM(1,2) = CM(2,1) = 0.0;
   LOG(INFO) << " Measurement covariance\n" << CM;

   Matrix<double> InvMC(inverse(CM));
   LOG(INFO) << " Inverse measurement covariance\n" << InvMC;
   LOG(INFO) << " MC * inv\n" << CM * InvMC;

   int n;
   for(n=0,i=0; i<M; i++) {                // loop over data in the pass

      n++;
      //if(n==1) {  // initial estimate - high uncertainty
      //   ident(Cov);
      //   Cov(0,0) = 100.0;
      //   Cov(1,1) = 0.0001;
      //   X(0) = 0.0; X(1) = 4.0; X(2) = -2.0;
      //   SRIF.addAPriori(Cov,X);
      //}

      // fill the data vector
      D(0) = wl1*data[5*i+1]+bias1 - wl2*data[5*i+2]-bias2;      // L1-L2
      D(1) = wl1*data[5*i+1]+bias1 - data[5*i+3];                // L1-P1
      D(2) = wl2*data[5*i+2]+bias2 - data[5*i+4];                // L2-P2
      Dsave = D;

      //// output raw data
      //LOG(INFO) << "DAT " << setw(3) << n
      //   << fixed << setprecision(4)
      //   << " " << setw(8) << D(0)
      //   << " " << setw(8) << D(1)
      //   << " " << setw(8) << D(2);

      // MU - D will be replaced with residuals
      SRIF.measurementUpdate(H, D, CM);

      // solution after MU
      SRIF.getStateAndCovariance(X,Cov,&small,&big);

      // output after MU
      LOG(INFO) << "KMU " << setw(3) << n
         << fixed << setprecision(4)
         << " " << setw(9) << big/small
         << " " << setw(7) << X(0)
         << " " << setw(7) << X(1)
         << " " << setw(7) << X(2)
         << " " << setw(8) << sqrt(Cov(0,0))
         << " " << setw(7) << sqrt(Cov(1,1))
         << " " << setw(7) << sqrt(Cov(2,2))
         << " " << setw(7) << D(0)                // residuals
         << " " << setw(7) << D(1)
         << " " << setw(7) << D(2)
         ;

      ident(PhiInv);    // inverse state transition - trivial
      ident(G);         // noise to state - trivial
      // process noise - NB Rw is an information matrix = inverse sqrt p.n. cov.
      // I must be bumped relative to biases
      Rw = 0.0;
      Rw(0,0) = 1.e-2;       // I
      Rw(1,1) = 1.0/sigL1;   // bias 1
      Rw(2,2) = 1.0/sigL2;   // bias 2
      Zw = 0.0;              // process noise state - yes this is required

      // TU
      SRIF.timeUpdate(PhiInv, Rw, G, Zw, Rwx);

      // save for smoother
      RwStore[n] = Rw;
      RwxStore[n] = Rwx;
      ZwStore[n] = Zw;
      DStore[n] = Dsave;

      // solution after TU
      SRIF.getStateAndCovariance(X,Cov,&small,&big);
      D = Dsave;
      D = D - H * X;         // compute residuals
      LOG(INFO) << "KTU " << setw(3) << n
         << fixed << setprecision(4)
         << " " << setw(9) << big/small
         << " " << setw(7) << X(0)
         << " " << setw(7) << X(1)
         << " " << setw(7) << X(2)
         << " " << setw(8) << sqrt(Cov(0,0))
         << " " << setw(7) << sqrt(Cov(1,1))
         << " " << setw(7) << sqrt(Cov(2,2))
         << " " << setw(7) << D(0)
         << " " << setw(7) << D(1)
         << " " << setw(7) << D(2)
         ;

   }  // end loop over data

   // Smooth ----------------------
   // get final solution = apriori for smoother
   SRIF.getStateAndCovariance(X,Cov,&small,&big);
   big=0.0;

   // loop over the stored smoother data in reverse order
   for(n=RwStore.size(); n>0; n--) {
      Rw = RwStore[n];
      Rwx = RwxStore[n];
      Zw = ZwStore[n];
      ident(PhiInv);
      ident(G);

      // could uses the SRIS but the DM smoother is faster and simpler
      //SRIF.smootherUpdate(Phi,Rw,G,Zw,Rwx);   // Phi not PhiInv
      //SRIF.getStateAndCovariance(X,Cov,&small,&big);
      //
      SRIF.DMsmootherUpdate(Cov,X,PhiInv,Rw,G,Zw,Rwx);

      // output after SU
      D = DStore[n];
      D = D - H * X;         // compute residuals
      LOG(INFO) << "KSU " << setw(3) << n
         << fixed << setprecision(4)
         << " " << setw(8) << big/small
         << " " << setw(7) << X(0)
         << " " << setw(7) << X(1)
         << " " << setw(7) << X(2)
         << " " << setw(8) << sqrt(Cov(0,0))
         << " " << setw(7) << sqrt(Cov(1,1))
         << " " << setw(7) << sqrt(Cov(2,2))
         << " " << setw(7) << D(0)
         << " " << setw(7) << D(1)
         << " " << setw(7) << D(2)
         ;
   }
   
   return 0;
}
catch(Exception& e) { cout << "Exception: " << e << endl; }
}
