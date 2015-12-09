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

#include <iostream>

#include "Matrix.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"

using namespace std;


double max(const gpstk::Matrix<double>& A)
{
   double m=0;
   for(int i = 0; i < A.rows(); i++)
   {
      for(int j = 0; j < A.cols(); j++)
      {
         double mag=std::abs(A(i,j));
         if (mag > m)
            m = mag;
      }
   }
   return m;
}

double max(const gpstk::Vector<double>& V)
{
   double m=0;
   for(int i = 0; i < V.size(); i++)
   {
      double mag=std::abs(V(i));
      if (mag > m)
         m = mag;
   }
   return m;
}

void choleskyTest(size_t r, size_t c,
                  double _A[], double _B[], double _BSref[],
                  TestUtil& testFramework, const std::string& str)
{
   testFramework.changeSourceMethod(str);
   gpstk::Matrix<double> A(r,c);
   A = _A;
   gpstk::Cholesky<double> C;
   C(A);
   gpstk::Vector<double> B(r), BSref(r);
   B = _B;
   BSref = _BSref;
   C.backSub(B);
   
   gpstk::Matrix<double> LLt(r,c), UUt(r,c);
   LLt = C.L * transpose(C.L);
   UUt = C.U * transpose(C.U);

   double eps=10*DBL_EPSILON;
   testFramework.assert( max(A-LLt) < eps, "L * transpose(L) != A", __LINE__);
   testFramework.assert( max(A-UUt) < eps, "U * transpose(U) != A", __LINE__);
   testFramework.assert( max(B-BSref) < eps, "Back subsitution incorrect", __LINE__);
   if (false)
      cout << "A-LLt:" << A - LLt << endl
           << "A-UUt:" << A - UUt << endl
           << "B-BSref:" << B-BSref << endl;
}

void choleskyCroutTest(size_t r, size_t c,
                  double _A[], double _B[], double _BSref[],
                  TestUtil& testFramework, const std::string& str)
{
   testFramework.changeSourceMethod(str);
   gpstk::Matrix<double> A(r,c);
   A = _A;
   gpstk::CholeskyCrout<double> C;
   C(A);
   gpstk::Vector<double> B(r), BSref(r);
   B = _B;
   BSref = _BSref;
   C.backSub(B);
   
   gpstk::Matrix<double> LLt(r,c), UtU(r,c);
   LLt = C.L * transpose(C.L);
   UtU = transpose(C.U) * C.U;

   double eps=10*DBL_EPSILON;
   testFramework.assert( max(A-LLt) < eps, "L * transpose(L) != A", __LINE__);
   testFramework.assert( max(A-UtU) < eps, "transpose(U) * U != A", __LINE__);
   testFramework.assert( max(B-BSref) < eps, "Back subsitution incorrect", __LINE__);
   if (false)
      cout << "A-LLt:" << A - LLt << endl
           << "A-UtU:" << A - UtU << endl
           << "B-BSref:" << B-BSref << endl;
}

int main()
{
   double a22[4] = {2,1,1,2};
   double b2[2] = {1,2};
   double bs2[2] = {0,1};

   double a33[9] = {2,-1,0,-1,2,-1,0,-1,2};
   double b3[3] = {7,-3,2};
   double bs3[3] = {4.25,1.5,1.75};

   double a44[16] = {2,-1,0,0,-1,2,-1,0,0,-1,2,-1,0,0,-1,2};
   double b4[4] = {5,1,-2,6};
   double bs4[4] ={5,5,4,5}; 
   
   TestUtil testFramework("Matrix Cholesky", "--", __FILE__, __LINE__);
   choleskyTest(2, 2, a22, b2, bs2, testFramework, "2x2");
   choleskyTest(3, 3, a33, b3, bs3, testFramework, "3x3");
   choleskyTest(4, 4, a44, b4, bs4, testFramework, "4x4");

   TestUtil testFramework2("Matrix CholeskyCrout", "--", __FILE__, __LINE__);
   choleskyCroutTest(2, 2, a22, b2, bs2, testFramework2, "2x2");
   choleskyCroutTest(3, 3, a33, b3, bs3, testFramework2, "3x3");
   choleskyCroutTest(4, 4, a44, b4, bs4, testFramework2, "4x4");
   
   std::cout << "Total Failures for " << __FILE__ << ": " << testFramework.countFails() << std::endl;

   return testFramework.countFails();
}
