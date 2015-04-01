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

//Householder
//Only tests if matrix is Upper triangular!
      	//try using positive definite matrices for precise answers

gpstk::Householder<double> HHA5, HHA6, HHA7, HHA8, HHA9;
gpstk::Matrix<double> A5, A6, A7, A8, A9;
gpstk::Matrix<double> CompareHHA5, CompareHHA6, CompareHHA7, CompareHHA8, CompareHHA9;

void HHinitializer(void)
{
	gpstk::Matrix<double> A5Temp(3,3), A6Temp(3,3), A7Temp(3,3), A8Temp(2,2), A9Temp(4,4);

	gpstk::Matrix<double> CompareHHA1Temp(2,2), CompareHHA2Temp(3,3), CompareHHA3Temp(4,4),
		CompareHHA5Temp(3,3), CompareHHA6Temp(3,3), CompareHHA7Temp(3,3), CompareHHA8Temp(2,2), CompareHHA9Temp(4,4);
		
	//A5
  	double temp5[9] = {1,1,0,
                       1,0,1,
                       0,1,1};
    //CompA5
  	double temp51[9] = {pow(2,.5), pow(2,-.5), pow(2,-.5), 
  						0, pow(1.5,.5), pow(6,-.5),
  						0,0,2./pow(3,.5)};
  	//A6
  	double temp6[9] = {12,-51,4,
  					   6,167,-68,
  					  -4,24,41};
	//Comp6
  	double temp61[9] = {14,21,-14,
  						0,175,-70,
  						0,0,-35};
  	//A7
  	double temp7[9] = {1,2,3,
                      -1,0,-3,
                       0,-2,3};
    //CompA7
  	double temp71[9] = {-1*pow(2,.5),-1*pow(2,.5),-1*pow(18,.5),
  						0,-1*pow(6,.5),pow(6,.5),
  						0,0,pow(3,.5)};
  	//A8
  	double temp8[4] = {6,5,
  					  -5,7}; //A8
  	//CompA8
  	double temp81[4] = {pow(61, .5), -5./pow(61, .5),
  						0, 67./pow(61, .5)};
  	//A9
  	double temp9[16] = {2,1,0,0,
  					   1,2,1,0,
  					   	0,1,2,1,
  					   	0,0,1,2};
	//CompA9
  	double temp91[16] = {pow(5, .5), 4./pow(5, .5), 1./pow(5, .5), 0,
						0, pow(14./5, .5), 3*pow(2./35, .5)+pow(10./7, .5), pow(5./14, .5),
						0, 0, pow(15./7, .5), 2*pow(3./35, .5)+2*pow(7./15, .5),
						0, 0, 0, pow(5./6, .5)};

	A5Temp=temp5; A6Temp=temp6; A7Temp=temp7; A8Temp=temp8; A9Temp=temp9;
	CompareHHA5Temp=temp51; CompareHHA6Temp=temp61; CompareHHA7Temp=temp71; CompareHHA8Temp=temp81; CompareHHA9Temp=temp91;

	A5=A5Temp; A6=A6Temp; A7=A7Temp; A8=A8Temp; A9=A9Temp;
	CompareHHA5=CompareHHA5Temp; CompareHHA6=CompareHHA6Temp; CompareHHA7=CompareHHA7Temp;
	CompareHHA8=CompareHHA8Temp; CompareHHA9=CompareHHA9Temp;

  	HHA5(A5); HHA6(A6); HHA7(A7); HHA8(A8); HHA9(A9);
}
         
/*

      	failMesg = "Bleh";
      	testFramework.assert(std::abs(std::abs(gpstk::det(HHA1.A)) - std::abs(gpstk::det(A7))) > eps, failMesg, __LINE__);
      	testFramework.assert(std::abs(std::abs(gpstk::det(HHA2.A)) - std::abs(gpstk::det(A2))) > eps, failMesg, __LINE__);
      	testFramework.assert(std::abs(std::abs(gpstk::det(HHA3.A)) - std::abs(gpstk::det(A3))) > eps, failMesg, __LINE__);
      	//testFramework.assert(std::abs(std::abs(gpstk::det(HHA4.A)) - std::abs(gpstk::det(A4)) > eps, failMesg, __LINE__);
*/

