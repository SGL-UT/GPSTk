#pragma ident "$Id$"

/**
 * @file IonexModel.cpp
 * This is a class to compute the main values related to a given
 * GNSS IONEX model, i.e., the ionospheric correction and DCB values
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
//  Octavian Andrei - FGI ( http://www.fgi.fi ). 2008
//
//============================================================================


#include "IonexModel.hpp"
#include "icd_200_constants.hpp"          // C_GPS_M


namespace gpstk
{

      // Index initially assigned to this class.
   int IonexModel::classIndex = 5100000;


      // Returns an index identifying this object.
   int IonexModel::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string IonexModel::getClassName() const
   { return "IonexModel"; }



      // Explicit constructor, taking as input a Position object
      // containing reference station coordinates.
   IonexModel::IonexModel(const Position& RxCoordinates)
      throw(Exception)
   {

      pDefaultMaps = NULL;
      defaultObservable = TypeID::P1;
      useDCB = true;
      setInitialRxPosition(RxCoordinates);
      setIndex();

   }  // End of constructor 'IonexModel::IonexModel()'



      /* Explicit constructor, taking as input reference station
       * coordinates and ionex maps (Ionex Store object) to be used.
       *
       * @param RxCoordinates    Receiver coordinates.
       * @param istore           IonexStore object to be used by default.
       * @param dObservable      Observable type to be used by default.
       * @param applyDCB         Whether or not C1 observable will be
       *                         corrected from DCB effect.
       *
       */
   IonexModel::IonexModel( const Position& RxCoordinates,
                           IonexStore& istore,
                           const TypeID& dObservable,
                           const bool& applyDCB )
         throw(Exception)
      {

         setInitialRxPosition(RxCoordinates);
         setDefaultMaps(istore);
         defaultObservable = dObservable;
         useDCB = applyDCB;
         setIndex();

      }  // End of constructor 'IonexModel::IonexModel()'



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& IonexModel::Process( const DayTime& time,
                                         satTypeValueMap& gData )
      throw(Exception)
   {

         // Let's get TEC, RMS and ionosphere height for current receiver
         // position at current epoch
      Vector<double> val = pDefaultMaps->getIonexValue(time, rxPos);

         // make it handy for future use
      double tecval = val[0];
      double ionoHeight = val[2];

      SatIDSet satRejectedSet;

      try
      {

            // Loop through all the satellites
         satTypeValueMap::iterator stv;
         for(stv = gData.begin(); stv != gData.end(); ++stv)
         {

               // First check if ionex maps were set
            if(pDefaultMaps==NULL)
            {

                  // If ionex maps are missing, then remove all satellites
               satRejectedSet.insert( (*stv).first );

               continue;

            }

               // If satellite coordinates or elevation is missing, then
               // remove satellite
            if( (*stv).second.find(TypeID::satX)      == (*stv).second.end() ||
                (*stv).second.find(TypeID::satY)      == (*stv).second.end() ||
                (*stv).second.find(TypeID::satZ)      == (*stv).second.end() ||
                (*stv).second.find(TypeID::elevation) == (*stv).second.end() )
            {

               satRejectedSet.insert( (*stv).first );

               continue;

            }
            else
            {

                  // Satellite position object
               Position SV( (*stv).second(TypeID::satX),
                            (*stv).second(TypeID::satY),
                            (*stv).second(TypeID::satZ) );

                  // Scalars to hold satellite elevation, ionospheric map and 
                  // ionospheric slant delays
               double elevation( (*stv).second(TypeID::elevation) );
               double ionoMap(0.0);
               double ionexL1(0.0), ionexL2(0.0), ionexL5(0.0);   // GPS
               double ionexL6(0.0), ionexL7(0.0), ionexL8(0.0);   // Galileo

               try
               {

                  ionoMap = pDefaultMaps->iono_mapping_function( elevation,
                                                                 ionoHeight);

                     // Compute ionospheric slant correction
                  ionexL1 = pDefaultMaps->getIonoL1( elevation,
                                                     tecval,
                                                     ionoHeight);

                  ionexL2 = pDefaultMaps->getIonoL2( elevation,
                                                     tecval,
                                                     ionoHeight);

                  ionexL5 = pDefaultMaps->getIonoL5( elevation,
                                                     tecval,
                                                     ionoHeight);

                  ionexL6 = pDefaultMaps->getIonoL6( elevation,
                                                     tecval,
                                                     ionoHeight);

                  ionexL7 = pDefaultMaps->getIonoL7( elevation,
                                                     tecval,
                                                     ionoHeight);

                  ionexL8 = pDefaultMaps->getIonoL8( elevation,
                                                     tecval,
                                                     ionoHeight);

               }
               catch(InvalidRequest)
               {

                     // If some problem appears, then schedule this
                     // satellite for removal
                  satRejectedSet.insert( (*stv).first );

                  continue;    // Skip this SV if problems arise

               };

                  // Now we have to add the new values (i.e., ionosphere delays)
                  // to the data structure
               (*stv).second[TypeID::ionoTEC] = tecval;
               (*stv).second[TypeID::ionoMap] = ionoMap;
               (*stv).second[TypeID::ionoL1]  = ionexL1;
               (*stv).second[TypeID::ionoL2]  = ionexL2;
               (*stv).second[TypeID::ionoL5]  = ionexL5;
               (*stv).second[TypeID::ionoL6]  = ionexL6;
               (*stv).second[TypeID::ionoL7]  = ionexL7;
               (*stv).second[TypeID::ionoL8]  = ionexL8;
               
               

                  // DCB corrections for P1 measurements and satellite clock
                  // values should be considered because precise ephemerides
                  // and satellite clock information for SP3 orbit file always
                  // refers to the ionosphere-free linear combination (LC)
                  // see Appendix B, pg.14 of the Ionex manual
                  // Useful link:

      // http://www.ngs.noaa.gov/IGSWorkshop2008/docs/Schaer_DCB_IGSWS2008.ppt

                  // Computing Differential Code Biases (DCB - meters)
               double tempDCB( getDCBCorrections( time,
                                                 (*pDefaultMaps),
                                                 (*stv).first ) );


                  // Apply correction to P1 observable, if appropriate
               if(useDCB)
               {
                     // the second LC factor (see gpstk::LinearCombinations.cpp)
                  double kappa2(-1.0/0.646944444);

                     // Look for P1
                  if( (*stv).second.find(TypeID::P1) != (*stv).second.end() )
                  {
                     (*stv).second[TypeID::P1] -= (kappa2 * C_GPS_M * tempDCB);
                  }

                     // what about LC satellite clock values?. Must they
                     // be corrected?
                     // Below is Stefan Schaer's answer to my question
                     // Stefan Schaer: "Satellite-specific P1-P2 corrections 
                     // have to be applied only once (either with respect to 
                     // the pseudorange observations or the satellite 
                     // clock offsets)".

                     // Look for dtSat
             /*   if( (*stv).second.find(TypeID::dtSat) != (*stv).second.end())
                  {
                        // should be in meters due to 'TypeID::dSat'
                     double corr = kappa2* tempDCB * C_GPS_M;

                     (*stv).second[TypeID::dtSat] += corr;
                  }*/

                  (*stv).second[TypeID::instC1] = tempDCB * C_GPS_M;

               }  // End of 'if(useDCB)...'

            }  // End of 'if( (*stv).second.find(TypeID::satX) == ... else ...'

         }  // End of loop 'for(stv = gData.begin()...'


            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }   // End of try...
      catch(Exception& e)
      {

         GPSTK_RETHROW(e);

      }

   }  // End of method 'IonexModel::Process()'



      /* Method to set the initial (a priori) position of receiver.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int IonexModel::setInitialRxPosition(const Position& RxCoordinates)
      throw(GeometryException)
   {

      try
      {

         rxPos = RxCoordinates;

         return 0;

      }
      catch(GeometryException)
      {
         return -1;
      }

   }  // End of method 'IonexModel::setInitialRxPosition()'



      // Method to set the initial (a priori) position of receiver.
   int IonexModel::setInitialRxPosition(void)
      throw(GeometryException) 
   {

      try
      {

         Position rxpos(0.0, 0.0, 0.0, Position::Cartesian, NULL);

         setInitialRxPosition(rxpos);

         return 0;

      }
      catch(GeometryException)
      {
         return -1;
      }

   }  // End of method 'IonexModel::setInitialRxPosition()'



         /** Method to get DCB corrections.
          *
          * @param time       Epoch.
          * @param Maps       Store that contains the Ionex maps.
          * @param sat        SatID of satellite of interest
          *
          * @ return          Differential Code Bias (seconds)
          */
   double IonexModel::getDCBCorrections( const DayTime& time,
                                         const IonexStore& Maps,
                                         SatID sat )
      throw()
   {

      try
      {

         double dcb = Maps.findDCB(sat,time);   // nanoseconds

         return dcb*1e-9;

      }
      catch(...)
      {

         return 0.0;

      }

   }  // End of method 'IonexModel::getDCBCorrections()'



}  // End of namespace gpstk
