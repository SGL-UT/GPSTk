#pragma ident "$Id$"
// file:

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//============================================================================

#include "xPolyFit.hpp"

//Note to self Find out what covariance is and then add that to the test cases

CPPUNIT_TEST_SUITE_REGISTRATION (xPolyFit);


void xPolyFit :: setUp (void)
{ 
}

/*
**** This test is designed to test the validity of the PolyFit constructors
**** Both of the two constructors are tested to make sure they don't throw
**** a random exceptions.

**** Also this test makes sure that the constructer set up everything as we
**** expected it to (e.g. N to 0)

*/

void xPolyFit :: constrTest (void)
{
	try
	{
	CPPUNIT_ASSERT_NO_THROW(gpstk::PolyFit<double> PolyCheck);
	CPPUNIT_ASSERT_NO_THROW(gpstk::PolyFit<double> PolyNumCheck(4));
	
	gpstk::PolyFit<double> Poly;
	CPPUNIT_ASSERT_EQUAL((unsigned) 0, Poly.N());
	CPPUNIT_ASSERT_EQUAL((unsigned) 0, Poly.Degree());
	CPPUNIT_ASSERT_EQUAL(true, Poly.isSingular());
	gpstk::PolyFit<double> Poly4((unsigned) 4);
	CPPUNIT_ASSERT_EQUAL((unsigned) 0, Poly4.N());
	CPPUNIT_ASSERT_EQUAL((unsigned) 4, Poly4.Degree());
	CPPUNIT_ASSERT_EQUAL(true, Poly4.isSingular());
	}
	catch (gpstk::Exception& e)
	{
	cout << e;
	}
}

/*
**** This test is designed to test the validity of the three add members of the PolyFit class
**** Addition to the PolyFit object is tested with individual datum, gpstk::Vectors of data and
**** std::vectors of data.

**** These are tested against a least squares polynomial fit that was done by hand

**** Please note isSingular, Solution, Degreem N and Solve were tested inderectly
**** Please note, I don't know enough about Covariance to test it for the example by hand

*/
void xPolyFit :: addTest (void)
{
	gpstk::PolyFit<double> AddSingle(2);
	gpstk::PolyFit<double> AddGVect(2);
	gpstk::PolyFit<double> AddSVect(2);
	
	double data[4] = {0.,2.,4.,-1.};
	double time[4] = {3.,3.,4.,2.,};
	
	gpstk::Vector<double> gData(4,0.);
	gData[0] = 0.;
	gData[1] = 2.;
	gData[2] = 4.;
	gData[3] = -1.;
	gpstk::Vector<double> gTime(4,0.);
	gTime[0] = 3.;
	gTime[1] = 3.;
	gTime[2] = 4.;
	gTime[3] = 2.;
	
	std::vector<double> vData(4,0.);
	vData[0] = 0.;
	vData[1] = 2.;
	vData[2] = 4.;
	vData[3] = -1.;
	std::vector<double> vTime(4,0.);
	vTime[0] = 3.;
	vTime[1] = 3.;
	vTime[2] = 4.;
	vTime[3] = 2.;
	
	//Done by hand
	gpstk::Vector<double> ExpSolution(2,0.);
	ExpSolution[0] = 152./59;
	ExpSolution[1] = 20./59;
	
	for (int i =0;i<4;i++)
	{
		AddSingle.Add(time[i],data[i]);
	}
	gpstk::Vector<double> SingleSolution = AddSingle.Solution();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ExpSolution[0],SingleSolution[0],1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ExpSolution[1],SingleSolution[1],1e-6);
	CPPUNIT_ASSERT_EQUAL((unsigned) 4, AddSingle.N());
	CPPUNIT_ASSERT_EQUAL((unsigned) 2, AddSingle.Degree());
	CPPUNIT_ASSERT_EQUAL(false, AddSingle.isSingular());
	
	//Add on an unweighted sample, N should increase but everything else should be the same
	AddSingle.Add(7.,20.,0);
	
	gpstk::Vector<double> SingleSolution2 = AddSingle.Solution();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ExpSolution[0],SingleSolution2[0],1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ExpSolution[1],SingleSolution2[1],1e-6);
	CPPUNIT_ASSERT_EQUAL((unsigned) 5, AddSingle.N());
	CPPUNIT_ASSERT_EQUAL((unsigned) 2, AddSingle.Degree());
	CPPUNIT_ASSERT_EQUAL(false, AddSingle.isSingular());
	
	
	AddGVect.Add(gTime,gData);
	gpstk::Vector<double> gVectSolution = AddGVect.Solution();
	
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ExpSolution[0],gVectSolution[0],1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ExpSolution[1],gVectSolution[1],1e-6);
	CPPUNIT_ASSERT_EQUAL((unsigned) 4, AddGVect.N());
	CPPUNIT_ASSERT_EQUAL((unsigned) 2, AddGVect.Degree());
	CPPUNIT_ASSERT_EQUAL(false, AddGVect.isSingular());
	
	AddSVect.Add(vTime,vData);
	gpstk::Vector<double> sVectSolution = AddSVect.Solution();
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ExpSolution[0],sVectSolution[0],1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ExpSolution[1],sVectSolution[1],1e-6);
	CPPUNIT_ASSERT_EQUAL((unsigned) 4, AddSVect.N());
	CPPUNIT_ASSERT_EQUAL((unsigned) 2, AddSVect.Degree());
	CPPUNIT_ASSERT_EQUAL(false, AddSVect.isSingular());
	
}

