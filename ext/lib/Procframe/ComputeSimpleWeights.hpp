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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011 
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
 * @file ComputeSimpleWeights.hpp
 * This class computes simple satellites weights partially based on the
 * variance model by Kennedy, 2002. It is meant to be used with GNSS data
 * structures.
 */

#ifndef GPSTK_COMPUTESIMPLEWEIGHTS_HPP
#define GPSTK_COMPUTESIMPLEWEIGHTS_HPP

#include "DataStructures.hpp"
#include "ProcessingClass.hpp"
#include "TropModel.hpp"
#include "GNSSconstants.hpp"             // DEG_TO_RAD


namespace gpstk
{

      /// @ingroup DataStructures 
      //@{


      /** This class computes simple satellites weights partially based on the
       *  variance model by Kennedy, 2002.
       *
       * It is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Declare a GDS object
       *   gnssRinex gRin;
       *
       *      // Create a 'ComputeSimpleWeights' object
       *   ComputeSimpleWeights sWeights;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> sWeights;
       *   }
       * @endcode
       *
       * The "ComputeSimpleWeights" object will visit every satellite in the
       * GNSS data structure that is "gRin" and will try to compute its weight
       * based on a simple algorithm.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the weights inserted along their corresponding
       * satellites. Be warned that if it is not possible to compute the
       * weight for a given satellite, it will be summarily deleted from
       * the data structure.
       *
       * @sa ComputeIURAWeights.hpp, ComputeMOPSWeights.hpp.
       */
   class ComputeSimpleWeights: public ProcessingClass
   {
   public:

         /// Default constructor.
      ComputeSimpleWeights()
      { };


         /** Returns a satTypeValueMap object, adding the new data
          *  generated when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        satTypeValueMap& gData )
         throw(ProcessingException);


         /** Returns a gnnsSatTypeValue object, adding the new data
          *  generated when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException);


         /** Returns a gnnsRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeSimpleWeights() {};


   private:


         /// Nominal position.
      Position nominalPos;


   }; // End of class 'ComputeSimpleWeights'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_COMPUTESIMPLEWEIGHTS_HPP
