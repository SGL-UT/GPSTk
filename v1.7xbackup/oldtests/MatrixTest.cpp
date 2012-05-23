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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "Vector.hpp"
#include "Matrix.hpp"
#include "PolyFit.hpp"
#include "Stats.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>   // for copyfmt

/**
 * @file MatrixTest.cpp
 *
 */

using namespace std;

int ReadMatrix(gpstk::Matrix<double>& M, string& file)
{
   ifstream infile(file.c_str());
   if(!infile) return -1;

   enum Type{GEN,LOW,UPT,DIA,SYM,SQU};
   string labels[6]={"GEN","LOW","UPT","DIA","SYM","SQU"};
   bool dimmed=false;
   char ch;
   int r=0,c=0,i=0,j=0;
   string buffer;
   Type type=GEN;
   while(infile >> buffer) {
      if(buffer[0] == '#') {        // skip to end of line
         while(infile.get(ch)) { if(ch=='\n') break; }
      }
      else {
         string::size_type in;
         if((in=buffer.find("r=")) != string::npos) {
            r = strtol(buffer.substr(in+2).c_str(),0,10);
            //cout << "Found r = " << r << endl;
            if(type != GEN) c=r;
         }
         else if((in=buffer.find("c=")) != string::npos) {
            c = strtol(buffer.substr(in+2).c_str(),0,10);
            //cout << "Found c = " << c << endl;
            if(type != GEN) r=c;
         }
         else if((in=buffer.find("t=")) != string::npos) {
            if((in=buffer.find("LOW")) != string::npos) type=LOW;
            if((in=buffer.find("UPT")) != string::npos) type=UPT;
            if((in=buffer.find("DIA")) != string::npos) type=DIA;
            if((in=buffer.find("SYM")) != string::npos) type=SYM;
            if((in=buffer.find("SQU")) != string::npos) type=SQU;
            //cout << "Found type " << labels[type] << endl;
         }
         else if(buffer == string(":::")) {
            if(r*c == 0) return 0;
            M = gpstk::Matrix<double>(r,c,double(0));
            dimmed = true;
         }
         else {
            if(!dimmed) continue;
            M(i,j) = strtod(buffer.c_str(),0);
            j++;
            switch(type) {
               case LOW: if(j>i) { i++; j=0; } break;
               case UPT: if(j>=c) { i++; j=i; } break;
               case DIA: i=j; break;
               case SYM: M(j-1,i) = M(i,j-1); if(j>i) { i++; j=0; } break;
               case SQU: case GEN: default: if(j>=c) { i++; j=0; } break;
            }
         }
      }
   }
   infile.close();
   if(!dimmed) return -2;
   return 0;
}

void VectorTest(void)
{
   cout << "\n -------------- Vector Test ---------------------------------\n";
   gpstk::Vector<double> V(10);
   V += 3.1415;
   cout << "V = " << V << endl;

   double dat[10]={1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9,10.1};
   V = dat;
   cout << "V = " << V << endl;
   //cout << "Median of V " << median(V) << endl;
   cout << "V min " << min(V) << ", max " << max(V) << ", sum " << sum(V) << endl;

      // slice is (init,number,stride)
   gpstk::VectorSlice<double> Vodd(V,std::slice(0,5,2));
   gpstk::ConstVectorSlice<double> Veve(V,std::slice(1,5,2));
   cout << "Vodd = " << Vodd << endl;
   cout << "Veve = " << Veve << endl;
   Vodd[1] = Vodd[3] = 0;
   //Veve[1] = Veve[4] = 99; compiler won't let you have Veve as l-value
   V[3] = V[7] = 99;    // but you can change V, and that will show in Veve
   cout << "Vodd = " << Vodd << endl;
   cout << "Veve = " << Veve << endl;
   cout << "Minkowski of Vodd and Veve " << Minkowski(Vodd,Veve) << endl;
   cout << "dot of Vodd and Veve " << dot(Vodd,Veve) << endl;
   cout << "V    = " << V << endl;
   cout << "V min " << min(V) << ", max " << max(V) << ", sum " << sum(V) << ", norm " << norm(V) << endl;

   gpstk::Vector<double> W=V;
   V.zeroTolerance = 1.e-15;
   cout << "Zero tolerance for V is " << V.zeroTolerance << endl;
   V.zeroTolerance = 1.e-5;
   cout << "Zero tolerance for W is " << W.zeroTolerance << endl;

   W += V;
   cout << "Here is W the usual way :\n";
   cout << ' ' << fixed << setfill('.') << setw(8) << setprecision(3) << W << endl;

   cout << "Here is W the saved way :\n";
   cout << fixed << setfill('.') << setw(8) << setprecision(3);
   ofstream savefmt;
   savefmt.copyfmt(cout);
   for(int i=0; i<W.size(); i++) {
      cout.copyfmt(savefmt);
      cout << W[i];
   }
   cout << setfill(' ') << endl;

   gpstk::Vector<double> Sum;
   Sum = V + W;
   cout << "Sum = " << setw(8) << Sum << endl;

   gpstk::ConstVectorSlice<double> CVS(V,std::slice(0,V.size(),1));
   cout << "CVS = " << setw(13) << CVS << endl;
}