/*
**** This test is designed to test the validity of the reset member of the PolyFit class
**** Reset is tested by first adding data to a blank PolyFit object and then clearing
**** that data (Please note this dad was already tested in the previous test)

**** Please note isSingular, Solution, Degreem N and Solve were tested inderectly here

*/
void xPolyFit :: resetTest (void)
{
	//Polynomial will be reset without user inputed parameter
	gpstk::PolyFit<double> resetPolyD(2);
	
	//Poly will be reset with a parameter
	gpstk::PolyFit<double> resetPolyP(2);
	

	double data[4] = {0.,2.,4.,-1.};
	double time[4] = {3.,3.,4.,2.,};
	
	for (int i =0;i<4;i++)
	{
		resetPolyD.Add(time[i],data[i]);
		resetPolyP.Add(time[i],data[i]);
	}
	
	resetPolyD.Reset();
	
	gpstk::Matrix<double> Blank(2,2,0.);
	gpstk::Vector<double> Zero(2,0.);
	
	gpstk::Vector<double> resetPolyDSolution = resetPolyD.Solution();
	gpstk::Matrix<double> resetPolyDCov = resetPolyD.Covariance();
	
	CPPUNIT_ASSERT_EQUAL((unsigned) 0, resetPolyD.N());
	CPPUNIT_ASSERT_EQUAL((unsigned) 2, resetPolyD.Degree());
	CPPUNIT_ASSERT_EQUAL(true, resetPolyD.isSingular());
	CPPUNIT_ASSERT_EQUAL(Blank[0][0],resetPolyDCov[0][0]);
	CPPUNIT_ASSERT_EQUAL(Blank[0][1],resetPolyDCov[0][1]);
	CPPUNIT_ASSERT_EQUAL(Blank[1][0],resetPolyDCov[1][0]);
	CPPUNIT_ASSERT_EQUAL(Blank[1][1],resetPolyDCov[1][1]);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Zero[0],resetPolyDSolution[0],1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Zero[1],resetPolyDSolution[1],1e-6);
	
	resetPolyP.Reset((unsigned) 3);
	
	gpstk::Matrix<double> BlankP(3,3,0.);
	gpstk::Vector<double> ZeroP(3,0.);
	
	gpstk::Vector<double> resetPolyPSolution = resetPolyP.Solution();
	gpstk::Matrix<double> resetPolyPCov = resetPolyP.Covariance();
	
	CPPUNIT_ASSERT_EQUAL((unsigned) 0, resetPolyP.N());
	CPPUNIT_ASSERT_EQUAL((unsigned) 3, resetPolyP.Degree());
	CPPUNIT_ASSERT_EQUAL(true, resetPolyP.isSingular());
	CPPUNIT_ASSERT_EQUAL(BlankP[0][0],resetPolyPCov[0][0]);
	CPPUNIT_ASSERT_EQUAL(BlankP[0][1],resetPolyPCov[0][1]);
	CPPUNIT_ASSERT_EQUAL(BlankP[0][2],resetPolyPCov[0][2]);
	CPPUNIT_ASSERT_EQUAL(BlankP[1][0],resetPolyPCov[1][0]);
	CPPUNIT_ASSERT_EQUAL(BlankP[1][1],resetPolyPCov[1][1]);
	CPPUNIT_ASSERT_EQUAL(BlankP[1][2],resetPolyPCov[1][2]);
	CPPUNIT_ASSERT_EQUAL(BlankP[2][0],resetPolyPCov[2][0]);
	CPPUNIT_ASSERT_EQUAL(BlankP[2][1],resetPolyPCov[2][1]);
	CPPUNIT_ASSERT_EQUAL(BlankP[2][2],resetPolyPCov[2][2]);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ZeroP[0],resetPolyPSolution[0],1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ZeroP[1],resetPolyPSolution[1],1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(ZeroP[2],resetPolyPSolution[2],1e-6);
	
}

