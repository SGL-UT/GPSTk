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
 * @file tsriftu.cpp
 * Test the time update in class SRIFilter
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
   int N=3,Ns=3;
   Matrix<double> R(N,N,0.0);
   R(0,0) = -2.825;
   R(0,1) = 0.9551;
   R(1,1) = -2.824;
   R(0,2) = -0.1459;
   R(1,2) = 0.5276;
   R(2,2) = -1.930;

   Vector<double> Z(N);
   Z(0) = -73.28;
   Z(1) = -4.581;
   Z(2) = 1.567;

   Matrix<double> Phinv(N,N);
   Phinv = 0.0;
   Phinv(0,0) = 1.0;
   Phinv(1,1) = 1.0;
   Phinv(2,2) = 1.0;
   double dt=-0.1;
   Phinv(0,1) = dt;
   Phinv(0,2) = dt*dt;
   Phinv(1,2) = dt;

   Matrix<double> G(N,Ns);
   G = 0.0;
   G(0,0) = 1.0;
   G(1,1) = 1.0;
   G(2,2) = 1.0;
   G(0,1) = 0.05;
   G(0,2) = 0.001667;
   G(1,2) = 0.05;

   Matrix<double> Rw(Ns,Ns,0.0);
   Rw(0,0) = 8485.0;
   Rw(0,1) = 0.0;
   Rw(1,1) = 109.50;
   Rw(0,2) = 0.0;
   Rw(1,2) = 0.0;
   Rw(2,2) = 3.162;

   Matrix<double> Rwx(Ns,N,0.0);    // Rwx must be 0
   Vector<double> Zw(Ns);
   Zw(0) = 1.0;
   Zw(1) = 1.2;
   Zw(2) = 1.3;

   Namelist NL,NLs;
   NL += "X"; NL += "Y"; NL += "Z";
   NLs += "Xs"; NLs += "Ys";
   if(Ns > 2) NLs += "Zs";
   SRIFilter srif(R,Z,NL);

   cout << "SRI before TU:\n" << setw(13) << setprecision(7) << srif << endl;

   LabelledMatrix LP(NL,Phinv);
   LP.setw(13).setprecision(7);
   cout << "Phinv before TU:\n" << LP << endl;

   LabelledMatrix LG(NL,NLs,G);
   LG.setw(13).setprecision(7);
   cout << "G before TU:\n" << LG << endl;

   LabelledMatrix LRw(NLs,Rw);
   LRw.setw(13).setprecision(7);
   cout << "Rw before TU:\n" << LRw << endl;

   LabelledMatrix LRwx(NLs,NL,Rwx);
   LRwx.setw(13).setprecision(7);
   cout << "Rwx before TU:\n" << LRwx << endl;

   LabelledVector LZw(NLs,Zw);
   LZw.setw(13).setprecision(7);
   cout << "Zw before TU:\n" << LZw << endl;

   // Do the HH manually
   //       _  (Ns)        (N)        (1)  _          _                  _
   // (Ns) |    Rw          0          Zw   |   ==>  |   Rw   Rwx   Zw    |
   // (N)  |  -R*Phinv*G   R*Phinv     Z    |   ==>  |   0     R    Z     | .
   //      -                               -         -                   -

   Matrix<double> Big;
   Big = ( Rw         || Rwx     || Zw )
      && ( -R*Phinv*G || R*Phinv || Z  );

   cout << "\nCall SrifTU\n\n";
   srif.timeUpdate(Phinv,Rw,G,Zw,Rwx);

   cout << "SRI after TU:\n" << setw(13) << setprecision(7) << srif << endl;
   cout << "Phinv after TU:\n" << LP << endl;
   cout << "G after TU:\n" << LG << endl;
   cout << "Rw after TU:\n" << LRw << endl;
   cout << "Rwx after TU:\n" << LRwx << endl;
   cout << "Zw after TU:\n" << LZw << endl;

   cout << "\nNow do the TU manually:\n";
   cout << "\tIf Rd=R*Phinv,\n\t|  Rw   0  Zw |  -->  | Rw Rwx Zw |\n";
   cout << "\t| -Rd*G Rd Z  |  -->  | 0  R   Z  |\n\n";

   Namelist NLB=NLs|NL;
   NLB += "State";
   LabelledMatrix LBig(NLs|NL,NLB,Big);
   LBig.setw(13).setprecision(7);
   cout << "Composite matrix before HH:\n" << LBig << endl;

   Householder<double> HHB;
   HHB(Big);
   LabelledMatrix LBH(NLs|NL,NLB,HHB.A);
   LBH.setw(13).setprecision(7);
   cout << "Composite matrix after HH:\n" << LBH << endl;
}
catch(Exception& e) {
   cerr << "Caught exception\n" << e << endl;
   return -1;
}
}