int Matrix_T::HHSquareTest(void)
{
	TestUtil testFramework("Matrix Householder", "Square", __FILE__, __LINE__);

  	//R matrix (obtained by Householder transformation) of QR decomposition should be a square
  	failMesg = "Check if Householder Transformation of A5 is square.";
  	if (HHA5.A.rows() != HHA5.A.cols()) testFramework.assert(false, failMesg, __LINE__);
  	else testFramework.assert(true, failMesg, __LINE__);

  	failMesg = "Check if Householder Transformation of A6 is square.";
  	if (HHA6.A.rows() != HHA6.A.cols()) testFramework.assert(false, failMesg, __LINE__);
  	else testFramework.assert(true, failMesg, __LINE__);

  	failMesg = "Check if Householder Transformation of A7 is square.";
  	if (HHA7.A.rows() != HHA7.A.cols()) testFramework.assert(false, failMesg, __LINE__);
  	else testFramework.assert(true, failMesg, __LINE__);


  	return testFramework.countFails();
}

//Simply test if the matrix is in Upper triangular form
int Matrix_T::HHUpperTriangularTest(void)
{
	TestUtil testFramework("Matrix Householder", "Upper Triangular", __FILE__, __LINE__);

	int zeroCount, badCount = 0;

	zeroCount = 0;
  	for (int i = 0; i < HHA5.A.rows(); i++) {
  		for (int j = 0; j < HHA5.A.cols(); j++) {	 
  			if (j == zeroCount)
  				if (HHA5.A(i,j) == 0)
  					badCount++;
  			if (j < zeroCount)
  				if (HHA5.A(i,j) != 0)
  					badCount++;
  			}
  		zeroCount++;}
	failDescriptionStream << "Check if Householder Transformation of A5 is upper triangular. " << badCount << " of the elements below the diagonal are non-zero.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
    badCount = 0;

	zeroCount = 0;
  	for (int i = 0; i < HHA6.A.rows(); i++) {
  		for (int j = 0; j < HHA6.A.cols(); j++) {
  			if (j == zeroCount)
  				if (HHA6.A(i,j) == 0)
  					badCount++;
  			if (j < zeroCount)
  				if (HHA6.A(i,j) != 0)
  					badCount++;
  			}
  		zeroCount++;}
	failDescriptionStream << "Check if Householder Transformation of A6 is upper triangular. " << badCount << " of the elements below the diagonal are non-zero.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
    badCount = 0;

	zeroCount = 0;
  	for (int i = 0; i < HHA7.A.rows(); i++) {
  		for (int j = 0; j < HHA7.A.cols(); j++) {
  			if (j == zeroCount)
  				if (HHA7.A(i,j) == 0)
  					badCount++;
  			if (j < zeroCount)
  				if (HHA7.A(i,j) != 0)
  					badCount++;
  			}
  		zeroCount++;}
	failDescriptionStream << "Check if Householder Transformation of A7 is upper triangular. " << badCount << " of the elements below the diagonal are non-zero.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
    badCount = 0;

	zeroCount = 0;
  	for (int i = 0; i < HHA8.A.rows(); i++) {
  		for (int j = 0; j < HHA8.A.cols(); j++) {
  			if (j == zeroCount)
  				if (HHA8.A(i,j) == 0)
  					badCount++;
  			if (j < zeroCount)
  				if (HHA8.A(i,j) != 0)
  					badCount++;
  			}
  		zeroCount++;}
	failDescriptionStream << "Check if Householder Transformation of A8 is upper triangular. " << badCount << " of the elements below the diagonal are non-zero.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
    badCount = 0;

	zeroCount = 0;
  	for (int i = 0; i < HHA9.A.rows(); i++) {
  		for (int j = 0; j < HHA9.A.cols(); j++) {
  			if (j == zeroCount)
  				if (HHA9.A(i,j) == 0)
  					badCount++;
  			if (j < zeroCount)
  				if (HHA9.A(i,j) != 0)
  					badCount++;
  			}
  		zeroCount++;}
	failDescriptionStream << "Check if Householder Transformation of A9 is upper triangular. " << badCount << " of the elements below the diagonal are non-zero.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
    badCount = 0;

    return testFramework.countFails();
}

