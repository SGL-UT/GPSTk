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

gpstk::Cholesky<double> C5, C6, C7, C8, C4;
gpstk::Matrix<double> A5, A6, A7, A8;
gpstk::Matrix<double> CompareChA5, CompareChLA5, CompareChUA5, CompareChA6, CompareChLA6,
					CompareChUA6, CompareChA7, CompareChLA7, CompareChUA7;

void CholeskyInitializer(void)
{
	gpstk::Matrix<double> CompareChA5Temp(2,2), CompareChLA5Temp(2,2), CompareChUA5Temp(2,2),
	        CompareChA6Temp(3,3), CompareChLA6Temp(3,3), CompareChUA6Temp(3,3),
	        CompareChA7Temp(4,4), CompareChLA7Temp(4,4), CompareChUA7Temp(4,4);
	gpstk::Matrix<double> A5Temp(2,2), A6Temp(3,3), A7Temp(4,4);

	//needs to be symmetric positive definite
	double temp5[4] = {2,1,1,2};
	double temp51[4] = {pow(2,.5), 0, pow(2,-0.5), pow(1.5,.5)};
	double temp52[4] = {pow(2,.5), pow(2,-0.5), 0, pow(1.5,.5)};
	A5Temp = temp5;
	CompareChLA5Temp = temp51; CompareChUA5Temp = temp52;

	double temp6[9] = {2,-1,0,-1,2,-1,0,-1,2};
	double temp61[9] = {pow(2,.5),0,0,-1*pow(2,-.5),pow(1.5,.5),0,0,-1*pow(2./3,.5),pow(4./3,.5)};
	double temp62[9] = {pow(2,.5),-1*pow(2,-.5),0,0,pow(1.5,.5),-1*pow(2./3,.5),0,0,pow(4./3,.5)};
	A6Temp = temp6;
	CompareChLA6Temp = temp61; CompareChUA6Temp = temp62;

	double temp7[16] = {2,-1,0,0,-1,2,-1,0,0,-1,2,-1,0,0,-1,2};
	double temp71[16] = {pow(2,.5),0,0,0,-1*pow(2,-.5),pow(1.5,.5),0,0,0,-1*pow(2./3,.5),2*pow(3,-.5),0,0,0,-1*pow(3./4,.5),pow(5./4,.5)};
	double temp72[16] = {pow(2,.5),-1*pow(2,-.5),0,0,0,pow(1.5,.5),-1*pow(2./3,.5),0,0,0,2*pow(3,-.5), -1*pow(3./4,.5),0,0,0,pow(5./4,.5)};
	A7Temp = temp7;
	CompareChLA7Temp = temp71; CompareChUA7Temp = temp72;

	A5 = A5Temp; A6 = A6Temp; A7 = A7Temp;
	CompareChA5 = CompareChA5Temp; CompareChA6 = CompareChA6Temp; CompareChA7 = CompareChA7Temp;
	CompareChLA5 = CompareChLA5Temp; CompareChLA6 = CompareChLA6Temp; CompareChLA7 = CompareChLA7Temp;
	CompareChUA5 = CompareChUA5Temp; CompareChUA6 = CompareChUA6Temp; CompareChUA7 = CompareChUA7Temp;
}

int Matrix_T::CholeskyInitializationTest()
{
	TestUtil testFramework("Matrix Cholesky", "Square decomposition", __FILE__, __LINE__);

	C5(A5);
	C6(A6);
	C7(A7);
	failMesg = "Was able to perform Cholesky Factorization on non-square matrix";
	try {C4(A4); testFramework.assert(false, failMesg, __LINE__);}
	catch(gpstk::MatrixException e) {testFramework.assert(true, failMesg, __LINE__);}

	return testFramework.countFails();
}

