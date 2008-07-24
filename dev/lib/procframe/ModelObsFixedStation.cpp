#pragma ident "$Id$"

/**
 * @file ModelObsFixedStation.cpp
 * This is a class to compute modeled (corrected) observations from a
 * reference station (whose position is known), using GNSS data structures.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================


#include "ModelObsFixedStation.hpp"


namespace gpstk
{


      // Index initially assigned to this class
   int ModelObsFixedStation::classIndex = 4100000;


      // Returns an index identifying this object.
   int ModelObsFixedStation::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ModelObsFixedStation::getClassName() const
   { return "ModelObsFixedStation"; }



      /* Explicit constructor taking as input reference
       * station coordinates.
       *
       * Those coordinates may be Cartesian (X, Y, Z in meters) or Geodetic
       * (Latitude, Longitude, Altitude), but defaults to Cartesian.
       *
       * Also, a pointer to GeoidModel may be specified, but default is
       * NULL (in which case WGS84 values will be used).
       *
       * @param aRx   first coordinate [ X(m), or latitude (degrees N) ]
       * @param bRx   second coordinate [ Y(m), or longitude (degrees E) ]
       * @param cRx   third coordinate [ Z, height above ellipsoid or
       *              radius, in meters ]
       * @param s     coordinate system (default is Cartesian, may be set
       *              to Geodetic).
       * @param geoid pointer to GeoidModel (default is null, implies WGS84)
       */
   ModelObsFixedStation::ModelObsFixedStation( const double& aRx,
                                               const double& bRx,
                                               const double& cRx,
                                               Position::CoordinateSystem s,
                                               GeoidModel *geoid )
   {

      minElev = 10.0;
      useTGD = true;
      pDefaultIonoModel = NULL;
      pDefaultTropoModel = NULL;
      defaultObservable = TypeID::C1;
      pDefaultEphemeris = NULL;
      InitializeValues();
      setInitialRxPosition(aRx, bRx, cRx, s, geoid);
      setIndex();

   }  // End of 'ModelObsFixedStation::ModelObsFixedStation()'



      // Explicit constructor, taking as input a Position object
      // containing reference station coordinates.
   ModelObsFixedStation::ModelObsFixedStation(const Position& RxCoordinates)
   {

      minElev = 10.0;
      useTGD = true;
      pDefaultIonoModel = NULL;
      pDefaultTropoModel = NULL;
      defaultObservable = TypeID::C1;
      pDefaultEphemeris = NULL;
      InitializeValues();
      setInitialRxPosition(RxCoordinates);
      setIndex();

   }  // End of 'ModelObsFixedStation::ModelObsFixedStation()'



      /* Explicit constructor, taking as input reference station
       * coordinates, default ionospheric and tropospheric models,
       * ephemeris to be used, default observable and whether TGD will
       * be computed or not.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dIonoModel    Ionospheric model to be used by default.
       * @param dTropoModel   Tropospheric model to be used by default.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       */
   ModelObsFixedStation::ModelObsFixedStation( const Position& RxCoordinates,
                                               IonoModelStore& dIonoModel,
                                               TropModel& dTropoModel,
                                               XvtStore<SatID>& dEphemeris,
                                               const TypeID& dObservable,
                                               bool usetgd )
   {

      minElev = 10.0;
      InitializeValues();
      setInitialRxPosition(RxCoordinates);
      setDefaultIonoModel(dIonoModel);
      setDefaultTropoModel(dTropoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;
      setIndex();

   }  // End of 'ModelObsFixedStation::ModelObsFixedStation()'



      /* Explicit constructor, taking as input reference station
       * coordinates, default ionospheric model, ephemeris to be used,
       * default observable and whether TGD will be computed or not.
       *
       * The default tropospheric model will be set to NULL.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dIonoModel    Ionospheric model to be used by default.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       */
   ModelObsFixedStation::ModelObsFixedStation( const Position& RxCoordinates,
                                               IonoModelStore& dIonoModel,
                                               XvtStore<SatID>& dEphemeris,
                                               const TypeID& dObservable,
                                               bool usetgd )
   {

      minElev = 10.0;
      pDefaultTropoModel = NULL;
      InitializeValues();
      setInitialRxPosition(RxCoordinates);
      setDefaultIonoModel(dIonoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;
      setIndex();

   }  // End of 'ModelObsFixedStation::ModelObsFixedStation()'



      /* Explicit constructor, taking as input reference station
       * coordinates, default tropospheric model, ephemeris to be used,
       * default observable and whether TGD will be computed or not.
       *
       * The default ionospheric model will be set to NULL.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dTropoModel   Tropospheric model to be used by default.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       */
   ModelObsFixedStation::ModelObsFixedStation( const Position& RxCoordinates,
                                               TropModel& dTropoModel,
                                               XvtStore<SatID>& dEphemeris,
                                               const TypeID& dObservable,
                                               bool usetgd )
   {

      minElev = 10.0;
      pDefaultIonoModel = NULL;
      InitializeValues();
      setInitialRxPosition(RxCoordinates);
      setDefaultTropoModel(dTropoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;
      setIndex();

   }  // End of 'ModelObsFixedStation::ModelObsFixedStation()'



      /* Explicit constructor, taking as input reference station
       * coordinates, ephemeris to be used, default observable and whether
       * TGD will be computed or not.
       *
       * Both the tropospheric and ionospheric models will be set to NULL.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       */
   ModelObsFixedStation::ModelObsFixedStation( const Position& RxCoordinates,
                                               XvtStore<SatID>& dEphemeris,
                                               const TypeID& dObservable,
                                               bool usetgd )
   {

      minElev = 10.0;
      pDefaultIonoModel = NULL;
      pDefaultTropoModel = NULL;
      InitializeValues();
      setInitialRxPosition(RxCoordinates);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;
      setIndex();

   }  // End of 'ModelObsFixedStation::ModelObsFixedStation()'



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ModelObsFixedStation::Process( const DayTime& time,
                                                   satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

         SatIDSet satRejectedSet;

            // Loop through all the satellites
         satTypeValueMap::iterator stv;
         for(stv = gData.begin(); stv != gData.end(); ++stv)
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

            try
            {
                  // Compute most of the parameters
               tempPR = cerange.ComputeAtTransmitTime( time,
                                                       observable,
                                                       rxPos,
                                                       (*stv).first,
                                                    *(getDefaultEphemeris()) );
            }
            catch(InvalidRequest& e)
            {

                  // If some problem appears, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*stv).first );

               continue;    // Skip this SV if problems arise

            }

               // Let's test if satellite has enough elevation over horizon
            if ( rxPos.elevationGeodetic(cerange.svPosVel) < minElev )
            {

                  // Mark this satellite if it doesn't have enough elevation
               satRejectedSet.insert( (*stv).first );

               continue;

            }

               // If given, computes tropospheric model
            if ( pDefaultTropoModel )
            {

               tempTrop = getTropoCorrections( pDefaultTropoModel,
                                               cerange.elevationGeodetic );

               (*stv).second[TypeID::tropoSlant] = tempTrop;

            }
            else
            {
               (*stv).second[TypeID::tropoSlant] = 0.0;
            }

               // If given, computes ionospheric model
            if( pDefaultIonoModel )
            {

                  // Convert Position rxPos to Geodetic rxGeo
               Geodetic rxGeo( rxPos.getGeodeticLatitude(),
                               rxPos.getLongitude(),
                               rxPos.getAltitude() );

               tempIono = getIonoCorrections( pDefaultIonoModel,
                                              time,
                                              rxGeo,
                                              cerange.elevationGeodetic,
                                              cerange.azimuthGeodetic );

               (*stv).second[TypeID::ionoSlant] = tempIono;

            }
            else
            {
               (*stv).second[TypeID::ionoSlant] = 0.0;
            }


            tempModeledPR = tempPR + tempTrop + tempIono;


               // Computing Total Group Delay (TGD - meters) and adding
               // it to result
            if( useTGD )
            {

               tempTGD = getTGDCorrections( time,
                                            (*pDefaultEphemeris),
                                            (*stv).first );

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
               // When using pseudorange method, this is 1.0
            (*stv).second[TypeID::cdt] = 1.0;

               // Now we have to add the new values to the data structure
            (*stv).second[TypeID::rho] = cerange.rawrange;
            (*stv).second[TypeID::rel] = -cerange.relativity;
            (*stv).second[TypeID::elevation] = cerange.elevationGeodetic;
            (*stv).second[TypeID::azimuth] = cerange.azimuthGeodetic;

               // Get the instrumental delays right
            if( useTGD )
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

               }

               (*stv).second[instDelayType] = tempTGD;


            } // End of if


         } // End of loop for (stv = gData.begin()...

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }   // End of try...
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::int2x( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ModelObsFixedStation::Process()'



      /* Method to set the initial (a priori) position of receiver.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int ModelObsFixedStation::setInitialRxPosition( const double& aRx,
                                                   const double& bRx,
                                                   const double& cRx,
                                                Position::CoordinateSystem s,
                                                   GeoidModel *geoid )
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

   }  // End of method 'ModelObsFixedStation::setInitialRxPosition()'



      // Method to set the initial (a priori) position of receiver.
   int ModelObsFixedStation::setInitialRxPosition(
                                             const Position& RxCoordinates )
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

   }  // End of method 'ModelObsFixedStation::setInitialRxPosition()'



      // Method to set the initial (a priori) position of receiver.
   int ModelObsFixedStation::setInitialRxPosition()
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

   }  // End of method 'ModelObsFixedStation::setInitialRxPosition()'



      // Method to get the tropospheric corrections.
   double ModelObsFixedStation::getTropoCorrections( TropModel *pTropModel,
                                                     double elevation )
   {

      double tropoCorr(0.0);

      try
      {
         tropoCorr = pTropModel->correction(elevation);

            // Check validity
         if( !(pTropModel->isValid()) )
         {
            tropoCorr = 0.0;
         }
      }
      catch(TropModel::InvalidTropModel& e)
      {
         tropoCorr = 0.0;
      }

      return tropoCorr;

   }  // End of method 'ModelObsFixedStation::getTropoCorrections()'


      // Method to get the ionospheric corrections.
   double ModelObsFixedStation::getIonoCorrections( IonoModelStore *pIonoModel,
                                                    DayTime Tr,
                                                    Geodetic rxGeo,
                                                    double elevation,
                                                    double azimuth )
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

   }  // End of method 'ModelObsFixedStation::getIonoCorrections()'



      // Method to get TGD corrections.
   double ModelObsFixedStation::getTGDCorrections( DayTime Tr,
                                                   const XvtStore<SatID>& Eph,
                                                   SatID sat )
   {

      try
      {
         const GPSEphemerisStore& bce =
                                 dynamic_cast<const GPSEphemerisStore&>(Eph);

         bce.findEphemeris(sat,Tr);

         return ( bce.findEphemeris(sat,Tr).getTgd() * C_GPS_M );
      }
      catch(...)
      {
         return 0.0;
      }

   }  // End of method 'ModelObsFixedStation::getTGDCorrections()'



}  // End of namespace gpstk
