
/**
 * @file ComputeLC.hpp
 * This class eases computing LC combination for GNSS data structures.
 */

#ifndef COMPUTE_LC_GPSTK
#define COMPUTE_LC_GPSTK

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================



#include "ComputeCombination.hpp"


namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /** This class eases computing LC combination for GNSS data structures.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   ComputeLC getLC;
     *
     *   while(rin >> gRin) {
     *      gRin >> getLC;
     *   }
     * @endcode
     *
     * The "ComputeLC" object will visit every satellite in the GNSS data
     * structure that is "gRin" and will try to compute its LC combination.
     *
     * When used with the ">>" operator, this class returns the same incoming
     * data structure with the LC inserted along their corresponding
     * satellites. Be warned that if a given satellite does not have the 
     * observations required, it will be summarily deleted from the data
     * structure.
     *
     * All observations are in meters.
     *
     */
    class ComputeLC : public ComputeCombination
    {
    public:

        /// Default constructor
        ComputeLC() : DEN(0.646944444)
        {
            type1 = TypeID::L1;
            type2 = TypeID::L2;
            resultType = TypeID::LC;
            setIndex();
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(satTypeValueMap& gData)
        {
            ComputeCombination::Process(gData);

            return gData;
        }


        /// Returns an index identifying this object.
        virtual int getIndex(void) const;


        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


        /** Sets the index to a given arbitrary value. Use with caution.
         *
         * @param newindex      New integer index to be assigned to current object.
         */
        void setIndex(const int newindex) { (*this).index = newindex; };


        /// Destructor
        virtual ~ComputeLC() {};


    protected:

        /// Compute the combination of observables.
        virtual double getCombination(const double& obs1, const double& obs2)
        {
            return ( (GAMMA_GPS*obs1 - obs2)/(DEN) );
        };


    private:

        const double DEN;     // DEN = GAMMA_GPS - 1

        /// Initial index assigned to this class.
        static int classIndex;

        /// Index belonging to this object.
        int index;

        /// Sets the index and increment classIndex.
        void setIndex(void) { (*this).index = classIndex++; }; 

   }; // end class ComputeLC
   

   //@}
   
}

#endif
