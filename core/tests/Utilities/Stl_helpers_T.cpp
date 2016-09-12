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

#include "stl_helpers.hpp"
#include "TestUtil.hpp"
#include <iostream>

class Stl_helpers_T
{
public: 
   Stl_helpers_T(){ eps = 1E-12;}// Default Constructor, set the precision value
   ~Stl_helpers_T() {} // Default Desructor

//========================================================================================
//	maxAndMinTest will check if max and min correctly identify maximum and minimum
//========================================================================================
   int maxAndMinTest(void) 
   {
      gpstk::TestUtil testFramework("stl_helpers", "Max and Min", __FILE__, __LINE__);
      std::string testMesg;

      std::list<int> ilist1;
      std::list<int> ilist2;
      std::list<float> flist1;
      std::list<float> flist2;

      for (int i=0; i<5; i++) {ilist1.push_back(i+1);};

      ilist2.push_back(54);
      ilist2.push_back(-90);
      ilist2.push_back(12);
      ilist2.push_back(100);
      ilist2.push_back(65);

      for (float i=0; i<5; i++) {flist1.push_back(i+1);};

      flist2.push_back(54);
      flist2.push_back(-90);
      flist2.push_back(12);
      flist2.push_back(100);
      flist2.push_back(65);


         //---------------------------------------------------------------------------
         //Verify max method functions correctly
         //---------------------------------------------------------------------------
      testMesg = "gpstk::max() did not return the maximum value of a integer";
      testFramework.assert(gpstk::max(ilist1) == 5, testMesg, __LINE__);
      testFramework.assert(gpstk::max(ilist2) == 100, testMesg, __LINE__);
      testMesg = "gpstk::max() did not return the maximum value of a float";
      testFramework.assert(fabs(gpstk::max(flist1) - 5) < eps, testMesg, __LINE__);
      testFramework.assert(fabs(gpstk::max(flist2) - 100) < eps, testMesg, __LINE__);
		
         //---------------------------------------------------------------------------
         //Verify min method functions correctly
         //---------------------------------------------------------------------------
      testMesg = "gpstk::min() did not return the minimum value of a integer";
      testFramework.assert(gpstk::min(ilist1) == 1, testMesg, __LINE__);
      testFramework.assert(gpstk::min(ilist2) == -90, testMesg, __LINE__);
      testMesg = "gpstk::min() did not return the minimum value of a float";
      testFramework.assert(fabs(gpstk::min(flist1) - 1) < eps, testMesg, __LINE__);		
      testFramework.assert(fabs(gpstk::min(flist2) + 90) < eps, testMesg, __LINE__);

      return testFramework.countFails();
   }

//=========================================================================================
//	statsTest will check if the stats methods correctly identify maximum and minimum
//=========================================================================================
   int statsTest(void)
   {
      gpstk::TestUtil testFramework("stl_helpers", "Stats", __FILE__, __LINE__);
      std::string testMesg;


      std::list<int> ilist1;
      std::list<float> flist1;

      for (int i=0; i<5; i++) {ilist1.push_back(i+1);};
      for (float i=0; i<5; i++) {flist1.push_back(i+1);};
		
      gpstk::stats<int> (ilist1);
      gpstk::stats<float> (flist1);

      float expectedN = 5.0;
      float expectedMean = 3.0;
      float expectedSigma = 2.5;
      expectedSigma = std::sqrt(2.5);

         //---------------------------------------------------------------------------
         //Verify n was calculated correctly
         //---------------------------------------------------------------------------
      testMesg = "The computed n value is incorrect for a integer";
      testFramework.assert(gpstk::stats<int>(ilist1).n == 5, testMesg, __LINE__);
      testMesg = "The computed n value is incorrect for a float";
      testFramework.assert(fabs(gpstk::stats<float>(flist1).n - expectedN) < eps, testMesg, __LINE__);

         //---------------------------------------------------------------------------
         //Verify the mean was calculated correctly
         //---------------------------------------------------------------------------
      testMesg = "The computed mean value is incorrect for a integer";
      testFramework.assert(gpstk::stats<int>(ilist1).mean == 3, testMesg, __LINE__);
      testMesg = "The computed mean value is incorrect for a float";
      testFramework.assert(fabs(gpstk::stats<float>(flist1).mean - expectedMean) < eps, testMesg, __LINE__);
		
         //---------------------------------------------------------------------------
         //Verify sigma was calculated correctly
         //---------------------------------------------------------------------------
      testMesg = "The computed sigma value is incorrect for a integer";
      testFramework.assert(gpstk::stats<int>(ilist1).sigma == 1, testMesg, __LINE__);
      testMesg = "The computed sigma value is incorrect for a float";
      testFramework.assert(fabs(gpstk::stats<float>(flist1).sigma - expectedSigma) < eps, testMesg, __LINE__);

      return testFramework.countFails();
   }
//==================================================================================================
//	vectorIndexTest will check if vectorIndex can find the index of the first index of a element
//==================================================================================================
   int vectorIndexTest(void)
   {
      gpstk::TestUtil testFramework("stl_helpers", "VectorIndex", __FILE__, __LINE__);
      std::string testMesg;

      std::vector<int> iOneTime(5);
      for (int i=0; i<5; i++) {iOneTime[i] = i;};
      std::vector<int> iTwoTimes(5);
      iTwoTimes[0] = 5;
      iTwoTimes[1] = 2;
      iTwoTimes[2] = 3;
      iTwoTimes[3] = 2;
      iTwoTimes[4] = 3;
      std::vector<int> iNone(5);
      for (int i=10; i<15; i++) {iNone[i-10] = i;};

      std::vector<float> fOneTime(5);
      for (float i=0; i<5; i++) {fOneTime[i] = i;};
      std::vector<float> fTwoTimes(5);
      fTwoTimes[0] = 5;
      fTwoTimes[1] = 2;
      fTwoTimes[2] = 3;
      fTwoTimes[3] = 2;
      fTwoTimes[4] = 3;
      std::vector<float> fNone(5);
      for (float i=10; i<15; i++) {iNone[i-10] = i;};


      int expectedIndexOneTime = 3;
      int expectedIndexTwoTimes = 2;
      int expectedIndexNone = -1;

      testMesg = "VectorIndex() did not find the first instance of a given integer element";
      testFramework.assert(gpstk::vectorindex<int>(iOneTime, 3) == expectedIndexOneTime, testMesg, __LINE__);
      testFramework.assert(gpstk::vectorindex<int>(iTwoTimes, 3) == expectedIndexTwoTimes, testMesg, __LINE__);
      testFramework.assert(gpstk::vectorindex<int>(iNone, 3) == expectedIndexNone, testMesg, __LINE__);
		
      testMesg = "VectorIndex() did not find the first instance of a given float element";
      testFramework.assert(gpstk::vectorindex<float>(fOneTime, 3) == expectedIndexOneTime, testMesg, __LINE__);
      testFramework.assert(gpstk::vectorindex<float>(fTwoTimes, 3) == expectedIndexTwoTimes, testMesg, __LINE__);
      testFramework.assert(gpstk::vectorindex<float>(fNone, 3) == expectedIndexNone, testMesg, __LINE__);

      return testFramework.countFails();
   }

private:
   double eps;

};


int main() //Main function to initialize and run all tests above
{
	Stl_helpers_T testClass;
	int check, errorCounter = 0;

	check = testClass.maxAndMinTest();
	errorCounter += check;

	check = testClass.statsTest();
	errorCounter += check;

	check = testClass.vectorIndexTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