void MatrixTest1(int argc, char **argv)
{
   cout << "\n -------------- Matrix Test 1 ---------------------------------\n";
   gpstk::Matrix<double> MD(2,5);
   double dat[10]={1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9,10.1};
   MD = dat;
   cout << "Matrix (" << MD.rows() << "," << MD.cols() << ") from double* :\n" << fixed << setprecision(1) << setw(5) << MD << endl;

   string filename(argv[5]);
   gpstk::Matrix<double> MF;
   int iret=ReadMatrix(MF,filename);
   if(iret < 0) {
      cerr << "Error: could not open file " << filename << endl;
      return;
   }
   //cout << "Matrix (" << MF.rows() << "," << MF.cols() << ") from file :\n" << fixed << setprecision(1) << setw(7) << MF << endl;

      // pick off last column
   gpstk::Vector<double> B(MF.colCopy(MF.cols()-1));

      // copy all but last column
   gpstk::Matrix<double> A(MF,0,0,MF.rows(),MF.cols()-1);
   cout << "Partials Matrix (" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << A << endl;
   cout << "Data vector (" << B.size() << ") :\n" << fixed << setprecision(3) << setw(10) << B << endl;

      // take ATA, then invert
   gpstk::Matrix<double> AT,ATA;
   AT = transpose(A);
   //cout << "Transposed matrix (" << AT.rows() << "," << AT.cols() << ") :\n" << fixed << setprecision(1) << setw(7) << AT << endl;
   ATA = AT * A;
   //cout << "ATA matrix (" << ATA.rows() << "," << ATA.cols() << ") :\n" << fixed << setprecision(3) << setw(13) << ATA << endl;
   gpstk::Matrix<double> Ainv;
   try { Ainv=inverse(ATA); }
   catch (gpstk::Exception& e) { cout << e << endl; return;}
   cout << "Covariance matrix (" << Ainv.rows() << "," << Ainv.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << Ainv << endl;

   gpstk::Vector<double> Sol;
   Sol = Ainv * AT * B;
   cout << "Solution vector (" << Sol.size() << ") :\n" << fixed << setprecision(3) << setw(10) << Sol << endl;
   gpstk::Vector<double> Two;
   Two = B - A*Sol;
   cout << "Residual vector (" << Two.size() << ") :\n" << scientific << setprecision(3) << setw(10) << Two << endl;

   Two = Sol + Sol;
   cout << "2*Solution vector (" << Two.size() << ") :\n" << fixed << setprecision(3) << setw(10) << Two << endl;

   gpstk::Matrix<double> TA;
   TA = A + A;
   cout << "2*Partials Matrix (" << TA.rows() << "," << TA.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << TA << endl;

   int i;
   gpstk::Vector<double> c(8),b(7);
   gpstk::Matrix<double> W;
   for(i=0; i<8; i++) c(i)=3*i;
   for(i=0; i<7; i++) b(i)=i+1;
   W = outer(c,b);
   cout << "Vector c(" << c.size() << ") :\n" << fixed << setprecision(3) << setw(7) << c << endl;
   cout << "Vector b(" << b.size() << ") :\n" << fixed << setprecision(3) << setw(7) << b << endl;
   cout << "Their outer product (" << W.rows() << "," << W.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << W << endl;
   cout << "Their norms " << norm(b) << " and " << norm(c) << " and dot " << dot(c,b) << " and cosine " << cosVec(c,b) << endl;
}

