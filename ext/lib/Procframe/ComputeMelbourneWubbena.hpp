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
 * @file ComputeMelbourneWubbena.hpp
 * This class eases computing Melbourne-Wubbena combination for GNSS
 * data structures.
 */

#ifndef GPSTK_COMPUTEMELBOURNEWUBBENA_HPP
#define GPSTK_COMPUTEMELBOURNEWUBBENA_HPP

#include "ComputeCombination.hpp"


namespace gpstk
{

      /// @ingroup DataStructures 
      //@{


      /** This class eases computing Melbourne-Wubbena combination for GNSS
       *  data structures.
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
       *   ComputeMelbourneWubbena getMW;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> getMW;
       *   }
       * @endcode
       *
       * The "ComputeMelbourne-Wubbena" object will visit every satellite in
       * the GNSS data structure that is "gRin" and will try to compute its
       * Melbourne-Wubbena combination.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the Melbourne-Wubbena combinations inserted along
       * their corresponding satellites. Be warned that if a given satellite
       * does not have the observations required, it will be summarily deleted
       * from the data structure.
       *
       * Sometimes, the Rinex observations file does not have P1, but provides
       * C1 instead. In such cases, you must use the useC1() method.
       *
       * All observations are in meters.
       *
       * @sa ComputeLinear.hpp and LinearCombinations.hpp for a different
       * approach to the same task.
       */
   class ComputeMelbourneWubbena : public ComputeCombination
   {
   public:

         /// Default constructor
      ComputeMelbourneWubbena();


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(ProcessingException);


         /// Some Rinex data files provide C1 instead of P1. Use this method
         /// in those cases.
      virtual ComputeMelbourneWubbena& useC1(void)
      { type1 = TypeID::C1; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeMelbourneWubbena() {};


    protected:


         /// Compute the combination of observables.
      virtual double getCombination( const double& p1,
                                     const double& p2,
                                     const double& l1,
                                     const double& l2 );


         /// Dummy function.
      virtual double getCombination( const double& obs1,
                                     const double& obs2 )
      { return 0.0; };


    private:


         /// Type of observation to be combined. Nro 3.
      TypeID type3;


         /// Type of observation to be combined. Nro 4.
      TypeID type4;


      const double DEN1;      // DEN1 = L1_FREQ + L2_FREQ
      const double DEN2;      // DEN2 = L1_FREQ - L2_FREQ


   }; // End of class 'ComputeMelbourneWubbena'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_COMPUTEMELBOURNEWUBBENA_HPP
