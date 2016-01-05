//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2011 
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file ComputePC.hpp
 * This class eases computing PC combination for GNSS data structures.
 */

#ifndef GPSTK_COMPUTEPC_HPP
#define GPSTK_COMPUTEPC_HPP

#include "ComputeCombination.hpp"


namespace gpstk
{

      /// @ingroup DataStructures 
      //@{


      /** This class eases computing PC combination for GNSS data structures.
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
       *   ComputePC getPC;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> getPC;
       *   }
       * @endcode
       *
       * The "ComputePC" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will try to compute its PC combination.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the PC inserted along their corresponding
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
       *
       * @warning When using C1 instead of P1 to compute this combination, be
       * aware that instrumental errors will NOT cancel, introducing a bias that
       * must be taken into account by other means.
       *
       */
   class ComputePC : public ComputeCombination
   {
   public:

         /// Default constructor
      ComputePC();


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
      virtual ComputePC& useC1(void)
      { type1 = TypeID::C1; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputePC() {};


   protected:


         /// Compute the combination of observables.
      virtual double getCombination( const double& obs1,
                                     const double& obs2 )
      { return ( (GAMMA_GPS*obs1 - obs2)/(DEN) ); };


   private:


      const double DEN;     // DEN = GAMMA_GPS - 1


   }; // End of class 'ComputePC'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_COMPUTEPC_HPP
