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

#include "Matrix.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"

void hhtest(size_t r, size_t c, double a[], double hh_ref[], gpstk::TestUtil& testFramework)
{
   gpstk::Matrix<double> A(r,c), HH_ref(r,c);
   A = a;
   HH_ref = hh_ref;
   gpstk::Householder<double> HH;
   HH(A);

   testFramework.assert(
      HH.A.rows() == HH.A.cols(),
      "Householder Transformation is square.",
      __LINE__);

   int fail = 0;
   for (int i = 0; i < HH.A.rows(); i++)
   {
      for (int j = 0; j < HH.A.cols(); j++)
      {
         if (j == i && HH.A(i,j) == 0)
            fail++;
         if (j < i && HH.A(i,j) != 0)
            fail++;
      }
   }
   testFramework.assert(
      fail==0,
      "Householder Transformation is upper triangular",
      __LINE__);

   int zeroCount=0, badCount = 0;
   int iRowNegCount=0, oRowNegCount=0;
   double eps = 1e-10;
   for(int i = 0; i < A.rows(); i++)
   {
      for(int j = 0; j < A.cols(); j++)
      {
         //if values is WRONG, determine if it's negative or just plain wrong
         if ((std::abs(HH.A(i,j) - HH_ref(i,j)) > eps))
         {
            if (std::abs(-1 * HH.A(i,j) - HH_ref(i,j)) < eps)
               iRowNegCount++;
            else
               badCount++;
         }
         if (HH.A(i,j) == 0)
            zeroCount++;
      }
      //If all non-zero values in a row are negative, mark row as negative
      if (iRowNegCount == A.cols() - zeroCount)
         oRowNegCount++;
      //Otherwise, all negative values become plain wrong
      else if (iRowNegCount != 0)
         badCount += iRowNegCount;
      zeroCount = 0;
      iRowNegCount = 0;
   }
   testFramework.assert(
      badCount==0,
      "Householder Transformation matches expected values",
      __LINE__);
}

int main()
{
   int errorCounter = 0;
   gpstk::TestUtil testFramework("Matrix Householder", "--", __FILE__, __LINE__);

   // all of the reference values were computed using WolframAlpha to compute
   // the R value from a QR factorization
   // for A8 https://www.wolframalpha.com/input/?i=QR+factorization++{{6,5},{-5,7}}
   
   double a5[] = {
      1,1,0,
      1,0,1,
      0,1,1};
   double ref5[] = {
      pow(2,.5), pow(2,-.5), pow(2,-.5), 
      0, pow(1.5,.5), pow(6,-.5),
      0,0,2./pow(3,.5)};
   testFramework.changeSourceMethod("A5");
   hhtest(3, 3, a5, ref5, testFramework);
   
   double a6[9] = {
      12,-51,4,
      6,167,-68,
      -4,24,41};
   double ref6[9] = {
      14, 21, -262./7,
      0, 175, -1958./35,
      0, 0, 1481./35};
   testFramework.changeSourceMethod("A6");
   hhtest(3, 3, a6, ref6, testFramework);
        
   double a7[9] = {
      1,2,3,
      -1,0,-3,
      0,-2,3};
   double ref7[9] = {
      -1*pow(2,.5),-1*pow(2,.5),-1*pow(18,.5),
      0,-1*pow(6,.5),pow(6,.5),
      0,0,pow(3,.5)};
   testFramework.changeSourceMethod("A7");
   hhtest(3, 3, a7, ref7, testFramework);

   double a8[4] = {
      6,5,
      -5,7};
   double ref8[4] = {
      pow(61, .5), -5./pow(61, .5),
      0, 67./pow(61, .5)};
   testFramework.changeSourceMethod("A8");
   hhtest(2, 2, a8, ref8, testFramework);

   double a9[16] = {
      2,1,0,0,
      1,2,1,0,
      0,1,2,1,
      0,0,1,2};
   double ref9[16] = {
      pow(5, .5), 4./pow(5, .5), 1./pow(5, .5), 0,
      0, pow(14./5, .5), 3*pow(2./35, .5)+pow(10./7, .5), pow(5./14, .5),
      0, 0, pow(15./7, .5), 2*pow(3./35, .5)+2*pow(7./15, .5),
      0, 0, 0, pow(5./6, .5)};
   testFramework.changeSourceMethod("A9");
   hhtest(4, 4, a9, ref9, testFramework);

   std::cout << "Total Failures for " << __FILE__ << ": " << testFramework.countFails() << std::endl;

   return testFramework.countFails();
}
