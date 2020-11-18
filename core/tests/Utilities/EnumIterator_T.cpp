//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================

#include "TestUtil.hpp"
#include "EnumIterator.hpp"
#include <iostream>

using namespace std;

enum class TestEnum1
{
   One,
   Two,
   Three,
   Four,
   Five,
   Last
};

ostream& operator<<(ostream& s, TestEnum1 e)
{
   s << static_cast<int>(e);
   return s;
}

class EnumIterator_T
{
public:
   unsigned constructorTest();
   unsigned incrementTest();
   unsigned inequalityTest();
   unsigned beginEndTest();
};


unsigned EnumIterator_T ::
constructorTest()
{
   TUDEF("EnumIterator", "EnumIterator()");
   typedef gpstk::EnumIterator<TestEnum1, TestEnum1::One, TestEnum1::Last> TestIterator1;
   TestIterator1 test1;
      // this also tests the dereference operator
   TUASSERTE(TestEnum1, TestEnum1::One, *test1);

   typedef gpstk::EnumIterator<TestEnum1, TestEnum1::Two, TestEnum1::Last> TestIterator2;
   TestIterator2 test2;
      // this also tests the dereference operator
   TUASSERTE(TestEnum1, TestEnum1::Two, *test2);

   TUCSM("EnumIterator(C)");
   TestIterator1 test3(TestEnum1::Three);
      // this also tests the dereference operator
   TUASSERTE(TestEnum1, TestEnum1::Three, *test3);

   TURETURN();
}


unsigned EnumIterator_T ::
incrementTest()
{
   TUDEF("EnumIterator", "operator++");
   typedef gpstk::EnumIterator<TestEnum1, TestEnum1::One, TestEnum1::Last> TestIterator1;
   TestIterator1 test1;
      // this also tests the dereference operator
   TUASSERTE(TestEnum1, TestEnum1::One, *test1);
   ++test1;
   TUASSERTE(TestEnum1, TestEnum1::Two, *test1);
   TURETURN();
}


unsigned EnumIterator_T ::
inequalityTest()
{
   TUDEF("EnumIterator", "operator!=");
   typedef gpstk::EnumIterator<TestEnum1, TestEnum1::One, TestEnum1::Last> TestIterator1;
   TestIterator1 test1, test2;
   TUASSERT(!(test1.operator!=(test2)));
   ++test2;
   TUASSERT(test1.operator!=(test2));
   TURETURN();
}


unsigned EnumIterator_T ::
beginEndTest()
{
   TUDEF("EnumIterator", "begin/end");
   typedef gpstk::EnumIterator<TestEnum1, TestEnum1::One, TestEnum1::Last> TestIterator1;
   unsigned count = 0;
   TestIterator1 test;
   for (test = test.begin(); test != test.end(); ++test, count++)
   {
   }
   TUASSERTE(unsigned, 5, count);
      // nicer syntax
   count = 0;
   for (TestEnum1 e : TestIterator1())
   {
      count++;
   }
   TUASSERTE(unsigned, 5, count);
   TURETURN();
}


int main()
{
   unsigned errorTotal = 0;
   EnumIterator_T testClass;
   errorTotal += testClass.constructorTest();
   errorTotal += testClass.incrementTest();
   errorTotal += testClass.inequalityTest();
   errorTotal += testClass.beginEndTest();
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
   return errorTotal;
}
