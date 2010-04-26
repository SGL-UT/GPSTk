#pragma ident "$Id: $"


/**
 * @file AtmosphericDrag.cpp
 * This class computes the acceleration due to drag on a satellite
 * using an Earth atmosphere model that conforms to the computeDensity 
 * abstract method.
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

#include "AtmosphericDrag.hpp"
#include "ReferenceFrames.hpp"

namespace gpstk
{

	const double AtmosphericDrag::rho_0[CIRA_SIZE] = 
	{
		1.057E-03, 3.206E-04, 8.770E-05, 1.905E-05, 3.396E-06, 5.297E-07,
		9.661E-08, 2.438E-08, 8.484E-09, 3.845E-09, 2.070E-09, 5.464E-10,
		2.789E-10, 7.248E-11, 2.418E-11, 9.158E-12, 3.725E-12, 1.585E-12, 
		6.967E-13, 1.454E-13, 3.614E-14, 1.170E-14, 5.245E-15, 3.019E-15
	};

	const double AtmosphericDrag::H[CIRA_SIZE] = 
	{
		8.382,	7.714,	6.549,	5.799,	5.382,	5.877,
		7.263,	9.473,	12.636,	16.149,	22.523,	29.740,
		37.105, 45.546, 53.628, 53.298, 58.515, 60.828,
		63.822, 71.835,	88.667, 124.64, 181.05, 268.0
	};

	const double AtmosphericDrag::h0[CIRA_SIZE] = 
	{
		50,  60,  70,	80,	 90, 100,
		110, 120, 130, 140, 150, 180,
		200, 250, 300, 350, 400, 450,
		500, 600, 700, 800, 900, 1000
	};

	
	   // this is the real one
	void AtmosphericDrag::doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc)
	{
		double omega_e = rb.getSpinRate(utc);

		Vector<double> r = sc.R();	// satellite position in m
		Vector<double> v = sc.V();	// satellite velocity in m/s

		const double cd = sc.getDragCoeff();
		const double area = sc.getDragArea();
		const double mass = sc.getDryMass();

		double rmag = norm(r);
		double beta = cd * area / mass;  // [m^2/kg]

		// compute the atmospheric density
		double rho = computeDensity(utc, rb, r, v);	// [kg/m^3]
		
		// compute the relative velocity vector and magnitude
		Vector<double> we(3,0.0);
		we(2)= omega_e;

		Vector<double> wxr = cross(we,r);
		Vector<double> vr = v - wxr;
		double vrmag = norm(vr);
		
		// form -1/2 (Cd*A/m) rho
		double coeff = -0.5 * beta * rho;
		double coeff2 = coeff * vrmag;

		// compute the acceleration in ECI frame (km/s^2)
		a = vr * coeff2;									       ///////// a

		// Partial reference: Montenbruck,P248

		// form partial of drag wrt v  
		// da_dv = -0.5*Cd*(A/M)*p*(vr*transpose(vr)/vr+vr1)
		Matrix<double> tr(3,1,0.0);
		tr(0,0)=vr(0);
		tr(1,0)=vr(1);
		tr(2,0)=vr(2);

		Matrix<double> vrvrt = tr*transpose(tr); 
		vrvrt = vrvrt / vrmag;
		
		double eye3[3*3] = {1,0,0,0,1,0,0,0,1};
		Matrix<double> vrm(3,3,0.0);
		vrm = eye3;

		vrm = vrm * vrmag;
		da_dv = (vrvrt + vrm) * coeff;					//////// da_dv

		// da_dr
		// da_dr = -0.5*Cd*(A/M)*vr*dp_dr-da_dv*X(w)
		da_dr.resize(3,3,0.0);

		Matrix<double> X(3,3,0.0);
		X(0,1) = -we(2);		// -wz
		X(0,2) = +we(1);		//  wy
		X(1,0) = +we(2);		// +wz
		X(1,2) = -we(0);		// -wx
		X(2,0) = -we(1);		// -wy
		X(2,1) = +we(0);		// +wx
		
		Matrix<double> part1(3,3,0.0);
		Matrix<double> part2(3,3,0.0);
		
	
		// Get the J2000 to TOD transformation
      Matrix<double> N = ReferenceFrames::J2kToTODMatrix(utc.asTT());

		// Transform r from J2000 to TOD
		Vector<double> r_tod = N * r;
		Position geoidPos(r_tod(0),r_tod(1),r_tod(3));
		
      // Satellite height
		double height = geoidPos.getAltitude()/1000.0;              //  convert to [km]
		
		const int n = CIRA_SIZE; ;

		int bracket = 0;

		if (height >= h0[n-1]) 
		{
			bracket = n - 1;
		}
		else 
		{
			for (int i = 0; i < (n-1); i++) 
			{
				if ((height >= h0[i]) && (height < h0[i+1]))
				{
					bracket = i;
				}
			}
		}  // End 'if (height >= h0[n-1]) '
		
		double Hh = H[bracket];
		double coeff4 = -1.0 / (Hh * rmag);

		Vector<double> drhodr = r*coeff4;
		
      Matrix<double> tr2(3,1,0.0);
		tr2(0,0) = drhodr(0);
		tr2(1,0) = drhodr(1);
		tr2(2,0) = drhodr(2);

		part1 = tr*transpose(tr2);		// //Matrix part1 = vr.outerProduct(drhodr);
		part1 = part1*coeff2;

		//part1 = dp_dr*a/rho;
		part2 =-da_dv*X;
		da_dr = part1-part2;

		// form partial of drag wrt cd
		double coeff3 = coeff2 / cd;
		this->dadcd = vr*coeff3;				            ////////   da_dcd

		this->da_dcd(0,0) = dadcd(0);
		this->da_dcd(1,0) = dadcd(1);
		this->da_dcd(2,0) = dadcd(2);

	}  // End of method 'AtmosphericDrag::doCompute()'


}  // End of namespace 'gpstk'