void MatrixTest2(void)
{
   cout << "\n -------------- Matrix Test 2 ---------------------------------\n";
   for(int n=2; n<13; n++) {
      double big,small,cond;
      gpstk::Matrix<double> MM(n,n),Minv,Prod;
      for(int i=0; i<n; i++) {
         for(int j=0; j<n; j++) {
            MM(i,j) = 1.0/(i+j+1.0);
         }
      }
      cout << "Tough matrix (" << MM.rows() << "," << MM.cols() << ") :\n" << fixed << setprecision(6) << setw(9) << MM << endl;
      Minv = inverse(MM);
      try { Minv=inverse(MM); }
      catch (gpstk::Exception& e) { cout << e << endl; break;}
      cond = condNum(MM,big,small);
      cout << "Condition number for " << n << " is " << fixed << setprecision(3) << big << "/" << scientific << setprecision(3) << small;
      if(small > 0.0) cout << " = " << setprecision(3) << big/small;
      cout << endl;
      cout << "Inverse matrix (" << Minv.rows() << "," << Minv.cols() << ") :\n" << fixed << setprecision(3) << setw(10+(n>5?n:0)) << Minv << endl;
      Prod = Minv * MM;
      gpstk::Vector<double>V;
      V.zeroTolerance = 1.e-3;
      Prod.zeroize();
      cout << "Unity matrix (" << Prod.rows() << "," << Prod.cols() << ") ? :\n" << fixed << setprecision(9) << setw(12) << Prod << endl;
   }
}

void MatrixTest3(int argc, char **argv)
{
   cout << "\n -------------- Matrix Test 3 ---------------------------------\n";
   cout << "Read and print matrix from file\n";
   double cond,big,small;
   gpstk::Matrix<double> A,Ainv,P;
   for(int i=1; i<argc; i++) {
      string filename=argv[i];
      cout << "File " << filename;
      int iret=ReadMatrix(A,filename);
      cout << " (" << iret << ") Matrix(" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << A << endl;

      if(A.rows() != A.cols()) {
         gpstk::Matrix<double> AT;
         AT = gpstk::transpose(A);
         A = AT*A;
         cout << " ATA Matrix(" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << A << endl;
      }
      cond = condNum(A,big,small);
      cout << "Condition number is " << fixed << setprecision(3) << big << "/" << scientific << setprecision(3) << small;
      cout << " = " << fixed << big/small << endl;
      try { Ainv=gpstk::inverse(A); }
      catch (gpstk::Exception& e) { cout << e << endl; continue;}
      cout << "Inverse matrix (" << Ainv.rows() << "," << Ainv.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << Ainv << endl;
      P = Ainv * A;
      cout << "Unity matrix (" << P.rows() << "," << P.cols() << ") ? :\n" << fixed << setprecision(9) << setw(12) << P << endl;
   }
}

