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
 * @file tsrifsu.cpp
 * Test the smoother updates in class SRIFilter
 */

#include <iostream>
#include "Exception.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "SRIFilter.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char **argv)
{
try {
   int test=1;        // 1 for SRIFSU, 2 for HH, 3 for SRISDM
   int n=3,ns=3;
   //ns = 2;

   if(argc > 1) test=atoi(argv[1]);

   Matrix<double> R(n,n,0.0);
   R(0,0) = -2.825;
   R(0,1) = 0.9551;
   R(1,1) = -2.824;
   R(0,2) = -0.1459;
   R(1,2) = 0.5276;
   R(2,2) = -1.930;

   Vector<double> z(n);
   z(0) = -73.28;
   z(1) = -4.581;
   z(2) = 1.567;

   Matrix<double> Phi(n,n,0.0);
   Phi(0,0) = 1.0;
   Phi(1,1) = 1.0;
   Phi(2,2) = 1.0;
   double dt=5.0;
   Phi(0,1) = dt;
   Phi(0,2) = dt*dt;
   Phi(1,2) = dt;

   Matrix<double> G(n,ns,0.0);
   G(0,0) = 1.0;
   G(1,1) = 1.0;
   if(ns > 2) G(2,2) = 1.0;
   G(0,1) = 0.05;
   if(ns > 2) G(0,2) = 0.001667;
   if(ns > 2) G(1,2) = 0.05;

   Matrix<double> Rw(ns,ns,0.0);
   Rw(0,0) = 8485.0;
   Rw(0,1) = 0.0;
   Rw(1,1) = 109.50;
   if(ns > 2) Rw(0,2) = 0.0;
   if(ns > 2) Rw(1,2) = 0.0;
   if(ns > 2) Rw(2,2) = 3.162;

   Matrix<double> Rwx(ns,n,0.0);
   Rwx(0,0) = 0.028934;
   Rwx(0,1) = 5.30428;
   Rwx(0,2) = 1.667;
   Rwx(1,0) = 1.2093;
   Rwx(1,1) = 1.384725;
   Rwx(1,2) = 2.340;
   Rwx(0,2) = -1.2039;
   Rwx(1,2) = 8.9984;
   if(ns > 2) Rwx(2,2) = 0.0023143;

   Vector<double> zw(ns);
   zw(0) = 3.28;
   zw(1) = 2.581;
   if(ns > 2) zw(2) = -1.567;

   Namelist NL,NLs;
   NL += "X"; NL += "Y"; NL += "Z";
   NLs += "Xs"; NLs += "Ys";
   if(ns > 2) NLs += "Zs";
   SRIFilter srif(R,z,NL);

   cout << "SRI before SU:\n" << setw(13) << setprecision(7) << srif << endl;

   LabelledMatrix LP(NL,Phi);
   LP.setw(13).setprecision(7);
   cout << "Phi before SU:\n" << LP << endl;

   Matrix<double> Phinv;
   Phinv = inverse(Phi);
   LabelledMatrix LPi(NL,Phinv);
   LPi.setw(13).setprecision(7);
   cout << "Phinv before SU:\n" << LPi << endl;

   LabelledMatrix LG(NL,NLs,G);
   LG.setw(13).setprecision(7);
   cout << "G before SU:\n" << LG << endl;

   LabelledMatrix LRw(NLs,NLs,Rw);
   LRw.setw(13).setprecision(7);
   cout << "Rw before SU:\n" << LRw << endl;

   LabelledMatrix LRwx(NLs,NL,Rwx);
   LRwx.setw(13).setprecision(7);
   cout << "Rwx before SU:\n" << LRwx << endl;

   LabelledVector Lzw(NL,zw);
   Lzw.setw(13).setprecision(7);
   cout << "zw before SU:\n" << Lzw << endl;

   Vector<double> X;
   Matrix<double> C;
   double small,big;
   LabelledMatrix LC(NL,NL,C);
   LabelledVector LX(NL,X);
   srif.getStateAndCovariance(X,C,&small,&big);

   LC.setw(13).setprecision(7);
   cout << "Covariance before SU:\n" << LC << endl;
   LX.setw(13).setprecision(7);
   cout << "State before SU:\n" << LX << endl;

   if(test==1) {
      cout << "\nCall smoother update\n\n";
      srif.smootherUpdate(Phi,Rw,G,zw,Rwx);
      srif.getStateAndCovariance(X,C,&small,&big);
   }
   else if(test==2) {
      cout << "\nDo the HH manually\n\n";
      //       _  (Ns)     (N)      (1) _          _                  _
      // (Ns) |  Rw+Rwx*G  Rwx*Phi  zw   |   ==>  |   Rw   Rwx   zw    |
      // (N)  |  R*G       R*Phi    z    |   ==>  |   0     R    z     | .
      //       -                        -          -                  -
      Matrix<double> Big;
      Big = ( (Rw + Rwx*G) || Rwx*Phi || zw )
         && (  R*G         || R*Phi   || z  );

      Namelist NLB=NLs|NL;
      NLB += "State";
      LabelledMatrix LB(NLs|NL,NLB,Big);
      LB.setw(13).setprecision(7);
      cout << "Composite Matrix\n" << LB << endl;

      Householder<double> HHB;
      HHB(Big);
      LabelledMatrix LBH(NLs|NL,NLB,HHB.A);
      LBH.setw(13).setprecision(7);
      cout << "Composite Matrix after HH\n" << LBH << endl;
            //                   (matrix,col index,slice=(start,length,stride))
      MatrixColSlice<double> Zslice(HHB.A,n+ns,std::slice(ns,n,1));
      //cout << "Zslice is (" << Zslice.rows() << "," << Zslice.cols() << ")\n"
      //<< Zslice << endl;
      z = Zslice.colCopy(0);
      //MatrixSlice<double> Rslice(HHB.A,ns,ns,n,n);
      //R = Matrix<double>(Rslice);
      R = Matrix<double>(HHB.A,ns,ns,n,n);
      // Householder() does this, but just in case....
      //for(int i=0; i<n; i++) for(int j=0; j<i; j++) R(i,j) = 0.0;
      srif = SRIFilter(R,z,NL);
      srif.getStateAndCovariance(X,C,&small,&big);
   }
   else if(test==3) {
      cout << "\nCall the C/X version of the smoother update\n\n";
      SRIFilter::DMsmootherUpdate(C,X,Phinv,Rw,G,zw,Rwx);
      Cholesky<double> Ch;
      Ch(C);
      R = inverse(Ch.U);
      z = R * X;
      srif = SRIFilter(R,z,NL);
   }
   cout << "Phinv after SU:\n" << LP << endl;        // note LP not LPi
   cout << "G after SU:\n" << LG << endl;
   cout << "Rw after SU:\n" << LRw << endl;
   cout << "Rwx after SU:\n" << LRwx << endl;
   cout << "zw after SU:\n" << Lzw << endl;
   cout << "SRI after SU:\n" << setw(13) << setprecision(7) << srif << endl;
   cout << "Covariance after SU:\n" << LC << endl;
   cout << "State after SU:\n" << LX << endl;

   return 0;
}
catch(Exception& e) {
   cerr << "Caught exception\n" << e << endl;
   return -1;
}
}


