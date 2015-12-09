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

void SVDTest(size_t r, size_t c,
                  double xA[], double xB[], double xBSref[],
                  TestUtil& testFramework, const std::string& str)
{
   testFramework.changeSourceMethod(str);
   double eps=100*DBL_EPSILON;
   gpstk::Matrix<double> A(r,c);
   A = xA;
   gpstk::SVD<double> svd;
   svd(A);
   gpstk::Matrix<double> S(r, c, 0.0);
   for (int i=0; i<r; i++)
      S(i,i) = svd.S(i);
   TUASSERTFEPS( A, svd.U * S * transpose(svd.V), eps);
   
   TUASSERTFEPS( gpstk::ident<double>(r), svd.U * transpose(svd.U), eps);
   TUASSERTFEPS( gpstk::ident<double>(r), svd.U * transpose(svd.U), eps);

   if (r == c)
   {
      gpstk::Vector<double> B(r), BSref(r);
      B = xB;
      BSref = xBSref;
      svd.backSub(B);
      TUASSERTFEPS( B, BSref, eps);
   }
}


int main()
{
   TestUtil testFramework("Matrix SVD", "--", __FILE__, __LINE__);
   
   double a22[] = {2,1,1,2};
   double b2[] = {1,2};
   double bs2[] = {0,1};
   SVDTest(2, 2, a22, b2, bs2, testFramework, "2x2");

   double a23[] = {4, 11, 14, 8, 7, -2};
   SVDTest(2, 3, a23, NULL, NULL, testFramework, "2x3");
   SVDTest(3, 2, a23, NULL, NULL, testFramework, "3x2");

   double a33[] = {2,-1,0,-1,2,-1,0,-1,2};
   double b3[] = {7,-3,2};
   double bs3[] = {4.25,1.5,1.75};
   SVDTest(3, 3, a33, b3, bs3, testFramework, "3x3");

   double a44[] = {2,-1,0,0,-1,2,-1,0,0,-1,2,-1,0,0,-1,2};
   double b4[] = {5,1,-2,6};
   double bs4[] ={5,5,4,5};   
   SVDTest(4, 4, a44, b4, bs4, testFramework, "4x4");

   std::cout << "Total Failures for " << __FILE__ << ": " << testFramework.countFails() << std::endl;

   return testFramework.countFails();
}
