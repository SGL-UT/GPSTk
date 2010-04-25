
#pragma ident "$Id: $"

/**
* @file Spacecraft.cpp
* The Spacecraft class encapsulates physical parameters.
*/

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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================

#include "Spacecraft.hpp"
#include "Exception.hpp"

namespace gpstk
{

      // Default constructor
	Spacecraft::Spacecraft(std::string name)
	{
		scName = name;

		reflectCoeff= 1.0;		//
		dragCoeff	= 2.0;		//
		crossArea	= 5;		   // m^2
		dryMass		= 1000;		// kg

		resetState();

	}


	void Spacecraft::resetState()
	{
		// resize
		r.resize(3,0.0);
		v.resize(3,0.0);
		p.resize(0,0.0);

		dr_dr0.resize(9,0.0);		// I
		dr_dv0.resize(9,0.0);		// 0
		dr_dp0.resize(0,0.0);		// 0

		dv_dr0.resize(9,0.0);		// 0
		dv_dv0.resize(9,0.0);		// I
		dv_dp0.resize(0,0.0);		// 0

		// set eye elements to 1
		dr_dr0(0) = 1.0;
		dr_dr0(4) = 1.0;
		dr_dr0(8) = 1.0;

		dv_dv0(0) = 1.0;
		dv_dv0(4) = 1.0;
		dv_dv0(8) = 1.0;
	}

	void Spacecraft::initStateVector(Vector<double> rv, Vector<double> dp)
	{
		// first the size of input vector should be checked
		if(rv.size()!=6)
		{
			Exception e("Error in Spacecraft::initStateVector(): the size of rv should be 6.");
			GPSTK_THROW(e);
		}

		resetState();
		
		// set position
		r(0) = rv(0);
		r(1) = rv(1);
		r(2) = rv(2);

		// set velocity
		v(0) = rv(3);
		v(1) = rv(4);
		v(2) = rv(5);
		
		// set force model parameters
		p = dp;

		// set dr_dp0 and dv_dp0
		const int np = p.size();

		dr_dp0.resize(3*np,0.0);
		dv_dp0.resize(3*np,0.0);

	}  // End of method 'Spacecraft::initStateVector()'

	   // Get Transition Matrix
	Matrix<double> Spacecraft::getTransitionMatrix()
	{
      /* Transition Matrix
          |                          |
          | dr_dr0	dr_dv0	dr_dp0 |
          |                          |
      phi=| dv_dr0	dv_dv0	dv_dp0 |
          |                          |
          | 0			0		     I    |
          |                          |
      */
		
		const int np=p.size();

		Matrix<double> phiMatrix(np+6,np+6,0.0);
		
		/// dr/dr0
		phiMatrix(0,0)=dr_dr0(0);
		phiMatrix(0,1)=dr_dr0(1);
		phiMatrix(0,2)=dr_dr0(2);
		phiMatrix(1,0)=dr_dr0(3);
		phiMatrix(1,1)=dr_dr0(4);
		phiMatrix(1,2)=dr_dr0(5);
		phiMatrix(2,0)=dr_dr0(6);
		phiMatrix(2,1)=dr_dr0(7);
		phiMatrix(2,2)=dr_dr0(8);
		/// dr/dv0
		phiMatrix(0,3)=dr_dv0(0);
		phiMatrix(0,4)=dr_dv0(1);
		phiMatrix(0,5)=dr_dv0(2);
		phiMatrix(1,3)=dr_dv0(3);
		phiMatrix(1,4)=dr_dv0(4);
		phiMatrix(1,5)=dr_dv0(5);
		phiMatrix(2,3)=dr_dv0(6);
		phiMatrix(2,4)=dr_dv0(7);
		phiMatrix(2,5)=dr_dv0(8);
		/// dv/dr0
		phiMatrix(3,0)=dv_dr0(0);
		phiMatrix(3,1)=dv_dr0(1);
		phiMatrix(3,2)=dv_dr0(2);
		phiMatrix(4,0)=dv_dr0(3);
		phiMatrix(4,1)=dv_dr0(4);
		phiMatrix(4,2)=dv_dr0(5);
		phiMatrix(5,0)=dv_dr0(6);
		phiMatrix(5,1)=dv_dr0(7);
		phiMatrix(5,2)=dv_dr0(8);
		/// dv/dv0
		phiMatrix(3,3)=dv_dv0(0);
		phiMatrix(3,4)=dv_dv0(1);
		phiMatrix(3,5)=dv_dv0(2);
		phiMatrix(4,3)=dv_dv0(3);
		phiMatrix(4,4)=dv_dv0(4);
		phiMatrix(4,5)=dv_dv0(5);
		phiMatrix(5,3)=dv_dv0(6);
		phiMatrix(5,4)=dv_dv0(7);
		phiMatrix(5,5)=dv_dv0(8);

		/// dr/dp0
		for(int i=0;i<np;i++)
		{
			phiMatrix(0,i+6) = dr_dp0(i+0*np);
			phiMatrix(1,i+6) = dr_dp0(i+1*np);
			phiMatrix(2,i+6) = dr_dp0(i+2*np);

			phiMatrix(3,i+6) = dv_dp0(i+0*np);
			phiMatrix(4,i+6) = dv_dp0(i+1*np);
			phiMatrix(5,i+6) = dv_dp0(i+2*np);

			phiMatrix(i+6,i+6) = 1.0;
			phiMatrix(i+6,i+6) = 1.0;
			phiMatrix(i+6,i+6) = 1.0;
		}

		return phiMatrix;

	}  // End of method 'Spacecraft::getTransitionMatrix()'


