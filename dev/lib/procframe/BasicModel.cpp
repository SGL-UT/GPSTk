#pragma ident "$Id$"

/**
 * @file BasicModel.cpp
 * This is a class to compute the basic parts of a GNSS model, i.e.:
 * Geometric distance, relativity correction, satellite position and
 * velocity at transmission time, satellite elevation and azimuth, etc.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================


#include "BasicModel.hpp"


namespace gpstk
{


      // Index initially assigned to this class
   int BasicModel::classIndex = 4000000;


      // Returns an index identifying this object.
   int BasicModel::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string BasicModel::getClassName() const
   { return "BasicModel"; }



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
   BasicModel::BasicModel( const double& aRx,
                           const double& bRx,
                           const double& cRx,
                           Position::CoordinateSystem s,
                           GeoidModel *geoid )
   {

      minElev = 10.0;
      pDefaultEphemeris = NULL;
      defaultObservable = TypeID::C1;
      useTGD = false;
      setInitialRxPosition(aRx, bRx, cRx, s, geoid);
      setIndex();

   }  // End of 'BasicModel::BasicModel()'


      // Explicit constructor, taking as input a Position object
      // containing reference station coordinates.
   BasicModel::BasicModel(const Position& RxCoordinates)
   {

      minElev = 10.0;
      pDefaultEphemeris = NULL;
      defaultObservable = TypeID::C1;
      useTGD = false;
      setInitialRxPosition(RxCoordinates);
      setIndex();

   }  // End of 'BasicModel::BasicModel()'



      /* Explicit constructor, taking as input reference station
       * coordinates, ephemeris to be used, default observable
       * and whether TGD will be computed or not.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param applyTGD      Whether or not C1 observable will be
       *                      corrected from TGD effect or not.
       *
       */
   BasicModel::BasicModel( const Position& RxCoordinates,
                           XvtStore<SatID>& dEphemeris,
                           const TypeID& dObservable,
                           const bool& applyTGD )
   {

      minElev = 10.0;
      setInitialRxPosition(RxCoordinates);
      setDefaultEphemeris(dEphemeris);
      defaultObservable = dObservable;
      useTGD = applyTGD;
      setIndex();

   }  // End of 'BasicModel::BasicModel()'



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& BasicModel::Process( const DayTime& time,
                                         satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

         SatIDSet satRejectedSet;

            // Loop through all the satellites
         satTypeValueMap::iterator stv;
         for( stv = gData.begin();
              stv != gData.end();
              ++stv )
         {
               // Scalar to hold temporal value
            double observable( (*stv).second(defaultObservable) );

               // A lot of the work is done by a CorrectedEphemerisRange object
            CorrectedEphemerisRange cerange;

            try
            {
                  // Compute most of the parameters
               cerange.ComputeAtTransmitTime( time,
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

               // Computing Total Group Delay (TGD - meters), if possible
            double tempTGD(getTGDCorrections( time,
                                              (*pDefaultEphemeris),
                                              (*stv).first ) );

               // Now we have to add the new values to the data structure
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

               // Let's insert satellite position at transmission time
            (*stv).second[TypeID::satX] = cerange.svPosVel.x[0];
            (*stv).second[TypeID::satY] = cerange.svPosVel.x[1];
            (*stv).second[TypeID::satZ] = cerange.svPosVel.x[2];

               // Let's insert satellite velocity at transmission time
            (*stv).second[TypeID::satVX] = cerange.svPosVel.v[0];
            (*stv).second[TypeID::satVY] = cerange.svPosVel.v[1];
            (*stv).second[TypeID::satVZ] = cerange.svPosVel.v[2];

               // Apply correction to C1 observable, if appropriate
            if(useTGD)
            {
                  // Look for C1
               if( (*stv).second.find(TypeID::C1) != (*stv).second.end() )
               {
                  (*stv).second[TypeID::C1] =
                                       (*stv).second[TypeID::C1] - tempTGD;
               };
            };

            (*stv).second[TypeID::instC1] = tempTGD;

         } // End of loop for(stv = gData.begin()...

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }   // End of try...
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'BasicModel::Process()'



      /* Method to set the initial (a priori) position of receiver.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int BasicModel::setInitialRxPosition( const double& aRx,
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

   }  // End of method 'BasicModel::setInitialRxPosition()'



      // Method to set the initial (a priori) position of receiver.
   int BasicModel::setInitialRxPosition(const Position& RxCoordinates)
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

   }  // End of method 'BasicModel::setInitialRxPosition()'



      // Method to set the initial (a priori) position of receiver.
   int BasicModel::setInitialRxPosition()
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

   }  // End of method 'BasicModel::setInitialRxPosition()'



      // Method to get TGD corrections.
   double BasicModel::getTGDCorrections( DayTime Tr,
                                         const XvtStore<SatID>& Eph,
                                         SatID sat )
      throw()
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

   }  // End of method 'BasicModel::getTGDCorrections()'



}  // End of namespace gpstk
