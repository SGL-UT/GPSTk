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

void choleskyTest(size_t r, size_t c,
                  double xA[], double xB[], double xBSref[],
                  gpstk::TestUtil& testFramework, const std::string& str)
{
   testFramework.changeSourceMethod(str);
   double eps=5*DBL_EPSILON;
   gpstk::Matrix<double> A(r,c);
   A = xA;
   gpstk::Cholesky<double> C;
   C(A);

   TUASSERTFEPS( A, C.L * transpose(C.L), eps);
   TUASSERTFEPS( A, C.U * transpose(C.U), eps);
   
   gpstk::Vector<double> B(r), BSref(r);
   B = xB;
   BSref = xBSref;
   C.backSub(B);
   TUASSERTFEPS( B, BSref, eps);   
}


void choleskyCroutTest(size_t r, size_t c,
                  double xA[], double xB[], double xBSref[],
                  gpstk::TestUtil& testFramework, const std::string& str)
{
   testFramework.changeSourceMethod(str);
   double eps=5*DBL_EPSILON;
   gpstk::Matrix<double> A(r,c);
   A = xA;
   gpstk::CholeskyCrout<double> C;
   C(A);
   
   TUASSERTFEPS( A, C.L * transpose(C.L), eps);
   TUASSERTFEPS( A, transpose(C.U) * C.U, eps);
   
   gpstk::Vector<double> B(r), BSref(r);
   B = xB;
   BSref = xBSref;
   C.backSub(B);
   TUASSERTFEPS( B, BSref, eps);
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
   
   gpstk::TestUtil testFramework("Matrix Cholesky", "--", __FILE__, __LINE__);
   choleskyTest(2, 2, a22, b2, bs2, testFramework, "2x2");
   choleskyTest(3, 3, a33, b3, bs3, testFramework, "3x3");
   choleskyTest(4, 4, a44, b4, bs4, testFramework, "4x4");

   gpstk::TestUtil testFramework2("Matrix CholeskyCrout", "--", __FILE__, __LINE__);
   choleskyCroutTest(2, 2, a22, b2, bs2, testFramework2, "2x2");
   choleskyCroutTest(3, 3, a33, b3, bs3, testFramework2, "3x3");
   choleskyCroutTest(4, 4, a44, b4, bs4, testFramework2, "4x4");

   unsigned tf = testFramework.countFails() + testFramework2.countFails();
   std::cout << "Total Failures for " << __FILE__ << ": " << tf << std::endl;

   return tf;
}