	void Spacecraft::setTransitionMatrix(Matrix<double> phiMatrix)
	{
      /* Transition Matrix
          |                          |
          | dr_dr0	dr_dv0	dr_dp0 |
          |                          |
      phi=| dv_dr0	dv_dv0	dv_dp0 |
          |                          |
          | 0			0		    I     |
          |                          |
      */

		const int np = phiMatrix.rows()-6;

		// resize the vectors
		p.resize(np,0.0);
		dr_dp0.resize(3*np,0.0);
		dv_dp0.resize(3*np,0.0);

		// dr/dr0
		dr_dr0(0) = phiMatrix(0,0);
		dr_dr0(1) = phiMatrix(0,1);
		dr_dr0(2) = phiMatrix(0,2);
		dr_dr0(3) = phiMatrix(1,0);
		dr_dr0(4) = phiMatrix(1,1);
		dr_dr0(5) = phiMatrix(1,2);
		dr_dr0(6) = phiMatrix(2,0);
		dr_dr0(7) = phiMatrix(2,1);
		dr_dr0(8) = phiMatrix(2,2);
		// dr/dv0
		dr_dv0(0) = phiMatrix(0,3);
		dr_dv0(1) = phiMatrix(0,4);
		dr_dv0(2) = phiMatrix(0,5);
		dr_dv0(3) = phiMatrix(1,3);
		dr_dv0(4) = phiMatrix(1,4);
		dr_dv0(5) = phiMatrix(1,5);
		dr_dv0(6) = phiMatrix(2,3);
		dr_dv0(7) = phiMatrix(2,4);
		dr_dv0(8) = phiMatrix(2,5);
		// dv/dr0
		dv_dr0(0) = phiMatrix(3,0);
		dv_dr0(1) = phiMatrix(3,1);
		dv_dr0(2) = phiMatrix(3,2);
		dv_dr0(3) = phiMatrix(4,0);
		dv_dr0(4) = phiMatrix(4,1);
		dv_dr0(5) = phiMatrix(4,2);
		dv_dr0(6) = phiMatrix(5,0);
		dv_dr0(7) = phiMatrix(5,1);
		dv_dr0(8) = phiMatrix(5,2);
		// dv/dv0
		dv_dv0(0) = phiMatrix(3,3);
		dv_dv0(1) = phiMatrix(3,4);
		dv_dv0(2) = phiMatrix(3,5);
		dv_dv0(3) = phiMatrix(4,3);
		dv_dv0(4) = phiMatrix(4,4);
		dv_dv0(5) = phiMatrix(4,5);
		dv_dv0(6) = phiMatrix(5,3);
		dv_dv0(7) = phiMatrix(5,4);
		dv_dv0(8) = phiMatrix(5,5);

		// dr/dp0
		for(int i=0;i<np;i++)
		{
			dr_dp0(i+0*np) = phiMatrix(0,i+6);
			dr_dp0(i+1*np) = phiMatrix(1,i+6);
			dr_dp0(i+2*np) = phiMatrix(2,i+6);

			dv_dp0(i+0*np) = phiMatrix(3,i+6);
			dv_dp0(i+1*np) = phiMatrix(4,i+6);
			dv_dp0(i+2*np) = phiMatrix(5,i+6);
		}

	}  // End of method 'Spacecraft::setTransitionMatrix()'


