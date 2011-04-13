#pragma ident "$Id: xMatrix.cpp 1979 2009-06-24 18:05:07Z afarris $"
// file: xMatrix.cpp

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

#include "xMatrix.hpp"
#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION (xMatrix);

void xMatrix :: setUp (void) 
{
   gpstk::Vector<double> v1 = gpstk::Vector<double>();
   gpstk::Vector<double> v2 = gpstk::Vector<double>();
   gpstk::Vector<double> v3 = gpstk::Vector<double>();
   gpstk::Vector<double> v4 = gpstk::Vector<double>();
   v1 = v1 && 1 && 2 && 3 && 4;
   v2 = v2 && 5 && 6 && 7 && 8;
   v3 = v3 && 9 && 10 && 11 && 12;
   v4 = v4 && 13 && 14 && 15 && 16;
   
   a = new gpstk::Matrix<double>(2, 2, 1);
   b = new gpstk::Matrix<double>(8, 2, 3);
   c = new gpstk::Matrix<double>(4, 2, 5);
   d = new gpstk::Matrix<double>(4, 4, 7);
   e = new gpstk::Matrix<double>(8, 2, v1&&v2&&v3&&v4);
   f = new gpstk::Matrix<double>(4, 4, v4&&v3&&v2&&v1);
   g = new gpstk::Matrix<double>(2, 2, v1);
   h = new gpstk::Matrix<double>(4, 2, v2&&v2);
}

void xMatrix :: tearDown (void)
{
    // finally delete objects
    delete a; delete b; delete c; delete d;
    delete e; delete f; delete g; delete h;
}

void xMatrix :: sizeTest (void)
{
   CPPUNIT_ASSERT_EQUAL((size_t)4,(*a).size());
   CPPUNIT_ASSERT_EQUAL((size_t)2,(*a).rows());
   CPPUNIT_ASSERT_EQUAL((size_t)2,(*a).cols());

   CPPUNIT_ASSERT_EQUAL((size_t)16,(*b).size());
   CPPUNIT_ASSERT_EQUAL((size_t)8,(*b).rows());
   CPPUNIT_ASSERT_EQUAL((size_t)2,(*b).cols());

   CPPUNIT_ASSERT_EQUAL((size_t)8,(*c).size());
   CPPUNIT_ASSERT_EQUAL((size_t)4,(*c).rows());
   CPPUNIT_ASSERT_EQUAL((size_t)2,(*c).cols());

   CPPUNIT_ASSERT_EQUAL((size_t)16,(*d).size());
   CPPUNIT_ASSERT_EQUAL((size_t)4,(*d).rows());
   CPPUNIT_ASSERT_EQUAL((size_t)4,(*d).cols());

   CPPUNIT_ASSERT_EQUAL((size_t)16,(*e).size());
   CPPUNIT_ASSERT_EQUAL((size_t)8,(*e).rows());
   CPPUNIT_ASSERT_EQUAL((size_t)2,(*e).cols());

   CPPUNIT_ASSERT_EQUAL((size_t)16,(*f).size());
   CPPUNIT_ASSERT_EQUAL((size_t)4,(*f).rows());
   CPPUNIT_ASSERT_EQUAL((size_t)4,(*f).cols());

   CPPUNIT_ASSERT_EQUAL((size_t)4,(*g).size());
   CPPUNIT_ASSERT_EQUAL((size_t)2,(*g).rows());
   CPPUNIT_ASSERT_EQUAL((size_t)2,(*g).cols());

   CPPUNIT_ASSERT_EQUAL((size_t)8,(*h).size());
   CPPUNIT_ASSERT_EQUAL((size_t)4,(*h).rows());
   CPPUNIT_ASSERT_EQUAL((size_t)2,(*h).cols());
}

void xMatrix :: getTest (void)
{
   for(int i = 0; i < (*a).rows(); i++)
      for(int j = 0; j < (*a).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(1.,(*a)(i,j));

   for(int i = 0; i < (*b).rows(); i++)
      for(int j = 0; j < (*b).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(3.,(*b)(i,j));

   for(int i = 0; i < (*c).rows(); i++)
      for(int j = 0; j < (*c).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(5.,(*c)(i,j));

   for(int i = 0; i < (*d).rows(); i++)
      for(int j = 0; j < (*d).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(7.,(*d)(i,j));

   for(int i = 0; i < (*e).rows(); i++)
      for(int j = 0; j < (*e).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(1+i+j*8.,(*e)(i,j));

   for(int i = 0; i < (*f).rows(); i++)
      for(int j = 0; j < (*f).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(1+i+(3-j)*4.,(*f)(i,j));

   for(int i = 0; i < (*g).rows(); i++)
      for(int j = 0; j < (*g).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(1+i+j*2.,(*g)(i,j));

   for(int i = 0; i < (*h).rows(); i++)
      for(int j = 0; j < (*h).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(5.+i,(*h)(i,j));
}

void xMatrix :: operatorTest (void)
{
   (*d) += (*f); // 4x4
   for(int i = 0; i < (*d).rows(); i++)
      for(int j = 0; j < (*d).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(8+i+(3-j)*4.,(*d)(i,j));
   (*d) -= (*f); // 4x4

   (*a) += (*g); // 2x2
   for(int i = 0; i < (*a).rows(); i++)
      for(int j = 0; j < (*a).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(2+i+j*2.,(*a)(i,j));
   (*a) -= (*g); // 2x2

   (*b) -= (*e); // 8x2
   for(int i = 0; i < (*b).rows(); i++)
      for(int j = 0; j < (*b).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(3-(1+i+j*8.),(*b)(i,j));
   (*b) += (*e); // 8x2

   (*c) -= (*h); // 4x2
   for(int i = 0; i < (*c).rows(); i++)
      for(int j = 0; j < (*c).cols(); j++)
         CPPUNIT_ASSERT_EQUAL(5-(5.+i),(*c)(i,j));
   (*c) += (*h); // 4x2
}
