
/**
 * @file ComputeCombination.hpp
 * This is the base class to ease computing combination of data for GNSS data structures.
 */

#ifndef COMPUTE_COMBINATION_GPSTK
#define COMPUTE_COMBINATION_GPSTK

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



#include "ProcessingClass.hpp"


namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /// This class eases computing combination of data for GNSS data structures.
    class ComputeCombination : public ProcessingClass
    {
    public:

        /// Default constructor
        ComputeCombination() {};


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(satTypeValueMap& gData)
        {
            double value1(0.0);
            double value2(0.0);

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
                }
                catch(...)
                {
                    // If some value is missing, then schedule this satellite for removal
                    satRejectedSet.insert( (*it).first );
                    continue;
                }
                // If everything is OK, then get the new value inside the structure
                (*it).second[resultType] = getCombination(value1, value2);
            }
            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
        };


        /** Returns a gnnsSatTypeValue object, adding the new data generated when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
        {
            (*this).Process(gData.body);
            return gData;
        };


        /** Returns a gnnsRinex object, adding the new data generated when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Process(gnssRinex& gData)
        {
            (*this).Process(gData.body);
            return gData;
        };


        /// Returns an index identifying this object.
        virtual int getIndex(void) const { return 1599999; };


        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const { return "ComputeCombination"; };


        /// Destructor
        virtual ~ComputeCombination() {};


    protected:
        /// Compute the combination of observables. You must define this method according to your specific combination.
        virtual double getCombination(const double& obs1, const double& obs2) = 0;

        /// Type of observation to be combined. Nro 1.
        TypeID type1;

        /// Type of observation to be combined. Nro 2.
        TypeID type2;

        /// Type assigned to the resulting combination.
        TypeID resultType;


   }; // end class ComputeCombination
   

   //@}
   
}

#endif
