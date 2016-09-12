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


//LU
    //Testing determinant of LU compared to A
    //Testing P * (L * U) = A

gpstk::Matrix<double> L1, U1, L2, U2, L3, U3;
gpstk::Matrix<double> CompareLUA1, CompareLUA2, CompareLUA3;
gpstk::LUDecomp<double> LUA1, LUA2, LUA3, LUA4;
gpstk::Matrix<double> P1, P2, P3;

//-------------------------------------------------------------------------------
//Helper functions, don't actually run any tests
void LMatrixGeneration(gpstk::Matrix<double>& Lno, gpstk::LUDecomp<double> LUno)
{
	int zeroCount = Lno.cols() - 1;
	for (int i = 0; i < Lno.rows(); i++) {
        for (int j = 0; j < Lno.cols(); j++)
        //Where a 0 should be in L of LU
      		if (j > Lno.cols() - zeroCount - 1)
        		Lno(i,j) = 0;
		//Where a 1 should be in L of LU
      		else if (j == Lno.cols() - zeroCount - 1)
            	Lno(i,j) = 1;
        //Value in L of LU
        	else if(j < Lno.cols() - zeroCount - 1)
        		Lno(i,j) = LUno.LU(i,j);
 			zeroCount -= 1;}
}

void UMatrixGeneration(gpstk::Matrix<double>& Uno, gpstk::LUDecomp<double> LUno)
{
	int zeroCount = 0;
	for (int i = 0; i< Uno.rows(); i++) {
		for (int j = 0; j < Uno.cols(); j++)
			//Value in U of LU
	  		if (j >=  zeroCount)
	    		Uno(i,j) = LUno.LU(i,j);
			//Where a 0 should be in U of LU
			else if (j < zeroCount)
				Uno(i,j) = 0;
	zeroCount += 1;}
}

void PermuationMatrixGeneration(gpstk::Matrix<double>& Pno, gpstk::LUDecomp<double>& LUno)
{
	//Identity Matrix Construction
	int iCount = 0;
	for (int i = 0; i < Pno.rows(); i++)
		for (int j = 0; j < Pno.cols(); j++)
		if ((j == iCount) && (i == iCount)) {
	    	Pno(i,j) = 1;
	    	iCount += 1;}
	  	else
	    	Pno(i,j) = 0;

	//Make Identity Martix the Permuation Matrix
	for (int i = 0; i < LUno.Pivot.size(); i++)
		Pno.swapRows(i, LUno.Pivot[i]);
}

void LUDecompInitializer(void)
{
	gpstk::Matrix<double> L1Temp(2,2), U1Temp(2,2), L2Temp(3,3), U2Temp(3,3), L3Temp(4,4), U3Temp(4,4);
	gpstk::Matrix<double> CompareLUA1Temp(2,2), CompareLUA2Temp(3,3), CompareLUA3Temp(4,4);
	gpstk::Matrix<double> P1Temp(2,2), P2Temp(3,3), P3Temp(4,4);

	L1 = L1Temp; U1 = U1Temp; L2 = L2Temp; U2 = U2Temp; L3 = L3Temp; U3 = U3Temp;
	CompareLUA1 = CompareLUA1Temp; CompareLUA2 = CompareLUA2Temp; CompareLUA3 = CompareLUA3Temp;
	P1 = P1Temp; P2 = P2Temp; P3 = P3Temp;

	//L matrix generation
	LMatrixGeneration(L1, LUA1);
	LMatrixGeneration(L2, LUA2);
	LMatrixGeneration(L3, LUA3);

	//U matrix generation
	UMatrixGeneration(U1, LUA1);
	UMatrixGeneration(U2, LUA2);
	UMatrixGeneration(U3, LUA3);

	//P matrix generation
	PermuationMatrixGeneration(P1, LUA1);
	PermuationMatrixGeneration(P2, LUA2);
	PermuationMatrixGeneration(P3, LUA3);

	CompareLUA1 = P1 * (L1 * U1);
	CompareLUA2 = P2 * (L2 * U2);
	CompareLUA3 = P3 * (L3 * U3);		
}

//-------------------------------------------------------------------------------
//Now the tests start

int Matrix_T::LUinitializationTest(void)
{
	gpstk::TestUtil testFramework("Matrix LU","LU initialize", __FILE__, __LINE__);

	LUA1(A1); LUA2(A2); LUA3(A3);
	failMesg = "Able to perform LU decomposition on non-square matrix";
	try{LUA4(A4); testFramework.assert(false, failMesg, __LINE__);}
	catch(gpstk::Exception e) {testFramework.assert(true, failMesg, __LINE__);}\

	return testFramework.countFails();
}