	// get State Transition Matrix 6*6
	Matrix<double> Spacecraft::getStateTransitionMatrix()
	{
      /* Transition Matrix
          |                  |
          | dr_dr0	dr_dv0  |
      phi=|                  |
          | dv_dr0	dv_dv0  |
          |                  |
      */
		const int np=p.size();

		Matrix<double> phiMatrix(6,6,0.0);

		/// dr/dr0
		phiMatrix(0,0)=dr_dr0(0);
		phiMatrix(0,1)=dr_dr0(1);
		phiMatrix(0,2)=dr_dr0(2);
		phiMatrix(1,0)=dr_dr0(3);
		phiMatrix(1,1)=dr_dr0(4);
		phiMatrix(1,2)=dr_dr0(5);
		phiMatrix(2,0)=dr_dr0(6);
		phiMatrix(2,1)=dr_dr0(7);
		phiMatrix(2,2)=dr_dr0(8);
		/// dr/dv0
		phiMatrix(0,3)=dr_dv0(0);
		phiMatrix(0,4)=dr_dv0(1);
		phiMatrix(0,5)=dr_dv0(2);
		phiMatrix(1,3)=dr_dv0(3);
		phiMatrix(1,4)=dr_dv0(4);
		phiMatrix(1,5)=dr_dv0(5);
		phiMatrix(2,3)=dr_dv0(6);
		phiMatrix(2,4)=dr_dv0(7);
		phiMatrix(2,5)=dr_dv0(8);
		/// dv/dr0
		phiMatrix(3,0)=dv_dr0(0);
		phiMatrix(3,1)=dv_dr0(1);
		phiMatrix(3,2)=dv_dr0(2);
		phiMatrix(4,0)=dv_dr0(3);
		phiMatrix(4,1)=dv_dr0(4);
		phiMatrix(4,2)=dv_dr0(5);
		phiMatrix(5,0)=dv_dr0(6);
		phiMatrix(5,1)=dv_dr0(7);
		phiMatrix(5,2)=dv_dr0(8);
		/// dv/dv0
		phiMatrix(3,3)=dv_dv0(0);
		phiMatrix(3,4)=dv_dv0(1);
		phiMatrix(3,5)=dv_dv0(2);
		phiMatrix(4,3)=dv_dv0(3);
		phiMatrix(4,4)=dv_dv0(4);
		phiMatrix(4,5)=dv_dv0(5);
		phiMatrix(5,3)=dv_dv0(6);
		phiMatrix(5,4)=dv_dv0(7);
		phiMatrix(5,5)=dv_dv0(8);

		return phiMatrix;

	}  // End of method 'Spacecraft::getStateTransitionMatrix()'