int Matrix_T::CholeskyLTest()
{
	TestUtil testFramework("Matrix Cholesky", "L Matrix Check", __FILE__, __LINE__);
	int badCount = 0;

	for(int i = 0; i < A5.rows(); i++)
		for(int j = 0; j < A5.cols(); j++)
			if (std::abs(C5.L(i,j) - CompareChLA5(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if Cholesky factor L of A5 is correct. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter

	std::cout<<C5.L<<"\n\n"<<CompareChLA5<<"\n\n";

	for(int i = 0; i < A6.rows(); i++)
		for(int j = 0; j < A6.cols(); j++)
			if (std::abs(C6.L(i,j) - CompareChLA6(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if Cholesky factor L of A6 is correct. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter

	std::cout<<C6.L<<"\n\n"<<CompareChLA6<<"\n\n";

	for(int i = 0; i < A7.rows(); i++)
		for(int j = 0; j < A7.cols(); j++)
			if (std::abs(C7.L(i,j) - CompareChLA7(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if Cholesky factor L of A7 is correct. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter

	std::cout<<C7.L<<"\n\n"<<CompareChLA7<<"\n\n";

	return testFramework.countFails();
}

int Matrix_T::CholeskyUTest()
{
	TestUtil testFramework("Matrix Cholesky", "U Matrix Check", __FILE__, __LINE__);
	int badCount = 0;

	for(int i = 0; i < A5.rows(); i++)
		for(int j = 0; j < A5.cols(); j++)
			if (std::abs(C5.U(i,j) - CompareChUA5(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if Cholesky factor U of A5 is correct. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter

	std::cout<<C5.U<<"\n\n"<<CompareChUA5<<"\n\n";

	for(int i = 0; i < A6.rows(); i++)
		for(int j = 0; j < A6.cols(); j++)
			if (std::abs(C6.U(i,j) - CompareChUA6(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if Cholesky factor U of A6 is correct. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter

	std::cout<<C6.U<<"\n\n"<<CompareChUA6<<"\n\n";

	for(int i = 0; i < A7.rows(); i++)
		for(int j = 0; j < A7.cols(); j++)
			if (std::abs(C7.U(i,j) - CompareChUA7(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if Cholesky factor U of A7 is correct. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter

	std::cout<<C7.U<<"\n\n"<<CompareChUA7<<"\n\n";

	return testFramework.countFails();
}

int Matrix_T::CholeskyLUTest()
{
	TestUtil testFramework("Matrix Cholesky", "L * U = A", __FILE__, __LINE__);
	int badCount = 0;

	CompareChA5 = C5.L * C5.U;
	CompareChA6 = C6.L * C6.U;
	CompareChA7 = C7.L * C7.U;

	for(int i = 0; i < A5.rows(); i++)
		for(int j = 0; j < A5.cols(); j++)
			if (std::abs(A5(i,j) - CompareChA5(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if Cholesky decomposition L * U = A5 is correct. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter

	std::cout<<A5<<"\n\n"<<CompareChA5<<"\n\n";

	for(int i = 0; i < A6.rows(); i++)
		for(int j = 0; j < A6.cols(); j++)
			if (std::abs(A6(i,j) - CompareChA6(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if Cholesky decomposition L * U = A6 is correct. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter

	std::cout<<A6<<"\n\n"<<CompareChA6<<"\n\n";

	for(int i = 0; i < A7.rows(); i++)
		for(int j = 0; j < A7.cols(); j++)
			if (std::abs(A7(i,j) - CompareChA7(i,j)) > eps) {badCount++;}
	failDescriptionStream << "Check if Cholesky decomposition L * U = A7 is correct. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter

	std::cout<<A7<<"\n\n"<<CompareChA7<<"\n\n";

	return testFramework.countFails();
}

int Matrix_T::CholeskyBackSubTest()
{
	TestUtil testFramework("Matrix Cholesky", "BackSub", __FILE__, __LINE__);
	int badCount = 0;

	double temp5[2] = {1,2};
	double temp51[2] = {0,1};
	double temp6[3] = {7,-3,2};
	double temp61[3] = {4.25,1.5,1.75};
	double temp7[4] = {5,1,-2,6};
	double temp71[4] ={5,5,4,5}; 

	gpstk::Vector<double> B5(2), B6(3), B7(4);
	gpstk::Vector<double> A5sol(2), A6sol(3), A7sol(4);
	gpstk::Vector<double> CompareA5sol(2), CompareA6sol(3), CompareA7sol(4);

	B5 = temp5; B6 = temp6; B7 = temp7;
	CompareA5sol = temp51; CompareA6sol = temp61; CompareA7sol = temp71;

	C5.backSub(B5); C6.backSub(B6); C7.backSub(B7);
	A5sol = B5; A6sol = B6; A7sol = B7;

    for(int i = 0; i < A5sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of Cholesky decomposition is incorrect";
    if (std::abs(CompareA5sol[i] - A5sol[i]) > eps) {badCount++;}}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

    for(int i = 0; i < A6sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of Cholesky decomposition is incorrect";
    if (std::abs(CompareA6sol[i] - A6sol[i]) > eps) {badCount++;}}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

    for(int i = 0; i < A7sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of Cholesky decomposition is incorrect";
    if (std::abs(CompareA7sol[i] - A7sol[i]) > eps) {badCount++;}}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

	return testFramework.countFails();
}

int main()
{
	Matrix_T testClass;
	int check, errorCounter = 0;

	CholeskyInitializer();

	check = testClass.CholeskyInitializationTest();
	errorCounter += check;

	check = testClass.CholeskyLTest();
	errorCounter += check;

	check = testClass.CholeskyUTest();
	errorCounter += check;

	check = testClass.CholeskyLUTest();
	errorCounter += check;

	check = testClass.CholeskyBackSubTest();

	//still need to test backsub

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