void MatrixTest4(void)
{
   cout << "\n -------------- Matrix Test 4 ---------------------------------\n";
   const int N=7; // rows = columns
   double mat[N*N]={       // storage by columns
      8.317,  6.212,  2.574,  5.317,  2.080, -9.133, -2.755,
      0.212,  3.292,  1.574,  1.028,  3.370, -2.077, -2.739,
      5.740,  1.574,  1.911,  1.390,  8.544,  8.930,  9.216,
      4.317,  1.028,  1.039,  7.126,  4.512,  8.538,  5.226,
      -1.109,  7.438,  7.236,  6.783,  0.356, -9.509, -0.109,
      0.174,  5.408, -9.503, -6.527, -6.589, -6.375, -7.239,
      1.960,  6.592,  9.440,  4.428, -4.531,  5.084,  4.296
   };
   double dat[N]={ 14.289,  9.284, -1.128,  8.389, -6.929,  4.664,  7.590 };

   int i,j;
   gpstk::Matrix<double> A(N,N);
   gpstk::Vector<double> b(N);
   A = mat;
   A = transpose(A);
   b = dat;

   cout << "Matrix A(" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << A << endl;
   cout << "Vector b(" << b.size() << ") :\n" << fixed << setprecision(3) << setw(7) << b << endl;
   cout << "\nNow solve using SVD\n";

   gpstk::SVD<double> Asvd;
   gpstk::Matrix<double> U,V;
   gpstk::Vector<double> S;

   Asvd(A);
   U = Asvd.U;
   V = Asvd.V;
   S = Asvd.S;
   cout << "Singular Values (" << S.size() << ") :\n" << fixed << setprecision(3) << setw(7) << S << endl;
   cout << "Matrix U(" << U.rows() << "," << U.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << U << endl;
   cout << "Matrix V(" << V.rows() << "," << V.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << V << endl;

   gpstk::Matrix<double> P,W(A);
   W = 0.0;
   for(i=0; i<S.size(); i++) W(i,i)=S(i);
   P = U * W * transpose(V);
   P = P - A;
   P.zeroize();
   cout << "Difference (" << P.rows() << "," << P.cols() << ") :\n" << scientific << setprecision(3) << setw(10) << P << endl;

   cout << "Determinant of A = " << scientific << setprecision(3) << Asvd.det();
   double d=1;
   for(i=0; i<N; i++) d *= S(i);
   cout << " -- Compare to " << scientific << setprecision(3) << d << endl;
   gpstk::Vector<double> X(b);
   Asvd.backSub(X);
   cout << "Solution via backsubstitution (" << X.size() << ") :\n" << fixed << setprecision(3) << setw(7) << X << endl;
   S = b-A*X;
   cout << "Solution residuals (" << S.size() << ") :\n" << scientific << setprecision(3) << setw(7) << S << endl;

   cout << "\nSort in ascending order\n";
   Asvd.sort(false);
   U = Asvd.U;
   V = Asvd.V;
   S = Asvd.S;
   cout << "Singular Values (" << S.size() << ") :\n" << fixed << setprecision(3) << setw(7) << S << endl;
   cout << "Matrix U(" << U.rows() << "," << U.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << U << endl;
   cout << "Matrix V(" << V.rows() << "," << V.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << V << endl;

   // now chop off the last column of A and repeat
   cout << "\n\nNow reduce A by one column and repeat\n";
   A = gpstk::Matrix<double>(A,0,0,A.rows(),A.cols()-1);
   cout << "Matrix A(" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << A << endl;
   Asvd(A);
   S = Asvd.S;
   U = Asvd.U;
   V = Asvd.V;
   cout << "Singular Values (" << S.size() << ") :\n" << fixed << setprecision(3) << setw(7) << S << endl;
   cout << "Matrix U(" << U.rows() << "," << U.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << U << endl;
   cout << "Matrix V(" << V.rows() << "," << V.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << V << endl;
   W = A;
   W = 0.0;
   for(i=0; i<S.size(); i++) W(i,i)=S(i);
   P = U * W * transpose(V);
   P = P - A;
   P.zeroize();
   cout << "Difference (" << P.rows() << "," << P.cols() << ") :\n" << scientific << setprecision(3) << setw(10) << P << endl;
}

void MatrixTest5(void)
{
   cout << "\n -------------- Matrix Test 5 ---------------------------------\n";
   const int N=7; // rows = columns
   double mat[N*N]={       // storage by columns
      8.317,  6.212,  2.574,  5.317,  2.080, -9.133, -2.755,
      0.212,  3.292,  1.574,  1.028,  3.370, -2.077, -2.739,
      5.740,  1.574,  1.911,  1.390,  8.544,  8.930,  9.216,
      4.317,  1.028,  1.039,  7.126,  4.512,  8.538,  5.226,
      -1.109,  7.438,  7.236,  6.783,  0.356, -9.509, -0.109,
      0.174,  5.408, -9.503, -6.527, -6.589, -6.375, -7.239,
      1.960,  6.592,  9.440,  4.428, -4.531,  5.084,  4.296
   };
   double dat[N]={ 14.289,  9.284, -1.128,  8.389, -6.929,  4.664,  7.590 };

   int i,j;
   gpstk::Matrix<double> A(N,N);
   gpstk::Vector<double> b(N);
   A = mat;
   A = gpstk::transpose(A);
   b = dat;

   cout << "Matrix A(" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << A << endl;
   cout << "Vector b(" << b.size() << ") :\n" << fixed << setprecision(3) << setw(7) << b << endl;
   cout << "\nNow solve using LUD\n";

   gpstk::LUDecomp<double> LUA;
   LUA(A);
   //cout << "LU: Matrix L(" << LUA.L.rows() << "," << LUA.L.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << LUA.L << endl;
   //cout << "LU: Matrix U(" << LUA.U.rows() << "," << LUA.U.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << LUA.U << endl;
   //cout << "LU: Matrix P(" << LUA.P.rows() << "," << LUA.P.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << LUA.P << endl;
   cout << "Matrix LU(" << LUA.LU.rows() << "," << LUA.LU.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << LUA.LU << endl;
   cout << "Determinant of A = " << scientific << setprecision(3) << LUA.det() << endl;

   gpstk::Vector<double> X(b),S;
   try { LUA.backSub(X); }
   catch (gpstk::Exception& e) { cout << e << endl; return;}
   cout << "Solution via backsubstitution (" << X.size() << ") :\n" << fixed << setprecision(3) << setw(7) << X << endl;
   S = b-A*X;
   cout << "Solution residuals (" << S.size() << ") :\n" << scientific << setprecision(3) << setw(7) << S << endl;
}