/*
**** This test is designed to test the validity of the Evaluate member of the PolyFit class
**** This particular test only involves using Evaluate for a single datum and a blank, singular
**** and a normal PolyFit.

**** The Evaluate is tested by using the solution to a + b*x = d

*/
void xPolyFit :: evalTestSingle (void)
{
	gpstk::PolyFit<double> Blank;
	gpstk::PolyFit<double> Single(2);
	gpstk::PolyFit<double> Eval(2);
	
	double data[4] = {0.,2.,4.,-1.};
	double time[4] = {3.,3.,4.,2.,};
	double dataS[4] = {1.,1.,1.,1.};
	double timeS[4] = {0.,0.,0.,1.,};
	
	for (int i =0;i<4;i++)
	{
		Eval.Add(time[i],data[i]);
		Single.Add(timeS[i],dataS[i]);
	}
	
	gpstk::Vector<double> EvalSolution = Eval.Solution();
	
	CPPUNIT_ASSERT_EQUAL(0.,Blank.Evaluate(3.));
	CPPUNIT_ASSERT_EQUAL(0.,Single.Evaluate(10.));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(EvalSolution[0]+3.*EvalSolution[1], Eval.Evaluate(3.),1e-6);
}

/*
**** This test is designed to test the validity of the Evaluate member of the PolyFit class
**** This particular test only involves using Evaluate for a gpstk::Vectors of data with
**** a blank, singular, and a normal PolyFit.

**** The Evaluate is tested by using the solution to a + b*x = d

*/
void xPolyFit :: evalTestVector (void)
{
	gpstk::PolyFit<double> Blank;
	gpstk::PolyFit<double> Single(2);
	gpstk::PolyFit<double> Eval(2);
	
	gpstk::Vector<double> SData(4,0.);
	SData[0] = 1.;
	SData[1] = 1.;
	SData[2] = 1.;
	SData[3] = 1.;
	gpstk::Vector<double> STime(4,0.);
	STime[0] = 0.;
	STime[1] = 0.;
	STime[2] = 0.;
	STime[3] = 1.;
	
	gpstk::Vector<double> EData(4,0.);
	EData[0] = 0.;
	EData[1] = 2.;
	EData[2] = 4.;
	EData[3] = -1.;
	gpstk::Vector<double> ETime(4,0.);
	ETime[0] = 3.;
	ETime[1] = 3.;
	ETime[2] = 4.;
	ETime[3] = 2.;
	
	Single.Add(STime,SData);
	Eval.Add(ETime,EData);
	
	gpstk::Vector<double> EvalSolution = Eval.Solution();
	
	CPPUNIT_ASSERT_EQUAL(0.,Blank.Evaluate(3.));
	CPPUNIT_ASSERT_EQUAL(0.,Single.Evaluate(10.));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(EvalSolution[0]+3.*EvalSolution[1], Eval.Evaluate(3.),1e-6);
}
