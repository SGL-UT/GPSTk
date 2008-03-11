
/**
 * @file NablaOp.cpp
 * This is a class to apply the Nabla operator (differences on satellite-related data) to GNSS data structures.
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


#include "NablaOp.hpp"


namespace gpstk
{

    // Returns a reference to a gnssSatTypeValue object after differencing the
    // data type values given in the diffTypes field with respect to reference
    // satellite data.
    //
    // @param gData     Data object holding the data.
    //
    satTypeValueMap& NablaOp::Process(satTypeValueMap& gData)
    {

        double maxElevation(0.0);

        // If configured to do so, let's look for the reference satellite
        if (lookReferenceSat) {
            // Loop through all the satellites in the station data set, looking for the reference satellite
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it) 
            {
                if ( gData((*it).first)(TypeID::elevation) > maxElevation )
                {
                    refSat = (*it).first;
                    maxElevation = gData((*it).first)(TypeID::elevation);
                }
            }
        }

        // We will use the reference satellite data as reference data
        satTypeValueMap refData(gData.extractSatID(refSat));

        // We must remove the reference satellite data from the data set
        gData.removeSatID(refSat);

        SatIDSet satRejectedSet;

        // Loop through all the satellites in the station data set
        satTypeValueMap::iterator it;
        for (it = gData.begin(); it != gData.end(); ++it) 
        {
            // We must compute the difference for all the types in diffTypes set
            TypeIDSet::const_iterator itType;
            for (itType = diffTypes.begin(); itType != diffTypes.end(); ++itType)
            {

                double value1(0.0);
                double value2(0.0);

                try
                {
                    // Let's try to compute the difference
                    value1 = gData((*it).first)(*itType);
                    value2 = refData(refSat)(*itType);

                    gData((*it).first)((*itType)) =  value1 - value2;
                }
                catch(...) 
                {
                    // If some value is missing, then schedule this satellite for removal
                    satRejectedSet.insert( (*it).first );
                    continue;
                }
            }

        }
        // Remove satellites with missing data
        gData.removeSatID(satRejectedSet);

        return gData;

    }  // end NablaOp::Process()


    // Index initially assigned to this class
    int NablaOp::classIndex = 4600000;


    // Returns an index identifying this object.
    int NablaOp::getIndex() const { return (*this).index; }


    // Returns a string identifying this object.
    std::string NablaOp::getClassName() const { return "NablaOp"; }



} // end namespace gpstk