int Matrix_T::HHRMatrixTest(void)
{
	TestUtil testFramework("Matrix Householder", "R Matrix", __FILE__, __LINE__);

	int zeroCount, badCount = 0;
    int iRowNegCount, oRowNegCount;

  	zeroCount = 0;
    iRowNegCount = 0;
    oRowNegCount = 0;
	for(int i = 0; i < A5.rows(); i++) {
		for(int j = 0; j < A5.cols(); j++) {
			//if values is WRONG, determine if it's negative or just plain wrong
 			if ((std::abs(HHA5.A(i,j) - CompareHHA5(i,j)) > eps)) {
 				if (std::abs(-1 * HHA5.A(i,j) - CompareHHA5(i,j)) < eps)	         			//badCount++; Mark value as negative, not incorrect
     				iRowNegCount++;
 				else badCount++;
			}
			if (HHA5.A(i,j) == 0)
				zeroCount++;
 		}
 		//If all non-zero values in a row are negative, mark row as negative
 		if (iRowNegCount == A5.cols() - zeroCount) {oRowNegCount++;}
 		//Otherwise, all negative values become plain wrong
 		else if (iRowNegCount != 0) badCount += iRowNegCount;
 		zeroCount = 0;
 		iRowNegCount = 0;
 	}
	failDescriptionStream << "Check if Householder Transformation of A5 returns the right matrix. " << badCount << " of the elements are incorrect, and "<< oRowNegCount << " rows are multiplied by -1.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter         
     

  	//std::cout<<HHA5.A<<"\n\n"<<CompareHHA5<<"\n";

  	zeroCount = 0;
    iRowNegCount = 0;
    oRowNegCount = 0;
	for(int i = 0; i < A6.rows(); i++) {
		for(int j = 0; j < A6.cols(); j++) {
			//if values is WRONG, determine if it's negative or just plain wrong
 			if ((std::abs(HHA6.A(i,j) - CompareHHA6(i,j)) > eps)) {
 				if (std::abs(-1 * HHA6.A(i,j) - CompareHHA6(i,j)) < eps)	         			//badCount++; Mark value as negative, not incorrect
     				iRowNegCount++;
 				else badCount++;
			}
			if (HHA6.A(i,j) == 0)
				zeroCount++;
 		}
 		//If all non-zero values in a row are negative, mark row as negative
 		if (iRowNegCount == A6.cols() - zeroCount) {oRowNegCount++;}
 		//Otherwise, all negative values become plain wrong
 		else if (iRowNegCount != 0) badCount += iRowNegCount;
 		zeroCount = 0;
 		iRowNegCount = 0;
 	}
	failDescriptionStream << "Check if Householder Transformation of A6 returns the right matrix. " << badCount << " of the elements are incorrect, and "<< oRowNegCount << " rows are multiplied by -1.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter         


	std::cout<<HHA6.A<<"\n\n"<<CompareHHA6<<"\n";

  	zeroCount = 0;
    iRowNegCount = 0;
    oRowNegCount = 0;
	for(int i = 0; i < A7.rows(); i++) {
		for(int j = 0; j < A7.cols(); j++) {
			//if values is WRONG, determine if it's negative or just plain wrong
 			if ((std::abs(HHA7.A(i,j) - CompareHHA7(i,j)) > eps)) {
 				if (std::abs(-1 * HHA7.A(i,j) - CompareHHA7(i,j)) < eps)	         			//badCount++; Mark value as negative, not incorrect
     				iRowNegCount++;
 				else badCount++;
			}
			if (HHA7.A(i,j) == 0)
				zeroCount++;
 		}
 		//If all non-zero values in a row are negative, mark row as negative
 		if (iRowNegCount == A7.cols() - zeroCount) {oRowNegCount++;}
 		//Otherwise, all negative values become plain wrong
 		else if (iRowNegCount != 0) badCount += iRowNegCount;
 		zeroCount = 0;
 		iRowNegCount = 0;
 	}
	failDescriptionStream << "Check if Householder Transformation of A7 returns the right matrix. " << badCount << " of the elements are incorrect, and "<< oRowNegCount << " rows are multiplied by -1.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter         

	//std::cout<<HHA7.A<<"\n\n"<<CompareHHA7<<"\n";

  	zeroCount = 0;
    iRowNegCount = 0;
    oRowNegCount = 0;
	for(int i = 0; i < A8.rows(); i++) {
		for(int j = 0; j < A8.cols(); j++) {
			//if values is WRONG, determine if it's negative or just plain wrong
 			if ((std::abs(HHA8.A(i,j) - CompareHHA8(i,j)) > eps)) {
 				if (std::abs(-1 * HHA8.A(i,j) - CompareHHA8(i,j)) < eps)	         			//badCount++; Mark value as negative, not incorrect
     				iRowNegCount++;
 				else badCount++;
			}
			if (HHA8.A(i,j) == 0)
				zeroCount++;
 		}
 		//If all non-zero values in a row are negative, mark row as negative
 		if (iRowNegCount == A8.cols() - zeroCount) {oRowNegCount++;}
 		//Otherwise, all negative values become plain wrong
 		else if (iRowNegCount != 0) badCount += iRowNegCount;
 		zeroCount = 0;
 		iRowNegCount = 0;
 	}
	failDescriptionStream << "Check if Householder Transformation of A8 returns the right matrix. " << badCount << " of the elements are incorrect, and "<< oRowNegCount << " rows are multiplied by -1.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter         
     
	//std::cout<<HHA8.A<<"\n\n"<<CompareHHA8<<"\n";

  	zeroCount = 0;
    iRowNegCount = 0;
    oRowNegCount = 0;
	for(int i = 0; i < A8.rows(); i++) {
		for(int j = 0; j < A9.cols(); j++) {
			//if values is WRONG, determine if it's negative or just plain wrong
 			if ((std::abs(HHA9.A(i,j) - CompareHHA9(i,j)) > eps)) {
 				if (std::abs(-1 * HHA9.A(i,j) - CompareHHA9(i,j)) < eps)	         			//badCount++; Mark value as negative, not incorrect
     				iRowNegCount++;
 				else badCount++;
			}
			if (HHA9.A(i,j) == 0)
				zeroCount++;
 		}
 		//If all non-zero values in a row are negative, mark row as negative
 		if (iRowNegCount == A9.cols() - zeroCount) {oRowNegCount++;}
 		//Otherwise, all negative values become plain wrong
 		else if (iRowNegCount != 0) badCount += iRowNegCount;
 		zeroCount = 0;
 		iRowNegCount = 0;
 	}
	failDescriptionStream << "Check if Householder Transformation of A9 returns the right matrix. " << badCount << " of the elements are incorrect, and "<< oRowNegCount << " rows are multiplied by -1.";
	failDescriptionString = failDescriptionStream.str(); failDescriptionStream.str("");
    testFramework.assert(badCount==0, failDescriptionString, __LINE__);
  	badCount = 0; // Reset error counter         

	//std::cout<<HHA9.A<<"\n\n"<<CompareHHA9<<"\n";

  	return testFramework.countFails();
}

int main()
{
	Matrix_T testClass;
	int check, errorCounter = 0;

	HHinitializer();

	check = testClass.HHSquareTest();
	errorCounter += check;

	check = testClass.HHUpperTriangularTest();
	errorCounter += check;

	check = testClass.HHRMatrixTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