void MatrixTest6(int argc, char **argv)
{
   cout << "\n -------------- Matrix Test 6 ---------------------------------\n";
   string filename(argv[7]);
   gpstk::Matrix<double> A;
   int iret=ReadMatrix(A,filename);
   if(iret < 0) {
      cerr << "Error: could not open file " << filename << endl;
      return;
   }
   double dat[4]={1.,2.,3.,4.};

   gpstk::Vector<double> b(4);
   b = dat;
   cout << "Matrix A(" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << A << endl;
   cout << "Vector b(" << b.size() << ") :\n" << fixed << setprecision(3) << setw(7) << b << endl;
   cout << "\nNow compute Cholesky\n";

   gpstk::Cholesky<double> CA;
   CA(A);
   cout << "\nCholesky of A (U) (" << CA.U.rows() << "," << CA.U.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << CA.U << endl;
   cout << "\nCholesky of A (L) (" << CA.L.rows() << "," << CA.L.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << CA.L << endl;

   gpstk::Matrix<double> B;
   B = A - CA.U * gpstk::transpose(CA.U);
   cout << "\nDifference U*UT with matrix A(" << B.rows() << "," << B.cols() << ") :\n" << scientific << setprecision(3) << setw(7) << B << endl;
   B = A - CA.L * gpstk::transpose(CA.L);
   cout << "\nDifference L*LT with matrix A(" << B.rows() << "," << B.cols() << ") :\n" << scientific << setprecision(3) << setw(7) << B << endl;

   gpstk::Vector<double> X(b);
   CA.backSub(X);
   cout << "\nSolution via backsubstitution (" << X.size() << ") :\n" << fixed << setprecision(3) << setw(7) << X << endl;
   X = b-A*X;
   cout << "Solution residuals (" << X.size() << ") :\n" << scientific << setprecision(3) << setw(7) << X << endl;
}

void MatrixTest7(int argc, char **argv)
{
   cout << "\n -------------- Matrix Test 7 ---------------------------------\n";
   string filename(argv[8]);
   gpstk::Matrix<double> A;
   int iret=ReadMatrix(A,filename);
   if(iret < 0) {
      cerr << "Error: could not open file " << filename << endl;
      return;
   }
   cout << "Matrix A(" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << A << endl;
   cout << "\nNow compute the Householder transformation\n";

   gpstk::Householder<double> HHA;
   HHA(A);
   cout << "HH: (" << HHA.A.rows() << "," << HHA.A.cols() << ") :\n" << fixed << setprecision(3) << setw(7) << HHA.A << endl;
}

