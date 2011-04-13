#pragma ident "$Id$"

/**
 * @file ModeledPR.cpp
 * Class to compute modeled pseudoranges of a mobile receiver
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007, 2008
//
//============================================================================



#include "ModeledPR.hpp"


namespace gpstk
{


      /* Explicit constructor, taking as input initial receiver
       * coordinates, default ionospheric and tropospheric models,
       * ephemeris to be used, default observable and whether TGD will
       * be computed or not.
       *
       * This constructor is meant to be used when working with GNSS
       * data structures in order to set the basic parameters from the
       * beginning.
       *
       * @param RxCoordinates Initial receiver coordinates.
       * @param dIonoModel    Ionospheric model to be used by default.
       * @param dTropoModel   Tropospheric model to be used by default.
       * @param dEphemeris    XvtStore<SatID> object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledPR::ModeledPR( const Position& RxCoordinates,
                         IonoModelStore& dIonoModel,
                         TropModel& dTropoModel,
                         XvtStore<SatID>& dEphemeris,
                         const TypeID& dObservable,
                         bool usetgd )
   {

      init();
      Prepare(RxCoordinates);
      setDefaultIonoModel(dIonoModel);
      setDefaultTropoModel(dTropoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;

   }  // End of 'ModeledPR::ModeledPR()'



      /* Explicit constructor, taking as input initial receiver
       * coordinates, default ionospheric model, ephemeris to be used,
       * default observable and whether TGD will be computed or not.
       *
       * The default tropospheric model will be set to NULL.
       *
       * This constructor is meant to be used when working with GNSS
       * data structures in order to set the basic parameters from the
       * beginning.
       *
       * @param RxCoordinates Initial receiver coordinates.
       * @param dIonoModel    Ionospheric model to be used by default.
       * @param dEphemeris    XvtStore<SatID> object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledPR::ModeledPR( const Position& RxCoordinates,
                         IonoModelStore& dIonoModel,
                         XvtStore<SatID>& dEphemeris,
                         const TypeID& dObservable,
                         bool usetgd )
   {

      init();
      Prepare(RxCoordinates);
      setDefaultIonoModel(dIonoModel);
      pDefaultTropoModel = NULL;
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;

   }  // End of 'ModeledPR::ModeledPR()'



      /* Explicit constructor, taking as input initial receiver
       * coordinates, default tropospheric model, ephemeris to be used,
       * default observable and whether TGD will be computed or not.
       *
       * The default ionospheric model will be set to NULL.
       *
       * This constructor is meant to be used when working with GNSS
       * data structures in order to set the basic parameters from the
       * beginning.
       *
       * @param RxCoordinates Initial receiver coordinates.
       * @param dTropoModel   Tropospheric model to be used by default.
       * @param dEphemeris    XvtStore<SatID> object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledPR::ModeledPR( const Position& RxCoordinates,
                         TropModel& dTropoModel,
                         XvtStore<SatID>& dEphemeris,
                         const TypeID& dObservable,
                         bool usetgd )
   {

      init();
      Prepare(RxCoordinates);
      pDefaultIonoModel = NULL;
      setDefaultTropoModel(dTropoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;

   }  // End of 'ModeledPR::ModeledPR()'



      /* Explicit constructor, taking as input initial receiver
       * coordinates, ephemeris to be used, default observable and
       * whether TGD will be computed or not.
       *
       * Both the tropospheric and ionospheric models will be set to NULL.
       *
       * This constructor is meant to be used when working with GNSS
       * data structures in order to set the basic parameters from the
       * beginning.
       *
       * @param RxCoordinates Initial receiver coordinates.
       * @param dEphemeris    XvtStore<SatID> object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledPR::ModeledPR( const Position& RxCoordinates,
                         XvtStore<SatID>& dEphemeris,
                         const TypeID& dObservable,
                         bool usetgd )
   {

      init();
      Prepare(RxCoordinates);
      pDefaultIonoModel = NULL;
      pDefaultTropoModel = NULL;
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;

   }  // End of 'ModeledPR::ModeledPR()'



      /* Explicit constructor, taking as input default ionospheric
       * and tropospheric models, ephemeris to be used, default
       * observable and whether TGD will be computed or not.
       *
       * This constructor is meant to be used when working with GNSS
       * data structures in order to set the basic parameters from the
       * beginning.
       *
       * @param dIonoModel    Ionospheric model to be used by default.
       * @param dTropoModel   Tropospheric model to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param dEphemeris    XvtStore<SatID> object to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledPR::ModeledPR( IonoModelStore& dIonoModel,
                         TropModel& dTropoModel,
                         XvtStore<SatID>& dEphemeris,
                         const TypeID& dObservable,
                         bool usetgd )
   {

      init();
      setDefaultIonoModel(dIonoModel);
      setDefaultTropoModel(dTropoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;

   }  // End of 'ModeledPR::ModeledPR()'



      /* Explicit constructor, taking as input default ionospheric
       * model, ephemeris to be used, default observable and whether
       * TGD will be computed or not.
       *
       * This constructor is meant to be used when working with GNSS
       * data structures in order to set the basic parameters from the
       * beginning.
       *
       * @param dIonoModel    Ionospheric model to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param dEphemeris    XvtStore<SatID> object to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledPR::ModeledPR( IonoModelStore& dIonoModel,
                         XvtStore<SatID>& dEphemeris,
                         const TypeID& dObservable,
                         bool usetgd )
   {

      init();
      setDefaultIonoModel(dIonoModel);
      pDefaultTropoModel = NULL;
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;

   }  // End of 'ModeledPR::ModeledPR()'



      /* Explicit constructor, taking as input default tropospheric
       * model, ephemeris to be used, default observable and whether
       * TGD will be computed or not.
       *
       * This constructor is meant to be used when working with GNSS
       * data structures in order to set the basic parameters from the
       * beginning.
       *
       * @param dTropoModel   Tropospheric model to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param dEphemeris    XvtStore<SatID> object to be used by default.
       * @param usetgd        Whether TGD will be used by default or not.
       *
       * @sa DataStructures.hpp.
       */
   ModeledPR::ModeledPR( TropModel& dTropoModel,
                         XvtStore<SatID>& dEphemeris,
                         const TypeID& dObservable,
                         bool usetgd )
   {

      init();
      pDefaultIonoModel = NULL;
      setDefaultTropoModel(dTropoModel);
      setDefaultObservable(dObservable);
      setDefaultEphemeris(dEphemeris);
      useTGD = usetgd;

   }  // End of 'ModeledPR::ModeledPR()'



      /* Method to set an a priori position of receiver using
       * Bancroft's method.
       *
       * @param Tr            Time of observation
       * @param Satellite     std::vector of satellites in view
       * @param Pseudorange   std::vector of pseudoranges measured from
       *                      rover station to satellites
       * @param Eph           Satellites Ephemeris
       *
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int ModeledPR::Prepare( const DayTime& Tr,
                           std::vector<SatID>& Satellite,
                           std::vector<double>& Pseudorange,
                           const XvtStore<SatID>& Eph )
   {

      Matrix<double> SVP;
      Bancroft Ban;
      Vector<double> vPos;
      PRSolution raimObj;

      try
      {
         raimObj.PrepareAutonomousSolution( Tr,
                                            Satellite,
                                            Pseudorange,
                                            Eph,
                                            SVP );

         if( Ban.Compute(SVP, vPos) < 0 )
         {
            return -1;
         }
      }
      catch(...)
      {
         return -1;
      }

      return Prepare(vPos(0), vPos(1), vPos(2));

   }  // End of method 'ModeledPR::Prepare()'



      /* Method to set an a priori position of receiver using
       * Bancroft's method.
       *
       * @param Tr            Time of observation
       * @param Satellite     Vector of satellites in view
       * @param Pseudorange   Pseudoranges measured from rover station to
       *                      satellites
       * @param Eph           Satellites Ephemeris
       *
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int ModeledPR::Prepare( const DayTime& Tr,
                           const Vector<SatID>& Satellite,
                           const Vector<double>& Pseudorange,
                           const XvtStore<SatID>& Eph )
   {

      int i;
      std::vector<SatID> vSat;
      std::vector<double> vPR;

         // Convert from gpstk::Vector to std::vector
      for (i = 0; i < (int)Satellite.size(); i++)
      {
         vSat.push_back(Satellite[i]);
      }

      for (i = 0; i < (int)Pseudorange.size(); i++)
      {
         vPR.push_back(Pseudorange[i]);
      }

      return Prepare(Tr, vSat, vPR, Eph);

   }  // End of method 'ModeledPR::Prepare()'



      /* Method to set an a priori position of receiver using Bancroft's
       * method. Intended to be used with GNSS data structures.
       *
       * @param time      DayTime object for this epoch
       * @param data      satTypeValueMap data structure holding
       *                  the data.
       *
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int ModeledPR::Prepare( const DayTime& time,
                           const satTypeValueMap& data )
   {

      int i;
      std::vector<SatID> vSat;
      std::vector<double> vPR;
      Vector<SatID> Satellite( data.getVectorOfSatID() );
      Vector<double> Pseudorange(
                           data.getVectorOfTypeID( getDefaultObservable() ) );

         // Convert from gpstk::Vector to std::vector
      for (i = 0; i < (int)Satellite.size(); i++)
      {
         vSat.push_back(Satellite[i]);
      }

      for (i = 0; i < (int)Pseudorange.size(); i++)
      {
         vPR.push_back(Pseudorange[i]);
      }

      return Prepare(time, vSat, vPR, (*(getDefaultEphemeris())) );

   }  // End of method 'ModeledPR::Prepare()'



      /* Method to set the initial (a priori) position of receiver before
      * Compute() method.
      * @return
      *  0 if OK
      *  -1 if problems arose
      */
   int ModeledPR::Prepare( const double& aRx,
                           const double& bRx,
                           const double& cRx,
                           Position::CoordinateSystem s,
                           GeoidModel *geoid )
   {

      int result = setInitialRxPosition(aRx, bRx, cRx, s, geoid);

         // If everything is OK, the model is prepared
      if( result == 0 )
      {
         modelPrepared = true;
      }
      else
      {
         modelPrepared = false;
      }

      return result;

   }  // End of method 'ModeledPR::Prepare()'



      /* Method to set the initial (a priori) position of receiver before
       * Compute() method.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int ModeledPR::Prepare(const Position& RxCoordinates)
   {

      int result = setInitialRxPosition(RxCoordinates);

         // If everything is OK, the model is prepared
      if( result == 0 )
      {
         modelPrepared = true;
      }
      else
      {
         modelPrepared = false;
      }

      return result;

   }  // End of method 'ModeledPR::Prepare()'



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ModeledPR::processModel( const DayTime& time,
                                             satTypeValueMap& gData )
      throw(Exception)
   {

         // First, if the model is not prepared let's take care of it
      if( !getModelPrepared() )
      {
         Prepare(time, gData);
      }

      ModeledReferencePR::processModel(time, gData);

      return gData;

   }   // End of method 'ModeledPR::processModel()'


}  // End of namespace gpstk
