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

/* Test the accessor operator */
#include "Matrix_T.hpp"

//Constants
int Matrix_T::InitializeConstantsTest (void)
{
	TestUtil testFramework("Matrix","Constant Constructor",__FILE__,__LINE__);
  	std::string failMesg;
  	int badCount = 0;


	//4 matrices filled with constants
	gpstk::Matrix<int> a(2,2,1);
	gpstk::Matrix<int> b(8, 2, 3);
	gpstk::Matrix<int> c(4, 2, 5);
	gpstk::Matrix<int> d(4, 4, 7);

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

  	return testFramework.countFails();
}

//-------------------------------------------------------------------------
//gpstk::Vectors -- sort of implicitly tests intialization from array
int Matrix_T::InitializeVectorsTest (void)
{
	TestUtil testFramework("Matrix","Vector Constructor",__FILE__,__LINE__);
	int badCount = 0;

  	//Initialize 4 gpstk::Vectors
	gpstk::Vector<int> v1 = gpstk::Vector<int>(16);
	for(int i = 0; i < 16; i++) v1[i] = i+1;
	gpstk::Vector<int> v2 = gpstk::Vector<int>(16);
	for(int i = 0; i < 16; i++) v2[i] = 16-i;
	gpstk::Vector<int> v3 = gpstk::Vector<int>(4);
	for(int i = 0; i < 4; i++) v3[i] = i+1;
	gpstk::Vector<int> v4 = gpstk::Vector<int>(8);
	for(int i = 0; i < 4; i++) {v4[i] = i+1+4; v4[i+4] = i+1+4;}

	//Make matrices from gpstk::Vectors
	gpstk::Matrix<int> e(8, 2, v1);
	gpstk::Matrix<int> f(4, 4, v2);
	gpstk::Matrix<int> g(2, 2, v3);
	gpstk::Matrix<int> h(4, 2, v4);


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

//add sub Matrix constructor


int main()
{
	int check, errorCounter = 0;
	Matrix_T testClass;

	check = testClass.InitializeConstantsTest();
	errorCounter += check;

	check = testClass.InitializeVectorsTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
