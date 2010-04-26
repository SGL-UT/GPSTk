#pragma ident "$Id: $"

/**
 * @file HarrisPriesterDrag.hpp
 * The HarrisPriester class computes the Harris-Priester atmosphere model.
 */

#ifndef	GPSTK_HARRIS_PRIESTER_DRAG_HPP
#define	GPSTK_HARRIS_PRIESTER_DRAG_HPP


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
#include "Matrix.hpp"
#include <map>

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

	   /**
	    * The HarrisPriester class computes the Harris-Priester atmosphere model.
	    * This code is from Montenbruck. Good for 100 - 2000 km altitude only.
	    * 
	    * This Model is checked on Sep 28th,2009, OK!!!
	    */
	class HarrisPriesterDrag : public AtmosphericDrag
	{
	public:
         /// Default constructor
		HarrisPriesterDrag();

         /// Default destructor
      virtual ~HarrisPriesterDrag() {};

		void test();

         /// Update woring F107(Mean Solar Flux)
		void updateF107(double f107 = 157);


		   /** Abstract class requires the subclass to compute the atmospheric density.
          * @param utc epoch in UTC
		    * @param rb  EarthRef object.
		    * @param r   Position vector.
          * @param v   Velocity vector
		    * @return Atmospheric density in kg/m^3
		    */
		virtual double computeDensity(UTCTime utc, EarthBody& rb, Vector<double> r, Vector<double> v);

   protected:

         /// do init some work
      void init();

      Matrix<double> getDensityCoeficentsByF107(double f107 = 157);

	protected:
         /// Upper height limit 2000.0 [km]
		static const double upper_limit;        

         /// Lower height limit 100.0 [km]
		static const double lower_limit;
         
         /// Right ascension lag 0.523599 [rad]
		static const double ra_lag;       
	 
		   /// F10.7 cm Mean Solar Flux 157 by Default
		double woringF107;

         /// HP coefficient for the workingF107
      Matrix<double> workingDens;


         /// Object holding HP coefficients
      std::map<int, Matrix<double> > hpCoeficient;
         
         /// Harris-Priester atmospheric density coefficients
         /// it's modified from JAT by Richard C.
      static const double hpcoef[10][177];

	}; // End of class 'HarrisPriesterDrag'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_HARRIS_PRIESTER_DRAG_HPP