int Matrix_T::LUdeterminantTest(void)
{
	gpstk::TestUtil testFramework("Matrix LU","LU Determinant", __FILE__, __LINE__);

	failMesg = "The LU decomposition's determinant is not equivalent to the determinant of the data matrix";
	testFramework.assert(std::abs(LUA1.det() - gpstk::det(A1)) < eps, failMesg, __LINE__);
	testFramework.assert(std::abs(LUA2.det() - gpstk::det(A2)) < eps, failMesg, __LINE__);
	testFramework.assert(std::abs(LUA3.det() - gpstk::det(A3)) < eps, failMesg, __LINE__);

	return testFramework.countFails();
}

int Matrix_T::LUATest(void)
{
	gpstk::TestUtil testFramework("Matrix LU","P * (L * U) = A", __FILE__, __LINE__);

	std::cout<<U1<<"\n\n"<<A1<<std::endl;

	//testFramework.assert( P1 * (L1 * U1) == A1)
	int badCount = 0;
	for(int i = 0; i < A1.rows(); i++)
		for(int j = 0; j < A1.cols(); j++)
     		if (A1(i,j) != CompareLUA1(i,j)) {badCount++;}
	failDescriptionStream << "Check if LU decomposition of A1 returns the right matrix. " << badCount << " of the elements are incorrect.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter

	for(int i = 0; i < A2.rows(); i++)
    	for(int j = 0; j < A2.cols(); j++)
   			if (A2(i,j) != CompareLUA2(i,j)) {badCount++;}
	failDescriptionStream << "Check if LU decomposition of A2 returns the right matrix. " << badCount << " of the elements are incorrect.";
   	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
  	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter  

	for(int i = 0; i < A3.rows(); i++)
	    for(int j = 0; j < A3.cols(); j++)
 			if (A3(i,j) != CompareLUA3(i,j)) {badCount++;}
   	failDescriptionStream << "Check if LU decomposition of A3 returns the right matrix. " << badCount << " of the elements are incorrect.";
  	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
  	testFramework.assert(badCount==0, failDescriptionString, __LINE__);
	badCount = 0; // Reset error counter          	

	return testFramework.countFails();
}

int Matrix_T::LUbackSubTest(void)
{
	gpstk::TestUtil testFramework("Matrix LU","LU Determinant", __FILE__, __LINE__);

  	//backSub overwrites input vectors
  	LUA1.backSub(B1);
  	LUA2.backSub(B2);
  	LUA3.backSub(B3);

  	gpstk::Vector<double> A1sol(2), A2sol(3), A3sol(4);
  	gpstk::Vector<double> CompareA1sol(2), CompareA2sol(3), CompareA3sol(4);

  	A1sol = B1;
  	A2sol = B2;
  	A3sol = B3;
  	double temp1[2]= {-45,19};
	double temp2[3]= {17./3,-31./3,7./3};
	double temp3[4]= {-132,-65,15,89};
	CompareA1sol = temp1;
	CompareA2sol = temp2;
	CompareA3sol = temp3;

	//testFramework.assert(Asol == CompareAsol, failDescriptionString, __LINE__);
    int badCount = 0;
    for(int i = 0; i < A1sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of LU decomposition is incorrect";
    if (std::abs(CompareA1sol[i] - A1sol[i]) > eps) {badCount++;} //sizes mismatch, check till v1 ends
  	}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

    for(int i = 0; i < A2sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of LU decomposition is incorrect";
    if (std::abs(CompareA2sol[i] - A2sol[i]) > eps) {badCount++;} //sizes mismatch, check till v1 ends
  	}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

    for(int i = 0; i < A3sol.size(); i++)
    {
 	failMesg = "The solution calculated from back subsitution of LU decomposition is incorrect";
    if (std::abs(CompareA3sol[i] - A3sol[i]) > eps) {badCount++; std::cout<<(CompareA3sol[i]-A3sol[i])<<std::endl;} //sizes mismatch, check till v1 ends
  	}
  	testFramework.assert(badCount==0, failMesg, __LINE__);
  	badCount = 0; // Reset error counter

	return testFramework.countFails();
}

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	Matrix_T testClass;

	check = testClass.LUinitializationTest(); //runs the gpstk::LUDecomp
	errorCounter += check;

	LUDecompInitializer(); //seperates gpstk::LUDecomps into P L and U matrices

	check = testClass.LUdeterminantTest();
	errorCounter += check;

	check = testClass.LUATest();
	errorCounter += check;

	check = testClass.LUbackSubTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
