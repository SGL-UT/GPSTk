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

#include "Matrix_T.hpp"

/* Test to check the sizing functions */
int Matrix_T::sizeTest (void)
{
	gpstk::TestUtil testFramework("Matrix","Size",__FILE__,__LINE__);
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

int main()
{
	int check, errorCounter = 0;
	Matrix_T testClass;

	check = testClass.sizeTest();

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
