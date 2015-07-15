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

gpstk::Matrix<double> A5, A6, A7, A8;
//gpstk::Vector<double> CompareSA5, CompareSA6, CompareSA7, CompareSA8;
//gpstk::Matrix<double> CompareUA5, CompareUA6, CompareUA7, CompareUA8,
//					  CompareVA5, CompareVA6, CompareVA7, CompareVA8;
gpstk::SVD<double> SVD5, SVD6, SVD7, SVD8;

/* Defines the A, U and V matrices and singular values for the SVD */
void SVDinitializer(void)
{
    
//=================================================================================================================
    //  Below details what the individual matrices of the SVD should be, but currently aren't used in this test

	gpstk::Matrix<double> A5Temp(2,2), A6Temp(2,3), A7Temp(3,2), A8Temp(3,3);
//	gpstk::Matrix<double> CompareUA5Temp(2,2), CompareUA6Temp(2,2), CompareUA7Temp(3,3), CompareUA8Temp(3,3),
//						  CompareVA5Temp(2,2), CompareVA6Temp(3,3), CompareVA7Temp(2,2), CompareVA8Temp(3,3);	

	double temp5[4] = {2,2,
						1,-1};
//	double temp5S[2] = {1,1};
//	double temp5U[4] = {2*pow(2,.5), 0, 0, pow(2,.5)};
//	double temp5V[4] = {pow(2,-.5), -1*pow(2,-.5), pow(2,-.5), pow(2,-.5)};
	double temp6[6] = {4, 11, 14, 8, 7, -2};
//	double temp6S[2] = {6*pow(10,.5), 10*pow(10,.5)};
//	double temp6U[4] = {3*pow(10,-.5), pow(10,-.5), pow(10,.5), -3*pow(10,-.5)};
//	double temp6V[9] = {1./3, -2./3, 2./3, 2./3, -1./3, -2./3, 2./3, 2./3, 1./3};
	double temp7[6] = {1, -1, -2, 2, 2, -2};
//	double temp7S[3] = {3*pow(2,.5), 0};
//	double temp7U[9] = {1./3, 2*pow(5,-.5), -2*pow(45,.5), -2./3, pow(5,-.5), 4*pow(45,-.5), 2./3, 0, 5*pow(45,-.5)};
//	double temp7V[4] = {pow(2,-.5), -1*pow(2,-.5), pow(2,-.5), pow(2,-.5)};
	double temp8[9] = {-1,2,2,
						2,2,-1,
						2,-1,2};
//	double temp8S[3] = {3,3,3};
//	double temp8U[9] = {-1./3, 2./3, 2./3, 2./3, 2./3, -1./3, 2./3, -1./3, 2./3};
//	double temp8V[9] = {1,0,0,0,1,0,0,0,1};		

	A5Temp = temp5; A6Temp = temp6; A7Temp = temp7; A8Temp = temp8;
//	CompareUA5Temp = temp5U; CompareUA6Temp = temp6U; CompareUA7Temp = temp7U; CompareUA8Temp = temp8U;
//	CompareVA5Temp = temp5V; CompareVA6Temp = temp6V; CompareVA7Temp = temp7V; CompareVA8Temp = temp8V;

	A5 = A5Temp; A6 = A6Temp; A7 = A7Temp; A8 = A8Temp;
//	CompareUA5 = CompareUA5Temp; CompareUA6 = CompareUA6Temp; CompareUA7 = CompareUA7Temp; CompareUA8 = CompareUA8Temp;
//	CompareVA5 = CompareVA5Temp; CompareVA6 = CompareVA6Temp; CompareVA7 = CompareVA7Temp; CompareVA8 = CompareVA8Temp;

//===================================================================================================================
    
	SVD5(A5); SVD6(A6); SVD7(A7); SVD8(A8);

//Sort s values in correct order
	SVD5.sort(true); SVD6.sort(true); SVD7.sort(true); SVD8.sort(true);
}

gpstk::Matrix<double> SMatrixGeneration(gpstk::Matrix<double> U, gpstk::Matrix<double> V, gpstk::Vector<double> vect)
{

	int sRows, sCols;
	sRows = U.rows(); sCols = V.rows();

	gpstk::Matrix<double> sMatrix(sRows,sCols);
	for (int i=0; i < (sRows); i++)
	{
		for (int j=0; j < (sCols); j++)
		{
			sMatrix(i,j) = 0;
			if (i == j) sMatrix(i,j) = vect(i);
		}
	}

	//std::cout<<U<<"\n\n"<<sMatrix<<"\n\n"<<V<<"\n\n";

	return U * sMatrix * V;
}

