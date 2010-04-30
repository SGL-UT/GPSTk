#pragma ident "$Id: $"

/**
 * @file LEOSatOrbit.hpp
 * This class handle equation of motion for low earth orbit satellites.
 */

#ifndef GPSTK_LEO_SAT_ORBIT_HPP
#define GPSTK_LEO_SAT_ORBIT_HPP


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

#include "SatOrbit.hpp"

namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /** This class handle equation of motion for low earth orbit satellites.
       */

   class LEOSatOrbit : public SatOrbit
   {
   public:
         /// Default constructor
      LEOSatOrbit();


         /// Default destructor
      virtual ~LEOSatOrbit(){};


      virtual Vector<double> getDerivatives(const double&         t,
                                            const Vector<double>& y );

   protected:



   }; // End of class 'LEOSatOrbit'

      // @}

}  // End of namespace 'gpstk'


#endif   // GPSTK_LEO_SAT_ORBIT_HPP
