
/**
 * @file ProcessingList.hpp
 * This is a class to store ProcessingClass objects in a list.
 */

#ifndef PROCESSING_LIST_GPSTK
#define PROCESSING_LIST_GPSTK

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


    /** This is a class to store ProcessingClass objects in a list.
     * This class allows to create run-time lists of processing actions to
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
     *   ProcessingList pList;  // Declare a ProcessingList object
     *   pList.push_back(myFilter);     // Add GDS processing objects to this list
     *   pList.push_back(getPC);
     *
     *   while(rin >> gRin) {
     *      gRin >> pList;      // gRin is processed according to the list
     *   }
     * @endcode
     *
     * \warning: A sort() method is provided. However, it will sort processing objects according to
     * a rather arbitrary indexing that will suit many needs, but not all.
     *
     */    
    class ProcessingList : public ProcessingClass
    {
    public:

        /// Default constructor.
        ProcessingList()
        {
            setIndex();
        };


        /** Processing method. It returns a gnnsSatTypeValue object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
        {
            std::vector<ProcessingClass*>::iterator pos;
            for (pos = proclist.begin(); pos != proclist.end(); ++pos)
            {
                (*pos)->Process(gData);
            }

            return gData;
        };


        /** Processing method. It returns a gnnsRinex object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Process(gnssRinex& gData)
        {
            std::vector<ProcessingClass*>::iterator pos;
            for (pos = proclist.begin(); pos != proclist.end(); ++pos)
            {
                (*pos)->Process(gData);
            }

            return gData;
        };


        /// Returns a pointer to the first element.
        virtual ProcessingClass* front(void)
        {
            return (proclist.front());
        };


        /// Returns a pointer to the last element.
        virtual ProcessingClass* back(void)
        {
            return (proclist.back());
        };


        /// Inserts a new element at the beginning.
        virtual void push_front(ProcessingClass& pClass)
        {
           // Add space to the end 
           proclist.resize(proclist.size()+1);

           // Roll the vector forward
           size_t idx;
           for (idx=(proclist.size()-1);idx>0;idx++)
           {
              proclist[idx]=proclist[idx-1]; 
           }

           // Put the new member at the start of the vector
           proclist[0]=&pClass;

           return;
        };


        /// Inserts a new element at the end.
        virtual void push_back(ProcessingClass& pClass)
        {
            proclist.push_back( (&pClass) );
            return;
        };


        /// Removes the first element. It does NOT return it.
        virtual void pop_front(void)
        {
           // Roll the vector backward
           size_t idx;
           for (idx=0; idx<(proclist.size()-1);idx++)
           {
              proclist[idx]=proclist[idx+1]; 
           }

           // Remove space at the end 
           proclist.resize(proclist.size()-1);

           return;
        };


        /// Removes the last element. It does NOT return it.
        virtual void pop_back(void)
        {
            proclist.pop_back();
            return;
        };


        /// Returns TRUE if the ProcessingList size is zero (0).
        virtual bool empty(void) const
        {
            return (proclist.empty());
        };


        /// Returns the size of the ProcessingList.
        virtual int size(void) const
        {
            return (proclist.size());
        };


        /// Removes all the elements from the ProcessingList.
        virtual void clear(void)
        {
            return (proclist.clear());
        };


        /** Sorts the elements from the ProcessingList.
         *
         * You should use this method with caution, because the sorting is done according to a
         * (rather arbitrary) index assigned to each ProcessingClass object.
         *
         * The idea of this index is to establish a kind of precedence for processing actions. For
         * instance, you should first compute the PC (ionosphere-free) combination before trying to
         * smooth PC code observations. Therefore, ComputePC objects will have smaller indexes than
         * PCSmoother objects.
         *
         * Also, if several objects of the same class are declared, those declared first will have 
         * smaller indexes and will be used earlier.
         *
         * Be warned that there may be lots of cases where the indexes assigned by default will not
         * match the intended GNSS data processing chain, and as so, you must not blindly confide in
         * the sort() method. You MUST ALWAYS know what you are doing.
         */
        virtual void sort(void)
        {
              //proclist.sort(gpstk::CompareIndex());
              std::sort(proclist.begin(), proclist.end(), CompareIndex());
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
        virtual ~ProcessingList() {};


    private:

        /// stl::vector holding pointers to ProcessingClass objects.
        std::vector<ProcessingClass*> proclist;

        /// Initial index assigned to this class.
        static int classIndex;

        /// Index belonging to this object.
        int index;

        /// Sets the index and increment classIndex.
        void setIndex(void) { (*this).index = classIndex++; }; 




   }; // end class ProcessingList
   

   //@}
   
}

#endif
