#pragma ident "$Id$"
// file: xRungeKutta4.cpp

#include "xRungeKutta4.hpp"
#include "pendulum.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xRungeKutta4);


void xRungeKutta4 :: setUp (void)
{ 
}

void xRungeKutta4 :: quarterTest (void)
{
	gpstk::Matrix<double> x0(2,1), truncError(2,1);
	x0(0,0) = 0.001; // Initial angle in radians
	x0(1,0) = 0.0; // Initial angular velocity in radians/second
	
	PendulumIntegrator pModel(x0,0.);
	
	double g = 9.81, L = 1.0;
	pModel.setPhysics(g,L);
	
	double deltaT = .00001;  // Step size in seconds for integrator
	
	double time = 0;
	double Nper = 2.25; // number of periods
	
	double addError = 0; //Total Error for angle
	double addDotError = 0; //Total Error for rate of change in angle
	
	long count = 0;
	
	while (pModel.getTime() < Nper * (2*3.14159265/sqrt(g/L)))
	{
		pModel.integrateTo((count++)*deltaT,truncError);
		addError += fabs(truncError(0,0));
		addDotError += fabs(truncError(1,0));
	}
	
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,pModel.getState()(0,0),addError*4);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-sqrt(2*g*(1-cos(.001))),pModel.getState()(1,0),addDotError*2);
	
}

void xRungeKutta4 :: halfTest (void)
{
	gpstk::Matrix<double> x0(2,1), truncError(2,1);
	x0(0,0) = 0.001; // Initial angle in radians
	x0(1,0) = 0.0; // Initial angular velocity in radians/second
	
	PendulumIntegrator pModel(x0,0.);
	
	double g = 9.81, L = 1.0;
	pModel.setPhysics(g,L);
	
	double deltaT = .00001;  // Step size in seconds for integrator
	
	double time = 0;
	double Nper = 2.5; // number of periods
	
	double addError = 0; //Total Error for angle
	double addDotError = 0; //Total Error for rate of change in angle
	
	long count = 0;
	
	while (pModel.getTime() < Nper * (2*3.14159265/sqrt(g/L)))
	{
		pModel.integrateTo((count++)*deltaT,truncError);
		addError += fabs(truncError(0,0));
		addDotError += fabs(truncError(1,0));
	}
	
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-x0(0,0),pModel.getState()(0,0),addError*2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(x0(1,0),pModel.getState()(1,0),addDotError*2);
	
}

void xRungeKutta4 :: fullTest (void)
{
	gpstk::Matrix<double> x0(2,1), truncError(2,1);
	x0(0,0) = 0.001; // Initial angle in radians
	x0(1,0) = 0.0; // Initial angular velocity in radians/second
	
	PendulumIntegrator pModel(x0,0.);
	
	double g = 9.81, L = 1.0;
	pModel.setPhysics(g,L);
	
	double deltaT = .00001;  // Step size in seconds for integrator
	
	double time = 0;
	double Nper = 2; // number of periods
	
	double addError = 0; //Total Error for angle
	double addDotError = 0; //Total Error for rate of change in angle
	
	long count = 0;
	
	while (pModel.getTime() < Nper * (2*3.14159265/sqrt(g/L)))
	{
		pModel.integrateTo((count++)*deltaT,truncError);
		addError += fabs(truncError(0,0));
		addDotError += fabs(truncError(1,0));
	}
	
	CPPUNIT_ASSERT_DOUBLES_EQUAL(x0(0,0),pModel.getState()(0,0),addError*2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(x0(1,0),pModel.getState()(1,0),addDotError*2);
	
}
