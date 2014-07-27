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
 * @file ComputeMelbourneWubbena.cpp
 * This class eases computing Melbourne-Wubbena combination for GNSS
 * data structures.
 */

#include "ComputeMelbourneWubbena.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string ComputeMelbourneWubbena::getClassName() const
   { return "ComputeMelbourneWubbena"; }


      // Default constructor
   ComputeMelbourneWubbena::ComputeMelbourneWubbena()
      : type3(TypeID::L1), type4(TypeID::L2), DEN1(L1_FREQ_GPS + L2_FREQ_GPS),
        DEN2(L1_FREQ_GPS - L2_FREQ_GPS)
   {
      type1 = TypeID::P1;
      type2 = TypeID::P2;
      resultType = TypeID::MWubbena;
   }


      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeMelbourneWubbena::Process(satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         double value1(0.0);
         double value2(0.0);
         double value3(0.0);
         double value4(0.0);

         SatIDSet satRejectedSet;

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it) 
         {
            try
            {
                  // Try to extract the values
               value1 = (*it).second(type1);
               value2 = (*it).second(type2);
               value3 = (*it).second(type3);
               value4 = (*it).second(type4);
            }
            catch(...)
            {
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
               continue;
            }

               // If everything is OK, then get the new value inside
               // the structure
            (*it).second[resultType] = getCombination( value1,
                                                       value2,
                                                       value3,
                                                       value4 );
         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeMelbourneWubbena::Process()'



      // Compute the combination of observables.
   double ComputeMelbourneWubbena::getCombination( const double& p1,
                                                   const double& p2,
                                                   const double& l1,
                                                   const double& l2 )
   {

      return (   ( L1_FREQ_GPS*l1 - L2_FREQ_GPS*l2 ) / ( DEN2 )
               - ( L1_FREQ_GPS*p1 + L2_FREQ_GPS*p2 ) / ( DEN1 ) );

   }  // End of method 'ComputeMelbourneWubbena::getCombination()'


}  // End of namespace gpstk