int Matrix_T::SVDATest(void)
{
	TestUtil testFramework("Matrix SVD", "U * S * V^T = A", __FILE__, __LINE__);
	int badCount = 0;

	gpstk::Matrix<double> CompareA5(2,2), CompareA6(2,3), CompareA7(3,2), CompareA8(3,3);

	CompareA5 = SMatrixGeneration(SVD5.U, SVD5.V, SVD5.S);
	CompareA6 = SMatrixGeneration(SVD6.U, SVD6.V, SVD6.S);
	CompareA7 = SMatrixGeneration(SVD7.U, SVD7.V, SVD7.S);
	CompareA8 = SMatrixGeneration(SVD8.U, SVD8.V, SVD8.S);

	//std::cout<<"A5------------------\n"<<SMatrixGeneration(SVD5.U, SVD5.V, SVD5.S)<<'\n'<<SVD5.det()<<"\n\n";
	//std::cout<<"A6------------------\n"<<SMatrixGeneration(SVD6.U, SVD6.V, SVD6.S)<<'\n'<<SVD6.det()<<"\n\n";
	//std::cout<<"A7------------------\n"<<SMatrixGeneration(SVD7.U, SVD7.V, SVD7.S)<<'\n'<<SVD7.det()<<"\n\n";
	//std::cout<<"A8------------------\n"<<SMatrixGeneration(SVD8.U, SVD8.V, SVD8.S)<<'\n'<<SVD8.det()<<"\n\n";

	for(int i = 0; i < A5.rows(); i++)
		for(int j = 0; j < A5.cols(); j++)
     		if (A5(i,j) != CompareA5(i,j)) {badCount++;}
	failDescriptionStream << "Check if SVD of A5 returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter

	for(int i = 0; i < A6.rows(); i++)
		for(int j = 0; j < A6.cols(); j++)
     		if (A6(i,j) != CompareA6(i,j)) {badCount++;}
	failDescriptionStream << "Check if SVD of A6 returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter	

	for(int i = 0; i < A7.rows(); i++)
		for(int j = 0; j < A7.cols(); j++)
     		if (A7(i,j) != CompareA7(i,j)) {badCount++;}
	failDescriptionStream << "Check if SVD of A7 returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter	

	for(int i = 0; i < A8.rows(); i++)
		for(int j = 0; j < A8.cols(); j++)
     		if (A8(i,j) != CompareA8(i,j)) {badCount++;}
	failDescriptionStream << "Check if SVD of A8 returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter
	
	return testFramework.countFails();
}

int Matrix_T::SVDBackSubTest(void)
{
	TestUtil testFramework("Matrix SVD", "BackSub", __FILE__, __LINE__);
	int badCount = 0;

	double temp5[2] = {2,1};
	double temp51[2] = {1,0};
	double temp6[2] = {7,-3};
	double temp61[2] = {4.25,1.5};
	double temp7[3] = {5,1,-2};
	double temp71[3] = {5,5,4};
	double temp8[3] = {1,2,1};
	double temp81[3] = {5./9, 5./9, 2./9};

	gpstk::Vector<double> B5(2), B6(2), B7(3), B8(3);
	gpstk::Vector<double> A5sol(2), A6sol(2), A7sol(3), A8sol(3);
	gpstk::Vector<double> CompareA5sol(2), CompareA6sol(2), CompareA7sol(3), CompareA8sol(3);

	B5 = temp5; B6 = temp6; B7 = temp7; B8 = temp8;
	CompareA5sol = temp51; CompareA6sol = temp61; CompareA7sol = temp71; CompareA8sol = temp81;

	SVD5.backSub(B5); SVD6.backSub(B6); SVD7.backSub(B7); SVD8.backSub(B8);
	A5sol = B5; A6sol = B6; A7sol = B7; A8sol = B8;

    for(int i = 0; i < A5sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of SVD is incorrect";
    if (std::abs(CompareA5sol[i] - A5sol[i]) > eps) {badCount++;}}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

//Can't backsub accurately for non-square matrices

/*
    for(int i = 0; i < A6sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of SVD is incorrect";
    if (std::abs(CompareA6sol[i] - A6sol[i]) > eps) {badCount++;}}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter
  	std::cout<<A6sol<<std::endl;

    for(int i = 0; i < A7sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of SVD is incorrect";
    if (std::abs(CompareA7sol[i] - A7sol[i]) > eps) {badCount++;}}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter
  	std::cout<<A7sol<<std::endl;
*/

    for(int i = 0; i < A8sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of SVD is incorrect";
    if (std::abs(CompareA8sol[i] - A8sol[i]) > eps) {badCount++;}}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter


	return testFramework.countFails();
}

int Matrix_T::SVDDeterminantTest(void)
{
	TestUtil testFramework("Matrix SVD", "SVD Determinant", __FILE__, __LINE__);

	failMesg = "Calculated determinant by SVD was incorrect for A5";
	testFramework.assert((std::abs(SVD5.det() - (-4)) < eps), failMesg, __LINE__);
	failMesg = "Able to take determinant of non-square matrix with SVD.";
	try{SVD6.det(); testFramework.assert(false, failMesg, __LINE__);}
	catch(gpstk::Exception e) {testFramework.assert(true, failMesg, __LINE__);}
	try{SVD6.det(); testFramework.assert(false, failMesg, __LINE__);}
	catch(gpstk::Exception e) {testFramework.assert(true, failMesg, __LINE__);}
	failMesg = "Calculated determinant by SVD was incorrect for A8";
	testFramework.assert((std::abs(SVD8.det() - (-27)) < eps), failMesg, __LINE__);

	return testFramework.countFails();
}

int main(void)
{
	Matrix_T testClass;
	int check, errorCounter = 0;

	SVDinitializer();

	check = testClass.SVDATest();
	errorCounter += check;

	check = testClass.SVDBackSubTest();
	errorCounter += check;

	check = testClass.SVDDeterminantTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}