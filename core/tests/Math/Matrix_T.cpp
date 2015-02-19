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
#include <sstream>
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
		std::string failMesg;

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

		failMesg = "Does the size method function as expected?";
		testFramework.assert((size_t)4 == a.size(), failMesg, __LINE__);

		failMesg = "Does the rows method function as expected?";
		testFramework.assert((size_t)2 == a.rows(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)2 == a.cols(), failMesg, __LINE__);

		failMesg = "Does the size method function as expected?";
		testFramework.assert((size_t)16 == b.size(), failMesg, __LINE__);

		failMesg = "Does the rows method function as expected?";
		testFramework.assert((size_t)8 == b.rows(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)2 == b.cols(), failMesg, __LINE__);

		failMesg = "Does the size method function as expected?";
		testFramework.assert((size_t)8 == c.size(), failMesg, __LINE__);

		failMesg = "Does the rows method function as expected?";
		testFramework.assert((size_t)4 == c.rows(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)2 == c.cols(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)16 == d.size(), failMesg, __LINE__);

		failMesg = "Does the rows method function as expected?";
		testFramework.assert((size_t)4 == d.rows(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)4 == d.cols(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)16 == e.size(), failMesg, __LINE__);

		failMesg = "Does the rows method function as expected?";
		testFramework.assert((size_t)8 == e.rows(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)2 == e.cols(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)16 == f.size(), failMesg, __LINE__);

		failMesg = "Does the rows method function as expected?";
		testFramework.assert((size_t)4 == f.rows(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)4 == f.cols(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)4 == g.size(), failMesg, __LINE__);

		failMesg = "Does the rows method function as expected?";
		testFramework.assert((size_t)2 == g.rows(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)2 == g.cols(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)8 == h.size(), failMesg, __LINE__);

		failMesg = "Does the rows method function as expected?";
		testFramework.assert((size_t)4 == h.rows(), failMesg, __LINE__);

		failMesg = "Does the cols method function as expected?";
		testFramework.assert((size_t)2 == h.cols(), failMesg, __LINE__);
		
		return testFramework.countFails();
	}

	/* Test the accessor operator */
	int getTest (void)
	{
		TestUtil testFramework("Matrix","getTest",__FILE__,__LINE__);
      	std::string failMesg;
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

		failMesg = "Is matrix a initializated properly?";
   		for(int i = 0; i < a.rows(); i++)
      			for(int j = 0; j < a.cols(); j++)
         			if (1 != a(i,j)) {badCount++;}
        testFramework.assert(badCount==0, failMesg, __LINE__);
      	badCount = 0; // Reset error counter

		failMesg = "Is matrix b initializated properly?";
   		for(int i = 0; i < b.rows(); i++)
      			for(int j = 0; j < b.cols(); j++)
         			if (3 != b(i,j)) {badCount++;}
        testFramework.assert(badCount==0, failMesg, __LINE__);
      	badCount = 0; // Reset error counter

		failMesg = "Is matrix c initializated properly?";
		for(int i = 0; i < c.rows(); i++)
		      	for(int j = 0; j < c.cols(); j++)
         			if (5 != c(i,j)) {badCount++;}
        testFramework.assert(badCount==0, failMesg, __LINE__);
      	badCount = 0; // Reset error counter

		failMesg = "Is matrix d initializated properly?";
		for(int i = 0; i < d.rows(); i++)
		      	for(int j = 0; j < d.cols(); j++)
					if (7 != d(i,j)) {badCount++;}
        testFramework.assert(badCount==0, failMesg, __LINE__);
      	badCount = 0; // Reset error counter

		for(int i = 0; i < e.rows(); i++)
		      	for(int j = 0; j < e.cols(); j++)
				if (v1(i*e.cols()+j) != e(i,j)) {badCount++;}

	failDescriptionStream << "Check to see if gpstk::Matrix set the gpstk::Vector of values properly into a 8x2 matrix. " << badCount << " of them are set improperly.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
        testFramework.assert(badCount==0, failDescriptionString, __LINE__);
      	badCount = 0; // Reset error counter

		for(int i = 0; i < f.rows(); i++)
		      	for(int j = 0; j < f.cols(); j++)
				if (v2(i*f.cols()+j) != f(i,j)) {badCount++;}
	failDescriptionStream << "Check to see if gpstk::Matrix set the gpstk::Vector of values properly into a 4x4 matrix. " << badCount << " of them are set improperly.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
        testFramework.assert(badCount==0, failDescriptionString, __LINE__);
      	badCount = 0; // Reset error counter

		for(int i = 0; i < g.rows(); i++)
		      	for(int j = 0; j < g.cols(); j++)
				if (v3(i*g.cols()+j) != g(i,j)) {badCount++;}
	failDescriptionStream << "Check to see if gpstk::Matrix set the gpstk::Vector of values properly into a 2x2 matrix. " << badCount << " of them are set improperly.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
        testFramework.assert(badCount==0, failDescriptionString, __LINE__);
      	badCount = 0; // Reset error counter

		for(int i = 0; i < h.rows(); i++)
		      	for(int j = 0; j < h.cols(); j++)
			 	if (v4(i*h.cols()+j) != h(i,j)) {badCount++;}
	failDescriptionStream << "Check to see if gpstk::Matrix set the gpstk::Vector of values properly into a 4x2 matrix. " << badCount << " of them are set improperly.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
        testFramework.assert(badCount==0, failDescriptionString, __LINE__);
      	badCount = 0; // Reset error counter

		return testFramework.countFails();
	}

	/* Test the add/subtract and set operators */
	int operatorTest (void)
	{
		TestUtil testFramework("Matrix","Operators",__FILE__,__LINE__);
      	std::string failMesg;
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
         			if (v3(i*g.cols()+j)+1 != a(i,j)) {badCount++;}
	failDescriptionStream << "Check to see if addition between two 2x2 gpstk::Matrix objects works. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
        testFramework.assert(badCount==0, failDescriptionString, __LINE__);
      	badCount = 0; // Reset error counter;
   		a -= g; // 2x2 reset a

//--------------Matrix_operatorTest_2 - Does 8x2 subtraction work as expected?
   		b -= e; // 8x2 subtraction
   			for(int i = 0; i < b.rows(); i++)
      				for(int j = 0; j < b.cols(); j++)
					//cout << b(i,j) << " " << 3-(1+i+j*8) << endl;
         				if (3-v1(i*e.cols()+j) != b(i,j)) {badCount++;}
	failDescriptionStream << "Check to see if subtraction between two 8x2 gpstk::Matrix objects works. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
        testFramework.assert(badCount==0, failDescriptionString, __LINE__);
      	badCount = 0; // Reset error counter;
   		b += e; // 8x2 reset b

//--------------Matrix_operatorTest_3 - Does 4x2 subtraction work as expected?
   		c -= h; // 4x2 subtraction
   		for(int i = 0; i < c.rows(); i++)
      			for(int j = 0; j < c.cols(); j++)
					//cout << c(i,j) << " " << 5-(5.+i) << endl;
         				if (5-v4(i*h.cols()+j) != c(i,j)) {badCount++;}
	failDescriptionStream << "Check to see if subtraction between two 4x2 gpstk::Matrix objects works. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
        testFramework.assert(badCount==0, failDescriptionString, __LINE__);
      	badCount = 0; // Reset error counter;
   		c += h; // 4x2 reset c

//--------------Matrix_operatorTest_4 - Does 4x4 addition work as expected?
   		d += f; // 4x4 addition
   		for(int i = 0; i < d.rows(); i++)
      			for(int j = 0; j < d.cols(); j++)
				//cout << d(i,j) << " " << 16-i-4*j+7 << endl;
         			if (v2(i*f.cols()+j)+7 != d(i,j)) {badCount++;}
	failDescriptionStream << "Check to see if addition between two 4x4 gpstk::Matrix objects works. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
        testFramework.assert(badCount==0, failDescriptionString, __LINE__);
      	badCount = 0; // Reset error counter
   		d -= f; // 4x4 reset d

		return testFramework.countFails();
	}
	private:
		std::stringstream failDescriptionStream;
		std::string       failDescriptionString;

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
