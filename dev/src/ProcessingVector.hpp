
/**
 * @file ProcessingVector.hpp
 * This is a class to store ProcessingClass objects in a vector.
 */

#ifndef PROCESSING_VECTOR_GPSTK
#define PROCESSING_VECTOR_GPSTK

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


#include <vector>
#include "ProcessingClass.hpp"


namespace gpstk
{

    /** @addtogroup GPSsolutions */
    //@{


    /** This is a class to store ProcessingClass objects in a vector.
     * This class allows to create run-time std::vectors of processing actions to
     * be applied to GNSS data structures (GDS). 
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;        // This is a GDS object
     *   SimpleFilter myFilter  // SimpleFilter is a child from ProcessingClass
     *   ComputePC getPC;       // ComputePC is a child from ProcessingClass
     *
     *   ProcessingVector pVector;  // Declare a ProcessingVector object
     *   pVector.push_back(myFilter);     // Add GDS processing objects to this vector
     *   pVector.push_back(getPC);
     *
     *   while(rin >> gRin) {
     *      gRin >> pVector;      // gRin is processed according to the vector
     *   }
     * @endcode
     *
     */    
    class ProcessingVector : public ProcessingClass
    {
    public:

        /// Default constructor.
        ProcessingVector()
        {
            setIndex();
        };


        /** Processing method. It returns a satTypeValueMap object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(satTypeValueMap& gData)
        {
            std::vector<ProcessingClass*>::iterator pos;
            for (pos = procvector.begin(); pos != procvector.end(); ++pos)
            {
                (*pos)->Process(gData);                
            }

            return gData;
        };


        /** Processing method. It returns a gnnsSatTypeValue object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
        {
            (*this).Process(gData.body);
            return gData;
        };


        /** Processing method. It returns a gnnsRinex object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Process(gnssRinex& gData)
        {
            (*this).Process(gData.body);
            return gData;
        };


        /// Returns a pointer to the first element.
        virtual ProcessingClass* front(void)
        {
            return (procvector.front());
        };


        /// Returns a pointer to the last element.
        virtual ProcessingClass* back(void)
        {
            return (procvector.back());
        };


        /// Returns a pointer to the n'th element.
        ProcessingClass* operator[](int n)
        {
            return (procvector[n]);
        };


        /// Inserts a new element at the end.
        virtual void push_back(ProcessingClass& pClass)
        {
            procvector.push_back( (&pClass) );
            return;
        };


        /// Removes the last element. It does NOT return it.
        virtual void pop_back(void)
        {
            procvector.pop_back();
            return;
        };


        /// Returns TRUE if the ProcessingVector size is zero (0).
        virtual bool empty(void) const
        {
            return (procvector.empty());
        };


        /// Returns the size of the ProcessingVector.
        virtual int size(void) const
        {
            return (procvector.size());
        };


        /// Removes all the elements from the ProcessingVector.
        virtual void clear(void)
        {
            return (procvector.clear());
        };


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
        virtual ~ProcessingVector() {};


    private:

        /// stl::vector holding pointers to ProcessingClass objects.
        std::vector<ProcessingClass*> procvector;

        /// Initial index assigned to this class.
        static int classIndex;

        /// Index belonging to this object.
        int index;

        /// Sets the index and increment classIndex.
        void setIndex(void) { (*this).index = classIndex++; }; 

   }; // end class ProcessingVector
   

   //@}
   
}

#endif
