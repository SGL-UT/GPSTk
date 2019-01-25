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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
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
 * @file DoubleOp.cpp
 * This is a class to apply the Double Difference operator (differences on
 * ground-related data and satellite) to GNSS data structures.
 */

#include "DoubleOp.hpp"

namespace gpstk
{

   // Returns a string identifying this object.
   std::string DoubleOp::getClassName() const
   { return "DoubleOp"; }


      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with respect to
       * reference station data in 'refData' field.
       *
       * @param gData      Data object holding the data.
       */
   satTypeValueMap& DoubleOp::Process(satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {
         // First, we get difference data between two stations 
         sdStations.Process(gData);
         
         // Second, we should check if the elevation of the ref satellite
         // is useable, if not, pick up a new ref satellite with the highest
         // elevation
         bool lookHigestElevation = true;

         if(refSatID.isValid())
         {
            satTypeValueMap::iterator it = gData.find(refSatID);
            if(it!=gData.end())
            {
               double elev = gData(it->first)(TypeID::elevation);
               if(elev > refSatMinElev) lookHigestElevation = false;
            }
         }

         if(lookHigestElevation)
         {
            double maxElevation(0.0);
            
            // Loop through all satellites in reference station data set,
            // looking for reference satellite
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it)
            {

               // The satellite with the highest elevation will usually be
               // the reference satellite
               if ( gData((*it).first)(TypeID::elevation) > maxElevation )
               {

                  refSatID = (*it).first;
                  maxElevation = gData((*it).first)(TypeID::elevation);
               }

            }  // end for

         }  // End 'if(lookHigestElevation)'
         
         // At last, We get the final DD data
         sdSatellites.setRefSat(refSatID);
         sdSatellites.Process(gData);
         
         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'DoubleOp::Process()'


}  // End of namespace gpstk