void MatrixTest8(void)
{
   cout << "\n -------------- Matrix Test 8 ---------------------------------\n";
   const int n=6;
	int i;
   double big,small,cond;
   gpstk::Vector<double>V;
   gpstk::Matrix<double> M(n,n),Minv,Prod;

   for(i=0; i<n; i++) {
      for(int j=0; j<n; j++) {
         M(i,j) = 1.0/(i+j+1.0);
      }
   }
   cout << "Tough matrix (" << M.rows() << "," << M.cols() << ") :\n" << fixed << setprecision(6) << setw(9) << M << endl;

   cond = gpstk::condNum(M,big,small);
   cout << "Condition number is " << fixed << setprecision(3) << big << "/" << scientific << setprecision(3) << small;
   if(small > 0.0) cout << " = " << setprecision(3) << big/small;
   cout << endl;
   
   for(i=0; i<3; i++) {
      try {
         if(i==0) { cout << "Gaussian elimiation:\n"; Minv = inverse(M); }
         else if(i==1) { cout << "LUD:\n"; Minv = inverseLUD(M); }
         else if(i==2) { cout << "SVD:\n"; Minv = inverseSVD(M); }
      }
      catch (gpstk::Exception& e) { cout << e << endl; continue;}
      cout << "Inverse matrix (" << Minv.rows() << "," << Minv.cols() << ") :\n" << fixed << setprecision(3) << setw(13) << Minv << endl;
      Prod = Minv * M;
      //V.zeroTolerance = 1.e-6;
      Prod.zeroize();
      cout << "Unity matrix ? (" << Prod.rows() << "," << Prod.cols() << ") ? :\n" << fixed << setprecision(9) << setw(12) << Prod << endl;
   }
}

void MatrixTest9(int argc, char **argv)
{
   cout << "\n -------------- Matrix Test 9 ---------------------------------\n";
   cout << "Read matrix and vector (in form M||V) from file, invert and solve\n";
   double cond,big,small;
   gpstk::Matrix<double> R,A,Ainv,P;
   gpstk::Vector<double> B,X;
   for(int i=1; i<argc; i++) {
      string filename=argv[i];
      cout << "File " << filename;
      int iret=ReadMatrix(R,filename);
      cout << " (" << iret << ") Matrix(" << R.rows() << "," << R.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << R << endl;
         // pick off last column
      B = gpstk::Vector<double>(R.colCopy(R.cols()-1));
         // copy all but last column
      A = gpstk::Matrix<double>(R,0,0,R.rows(),R.cols()-1);
      cout << "Partials Matrix (" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << A << endl;
      cout << "Data vector (" << B.size() << ") :\n" << fixed << setprecision(3) << setw(10) << B << endl << endl;

      if(A.rows() != A.cols()) {
         gpstk::Matrix<double> AT;
         AT = gpstk::transpose(A);
         A = AT*A;
         cout << " ATA Matrix(" << A.rows() << "," << A.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << A << endl;
      }
      cond = gpstk::condNum(A,big,small);
      cout << "Condition number is " << fixed << setprecision(3) << big << "/" << scientific << setprecision(3) << small;
      cout << " = " << fixed << big/small << endl;
      try 
      {
         Ainv=gpstk::inverse(A); 
         cout << "Inverse matrix (" << Ainv.rows() << "," << Ainv.cols() << ") :\n" << fixed << setprecision(3) << setw(10) << Ainv << endl;
         P = Ainv * A;
         cout << "Unity matrix (" << P.rows() << "," << P.cols() << ") ? :\n" << fixed << setprecision(9) << setw(12) << P << endl << endl;
         
         //X = Ainv * B;
         //cout << "Solution vector (" << X.size() << ") :\n" << fixed << setprecision(3) << setw(10) << X << endl;
      }
      catch (gpstk::Exception& e) { cout << e << endl; continue;}
   }
}

