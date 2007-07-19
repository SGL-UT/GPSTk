
/**
 * @file DeltaOp.cpp
 * This is a class to apply the Delta operator (differences on ground-related data) to GNSS data structures.
 */

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================


#include "DeltaOp.hpp"


namespace gpstk
{

    // Returns a reference to a gnssSatTypeValue object after differencing the
    // data type values given in the diffTypes field with respect to reference
    // station data in refData field.
    //
    // @param gData     Data object holding the data.
    //
    satTypeValueMap& DeltaOp::Difference(satTypeValueMap& gData)
    {
        SatIDSet satRejectedSet;

        // Loop through all the satellites in the station data set
        satTypeValueMap::iterator it;
        for (it = gData.begin(); it != gData.end(); ++it) 
        {
            satTypeValueMap::const_iterator itref;
            // Let's find if the same satellite is present in refData 
            itref = refData.find((*it).first);

            // If we found the satellite, let's proceed with the differences
            if (itref != refData.end())
            {
                // We must compute the difference for all the types in diffTypes set
                TypeIDSet::const_iterator itType;
                for (itType = diffTypes.begin(); itType != diffTypes.end(); ++itType)
                {
                    try
                    {
                        // Let's try to compute the difference
                        gData((*it).first)((*itType)) = ( gData((*it).first)((*itType)) - refData((*it).first)((*itType)) );
                    }
                    catch(...) 
                    {
                        continue;    // Skip this value if problems arise
                    }
                }

            } else {
                // If we didn't find the same satellite in both sets, mark it for deletion
                satRejectedSet.insert( (*it).first );
                continue;
            };
        }

        // If ordered so, delete the missing satellites
        if (deleteMissingSats) gData.removeSatID(satRejectedSet);

        return gData;

    }  // end DeltaOp::Difference()


} // end namespace gpstk
