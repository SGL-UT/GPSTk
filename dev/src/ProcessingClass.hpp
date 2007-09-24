
/**
 * @file ProcessingClass.hpp
 * This is an abstract base class for objects processing GNSS Data Structures.
 */

#ifndef PROCESSING_CLASS_GPSTK
#define PROCESSING_CLASS_GPSTK

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



#include "DataStructures.hpp"


namespace gpstk
{

    /** @addtogroup GPSsolutions */
    //@{


    /** This is an abstract base class for objects processing GNSS Data Structures (GDS).
     * Children of this class are meant to be used together with GNSS data structures 
     * objects found in "DataStructures" class, processing and transforming them.
     *
     * A typical way to use a derived class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;        // This is a GDS object
     *   ComputeLC getLC;       // ComputeLC is a child from ProcessingClass
     *
     *   while(rin >> gRin) {
     *      gRin >> getLC;      // getLC objects 'process' data inside gRin
     *   }
     * @endcode
     *
     * All children from ProcessingClass must implement the following methods:
     *
     * - Process(): These methods will be in charge of doing the real processing on the data.
     * - getIndex(): This method should return an unique index identifying the object.
     * - getClassName(): This method should return a string identifying the class the object belongs to.
     *
     */
    class ProcessingClass
    {
    public:

        /** Abstract method. It returns a satTypeValueMap object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(satTypeValueMap& gData) = 0;


        /** Abstract method. It returns a gnnsSatTypeValue object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData) = 0;


        /** Abstract method. It returns a gnnsRinex object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Process(gnssRinex& gData) = 0;


        /// Abstract method. It returns an unique index identifying the object.
        virtual int getIndex(void) const = 0;

        /// Abstract method. It returns a string identifying the class the object belongs to.
        virtual std::string getClassName(void) const = 0;


        /// operator == for ProcessingClass
        bool operator==(const ProcessingClass& right) const
        { return ( (*this).getIndex() == right.getIndex() ); }

        /// operator != for ProcessingClass
        bool operator!=(const ProcessingClass& right) const
        { return !(operator==(right)); }

        /// operator < for ProcessingClass
        virtual bool operator<(const ProcessingClass& right) const
        { return ( (*this).getIndex() < right.getIndex() ); }

        /// operator > for ProcessingClass
        bool operator>(const ProcessingClass& right) const
        { return ( (*this).getIndex() > right.getIndex() ); }

        /// operator <= for ProcessingClass
        bool operator<=(const ProcessingClass& right) const
        { return ( (*this).getIndex() <= right.getIndex() ); }

        /// operator >= for SatID
        bool operator>=(const ProcessingClass& right) const
        { return ( (*this).getIndex() >= right.getIndex() ); }


        /// Destructor
        virtual ~ProcessingClass() {};



   }; // end class ProcessingClass
   

    /// Input operator from gnssSatTypeValue to ProcessingClass.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, ProcessingClass& procClass)
    {
            procClass.Process(gData);
            return gData;
    }


    /// Input operator from gnssRinex to ProcessingClass.
    inline gnssRinex& operator>>(gnssRinex& gData, ProcessingClass& procClass)
    {
            procClass.Process(gData);
            return gData;
    }


    /** This function is a Binary Predicate (in STL parlance), and it is used to allow comparison 
     * and sorting by object index (instead of memory address) when using pointers. 
     */
    inline bool CompareIndex(const ProcessingClass* a, const ProcessingClass* b)
    {
        return ( (a->getIndex()) < (b->getIndex()) );
    }



   //@}
   
}


#endif
