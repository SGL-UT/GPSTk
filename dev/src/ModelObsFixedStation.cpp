
/**
 * @file ModelObsFixedStation.cpp
 * This is a class to compute modeled (corrected) observations from a reference station (whose position is known), using GNSS data structures.
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


#include "ModelObsFixedStation.hpp"


namespace gpstk
{


    // Index initially assigned to this class
    int ModelObsFixedStation::classIndex = 3000000;


    // Returns an index identifying this object.
    int ModelObsFixedStation::getIndex() const { return (*this).index; }


    // Returns a string identifying this object.
    std::string ModelObsFixedStation::getClassName() const { return "ModelObsFixedStation"; }


     /* Returns a satTypeValueMap object, adding the new data generated when calling a modeling object.
      *
      * @param time      Epoch.
      * @param gData     Data object holding the data.
      */
    satTypeValueMap& ModelObsFixedStation::Process(const DayTime& time, satTypeValueMap& gData) throw(Exception)
    {

        SatIDSet satRejectedSet;


        try
        {

            // Loop through all the satellites
            satTypeValueMap::iterator stv;
            for (stv = gData.begin(); stv != gData.end(); ++stv) 
            {
                // Scalars to hold temporal values
                double tempPR(0.0);
                double tempTrop(0.0);
                double tempIono(0.0);
                double tempModeledPR(0.0);
                double tempTGD(0.0);
                double tempPrefit(0.0);
                double observable( (*stv).second(defaultObservable) );

                // A lot of the work is done by a CorrectedEphemerisRange object
                CorrectedEphemerisRange cerange;

                try {
                    // Compute most of the parameters
                    tempPR = cerange.ComputeAtTransmitTime(time, observable, rxPos, (*stv).first, *(*this).getDefaultEphemeris() );
                }
                catch(InvalidRequest& e) {
                    // If some problem appears, then schedule this satellite for removal
                    satRejectedSet.insert( (*stv).first );
                    continue;    // Skip this SV if problems arise
                };

                // Let's test if satellite has enough elevation over horizon
                if (rxPos.elevationGeodetic(cerange.svPosVel) < (*this).minElev)
                {
                    // Mark this satellite if it doesn't have enough elevation
                    satRejectedSet.insert( (*stv).first );
                    continue;
                }

                // If given, computes tropospheric model
                if(pDefaultTropoModel){
                    tempTrop = getTropoCorrections(pDefaultTropoModel, cerange.elevationGeodetic);
                    (*stv).second[TypeID::tropoSlant] = tempTrop;
                }

                // If given, computes ionospheric model
                if(pDefaultIonoModel)
                {
                    // Convert Position rxPos to Geodetic rxGeo
                    Geodetic rxGeo(rxPos.getGeodeticLatitude(), rxPos.getLongitude(), rxPos.getAltitude());
                    tempIono = getIonoCorrections(pDefaultIonoModel, time, rxGeo, cerange.elevationGeodetic, cerange.azimuthGeodetic);
                    (*stv).second[TypeID::ionoSlant] = tempIono;
                };

                tempModeledPR = tempPR + tempTrop + tempIono;

                // Computing Total Group Delay (TGD - meters) and adding it to result
                if(useTGD)
                {
                    tempTGD = getTGDCorrections(time, (*pDefaultEphemeris), (*stv).first);
                    tempModeledPR += tempTGD;
                }

                tempPrefit = observable - tempModeledPR;

                // Now we have to add the new values to the data structure
                (*stv).second[TypeID::prefitC] = tempPrefit;
                (*stv).second[TypeID::dtSat] = cerange.svclkbias;

                // Now, lets insert the geometry matrix
                (*stv).second[TypeID::dx] = cerange.cosines[0];
                (*stv).second[TypeID::dy] = cerange.cosines[1];
                (*stv).second[TypeID::dz] = cerange.cosines[2];
                (*stv).second[TypeID::cdt] = 1.0;   // When using pseudorange method, this is 1.0

                // Now we have to add the new values to the data structure
                (*stv).second[TypeID::rho] = cerange.rawrange;
                (*stv).second[TypeID::rel] = -cerange.relativity;
                (*stv).second[TypeID::elevation] = cerange.elevationGeodetic;
                (*stv).second[TypeID::azimuth] = cerange.azimuthGeodetic;

                // Get the instrumental delays right
                if (useTGD)
                {
                    TypeID instDelayType;
                    switch ( (*this).getDefaultObservable().type )
                    {
                        case TypeID::C1:
                            instDelayType = TypeID::instC1;
                            break;
                        case TypeID::C2:
                            instDelayType = TypeID::instC2;
                            break;
                        case TypeID::C5:
                            instDelayType = TypeID::instC5;
                            break;
                        case TypeID::C6:
                            instDelayType = TypeID::instC6;
                            break;
                        case TypeID::C7:
                            instDelayType = TypeID::instC7;
                            break;
                        case TypeID::C8:
                            instDelayType = TypeID::instC8;
                            break;
                        default:
                            instDelayType = TypeID::instC1;
                    };
                    (*stv).second[instDelayType] = tempTGD;
                }


            } // End of loop for (stv = gData.begin()...

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;

        }   // End of try...
        catch(Exception& e) {
            GPSTK_RETHROW(e);
        }

    }   // End ModelObsFixedStation::Process(const DayTime& time, satTypeValueMap& gData)


} // end namespace gpstk
