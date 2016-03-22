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

#include "Vector.hpp"
#include "TestUtil.hpp"
#include <iostream>


class Vector_T
{
   public:

   /* Tests the initialization of Vector objects */
   int initializationTest(void)
   {
      gpstk::TestUtil testFramework("Vector","initializationTest",__FILE__,__LINE__);
      std::string failMesg;

      int badCount=0;

      gpstk::Vector<double> v1(250,1.0);
      gpstk::Vector<double> v2(1000,5.0);
      gpstk::Vector<double> st(250); //Initialize without values
      gpstk::Vector<double> Compare(v1); //Copy Constructor

      std::valarray<double> valarray1(10.0, 250);
      gpstk::Vector<double> val(valarray1);

      int i = 0;

      for(i=0; i<v1.size(); i++)
      {
      failMesg = "Were the array values set to expectation?";
         if (1.0 != v1[i]) {badCount++;}
         if (1.0 != v1(i)) {badCount++;}
      }
      testFramework.assert(badCount==0, failMesg, __LINE__);
      badCount = 0; // Reset error counter

      failMesg = "Was the size set to expectation?";
      testFramework.assert(i == v1.size(), failMesg, __LINE__);

      for(i=0; i<v2.size(); i++)
      {
      failMesg = "Were the array values set to expectation?";
         if (5.0 != v2[i]) {badCount++;}
         if (5.0 != v2(i)) {badCount++;}
      }
      testFramework.assert(badCount==0, failMesg, __LINE__);
      badCount = 0; // Reset error counter

      failMesg = "Was the size set to expectation?";
      testFramework.assert(i == v2.size(), failMesg, __LINE__);

      failMesg = "Was the size set to expectation?";
      testFramework.assert(250 == st.size(), failMesg, __LINE__);

      for(i=0; i<Compare.size(); i++)
      {
      failMesg = "Were the array values set to expectation?";
         if (1.0 != Compare[i]) {badCount++;}
         if (1.0 != Compare(i)) {badCount++;}
      }
      testFramework.assert(badCount==0, failMesg, __LINE__);
      badCount = 0; // Reset error counter

      failMesg = "Was the size set to expectation?";
      testFramework.assert(i == Compare.size(), failMesg, __LINE__);

      for(i=0; i<val.size(); i++)
      {
      failMesg = "Were the array values set to expectation?";
         if (10.0 != val[i]) {badCount++;}
         if (10.0 != val(i)) {badCount++;}
      }
      testFramework.assert(badCount==0, failMesg, __LINE__);
      badCount = 0; // Reset error counter

      failMesg = "Was the size set to expectation?";
      testFramework.assert(i == val.size(), failMesg, __LINE__);

//===============================================================
// Add test for VectorBase and subvector constructors
//===============================================================

      return testFramework.countFails();
   }

   /* Tests addition and subtraction between vector objects */
   int operatorTest(void)
   {
      gpstk::TestUtil testFramework( "Vector", "== Operator", __FILE__, __LINE__);
      std::string failMesg;

      gpstk::Vector<double> v1(3,1.0);
      gpstk::Vector<double> v2(12,3.0);
      gpstk::Vector<double> v3 = v2;
      gpstk::Vector<double> v4 = -v1;

      int badCount = 0;

      for(int i = 0; i < v2.size(); i++)
      {
      failMesg = "Are equivalent objects equivalent?";
         if (v2[i] != v3[i]) {badCount++;}
      }
      testFramework.assert(badCount==0, failMesg, __LINE__);

      for(int i = 0; i < v1.size(); i++)
      {
      failMesg = "Are equivalent objects equivalent?";
         if (-v1[i] != v4[i]) {badCount++;}
      }
      testFramework.assert(badCount==0, failMesg, __LINE__);

      testFramework.changeSourceMethod("+= Operator");
      v2 += v3; // 6 6 6 ...
      v2 += 2; // 8 8 8 ...

      for(int i = 0; i < v2.size(); i++)
      {
      failMesg = "Were the previous addition operators successful?";
         if (8. != v2[i]) {badCount++;}
      }
      testFramework.assert(badCount==0, failMesg, __LINE__);

      testFramework.changeSourceMethod("+= Operator");
      v2 -= v3; // 5 5 5 ...
      v2 -= 4; // 1 1 1 ...
   
      for(int i = 0; i < v1.size(); i++)
      {
      failMesg = "Were the previous subtraction operators successful?";
         if (v1[i] != v2[i]) {badCount++;} //sizes mismatch, check till v1 ends
      }
      testFramework.assert(badCount==0, failMesg, __LINE__);

      v2 += 2; // 3 3 3 ...
   
      v1 = v1&&v2; // 3 3 3 3 3 3 ...

      failMesg = "Was the previous && operators successful in joining similar vectors?";
      testFramework.assert((size_t)15==v1.size(), failMesg, __LINE__);

      v1 = v1&&v3;

      failMesg = "Was the previous && operators successful in joining different vectors?";
      testFramework.assert((size_t)27==v1.size(), failMesg, __LINE__);

      return testFramework.countFails();
   }
};

int main()
{
   int check, errorCounter = 0;
   Vector_T testClass;

   check = testClass.initializationTest();
   errorCounter += check;

   check = testClass.operatorTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

   return errorCounter; //Return the total number of errors
}
