/**
 * @file ModeledReferencePR.cpp
 * Class to compute modeled pseudoranges using a reference station
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
//  Dagoberto Salazar - gAGE. 2006
//
//============================================================================



#include "ModeledReferencePR.hpp"

using namespace std;
namespace gpstk
{

    /* Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
     * @param Tr            Measured time of reception of the data.
     * @param Satellite     Vector of satellites; on successful return, satellites that
     *                      were excluded by the algorithm are marked by a negative
     *                      Satellite[i].id
     * @param Pseudorange   Vector of raw pseudoranges (parallel to satellite), in meters.
     * @param Eph           EphemerisStore to be used.
     * @param pTropModel    Pointer to tropospheric model to be used (@sa TropModel.hpp). By
     *                      default points to NULL.
     * @param pIonoModel    Pointer to ionospheric model to be used (@sa IonoModelStore.hpp).
     *                      By default points to NULL.
     * @param extraBiases   Vector of extra biases to be added to the model.
     *
     * @return
     *  Number of satellites with valid data
     */
    int ModeledReferencePR::Compute(const DayTime& Tr, Vector<SatID>& Satellite, 
        Vector<double>& Pseudorange, const EphemerisStore& Eph, const Vector<double>& extraBiases,
        TropModel *pTropModel, IonoModelStore *pIonoModel) throw(Exception)
        {
        try {

            int N = Satellite.size();
            if(N <= 0) return 0;        // No deal if there are no satellites

            int i;
            int eN = int(extraBiases.size()) - 1;  // Maximum valid index of extraBiases vector
            int validSats = 0;
            // Some std::vector to hold temporal values (do not confuse with gpstk::Vector)
            vector<double> vPR;
            vector<double> vGeometricRho;
            vector<double> vClock;
            vector<double> vTGD;
            vector<double> vRel;
            vector<double> vTrop;
            vector<double> vIono;
            vector<double> vObservedPR;
            vector<double> vModeledPR;
            vector<double> vPrefit;
            vector<double> vElevation;
            vector<double> vAzimuth;
            vector<Xvt> vXvt;
            vector<DayTime> vTxTime;
            vector<SatID> vAvailableSV;
            vector<SatID> vRejectedSV;
            vector<Triple> vCosines;
            vector<Triple>::iterator iter;  // Iterator for vCosines vector
            // A lot of the work is done by a CorrectedEphemerisRange object
            CorrectedEphemerisRange cerange;
            validData = false;

            // Let's make sure everything is clean
            rejectedSV.resize(0);
            availableSV.resize(0);
            geometricRho.resize(0);
            svClockBiases.resize(0);
            svXvt.resize(0);
            svTxTime.resize(0);
            svTGD.resize(0);
            svRelativity.resize(0);
            ionoCorrections.resize(0);
            tropoCorrections.resize(0);
            observedPseudoranges.resize(0);
            modeledPseudoranges.resize(0);
            prefitResiduals.resize(0);
            elevationSV.resize(0);
            azimuthSV.resize(0);
            geoMatrix.resize(0, 0);

            for (i=0; i<N; i++) {
                if(Satellite[i].id <= 0) {      // Skip marked satellites
                    vRejectedSV.push_back(Satellite[i]);
                    continue;
                }
                try {
                    // Scalars to hold temporal values
                    double tempPR = 0.0;
                    double tempTGD = 0.0;
                    double tempTrop = 0.0;
                    double tempIono = 0.0;
                    double tempModeledPR = 0.0;
                    double tempPrefit = 0.0;

                    try {
                        // Compute most of the parameters
                        tempPR = cerange.ComputeAtTransmitTime(Tr, Pseudorange[i], rxPos, Satellite[i], Eph);
                    }
                    catch(EphemerisStore::NoEphemerisFound& e) {
                        // If there were no ephemeris for this satellite, let's mark it
                        Satellite[i].id = -std::abs(Satellite[i].id);
                        vRejectedSV.push_back(Satellite[i]);
                        continue;
                    };

                    // Let's test if satellite has enough elevation over horizon
                    if (rxPos.elevationGeodetic(cerange.svPosVel) < (*this).minElev) {
                        // Mark this satellite if it doesn't have enough elevation
                        Satellite[i].id = -std::abs(Satellite[i].id);
                        vRejectedSV.push_back(Satellite[i]);
                        continue;
                    }

                    // If given, computes tropospheric model
                    if(pTropModel) tempTrop = getTropoCorrections(pTropModel, cerange.elevationGeodetic);

                    // If given, computes ionospheric model
                    if(pIonoModel) {
                        Geodetic rxGeo(rxPos.getGeodeticLatitude(), rxPos.getLongitude(), rxPos.getAltitude());     // Convert Position rxPos to Geodetic rxGeo
                        tempIono = getIonoCorrections(pIonoModel, Tr, rxGeo, cerange.elevationGeodetic, cerange.azimuthGeodetic);
                    };

                    tempModeledPR = tempPR + tempTrop + tempIono;

                    // Check if there could be an extra bias defined for this satellite
                    if (i <= eN ) tempModeledPR += extraBiases(i);

                    // Computing Total Group Delay (TGD - meters) and adding it to result
                    if(useTGD) {
                        tempTGD = getTGDCorrections(Tr, Eph, Satellite[i]);
                        tempModeledPR += tempTGD;
                    }

                    tempPrefit = Pseudorange[i] - tempModeledPR;

                    // Let's store the results in their corresponding std::vector
                    vGeometricRho.push_back(cerange.rawrange);
                    vClock.push_back(cerange.svclkbias);
                    vXvt.push_back(cerange.svPosVel);
                    vTxTime.push_back(cerange.transmit);
                    vTGD.push_back(tempTGD);
                    // Relativity was computed with a negative sign added
                    vRel.push_back(-cerange.relativity);
                    vIono.push_back(tempIono);
                    vTrop.push_back(tempTrop);
                    vObservedPR.push_back(Pseudorange[i]);
                    vModeledPR.push_back(tempModeledPR);
                    vPrefit.push_back(tempPrefit);
                    vElevation.push_back(cerange.elevationGeodetic);
                    vAzimuth.push_back(cerange.azimuthGeodetic);
                    vAvailableSV.push_back(Satellite[i]);
                    vCosines.push_back(cerange.cosines);

                    validSats += 1;     // If everything is OK, increment valid sat's counter

                }   // End of try
                catch(EphemerisStore::NoEphemerisFound& e) {
                    // If there were no ephemeris for this satellite, let's mark it
                    Satellite[i].id = -std::abs(Satellite[i].id);
                    vRejectedSV.push_back(Satellite[i]);
                    continue;
                }
                catch(...) {
                    Exception unknownEx("An unknown exception has happened in ModeledReferencePR object.");
                    GPSTK_THROW(unknownEx);
                }

            }  // End of "for" loop

            // Let's storage results in their corresponding gpstk::Vector
            rejectedSV = vRejectedSV;
            availableSV = vAvailableSV;
            geometricRho = vGeometricRho;
            svClockBiases = vClock;
            svXvt = vXvt;
            svTxTime = vTxTime;
            svTGD = vTGD;
            svRelativity = vRel;
            ionoCorrections = vIono;
            tropoCorrections = vTrop;
            observedPseudoranges = vObservedPR;
            modeledPseudoranges = vModeledPR;
            prefitResiduals = vPrefit;
            elevationSV = vElevation;
            azimuthSV = vAzimuth;

            // Let's fill in the Geometry Matrix
            geoMatrix.resize((size_t)validSats, 4); // First, set the correct size
            int counter=0;
            for ( iter=vCosines.begin(); iter!=vCosines.end(); iter++ )
            {
                geoMatrix(counter,0) = (*iter)[0];
                geoMatrix(counter,1) = (*iter)[1];
                geoMatrix(counter,2) = (*iter)[2];
                geoMatrix(counter,3) = 1.0;  // When using pseudorange method, this is 1.0
                counter++;
            }

            if (validSats >= 4) validData = true;

            return validSats;  // Return number of valid satellites

        }  // end of "try"
        catch(Exception& e) {
            GPSTK_RETHROW(e);
        }
    }  // end ModeledReferencePR::Compute()


    // Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
    int ModeledReferencePR::Compute(const DayTime& Tr, Vector<SatID>& Satellite, 
        Vector<double>& Pseudorange, const EphemerisStore& Eph) throw(Exception)
        {
            // Create missing parameters
            Vector<double> vectorBIAS(1, 0.0);

            // Let's call the most complete Compute method defined above
            return ModeledReferencePR::Compute(Tr, Satellite, Pseudorange, Eph, vectorBIAS);
    }


    // Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
    int ModeledReferencePR::Compute(const DayTime& Tr, Vector<SatID>& Satellite, 
        Vector<double>& Pseudorange, const EphemerisStore& Eph, TropModel *pTropModel)
        throw(Exception)
        {
            // Create missing parameters
            Vector<double> vectorBIAS(1, 0.0);

            // Let's call the most complete Compute method defined above
            return ModeledReferencePR::Compute(Tr, Satellite, Pseudorange, Eph, vectorBIAS, pTropModel);
    }


    // Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
    int ModeledReferencePR::Compute(const DayTime& Tr, Vector<SatID>& Satellite, 
        Vector<double>& Pseudorange, const EphemerisStore& Eph, const Vector<double>& extraBiases,
        IonoModelStore *pIonoModel) throw(Exception)
        {
            // Create missing parameters
            TropModel *pTropModel=NULL;

            // Let's call the most complete Compute method defined above
            return ModeledReferencePR::Compute(Tr, Satellite, Pseudorange, Eph, extraBiases, pTropModel, pIonoModel);    
    }


    // Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
    int ModeledReferencePR::Compute(const DayTime& Tr, Vector<SatID>& Satellite, 
        Vector<double>& Pseudorange, const EphemerisStore& Eph, IonoModelStore *pIonoModel)
        throw(Exception)
        {
            // Create missing parameters
            Vector<double> vectorBIAS(1, 0.0);
            TropModel *pTropModel=NULL;

            // Let's call the most complete Compute method defined above
            return ModeledReferencePR::Compute(Tr, Satellite, Pseudorange, Eph, vectorBIAS, pTropModel, pIonoModel);    
    }


    // Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
    int ModeledReferencePR::Compute(const DayTime& Tr, Vector<SatID>& Satellite, 
        Vector<double>& Pseudorange, const EphemerisStore& Eph, TropModel *pTropModel,
        IonoModelStore *pIonoModel) throw(Exception)
        {
            // Create missing parameters
            Vector<double> vectorBIAS(1, 0.0);

            // Let's call the most complete Compute method defined above
            return ModeledReferencePR::Compute(Tr, Satellite, Pseudorange, Eph, vectorBIAS, pTropModel, pIonoModel);    
    }


    /* Compute just one modeled pseudorange, given satellite ID's, pseudorange and other data
     * @param Tr            Measured time of reception of the data.
     * @param Satellite     ID's of satellite
     * @param Pseudorange   Pseudorange (parallel to satellite), in meters.
     * @param Eph           EphemerisStore to be used.
     * @param pTropModel    Pointer to tropospheric model to be used (@sa TropModel.hpp). By
     *                      default points to NULL.
     * @param pIonoModel    Pointer to ionospheric model to be used (@sa IonoModelStore.hpp).
     *                      By default points to NULL.
     * @param extraBiases   Extra bias to be added to the model.
     *
     * @return
     *  1 if satellite has valid data
     */
    int ModeledReferencePR::Compute(const DayTime& Tr, SatID& Satellite, double& Pseudorange,
        const EphemerisStore& Eph, const double& extraBiases, TropModel *pTropModel,
        IonoModelStore *pIonoModel) throw(Exception)
        {
            // Create Vectors from scalar values
            Vector<SatID> vectorSV(1, Satellite);
            Vector<double> vectorPR(1, Pseudorange);
            Vector<double> vectorBIAS(1, extraBiases);
            
            // Now, let's call one of the Compute methods defined above
            return ModeledReferencePR::Compute(Tr, vectorSV, vectorPR, Eph, vectorBIAS, pTropModel, pIonoModel);
    }


} // namespace