void PolyTest(void)
{
   cout << "\n -------------- Poly Test ---------------------------------\n";
   /*  33 points in each of two fits:
    * timetag      t     data    fit  resid
    * 351569.981 -1.0000 -1.750 -1.755  0.005
    * 351580.003 -0.9375 -1.702 -1.713  0.011
    * 351590.026 -0.8750 -1.663 -1.672  0.009
    * 351599.962 -0.8125 -1.639 -1.633 -0.006
    * 351609.984 -0.7500 -1.590 -1.595  0.005
    * 351620.006 -0.6875 -1.579 -1.558 -0.020
    * 351630.029 -0.6250 -1.536 -1.523 -0.013
    * 351639.965 -0.5625 -1.502 -1.489 -0.013
    * 351649.987 -0.5000 -1.441 -1.457  0.015
    * 351660.010 -0.4375 -1.445 -1.426 -0.019
    * 351670.032 -0.3750 -1.412 -1.396 -0.016
    * 351679.968 -0.3125 -1.370 -1.368 -0.002
    * 351689.990 -0.2500 -1.328 -1.340  0.012
    * 351700.013 -0.1875 -1.286 -1.315  0.029
    * 351710.035 -0.1250 -1.266 -1.290  0.025
    * 351719.971 -0.0625 -1.270 -1.267 -0.003
    * 351729.994  0.0000 -1.249 -1.246 -0.003
    * 351740.016  0.0625 -1.235 -1.226 -0.010
    * 351750.038  0.1250 -1.197 -1.207  0.010
    * 351759.974  0.1875 -1.214 -1.189 -0.025
    * 351769.997  0.2500 -1.183 -1.173 -0.010
    * 351780.019  0.3125 -1.156 -1.158  0.003
    * 351790.042  0.3750 -1.135 -1.145  0.010
    * 351799.978  0.4375 -1.134 -1.133 -0.001
    * 351810.000  0.5000 -1.098 -1.122  0.024
    * 351820.022  0.5625 -1.102 -1.113  0.011
    * 351829.958  0.6250 -1.118 -1.105 -0.013
    * 351839.981  0.6875 -1.103 -1.098 -0.005
    * 351850.003  0.7500 -1.115 -1.093 -0.022
    * 351860.026  0.8125 -1.076 -1.089  0.013
    * 351869.962  0.8750 -1.101 -1.086 -0.015
    * 351879.984  0.9375 -1.068 -1.085  0.017
    * 351890.006  1.0000 -1.088 -1.085 -0.003
    * timetag      t     data    fit  resid
    * 351900.029 -1.0000 -1.088 -1.029 -0.060
    * 351909.965 -0.9375 -1.119 -1.068 -0.051
    * 351919.987 -0.8750 -1.118 -1.105 -0.013
    * 351930.010 -0.8125 -1.158 -1.140 -0.018
    * 351940.032 -0.7500 -1.146 -1.174  0.028
    * 351949.968 -0.6875 -1.201 -1.206  0.004
    * 351959.990 -0.6250 -1.207 -1.235  0.029
    * 351970.013 -0.5625 -1.222 -1.264  0.041
    * 351980.035 -0.5000 -1.230 -1.290  0.059
    * 351989.971 -0.4375 -1.269 -1.314  0.045
    * 351999.994 -0.3750 -1.269 -1.337  0.068
    * 352010.016 -0.3125 -1.335 -1.358  0.022
    * 352020.038 -0.2500 -1.359 -1.377  0.018
    * 352029.974 -0.1875 -1.391 -1.394  0.003
    * 352039.997 -0.1250 -1.391 -1.410  0.019
    * 352050.019 -0.0625 -1.432 -1.423 -0.009
    * 352060.042  0.0000 -1.440 -1.435 -0.004
    * 352069.978  0.0625 -1.483 -1.445 -0.038
    * 352080.000  0.1250 -1.520 -1.453 -0.067
    * 352090.022  0.1875 -1.514 -1.460 -0.055
    * 352099.958  0.2500 -1.514 -1.464 -0.050
    * 352109.981  0.3125 -1.478 -1.467 -0.010
    * 352120.003  0.3750 -1.496 -1.468 -0.028
    * 352130.026  0.4375 -1.508 -1.467 -0.040
    * 352139.962  0.5000 -1.507 -1.465 -0.043
    * 352149.984  0.5625 -1.437 -1.460  0.023
    * 352160.006  0.6250 -1.439 -1.454  0.015
    * 352170.029  0.6875 -1.419 -1.446  0.027
    * 352179.965  0.7500 -1.388 -1.436  0.048
    * 352189.987  0.8125 -1.408 -1.424  0.016
    * 352200.010  0.8750 -1.390 -1.411  0.021
    * 352210.032  0.9375 -1.391 -1.395  0.005
    * 352219.968  1.0000 -1.385 -1.378 -0.007
    */
   double t[33] = { -1.0000, -0.9375, -0.8750, -0.8125, -0.7500, -0.6875,
      -0.6250, -0.5625, -0.5000, -0.4375, -0.3750, -0.3125, -0.2500, -0.1875,
      -0.1250, -0.0625, 0.0000, 0.0625, 0.1250, 0.1875, 0.2500, 0.3125, 0.3750,
      0.4375, 0.5000, 0.5625, 0.6250, 0.6875, 0.7500, 0.8125, 0.8750, 0.9375,
      1.0000 };
   double d1[33] = { -1.750, -1.702, -1.663, -1.639, -1.590, -1.579, -1.536,
      -1.502, -1.441, -1.445, -1.412, -1.370, -1.328, -1.286, -1.266, -1.270,
      -1.249, -1.235, -1.197, -1.214, -1.183, -1.156, -1.135, -1.134, -1.098,
      -1.102, -1.118, -1.103, -1.115, -1.076, -1.101, -1.068, -1.088 };
   double d2[33] = { -1.088, -1.119, -1.118, -1.158, -1.146, -1.201, -1.207,
      -1.222, -1.230, -1.269, -1.269, -1.335, -1.359, -1.391, -1.391, -1.432,
      -1.440, -1.483, -1.520, -1.514, -1.514, -1.478, -1.496, -1.508, -1.507,
      -1.437, -1.439, -1.419, -1.388, -1.408, -1.390, -1.391, -1.385 };

   gpstk::Vector<double> T(33),D1(33),D2(33);
   T = t;
   D1 = d1;
   D2 = d2;
   //cout << "Vector T(" << T.size() << ") :\n" << fixed << setprecision(3) << setw(7) << T << endl;
   //cout << "Vector D1(" << D1.size() << ") :\n" << fixed << setprecision(3) << setw(7) << D1 << endl;
   //cout << "Vector D2(" << D2.size() << ") :\n" << fixed << setprecision(3) << setw(7) << D2 << endl;

   size_t i,rev=1;
   gpstk::Vector<double> Fit,Resid,Sol;
   gpstk::PolyFit<double> PF(3);
   gpstk::Stats<double> St1,St2;
   gpstk::TwoSampleStats<double> Tss;
   do {
      if(rev==1) PF.Add(D1,T);
      else for(i=0; i<T.size(); i++) PF.Add(D2[i],T[i]);
      gpstk::Matrix<double> C;
      C = PF.Covariance();
      cout << "Matrix Cov(" << C.rows() << "," << C.cols() << ") :\n" << fixed << setprecision(3) << setw(8) << C << endl;
      Sol = PF.Solution();
      cout << "Vector Sol(" << Sol.size() << ") :\n" << fixed << setprecision(3) << setw(8) << Sol << endl;
      Fit = PF.Evaluate(T);
      Resid = (rev==1?D1:D2)-Fit;

      cout << "    t     data    fit  resid\n";
      for(i=0; i<T.size(); i++) {
         cout << setw(2) << i;
         cout << fixed << setw(8) << setprecision(4) << T(i);
         cout << fixed << setw(8) << setprecision(3) << (rev==1?D1(i):D2(i));
         cout << fixed << setw(8) << setprecision(3) << Fit(i);
         cout << fixed << setw(8) << setprecision(3) << Resid(i);
         cout << endl;
         St1.Add(Resid(i));
         St2.Add(rev==1?D1(i):D2(i));
         Tss.Add(T(i),(rev==1?D1(i):D2(i)));
      }

      cout << "Stats on residuals\n";
      cout << scientific << setw(10) << setprecision(3) << St1 << endl;

      cout << "Stats on data\n";
      cout << scientific << setw(10) << setprecision(3) << St2 << endl;

      cout << "2-sample Stats on time,data\n";
      cout << scientific << setw(10) << setprecision(3) << Tss << endl;

      if(++rev>2) break;
      PF.Reset();
      St1.Reset();
      St2.Reset();
      Tss.Reset();
   } while(1);
}

int main(int argc, char **argv)
{
   if (argc!=10)
   {
      cout << " Need 9 files to chew on" << endl;
      exit(-1);
   }

   VectorTest();
   MatrixTest1(argc, argv);    // general stuff
   MatrixTest2();    // condition number and inverse
   MatrixTest3(argc, argv);      // Read and condition number
   MatrixTest4();    // SVD
   MatrixTest5();    // LUD
   MatrixTest6(argc, argv);    // Cholesky
   MatrixTest7(argc, argv);    // Householder
   MatrixTest8();    // Inverse via Gauss,LUD,SVD
   MatrixTest9(argc, argv);    // Read a matrix and data vector and solve Ax=b
   PolyTest();       // PolyFit
	return 0;
}

