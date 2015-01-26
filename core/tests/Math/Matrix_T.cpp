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
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class Matrix_T
{
	public:
	Matrix_T() // Default Constructor, set values that will be used frequently
	{
		eps = 1e-12;

	}
	~Matrix_T() // Default Desructor
	{       // finally delete objects

	} 
	double eps;

	/* Test to check the sizing functions */
	int sizeTest (void)
	{
		TestUtil testFramework("Matrix","Size",__FILE__,__LINE__);
		testFramework.init();

   		gpstk::Vector<int> v1 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v1[i] = i+1;
   		gpstk::Vector<int> v2 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v2[i] = 16-i;
   		gpstk::Vector<int> v3 = gpstk::Vector<int>(4);
   		for(int i = 0; i < 4; i++) v3[i] = i+1;
   		gpstk::Vector<int> v4 = gpstk::Vector<int>(8);
   		for(int i = 0; i < 4; i++) {v4[i] = i+1+4; v4[i+4] = i+1+4;}

   		gpstk::Matrix<int> a(2,2,1);
   		gpstk::Matrix<int> b(8, 2, 3);
   		gpstk::Matrix<int> c(4, 2, 5);
   		gpstk::Matrix<int> d(4, 4, 7);
   		gpstk::Matrix<int> e(8, 2, v1);
   		gpstk::Matrix<int> f(4, 4, v2);
   		gpstk::Matrix<int> g(2, 2, v3);
   		gpstk::Matrix<int> h(4, 2, v4);

//--------------Matrix_sizeTest_1 - Does the size method function as expected?
		testFramework.assert((size_t)4 == a.size());
		testFramework.next();

//--------------Matrix_sizeTest_2 - Does the rows method function as expected?
		testFramework.assert((size_t)2 == a.rows());
		testFramework.next();

//--------------Matrix_sizeTest_3 - Does the cols method function as expected?
		testFramework.assert((size_t)2 == a.cols());
		testFramework.next();

//--------------Matrix_sizeTest_4 - Does the size method function as expected?
		testFramework.assert((size_t)16 == b.size());
		testFramework.next();

//--------------Matrix_sizeTest_5 - Does the rows method function as expected?
		testFramework.assert((size_t)8 == b.rows());
		testFramework.next();

//--------------Matrix_sizeTest_6 - Does the cols method function as expected?
		testFramework.assert((size_t)2 == b.cols());
		testFramework.next();

//--------------Matrix_sizeTest_7 - Does the size method function as expected?
		testFramework.assert((size_t)8 == c.size());
		testFramework.next();

//--------------Matrix_sizeTest_8 - Does the rows method function as expected?
		testFramework.assert((size_t)4 == c.rows());
		testFramework.next();

//--------------Matrix_sizeTest_9 - Does the cols method function as expected?
		testFramework.assert((size_t)2 == c.cols());
		testFramework.next();

//--------------Matrix_sizeTest_10 - Does the cols method function as expected?
		testFramework.assert((size_t)16 == d.size());
		testFramework.next();

//--------------Matrix_sizeTest_11 - Does the rows method function as expected?
		testFramework.assert((size_t)4 == d.rows());
		testFramework.next();

//--------------Matrix_sizeTest_12 - Does the cols method function as expected?
		testFramework.assert((size_t)4 == d.cols());
		testFramework.next();

//--------------Matrix_sizeTest_13 - Does the cols method function as expected?
		testFramework.assert((size_t)16 == e.size());
		testFramework.next();

//--------------Matrix_sizeTest_14 - Does the rows method function as expected?
		testFramework.assert((size_t)8 == e.rows());
		testFramework.next();

//--------------Matrix_sizeTest_15 - Does the cols method function as expected?
		testFramework.assert((size_t)2 == e.cols());
		testFramework.next();

//--------------Matrix_sizeTest_16 - Does the cols method function as expected?
		testFramework.assert((size_t)16 == f.size());
		testFramework.next();

//--------------Matrix_sizeTest_17 - Does the rows method function as expected?
		testFramework.assert((size_t)4 == f.rows());
		testFramework.next();

//--------------Matrix_sizeTest_18 - Does the cols method function as expected?
		testFramework.assert((size_t)4 == f.cols());
		testFramework.next();

//--------------Matrix_sizeTest_19 - Does the cols method function as expected?
		testFramework.assert((size_t)4 == g.size());
		testFramework.next();

//--------------Matrix_sizeTest_20 - Does the rows method function as expected?
		testFramework.assert((size_t)2 == g.rows());
		testFramework.next();

//--------------Matrix_sizeTest_21 - Does the cols method function as expected?
		testFramework.assert((size_t)2 == g.cols());
		testFramework.next();

//--------------Matrix_sizeTest_22 - Does the cols method function as expected?
		testFramework.assert((size_t)8 == h.size());
		testFramework.next();

//--------------Matrix_sizeTest_23 - Does the rows method function as expected?
		testFramework.assert((size_t)4 == h.rows());
		testFramework.next();

//--------------Matrix_sizeTest_24 - Does the cols method function as expected?
		testFramework.assert((size_t)2 == h.cols());
		
		return testFramework.countFails();
	}

	/* Test the accessor operator */
	int getTest (void)
	{
		TestUtil testFramework("Matrix","getTest",__FILE__,__LINE__);
      	testFramework.init();
      	int badCount = 0;

   		gpstk::Vector<int> v1 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v1[i] = i+1;
   		gpstk::Vector<int> v2 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v2[i] = 16-i;
   		gpstk::Vector<int> v3 = gpstk::Vector<int>(4);
   		for(int i = 0; i < 4; i++) v3[i] = i+1;
   		gpstk::Vector<int> v4 = gpstk::Vector<int>(8);
   		for(int i = 0; i < 4; i++) {v4[i] = i+1+4; v4[i+4] = i+1+4;}

   		gpstk::Matrix<int> a(2,2,1);
   		gpstk::Matrix<int> b(8, 2, 3);
   		gpstk::Matrix<int> c(4, 2, 5);
   		gpstk::Matrix<int> d(4, 4, 7);
   		gpstk::Matrix<int> e(8, 2, v1);
   		gpstk::Matrix<int> f(4, 4, v2);
   		gpstk::Matrix<int> g(2, 2, v3);
   		gpstk::Matrix<int> h(4, 2, v4);

//--------------Matrix_getTest_1 - Is matrix a initializated properly?
   		for(int i = 0; i < a.rows(); i++)
      			for(int j = 0; j < a.cols(); j++)
         			if (1 != a(i,j)) {badCount++;}
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter

//--------------Matrix_getTest_2 - Is matrix b initializated properly?
   		for(int i = 0; i < b.rows(); i++)
      			for(int j = 0; j < b.cols(); j++)
         			if (3 != b(i,j)) {badCount++;}
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter

//--------------Matrix_getTest_3 - Is matrix c initializated properly?
		for(int i = 0; i < c.rows(); i++)
		      	for(int j = 0; j < c.cols(); j++)
         			if (5 != c(i,j)) {badCount++;}
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter

//--------------Matrix_getTest_4 - Is matrix d initializated properly?
		for(int i = 0; i < d.rows(); i++)
		      	for(int j = 0; j < d.cols(); j++)
					if (7 != d(i,j)) {badCount++;}
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter

//--------------Matrix_getTest_5 - Is matrix e initializated properly?
		for(int i = 0; i < e.rows(); i++)
		      	for(int j = 0; j < e.cols(); j++)
					if (1+i+j*8 != e(i,j)) {badCount++;}
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter

//--------------Matrix_getTest_6 - Is matrix f initializated properly?
		for(int i = 0; i < f.rows(); i++)
		      	for(int j = 0; j < f.cols(); j++)
					if (16-i-4*j != f(i,j)) {badCount++;}
				//cout << f(i,j) << " " << 16-i-4*j << endl;
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter

//--------------Matrix_getTest_7 - Is matrix g initializated properly?
		for(int i = 0; i < g.rows(); i++)
		      	for(int j = 0; j < g.cols(); j++)
					if (1+i+j*2 != g(i,j)) {badCount++;}
				//cout << g(i,j) << " " << 1+i+j*2 << endl;
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter

//--------------Matrix_getTest_8 - Is matrix h initializated properly?
		for(int i = 0; i < h.rows(); i++)
		      	for(int j = 0; j < h.cols(); j++)
			 		if (5.+i != h(i,j)) {badCount++;}
				//cout << h(i,j) << " " << 5.+i << endl;
        testFramework.assert(badCount==0);
      	badCount = 0; // Reset error counter

		return testFramework.countFails();
	}

	/* Test the add/subtract and set operators */
	int operatorTest (void)
	{
		TestUtil testFramework("Matrix","Operators",__FILE__,__LINE__);
      	testFramework.init();
      	int badCount = 0;

   		gpstk::Vector<int> v1 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v1[i] = i+1;
   		gpstk::Vector<int> v2 = gpstk::Vector<int>(16);
   		for(int i = 0; i < 16; i++) v2[i] = 16-i;
   		gpstk::Vector<int> v3 = gpstk::Vector<int>(4);
   		for(int i = 0; i < 4; i++) v3[i] = i+1;
   		gpstk::Vector<int> v4 = gpstk::Vector<int>(8);
   		for(int i = 0; i < 4; i++) {v4[i] = i+1+4; v4[i+4] = i+1+4;}

   		gpstk::Matrix<int> a(2,2,1);
   		gpstk::Matrix<int> b(8, 2, 3);
   		gpstk::Matrix<int> c(4, 2, 5);
   		gpstk::Matrix<int> d(4, 4, 7);
   		gpstk::Matrix<int> e(8, 2, v1);
   		gpstk::Matrix<int> f(4, 4, v2);
   		gpstk::Matrix<int> g(2, 2, v3);
   		gpstk::Matrix<int> h(4, 2, v4);

//--------------Matrix_operatorTest_1 - Does 2x2 additon function as expected?
   		a += g; // 2x2 addition
   		for(int i = 0; i < a.rows(); i++)
      			for(int j = 0; j < a.cols(); j++)
				//cout << a(i,j) << " " << 2+i+j*2 << endl;
         			if (2+i+j*2. != a(i,j)) {badCount++;}
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter;
   		a -= g; // 2x2 reset a

//--------------Matrix_operatorTest_2 - Does 8x2 subtraction work as expected?
   		b -= e; // 8x2 subtraction
   			for(int i = 0; i < b.rows(); i++)
      				for(int j = 0; j < b.cols(); j++)
					//cout << b(i,j) << " " << 3-(1+i+j*8) << endl;
         				if (3-(1+i+j*8.) != b(i,j)) {badCount++;}
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter;
   		b += e; // 8x2 reset b

//--------------Matrix_operatorTest_3 - Does 4x2 subtraction work as expected?
   		c -= h; // 4x2 subtraction
   		for(int i = 0; i < c.rows(); i++)
      			for(int j = 0; j < c.cols(); j++)
					//cout << c(i,j) << " " << 5-(5.+i) << endl;
         				if (5-(5.+i) != c(i,j)) {badCount++;}
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter;
   		c += h; // 4x2 reset c

//--------------Matrix_operatorTest_4 - Does 4x4 addition work as expected?
   		d += f; // 4x4 addition
   		for(int i = 0; i < d.rows(); i++)
      			for(int j = 0; j < d.cols(); j++)
				//cout << d(i,j) << " " << 16-i-4*j+7 << endl;
         			if (16-i-4*j+7 != d(i,j)) {badCount++;}
        testFramework.assert(badCount==0);
        testFramework.next();
      	badCount = 0; // Reset error counter
   		d -= f; // 4x4 reset d

		return testFramework.countFails();
	}
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	Matrix_T testClass;

	check = testClass.sizeTest();
	errorCounter += check;

	check = testClass.getTest();
	errorCounter += check;

	check = testClass.operatorTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