	// get Sensitivity Matrix 6*np
	Matrix<double> Spacecraft::getSensitivityMatrix()
	{
      /* Transition Matrix
          |        |
          | dr_dp0 |
      s = |        |
          | dv_dp0 |
          |        |
      */
		const int np=p.size();

		Matrix<double> sMatrix(6,np,0.0);
		
		for(int i=0;i<np;i++)
		{
			sMatrix(0,i) = dr_dp0(i+0*np);
			sMatrix(1,i) = dr_dp0(i+1*np);
			sMatrix(2,i) = dr_dp0(i+2*np);

			sMatrix(3,i) = dv_dp0(i+0*np);
			sMatrix(4,i) = dv_dp0(i+1*np);
			sMatrix(5,i) = dv_dp0(i+2*np);
		}

		return sMatrix;

	}  // End of method 'Spacecraft::getSensitivityMatrix()'


	Vector<double> Spacecraft::getStateVector()
	{
		const int np=p.size();
		Vector<double> y(6*np+42);

		y(0) = r(0);
		y(1) = r(1);
		y(2) = r(2);
		y(3) = v(0);
		y(4) = v(1); 
		y(5) = v(2);
		
		for(int i=0;i<9;i++)
		{
			y(6+i) = dr_dr0(i);
			y(15+i) = dr_dv0(i);
			
			y(24+3*np+i) = dv_dr0(i);
			y(33+3*np+i) = dv_dv0(i);
		}
		for(int i=0;i<3*np;i++)
		{
			y(24+i) = dr_dp0(i);
			y(42+3*np+i) = dv_dp0(i);
		}

		return y;

	}  // End of method 'Spacecraft::getStateVector()'


	void Spacecraft::setStateVector(Vector<double> y)
	{
		const int dim = y.size();
		const int np = (dim-42)/6;
		
		// resize the vectors
		p.resize(np,0.0);
		dr_dp0.resize(3*np,0.0);
		dv_dp0.resize(3*np,0.0);
		
		r(0) = y(0);
		r(1) = y(1);
		r(2) = y(2);
		v(0) = y(3);
		v(1) = y(4);
		v(2) = y(5);

		for(int i=0;i<9;i++)
		{
			dr_dr0(i) = y(6+i);
			dr_dv0(i) = y(15+i);

			dv_dr0(i) = y(24+3*np+i);
			dv_dv0(i) = y(33+3*np+i);
		}
		
		for(int i=0;i<3*np;i++)
		{
			dr_dp0(i) = y(24+i);
			dv_dp0(i) = y(42+3*np+i);
		}

	}  // End of method 'Spacecraft::setStateVector(Vector<double> y)'

	void Spacecraft::test()
	{
		cout<<"testing Spacecraft"<<endl;
		
		/*
		r.resize(3,8.0);
		v.resize(3,8.0);
		p.resize(2,0.0);

		dr_dr0.resize(9,0.0);		// I
		dr_dv0.resize(9,0.0);		// 0
		dr_dp0.resize(6,3.0);		// 0

		dv_dr0.resize(9,0.0);		// 0
		dv_dv0.resize(9,0.0);		// I
		dv_dp0.resize(6,4.0);		// 0

		// set eye elements to 1
		dr_dr0(0) = 1.0;
		dr_dr0(4) = 1.0;
		dr_dr0(8) = 1.0;

		dv_dv0(0) = 1.0;
		dv_dv0(4) = 1.0;
		dv_dv0(8) = 1.0;
		
	
		writeToFile("default.sc");

		// it work well
		*/

	}	// End of method 'Spacecraft::test()'


      // Stream output for DayTime objects.  Typically used for debugging.
      // @param s stream to append formatted DayTime to.
      // @param t Spacecraft to append to stream \c s.
      // @return reference to \c s.
   ostream& operator<<( ostream& s, 
                        const gpstk::Spacecraft& sc )
   {
      // s << endl;

      return s;

   }  // End of 'ostream& operator<<()'

}  // End of namespace 'gpstk'

