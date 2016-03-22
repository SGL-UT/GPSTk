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

template<typename T>
void SVDTest(size_t r, size_t c,
                  T xA[], T xB[], T xBSref[],
                  gpstk::TestUtil& testFramework)
{
   ostringstream oss;
   oss << r << "x" << c;
   testFramework.changeSourceMethod(oss.str());
   T eps=100*std::numeric_limits<T>::epsilon();
   gpstk::Matrix<T> A(r,c);
   A = xA;
   gpstk::SVD<T> svd;
   svd(A);
   gpstk::Matrix<T> S(r, c, 0.0);
   for (int i=0; i < min(r,c); i++)
      S(i,i) = svd.S(i);
   TUASSERTFEPS( A, svd.U * S * transpose(svd.V), eps);
   
   TUASSERTFEPS( gpstk::ident<T>(r), svd.U * transpose(svd.U), eps);
   TUASSERTFEPS( gpstk::ident<T>(c), svd.V * transpose(svd.V), eps);

   if (r == c)
   {
      gpstk::Vector<T> B(r), BSref(r);
      B = xB;
      BSref = xBSref;
      svd.backSub(B);
      TUASSERTFEPS( B, BSref, eps);
   }
}


template<typename T>
unsigned multipass()
{
   gpstk::TestUtil testFramework("Matrix SVD<"+gpstk::typeString<T>()+">", "--", __FILE__, __LINE__);
   T a22[] = {2,1,1,2};
   T b2[] = {1,2};
   T bs2[] = {0,1};
   SVDTest(2, 2, a22, b2, bs2, testFramework);

   T a23[] = {4, 11, 14, 8, 7, -2};
   SVDTest<T>(3, 2, a23, NULL, NULL, testFramework);
   SVDTest<T>(2, 3, a23, NULL, NULL, testFramework);

   T a33[] = {2,-1,0,-1,2,-1,0,-1,2};
   T b3[] = {7,-3,2};
   T bs3[] = {4.25,1.5,1.75};
   SVDTest<T>(3, 3, a33, b3, bs3, testFramework);

   T a44[] = {2,-1,0,0,-1,2,-1,0,0,-1,2,-1,0,0,-1,2};
   T b4[] = {5,1,-2,6};
   T bs4[] ={5,5,4,5};   
   SVDTest<T>(4, 4, a44, b4, bs4, testFramework);
   SVDTest<T>(2, 8, a44, NULL, NULL, testFramework);
   SVDTest<T>(8, 2, a44, NULL, NULL, testFramework);
   return testFramework.countFails();
}


int main()
{
   unsigned ec=0;

   ec += multipass<float>();
   ec += multipass<double>();
   ec += multipass<long double>();
   
   std::cout << "Total Failures for " << __FILE__ << ": " << ec << std::endl;

   return ec;
}
