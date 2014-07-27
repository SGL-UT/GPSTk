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
 * @file DeltaOp.cpp
 * This is a class to apply the Delta operator (differences on
 * ground-related data) to GNSS data structures.
 */

#include "DeltaOp.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string DeltaOp::getClassName() const
   { return "DeltaOp"; }


      /* Method to add a set of data value types to be differenced.
       *
       * @param diffSet       TypeIDSet of data values to be added to the
       *                      ones being differenced.
       */
   DeltaOp& DeltaOp::addDiffTypeSet(const TypeIDSet& diffSet)
   {

         // Iterate over 'diffSet' and add its components to 'diffTypes'
      TypeIDSet::const_iterator pos;
      for (pos = diffSet.begin(); pos != diffSet.end(); ++pos)
      {
         diffTypes.insert(*pos);
      }

      return (*this);

   }  // End of method 'DeltaOp::addDiffTypeSet()'



      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with respect to
       * reference station data in 'refData' field.
       *
       * @param gData      Data object holding the data.
       */
   satTypeValueMap& DeltaOp::Process(satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         SatIDSet satRejectedSet;

            // Loop through all the satellites in the station data set
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {

               // Let's find if the same satellite is present in refData
            satTypeValueMap::const_iterator itref;
            itref = refData.find((*it).first);

               // If we found the satellite, let's proceed with the differences
            if (itref != refData.end())
            {

                  // We must compute the difference for all the types in
                  // 'diffTypes' set
               TypeIDSet::const_iterator itType;
               for( itType = diffTypes.begin();
                    itType != diffTypes.end();
                    ++itType )
               {

                  double value1(0.0);
                  double value2(0.0);

                  try
                  {

                        // Let's try to compute the difference
                     value1 = gData((*it).first)(*itType);
                     value2 = refData((*it).first)(*itType);

                        // Get difference into data structure
                     gData((*it).first)((*itType)) =  value1 - value2;

                  }
                  catch(...)
                  {

                        // If some value is missing, then schedule this
                        // satellite for removal
                     satRejectedSet.insert( (*it).first );

                        // Skip this value if problems arise
                     continue;

                  }

               }  // End of 'for( itType = diffTypes.begin(); ...'

                  // update CSFlag
               if(updateCSFlag)
               {
                  double CSValue1 = gData[it->first][TypeID::CSL1] 
                                   +refData[it->first][TypeID::CSL1];
                  double CSValue2 = gData[it->first][TypeID::CSL2] 
                                   +refData[it->first][TypeID::CSL2];


                  gData[it->first][TypeID::CSL1] = (CSValue1 > 0.0) ? 1.0 : 0.0;
                  
                  gData[it->first][TypeID::CSL2] = (CSValue2 > 0.0) ? 1.0 : 0.0;

               }  // End of 'if(updateCSFlag)'

            }
            else
            {

                  // If we didn't find the same satellite in both sets, mark
                  // it for deletion
               satRejectedSet.insert( (*it).first );

               continue;

            }  // End of 'if (itref != refData.end())'

         }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'

            // If ordered so, delete the missing satellites
         if (deleteMissingSats)
         {
            gData.removeSatID(satRejectedSet);
         }

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'DeltaOp::Process()'


}  // End of namespace gpstk
