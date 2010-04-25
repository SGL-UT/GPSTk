#pragma ident "$Id: $"

/**
* @file EarthBody.cpp
* Class to handle earth planet, it'll be taken as the central
* body of the spacecraft.
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


#include "EarthBody.hpp"
#include "ASConstant.hpp"

namespace gpstk
{
	   // Earth's rotation rate in rad/s.
	const double EarthBody::omegaEarth = 7.292115E-05;  
	
      // Equatorial radius of earth in m from WGS-84
	const double EarthBody::radiusEarth = 6378.1363e3;
	
      // Flattening factor of earth from WGS-84
	const double EarthBody::flatEarth = 0.00335281; 
	
      // Earth gravity constant in m^3/s^2 from JGM3
	const double EarthBody::gmEarth = 398600.4415e+9;    // [m^3/s^2]; JGM3

	
	
	   // Returnts the dynamic Earth rotation rate. 
	double EarthBody::getSpinRate(UTCTime t)
	{
		double mjdUT1 = t.mjdUT1();
	
		double Mjd0 = floor(mjdUT1);;
		double Tu   = (Mjd0  - ASConstant::MJD_J2000)/36525.0;

		return (7292115.8553e-11 + 4.3e-15 * Tu);

	}  // End of method 'EarthBody::getSpinRate()'


}  // End of namespace 'gpstk'
