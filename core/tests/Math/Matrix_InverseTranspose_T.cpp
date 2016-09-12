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

int Matrix_T::inverseTest(void)
{
	gpstk::TestUtil testFramework("Matrix", "Inverse", __FILE__, __LINE__);

	gpstk::Matrix<double> A1inv(2,2),A2inv(3,3),A3inv(4,4),A4inv(3,4);
	gpstk::Matrix<double> CompareA1inv(2,2),CompareA2inv(3,3),CompareA3inv(4,4),CompareA4inv(3,4);
	A1inv = gpstk::inverse(A1);
	A2inv = gpstk::inverse(A2);
	A3inv = gpstk::inverse(A3);
	try{A4inv = gpstk::inverse(A4); testFramework.assert(false, failMesg, __LINE__);}
	catch(gpstk::Exception e) {testFramework.assert(true, failMesg, __LINE__);}

	double temp1[4] = {-7,-5,3,2};
	double temp2[9] = {7./3,2./3,2./3,-17./3,-1./3,-4./3,2./3,1./3,1./3};
	double temp3[16] = {18, -35, -28, 1, 9, -18, -14, 1, -2, 4, 3, 0, -12, 24, 19, -1};

	CompareA1inv = temp1;
	CompareA2inv = temp2;
	CompareA3inv = temp3;

	int badCount = 0;

	//testFramework.assert(Ainv == CompareAinv, testMesg, __LINE__);
	for(int i = 0; i < A1inv.rows(); i++)
		for(int j = 0; j < A1inv.cols(); j++)
     			if (std::abs(A1inv(i,j) - CompareA1inv(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if gpstk::inverse(A1) returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter

	for(int i = 0; i < A2inv.rows(); i++)
		for(int j = 0; j < A2inv.cols(); j++)
     			if (std::abs(A2inv(i,j) - CompareA2inv(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if gpstk::inverse(A2) returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter

	for(int i = 0; i < A3inv.rows(); i++)
		for(int j = 0; j < A3inv.cols(); j++)
     			if (std::abs(A3inv(i,j) - CompareA3inv(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if gpstk::inverse(A3) returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter      	      	

  	return testFramework.countFails();
}

int Matrix_T::transposeTest(void)
{
	gpstk::TestUtil testFramework("Matrix", "Transpose", __FILE__, __LINE__);

	gpstk::Matrix<double> A1T(2,2),A2T(3,3),A3T(4,4),A4T(4,5);
	gpstk::Matrix<double> CompareA1T(2,2),CompareA2T(3,3),CompareA3T(4,4),CompareA4T(5,4);
	A1T = gpstk::transpose(A1);
	A2T = gpstk::transpose(A2);
	A3T = gpstk::transpose(A3);
	A4T = gpstk::transpose(A4);

	double temp4[4] = {2,-3,5,-7};
	double temp5[9] = {1,3,-5,0,1,-1,-2,-2,9};
	double temp6[16] = {2,1,0,0,3,0,2,2,1,3,-3,3,5,1,2,1};
	double temp7[20] = {8,7,1,-78,5,-9,7,24,18,5,10,20,-2,0,11,-68,1.5,7,47,0};		

	CompareA1T = temp4;
	CompareA2T = temp5;
	CompareA3T = temp6;
	CompareA4T = temp7;

	int badCount = 0;

	//testFramework.assert(AT == CompareAT, testMesg, __LINE__);
	for(int i = 0; i < A1T.rows(); i++)
		for(int j = 0; j < A1T.cols(); j++)
     			if (A1T(i,j) != CompareA1T(i,j)) {badCount++;}
	failDescriptionStream << "Check if gpstk::transpose(A1) returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter

	for(int i = 0; i < A2T.rows(); i++)
		for(int j = 0; j < A2T.cols(); j++)
     			if (A2T(i,j) != CompareA2T(i,j)) {badCount++;}
	failDescriptionStream << "Check if gpstk::transpose(A2) returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter

	for(int i = 0; i < A3T.rows(); i++)
		for(int j = 0; j < A3T.cols(); j++)
     			if (A3T(i,j) != CompareA3T(i,j)) {badCount++;}
	failDescriptionStream << "Check if gpstk::transpose(A3) returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter

	for(int i = 0; i < A4T.rows(); i++)
		for(int j = 0; j < A4T.cols(); j++)
     			if (A4T(i,j) != CompareA4T(i,j)) {badCount++;}
	failDescriptionStream << "Check if gpstk::transpose(A4) returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter      	

  	return testFramework.countFails();
  }


int Matrix_T::solutionTest(void)
{
	gpstk::TestUtil testFramework("Matrix", "Solution", __FILE__, __LINE__);

//Solution via Ainv*B, DEPENDENT ON INVERSE FUNCTION
//Is there another way to solve the system? Need to find A4sol still
	gpstk::Vector<double> A1sol(2), A2sol(3), A3sol(4);

	gpstk::Vector<double> CompareA1sol(2), CompareA2sol(3), CompareA3sol(4);

	A1sol = gpstk::inverse(A1) * B1;
	A2sol = gpstk::inverse(A2) * B2;
	A3sol = gpstk::inverse(A3) * B3;
	double temp8[2]= {-45,19};
	double temp9[3]= {17./3,-31./3,7./3};
	double temp10[4]= {-132,-65,15,89};
	CompareA1sol = temp8;
	CompareA2sol = temp9;
	CompareA3sol = temp10;

	int badCount = 0;

	//testFramework.assert(Asol == CompareAsol, failDescriptionString, __LINE__);
    for(int i = 0; i < A1sol.size(); i++)
    {
 	failMesg = "The solution calculated from A1inverse * b is incorrect";
    if (std::abs(CompareA1sol[i] - A1sol[i]) > eps) {badCount++;} //sizes mismatch, check till v1 ends
  	}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

    for(int i = 0; i < A2sol.size(); i++)
    {
 	failMesg = "The solution calculated from A2inverse * b is incorrect";
    if (std::abs(CompareA2sol[i] - A2sol[i]) > eps) {badCount++;} //sizes mismatch, check till v1 ends
  	}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

    for(int i = 0; i < A3sol.size(); i++)
    {
 	failMesg = "The solution calculated from A3inverse * b is incorrect";
    if (std::abs(CompareA3sol[i] - A3sol[i]) > eps) {badCount++; std::cout<<(CompareA3sol[i]-A3sol[i])<<std::endl;} //sizes mismatch, check till v1 ends
  	}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

  	return testFramework.countFails();
}


int Matrix_T::determinantTest(void)
{
	gpstk::TestUtil testFramework("Matrix", "Determinant", __FILE__, __LINE__);

  	double CompareDetA1 = 1.0;
  	double CompareDetA2 = 3.0;
  	double CompareDetA3 = 1.0;

  	failMesg = "The calculated determinant is incorrect";
  	testFramework.assert(std::abs(gpstk::det(A1) - CompareDetA1) < eps, failMesg, __LINE__);
  	testFramework.assert(std::abs(gpstk::det(A2) - CompareDetA2) < eps, failMesg, __LINE__);
  	testFramework.assert(std::abs(gpstk::det(A3) - CompareDetA3) < eps, failMesg, __LINE__);

  	return testFramework.countFails();
}

int main() //Main function to initialize and run all tests above
{

	int check, errorCounter = 0;
	Matrix_T testClass;

	check = testClass.inverseTest();
	errorCounter += check;

	check = testClass.transposeTest();
	errorCounter += check;

	check = testClass.solutionTest();
	errorCounter += check;

	check = testClass.determinantTest();
	errorCounter +=  check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
