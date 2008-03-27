#pragma ident "$Id$"

/**
 * @file SimpleFilter.hpp
 * This class filters out satellites with observations grossly out of bounds.
 */

#ifndef SIMPLEFILTER_HPP
#define SIMPLEFILTER_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================



#include "ProcessingClass.hpp"


namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class filters out satellites with observations grossly out of
       *  bounds.
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
       * structure that is "gRin" and will check that the given code
       * observations are within some (preassigned) boundaries.
       *
       * By default, the algorithm will check C1 observables, the minimum limit
       * is 15000000.0 meters and the maximum limit is 30000000.0 meters. You
       * may change all these settings with the appropriate set methods.
       *
       * Also, you may set more than one observable to be checked by passing a
       * "TypeIDSet" object to the appropriate constructors or methods. For
       * instance:
       *
       * @code
       *   TypeIDSet typeSet;
       *   typeSet.insert(TypeID::P1);
       *   typeSet.insert(TypeID::P2);
       *
       *   myFilter.setFilteredType(typeSet);
       * @endcode
       *
       * Be warned that if a given satellite does not have the observations
       * required, or if their are out of bounds, the full satellite record
       * will be summarily deleted from the data structure.
       *
       */
   class SimpleFilter : public ProcessingClass
   {
   public:

         /// Default constructor. By default, filter C1.
      SimpleFilter() : minLimit(15000000.0), maxLimit(30000000.0)
      { setFilteredType(TypeID::C1); setIndex(); };


         /** Explicit constructor
          *
          * @param type      TypeID to be filtered.
          * @param min       Minimum limit (in meters).
          * @param max       Maximum limit (in meters).
          */
      SimpleFilter( const TypeID& type,
                    const double& min,
                    const double& max )
         : minLimit(min), maxLimit(max)
      { setFilteredType(type); setIndex(); };


         /** Explicit constructor
          *
          * @param type      TypeID to be filtered.
          */
      SimpleFilter(const TypeID& type)
         : minLimit(15000000.0), maxLimit(30000000.0)
      { setFilteredType(type); setIndex(); };


         /** Explicit constructor
          *
          * @param typeSet   Set of TypeID's to be filtered.
          * @param min       Minimum limit (in meters).
          * @param max       Maximum limit (in meters).
          */
      SimpleFilter( const TypeIDSet& typeSet,
                    const double& min,
                    const double& max )
         : filterTypeSet(typeSet), minLimit(min), maxLimit(max)
      { setIndex(); };


         /** Explicit constructor
          *
          * @param typeSet   Set of TypeID's to be filtered.
          */
      SimpleFilter(const TypeIDSet& typeSet)
         : filterTypeSet(typeSet), minLimit(15000000.0), maxLimit(30000000.0)
      { setIndex(); };


         /** Returns a satTypeValueMap object, filtering the target
          *  observables.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData);


         /** Method to set the minimum limit.
          * @param min       Minimum limit (in meters).
          */
      virtual SimpleFilter& setMinLimit(const double& min)
      { minLimit = min; return (*this); };


         /// Method to get the minimum limit.
      virtual double getMinLimit() const
      { return minLimit; };


         /** Method to set the maximum limit.
          * @param max       Maximum limit (in meters).
          */
      virtual SimpleFilter& setMaxLimit(const double& max)
      { maxLimit = max; return (*this); };


         /// Method to get the maximum limit.
      virtual double getMaxLimit() const
      { return maxLimit; };


         /** Method to add a TypeID to be filtered.
          * @param type      Extra TypeID to be filtered.
          */
      virtual SimpleFilter& addFilteredType(const TypeID& type)
      { filterTypeSet.insert(type); return (*this); };


         /** Method to set a TypeID to be filtered. This method will erase
          *  previous types.
          * @param type      TypeID to be filtered.
          */
      virtual SimpleFilter& setFilteredType(const TypeID& type)
      { filterTypeSet.clear(); filterTypeSet.insert(type); return (*this); };


         /** Method to set the TypeID's to be filtered. This method will erase
          *  previous types.
          * @param typeSet       Set of TypeID's to be filtered.
          */
      virtual SimpleFilter& setFilteredType(const TypeIDSet& typeSet)
      { filterTypeSet.clear(); filterTypeSet = typeSet; return (*this); };


         /// Method to get the set of TypeID's to be filtered.
      virtual TypeIDSet getFilteredType() const
      { return filterTypeSet; };


         /** Returns a gnnsSatTypeValue object, filtering the target
          *  observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData);


         /** Returns a gnnsRinex object, filtering the target observables.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData);


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /** Sets the index to a given arbitrary value. Use with caution.
          *
          * @param newindex      New integer index to be assigned to current
          *                      object.
          */
      void setIndex(const int newindex)
      { index = newindex; }; 


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
      { return ( (value>=minLimit) && (value<=maxLimit) ); };


         /// Set of types to be filtered
      TypeIDSet filterTypeSet;

         /// Minimum value allowed for input data (in meters).
      double minLimit;

         /// Maximum value allowed for input data (in meters).
      double maxLimit;


   private:

         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; }; 

   }; // end class SimpleFilter

      //@}

}

#endif  // SIMPLEFILTER_HPP