/* the output should be:
SRI before TU:
                         X            Y            Z        State
            X   -2.8250000    0.9551000   -0.1459000  -73.2800000
            Y    0.0000000   -2.8240000    0.5276000   -4.5810000
            Z    0.0000000    0.0000000   -1.9300000    1.5670000
Phinv before TU:
                         X            Y            Z
            X    1.0000000   -0.1000000    0.0100000
            Y    0.0000000    1.0000000   -0.1000000
            Z    0.0000000    0.0000000    1.0000000
G before TU:
                        Xs           Ys           Zs
            X    1.0000000    0.0500000    0.0016670
            Y    0.0000000    1.0000000    0.0500000
            Z    0.0000000    0.0000000    1.0000000
Rw before TU:
                        Xs           Ys           Zs
           Xs 8485.0000000    0.0000000    0.0000000
           Ys    0.0000000  109.5000000    0.0000000
           Zs    0.0000000    0.0000000    3.1620000

Call SrifTU

SRI after TU:
                         X            Y            Z        State
            X    2.8203762   -1.2066130    0.2051914   73.2522809
            Y    0.0000000    2.7781779   -0.6182688    4.3616701
            Z    0.0000000    0.0000000    1.6473746   -1.3375316
Phinv after TU:
                         X            Y            Z
            X    2.8203762   -1.2066130    0.2051914
            Y   -0.0157050    2.7781779   -0.6182688
            Z    0.0442855   -0.1397111    1.6473746
G after TU:
                        Xs           Ys           Zs
            X    2.8250000   -1.0963499    0.2123923
            Y    0.0000000    2.8240000   -0.6685503
            Z    0.0000000    0.0000000    1.9300000
Rw after TU:
                        Xs           Ys           Zs
           Xs-8485.0004703    0.0003650   -0.0000707
           Ys    0.0000000 -109.5418959    0.0193684
           Zs    0.0000000    0.0000000   -3.7703069
Rwx after TU:
                         X            Y            Z
           Xs    0.0009406   -0.0004120    0.0000898
           Ys   -0.0282740    0.0851895   -0.0235808
           Zs    0.1590678   -0.5702501    1.1467160
Zw after TU:
                        Xs           Ys           Zs
                 0.0243979   -0.6153242    2.5120533

Now do the TU manually:
	If Rd=R*Phinv,
	|  Rw   0  Zw |  -->  | Rw Rwx Zw |
	| -RdG  Rd Zw |  -->  | 0  R   Z  |

Composite matrix before HH:
                        Xs           Ys           Zs            X            Y            Z        State
           Xs 8485.0000000    0.0000000    0.0000000    0.0000000    0.0000000    0.0000000    0.0000000
           Ys    0.0000000  109.5000000    0.0000000    0.0000000    0.0000000    0.0000000    0.0000000
           Zs    0.0000000    0.0000000    3.1620000    0.0000000    0.0000000    0.0000000    0.0000000
            X    2.8250000   -1.0963500    0.2124893   -2.8250000    1.2376000   -0.2696600  -73.2800000
            Y    0.0000000    2.8240000   -0.6688000    0.0000000   -2.8240000    0.8100000   -4.5810000
            Z    0.0000000    0.0000000    1.9300000    0.0000000    0.0000000   -1.9300000    1.5670000
Composite matrix after HH:
                        Xs           Ys           Zs            X            Y            Z        State
           Xs 8485.0004703   -0.0003650    0.0000707   -0.0009406    0.0004120   -0.0000898   -0.0243979
           Ys    0.0000000 -109.5418959    0.0193684   -0.0282740    0.0851895   -0.0235808   -0.6153242
           Zs    0.0000000    0.0000000    3.7703069   -0.1590678    0.5702501   -1.1467160   -2.5120533
            X    0.0000000    0.0000000    0.0000000    2.8203762   -1.2066130    0.2051914   73.2522809
            Y    0.0000000    0.0000000    0.0000000    0.0000000   -2.7781779    0.6182688   -4.3616701
            Z    0.0000000    0.0000000    0.0000000    0.0000000    0.0000000    1.6473746   -1.3375316
*/
