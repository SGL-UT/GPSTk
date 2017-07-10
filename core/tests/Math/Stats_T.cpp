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
#include <sstream>
#include <typeinfo>
#include "Stats.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"

#define TUA(EXP,GOT,METHOD) testFramework.changeSourceMethod(METHOD); testFramework.assert_equals<int>(EXP,GOT,__LINE__)
#define TUAE(EXP,GOT,EPS,METHOD) testFramework.changeSourceMethod(METHOD); testFramework.assert_equals(T(EXP),GOT,__LINE__,"",EPS)

template<class T>
size_t statsTest()
{
   std::string typeName = gpstk::typeString<T>();
   T precision = 10*std::numeric_limits<T>::epsilon();
   gpstk::TestUtil testFramework( "Stats<"+typeName+">", "--", __FILE__, __LINE__ );

   gpstk::Stats<T> statsObject;

   statsObject.Add((T)1.0, (T)1.0);
   TUA(1, statsObject.N(), "Add("+typeName+")");

   size_t N=3;
   gpstk::Vector<T> input(N), weight(N);
   for (int i = 0; i < N; i++)
      input[i] = weight[i] = (T)i+2;
   statsObject.Add(input, weight);
   N+=1; // Since there was already one value in the object
   TUA(N, statsObject.N(), "Add(Vector<"+typeName+">)");
   std::cout << std::setprecision(25) << statsObject << std::endl;
   T eMin=1, eMax=4, eAvg=3, eVar=1.333333333333333333333, eStd=1.15470053837925152890459, eNorm=10;

   TUAE(eAvg, statsObject.Average(), precision, "Average()");
   TUAE(eMax, statsObject.Maximum(), precision, "Maximum()");
   TUAE(eMin, statsObject.Minimum(), precision, "Minimum()");
   TUAE(eVar, statsObject.Variance(), precision, "Variance()");
   TUAE(eStd, statsObject.StdDev(), precision, "StdDev()");
   TUAE(eNorm, statsObject.Normalization(), precision, "Normalization()");
   TUA(true, statsObject.Weighted(), "Weighted()");

   statsObject.Subtract((float)1.0, (float)1.0);
   N-=1;
   TUA( N, statsObject.N(), "Subtract");

   statsObject.Subtract(input);
   N-=3;
   TUA(N, statsObject.N(), "Subtract(Vector<"+typeName+">)");

   TUAE(0.0, statsObject.Average(), precision, "Average()");
   TUAE(0.0, statsObject.Maximum(), precision, "Maximum()");
   TUAE(0.0, statsObject.Minimum(), precision, "Minimum()");
   TUAE(0.0, statsObject.Variance(), precision, "Variance()");
   TUAE(0.0, statsObject.StdDev(), precision, "StdDev()");

   gpstk::Stats<T> statsObject2;
   N=4;
   statsObject2.Load(N, eMin, eMax, eAvg, eVar, true, eNorm);
   TUA(N, statsObject2.N(), "Load()");

   TUAE(eAvg, statsObject2.Average(), precision, "Average()");
   TUAE(eMax, statsObject2.Maximum(), precision, "Maximum()");
   TUAE(eMin, statsObject2.Minimum(), precision, "Minimum()");
   TUAE(eVar, statsObject2.Variance(), precision, "Variance()");
   TUAE(eNorm, statsObject2.Normalization(), precision, "Normalization()");

   statsObject += statsObject2;
   TUA(N, statsObject2.N(), "operator+=");

   TUAE(eAvg, statsObject.Average(), precision, "Average()");
   TUAE(eMax, statsObject.Maximum(), precision, "Maximum()");
   TUAE(eMin, statsObject.Minimum(), precision, "Minimum()");
   TUAE(eVar, statsObject.Variance(), precision, "Variance()");
   TUAE(eNorm, statsObject.Normalization(), precision, "Normalization()");

   std::stringstream obtainedOutput;
   obtainedOutput << statsObject;
   
   testFramework.changeSourceMethod("operator<<");
   testFramework.assert(obtainedOutput.str().size() > 0, "", __LINE__);
   return testFramework.countFails();                  
}


int main()
{
   size_t fc=0;
   fc += statsTest<float>();
   fc += statsTest<double>();
   fc += statsTest<long double>();
   std::cout << "Total Failures for " << __FILE__ << ": " << fc << std::endl;
   return fc;
}
