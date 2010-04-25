#pragma ident "$Id: $"

/**
* @file EarthPoleTide.hpp
* 
*/

#ifndef GPSTK_POLE_TIDE_HPP
#define GPSTK_POLE_TIDE_HPP


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


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

	   /**
	    * Solid Earth Pole Tide
	    * reference: IERS Conventions 2003
	    */
	class EarthPoleTide
	{
	public:
         /// Default constructor
		EarthPoleTide(){}

         /// Default destructor
		~EarthPoleTide(){}

		
		   /** Solid pole tide to normalized earth potential coefficients
		    *
		    * @param mjdUtc   UTC in MJD
		    * @param dC21     correction to normalized coefficients dC21
		    * @param dS21     correction to normalized coefficients dS21
		    */
		void getPoleTide(double mjdUtc, double& dC21, double& dS21 );


   }; // End of class 'EarthPoleTide'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_POLE_TIDE_HPP


