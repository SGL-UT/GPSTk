
/**
 * @file ComputeMelbourneWubbena.hpp
 * This class eases computing Melbourne-Wubbena combination for GNSS data structures.
 */

#ifndef COMPUTE_MELBOURNEWUBBENA_GPSTK
#define COMPUTE_MELBOURNEWUBBENA_GPSTK

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


    /** This class eases computing Melbourne-Wubbena combination for GNSS data structures.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   ComputeMelbourneWubbena getMW;
     *
     *   while(rin >> gRin) {
     *      gRin >> getMW;
     *   }
     * @endcode
     *
     * The "ComputeMelbourne-Wubbena" object will visit every satellite in the 
     * GNSS data structure that is "gRin" and will try to compute its 
     * Melbourne-Wubbena combination.
     *
     * When used with the ">>" operator, this class returns the same incoming
     * data structure with the Melbourne-Wubbena combinations inserted along their
     * corresponding satellites. Be warned that if a given satellite does not 
     * have the observations required, it will be summarily deleted from the data
     * structure.
     *
     * Sometimes, the Rinex observations file does not have P1, but provides C1
     * instead. In such cases, you must use the useC1() method.
     *
     */
    class ComputeMelbourneWubbena : public ComputeCombination
    {
    public:

        /// Default constructor
        ComputeMelbourneWubbena() : type3(TypeID::L1), type4(TypeID::L2), DEN1(L1_FREQ + L2_FREQ), DEN2(L1_FREQ - L2_FREQ)
        {
            type1 = TypeID::P1;
            type2 = TypeID::P2;
            resultType = TypeID::MWubbena;
            setIndex();
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(satTypeValueMap& gData)
        {
            double value1(0.0);
            double value2(0.0);
            double value3(0.0);
            double value4(0.0);

            SatIDSet satRejectedSet;

            // Loop through all the satellites
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it) 
            {
                try
                {
                    // Try to extract the values
                    value1 = (*it).second(type1);
                    value2 = (*it).second(type2);
                    value3 = (*it).second(type3);
                    value4 = (*it).second(type4);
                }
                catch(...)
                {
                    // If some value is missing, then schedule this satellite for removal
                    satRejectedSet.insert( (*it).first );
                    continue;
                }
                // If everything is OK, then get the new value inside the structure
                (*it).second[resultType] = getCombination(value1, value2, value3, value4);
            }
            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
        };


        /// Some Rinex data files provide C1 instead of P1. Use this method in those cases.
        void useC1() { type1 = TypeID::C1; };


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
        virtual ~ComputeMelbourneWubbena() {};


    protected:
        /// Compute the combination of observables.
        virtual double getCombination(const double& p1, const double& p2, const double& l1, const double& l2)
        {
            return ( ( L1_FREQ*l1 - L2_FREQ*l2 ) / ( DEN2 ) - ( L1_FREQ*p1 + L2_FREQ*p2 ) / ( DEN1 ) );
        };

        /// Dummy function.
        virtual double getCombination(const double& obs1, const double& obs2) { return 0.0; };

    private:

        /// Type of observation to be combined. Nro 3.
        TypeID type3;

        /// Type of observation to be combined. Nro 4.
        TypeID type4;

        const double DEN1;      // DEN1 = L1_FREQ + L2_FREQ
        const double DEN2;      // DEN2 = L1_FREQ - L2_FREQ

        /// Initial index assigned to this class.
        static int classIndex;

        /// Index belonging to this object.
        int index;

        /// Sets the index and increment classIndex.
        void setIndex(void) { (*this).index = classIndex++; }; 

   }; // end class ComputeMelbourneWubbena
   

   //@}
   
}

#endif
