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
//  Octavian Andrei - FGI ( http://www.fgi.fi ). 2008, 2009
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
      setIonoMapType("NONE");
      setInitialRxPosition(RxCoordinates);
      setIndex();

   }  // End of constructor 'IonexModel::IonexModel()'



      /** Explicit constructor, taking as input reference station
       * coordinates and ionex maps (Ionex Store object) to be used.
       *
       * @param RxCoordinates    Receiver coordinates.
       * @param istore           IonexStore object to be used by default.
       * @param dObservable      Observable type to be used by default.
       * @param applyDCB         Whether or not P1 observable will be
       *                         corrected from DCB effect.
       * @param ionoMapType      Type of ionosphere mapping function (string)
       *                         @sa IonexStore::iono_mapping_function
       */
   IonexModel::IonexModel( const Position& RxCoordinates,
                           IonexStore& istore,
                           const TypeID& dObservable,
                           const bool& applyDCB,
                           const std::string& ionoMap)
         throw(Exception)
      {

         setInitialRxPosition(RxCoordinates);
         setDefaultMaps(istore);
         defaultObservable = dObservable;
         useDCB = applyDCB;
         setIonoMapType(ionoMap);
         setIndex();

      }  // End of constructor 'IonexModel::IonexModel()'



      /** Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& IonexModel::Process( const DayTime& time,
                                         satTypeValueMap& gData )
      throw(Exception)
   {

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
               satRejectedSet.insert( stv->first );

               continue;

            }

               // If elevation or azimuth is missing, then remove satellite
            if( stv->second.find(TypeID::elevation) == stv->second.end() ||
                stv->second.find(TypeID::azimuth)   == stv->second.end() )
            {

               satRejectedSet.insert( stv->first );

               continue;

            }
            else
            {

                  // Scalars to hold satellite elevation, azimuth, ionospheric 
                  // map and ionospheric slant delays
               double elevation( stv->second(TypeID::elevation) );
               double azimuth(   stv->second(TypeID::azimuth)   );
               double ionoMap(0.0);
               double ionexL1(0.0), ionexL2(0.0), ionexL5(0.0);   // GPS
               double ionexL6(0.0), ionexL7(0.0), ionexL8(0.0);   // Galileo

                  //	calculate the position of the ionospheric pierce-point 
                  // corresponding to the receiver-satellite ray
               Position IPP = rxPos.getIonosphericPiercePoint( elevation,
                                                               azimuth,
                                                               ionoHeight);

                  // TODO
                  // Checking the collinearity of rxPos, IPP and SV


                  // Let's get TEC, RMS and ionosphere height for IPP 
                  // at current epoch
               Position pos(IPP);
               pos.transformTo(Position::Geocentric);
               Triple val = pDefaultMaps->getIonexValue( time, pos );

                  // just to make it handy for useage
               double tecval = val[0];

               try
               {

                  ionoMap = pDefaultMaps->iono_mapping_function( elevation,
                                                                 ionoMapType);

                     // Compute ionospheric slant correction
                  ionexL1 = pDefaultMaps->getIonoL1( elevation,
                                                     tecval,
                                                     ionoMapType);

                  ionexL2 = pDefaultMaps->getIonoL2( elevation,
                                                     tecval,
                                                     ionoMapType);

                  ionexL5 = pDefaultMaps->getIonoL5( elevation,
                                                     tecval,
                                                     ionoMapType);

                  ionexL6 = pDefaultMaps->getIonoL6( elevation,
                                                     tecval,
                                                     ionoMapType);

                  ionexL7 = pDefaultMaps->getIonoL7( elevation,
                                                     tecval,
                                                     ionoMapType);

                  ionexL8 = pDefaultMaps->getIonoL8( elevation,
                                                     tecval,
                                                     ionoMapType);

               }
               catch(InvalidRequest)
               {

                     // If some problem appears, then schedule this
                     // satellite for removal
                  satRejectedSet.insert( stv->first );

                  continue;    // Skip this SV if problems arise

               }

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

                  // Computing Differential Code Biases (DCB - nanoseconds)
               double tempDCB( getDCBCorrections( time,
                                                 (*pDefaultMaps),
                                                  stv->first) );


                  // add to the GDS the  corresponding correction, 
                  // if appropriate
               if(useDCB)
               {

                     // the second LC factor (see gpstk::LinearCombinations.cpp)
                     // see pg.14, Ionex manual
                  double kappa2(-1.0/0.646944444);
                  double dcb(tempDCB * C_GPS_M * 1e-9);  // meters

                  if( stv->second.find(TypeID::instC1) == stv->second.end() )
                  {
                     stv->second[TypeID::instC1] = (kappa2 * dcb);
                  }
                  else
                  {
                     stv->second[TypeID::instC1] += (kappa2 * dcb);
                  }

               }  // End of 'if(useDCB)...'

            }  // End of 'if( stv->second.find(TypeID::elevation) == ... '

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



      /** Method to set the default ionosphere mapping function type.
       *  If no valid type than NONE is set.
       *
       * @param ionoMapType   Type of ionosphere mapping function (string)
       *                      @sa IonexStore::iono_mapping_function
       *
       * @warning No implementation for JPL's mapping function.
       */
   IonexModel& IonexModel::setIonoMapType(const std::string& ionoMap)
   { 

         // here we set the type
      ionoMapType = ( ionoMap != "NONE" && ionoMap != "SLM" && 
                      ionoMap != "MSLM" && ionoMap != "ESM") ? "NONE" : 
                                                               ionoMap;

         // and here the ionosphere height, in meters
      ionoHeight = (ionoMap == "MSLM") ? 506700.0 : 450000.0;

      return (*this);

   }



         /** Method to get DCB corrections.
          *
          * @param time       Epoch.
          * @param Maps       Store that contains the Ionex maps.
          * @param sat        SatID of satellite of interest
          *
          * @ return          Differential Code Bias (nanoseconds)
          */
   double IonexModel::getDCBCorrections( const DayTime& time,
                                         const IonexStore& Maps,
                                         SatID sat )
      throw()
   {

      try
      {

         double dcb = Maps.findDCB(sat,time);   // nanoseconds

         return dcb;

      }
      catch(...)
      {

         return 0.0;

      }

   }  // End of method 'IonexModel::getDCBCorrections()'



}  // End of namespace gpstk
