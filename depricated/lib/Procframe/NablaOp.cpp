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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009, 2011
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
 * @file NablaOp.cpp
 * This is a class to apply the Nabla operator (differences on
 * satellite-related data) to GNSS data structures.
 */

#include "NablaOp.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string NablaOp::getClassName() const
   { return "NablaOp"; }



      /* Default constructor.
       *
       * By default it will difference prefitC, dx, dy, and dz data and will
       * take as reference satellite the one with the highest elevation.
       */
   NablaOp::NablaOp()
      : lookReferenceSat(true)
   {

         // Insert default types to be differenced
      diffTypes.insert(TypeID::prefitC);
      diffTypes.insert(TypeID::dx);
      diffTypes.insert(TypeID::dy);
      diffTypes.insert(TypeID::dz);

   }  // End of constructor 'NablaOp::NablaOp()'



      /* Common constructor taking as input the reference satellite
       * to be used.
       *
       * @param rSat    SatID of satellite to be used as reference.
       */
   NablaOp::NablaOp(const SatID& rSat)
      : refSat(rSat), lookReferenceSat(false)
   {

         // Insert default types to be differenced
      diffTypes.insert(TypeID::prefitC);
      diffTypes.insert(TypeID::dx);
      diffTypes.insert(TypeID::dy);
      diffTypes.insert(TypeID::dz);

   }  // End of constructor 'NablaOp::NablaOp()'



      /* Method to add a set of data value types to be differenced.
       *
       * @param diffSet    TypeIDSet of data values to be added to the
       *                   ones being differenced.
       */
   NablaOp& NablaOp::addDiffTypeSet(const TypeIDSet& diffSet)
   {

         // Iterate over 'diffSet' to add its elements to 'diffTypes'
      TypeIDSet::const_iterator pos;
      for(pos = diffSet.begin(); pos != diffSet.end(); ++pos)
      {
         diffTypes.insert(*pos);
      }

      return (*this);

   }  // End of method 'NablaOp::addDiffTypeSet()'



    // Returns a reference to a gnssSatTypeValue object after differencing the
    // data type values given in the diffTypes field with respect to reference
    // satellite data.
    //
    // @param gData     Data object holding the data.
    //
    satTypeValueMap& NablaOp::Process(satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         double maxElevation(0.0);


            // If configured to do so, let's look for reference satellite
         if (lookReferenceSat)
         {

               // Loop through all satellites in reference station data set,
               // looking for reference satellite
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it)
            {

                  // The satellite with the highest elevation will usually be
                  // the reference satellite
               if ( gData((*it).first)(TypeID::elevation) > maxElevation )
               {

                  refSat = (*it).first;
                  maxElevation = gData((*it).first)(TypeID::elevation);

               }

            }

         }  // End of 'if (lookReferenceSat)'


            // We will use reference satellite data as reference data
         satTypeValueMap refData(gData.extractSatID(refSat));

            // We must remove reference satellite data from data set
         gData.removeSatID(refSat);


         SatIDSet satRejectedSet;


            // Loop through all the satellites in station data set
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {

               // We must compute the difference for all types in
               // 'diffTypes' set
            TypeIDSet::const_iterator itType;
            for(itType = diffTypes.begin(); itType != diffTypes.end(); ++itType)
            {

               double value1(0.0);
               double value2(0.0);

               try
               {

                     // Let's try to compute the difference
                  value1 = gData((*it).first)(*itType);
                  value2 = refData(refSat)(*itType);

                     // Get difference into data structure
                  gData((*it).first)((*itType)) =  value1 - value2;

               }
               catch(...)
               {

                     // If some value is missing, then schedule this satellite
                     // for removal
                  satRejectedSet.insert( (*it).first );

                  continue;

               }

            }  // End of 'for(itType = diffTypes.begin(); ...'

         }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'


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

   }  // End of method 'NablaOp::Process()'



}  // End of namespace gpstk
