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

#include "Stats.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"
#include <iostream>

using namespace std;

#define TUA(EXP,GOT,METHOD) testFramework.changeSourceMethod(METHOD); testFramework.assert_equals<int>(EXP,GOT,__LINE__)
#define TUAE(EXP,GOT,EPS,METHOD) testFramework.changeSourceMethod(METHOD); testFramework.assert_equals(T(EXP),GOT,__LINE__,"",EPS)

template<class T>
size_t statsTest()
{
   std::string typeName = gpstk::typeString<T>();
   T precision = 10*std::numeric_limits<T>::epsilon();
   gpstk::TestUtil testFramework( "Stats<"+typeName+">", "--", __FILE__, __LINE__ );

   gpstk::TwoSampleStats<T> tso;

   tso.Add(1, 2);	
   tso.Add(2, 5);
   tso.Add(3, 1);
   tso.Add(4, 4);
   tso.Add(5, 3);
   TUA(5, tso.N(), "Add()");
   
   TUAE(3, tso.AverageX(), precision, "AverageX()");
   TUAE(5, tso.MaximumX(), precision, "MaxX()");
   TUAE(1, tso.MinimumX(), precision, "MinX()");
   TUAE(2.5, tso.VarianceX(), precision, "VarianceX()");
   TUAE(sqrt(2.5), tso.StdDevX(), precision, "StdDevX()");

   TUAE(3, tso.AverageY(), precision, "AverageY()");
   TUAE(5, tso.MaximumY(), precision, "MaxY()");
   TUAE(1, tso.MinimumY(), precision, "MinY()");
   TUAE(2.5, tso.VarianceY(), precision, "VarianceY()");
   TUAE(sqrt(2.5), tso.StdDevY(), precision, "StdDevY()");

   TUAE(0.1, tso.Slope(), precision, "Slope()");
   TUAE(2.7, tso.Intercept(), precision, "Intercept()");
   TUAE(0.574456264653802865989, tso.SigmaSlope(), precision, "SigmaSlope()");
   TUAE(1.81659021245849499920, tso.SigmaYX(), precision, "SigmaYX()");
   TUAE(0.1, tso.Correlation(), precision, "Correlation()");
   
   return testFramework.countFails();                  
}


int main()
{
   unsigned ec=0;
   ec += statsTest<float>();
   ec += statsTest<double>();
   ec += statsTest<long double>();
	
   cout << "Total Failures for " << __FILE__ << ": " << ec << endl;
   return ec;
}
