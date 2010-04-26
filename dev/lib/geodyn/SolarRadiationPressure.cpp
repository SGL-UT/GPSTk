#pragma ident "$Id: $"

/**
 * @file SolarRadiationPressure.cpp
 * This class provides a generic model for the influence
 * of solar radiation pressure on a satellite.
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

#include "SolarRadiationPressure.hpp"
#include "ASConstant.hpp"
#include "ReferenceFrames.hpp"

namespace gpstk
{

	   /* Determines if the satellite is in sunlight or shadow.
	    * Taken from Montenbruck and Gill p. 80-83
	    * @param r ECI position vector of spacecraft [m].
	    * @param r_Sun Sun position vector (geocentric) [m].
	    * @param r_Moon Moon position vector (geocentric) [m].
	    * @return 0.0 if in shadow, 1.0 if in sunlight, 0 to 1.0 if in partial shadow
	    */
	double SolarRadiationPressure::getShadowFunction(Vector<double> r, 
                                                    Vector<double> r_Sun,
                                                    Vector<double> r_Moon,
		                                              SolarRadiationPressure::ShadowModel sm)
	{
		// shadow function
		double v = 0.0;
		
		// Mean Radious of Sun, Moon and Earth
		const double R_sun = ASConstant::R_Sun;
		const double R_moon = ASConstant::R_Moon;
		const double R_earth = ASConstant::R_Earth;

		Vector<double> e_Sun = r_Sun/norm(r_Sun);   // Sun direction unit vector

		double r_dot_sun = dot(r,e_Sun);
		
		if(r_dot_sun>0)
		{
			// Sunny side of central body is always fully lit and return
			v= 1.0;
			return v;
		}
		
		if(sm == SM_CYLINDRICAL)		
		{
			// Taken fram Jisheng Li P111, and checked with GMAT and Bernese5 SHADOW.f
			v = ((r_dot_sun>0 || norm(r-r_dot_sun*e_Sun)>R_earth) ? 1.0 : 0.0);
			return v;
		}
		else if(sm == SM_CONICAL)
		{
			/*
			// Taken from Montenbruck and Gill p. 80-83
			double s0, s2;
			
			// Montenbruck and Gill, eq. 3.79
			s0 = -dot(r,e_Sun); //-state[0]*unitsun[0] - state[1]*unitsun[1] - state[2]*unitsun[2];
			s2 = dot(r,r);		//state[0]*state[0] + state[1]*state[1] + state[2]*state[2];

			// Montenbruck and Gill, eq. 3.80
			double lsc = sqrt(s2 - s0*s0);

			// Montenbruck and Gill, eq. 3.81
			double sinf1 = (R_sun + R_earth) / norm(r_Sun);
			double sinf2 = (R_earth - R_earth) / norm(r_Sun);
			
			// Appropriate l1 and l2 temporarily
			double t1 = sinf1 * sinf1;
			double t2 = sinf2 * sinf2;
			double tanf1 = sqrt(t1 / (1.0 - t1));
			double tanf2 = sqrt(t2 / (1.0 - t2));
			
			// Montenbruck and Gill, eq. 3.82
			double c1 = s0 + R_earth / sinf1;
			double c2 = R_earth / sinf2 - s0;       // Different sign from M&G

			// Montenbruck and Gill, eq. 3.83
			double l1 = c1 * tanf1;
			double l2 = c2 * tanf2;

			if (lsc > l1)	// Outside of the penumbral cone
			{
				v = 1.0;
				return v;
			}
			else 
			{
				//lit = false;
				if (lsc < fabs(l2)) 
				{
					// Inside umbral cone
					if (c2 >= 0.0) 
					{ 
						// no annular ring
						percentSun = 0.0;
						dark = true;
					}
					else 
					{
						// annular eclipse
						pcbrad = asin(R_earth / sqrt(s2));
						percentSun = (psunrad*psunrad - pcbrad*pcbrad) / 
							(psunrad*psunrad);
						dark = false;
					}

					return;
				}
				// In penumbra
				pcbrad = asin(R_earth / sqrt(s2));
				percentSun = ShadowFunction(state);
				lit = false;
				dark = false;
			}*/

			//////////////////////////////////////////////////////////////////////////

			double r_sun_mag = norm(r_Sun);
			double r_mag = norm(r);
			
			Vector<double> d = r_Sun-r;				// vector from sc to sun
			double dmag = norm(d);					

         double a = std::asin(R_sun/dmag);					// eq. 3.85
         double b = std::asin(R_earth/r_mag);					// eq. 3.86
         double c = std::acos(-1.0*dot(r,d)/(r_mag*dmag));	// eq. 3.87

			if((a+b)<=c)			// in Sun light
			{
				v = 1.0;
			}
			else if(c < (b-a))		// in Umbra
			{
				v =  0.0;
			}
			else					// in Penumbra 
			{
				double x = (c*c+a*a-b*b)/(2*c);							// eq. 3.93
            double y = std::sqrt(a*a-x*x);
            double A = a*a*std::acos(x/a)+b*b*std::acos((c-x)/b)-c*y;			// eq. 3.92
				v = 1.0 - A/(ASConstant::PI*a*a);						// eq. 3.94
			}

			return v;
		}
		else
		{
			// unexpected value
			Exception e("Unexpect ShadowModel in getShadowFunction()");
			GPSTK_THROW(e);
		}

		return v;

	}  // End of method 'SolarRadiationPressure::getShadowFunction()'



	   /* Compute the acceleration due to a solar radiation pressure.
	    * @param r ECI position vector [m].
	    * @param r_Sun ECI position vector of Sun in m.
	    * @return Acceleration due to solar radiation pressure in m/s^2.
	    */
	Vector<double> SolarRadiationPressure::accelSRP(Vector<double> r, Vector<double> r_Sun) 
	{
		// Relative position vector of spacecraft w.r.t. Sun (from the sun to s/c)
		Vector<double> d = r-r_Sun;
		double dmag = norm(d);
		double dcubed = dmag * dmag * dmag;
		double au2 = ASConstant::AU * ASConstant::AU;
		
		double P_STK = 4.5344321837439e-06; //4.560E-6
		
      //double factor = CR * (area/mass) * P_STK * au2 / dcubed;
		
      double Ls = 3.823e26; //* STK [W]
		double factor = reflectCoeff * (crossArea/dryMass) * Ls 
                    / (4.0*ASConstant::PI*ASConstant::SPEED_OF_LIGHT*dcubed); // STK HPOP method
		
      Vector<double> out = d * factor;
		
		return  out;

	}  // End of method 'SolarRadiationPressure::accelSRP()'


	   /* Determines if the satellite is in sunlight or shadow based on simple cylindrical shadow model.
	    * Taken from Montenbruck and Gill p. 80-83
	    * @param r ECI position vector of spacecraft [m].
	    * @param r_Sun Sun position vector (geocentric) [m].
	    * @return 0.0 if in shadow, 1.0 if in sunlight, 0 to 1.0 if in partial shadow
	    */
	double SolarRadiationPressure::partial_illumination(Vector<double> r, Vector<double> r_Sun )
	{
		double r_sun_mag = norm(r_Sun);
		double r_mag = norm(r);

		double R_sun = ASConstant::R_Sun;
		double R_earth = ASConstant::R_Earth;
		Vector<double> d = r_Sun-r;
		double dmag = norm(d);
		double sd = -1.0 * dot(r,d);
      double a = std::asin(R_sun/dmag);
      double b = std::asin(R_earth/r_mag);
      double c = std::acos(sd/(r_mag*dmag));
		
      if((a+b)<=c) 
      {
         return 1.0;
      }
		else if(c < (b-a))
      {
         return 0.0;
      }
		else 
		{
			double x = (c*c+a*a-b*b)/(2*c);
         double y = std::sqrt(a*a-x*x);
         double A = a*a*std::acos(x/a)+b*b*std::acos((c-x)/b)-c*y;
			double nu = 1 - A/(ASConstant::PI*a*a);
			return nu;
		}

	}  // End of method 'SolarRadiationPressure::partial_illumination()'


	void SolarRadiationPressure::doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc)
	{
		crossArea = sc.getDragArea();
		dryMass = sc.getDryMass();
		reflectCoeff = sc.getReflectCoeff();

		Vector<double> r_sun = ReferenceFrames::getJ2kPosition(utc.asTDB(),SolarSystem::Sun);
		Vector<double> r_moon = ReferenceFrames::getJ2kPosition(utc.asTDB(),SolarSystem::Moon);
		
		// from km to m
		r_sun = r_sun*1000.0;
		r_moon = r_moon*1000.0;

		// a
		a = accelSRP(sc.R(),r_sun)*getShadowFunction(sc.R(),r_sun,r_moon,SM_CONICAL);

		// da_dr   reference to Montenbruck P248
		// and it's in the same way as the gravitational attraction of the sun
		da_dr.resize(3,3,0.0);

		double au2 = ASConstant::AU * ASConstant::AU;
		double factor = -1.0*reflectCoeff * (crossArea/dryMass) * ASConstant::P_Sol*au2;

		Vector<double> d = sc.R() - r_sun;
		double dmag = norm(d);
		double dcubed = dmag * dmag *dmag;

		Vector<double> temp1 = d / dcubed;			//  detRJ/detRJ^3

		double smag = norm(r_sun);
		double scubed = smag * smag * smag;

		double muod3 = factor / dcubed;
		double jk = 3.0 * muod3/dmag/dmag; 

		double xx = d(0);
		double yy = d(1);
		double zz = d(2);

		da_dr(0,0) = jk * xx * xx - muod3;
		da_dr(0,1) = jk * xx * yy;
		da_dr(0,2) = jk * xx * zz;

		da_dr(1,0) = da_dr(0,1);
		da_dr(1,1) = jk * yy * yy - muod3;
		da_dr(1,2) = jk * yy * zz;

		da_dr(2,0) = da_dr(0,2);
		da_dr(2,1) = da_dr(1,2);
		da_dr(2,2) = jk * zz * zz - muod3;

		// da_dv
		da_dv.resize(3,3,0.0);

		// da_dp
		dadcr.resize(3,0.0);
		dadcr = a /reflectCoeff;

		da_dcr(0,0) = dadcr(0);
		da_dcr(1,0) = dadcr(1);
		da_dcr(2,0) = dadcr(2);

	}  // End of method 'SolarRadiationPressure::doCompute()'

}  // End of namespace 'gpstk'

