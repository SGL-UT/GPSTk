
/**
 * @file SimpleFilter.hpp
 * This class filters out satellites with observations grossly out of bounds.
 */

#ifndef Simple_Filter_GPSTK
#define Simple_Filter_GPSTK

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



#include "DataStructures.hpp"


namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /** This class filters out satellites with observations grossly out of bounds.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   SimpleFilter myFilter;
     *
     *   while(rin >> gRin) {
     *      gRin >> myFilter;
     *   }
     * @endcode
     *
     * The "SimpleFilter" object will visit every satellite in the GNSS data
     * structure that is "gRin" and will check that the given code observations
     * are within some (preassigned) boundaries.
     *
     * By default, the algorithm will check C1 observables, the minimum limit
     * is 15000000.0 meters and the maximum limit is 30000000.0 meters. You can 
     * change all these settings with the appropriate set methods.
     *
     * Also, you may set more than one observable to be checked by passing a
     * "TypeIDSet" object to the appropriate constructors or methods. For instance:
     *
     * @code
     *   TypeIDSet typeSet;
     *   typeSet.insert(TypeID::P1);
     *   typeSet.insert(TypeID::P2);
     *
     *   myFilter.setFilteredType(typeSet);
     * @endcode
     *
     * Be warned that if a given satellite does not have the observations required, 
     * or if their are out of bounds, the full satellite record will be summarily 
     * deleted from the data structure.
     *
     */
    class SimpleFilter
    {
    public:

        /// Default constructor.
        SimpleFilter() : minLimit(15000000.0), maxLimit(30000000.0)
        {
            setFilteredType(TypeID::C1);   // By default, filter C1
        };


        /** Explicit constructor
         *
         * @param type      TypeID to be filtered.
         * @param min       Minimum limit (in meters).
         * @param max       Maximum limit (in meters).
         */
        SimpleFilter(const TypeID& type, const double& min, const double& max) : minLimit(min), maxLimit(max)
        {
            setFilteredType(type);
        };


        /** Explicit constructor
         *
         * @param type      TypeID to be filtered.
         */
        SimpleFilter(const TypeID& type) : minLimit(15000000.0), maxLimit(30000000.0)
        {
            setFilteredType(type);
        };


        /** Explicit constructor
         *
         * @param typeSet   Set of TypeID's to be filtered.
         * @param min       Minimum limit (in meters).
         * @param max       Maximum limit (in meters).
         */
        SimpleFilter(const TypeIDSet& typeSet, const double& min, const double& max) : filterTypeSet(typeSet), minLimit(min), maxLimit(max) {};


        /** Explicit constructor
         *
         * @param typeSet   Set of TypeID's to be filtered.
         */
        SimpleFilter(const TypeIDSet& typeSet) : filterTypeSet(typeSet), minLimit(15000000.0), maxLimit(30000000.0) {};


        /** Returns a satTypeValueMap object, filtering the target observables.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Filter(satTypeValueMap& gData)
        {

            SatIDSet satRejectedSet;

            // Check all the indicated TypeID's
            TypeIDSet::const_iterator pos;
            for (pos = filterTypeSet.begin(); pos != filterTypeSet.end(); ++pos)
            {

                double value(0.0);

                // Loop through all the satellites
                satTypeValueMap::iterator it;
                for (it = gData.begin(); it != gData.end(); ++it) 
                {
                    try
                    {
                        // Try to extract the values
                        value = (*it).second(*pos);

                        // Now, check that the value is within bounds
                        if ( !( checkValue(value) ) )
                        {
                            // If value is out of bounds, then schedule this satellite for removal
                            satRejectedSet.insert( (*it).first );
                        }
                    }
                    catch(...)
                    {
                        // If some value is missing, then schedule this satellite for removal
                        satRejectedSet.insert( (*it).first );
                    }
                }
                // Before checking next TypeID, let's remove satellites with data out of bounds
                gData.removeSatID(satRejectedSet);
            }

            return gData;
        };


        /** Method to set the minimum limit.
         * @param min       Minimum limit (in meters).
         */
        virtual void setMinLimit(const double& min)
        {
           minLimit = min;
        };


        /// Method to get the minimum limit.
        virtual double getMinLimit() const
        {
           return minLimit;
        };


        /** Method to set the maximum limit.
         * @param max       Maximum limit (in meters).
         */
        virtual void setMaxLimit(const double& max)
        {
           maxLimit = max;
        };


        /// Method to get the maximum limit.
        virtual double getMaxLimit() const
        {
           return maxLimit;
        };


        /** Method to add a TypeID to be filtered.
         * @param type      Extra TypeID to be filtered.
         */
        virtual void addFilteredType(const TypeID& type)
        {
            filterTypeSet.insert(type);
        };


        /** Method to set a TypeID to be filtered. This method will erase previous types.
         * @param type      TypeID to be filtered.
         */
        virtual void setFilteredType(const TypeID& type)
        {
            filterTypeSet.clear();      // This makes the set empty
            filterTypeSet.insert(type);
        };


        /** Method to set the TypeID's to be filtered. This method will erase previous types.
         * @param typeSet       Set of TypeID's to be filtered.
         */
        virtual void setFilteredType(const TypeIDSet& typeSet)
        {
            filterTypeSet.clear();      // This makes the set empty
            filterTypeSet = typeSet;
        };


        /// Method to get the set of TypeID's to be filtered.
        virtual TypeIDSet getFilteredType() const
        {
           return filterTypeSet;
        };


        /** Returns a gnnsSatTypeValue object, filtering the target observables.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Filter(gnssSatTypeValue& gData)
        {
            (*this).Filter(gData.body);
            return gData;
        };


        /** Returns a gnnsRinex object, filtering the target observables.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Filter(gnssRinex& gData)
        {
            (*this).Filter(gData.body);
            return gData;
        };


        /// Destructor
        virtual ~SimpleFilter() {};


    protected:
        /** Checks that the value is within the given limits.
         * @param value     The value to be test
         *
         * @return
         *  True if check was OK.
         */
        virtual bool checkValue(const double& value) const
        {
            return ( (value>=minLimit) && (value<=maxLimit) );
        };  // end SimpleFilter::checkValue()

        /// Set of types to be filtered
        TypeIDSet filterTypeSet;

        /// Minimum value allowed for input data (in meters).
        double minLimit;

        /// Maximum value allowed for input data (in meters).
        double maxLimit;


   }; // end class SimpleFilter


    /// Input operator from gnssSatTypeValue to SimpleFilter.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, SimpleFilter& sFilter)
    {
            sFilter.Filter(gData);
            return gData;
    }


    /// Input operator from gnssRinex to SimpleFilter.
    inline gnssRinex& operator>>(gnssRinex& gData, SimpleFilter& sFilter)
    {
            sFilter.Filter(gData);
            return gData;
    }

   

   //@}
   
}

#endif
