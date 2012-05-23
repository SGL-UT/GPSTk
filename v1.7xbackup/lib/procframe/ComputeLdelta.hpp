#pragma ident "$Id$"

/**
 * @file ComputeLdelta.hpp
 * This class eases computing Ldelta combination for GNSS data structures.
 */

#ifndef COMPUTELDELTA_HPP
#define COMPUTELDELTA_HPP

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



#include "ComputeCombination.hpp"


namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class eases computing Ldelta combination for GNSS data
       * structures.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *   ComputeLdelta getLdelta;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> getLdelta;
       *   }
       * @endcode
       *
       * The "ComputeLdelta" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will try to compute its Ldelta
       * combination.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the Ldelta inserted along their corresponding
       * satellites. Be warned that if a given satellite does not have the
       * observations required, it will be summarily deleted from the data
       * structure.
       *
       * All observations are in meters.
       *
       * @sa ComputeLinear.hpp and LinearCombinations.hpp for a different
       * approach to the same task.
       */
   class ComputeLdelta : public ComputeCombination
   {
   public:

         /// Default constructor
      ComputeLdelta();


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(ProcessingException)
      { ComputeCombination::Process(gData); return gData; };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeLdelta() {};


   protected:


        /// Compute the combination of observables.
        virtual double getCombination( const double& obs1,
                                       const double& obs2 )
        { return ( ( L1_FREQ*obs1 - L2_FREQ*obs2 ) / ( DEN ) ); };


   private:


      const double DEN;       // DEN = L1_FREQ - L2_FREQ


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'ComputeLdelta'

      //@}

}
#endif   // COMPUTELDELTA_HPP
