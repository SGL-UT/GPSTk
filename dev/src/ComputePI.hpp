
/**
 * @file ComputePI.hpp
 * This class eases computing PI combination for GNSS data structures.
 */

#ifndef Compute_PI_GPSTK
#define Compute_PI_GPSTK

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
//  Dagoberto Salazar - gAGE. 2007
//
//============================================================================



#include "ComputeCombination.hpp"


namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /** This class eases computing PI combination for GNSS data structures.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   ComputePI getPI;
     *
     *   while(rin >> gRin) {
     *      gRin >> getPI;
     *   }
     * @endcode
     *
     * The "ComputePI" object will visit every satellite in the GNSS data
     * structure that is "gRin" and will try to compute its PI combination.
     *
     * When used with the ">>" operator, this class returns the same incoming
     * data structure with the PI inserted along their corresponding
     * satellites. Be warned that if a given satellite does not have the 
     * observations required, it will be summarily deleted from the data
     * structure.
     *
     * Sometimes, the Rinex observations file does not have P1, but provides C1
     * instead. In such cases, you must use the useC1() method.
     *
     */
    class ComputePI : public ComputeCombination
    {
    public:

        /// Default constructor
        ComputePI()
        {
            type1 = TypeID::P1;
            type2 = TypeID::P2;
            resultType = TypeID::PI;
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Combine(satTypeValueMap& gData)
        {
            ComputeCombination::Combine(gData);

            return gData;
        }


        /// Some Rinex data files provide C1 instead of P1. Use this method in those cases.
        void useC1() { type1 = TypeID::C1; };


        /// Destructor
        virtual ~ComputePI() {};


    protected:
        /// Compute the combination of observables.
        virtual double getCombination(const double& obs1, const double& obs2)
        {
            return ( obs2 - obs1 );
        };

   }; // end class ComputePI
   

   //@}
   
}

#endif
