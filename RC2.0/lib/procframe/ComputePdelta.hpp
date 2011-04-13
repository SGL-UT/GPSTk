#pragma ident "$Id$"

/**
 * @file ComputePdelta.hpp
 * This class eases computing Pdelta combination for GNSS data structures.
 */

#ifndef COMPUTEPDELTA_HPP
#define COMPUTEPDELTA_HPP

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


      /** This class eases computing Pdelta combination for GNSS data
       *  structures.
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
       *   ComputePdelta getPdelta;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> getPdelta;
       *   }
       * @endcode
       *
       * The "ComputePdelta" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will try to compute its Pdelta
       * combination.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the Pdelta inserted along their corresponding
       * satellites. Be warned that if a given satellite does not have the
       * observations required, it will be summarily deleted from the data
       * structure.
       *
       * Sometimes, the Rinex observations file does not have P1, but provides
       * C1 instead. In such cases, you must use the useC1() method.
       *
       * All observations are in meters.
       *
       * @sa ComputeLinear.hpp and LinearCombinations.hpp for a different
       * approach to the same task.
       */
   class ComputePdelta : public ComputeCombination
   {
   public:


         /// Default constructor
      ComputePdelta();


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(ProcessingException)
      { ComputeCombination::Process(gData); return gData; };


         /// Some Rinex data files provide C1 instead of P1. Use this method
         /// in those cases.
      virtual ComputePdelta& useC1(void)
      { type1 = TypeID::C1; return (*this); };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputePdelta() {};


   protected:


         /// Compute the combination of observables.
      virtual double getCombination( const double& obs1,
                                     const double& obs2 )
      { return ( ( L1_FREQ*obs1 + L2_FREQ*obs2 ) / ( DEN ) ); };


   private:


      const double DEN;       // DEN = L1_FREQ + L2_FREQ


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'ComputePdelta'

      //@}

}
#endif   // COMPUTEPDELTA_HPP