/* the output should be equivalent to the following. Note that many quantities
 (Phi,Phinv,G,Rw,Rwx,zw) are intermediate values and so may differ in the 3 cases.
-------------------------------------------------------------------------------
For test==1:
-------------------------------------------------------------------------------
Matrix Print (UPT:3,3): SRI Covariance R before SU:
    -2.8250000     0.9551000    -0.1459000
           (0)    -2.8240000     0.5276000
           (0)           (0)    -1.9300000
Vector (l=3,b=0,e=2): SRI State vector before SU:
   -73.2800000    -4.5810000     1.5670000
Matrix Print (SQU:3,3): Phi before SU:
     1.0000000     5.0000000    25.0000000
     0.0000000     1.0000000     5.0000000
     0.0000000     0.0000000     1.0000000
Matrix Print (SQU:3,3): Phi inverse
     1.0000000    -5.0000000     0.0000000
     0.0000000     1.0000000    -5.0000000
     0.0000000     0.0000000     1.0000000
Matrix Print (GEN:3,3): G matrix before SU:
     1.0000000     0.0500000     0.0016670
     0.0000000     1.0000000     0.0500000
     0.0000000     0.0000000     1.0000000
Matrix Print (UPT:3,3): Rw matrix before SU:
  8485.0000000     0.0000000     0.0000000
           (0)   109.5000000     0.0000000
           (0)           (0)     3.1620000
Matrix Print (SYM:3,3): Covariance before SU:
     0.1396719     0.0429714     0.0030922
     0.0429714     0.1347628     0.0501563
     0.0030922     0.0501563     0.2684636
Vector (l=3,b=0,e=2): State X before SU:
    26.4789073     1.4704789    -0.8119171

Call SrifSU

Matrix Print (SQU:3,3): Phi inverse after SU:
     2.8308464    13.2002762    66.1692641
     0.0186639     2.7399474    13.1207326
     0.0370416    -0.1270013     1.6464965
Matrix Print (GEN:3,3): G after SU:
    -2.8250000     0.8156191    -0.1697817
     0.0000000    -2.8240000     0.6170483
     0.0000000     0.0000000    -1.9300000
Matrix Print (UPT:3,3): Rw after SU:
  8485.0000000     0.0000000     0.0000000
           (0)   109.5000000     0.0000000
           (0)           (0)     3.1620000
Matrix Print (GEN:3,3): Rwx after SU:
    -0.0300469    -5.4543935   -26.0693988
    -1.1881107    -7.4029263   -45.9955463
    -0.1329200    -0.1645391    -1.9486389
Vector (l=3,b=0,e=2): zw vector after SU:
    -3.3047654    -2.1576620    -0.4445507
Matrix Print (UPT:3,3): SRI Covariance R after SU:
     2.8308464    13.2002762    66.1692641
           (0)     2.7399474    13.1207326
           (0)           (0)     1.6464965
Vector (l=3,b=0,e=2): SRI State vector after SU:
    73.3032284     4.8851499    -0.5218388
Matrix Print (SYM:3,3): Covariance after SU:
     3.4237044     1.2242346    -0.3853596
     1.2242346     8.5920292    -1.7664210
    -0.3853596    -1.7664210     0.3688743
Vector (l=3,b=0,e=2): State X after SU:
    17.9117060     3.3006549    -0.3169389
-------------------------------------------------------------------------------
For test==2:
-------------------------------------------------------------------------------
Matrix Print (UPT:3,3): SRI Covariance R before SU:
    -2.8250000     0.9551000    -0.1459000
           (0)    -2.8240000     0.5276000
           (0)           (0)    -1.9300000
Vector (l=3,b=0,e=2): SRI State vector before SU:
   -73.2800000    -4.5810000     1.5670000
Matrix Print (SQU:3,3): Phi before SU:
     1.0000000     5.0000000    25.0000000
     0.0000000     1.0000000     5.0000000
     0.0000000     0.0000000     1.0000000
Matrix Print (SQU:3,3): Phi inverse
     1.0000000    -5.0000000     0.0000000
     0.0000000     1.0000000    -5.0000000
     0.0000000     0.0000000     1.0000000
Matrix Print (GEN:3,3): G matrix before SU:
     1.0000000     0.0500000     0.0016670
     0.0000000     1.0000000     0.0500000
     0.0000000     0.0000000     1.0000000
Matrix Print (UPT:3,3): Rw matrix before SU:
  8485.0000000     0.0000000     0.0000000
           (0)   109.5000000     0.0000000
           (0)           (0)     3.1620000
Matrix Print (SYM:3,3): Covariance before SU:
     0.1396719     0.0429714     0.0030922
     0.0429714     0.1347628     0.0501563
     0.0030922     0.0501563     0.2684636
Vector (l=3,b=0,e=2): State X before SU:
    26.4789073     1.4704789    -0.8119171

Do the HH manually

Do the SU manually:
Matrix Print (GEN:6,7): Composite matrix
  8485.0289340     5.3057267    -0.9386378     0.0289340     5.4489500    26.0408500     3.2800000
     1.2093000   110.9451900     9.0696522     1.2093000     7.4312250    46.1545250     2.5810000
     0.0000000     0.0000000     3.1643143     0.0000000     0.0000000     0.0023143    -1.5670000
    -2.8250000     0.8138500    -0.1028543    -2.8250000   -13.1699000   -65.9954000   -73.2800000
     0.0000000    -2.8240000     0.3864000     0.0000000    -2.8240000   -13.5924000    -4.5810000
     0.0000000     0.0000000    -1.9300000     0.0000000     0.0000000    -1.9300000     1.5670000

Do HouseHolder

Matrix Print (GEN:6,7): Composite matrix after HH
 -8485.0294905    -5.3212675     0.9373108    -0.0300469    -5.4543935   -26.0693988    -3.3047654
     1.2093000  -110.9833652    -9.0560140    -1.1881107    -7.4029263   -45.9955463    -2.1576620
     0.0000000     0.0000000    -3.7612976    -0.1329200    -0.1645391    -1.9486389    -0.4445507
    -2.8250000     0.8156191    -0.1697817     2.8308464    13.2002762    66.1692641    73.3032284
     0.0000000    -2.8240000     0.6170483     0.0186639     2.7399474    13.1207326     4.8851499
     0.0000000     0.0000000    -1.9300000     0.0370416    -0.1270013     1.6464965    -0.5218388
Matrix Print (SQU:3,3): Phi inverse after SU:
    -2.8250000   -13.1699000   -65.9954000
     0.0000000    -2.8240000   -13.5924000
     0.0000000     0.0000000    -1.9300000
Matrix Print (GEN:3,3): G after SU:
    -2.8250000     0.8138500    -0.1028543
     0.0000000    -2.8240000     0.3864000
     0.0000000     0.0000000    -1.9300000
Matrix Print (UPT:3,3): Rw after SU:
  8485.0000000     0.0000000     0.0000000
           (0)   109.5000000     0.0000000
           (0)           (0)     3.1620000
Matrix Print (GEN:3,3): Rwx after SU:
     0.0289340     5.4489500    26.0408500
     1.2093000     7.4312250    46.1545250
     0.0000000     0.0000000     0.0023143
Vector (l=3,b=0,e=2): zw vector after SU:
     3.2800000     2.5810000    -1.5670000
Matrix Print (UPT:3,3): SRI Covariance R after SU:
     2.8308464    13.2002762    66.1692641
           (0)     2.7399474    13.1207326
           (0)           (0)     1.6464965
Vector (l=3,b=0,e=2): SRI State vector after SU:
    73.3032284     4.8851499    -0.5218388
Matrix Print (SYM:3,3): Covariance after SU:
     3.4237044     1.2242346    -0.3853596
     1.2242346     8.5920292    -1.7664210
    -0.3853596    -1.7664210     0.3688743
Vector (l=3,b=0,e=2): State X after SU:
    17.9117060     3.3006549    -0.3169389
-------------------------------------------------------------------------------
For test==3:
-------------------------------------------------------------------------------
Matrix Print (UPT:3,3): SRI Covariance R before SU:
    -2.8250000     0.9551000    -0.1459000
           (0)    -2.8240000     0.5276000
           (0)           (0)    -1.9300000
Vector (l=3,b=0,e=2): SRI State vector before SU:
   -73.2800000    -4.5810000     1.5670000
Matrix Print (SQU:3,3): Phi before SU:
     1.0000000     5.0000000    25.0000000
     0.0000000     1.0000000     5.0000000
     0.0000000     0.0000000     1.0000000
Matrix Print (SQU:3,3): Phi inverse
     1.0000000    -5.0000000     0.0000000
     0.0000000     1.0000000    -5.0000000
     0.0000000     0.0000000     1.0000000
Matrix Print (GEN:3,3): G matrix before SU:
     1.0000000     0.0500000     0.0016670
     0.0000000     1.0000000     0.0500000
     0.0000000     0.0000000     1.0000000
Matrix Print (UPT:3,3): Rw matrix before SU:
  8485.0000000     0.0000000     0.0000000
           (0)   109.5000000     0.0000000
           (0)           (0)     3.1620000
Matrix Print (SYM:3,3): Covariance before SU:
     0.1396719     0.0429714     0.0030922
     0.0429714     0.1347628     0.0501563
     0.0030922     0.0501563     0.2684636
Vector (l=3,b=0,e=2): State X before SU:
    26.4789073     1.4704789    -0.8119171

Call SrisDM

Matrix Print (SQU:3,3): Phi inverse after SU:
     1.0000000     5.0000000    25.0000000
     0.0000000     1.0000000     5.0000000
     0.0000000     0.0000000     1.0000000
Matrix Print (GEN:3,3): G after SU:
     0.0001179     0.0004566     0.0005272
     0.0000000     0.0091324     0.0158128
     0.0000000     0.0000000     0.3162555
Matrix Print (UPT:3,3): Rw after SU:
     0.0001179     0.0000000     0.0000000
           (0)     0.0091324     0.0000000
           (0)           (0)     0.3162555
Matrix Print (GEN:3,3): Rwx after SU:
     0.0289340     5.3042800    -1.2039000
     1.2093000     1.3847250     8.9984000
     0.0000000     0.0000000     0.0023143
Vector (l=3,b=0,e=2): zw vector after SU:
     3.2800000     2.5810000    -1.5670000
Matrix Print (UPT:3,3): SRI Covariance R after SU:
     2.8308464    13.2002762    66.1692641
           (0)     2.7399474    13.1207326
           (0)           (0)     1.6464965
Vector (l=3,b=0,e=2): SRI State vector after SU:
    73.3032284     4.8851499    -0.5218388
Matrix Print (SQU:3,3): Covariance after SU:
     3.4237044     1.2242346    -0.3853596
     1.2242346     8.5920292    -1.7664210
    -0.3853596    -1.7664210     0.3688743
Vector (l=3,b=0,e=2): State X after SU:
    17.9117060     3.3006549    -0.3169389
-------------------------------------------------------------------------------
*/
