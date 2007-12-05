#pragma ident "$Id: $"

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007
//
//============================================================================



#include "ModeledReferencePR.hpp"

using namespace std;
namespace gpstk
{


      /* Explicit constructor, taking as input reference station
       * coordinates, default ionospheric and tropospheric models,
       * ephemeris to be used, default observable and whether TGD will 
       * be computed or not.
       *
       * This constructor is meant to be used when working with GNSS data
       * structures in order to set the basic parameters from the beginning.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dIonoModel    Ionospheric model to be used by default.
       * @param dTropoModel   Tropospheric model to be used by default.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledReferencePR::ModeledReferencePR( const Position& RxCoordinates,
                                           IonoModelStore& dIonoModel,
                                           TropModel& dTropoModel,
                                           XvtStore<SatID>& dEphemeris,
                                           const TypeID& dObservable,
                                           bool usetgd )
      throw(Exception)
   { 
      init();
      setInitialRxPosition(RxCoordinates);
      setDefaultIonoModel(dIonoModel);
      setDefaultTropoModel(dTropoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;
   }


      /* Explicit constructor, taking as input reference station
       * coordinates, default ionospheric model, ephemeris to be used,
       * default observable and whether TGD will be computed or not.
       *
       * The default tropospheric model will be set to NULL.
       *
       * This constructor is meant to be used when working with GNSS data
       * structures in order to set the basic parameters from the
       * beginning.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dIonoModel    Ionospheric model to be used by default.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledReferencePR::ModeledReferencePR( const Position& RxCoordinates,
                                           IonoModelStore& dIonoModel,
                                           XvtStore<SatID>& dEphemeris,
                                           const TypeID& dObservable,
                                           bool usetgd )
      throw(Exception)
   { 
      init();
      setInitialRxPosition(RxCoordinates);
      setDefaultIonoModel(dIonoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;
      pDefaultTropoModel = NULL;
   }


      /* Explicit constructor, taking as input reference station
       * coordinates, default tropospheric model, ephemeris to be used,
       * default observable and whether TGD will be computed or not.
       *
       * The default ionospheric model will be set to NULL.
       *
       * This constructor is meant to be used when working with GNSS 
       * data structures in order to set the basic parameters from the
       * beginning.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dTropoModel   Tropospheric model to be used by default.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledReferencePR::ModeledReferencePR( const Position& RxCoordinates,
                                           TropModel& dTropoModel,
                                           XvtStore<SatID>& dEphemeris,
                                           const TypeID& dObservable,
                                           bool usetgd )
      throw(Exception)
   {
      init();
      setInitialRxPosition(RxCoordinates);
      setDefaultTropoModel(dTropoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;
      pDefaultIonoModel = NULL;
   }


      /* Explicit constructor, taking as input reference station
       * coordinates, ephemeris to be used, default observable and 
       * whether TGD will be computed or not.
       *
       * Both the tropospheric and ionospheric models will be set to NULL.
       *
       * This constructor is meant to be used when working with GNSS 
       * data structures in order to set the basic parameters from the
       * beginning.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledReferencePR::ModeledReferencePR( const Position& RxCoordinates,
                                           XvtStore<SatID>& dEphemeris,
                                           const TypeID& dObservable,
                                           bool usetgd )
      throw(Exception)
   {
      init();
      setInitialRxPosition(RxCoordinates);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;
      pDefaultIonoModel = NULL;
      pDefaultTropoModel = NULL;
   }


      /* Compute the modeled pseudoranges, given satellite ID's,
       * pseudoranges and other data.
       *
       * @param Tr            Measured time of reception of the data.
       * @param Satellite     Vector of satellites
       * @param Pseudorange   Vector of raw pseudoranges (parallel to
       *                      satellite), in meters.
       * @param Eph           EphemerisStore to be used.
       * @param extraBiases   Vector of extra biases to be added to the model.
       * @param pTropModel    Pointer to tropospheric model to be used.
       *                      By default points to NULL.
       * @param pIonoModel    Pointer to ionospheric model to be used.
       *                      By default points to NULL.
       *
       * @return Number of satellites with valid data.
       */
   int ModeledReferencePR::Compute( const DayTime& Tr,
                                    Vector<SatID>& Satellite, 
                                    Vector<double>& Pseudorange,
                                    const XvtStore<SatID>& Eph,
                                    const Vector<double>& extraBiases,
                                    TropModel *pTropModel,
                                    IonoModelStore *pIonoModel )
      throw(Exception)
   {
      try
      {

         int N = Satellite.size();
         if(N <= 0) return 0;        // No deal if there are no satellites

         int i;
            // Maximum valid index of extraBiases vector
         int eN = int(extraBiases.size()) - 1;
         int validSats(0);
            // Some std::vector to hold temporal values (do not confuse 
            // with gpstk::Vector)
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

         for (i=0; i<N; i++)
         {
               // Skip marked satellites
            if(Satellite[i].id <= 0)
            {
                  // First, make sure we are using a positive satellite id
                  SatID tempSat( std::abs(Satellite[i].id),
                                 Satellite[i].system);
                  vRejectedSV.push_back(tempSat);
                  continue;
            }

            try
            {
                  // Scalars to hold temporal values
               double tempPR(0.0);
               double tempTGD(0.0);
               double tempTrop(0.0);
               double tempIono(0.0);
               double tempModeledPR(0.0);
               double tempPrefit(0.0);

               try
               {
                     // Compute most of the parameters
                  tempPR = cerange.ComputeAtTransmitTime( Tr,
                                                          Pseudorange[i],
                                                          rxPos,
                                                          Satellite[i],
                                                          Eph );
               }
               catch(InvalidRequest& e)
               {
                     // If there were no ephemeris for this satellite, 
                     // let's mark it
                  vRejectedSV.push_back(Satellite[i]);
                  continue;
               }; // End of "try/catch" for cerange

                  // Let's test if satellite has enough elevation over horizon
               if(rxPos.elevationGeodetic(cerange.svPosVel) < (*this).minElev)
               {
                     // Mark this satellite if it doesn't have
                     // enough elevation
                  vRejectedSV.push_back(Satellite[i]);
                  continue;
               };

                  // If given, computes tropospheric model
               if(pTropModel)
               {
                  tempTrop = getTropoCorrections( pTropModel,
                                                  cerange.elevationGeodetic );
               };

                  // If given, computes ionospheric model
               if(pIonoModel)
               {
                     // Convert Position rxPos to Geodetic rxGeo
                  Geodetic rxGeo( rxPos.getGeodeticLatitude(),
                                  rxPos.getLongitude(),
                                  rxPos.getAltitude() );

                  tempIono = getIonoCorrections( pIonoModel,
                                                 Tr,
                                                 rxGeo,
                                                 cerange.elevationGeodetic,
                                                 cerange.azimuthGeodetic );
               };

               tempModeledPR = tempPR + tempTrop + tempIono;

                  // Check if there could be an extra bias defined for 
                  // this satellite
               if (i <= eN )
               {
                  tempModeledPR += extraBiases(i);
               };

                  // Computing Total Group Delay (TGD - meters) and adding 
                  // it to result
               if(useTGD)
               {
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

                  // If everything is OK, increment valid sat's counter
               validSats += 1;

            }   // End of try
            catch(InvalidRequest& e)
            {
                  // If there were no ephemeris for this satellite, 
                  // let's mark it
               vRejectedSV.push_back(Satellite[i]);
               continue;
            }
            catch(...)
            {
               Exception unknownEx("An unknown exception has happened in \
                                    ModeledReferencePR object.");
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
            // First, set the correct size
         geoMatrix.resize((size_t)validSats, 4);
         int counter(0);
         for ( iter=vCosines.begin(); iter!=vCosines.end(); iter++ )
         {
            geoMatrix(counter,0) = (*iter)[0];
            geoMatrix(counter,1) = (*iter)[1];
            geoMatrix(counter,2) = (*iter)[2];
               // When using pseudorange method, this is 1.0
            geoMatrix(counter,3) = 1.0;
            counter++;
         }

         if (validSats >= 4)
         {
            validData = true;
         }

         return validSats;  // Return number of valid satellites

      }  // end of "try"
      catch(Exception& e)
      {
         GPSTK_RETHROW(e);
      }
   }  // end ModeledReferencePR::Compute()


      // Compute the modeled pseudoranges, given satellite ID's,
      // pseudoranges and other data.
   int ModeledReferencePR::Compute( const DayTime& Tr,
                                    Vector<SatID>& Satellite,
                                    Vector<double>& Pseudorange,
                                    const XvtStore<SatID>& Eph )
      throw(Exception)
   {
         // Create missing parameters
      Vector<double> vectorBIAS(1, 0.0);

         // Let's call the most complete Compute method defined above
      return ModeledReferencePR::Compute( Tr,
                                          Satellite,
                                          Pseudorange,
                                          Eph,
                                          vectorBIAS );
   }


      // Compute the modeled pseudoranges, given satellite ID's,
      // pseudoranges and other data.
   int ModeledReferencePR::Compute( const DayTime& Tr,
                                    Vector<SatID>& Satellite,
                                    Vector<double>& Pseudorange,
                                    const XvtStore<SatID>& Eph,
                                    TropModel *pTropModel )
      throw(Exception)
   {
         // Create missing parameters
      Vector<double> vectorBIAS(1, 0.0);

         // Let's call the most complete Compute method defined above
      return ModeledReferencePR::Compute( Tr,
                                          Satellite,
                                          Pseudorange,
                                          Eph,
                                          vectorBIAS,
                                          pTropModel );
   }


      // Compute the modeled pseudoranges, given satellite ID's,
      // pseudoranges and other data.
   int ModeledReferencePR::Compute( const DayTime& Tr,
                                    Vector<SatID>& Satellite, 
                                    Vector<double>& Pseudorange,
                                    const XvtStore<SatID>& Eph,
                                    const Vector<double>& extraBiases,
                                    IonoModelStore *pIonoModel )
      throw(Exception)
   {
         // Create missing parameters
      TropModel *pTropModel=NULL;

         // Let's call the most complete Compute method defined above
      return ModeledReferencePR::Compute( Tr,
                                          Satellite,
                                          Pseudorange,
                                          Eph,
                                          extraBiases,
                                          pTropModel,
                                          pIonoModel );
   }


      // Compute the modeled pseudoranges, given satellite ID's,
      // pseudoranges and other data.
   int ModeledReferencePR::Compute( const DayTime& Tr,
                                    Vector<SatID>& Satellite,
                                    Vector<double>& Pseudorange,
                                    const XvtStore<SatID>& Eph,
                                    IonoModelStore *pIonoModel )
      throw(Exception)
   {
         // Create missing parameters
      Vector<double> vectorBIAS(1, 0.0);
      TropModel *pTropModel=NULL;

         // Let's call the most complete Compute method defined above
      return ModeledReferencePR::Compute( Tr,
                                          Satellite,
                                          Pseudorange,
                                          Eph,
                                          vectorBIAS,
                                          pTropModel,
                                          pIonoModel );
   }


      // Compute the modeled pseudoranges, given satellite ID's,
      // pseudoranges and other data.
   int ModeledReferencePR::Compute( const DayTime& Tr,
                                    Vector<SatID>& Satellite, 
                                    Vector<double>& Pseudorange,
                                    const XvtStore<SatID>& Eph,
                                    TropModel *pTropModel,
                                    IonoModelStore *pIonoModel )
      throw(Exception)
   {
         // Create missing parameters
      Vector<double> vectorBIAS(1, 0.0);

         // Let's call the most complete Compute method defined above
      return ModeledReferencePR::Compute( Tr,
                                          Satellite,
                                          Pseudorange,
                                          Eph,
                                          vectorBIAS,
                                          pTropModel,
                                          pIonoModel );
   }


      /* Compute just one modeled pseudorange, given satellite ID's,
       * pseudorange and other data.
       *
       * @param Tr            Measured time of reception of the data.
       * @param Satellite     ID's of satellite
       * @param Pseudorange   Pseudorange (parallel to satellite), in meters.
       * @param Eph           XvtStore<SatID> to be used.
       * @param pTropModel    Pointer to tropospheric model to be used
       *                      By default points to NULL.
       * @param pIonoModel    Pointer to ionospheric model to be used
       *                      By default points to NULL.
       * @param extraBiases   Extra bias to be added to the model.
       *
       * @return
       *  1 if satellite has valid data
       */
   int ModeledReferencePR::Compute( const DayTime& Tr,
                                    SatID& Satellite,
                                    double& Pseudorange,
                                    const XvtStore<SatID>& Eph,
                                    const double& extraBiases,
                                    TropModel *pTropModel,
                                    IonoModelStore *pIonoModel )
      throw(Exception)
   {
         // Create Vectors from scalar values
      Vector<SatID> vectorSV(1, Satellite);
      Vector<double> vectorPR(1, Pseudorange);
      Vector<double> vectorBIAS(1, extraBiases);
            
         // Now, let's call one of the Compute methods defined above
      return ModeledReferencePR::Compute( Tr,
                                          vectorSV,
                                          vectorPR,
                                          Eph,
                                          vectorBIAS,
                                          pTropModel,
                                          pIonoModel );
   }


      /* Returns a satTypeValueMap object, adding the new data generated
       * when calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ModeledReferencePR::processModel( const DayTime& time,
                                                      satTypeValueMap& gData )
      throw(Exception)
   {

      Vector<SatID> Vsat = gData.getVectorOfSatID();
      Vector<double> Vprange=gData.getVectorOfTypeID(getDefaultObservable());

      try
      {
            // Call the Compute() method with the defaults. Those defaults 
            // MUST HAVE BEEN previously set, usually when creating this 
            // object with the appropriate constructor.
         Compute( time,
                  Vsat,
                  Vprange,
                  (*(getDefaultEphemeris())),
                  extraBiases,
                  getDefaultTropoModel(),
                  getDefaultIonoModel() );

            // Once we get the result, it may be necessary to make 
            // some satellite cleanup
         SatIDSet rejectedSet;
         for (size_t i = 0; i<rejectedSV.size(); ++i)
         {
            rejectedSet.insert(rejectedSV[i]);
         }

            // All rejected satellites are removed
         gData.removeSatID(rejectedSet);

            // Now we have to add the new values to the data structure
         gData.insertTypeIDVector(TypeID::prefitC, prefitResiduals);
         gData.insertTypeIDVector(TypeID::rho, geometricRho);
         gData.insertTypeIDVector(TypeID::dtSat, svClockBiases);
         gData.insertTypeIDVector(TypeID::rel, svRelativity);
         gData.insertTypeIDVector(TypeID::ionoSlant, ionoCorrections);
         gData.insertTypeIDVector(TypeID::tropoSlant, tropoCorrections);
         gData.insertTypeIDVector(TypeID::elevation, elevationSV);
         gData.insertTypeIDVector(TypeID::azimuth, azimuthSV);

            // Get the instrumental delays right
         if (useTGD)
         {
            TypeID instDelayType;
            switch ( getDefaultObservable().type )
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
            };  // End "switch"

            gData.insertTypeIDVector(instDelayType, svTGD);
         } // End "if"

            // Now, lets insert the geometry matrix
         TypeIDSet tSet;
         tSet.insert(TypeID::dx);
         tSet.insert(TypeID::dy);
         tSet.insert(TypeID::dz);
         tSet.insert(TypeID::cdt);
         gData.insertMatrix(tSet, geoMatrix);


         return gData;
      }  // End of "try"
      catch(Exception& e)
      {
         GPSTK_RETHROW(e);
      }

   }   // End ModeledReferencePR::processModel()


      // Initialization method
   void ModeledReferencePR::init() throw(Exception)
   {
      setInitialRxPosition();
      geometricRho(0);
      svClockBiases(0);
      svXvt(0);
      svTGD(0);
      svRelativity(0);
      ionoCorrections(0);
      tropoCorrections(0);
      modeledPseudoranges(0);
      prefitResiduals(0);
      extraBiases(0);
      availableSV(0);
      rejectedSV(0);
   }


      /* Method to set the initial (a priori) position of receiver.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int ModeledReferencePR::setInitialRxPosition( const double& aRx,
                                                 const double& bRx,
                                                 const double& cRx, 
                                                 Position::CoordinateSystem s,
                                                 GeoidModel *geoid )
      throw(GeometryException) 
   {

      try
      {
         Position rxpos(aRx, bRx, cRx, s, geoid);
         setInitialRxPosition(rxpos);
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }
   }


      // Method to set the initial (a priori) position of receiver.
   int ModeledReferencePR::setInitialRxPosition(const Position& RxCoordinates)
      throw(GeometryException) 
   {

      try
      {
         rxPos = RxCoordinates;
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }
   }


      // Method to set the initial (a priori) position of receiver.
   int ModeledReferencePR::setInitialRxPosition() throw(GeometryException) 
   {

      try
      {
         Position rxpos(0.0, 0.0, 0.0, Position::Cartesian, NULL);
         setInitialRxPosition(rxpos);
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }
   }


      // Method to get the tropospheric corrections.
   double ModeledReferencePR::getTropoCorrections( TropModel *pTropModel,
                                                   double elevation )
      throw() 
   {

      double tropoCorr(0.0);

      try
      {
         tropoCorr = pTropModel->correction(elevation);
            // Check validity
         if(!(pTropModel->isValid()))
         {
            tropoCorr = 0.0;
         }
      }
      catch(TropModel::InvalidTropModel& e)
      {
         tropoCorr = 0.0;
      }

      return tropoCorr;

   }


      // Method to get the ionospheric corrections.
   double ModeledReferencePR::getIonoCorrections( IonoModelStore *pIonoModel,
                                                  DayTime Tr,
                                                  Geodetic rxGeo,
                                                  double elevation,
                                                  double azimuth )
      throw()
   {

      double ionoCorr(0.0);

      try
      {
         ionoCorr = pIonoModel->getCorrection(Tr, rxGeo, elevation, azimuth);
      }
      catch(IonoModelStore::NoIonoModelFound& e)
      {
         ionoCorr = 0.0;
      }

      return ionoCorr;

   }


      // Method to get TGD corrections.
   double ModeledReferencePR::getTGDCorrections( DayTime Tr,
                                                 const XvtStore<SatID>& Eph,
                                                 SatID sat )
      throw() 
   {

      try
      {
         const GPSEphemerisStore& bce = 
                                 dynamic_cast<const GPSEphemerisStore&>(Eph);

         const EngEphemeris& eph = bce.findEphemeris(sat,Tr);

         return ( bce.findEphemeris(sat,Tr).getTgd() * C_GPS_M );

      }
      catch(...)
      {
         return 0.0;
      }
   }


} // namespace
